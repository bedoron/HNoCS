//
// Copyright (C) 2010-2011 Eitan Zahavi, The Technion EE Department
// Copyright (C) 2010-2011 Yaniv Ben-Itzhak, The Technion EE Department
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

#include "InPortSync.h"
#include "Predictor.h"
#include "ResponseDB.h"
#include "Utils.h"
// Behavior:
//
// NOTE: on each VC there is only 1 packet being received at a given time
// Also there is one packet being arbitrated on each out port
//
// On new packet arrival a sequence of sending (and then receiving same msg)
// to calcOp is performed.
//
// On empty Q[inVC] or pop of the EoP from Q[inVC] we need to call calcVC (on the SoP).
// This is done by removing the head of Q[inVC] sending it to calcVC and putting it back in
//
// Whenever a FLIT/PKT is sent on out a credit is sent on the in$o.
//
// There is no delay modeling for the internal crossbar. It is assumed that if
// a grant is provided it happens at least FLIT time after previous one
//
Define_Module(InPortSync);

#include <iostream>
using std::cout;
using std::stringstream;

void InPortSync::initialize() {
	numVCs = par("numVCs");
	flitsPerVC = par("flitsPerVC");
	collectPerHopWait = par("collectPerHopWait");
	int rows = par("rows");
	int columns = par("columns");
	statStartTime = par("statStartTime");

	QByiVC.resize(numVCs);
	curOutPort.resize(numVCs);
	curOutVC.resize(numVCs);
	curPktId.resize(numVCs, 0);
	curHeadId.resize(numVCs, 0);
	headResolution.resize(numVCs, PREDICTION_IGNORE);


	// send the credits to the other size
	for (int vc = 0; vc < numVCs; vc++)
		sendCredit(vc, flitsPerVC);

	QLenVec.setName("Inport_total_Queue_Length");

	if (collectPerHopWait) {
		qTimeBySrcDst_head_flit.resize(rows * columns);
		qTimeBySrcDst_body_flits.resize(rows * columns);
		for (int src = 0; src < rows * columns; src++) {
			qTimeBySrcDst_head_flit[src].resize(rows * columns);
			qTimeBySrcDst_body_flits[src].resize(rows * columns);
			for (int dst = 0; dst < rows * columns; dst++) {
				char str[64];
				char str1[64];
				sprintf(str, "%d_to_%d VC acquisition time", src, dst);
				sprintf(str1, "%d_to_%d transmission time", src, dst);
				qTimeBySrcDst_head_flit[src][dst].setName(str);
				qTimeBySrcDst_body_flits[src][dst].setName(str1);
			}
		}

	}

	// Get pointer to local predictor
	m_predictor = Predictor::GetMyPredictor(this);
}

// obtain the attached info
inPortFlitInfo* InPortSync::getFlitInfo(NoCFlitMsg *msg) {
	cObject *obj = msg->getControlInfo();
	if (obj == NULL) {
		throw cRuntimeError("-E- %s BUG - No Control Info for FLIT: %s",
				getFullPath().c_str(), msg->getFullName());
	}

	inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
	return info;
}

// send back a credit on the in port
void InPortSync::sendCredit(int vc, int numFlits) {
	if (gate("in$o")->getPathEndGate()->getType() != cGate::INPUT) {
		return;
	}
	EV<< "-I- " << getFullPath() << " sending " << numFlits
	<< " credits on VC=" << vc << endl;

	char credName[64];
	sprintf(credName, "cred-%d-%d", vc, numFlits);
	NoCCreditMsg *crd = new NoCCreditMsg(credName);
	crd->setKind(NOC_CREDIT_MSG);
	crd->setVC(vc);
	crd->setFlits(numFlits);
	crd->setSchedulingPriority(0);
	send(crd, "in$o");
}

	// create and send a Req to schedule the given FLIT, assume it is SoP
void InPortSync::sendReq(NoCFlitMsg *msg) {
	inPortFlitInfo *info = getFlitInfo(msg);
	int outPort = info->outPort;
	int inVC = info->inVC;
	int outVC = msg->getVC();

	if(outVC < 0) {
	    stringstream ss;
	    ss << "InPort on router " << getParentModule()->getParentModule()->getIndex();
	    ss << " on port " << getParentModule()->getIndex() << " has negative out VC !!!\n";
	    cerr << ss;
	    throw new cRuntimeError(ss.str().c_str());
	}


	if (msg->getType() != NOC_START_FLIT) {
		throw cRuntimeError("SendReq for flit which isn`t SoP");
	}

	EV<< "-I- " << getFullPath() << " sending Req through outPort:" << outPort
	<< " on VC: " << outVC << endl;

	char reqName[64];
	sprintf(reqName, "req-s:%d-d:%d-p:%d-f:%d", (msg->getPktId() >> 16), msg->getDstId(),
			(msg->getPktId() % (1<< 16)), msg->getFlitIdx());

	NoCReqMsg *req = new NoCReqMsg(reqName); // Create head flit ?
	req->setKind(NOC_REQ_MSG);
	req->setOutPortNum(outPort);
	req->setOutVC(outVC);
	req->setInVC(inVC);
	req->setPktId(msg->getPktId());
	req->setNumFlits(msg->getFlits());
	req->setNumGranted(0);
	req->setNumAcked(0);
	req->setSchedulingPriority(0);
	req->setPrediction(false);

	SessionMeta *meta = ResponseDB::getInstance()->find(msg->getId());
	if(PREDICTION_HIT == m_predictor->checkFlit(msg, meta)) {
	    req->setPrediction(true);
	}

	send(req, "ctrl$o", outPort);
}


void InPortSync::sendFlit(NoCFlitMsg *msg) {
	int inVC = getFlitInfo(msg)->inVC;
	int outPort = -1; //getFlitInfo(msg)->outPort;
//    int portToSendOn = -1;

    if(msg->getType()==NOC_START_FLIT) {
        inPortFlitInfo *info = (inPortFlitInfo*) msg->removeControlInfo();
        curOutPort[inVC] = info->outPort;
        curPktId[inVC] = msg->getPktId();
        curHeadId[inVC] = msg->getId();

        if(msg->getVC()<0) {
            stringstream ss;
            ss << "Flit arrived with negative VC\n";
            ss << "PktId: " << msg->getId() << "\n";
            ss << msg;
            throw new cRuntimeError(ss.str().c_str());
        }

        curOutVC[inVC] = msg->getVC();

        outPort = curOutPort[inVC];
    } else {
        msg->setVC(curOutVC[inVC]);

        // make sure the packet id is correct
        if (msg->getPktId() != curPktId[inVC]) {
            throw cRuntimeError("-E- got FLIT %d with packet 0x%x during packet 0x%x",
                    msg->getFlitIdx(), msg->getPktId(), curPktId[inVC]);
        }

        outPort = curOutPort[inVC];
        // on last FLIT need to zero out the current packet Id
        if (msg->getType() == NOC_END_FLIT) {
//            long int headID = curHeadId[inVC];
//            SessionMeta *meta = ResponseDB::getInstance()->find(headID);
            curPktId[inVC] = 0;
            curHeadId[inVC] = -1;
            curOutVC[inVC] = -1;
            curOutPort[inVC] = -1;
        }
    }

	if (gate("out", outPort)->getTransmissionChannel()->isBusy()) {
		EV << "-E-" << getFullPath() << " out port of InPort is busy! will be available in " << (gate("out", outPort)->getTransmissionChannel()->getTransmissionFinishTime()-simTime()) << endl;

//		cerr << msg;

//		cerr << "-E-" << getFullPath() << " out port of InPort is busy! will be available in " << (gate("out", outPort)->getTransmissionChannel()->getTransmissionFinishTime()-simTime()) << endl;
		throw cRuntimeError(
				"-E- Out port of InPort is busy!");
	}

	EV << "-I- " << getFullPath() << " sending Flit from inVC: " << inVC
	   << " through outPort:" << outPort << " on VC: " << msg->getVC() << endl;

	// delete the info object
	inPortFlitInfo *info = (inPortFlitInfo*) msg->removeControlInfo();
	delete info;

	// collect
	if (simTime()> statStartTime) {
		if (collectPerHopWait) {
			if (msg->getType() == NOC_START_FLIT) {
				qTimeBySrcDst_head_flit[msg->getSrcId()][msg->getDstId()].collect(1e9*(simTime().dbl() - msg->getArrivalTime().dbl()));
			} else {
				qTimeBySrcDst_body_flits[msg->getSrcId()][msg->getDstId()].collect(1e9*(simTime().dbl() - msg->getArrivalTime().dbl()));
			}
		}
	}
	// send to Sched
//	send(msg, "out", out);

	if(outPort<0 || outPort>4) {
	    stringstream ss;
	    ss << "Port is out of range: " << outPort << "\n";
	    cerr << ss;
	    throw new cRuntimeError(ss.str().c_str());
	}

	if((curOutVC[inVC] < 0) && (msg->getType() != NOC_END_FLIT)) {
	    cerr << "Sending a flit with VC < 0\n";
	    throw new cRuntimeError("VC Collapsed (<0) while sending a non end flit");
	}

	send(msg, "out", outPort);

	// send the credit back on the inVC of that FLIT
	sendCredit(inVC,1);
}

// Handle the Packet when it is back from the VC calc
// store the outVC in curOutVC[inVC] for next pops and Send the req
void InPortSync::handleCalcVCResp(NoCFlitMsg *msg) {
	// store the calc out VC in the current received packet on the inVC
	inPortFlitInfo *info = getFlitInfo(msg);
	int inVC = info->inVC;
	int outVC = msg->getVC();

    if(msg->getType() != NOC_START_FLIT) { // Incur delay
        if(QByiVC[inVC].isEmpty()) { // we really don't care
//            throw new cRuntimeError("Non head flit which traveled with delay arrived to an empty Q :-(");
//            cerr << "Non head flit which traveled with delay arrived to an empty Q :-(";
        }
        QByiVC[inVC].insert(msg);
        return;
    }

    if(outVC<0) {
        stringstream ss;
        ss << "flit arrived with negative VC: \n" << msg;
        cerr << ss;
        throw new cRuntimeError(ss.str().c_str());
    }

	// we queue the flits on their inVC
	if (QByiVC[inVC].isEmpty()) {
		QByiVC[inVC].insert(msg);
	} else {
		QByiVC[inVC].insertBefore(QByiVC[inVC].front(), msg);
	}

	// Total queue size
	measureQlength();

//	EV << "-I- " << getFullPath() << " Packet:" << (msg->getPktId() >> 16)
//	   << "." << (msg->getPktId() % (1<< 16))
//	   << " will be sent on VC:" << outVC << endl;


	/**
	 * TODO:
	 * Predict here ?
	 * this call will wait until the opposite Scheduler will send us a GNT.
	 * emit a prediction request event.
	 */
	sendReq(msg);
}

// Handle the packet when it is back from the Out Port calc
// Keep track of current out port per inVC
// if the Q is empty send to calc out VC or else Q it
void InPortSync::handleCalcOPResp(NoCFlitMsg *msg) {

    if(msg->getType() != NOC_START_FLIT) { // Incur delay
        send(msg,"calcVc$o");
        return;
    }


	int inVC = getFlitInfo(msg)->inVC;

	//curOutPort[inVC] = getFlitInfo(msg)->outPort;
	EV << "-I- " << getFullPath() << " Packet:" << (msg->getPktId() >> 16)
	   << "." << (msg->getPktId() % (1<< 16))
	   << " will be sent to port:" << curOutPort[inVC] << endl;

	// buffering is by inVC
	if (QByiVC[inVC].length() >= flitsPerVC) {
		throw cRuntimeError("-E- VC %d is already full receiving packet:%d",
				inVC, msg->getPktId());
	}

	SessionMeta *session = ResponseDB::getInstance()->find(msg);
	m_predictor->registerFlit(msg, session);
//	if(Resolution res = m_predictor->registerFlit(msg, session)) {
////	    cerr << "Resolution of register flit is " << PredictorApiIfc::ResolutionToString(res) << "\n";
//	}

//	cerr << "handleCalcOPResp\n";

	// send it to get the out VC
	if (QByiVC[inVC].empty()) {

	    Resolution res = m_predictor->checkFlit(msg, session);

//	    cerr << "Packet with resolution " << PredictorApiIfc::ResolutionToString(res) << " Entered handle Calc OPResp\n";

	    if(PREDICTION_HIT==res) {
            m_predictor->getVcCalc().PredictorSetOutVC(msg);
            take(msg);
            handleCalcVCResp(msg);
	    } else {
	        send(msg,"calcVc$o");
	    }

	} else {
//	    cerr << "Adding packet to Q\n";

		// we queue the flits on their inVC
		QByiVC[inVC].insert(msg);
		// Total queue size
		measureQlength();
	}
}

// handle received FLIT
void InPortSync::handleInFlitMsg(NoCFlitMsg *msg) {
	// allocate control info
	inPortFlitInfo *info = new inPortFlitInfo;
	msg->setControlInfo(info);
	int inVC = msg->getVC();
	info->inVC = inVC;

	EV << "InPortSync::handleInFlitMsg msg id is " << msg->getId() << " pkt id is " << msg->getPktId() << " ** \n";

	// record the first time the flit is transmitted by sched, in order to mask source-router latency effects
	if (msg->getFirstNet()) {
		msg->setFirstNetTime(simTime());
		msg->setFirstNet(false);
	}

	if (msg->getType() == NOC_START_FLIT) {
		// make sure current packet is 0
//		if (curPktId[inVC]) {
//			throw cRuntimeError("-E- got new packet 0x%x during packet 0x%x",
//					curPktId[inVC], msg->getPktId());
//		}
//		curPktId[inVC] = msg->getPktId();
//		curHeadId[inVC] = msg->getId();

		// for first flit we need to calc outVC and outPort
		EV << "-I- " << getFullPath() << " Received Packet:"
		   << (msg->getPktId() >> 16) << "." << (msg->getPktId() % (1<< 16))
		   << endl;
		//EV << "CHECK THE ABOVE TO SEE IF WE HAVE THE ORIGINAL ID SOMEWHERE HERE\n";

		// send it to get the out port calc
        SessionMeta *session = ResponseDB::getInstance()->find(msg);
        headResolution[inVC] = m_predictor->checkFlit(msg, session);
        if(PREDICTION_HIT == headResolution[inVC]) {
            m_predictor->getOpCalc().PredictorSetOutPort(msg);
            take(msg);
            handleCalcOPResp(msg);
        } else {
            if(session && session->getState()==SESSION_META_RESPONSE) {
//                cerr << "************Packet is sent to regular CALCOP\n";
//                cerr << msg;
            }
            send(msg, "calcOp$o");
        }
	} else {

		// since we do not allow interleaving of packets on same inVC we can use last head
		// of packet info (stored by inVC)
		//int outPort = curOutPort[inVC];
		//info->outPort = outPort;

		//cerr << "\t**** Regular OutPort: " << outPort << "\n";

		// queue
		EV << "-I- " << getFullPath() << " FLIT:" << (msg->getPktId() >> 16)
		   << "." << (msg->getPktId() % (1<< 16))
	       << "." << msg->getFlitIdx() << " Queued to be sent on OP:" << endl;
//		   << outPort << endl;

		// buffering is by inVC
		if (QByiVC[inVC].length() >= flitsPerVC) {
			throw cRuntimeError("-E- VC %d is already full receiving packet:%d",
					inVC, msg->getPktId());
		}

		// we always queue continue flits on their out Port and VC -
		// May cause BW issue if the arrival is a little "slower" then Sched GNT
		// since it realy depends on the order of events in same tick!

		if(PREDICTION_HIT == headResolution[inVC]) {
		    QByiVC[inVC].insert(msg);
		} else { // Incur delay on packet to overcome synchronization bug
		    send(msg, "calcOp$o");
		}

		// Total queue size
		measureQlength();
	}
}

		// A Gnt starts the sending on a FLIT on an output port
void InPortSync::handleGntMsg(NoCGntMsg *msg) {
	int outVC = msg->getOutVC();
	int inVC = msg->getInVC();
	int op = msg->getArrivalGate()->getIndex();

	EV << "-I- " << getFullPath() << " Gnt of inVC: " << inVC << " outVC:" << outVC
	   << " through gate:" << msg->getArrivalGate()->getFullPath() <<" SimTime:" <<simTime()<< endl;

	NoCFlitMsg* foundFlit = NULL;
	if (!QByiVC[inVC].empty()) {
		foundFlit = (NoCFlitMsg*)QByiVC[inVC].pop();
		//foundFlit->setVC(curOutVC[inVC]);

		// Total queue size
		measureQlength();

		// If NOC_END_FLIT, then check if there is another packet, if yes send to calcVC
		if (foundFlit->getType() == NOC_END_FLIT && !QByiVC[inVC].empty()) {
			NoCFlitMsg* nextPkt = (NoCFlitMsg*)QByiVC[inVC].pop();
			// need to get oVC and the response will send the req
			send(nextPkt,"calcVc$o");
		}

		sendFlit(foundFlit);

	} else {
		EV << "-I- Could not find any flit with inVC:" << inVC << endl;
		// send an NAK
		char nakName[64];
		sprintf(nakName, "nak-op:%d-ivc:%d-ovc:%d", op, inVC, outVC);
		NoCAckMsg *ack = new NoCAckMsg(nakName);
		ack->setKind(NOC_ACK_MSG);
		ack->setOutPortNum(op);
		ack->setInVC(inVC);
		ack->setOutVC(outVC);
		ack->setOK(false);
		send(ack, "ctrl$o", op);
	}
	delete msg;
}

void InPortSync::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	cGate *inGate = msg->getArrivalGate();

	NoCFlitMsg *flit = (NoCFlitMsg*)msg;
	if(flit->getPktId()==262144) {
      cerr << "Caught flit " << 262144 << " in router " << getParentModule()->getParentModule()->getIndex() << " port " << getParentModule()->getIndex() << "\n";
      cerr << flit;
      cerr << "----------------------------\n";
	}

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
	} else {
		throw cRuntimeError("Does not know how to handle message of type %d",
				msg->getKind());
		delete msg;
	}
}

InPortSync::~InPortSync() {
	// clean up messages at QByiVC
	numVCs = par("numVCs");
	NoCFlitMsg* msg = NULL;
	for (int vc = 0; vc < numVCs; vc++) {
		while (!QByiVC[vc].empty()) {
			msg = (NoCFlitMsg*) QByiVC[vc].pop();
			delete (msg); //cancelAndDelete?!
		}
	}
}

void InPortSync::measureQlength() {
	// measure Total queue length
	if (simTime() > statStartTime) {
		int numVCs = par("numVCs");
		int Qsize = 0;
		for (int vc = 0; vc < numVCs; vc++) {
			Qsize = Qsize + QByiVC[vc].length();
		}
		QLenVec.record(Qsize);
	}
}

void InPortSync::finish() {
	if (simTime() > statStartTime) {
		int Dst;
		int Src;
		int rows = par("rows");
		int columns = par("columns");
		if (collectPerHopWait) {
			for (Dst = 0; Dst < (rows * columns); Dst++) {
				for (Src = 0; Src < (rows * columns); Src++) {
					qTimeBySrcDst_head_flit[Src][Dst].record();
					qTimeBySrcDst_body_flits[Src][Dst].record();
				}
			}
		}
	}
}

