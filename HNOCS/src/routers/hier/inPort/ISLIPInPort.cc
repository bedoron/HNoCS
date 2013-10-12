//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ISLIPInPort.h"

// Behavior:
//
// NOTE: on each VC there is only 1 packet being received at a given time
// Also there is one packet being arbitrated on each out port
//
// On new packet arrival a sequence of sending (and then receiving same msg)
// to calcVc and then calcOp is performed.
//
// On FLIT receive the it is queued and a Req is sent.
//
// Each FLIT has to pass scheduling: REQ->GNT->ACK/NAK
//
// A REQ may be GNT by the Scheduler. It places that packet into the "in-flight".
// It also needs to send back an ACK to the Scheduler followed by the FLIT at the top of
// the QByOPiVCoVC
//
// If a NAK is provided (since the port is already too busy) the Scheduler will later
// send a GNT for the same VC.OP again.
//
// Whenever a FLIT/PKT is sent on out a credit is sent on the in$o.
//
// There is no delay modeling for the internal crossbar. It is assumed that if
// a grant is provided it happens at least FLIT time after previous one
//
Define_Module(ISLIPInPort)
;

void ISLIPInPort::initialize() {
	numVCs = par("numVCs");
	flitsPerVC = par("flitsPerVC");
	vcBufsUsed.resize(numVCs, 0);
	numParallelSends = par("numParallelSends");
	numBeingSent = 0;
	int numOutPorts = gateSize("out");
	QByOPiVCoVC.resize(numOutPorts);
	reqPending.resize(numOutPorts);
	curOutPort.resize(numVCs);
	curOutVC.resize(numVCs);
	curPktId.resize(numVCs, 0);

	// QByOPiVCoVC[outPort][inVC][outVC]
	for (int op = 0; op < numOutPorts; op++) {
		reqPending[op].resize(numVCs, 0);
		WATCH_VECTOR(reqPending[op]);
		QByOPiVCoVC[op].resize(numVCs);
		for (int vc = 0; vc < numVCs; vc++) {
			unsigned int outVCs = getOutPortNumVCs(op);
			QByOPiVCoVC[op][vc].resize(outVCs);
		}
	}

	// send the credits to the other size
	for (int vc = 0; vc < numVCs; vc++)
		sendCredit(vc, flitsPerVC);

}

// get the output port scheduler num VCs
unsigned int ISLIPInPort::getOutPortNumVCs(unsigned int outPortNum) {
	if (outPortNum >= (unsigned int)getParentModule()->gateSize("sw_in")) {
		opp_error("no sw_in[%d] port on parent", outPortNum);
	}
	cGate *oGate = getParentModule()->gate("sw_in", outPortNum);
	if (!oGate) {
		opp_error("could not get gate: sw_in[%d] port on parent", outPortNum);
	}
	cGate *remLowestGate = oGate->getPathEndGate();
	if (!remLowestGate) {
		opp_error("No remote port on gate: sw_in[%d] ", outPortNum);
	}
	cModule *mod = remLowestGate->getOwnerModule();
	Sched* sched = dynamic_cast<Sched*>(mod);
	return sched->par("numVCs");
}


// obtain the attached info
inPortFlitInfo* ISLIPInPort::getFlitInfo(NoCFlitMsg *msg) {
	cObject *obj = msg->getControlInfo();
	if (obj == NULL) {
		throw cRuntimeError("-E- %s BUG - No Control Info for FLIT: %s",
				getFullPath().c_str(), msg->getFullName());
	}

	inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
	return info;
}

// send back a credit on the in port
void ISLIPInPort::sendCredit(int vc, int numFlits) {
	if (gate("in$o")->getPathEndGate()->getType() != cGate::INPUT) {
		return;
	}
	EV<< "-I- " << getFullPath() << " sending " << numFlits
	<< " credits on VC=" << vc << endl;

	vcBufsUsed[vc]--;
	char credName[64];
	sprintf(credName, "cred-%d-%d", vc, numFlits);
	NoCCreditMsg *crd = new NoCCreditMsg(credName);
	crd->setKind(NOC_CREDIT_MSG);
	crd->setVC(vc);
	crd->setFlits(numFlits);
	send(crd, "in$o");
}

// create and send a Req to schedule the given FLIT
void ISLIPInPort::sendReq(NoCFlitMsg *msg) {
	inPortFlitInfo *info = getFlitInfo(msg);
	int outPort = info->outPort;
	int inVC = info->inVC;
	int outVC = msg->getVC();

	if (msg->getType() != NOC_START_FLIT) {
		throw cRuntimeError("SendReq for flit which isn`t SoP");
	}

	EV<< "-I- " << getFullPath() << " sending Req through outPort:" << outPort
	<< " on VC: " << outVC << endl;

	char reqName[64];
	sprintf(reqName, "req-s:%d-d:%d-p:%d-f:%d", (msg->getPktId() >> 16), msg->getDstId(),
			 (msg->getPktId() % (1<< 16)), msg->getFlitIdx());
  NoCReqMsg *req = new NoCReqMsg(reqName);
  req->setKind(NOC_REQ_MSG);
  req->setOutPortNum(outPort);
  req->setOutVC(outVC);
  req->setInVC(inVC);
  req->setPktId(msg->getPktId());
  req->setNumFlits(msg->getFlits());
  req->setNumGranted(0);
  req->setNumAcked(0);
  send(req, "ctrl$o", outPort);
}

	// when we get here it is assumed there is NO messages on the out port
void ISLIPInPort::sendFlit(NoCFlitMsg *msg) {
	int inVC = getFlitInfo(msg)->inVC;
	int outPort = getFlitInfo(msg)->outPort;

	EV<< "-I- " << getFullPath() << " sending Flit from inVC: " << inVC
	<< " through outPort:" << outPort << " on VC: " << msg->getVC() << endl;

	// delete the info object
	msg->removeControlInfo();

	// send to Sched and schedule self pop message
	send(msg, "out", outPort);

	// send the credit back on the inVC of that FLIT
	sendCredit(inVC,1);

	// track number of active ports
	numBeingSent++;

	// schedule the pop to reduce it back
	simtime_t txFinishTime = gate("out", outPort)->getTransmissionChannel()->getTransmissionFinishTime();
	if (txFinishTime <= simTime()) {
		throw cRuntimeError("-E- %s BUG - We just sent - must be busy!",
				getFullPath().c_str());
	}

	// schedule to wake us after the send
	NoCPopMsg *pop = new NoCPopMsg("pop");
	pop->setKind(NOC_POP_MSG);
	pop->setVC(msg->getVC());
	pop->setOutPortNum(outPort);
	pop->setSchedulingPriority(-1); // make sure it exec before the Gnts
	scheduleAt(txFinishTime, pop);
}

// Handle the Packet when it is back from the VC calc
// store at and send a request
void ISLIPInPort::handleCalcVCResp(NoCFlitMsg *msg) {
	// store the calc out VC in the current received packet on the inVC
	inPortFlitInfo *info = getFlitInfo(msg);
	int inVC = info->inVC;
	int outVC = msg->getVC();
	int outPort = info->outPort;

	curOutVC[inVC] = outVC;

	EV<< "-I- " << getFullPath() << " Packet:" << (msg->getPktId() >> 16)
	<< "." << (msg->getPktId() % (1<< 16))
	<< " will be sent on VC:" << outVC << endl;

	// buffering is by inVC
	if (vcBufsUsed[inVC] >= flitsPerVC) {
		throw cRuntimeError("-E- %s VC %d is already full receiving packet:%d",
				getFullPath().c_str(), inVC, msg->getPktId());
	}
	vcBufsUsed[inVC]++;

	// we queue the flits on their out Port and VC
	QByOPiVCoVC[outPort][inVC][outVC].insert(msg);

	// send the request for the entire packet
	sendReq(msg);
}

// Handle the packet when it is back from the Out Port calc
void ISLIPInPort::handleCalcOPResp(NoCFlitMsg *msg) {
	int inVC = getFlitInfo(msg)->inVC;
	curOutPort[inVC] = getFlitInfo(msg)->outPort;
	EV<< "-I- " << getFullPath() << " Packet:" << (msg->getPktId() >> 16)
	<< "." << (msg->getPktId() % (1<< 16))
	<< " will be sent to port:" << curOutPort[inVC] << endl;

	// send it to get the out port
	send(msg,"calcVc$o");
}

// handle received FLIT
void ISLIPInPort::handleInFlitMsg(NoCFlitMsg *msg) {
	// allocate control info
	inPortFlitInfo *info = new inPortFlitInfo;
	msg->setControlInfo(info);
	int inVC = msg->getVC();
	info->inVC = inVC;

	// record the first time the flit is transmitted by sched, in order to mask source-router latency effects
	if (msg->getFirstNet()) {
		msg->setFirstNetTime(simTime());
		msg->setFirstNet(false);
	}

	if (msg->getType() == NOC_START_FLIT) {

		// make sure current packet is 0
		if (curPktId[inVC]) {
			throw cRuntimeError("-E- got new packet %d during packet %d",
					curPktId[inVC], msg->getPktId());
		}
		curPktId[inVC] = msg->getPktId();

		// for first flit we need to calc outVC and outPort
		EV<< "-I- " << getFullPath() << " Received Packet:"
		<< (msg->getPktId() >> 16) << "." << (msg->getPktId() % (1<< 16))
		<< endl;
		// send it to get the out port calc
		send(msg, "calcOp$o");
	} else {
		// make sure the packet id is correct
		if (msg->getPktId() != curPktId[inVC]) {
			throw cRuntimeError("-E- got FLIT %d with packet 0x%x during packet 0x%x",
					msg->getFlitIdx(), msg->getPktId(), curPktId[inVC]);
		}
		if (msg->getType() == NOC_END_FLIT)
		curPktId[inVC] = 0;

		// since we do not allow interleaving pkts on same inVC we can use last head
		// of packet info (stored by inVC)
		int outPort = curOutPort[inVC];
		int outVC = curOutVC[inVC];
		info->outPort = outPort;

		msg->setVC(outVC);

		// queue
		EV << "-I- " << getFullPath() << " FLIT:" << (msg->getPktId() >> 16)
		<< "." << (msg->getPktId() % (1<< 16))
		<< "." << msg->getFlitIdx() << " Queued to be sent on OP:"
		<< outPort << " VC:" << outVC << endl;

		// buffering is by in VC
		if (vcBufsUsed[inVC] >= flitsPerVC) {
			throw cRuntimeError("-E- %s VC %d is already full receiving FLIT:%d.%d",
					getFullPath().c_str(), inVC, msg->getPktId());
		}
		vcBufsUsed[inVC]++;

		QByOPiVCoVC[outPort][inVC][outVC].insert(msg);

		// No more Req... Just head FLIT
		// sendReq(msg);
	}
}

		// A Gnt starts the sending on a FLIT on an output port
		// unless there are too many parallel sends.
void ISLIPInPort::handleGntMsg(NoCGntMsg *msg) {
	int outVC = msg->getOutVC();
	int inVC = msg->getInVC();
	int op = msg->getArrivalGate()->getIndex();

	EV<< "-I- " << getFullPath() << " Gnt of inVC: " << inVC << " outVC:" << outVC
	<< " through gate:" << msg->getArrivalGate()->getFullPath() << endl;

	// first we check for to many sends in parallel
	bool isNak = (numBeingSent >= numParallelSends);

	NoCFlitMsg* foundFlit = NULL;
	if (!isNak) {
		if (!QByOPiVCoVC[op][inVC][outVC].empty()) {
			foundFlit = (NoCFlitMsg*)QByOPiVCoVC[op][inVC][outVC].front();
			QByOPiVCoVC[op][inVC][outVC].pop();
		} else {
			EV << "-I- Could not find any flit with inVC:" << inVC << " outVC:"
			<< outVC << " and outPort:" << op << endl;
			isNak = true;
		}
	}

	char ackName[64];
	sprintf(ackName, "%s-op:%d-ivc:%d-ovc:%d", isNak ? "nak" : "ack", op, inVC, outVC);
	NoCAckMsg *ack = new NoCAckMsg(ackName);
	ack->setKind(NOC_ACK_MSG);
	ack->setOutPortNum(op);
	ack->setInVC(inVC);
	ack->setOutVC(outVC);
	if (isNak) {
		// NACK
		EV << "-I- " << getFullPath() << " NAK for Gnt on inVC: " << inVC << " outVC:" << outVC
		<< " outPort: " << op << endl;
		ack->setOK(false);
		send(ack, "ctrl$o", op);
	} else {
		EV << "-I- " << getFullPath() << " ACK for Gnt on inVC: " << inVC << " outVC:" << outVC
		<< " outPort: " << op << endl;

		ack->setOK(true);
		send(ack, "ctrl$o", op);
		sendFlit(foundFlit);
	}
	delete msg;
}

	// simply reduce the count of outstanding FLITs being sent
void ISLIPInPort::handlePopMsg(NoCPopMsg *msg) {
	int vc = msg->getVC();
	int op = msg->getOutPortNum();
	EV<< "-I- " << getFullPath() << " popped op=" << op << " vc=" << vc << endl;
	delete msg;
	numBeingSent--;
}

void ISLIPInPort::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	cGate *inGate = msg->getArrivalGate();
	if (msgType == NOC_FLIT_MSG) {
		if (inGate == gate("calcVc$i")) {
			handleCalcVCResp((NoCFlitMsg*) msg);
		} else if (inGate == gate("calcOp$i")) {
			handleCalcOPResp((NoCFlitMsg*) msg);
		} else {
			handleInFlitMsg((NoCFlitMsg*) msg);
		}
	} else if (msgType == NOC_GNT_MSG) {
		handleGntMsg((NoCGntMsg*) msg);
	} else if (msgType == NOC_POP_MSG) {
		handlePopMsg((NoCPopMsg*) msg);
	} else {
		throw cRuntimeError("Does not know how to handle message of type %d",
				msg->getKind());
		delete msg;
	}
}
