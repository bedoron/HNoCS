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
// On every move from outstanding-mem-read to none read a new line
// wait the given number of cycles and then send the mem-read and mark outstanding
// when response it obtained clear the outstanding and so on. Until trace is eof
//
// Changes: 14/11/2012:
// Ben Elazar Doron b.e.doron@gmail.com, Yevgeny Dibrov yevgenydibrov@gmail.com
// Added packet tagging
// Changes: 01/12/2012:
// Added Request Response Statistics Control


#include "CoreL1.h"
#include "ResponseDB.h"
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

Define_Module(CoreL1);

void CoreL1::initialize() {
	// parameters
	appId = par("appId"); // must be unique in the Core
	srcId = par("srcId"); // local srcId
	traceFileName = par("traceFileName"); // the name of the trace file to parse
	l2IdsStr = par("l2Ids"); // a list of L2 module IDs
	l2AppId = par("dstAppId"); // L2 destination app
	tClk_s = par("tClk");
	int a = par("cachePagePackets");
	int b = par("cachePagePktFlits");
	int c = par("flitSize");
	cachePageSize = a * b * c;
	credits = 0;
	cacheLinePackets = par("cacheLinePackets");
	cacheLinePktFlits = par("cacheLinePktFlits");

	// parse the list of L2 ids
	std::istringstream iss(l2IdsStr);
	while (!iss.eof()) {
		unsigned int sub;
		iss >> sub;
		l2Ids.push_back(sub);
	}

	if (strlen(traceFileName)) {
		outstandingMemAccess = false;
		trace.open(traceFileName);
		if (!trace.is_open()) {
			throw cRuntimeError("-E- could not open file: %s ", traceFileName);
		}
		lineNum = 0;
		char popMsgName[32];
		sprintf(popMsgName, "l2-%d.%d-pop", srcId, appId);
		popMsg = new cMessage(popMsgName);
		popMsg->setKind(NOC_POP_MSG);
		popMsg->setSchedulingPriority(0);
		if (readNextMemAccess())
			scheduleAt(simTime(), popMsg);
	} else {
		EV<< "-I- " << getFullPath() << " is off" << endl;
		popMsg = NULL;
	}

	finished=false;
	l2ReadTimeHist.setName("l2-read-trans-hist");
	l2ReadTimeHist.setRange(0.0, 5.0e-5);

	requestResponseHist.setName("reuquest-response-delta-hist");
	requestResponseHist.setRange(0.0, 5.0e-5);

	// tell our friends we are ready to receive one message...
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

// parse one line of the trace and set all values
bool CoreL1::readNextMemAccess() {
	lineNum++;
	int cycles, isW;
	long int curAddr;
	char buff[1024];
	if (!trace.getline(buff,1023)) {
		EV << "-I- " << getFullPath() << "Trace complete at:" << simTime().dbl() << endl;
		recordScalar("complete-time-ns", simTime().dbl()* 1e9);
		finished=true;
		return (false);
	}

	char *c, *i, *h, *a, *s;
	c = strtok(buff, " ");
	if (c == NULL) {
		throw cRuntimeError("-E- could not get cycles from: %s ", buff);
	}
	i = strtok(NULL," ");
	if (i == NULL) {
		throw cRuntimeError("-E- could not get isW from: %s ", buff);
	}
	h = strtok(NULL," ");
	if (h == NULL) {
		throw cRuntimeError("-E- could not get hit from: %s ", buff);
	}
	a = strtok(NULL," ");
	if (a == NULL) {
		throw cRuntimeError("-E- could not get address from: %s ", buff);
	}
	s = strtok(NULL," ");
	if (s == NULL) {
		throw cRuntimeError("-E- could not get size from: %s ", buff);
	}

	cycles = atoi(c);
	isW = atoi(i);
	curL2Hit = atoi(h);
	curAddr = strtoul(a, NULL, 0);
	curSize_b = atoi(s);

	// calculate delay
	curDelayToNextMem_s = tClk_s * cycles;
	// sanity
	// Workaround traces with negative cycles -> assign  curDelayToNextMem_s=0;
	if (curDelayToNextMem_s < 0){
	    //throw cRuntimeError("-E- got delay %g < 0 (cycles=%d)", curDelayToNextMem_s,cycles);
	    EV << getFullPath() << "got delay " << curDelayToNextMem_s << "0 (cycles=" << cycles << ") => reset curDelayToNextMem_s to ZERO" << endl;
	    curDelayToNextMem_s=0;
	}

	

	// calculate the destination L2
	unsigned int l2Idx = (curAddr/cachePageSize) % l2Ids.size();
	curL2Id = l2Ids[l2Idx];

	// get the op
	curOp = isW ? CMP_OP_WRITE : CMP_OP_READ;

	EV << "-I- " << getFullPath() << " obtained next mem access dt: " << curDelayToNextMem_s
			<< " isW: " << isW << " isHit: " << curL2Hit << " addr: " << curAddr
			<< " l2Id: " << curL2Id
			<< " size: " << curSize_b << endl;
	return(true);
}


// if anything on the outQ send it
void CoreL1::handleCreditMsg(NoCCreditMsg *msg) {
	if (!outQ.empty()) {
		CMPMsg *out = (CMPMsg *) outQ.pop();
		// 14/11/2012: check if this message should be logged

		if(out->getRoundtrip()) {	// Tag this outgoing request
			assert(ResponseDB::getInstance()->exists(out)==true);
			SessionMeta *session = ResponseDB::getInstance()->find(out);
			session->start(); // update time stamp
		}

		EV << "-I- " << getFullPath() << " sending msg: " << out->getMsgId()
			<< " to: " << out->getDstId() << endl;
		send(out, "out$o");
	} else {
		credits++;
	}
	delete msg;
}

// if we have a pop it means we read a mem access and now we can send it
void CoreL1::handlePopMsg() {
	// format the request based on the current state
	char msgName[32];
	sprintf(msgName, "corel1-%d.%d-req-%d", srcId, appId, curMsgId);
	CMPMsg *req = new CMPMsg(msgName);
	req->setKind(NOC_APP_MSG);
	req->setMsgId(curMsgId++);
	req->setSrcId(srcId);
	req->setSrcAppId(appId);
	req->setDstId(curL2Id);
	req->setDstAppId(0);
	req->setOperation(curOp);
	req->setOrigin(1);
	req->setL2hit(curL2Hit);
	req->setAddress(curAddr);
	req->setLength(curSize_b);
	req->setAppMsgLen(cacheLinePackets);
	req->setPktLength(cacheLinePktFlits);
	req->setRoundtrip(false);

	// Tag everything except HIT WRITE (It doesn't return a response)
	if((curOp != CMP_OP_WRITE) && (curL2Hit!=1)) { // TODO: check logics
		req->setRoundtrip(true);
//		cerr << "Request:" << req << "\nIs a miss transaction\n";
	}

	if ((curOp == CMP_OP_READ) && req->getRoundtrip()) {
		curReadStartTime = simTime();
//		assert(ResponseDB::getInstance()->exists(req->getId())==false); // Check there is no ID Multiplicity
//		SessionMeta *meta = new SessionMeta(req);
//		ResponseDB::getInstance()->add(meta);
//		cerr << "Adding CMPMsg to response db\n";
		ResponseDB::getInstance()->add(req);
//		cerr << "Testing that it was actually added...\n";

		if(!ResponseDB::getInstance()->exists(req)) {
		    cerr << "It wasn't added :(\n";
		    throw cRuntimeError("ResponseDB Didn't register CMP Message");
		}

	}

	if (!outQ.empty() || !credits) {
		outQ.insert(req);
	} else {
		EV << "-I- " << getFullPath() << " sending msg: " << req->getMsgId()
			<< " to: " << req->getDstId() << endl;
		send(req, "out$o");
		credits--;
	}

	// writes do not wait for response!
	if (curOp == CMP_OP_WRITE) {
		if (readNextMemAccess()) {
			scheduleAt(simTime() + curDelayToNextMem_s, popMsg);
		} else {
			EV<< " -I- " << getFullPath() << " trace completed" << endl;
		}
	}
}

// on receiving return a credit, parse next line and if ok set the next pop
void CoreL1::handleCmpMsg(CMPMsg *msg) {
	// send a credit to the NI to enable next recv
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd, "in$o");
	l2ReadTimeHist.collect(simTime()-curReadStartTime);

	if(msg->getRoundtrip()) {
		if(ResponseDB::getInstance()->exists(msg->getId())) {
			SessionMeta *meta = ResponseDB::getInstance()->find(msg->getId());
			meta->end();
			requestResponseHist.collect(simTime()-meta->getStartTime());
		}
	}

	cancelAndDelete(msg);
	if (readNextMemAccess()) {
		scheduleAt(simTime() + curDelayToNextMem_s, popMsg);
	} else {
		EV<< " -I- " << getFullPath() << " trace completed" << endl;
	}
}

void CoreL1::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	if (msgType == NOC_POP_MSG) {
		handlePopMsg();
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg *) msg);
	} else if (msgType == NOC_APP_MSG) {
		handleCmpMsg((CMPMsg *) msg);
	} else {
		throw cRuntimeError("-E- BUG unknown msg type:%d ", msgType);
	}
}

void CoreL1::finish()
{
	l2ReadTimeHist.record();
	requestResponseHist.record();
	if (finished == false){
		//EV<< " -I- " << getFullPath() << " trace is not completed !!" << endl;
		recordScalar("complete-time-ns", -1);

	}
}

CoreL1::~CoreL1()
{
	CMPMsg *msg;
	while (!outQ.empty()) {
		msg = (CMPMsg *)outQ.pop();
		cancelAndDelete(msg);
	}
	if (popMsg)
		cancelAndDelete(popMsg);
}
