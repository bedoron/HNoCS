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

#include "AppSrc.h"
#include "ResponseDB.h"
#include <string>
#include "CMP_m.h"
#include <assert.h>
#include "Utils.h"

using std::string;

Define_Module(AppSrc);

void AppSrc::initialize() {
	arbitration_type = par("arbitration_type");
	switch (arbitration_type) {
		case 0: // 0- winner takes all
			arbiterStarOnNextVCandApp=0;
		break;
		case 1:// 1- round robin
			arbiterStarOnNextVCandApp=1;
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
		msgPerAppVC[i].resize(numVCs,NULL);

	credits.resize(numVCs, 0);
	WATCH_VECTOR(credits);

	vcCurMsg.resize(numVCs, NULL);
	pktIdx.resize(numVCs, 0);
	flitIdx.resize(numVCs, 0);
	statStartTime = par("statStartTime");

	srcId = par("srcId");
	popMsg = NULL;

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
			send(creditMsg,"in$o",app);
		}
	}
	char popMsgName[32];
	sprintf(popMsgName, "pop-src-%d", srcId);
	popMsg = new NoCPopMsg(popMsgName);
	popMsg->setKind(NOC_POP_MSG);
	dstIdHist.setName("dst-hist");
}

// return true if found any msg to arbitrate or false if no message or no change
bool AppSrc::arbitrate() {

	// loop to find something to do
	if (!ev.isDisabled()) {
		EV << "-I- " << getFullPath() << " credits: ";
		for (int vc = 0; vc < numVCs; vc++)
			EV << vc << ":" << credits[vc] << " ";
		EV << endl;
		EV << "-I- " << getFullPath() << " messages: ";
		for (int app = 0; app < numApps; app++)
			for (int vc = 0; vc < numVCs; vc++)
				EV << app << "," << vc << ":" << (msgPerAppVC[app][vc] ? "Y" : "N") << " ";
		EV << endl;
	}

	// try next VC if not winner takes all
	for (int i = arbiterStarOnNextVCandApp; (i <= numVCs); i++) {
		int vc = (curVC + i) % numVCs;

		// we can not arbitrate the VC in the middle of a message
		if (vcCurMsg[vc] && (pktIdx[vc] < vcCurMsg[vc]->getAppMsgLen())) // if still sending packets from previous msg
			return(true);

		for (int j = arbiterStarOnNextVCandApp; j<=numApps;j++){
			int app = (curApp + j) % numApps;

			// if there is an outstanding message
			if (msgPerAppVC[app][vc]) {
				curVC = vc;
				curApp = app;
				vcCurMsg[vc] = msgPerAppVC[app][vc];
				msgPerAppVC[app][vc] = NULL;
				char creditMsgName[32];
				sprintf(creditMsgName, "crd-app-%d", app);
				NoCCreditMsg *creditMsg = new NoCCreditMsg(creditMsgName)  ;
				creditMsg->setKind(NOC_CREDIT_MSG);
				creditMsg->setVC(vc);
				creditMsg->setMsgs(1);
				creditMsg->setSchedulingPriority(0);
				pktIdx[vc] = 0;
				flitIdx[vc] = 0;
				send(creditMsg,"in$o",app);
				EV << "-I- " << getFullPath() << " arbitrate app: " << curApp << " vc: " << curVC << endl;
				return(true);
			}
		}
	}

	EV << "-I- " << getFullPath() << " nothing to arbitrate" << endl;
	return(false);
}

// send the FLIT out and schedule the next pop
void AppSrc::sendFlit(int vc) {
	if (credits[vc] <= 0)
		return;

	if (!vcCurMsg[vc] || (pktIdx[vc]== vcCurMsg[vc]->getAppMsgLen()))
		return;

	if (popMsg->isScheduled())
		return;

	int dstId = vcCurMsg[vc]->getDstId();

	char flitName[128];
	sprintf(flitName, "flit-s:%d-t:%d-p:%d-f:%d", srcId, dstId, pktIdx[vc],
			flitIdx[vc]);

	AppFlitMsg *flit = new AppFlitMsg(flitName);
	flit->setKind(NOC_FLIT_MSG);
	flit->setByteLength(flitSize_B);
	flit->setBitLength(8 * flitSize_B);
	flit->setVC(vc);
	flit->setSrcId(srcId);
	flit->setDstId(dstId);
	flit->setSchedulingPriority(0);
	flit->setFirstNet(true);
	flit->setPktId((srcId << 16) + (vcCurMsg[vc]->getMsgId() << 8) + pktIdx[vc]);
	flit->setFlitIdx(flitIdx[vc]);
	flit->setFlits(vcCurMsg[vc]->getPktLength());
	flit->setFirstNet(false); // for debug only!!
	flit->setDstAppId(vcCurMsg[vc]->getDstAppId());
	flit->setSrcAppId(vcCurMsg[vc]->getSrcAppId());
	flit->setPktIdx(pktIdx[vc]);
	flit->setAppMsgLen(vcCurMsg[vc]->getAppMsgLen());

	if (flitIdx[vc] == 0) {
		CMPMsg *msg = (CMPMsg*)(vcCurMsg[vc]);

//		EV << "Found head flit to tag";
		if(msg->getRoundtrip()) { // Head flit which is meant to be marked - Notice that we need to deal only with the first head
		    SessionMeta *meta = ResponseDB::getInstance()->find(msg->getId());
		    if(meta == 0) {
		        cerr << "************************************************************\n";
		        cerr << "Failure while looking for " << msg->getId() << " in ResponseDB \n";
		        cerr << msg << "\n";
		        throw cRuntimeError("No Session for a roundtrip message");
		    }
		    assert(meta != 0); //
		    meta->add(flit);
		}

		flit->setType(NOC_START_FLIT);
		flitIdx[vc]++;
		dstIdHist.collect(dstId);
	} else if (flitIdx[vc] == vcCurMsg[vc]->getPktLength() - 1) {
		flitIdx[vc]=0;
		pktIdx[vc]++;
		flit->setType(NOC_END_FLIT);

		// if the last FLIT of the message encapsulate the AppMsg after setting it size to zero
		if (vcCurMsg[vc]->getAppMsgLen() == pktIdx[vc]) {
			vcCurMsg[vc]->setOrigBytes(vcCurMsg[vc]->getByteLength());
			vcCurMsg[vc]->setBitLength(0);
			vcCurMsg[vc]->setByteLength(0);
			flit->encapsulate(vcCurMsg[vc]);
			vcCurMsg[vc] = NULL;
		}
	} else {
		flitIdx[vc]++;
		flit->setType(NOC_MID_FLIT);
	}

//	if(40990 == flit->getId()) {
	static int headz = 0;

	CMPMsg *msg = (CMPMsg*)(vcCurMsg[vc]);
    if((headz==0) && (msg!=0) && (msg->getId()==1011)) { // Flit is 1020
        ++headz;
        cerr << "---\n";
	    cerr << "***************************************************\n";
	    cerr << "Core ID: " << getParentModule()->getIndex() << "\n";
	    cerr << "MSG SRC: " << msg->getSrcId() << " DST: " << msg->getDstId() << "\n";
	    cerr << "Flit belongs to message " << msg->getId() << " Roundtrip is: "<< msg->getRoundtrip() << "\n";
	    cerr << flit;



	    SessionMeta *meta1 = ResponseDB::getInstance()->find(msg->getId());
	    SessionMeta *meta2 = ResponseDB::getInstance()->find(flit->getId());



	    cerr << "MSG SESSION ADDR: " << meta1 << " FLIT SESSION ADDR: " << meta2 << "\n";
	    cerr << "Session ID is: " << meta1->getSessionId() << "\n";
	    cerr << "Is request- " << meta1->isRequest(flit->getId()) << " Is response- " <<
	            meta1->isResponse(flit->getId()) << "\n";

	    cerr << "****************************************************\n";
	    cerr << msg << "\n";
	    cerr << "****************************************************\n";
	}



	flit->setInjectTime(simTime());
	EV<< "-I- " << getFullPath() << " flit injected at time: " << flit->getInjectTime() << endl;
	send(flit, "out$o");
	credits[vc]--;

	scheduleAt(gate("out$o")->getTransmissionChannel()->getTransmissionFinishTime(), popMsg);
}

void AppSrc::handleCreditMsg(NoCCreditMsg *msg) {
	int vc = msg->getVC();
	int flits = msg->getFlits();
	delete msg;
	credits[vc] += flits;
	sendFlit(vc);
}

void AppSrc::handleAppMsg(AppMsg *msg) {
	int vc = msg->getVC();
	int app= msg->getArrivalGate()->getIndex();

	if (msgPerAppVC[app][vc]){
		throw cRuntimeError("-E- %s Writing to full msgPerAppVC[%d][%d] ",
				getFullPath().c_str(),app,vc);
	}
	msgPerAppVC[app][vc] = msg;

	if (arbitrate()) sendFlit(curVC);
}

void AppSrc::handlePopMsg(cMessage *msg) {
	if (arbitrate())
		sendFlit(curVC);
}

void AppSrc::handleMessage(cMessage *msg) {
	int msgType = msg->getKind();
	if (msgType == NOC_POP_MSG) {
		handlePopMsg((NoCPopMsg*) msg);
	} else if (msgType == NOC_CREDIT_MSG) {
		handleCreditMsg((NoCCreditMsg*) msg);
	} else{
		handleAppMsg((AppMsg*) msg);
	}
}

void AppSrc::finish() {
	dstIdHist.record();
}

AppSrc::~AppSrc() {
	if (popMsg) {
		cancelAndDelete(popMsg);
	}

	for (int vc = 0; vc < numVCs; vc++) {
		for (int app = 0; app < numApps; app++) {
			if (msgPerAppVC[app][vc])
				cancelAndDelete(msgPerAppVC[app][vc]);
		}
		if (vcCurMsg[vc])
			cancelAndDelete(vcCurMsg[vc]);
	}
}
