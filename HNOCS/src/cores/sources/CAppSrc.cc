//
// Copyright (C) 2011 Eitan Zahavi, The Technion EE Department
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details."
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

// Behavior:
// NOTE: on the app side we have VCs. On the output we have NONE.
// Round robin between Apps and the VCs.

#include "CAppSrc.h"

Define_Module(CAppSrc)
;

void CAppSrc::initialize() {
	arbitration_type = par("arbitration_type");
	switch (arbitration_type) {
	case 0: // 0- winner takes all
		arbiterStarOnNextVCandApp = 0;
		break;
	case 1:// 1- round robin
		arbiterStarOnNextVCandApp = 1;
		break;
	default:
		throw cRuntimeError("-E- arbitration_type %d is unknown ",
				arbitration_type);
	}

	flitSize_B = par("flitSize");
	numVCs = par("numVCs");
	numApps = par("numApps");

	msgPerAppVC.resize(numApps);
	for (int i = 0; i < numApps; i++)
		msgPerAppVC[i].resize(numVCs, NULL);

	curMsg = NULL;
	pktIdx = 0;
	flitIdx = 0;
	statStartTime = par("statStartTime");

	srcId = par("srcId");
	popMsg = NULL;
	approvedPktId = -1;
	credits = 0;
	numOutReqs = 0;

	// provide credits to the apps so they can push messages
	for (int app = 0; app < numApps; app++) {
		for (int vc = 0; vc < numVCs; vc++) {
			NoCCreditMsg* creditMsg;
			char creditMsgName[32];
			sprintf(creditMsgName, "crd-app-%d", app);
			creditMsg = new NoCCreditMsg(creditMsgName);
			creditMsg->setKind(NOC_CREDIT_MSG);
			creditMsg->setVC(vc);
			creditMsg->setMsgs(1);
			creditMsg->setSchedulingPriority(0);
			send(creditMsg, "in$o", app);
		}
	}
	char popMsgName[32];
	sprintf(popMsgName, "pop-src-%d", srcId);
	popMsg = new NoCPopMsg(popMsgName);
	popMsg->setKind(NOC_POP_MSG);

	dstIdHist.setName("dst-hist");
}

// send the central request for the head flit of the Q
void CAppSrc::sendCentralReq() {
	if (approvedPktId >= 0) {
		throw cRuntimeError(
				"-E- can not send a request while previous one is still being sent");
	}
	if (numOutReqs) {
		throw cRuntimeError("-E- already have %d outstanding reqs", numOutReqs);
	}
	numOutReqs++;
	if (!curMsg) {
		throw cRuntimeError("-E- no curMsg");
	}
	char reqName[32];
	int pktId = (srcId << 16) + (curMsg->getMsgId() << 8) + pktIdx;
	sprintf(reqName, "creq-s:%d-d:%d-p:%x", srcId, curMsg->getDstId(), pktId);
	CentralRequestMsg* creq = new CentralRequestMsg(reqName);
	creq->setSrcId(srcId);
	creq->setDstId(curMsg->getDstId());
	creq->setFlits(curMsg->getPktLength());
	creq->setPktId(pktId);
	creq->setSchedulingPriority(0);
	creq->setAge(0);
	send(creq, "center$o");
}

// return true if found any msg to arbitrate or false if no message or no change
bool CAppSrc::arbitrate() {
	// can not arbitrate before the current packet is done


	// we can not arbitrate the VC in the middle of a message
	if (curMsg && (pktIdx < curMsg->getAppMsgLen())) // if still sending packets from previous msg
		return (true);

	// try next VC if not winner takes all
	for (int i = arbiterStarOnNextVCandApp; (i <= numVCs); i++) {
		int vc = (curVC + i) % numVCs;

		for (int j = arbiterStarOnNextVCandApp; j <= numApps; j++) {
			int app = (curApp + j) % numApps;

			// if there is an outstanding message
			if (msgPerAppVC[app][vc]) {
				curVC = vc;
				curApp = app;
				curMsg = msgPerAppVC[app][vc];
				msgPerAppVC[app][vc] = NULL;
				char creditMsgName[32];
				sprintf(creditMsgName, "crd-app-%d", app);
				NoCCreditMsg *creditMsg = new NoCCreditMsg(creditMsgName);
				creditMsg->setKind(NOC_CREDIT_MSG);
				creditMsg->setVC(vc);
				creditMsg->setMsgs(1);
				creditMsg->setSchedulingPriority(0);
				pktIdx = 0;
				flitIdx = 0;
				send(creditMsg, "in$o", app);
				EV<< "-I- " << getFullPath() << " arbitrate app: " << curApp <<" vc: " << curVC << endl;
				return(true);
			}
		}
	}

	EV << "-I- " << getFullPath() << " nothing to arbitrate" << endl;
	return(false);
}

// send the FLIT out and schedule the next pop
void CAppSrc::sendFlit() {
	if (!curMsg || (pktIdx == curMsg->getAppMsgLen()))
		return;

	if (popMsg->isScheduled())
		return;

	if (credits <= 0)
		throw cRuntimeError("-E- send with no credits");

	int dstId = curMsg->getDstId();
	int pktId = (srcId << 16) + (curMsg->getMsgId() << 8) + pktIdx;

	char flitName[128];
	sprintf(flitName, "flit-s:%d.%d-d:%d.%d-p:%d-f:%d", srcId, curMsg->getSrcAppId(),
			dstId, curMsg->getDstAppId(), pktId, flitIdx);
	AppFlitMsg *flit = new AppFlitMsg(flitName);
	flit->setKind(NOC_FLIT_MSG);
	flit->setByteLength(flitSize_B);
	flit->setBitLength(8 * flitSize_B);
	flit->setVC(curVC);
	flit->setSrcId(srcId);
	flit->setDstId(dstId);
	flit->setSchedulingPriority(0);
	flit->setFirstNet(true);
	flit->setPktId(pktId);
	flit->setFlitIdx(flitIdx);
	flit->setFlits(curMsg->getPktLength());
	flit->setFirstNet(false); // for debug only!!
	flit->setDstAppId(curMsg->getDstAppId());
	flit->setSrcAppId(curMsg->getSrcAppId());
	flit->setPktIdx(pktIdx);
	flit->setAppMsgLen(curMsg->getAppMsgLen());

	if (flitIdx == 0) {
		flit->setType(NOC_START_FLIT);
		dstIdHist.collect(dstId);
		flitIdx++;
	} else if (flitIdx == curMsg->getPktLength() - 1) {
		flitIdx = 0;
		pktIdx++;
		flit->setType(NOC_END_FLIT);

		// if the last FLIT of the message encapsulate the AppMsg after setting it size to zero
		if (curMsg->getAppMsgLen() == pktIdx) {
			curMsg->setOrigBytes(curMsg->getByteLength());
			curMsg->setBitLength(0);
			curMsg->setByteLength(0);
			flit->encapsulate(curMsg);
			curMsg = NULL;
		}

		// need to approve every packet
		approvedPktId = -1;

	} else {
		flitIdx++;
		flit->setType(NOC_MID_FLIT);
	}

	flit->setInjectTime(simTime());
	EV<< "-I- " << getFullPath() << " flit injected at time: " << flit->getInjectTime() << endl;
	send(flit, "out$o");
	credits--;

	// schedule next pop
	scheduleAt(gate("out$o")->getTransmissionChannel()->getTransmissionFinishTime(), popMsg);
}

void CAppSrc::handleCreditMsg(NoCCreditMsg *msg) {
	int vc = msg->getVC();
	int flits = msg->getFlits();
	delete msg;
	if (vc == 0) {
		credits += flits;
	} else {
		throw cRuntimeError("-E- non zero VC (%d) on Central ?", vc);
	}
}

void CAppSrc::handleAppMsg(AppMsg *msg) {
	int vc = msg->getVC();
	int app = msg->getArrivalGate()->getIndex();

	if (msgPerAppVC[app][vc]) {
		throw cRuntimeError("-E- %s Writing to full msgPerAppVC[%d][%d] ",
				getFullPath().c_str(), app, vc);
	}
	msgPerAppVC[app][vc] = msg;

	if (!popMsg->isScheduled())
		handlePopMsg();
}

void CAppSrc::handlePopMsg() {
	// if there is no active packet - try to activate one
	if (!numOutReqs && (approvedPktId < 0)) {
		if (arbitrate()) {
			sendCentralReq();
		}
	} else if (approvedPktId >= 0) {
		// send the next flit of the approved packet
		sendFlit();
	}
}

void CAppSrc::handleCentReqMsg(CentralRequestMsg *msg) {
	// an approval is received...
	if (msg->getSrcId() != srcId) {
		throw cRuntimeError(
				"-E- BUG - got replay with srcId:%d != local srcId:%d",
				msg->getSrcId(), srcId);
	}
	approvedPktId = msg->getPktId();
	numOutReqs--;
	EV << "-I- " << getFullPath() << " granted packet: " << approvedPktId << endl;
	sendFlit();
	delete msg;
}

void CAppSrc::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	if (msgType == NOC_POP_MSG) {
		handlePopMsg();
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg*) msg);
	} else if (msgType == NOC_APP_MSG) {
		handleAppMsg((AppMsg *)msg);
	} else {
		if (msg->getArrivalGate() == gate("center$i")) {
			handleCentReqMsg((CentralRequestMsg *) msg);
		} else {
			throw cRuntimeError("-E- BUG - did not expect msg of type:%d", msgType);
		}
	}
}

void CAppSrc::finish() {
	dstIdHist.record();
}

CAppSrc::~CAppSrc() {
	if (popMsg) {
		cancelAndDelete(popMsg);
	}

	for (int vc = 0; vc < numVCs; vc++) {
		for (int app = 0; app < numApps; app++) {
			if (msgPerAppVC[app][vc])
				cancelAndDelete(msgPerAppVC[app][vc]);
		}
	}
	if (curMsg)
		cancelAndDelete(curMsg);
}
