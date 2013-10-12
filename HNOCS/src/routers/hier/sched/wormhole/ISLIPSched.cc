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
//
// Behavior:
//
// The Arbiter count the number of outstanding requests on every [inPort][vc]
// It arbitrates on a clock timed by the clk message of type pop
//
// When a grant is sent to the inPort as a result of arbitration selecting this
// port/vc the HoQ Reqs counter is decreased. If a NAK is received the counter is
// increased again.
//
// Arbitration:
// Prefer to keep sending entire packet if possible
// If not cycle first through the VCs
// If no other VC has request it may switch port only if not in the middle of packet
//
#include "ISLIPSched.h"

Define_Module(ISLIPSched);

void ISLIPSched::initialize()
{
	numInPorts = gateSize("in");
	numVCs = par("numVCs");
	tClk_s = par("tClk");
	speculativeGntOnCompltedReq = par("speculativeGntOnCompltedReq");

	credits.resize(numVCs, 0);
	WATCH_VECTOR(credits);
	vcUsage.resize(numVCs, 0);
	WATCH_VECTOR(vcUsage);

	// arbitration state
	vcCurInPort.resize(numVCs,0);
	WATCH_VECTOR(vcCurInPort);

	winnerTakesAll = par("winnerTakesAll");

	curVC = numVCs - 1;
	isDisconnected = (gate("out$o",0)->getPathEndGate()->getType() != cGate::INPUT);

	HoQ.resize(numInPorts);
	for (int i = 0; i < numInPorts; i++) HoQ[i].resize(numVCs);
	numReqs = 0;

	vcCurReq.resize(numVCs,NULL);
	WATCH_VECTOR(vcCurReq);

	// start the clock
	clkMsg = new cMessage("clk");
	clkMsg->setKind(NOC_POP_MSG);
	scheduleAt(tClk_s, clkMsg);
	vc0Credits.setName("vc0Credits");
}

// The actual arbitration function - send the GNT to the selected ip/vc
// We implement no-iterating iSLIP. The InPort may NAK the GNT provided.
//
// The arbiter has to avoid mixing two packets on same oVC.
// * Changing inPort on same oVC is not allowed in the middle of a packet.
//   This is implemented by tracking the curReq[vc] which is set to NULL
//   once the EoP flit is passing.
// * If the curReq[vc]is not NULL no port change allowed
// * At the end of packet we can not switch to other inPort or even inVC of same inPort
//   before the flits of the packet are all sent (since they may be actually NaKed).
//   So the Req stay at the head of the HoQ[ip][oVC] until all its flits pass.
//
// The above seems to imply that no grants are allowed at end of packets.
// But if the next Req on the same HoQ[ip][oVC] has the same inVC it can be granted.
// So if there is no other port that has data to send we look inside the next req
// otherwise - stop for 2 cycles.
void ISLIPSched::arbitrate() {
	// loop to find something to do
	int nextInPort;
	int nextVC;
	bool found = false;

	// start with curVC
	int startAtNextVC = (winnerTakesAll ? 0 : 1);

	for (int i = startAtNextVC; !found && (i <= numVCs); i++) {
		int vc = (curVC + i) % numVCs;

		// are there credits on this VC?
		if (! credits[vc]) continue;

		// can not change port during a Req (if it is still the head of HoQ and has
		// some pending grants to make)
		int ip = vcCurInPort[vc];
		// if there is current Req and it is the same as the looked at HoQ[ip][vc] and
		// it is not completed - use it
		if (vcCurReq[vc] && HoQ[ip][vc].size() &&
			(HoQ[ip][vc].front() == vcCurReq[vc]) &&
			(vcCurReq[vc]->getNumGranted() != vcCurReq[vc]->getNumFlits())) {
			nextVC = vc;
			nextInPort = ip;
			found = true;
		} else {
			// go select the first Req (that is starting with current VC and next InPort
			// to curPort of the vc
			for (int j = 1; !found && (j <= numInPorts); j++) {
				int ip = (vcCurInPort[vc] + j) % numInPorts;
				// is there a pending req?
				if (HoQ[ip][vc].size()) {
					nextVC = vc;
					nextInPort = ip;
					found = true;
				}
			}
		}
	}
	if (!found) {
		EV << "-I- " << getFullPath() << " nothing to arbitrate" << endl;
		return;
	}

	NoCReqMsg *req = HoQ[nextInPort][nextVC].front();
	NoCReqMsg *prevReq = vcCurReq[nextVC];

	// if there is a non null CurReq for the nextVC we can not allow any other req
	if ( prevReq && (req != prevReq)) {
		EV << "-I- " << getFullPath() << " selected other port:" << nextInPort
		   << " while current Req is not fully completed. Ignore it." << endl;
		return;
	}

	curVC = nextVC;
	vcCurInPort[curVC] = nextInPort;

	// It is possible the Req was fully granted and if so we have nothing to do
	// unless there is a next Req on that HoQ and it shares the same inVC
	if ( req->getNumGranted() == req->getNumFlits()) {
		if (speculativeGntOnCompltedReq) {
			std::list<NoCReqMsg*>::iterator lI = HoQ[nextInPort][curVC].begin();
			lI++;
			int nextReqInVC = -1;
			if (lI != HoQ[nextInPort][curVC].end())
				nextReqInVC = (*lI)->getInVC();

			// need another entry with same inVC
			if ( nextReqInVC == req->getInVC()) {
				EV << "-I- " << getFullPath() << " sending next Req on port:" << nextInPort
				   << " on same inVC:" << req->getInVC() << endl;
				req = *lI;
			} else {
				EV << "-I- " << getFullPath() << " can not arbitrate next to Done Req on port:"
				   << nextInPort << " since prev inVC:" << req->getInVC() << " != next Req inVC:"
				   << nextReqInVC << endl;
				return;
			}
		} else {
			EV << "-I- " << getFullPath() << " Req waiting for last Acks on port:" << nextInPort
			   << " VC:" << curVC << endl;
			return;
		}
	}

    // update the current req pointer (we may had a more complex condition but this is OK)
    vcCurReq[curVC] = req;// HoQ[nextInPort][curVC].front();

    // if we have got here we can generate a Gnt on the head req.
	int prevGnted = req->getNumGranted();
	req->setNumGranted(prevGnted+1);

	const char *fType = "";
	if (prevGnted == 0) {
		fType = "SoP ";
	} else if (req->getNumGranted() == req->getNumFlits()) {
		fType = "EoP ";
	}
	EV << "-I- " << getFullPath() << " arbitrating " << fType << "VC:" << curVC
			<< " InPort:" << vcCurInPort[curVC] << " Req:" << req->getFullName()  << endl;

	// credit updates must happen here. Another option to put them on the flit receiver
	// would cause excessive grants that do not see the real state of the
	credits[curVC]--;
	vc0Credits.record(credits[curVC]);

	// send the Gnt
	char gntName[128];
	int inVC = req->getInVC();
	sprintf(gntName, "gnt-ivc:%d-ocv:%d-ip:%d", inVC, curVC, vcCurInPort[curVC]);
	NoCGntMsg *gnt = new NoCGntMsg(gntName);
	gnt->setKind(NOC_GNT_MSG);
	gnt->setOutVC(curVC);
	gnt->setInVC(inVC);
	send(gnt, "ctrl$o", vcCurInPort[curVC]);

	if (winnerTakesAll) {
		// after completing a Req start scanning from next VC
		if (req->getNumGranted() == req->getNumFlits())
			curVC = (curVC + 1) % numVCs;
	}
}

// a flit is received on the input so send it to the output
// also update Req waiting for last flit
void ISLIPSched::handleFlitMsg(NoCFlitMsg *msg ) {
	int vc = msg->getVC();
	int ip = msg->getArrivalGate()->getIndex();

	// the head of the HoQ MUST match
	NoCReqMsg *req = HoQ[ip][vc].front();

	// this info is only available on debug...
	if (req->getPktId() != msg->getPktId()) {
    	throw cRuntimeError("-E- Received PktId 0x%x that does not match the head Req PktId: 0x%x",
    			msg->getPktId(), req->getPktId());
	}

	// check if last FLIT of message
	if (msg->getType() == NOC_END_FLIT) {
		// the Req must have now zero pending grants and zero pending acks
		if (req->getNumGranted() != req->getNumFlits()) {
	    	throw cRuntimeError("-E- Received EoP PktId 0x%x but granted:%d != flits:%d",
	    			msg->getPktId(), req->getNumGranted(), req->getNumFlits());
		}
		if (req->getNumAcked() + 1 != req->getNumFlits()) {
	    	throw cRuntimeError("-E- Received EoP PktId 0x%x but acked:%d + 1 != flits:%d",
	    			msg->getPktId(), req->getNumAcked(), req->getNumFlits());
		}

		HoQ[ip][vc].pop_front();
		if (vcCurReq[vc] == req) vcCurReq[vc] = NULL;
		delete req;
		numReqs--;
	} else {
		// increase the number acked
		req->setNumAcked(req->getNumAcked()+1);
	}

	if (credits[vc] < 0) {
    	throw cRuntimeError("-E- %s Sending on VC %d has no credits packet:%d",
    			getFullPath().c_str(), vc, msg->getPktId());
	}

	send(msg, "out$o",0);

	simtime_t txFinishTime = gate("out$o", 0)->getTransmissionChannel()->getTransmissionFinishTime();
	if (txFinishTime <= simTime()) {
		throw cRuntimeError("-E- %s BUG - We just sent - must be busy!",
				getFullPath().c_str());
	}
}

// Place the Req on the HoQ
void ISLIPSched::handleReqMsg(NoCReqMsg *msg) {
	if (isDisconnected) {
    	throw cRuntimeError("-E- %s REQ on non Disconnected Port! Routing BUG",
    			getFullPath().c_str());
	}
	int vc = msg->getOutVC();
	int ip = msg->getArrivalGate()->getIndex();
	EV << "-I- " << getFullPath() << " Req on outVC:" << vc
	   << " InPort:" << ip << endl;

	numReqs++;
	HoQ[ip][vc].push_back(msg);
}

// ACK/NAK handling. Only NAK cause change in outstanding Reqs
void ISLIPSched::handleAckMsg(NoCAckMsg *msg) {
	int vc = msg->getOutVC();
	int ip = msg->getArrivalGate()->getIndex();
	if (msg->getOK()) {
		EV << "-I- " << getFullPath() << " ACK on VC:" << vc
				<< " InPort:" << ip << endl;
	} else {
		EV << "-I- " << getFullPath() << " NAK on VC:" << vc
				<< " InPort:" << ip << endl;
		// need to require one extra grant
		NoCReqMsg *req = HoQ[ip][vc].front();
		if (!req) {
	    	throw cRuntimeError("-E- No Req on InPort:%d VC %d during flit:%s",
	    			ip, vc, msg->getFullName());
		}
		req->setNumGranted(req->getNumGranted()-1);

		// since we have taken early credits need to recover
		credits[vc]++;
		vc0Credits.record(credits[vc]);
	}
	delete msg;
}

// Clock again only if there is anything to do
void ISLIPSched::handleClkMsg(cMessage *msg) {
	if (numReqs) {
		arbitrate();
		// Note - we must do it here to avoid a loop
		// other events may cause clk at cur simtime...
		scheduleAt(simTime() + tClk_s, clkMsg);
	}
}

void ISLIPSched::handleCreditMsg(NoCCreditMsg *msg) {
	int vc = msg->getVC();
	int num = msg->getFlits();
	credits[vc] += num;
	if (vc == 0) vc0Credits.record(credits[vc]);

	delete msg;
}

void ISLIPSched::handleMessage(cMessage *msg)
{
	int msgType = msg->getKind();
	if ( msgType == NOC_FLIT_MSG ) {
		handleFlitMsg( (NoCFlitMsg*)msg );
	} else if ( msgType == NOC_REQ_MSG ) {
		handleReqMsg( (NoCReqMsg*)msg );
	} else if ( msgType == NOC_ACK_MSG ) {
		handleAckMsg( (NoCAckMsg*)msg );
	} else if ( msgType == NOC_POP_MSG ) {
		handleClkMsg( msg );
	} else if ( msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg *)msg);
	} else {
		throw cRuntimeError("Does not know how to handle message of type %d", msg->getKind());
		delete msg;
	}

	// on any incoming message restart the clock...
	if (!clkMsg->isScheduled() && (numReqs > 0)) {
		double j = floor((simTime().dbl() - 1e-18) / tClk_s);
		double nextClk = (j+1)*tClk_s;
		if (nextClk < simTime().dbl()) nextClk += tClk_s;
		scheduleAt(nextClk, clkMsg);
	}

}

ISLIPSched::~ISLIPSched() {
	// cleanup owned Req
	if (clkMsg) cancelAndDelete(clkMsg);
	for (int ip = 0; ip < numInPorts; ip++) {
		for (int vc = 0; vc < numVCs; vc++) {
			while (HoQ[ip][vc].size()) {
				NoCReqMsg *req = HoQ[ip][vc].front();
				HoQ[ip][vc].pop_front();
				cancelAndDelete(req);
			}
		}
	}
}
