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
// On receiving a read request from L1 if L2-Hit send self message after hitLatency
// generate response when expire
// If read with L2-miss fwd it to the DRAM
// On receiving a write if L2-hit request - ignore it
// On receiving a write if L2-miss send the request to the DRAM
#include "L2.h"
#include "ResponseDB.h"
#include <assert.h>

Define_Module(L2);

void L2::initialize()
{
    appId = par("appId");    // must be unique in the Core
    srcId = par("srcId");    // local srcId
    dramId = par("dramId");   // we assume DRAM appId is 0
    l2HitLatency_s = par("l2HitLatency"); // time it takes to respond if read hit
    cacheLinePackets = par("cacheLinePackets");  // response num packets
    cacheLinePktFlits = par("cacheLinePktFlits"); // response packet size
    cachePagePackets = par("cachePagePackets"); // number of packets in cache page
    cachePacketFlits = par("cachePagePktFlits"); // number of flits in each cache packet

    credits = 0;

	char popMsgName[32];
	sprintf(popMsgName, "l2-%d.%d-pop", srcId, appId);
	popMsg = new cMessage(popMsgName);
	popMsg->setKind(NOC_POP_MSG);
	popMsg->setSchedulingPriority(0);

	// tell our friends we are ready to receive one message...
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

// if anything on the outQ send it
void L2::handleCreditMsg(NoCCreditMsg *msg)
{
	if (!outQ.empty()) {
		CMPMsg *out = (CMPMsg *)outQ.pop();
		send(out,"out$o");
	} else {
		credits++;
	}
	delete msg;
}

// a pop is only for hit-read move the head of the hitReadReqQ to the out
void L2::handlePopMsg()
{
	if (hitReadReqQ.empty()) return;
	CMPMsg *resp = (CMPMsg *)hitReadReqQ.pop();
	if (!credits) {
		outQ.insert(resp);
	} else {
		if(resp->getRoundtrip()) { // Fix generation time
			SessionMeta *session = ResponseDB::getInstance()->find(resp);
			assert(session!=0);
			session->gen();
		}
		send(resp,"out$o");
		credits--;
	}

	if (!hitReadReqQ.empty())
		scheduleAt(simTime() + l2HitLatency_s, popMsg);
}

// a Hit Read is going to generate a response in l2HitLatency_s
void L2::handleHitRead(CMPMsg *req)
{
	EV << "-I- " << getFullPath() << " Responding to Read with Hit from:" << req->getSrcId() << "."
	<< req->getSrcAppId() << " address:" << req->getAddress() << endl;
	// create the response packet and place it in the hitReadReqQ
	char msgName[32];
	sprintf(msgName, "l2-%d.%d-resp-%d", srcId, appId, req->getMsgId());
	CMPMsg *resp = new CMPMsg(msgName);
	resp->setKind(NOC_APP_MSG);
	resp->setSrcId(srcId);
	resp->setSrcAppId(appId);
	resp->setDstId(req->getSrcId());
	resp->setDstAppId(req->getSrcAppId());
	resp->setOperation(req->getOperation());
	resp->setOrigin(2);
	resp->setL2hit(req->getL2hit());
	resp->setAddress(req->getAddress());
	resp->setLength(req->getLength());
	resp->setAppMsgLen(cacheLinePackets);
	resp->setPktLength(cacheLinePktFlits);
	resp->setRoundtrip(req->getRoundtrip());

	if(req->getRoundtrip()) {
		SessionMeta *meta = ResponseDB::getInstance()->find(req);
		assert(meta!=0);
		meta->addResponse(resp);
	}

	hitReadReqQ.insert(resp);
	cancelAndDelete(req);

	if (!popMsg->isScheduled())
		scheduleAt(simTime() + l2HitLatency_s, popMsg);
}

// a Miss Read should generate a DRAM request immediately and Q the original
void L2::handleMissRead(CMPMsg *req)
{
	EV << "-I- " << getFullPath() << " Forwarding to DRAM Read with Miss from:" << req->getSrcId() << "."
	<< req->getSrcAppId() << " address:" << req->getAddress() << endl;
	// create the dram packet and place it in the outQ, store original in the dramReadReqQ
	char msgName[32];
	sprintf(msgName, "dram-%d.%d-resp-%d", srcId, appId, req->getMsgId());
	CMPMsg *pageReq = new CMPMsg(msgName);
	pageReq->setKind(NOC_APP_MSG);
	pageReq->setSrcId(srcId);
	pageReq->setSrcAppId(appId);
	pageReq->setDstId(dramId);
	pageReq->setDstAppId(0);
	pageReq->setOperation(CMP_OP_READ);
	pageReq->setOrigin(2);
	pageReq->setL2hit(req->getL2hit());
	pageReq->setAddress(req->getAddress());
	pageReq->setLength(req->getLength());
	pageReq->setAppMsgLen(cachePagePackets);
	pageReq->setPktLength(cachePacketFlits);
	pageReq->setRoundtrip(req->getRoundtrip());

	if(pageReq->getRoundtrip()) {			// Deal with Roundtrip packets
		// Associate new route with the triggering route - statistics ?
		SessionMeta *prevRequest = ResponseDB::getInstance()->find(req);
		assert(prevRequest != 0 );

		// Mark new route for storing
		SessionMeta *pageReqMeta = new SessionMeta(pageReq);
		pageReqMeta->setParent(prevRequest);
		ResponseDB::getInstance()->add(pageReqMeta);
	}

	dramReadReqQ.insert(req);
	// send the dram req or Q it
	if (credits) {
		send(pageReq, "out$o");
		credits--;
	} else {
		outQ.insert(pageReq);
	}
}

// a write with a hit means we can just drop the packet
void L2::handleHitWrite(CMPMsg *req)
{
	EV << "-I- " << getFullPath() << " Received Write with Hit from:" << req->getSrcId() << "."
	<< req->getSrcAppId() << " address:" << req->getAddress() << endl;
	cancelAndDelete(req);
}

// a write with a miss means we need to read the page from the dram
// NOTE: we mark it with WRITE
void L2::handleMissWrite(CMPMsg *req)
{
	EV << "-I- " << getFullPath() << " Forwarding to DRAM Write with Miss from:" << req->getSrcId() << "."
	<< req->getSrcAppId() << " address:" << req->getAddress() << endl;
	// create the dram packet and place it in the outQ, store original in the dramReadReqQ
	char msgName[32];
	sprintf(msgName, "dram-%d.%d-resp-%d", srcId, appId, req->getMsgId());
	CMPMsg *pageReq = new CMPMsg(msgName);
	pageReq->setKind(NOC_APP_MSG);
	pageReq->setSrcId(srcId);
	pageReq->setSrcAppId(appId);
	pageReq->setDstId(dramId);
	pageReq->setDstAppId(0);
	pageReq->setOperation(CMP_OP_WRITE);
	pageReq->setOrigin(2);
	pageReq->setL2hit(req->getL2hit());
	pageReq->setAddress(req->getAddress());
	pageReq->setLength(req->getLength());
	pageReq->setAppMsgLen(cachePagePackets);
	pageReq->setPktLength(cachePacketFlits);
	pageReq->setRoundtrip(true);    // Trace this transaction


	cancelAndDelete(req);

    assert(ResponseDB::getInstance()->exists(pageReq)==false);
    SessionMeta *meta = new SessionMeta(pageReq);
    ResponseDB::getInstance()->add(meta);

	// send the dram req or Q it
	if (credits) {
		send(pageReq, "out$o");
		credits--;
	} else {
		outQ.insert(pageReq);
	}
}

// DRAM may return a read page which should be forwarded to
// the originating L1. Write is simply ignored.
// NOTE: we rely on order of receiving the dram replies.
void L2::handleDramResp(CMPMsg *dramResp)	// TODO: FIX THIS FUNCTION
{
	// Always a part of Transaction
	SessionMeta *dramRespMeta = 0;
	if(dramResp->getRoundtrip()) {
		dramRespMeta = ResponseDB::getInstance()->find(dramResp->getId());
		assert(dramRespMeta != 0);
		dramRespMeta->end(); 	// Timestamp the response
	}

	int op = dramResp->getOperation();
	if (op == CMP_OP_WRITE) {
		EV << "-I- " << getFullPath() << " Receive DRAM Write page from:" << dramResp->getSrcId() << "."
		<< dramResp->getSrcAppId() << " address:" << dramResp->getAddress() << endl;
		assert(dramRespMeta!=0);
		// just consume it - has no side effects but the BW it consumes
		cancelAndDelete(dramResp);
	} else if (op == CMP_OP_READ) {
		// create a fwd to the L1 and push into outQ
		if (dramReadReqQ.empty()) {
			throw cRuntimeError("-E- got read page from DRAM but empty miss-read fifo?");
		}
		CMPMsg *req = (CMPMsg *)dramReadReqQ.pop();
		char msgName[32];
		sprintf(msgName, "l2-%d.%d-resp-%d", srcId, appId, req->getMsgId());
		CMPMsg *resp = new CMPMsg(msgName);
		resp->setKind(NOC_APP_MSG);
		resp->setSrcId(srcId);
		resp->setSrcAppId(appId);
		resp->setDstId(req->getSrcId());
		resp->setDstAppId(req->getSrcAppId());
		resp->setOperation(req->getOperation());
		resp->setOrigin(2);
		resp->setL2hit(req->getL2hit());
		resp->setAddress(req->getAddress());
		resp->setLength(req->getLength());
		resp->setAppMsgLen(cacheLinePackets);
		resp->setPktLength(cacheLinePktFlits);
		resp->setRoundtrip(req->getRoundtrip());

		EV << "-I- " << getFullPath() << " Received DRAM Read page from:" << dramResp->getSrcId() << "."
		<< dramResp->getSrcAppId() << " fwd to:" << req->getSrcId() << "." << req->getSrcAppId()
		<< " address:" << dramResp->getAddress() << endl;

		if(resp->getRoundtrip()) {
			SessionMeta *respMeta = dramRespMeta->getParent();
			SessionMeta *original = ResponseDB::getInstance()->find(req);
			assert(respMeta != 0);
			assert(original != 0);
			//assert(original == respMeta);
			//respMeta->addResponse(resp->getId());
			original->addResponse(resp->getId());
		}

		cancelAndDelete(dramResp);
		cancelAndDelete(req);

		// send the dram req or Q it
		if (credits) {
			send(resp, "out$o");
			credits--;
		} else {
			outQ.insert(resp);
		}
	} else {
		throw cRuntimeError("-E- BUG unknown op:%d ",op);
	}
}

// the real meat of the L2 classify incoming msgs and react
void L2::handleCmpMsg(CMPMsg *msg)
{
	// classify first by originator
	int origin = msg->getOrigin();
	if (origin == 1) { 						// Handle Core
		int op = msg->getOperation();
		if (op == CMP_OP_READ) {
			if (msg->getL2hit()) {
				handleHitRead(msg);
			} else {
				handleMissRead(msg);
			}
		} else if (op == CMP_OP_WRITE) {
			if (msg->getL2hit()) {
				handleHitWrite(msg);
			} else {
				handleMissWrite(msg);
			}
		} else {
			throw cRuntimeError("-E- BUG unknown op:%d ",op);
		}
	} else if (origin == 3) {				// Handle Dram
		handleDramResp(msg);
	} else {
		throw cRuntimeError("-E- BUG unknown originator:%d ",origin);
	}
	// send a credit to the NI to enable next recv
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

void L2::handleMessage(cMessage *msg)
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

L2::~L2()
{
	CMPMsg *msg;
	cancelAndDelete(popMsg);

	while (!outQ.empty()) {
		msg = (CMPMsg *)outQ.pop();
		cancelAndDelete(msg);
	}
	while (!dramReadReqQ.empty()) {
		msg = (CMPMsg *)dramReadReqQ.pop();
		cancelAndDelete(msg);
	}
	while (!hitReadReqQ.empty()) {
		msg = (CMPMsg *)hitReadReqQ.pop();
		cancelAndDelete(msg);
	}
}
