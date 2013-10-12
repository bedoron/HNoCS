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
//
// The EcoApp is a simple test application.
// It generates new messages on genMsg (next one is happening in volatile msgDelay_s).
// It queues new messages and send them only if there are credits on the output
// It does not take time to send a message so it may send as many as there are credits (no Pop)
// If the destination Id is -1 at init the application does not generate any messages
//
#include "EchoApp.h"

Define_Module(EchoApp)
;

void EchoApp::initialize() {
	appId = par("appId");
	srcId = par("srcId");
	statStartTime = par("statStartTime");
	genMsg = NULL;
	numSentMsgs = 0;
	numGenMsgs = 0;
	sendCredits = 0;
	numRecvMsgs = 0;
	curVC = par("VC");

	// a dstId parameter of -1 turns off the source...
	int dstId = par("dstId");
	if (dstId < 0) {
		EV<< "-I- " << getFullPath() << " is turned OFF" << endl;
	} else {
		char genMsgName[32];
		sprintf(genMsgName, "app-%d-gen",  appId);
		genMsg = new cMessage(genMsgName);
		genMsg->setKind(NOC_GEN_MSG);
		scheduleAt(0.5e-9, genMsg);
	}

	// tell our friends we are ready to receive one message...
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	send(crd,"in$o");
}

// send a messages out of the queue and if there are credits
// this routine is called whenever credit is received or a new message is generated into the Q
void EchoApp::sendMsgs() {
	while (sendCredits && !Q.empty()) {
		AppTestMsg *msg = (AppTestMsg*)Q.pop();
		msg->setTimestamp(simTime());
	    EV << "-I- " << getFullPath() << " Sent msg:" << msg->getPayload() << endl;
		send(msg, "out$o");
		sendCredits--;
		numSentMsgs++;
	}
}

// generate a new Msg and Q it then call sendMsgs
void EchoApp::handleGenMsg() {
	int curMsgId = numGenMsgs++;
	int curMsgLen = par("msgLen");
	int curPktLen = par("pktLen");
	int curDstId  = par("dstId");
	int curDstApp = par("dstAppId");

	char msgName[128];
	sprintf(msgName, "Msg-fa:%d-t:%d-ta:%d-m:%d", appId, curDstId, curDstApp, curMsgId);
	AppTestMsg *appMsg = new AppTestMsg(msgName);
	appMsg->setKind(NOC_APP_MSG);
	appMsg->setMsgId(curMsgId);
	appMsg->setSrcAppId(appId);
	appMsg->setDstAppId(curDstApp);
	appMsg->setAppMsgLen(curMsgLen);
	appMsg->setPktLength(curPktLen);
	appMsg->setSchedulingPriority(0);
	appMsg->setDstId(curDstId);
	appMsg->setSrcId(srcId);
    appMsg->setVC(curVC);
    appMsg->setPayload(msgName);
    EV << "-I- " << getFullPath() << " Generated and Queued msg:" << msgName << endl;
    Q.insert(appMsg);
	sendMsgs();

	// schedule next genMsg
	simtime_t nextGen = simTime() + par("msgDelay");
	scheduleAt(nextGen, genMsg);
}

void EchoApp::handleCreditMsg(NoCCreditMsg *msg) {
	if (msg->getVC() == curVC) {
		sendCredits += msg->getMsgs();
		sendMsgs();
	}
	delete msg;
}

void EchoApp::handleRecvMsg(AppTestMsg *msg) {
	NoCCreditMsg *crd = new NoCCreditMsg();
	crd->setKind(NOC_CREDIT_MSG);
	crd->setMsgs(1);
	crd->setVC(msg->getVC());
	send(crd,"in$o");
    EV << "-I- " << getFullPath() << " Received msg:" << msg->getPayload() << endl;
    numRecvMsgs++;
	delete msg;
}
void EchoApp::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	if (msgType == NOC_GEN_MSG) {
		handleGenMsg();
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg*) msg);
	} else {
		// must be a received msg
		AppTestMsg* appMsg = check_and_cast< AppTestMsg * >(msg);
		handleRecvMsg(appMsg);
	}
}

void EchoApp::finish() {
	recordScalar("generated-msgs", numGenMsgs);
	recordScalar("sent-msgs", numSentMsgs);
	recordScalar("num-received", numRecvMsgs);
}

EchoApp::~EchoApp() {
	if (genMsg) {
		cancelAndDelete(genMsg);
	}
}
