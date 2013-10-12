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
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
// Behavior:
// On obtaining a read request - Q it and if no pop msg generate one in latency.
// on receiving the pop message de-q the req and generate response message ...
// On getting write request - Q it and if no pop msg generate one in latency
//
#include "DRAM.h"
#include "ResponseDB.h"
#include <assert.h>

Define_Module(DRAM);

void DRAM::initialize()
{
	srcId = par("srcId");
	appId = par("appId");
	memLatency_s = par("memLatency");
    cachePagePackets = par("cachePagePackets"); // number of packets in cache page
    cachePacketFlits = par("cachePagePktFlits"); // number of flits in each cache packet

	char popMsgName[32];
	sprintf(popMsgName, "dram-%d.%d-pop", srcId, appId);
	popMsg = new NoCPopMsg(popMsgName);
	popMsg->setKind(NOC_POP_MSG);
	popMsg->setSchedulingPriority(0);

	credits = 0;

	// tell our friends we are ready to receive one message...
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

// on a pop return to the originator if read or just delete it and
// schedule next pop
void DRAM::handlePopMsg()
{
	if (reqQ.empty()) return;

	CMPMsg *req = (CMPMsg *)reqQ.pop();
	// NOTE: both read and write cause page read. We keep the op for the L2 to diff
	char msgName[32];
	sprintf(msgName, "dram-%d.%d-resp-%d", srcId, appId, req->getMsgId());
	CMPMsg *resp = new CMPMsg(msgName);
	resp->setKind(NOC_APP_MSG);
	resp->setSrcId(srcId);
	resp->setSrcAppId(appId);
	resp->setDstId(req->getSrcId());
	resp->setDstAppId(req->getSrcAppId());
	resp->setOperation(req->getOperation());
	resp->setOrigin(3);
	resp->setL2hit(req->getL2hit());
	resp->setAddress(req->getAddress());
	resp->setLength(req->getLength());
	resp->setAppMsgLen(cachePagePackets);
	resp->setPktLength(cachePacketFlits);
	resp->setRoundtrip(req->getRoundtrip()); // Propogate roundtrip packet value

	if(resp->getRoundtrip()) { // Mark stored packet for logging
		SessionMeta *respMeta = ResponseDB::getInstance()->find(req);
		assert(respMeta!=0);
		respMeta->addResponse(resp);
		respMeta->gen();	// Force generation time
	}

	if (credits) {
		// Resp IS the request
		send(resp, "out$o");
		credits--;
	} else {
		respQ.insert(resp);
	}
	cancelAndDelete(req);

	// if there are other in the Q schedule them
	if (!reqQ.empty())
		scheduleAt(simTime()+ memLatency_s, popMsg);
}

void DRAM::handleCmpMsg(CMPMsg *msg)
{
	// we always Q the incoming message after some checks
	if (msg->getOrigin() != 2) {
		throw cRuntimeError("-E- BUG - request from non level 2 origin:%d ",
						msg->getOrigin());
	}
	reqQ.insert(msg);
	if (!popMsg->isScheduled()) {
		scheduleAt(simTime()+memLatency_s, popMsg);
	}

	// send a credit to the NI to enable next recv
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

// if anything on the respQ send it
void DRAM::handleCreditMsg(NoCCreditMsg *msg)
{
	if (!respQ.empty()) {
		CMPMsg *resp = (CMPMsg *)respQ.pop();
		if(resp->getRoundtrip()) {
			SessionMeta *session = ResponseDB::getInstance()->find(resp);
			assert(session != 0);
			session->addResponse(resp);
			session->gen();
		}
		send(resp,"out$o");
	} else {
		credits++;
	}
	delete msg;
}

void DRAM::handleMessage(cMessage *msg)
{
	int msgType = msg->getKind();
	if (msgType == NOC_POP_MSG) {
		handlePopMsg();
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg *)msg);
	} else if (msgType == NOC_APP_MSG) {
		handleCmpMsg((CMPMsg *)msg);
	} else {
		throw cRuntimeError("-E- BUG unknown msg type:%d ",msgType);
	}
}

DRAM::~DRAM() {
	if (popMsg)
		cancelAndDelete(popMsg);

	while(!reqQ.empty()) {
		CMPMsg *req = (CMPMsg *)reqQ.pop();
		cancelAndDelete(req);
	}
}
