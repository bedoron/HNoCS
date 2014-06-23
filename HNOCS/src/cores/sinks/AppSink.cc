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

#include "AppSink.h"
#include "ResponseDB.h"

Define_Module(AppSink);

void AppSink::initialize()
{
    moduleId = getParentModule()->getIndex();
	numVCs = par("numVCs");
	numApps = gate("app$o",0)->size();

	end2EndLatency.setName("end-to-end-latency-ns"); // end-to-end latency per flit
	networkLatency.setName("network-latency-ns"); // network-latency per flit
	packetLatency.setName("packet-network-latency-ns"); // network-latency per packet

	// statistics for head-flits only
	SoPEnd2EndLatency.setName("SoP-end-to-end-latency-ns");
	SoPLatency.setName("SoP-network-latency-ns");
	SoPQTime.setName("SoP-queueing-time-ns");

	// statistics for tail-flits only
	EoPEnd2EndLatency.setName("EoP-end-to-end-latency-ns");
	EoPLatency.setName("EoP-network-latency-ns");
	EoPQTime.setName("EoP-queueing-time-ns");

	numReceivedPkt.setName("number-received-packets");

	// Vectors
	end2EndLatencyVec.setName("end-to-end-latency-ns");

	numRecPkt = 0;

	vcFLITs.resize(numVCs, 0);
	vcFlitIdx.resize(numVCs, 0);
	curPktId.resize(numVCs, -1);
	vcStartId.resize(numVCs, -1);

	SoPFirstNetTime.resize(numVCs, 0);
	statStartTime = par("statStartTime");

	// send the credits to the other size
	for (int vc = 0; vc < numVCs; vc++)
		sendCredit(vc, 100, "EXT"); // TODO: This is stupid -- horriffic w/a

	SoPEnd2EndLatencyHist.setName("SoP-E2E-Latency-Hist");
}

void AppSink::sendCredit(int vc, int num, const char* attach) {
	char credName[512];
	sprintf(credName, "sink-cred-%d-%d", vc, 1);
	if(attach != NULL) {
	    sprintf(credName, "%s [%s]", credName, attach);
	}
	NoCCreditMsg *crd = new NoCCreditMsg(credName);
	crd->setKind(NOC_CREDIT_MSG);
	crd->setVC(vc);
	crd->setFlits(num);
	send(crd, "in$o");
}

void AppSink::handleDataMsg(AppFlitMsg *flit) {
	int vc = flit->getVC();
	sendCredit(vc, 1, flit->getFullName());

	// some statistics
	if (simTime() > statStartTime) {
		vcFLITs[vc]++;

		if (flit->getFirstNet()) {
			throw cRuntimeError(
					"-E- BUG - received flit on vc %d, but firstNet flag set is true !",
					vc);
		}

		double eed = (simTime().dbl() - flit->getCreationTime().dbl());
		double d = (simTime().dbl() - flit->getFirstNetTime().dbl());
		double eed_ns = eed * 1e9;
		double d_ns = d * 1e9;

		end2EndLatency.collect(eed_ns);
		networkLatency.collect(d_ns);
		end2EndLatencyVec.record(eed_ns);

	if (flit->getType() == NOC_START_FLIT) {
			SoPEnd2EndLatency.collect(eed_ns);
			SoPEnd2EndLatencyHist.collect(eed_ns);

			SoPLatency.collect(d_ns);
			SoPQTime.collect(1e9 * (flit->getInjectTime().dbl()
					- flit->getCreationTime().dbl()));

			if (SoPFirstNetTime[vc] == 0) {
				SoPFirstNetTime[vc] = flit->getFirstNetTime();
				EV<< "-I- " << getFullPath() << "Assign SoPFirstNetTime[" << vc <<"]=" <<SoPFirstNetTime[vc] << endl;
			} else {
				throw cRuntimeError(
						"-E- BUG - SoPFirstNetTime[%d] != 0 at SoP statistics procedure ",
						vc);
			}
			numRecPkt++;
		}
		if (flit->getType() == NOC_END_FLIT) {
			EoPEnd2EndLatency.collect(eed_ns);
			EoPLatency.collect(d_ns);
			EoPQTime.collect(1e9 * (flit->getInjectTime().dbl() - flit->getCreationTime()).dbl());
			if (SoPFirstNetTime[vc] != 0) { // avoid collecting statistics when statStartTime is between SoP and EoP
				packetLatency.collect(1e9 * (simTime().dbl() - SoPFirstNetTime[vc].dbl()));
			}
			SoPFirstNetTime[vc] = 0;
		}
	}

	if (flit->getType() == NOC_END_FLIT) {
		// de-encapsulate the the embedded message on the last FLIT of the message
	    int pktIdx = flit->getPktIdx();
	    int appMsgLen = flit->getAppMsgLen();
		if (flit->getPktIdx() == flit->getAppMsgLen() - 1) {
			cPacket *decapMsg = flit->decapsulate();
			if (!decapMsg) {
				throw cRuntimeError("-E- BUG last flit of message does not carry msg??? ");
			}
			AppMsg* appMsg = check_and_cast< AppMsg * >(decapMsg);
			appMsg->setByteLength(appMsg->getOrigBytes());
			int dstApp = appMsg->getDstAppId();
			if (dstApp >= gate("app$o",0)->size()) {
				throw cRuntimeError("-E- BUG - got a message to app[%d] > %d ",
						dstApp, gate("app$o",0)->size()-1);
			}
			EV << "-I- " << getFullPath() << " fwd App Msg from: " << appMsg->getSrcId()
			<< "." << appMsg->getSrcAppId() << " msgId: " << appMsg->getMsgId()
			<< " to app: " << dstApp << endl;
			int id = vcStartId[vc];
//			ResponseDB *db 			= ResponseDB::getInstance();
//			SessionMeta *session 	= db->find(id);
			if(ResponseDB::getInstance()->exists(id)) {
				ResponseDB::getInstance()->find(id)->add(appMsg->getId());
			//	ResponseDB::getInstance()->find(appMsg->getId())->print(cerr);
			}

			send(appMsg, "app$o", dstApp);
		}
	}

	// Some checking...
	// PktId check ...
	if (flit->getType() == NOC_START_FLIT) {
		if (curPktId[vc] == -1) {
			curPktId[vc] = flit->getPktId();
			vcStartId[vc] = flit->getId();
		} else {
			throw cRuntimeError(
					"-E- BUG - Received SoP Index %d but expecting Pkt index %d on vc %d",
					flit->getPktId(), curPktId[vc], vc);
		}
	} else {

		if (flit->getPktId() != curPktId[vc]) {
			throw cRuntimeError(
					"-E- BUG - Received Pkt Index %d but expecting Pkt index %d on vc %d",
					flit->getPktId(), curPktId[vc], vc);
		}

	}

	if (flit->getType() == NOC_END_FLIT) {
		curPktId[vc] = -1;
		vcStartId[vc] = -1;
	}

	// flit Idx check ...
	if (vcFlitIdx[vc] != flit->getFlitIdx()) {
		throw cRuntimeError(
				"-E- BUG - Received flit Index %d but expecting flit index %d on vc %d",
				flit->getFlitIdx(), vcFlitIdx[vc], vc);
	}

	if (flit->getType() == NOC_END_FLIT) {
		if (vcFlitIdx[vc] == (flit->getFlits() - 1)) {
			vcFlitIdx[vc] = 0;
		} else {
			throw cRuntimeError(
					"-E- BUG - Received flit EoP but expecting flit index %d on vc %d",
					vcFlitIdx[vc], vc);
		}

	} else {
		vcFlitIdx[vc]++;
	}

	delete flit;
}

// for now we do not count credits from the App side - we assume it will receive any
// received msg
void AppSink::handleAppCreditMsg(cMessage* msg)
{
	delete msg;
}

void AppSink::logMsg(int modId, AppFlitMsg* appMsg, const char* prefix) {
    if (moduleId == modId) {
        std::cerr << prefix << ":" << appMsg->getFullName() <<  " - (vc:" << appMsg->getVC()<<") "<< appMsg->getId() << "\n";
    }
}

void AppSink::handleMessage(cMessage *msg)
{
	int msgType = msg->getKind();
	if (msgType == NOC_CREDIT_MSG) {
		handleAppCreditMsg((NoCCreditMsg*) msg);
	} else if (msgType == NOC_FLIT_MSG) {
//	    logMsg(1, (AppFlitMsg*) msg, "TO-SINK");
	    handleDataMsg((AppFlitMsg*) msg);
	} else {
		throw cRuntimeError("-E- BUG - unknown message type %d", msgType);
	}
}

void AppSink::finish()
{
	char name[32];
	double totalFlits = 0;
	int flitSize_B = par("flitSize"); // in bytes
	for (int vc = 0; vc < numVCs; vc++) {
		sprintf(name, "flit-per-vc-%d", vc);
		recordScalar(name, vcFLITs[vc]);
		totalFlits += vcFLITs[vc];
	}
	if (simTime() > statStartTime) {
		SoPEnd2EndLatency.record();
		SoPEnd2EndLatencyHist.record();
		SoPLatency.record();
		SoPQTime.record();
		EoPEnd2EndLatency.record();
		EoPLatency.record();
		EoPQTime.record();

		packetLatency.record();
		networkLatency.record();
		end2EndLatency.record();

		numReceivedPkt.collect(numRecPkt);
		numReceivedPkt.record();
		double BW_MBps = 1e-6 * totalFlits * flitSize_B / (simTime().dbl()- statStartTime);
		recordScalar("Sink-Total-BW-MBps", BW_MBps);
	}
}
