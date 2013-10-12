//
// Copyright (C) 2010-2011 Eitan Zahavi, The Technion EE Department
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
// Behavior:
// This source is a little different since it needs central scheduler
// approval to start sending messages.
//
// As such when an approval to send is obtained the flits of the approved packet MUST
// be on the FIFO and no issue with credits or other packets being sent is allowed.
// The entire packet is going to be sent
//
#include "CentSchedPktFifoSrc.h"

Define_Module(CentSchedPktFifoSrc)
;

void CentSchedPktFifoSrc::initialize() {
	credits = 0;
	pktIdx = 0;
	flitIdx = 0;
	flitSize_B = par("flitSize");
	maxQueuedPkts = par("maxQueuedPkts");
	numQueuedPkts = 0;
	WATCH(numQueuedPkts);
	WATCH(curPktLen);
	srcId = par("srcId");
	curPktLen = 1;
	curPktId = srcId << 16;
	char popMsgName[32];
	sprintf(popMsgName, "pop-s:%d", srcId);
	popMsg = new NoCPopMsg(popMsgName);
	popMsg->setKind(NOC_POP_MSG);
	popMsg->setSchedulingPriority(0);

	// a dstId parameter of -1 turns off the source...
	dstId = par("dstId");
	if (dstId < 0) {
		EV<< "-I- " << getFullPath() << " is turned OFF" << endl;
		genMsg = NULL;
	} else {
		char genMsgName[32];
		sprintf(genMsgName, "gen-%d", srcId);
		genMsg = new cMessage(genMsgName);
		genMsg->setSchedulingPriority(0);
		scheduleAt(simTime(), genMsg);
		dstIdHist.setName("dstId-Hist");
		dstIdHist.setRangeAutoUpper(0);
		dstIdHist.setCellSize(1.0);
		dstIdVec.setName("dstId");

		// handling messages
		curPktIdx = 0;
		curMsgLen = 0;
	}
	approvedPktId = -1;
	queueSize.setName("source-queue-size-percent");
}

		// send the central request for the head flit of the Q
void CentSchedPktFifoSrc::sendCentralReq() {
	if (Q.empty())
		return;
	if (approvedPktId >= 0) {
		throw cRuntimeError(
				"-E- can not send a request while previous one is still being sent");
	}
	NoCFlitMsg* flit = (NoCFlitMsg*) Q.front();
	char reqName[32];
	sprintf(reqName, "creq-s:%d-d:%d-p:%x", flit->getSrcId(), flit->getDstId(),
			flit->getPktId());
	CentralRequestMsg* creq = new CentralRequestMsg(reqName);
	creq->setSrcId(flit->getSrcId());
	creq->setDstId(flit->getDstId());
	creq->setFlits(flit->getFlits());
	creq->setPktId(flit->getPktId());
	creq->setSchedulingPriority(0);
	creq->setAge(0);
	send(creq, "center$o");
}

// Send the approved FLIT out and schedule the next pop - unless EoP
void CentSchedPktFifoSrc::sendFlitFromQ() {

	// if we try to send a flit it means we are in the middle of approved packet
	if (Q.empty()) {
		throw cRuntimeError(
				"-E- In central schedule can not sendFlit on empty FIFO!");
	}
	if (popMsg->isScheduled()) {
		throw cRuntimeError(
				"-E- In central schedule no way sendFlit during pop ");
	}
	if (credits <= 0) {
		throw cRuntimeError(
				"-E- In central schedule no way sendFlit with no credits ");
	}

	NoCFlitMsg* flit = (NoCFlitMsg*) Q.pop();
	if (flit->getPktId() != approvedPktId) {
		throw cRuntimeError(
				"-E- popped flit of msg 0x%x while approved message is 0x%x",
				flit->getPktId(), approvedPktId);
	}

	send(flit, "out$o");
	credits--;

	if (flit->getType() == NOC_END_FLIT) {
		approvedPktId = -1;
		if (numQueuedPkts <= 0) {
		  throw cRuntimeError("-E- popped flit of msg 0x%x cause underflow of numQueuedPkts: %d",
				      flit->getPktId());
		}
		numQueuedPkts--;
		// can not send the next request since it happens in zero time.
		// the next pop will handle it
	}

	// sched the next pop
	simtime_t txFinishTime =
	  gate("out$o")->getTransmissionChannel()->getTransmissionFinishTime();
	if (txFinishTime <= simTime()) {
		throw cRuntimeError("-E- %s BUG - We just sent - must be busy!",
				getFullPath().c_str());
	}
	scheduleAt(txFinishTime, popMsg);
}

// generate a new packet and Q all its flits
void CentSchedPktFifoSrc::handleGenMsg(cMessage *msg) {
	bool generatingOnEmptyQ = Q.empty();

	// if we already queued too many packets wait for a next gen ...
	if (numQueuedPkts < maxQueuedPkts) {
		numQueuedPkts++;
		// we change destination and packet length on MESSAGE boundary
		if (curPktIdx == curMsgLen) {
			curMsgLen = par("msgLen");
			if (curMsgLen <= 0) {
				throw cRuntimeError("-E- can not handle <= 0 packets message");
			}
			curPktIdx = 0;
			dstId = par("dstId");
			curPktLen = par("pktLen");
		}
		curPktVC = par("pktVC");
		dstIdHist.collect(dstId);
		dstIdVec.record(dstId);
		pktIdx++;
		curPktId = (srcId << 16) + pktIdx;
		curPktIdx++;

		for (flitIdx = 0; flitIdx < curPktLen; flitIdx++) {
			char flitName[128];
			sprintf(flitName, "flit-s:%d-t:%d-p:%d-f:%d", srcId, dstId, pktIdx,
					flitIdx);
			NoCFlitMsg *flit = new NoCFlitMsg(flitName);
			flit->setKind(NOC_FLIT_MSG);
			flit->setByteLength(flitSize_B);
			flit->setBitLength(8 * flitSize_B);
			flit->setVC(curPktVC);
			flit->setSrcId(srcId);
			flit->setDstId(dstId);
			flit->setPktId(curPktId);
			flit->setFlitIdx(flitIdx);
			flit->setSchedulingPriority(0);
			flit->setFlits(curPktLen);

			if (flitIdx == 0) {
				flit->setType(NOC_START_FLIT);
			} else if (flitIdx == curPktLen - 1) {
				flit->setType(NOC_END_FLIT);
			} else {
				flit->setType(NOC_MID_FLIT);
			}

			Q.insert(flit);
		}
	} // not a packet overflow

	queueSize.collect(1.0*numQueuedPkts / maxQueuedPkts);

	// if the gen was on empty Q then need to request
	if (generatingOnEmptyQ && !popMsg->isScheduled())
		sendCentralReq();

	// schedule next gen
	double flitArrivalDelay = par("flitArrivalDelay");
	scheduleAt(simTime() + curPktLen * flitArrivalDelay, genMsg);
}

void CentSchedPktFifoSrc::handleCreditMsg(NoCCreditMsg *msg) {
	int vc = msg->getVC();
	int flits = msg->getFlits();
	delete msg;
	if (vc == 0) {
		credits += flits;
	}
}

void CentSchedPktFifoSrc::handlePopMsg(cMessage *msg) {
	if (approvedPktId < 0)
		// throw cRuntimeError("-E- Not supposed to pop if no approved packet");
		sendCentralReq();
	else
		// send the next flit of the approved packet
		sendFlitFromQ();
}

void CentSchedPktFifoSrc::handleCentReqMsg(CentralRequestMsg *msg) {
	// an approval is received...
	if (msg->getSrcId() != srcId) {
		throw cRuntimeError(
				"-E- BUG - got replay with srcId:%d != local srcId:%d",
				msg->getSrcId(), srcId);
	}

	approvedPktId = msg->getPktId();
	sendFlitFromQ();
	delete msg;
}

void CentSchedPktFifoSrc::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	if (msgType == NOC_POP_MSG) {
		handlePopMsg((NoCPopMsg*) msg);
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg*) msg);
	} else {
		if (msg->getArrivalGate() == gate("center$i")) {
			handleCentReqMsg((CentralRequestMsg *) msg);
		} else {
			handleGenMsg(msg);
		}
	}
}

void CentSchedPktFifoSrc::finish() {
	if (dstId >= 0)
		dstIdHist.record();
	queueSize.record();
}

CentSchedPktFifoSrc::~CentSchedPktFifoSrc() {
	if (popMsg)
		cancelAndDelete(popMsg);

	if (genMsg)
		cancelAndDelete(genMsg);

	while (!Q.empty()) {
		NoCFlitMsg* flit = (NoCFlitMsg*) Q.pop();
		delete flit;
	}
}
