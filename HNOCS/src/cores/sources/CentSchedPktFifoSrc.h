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

#ifndef __HNOCS_CENTRAL_SCHED_SOURCE_H_
#define __HNOCS_CENTRAL_SCHED_SOURCE_H_

#include <omnetpp.h>
#include <NoCs_m.h>
#include <Central_m.h>

//
// A source of Packets made out of FLITs on a single VC (0)
// In order to send a packet need first to send a request to the central scheduler
// and only when approved sent it. Can send next request only after finished all sends.
//
class CentSchedPktFifoSrc : public cSimpleModule
{
private:
	// parameters:
	int srcId;
	int dstId;
	int flitSize_B;

	// state:
	int pktIdx;
	int flitIdx;
	int curPktLen;
	int curPktId;
	int curPktVC;
	int numQueuedPkts;
	int maxQueuedPkts;
	cQueue Q;
	NoCPopMsg *popMsg; // used to pop packets modeling the wire BW
	cMessage  *genMsg; // used to gen next flit
	int credits;       // number of credits on VC=0
	int approvedPktId; // keep the packet Id of the current approved packet
	int curMsgDst;     // the destination of the current msg
	int curMsgLen;     // length in packets of current msg
	int curPktIdx;     // the packet index in the msg

	// Statistics
	cLongHistogram dstIdHist;
	cOutVector dstIdVec;
	cStdDev queueSize; // queue fill in % tracked every generation event

	// methods
	void sendCentralReq();
	void sendFlitFromQ();
	void handleGenMsg(cMessage *msg);
	void handleCreditMsg(NoCCreditMsg *msg);
	void handlePopMsg(cMessage *msg);
	void handleCentReqMsg(CentralRequestMsg *msg);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

public:
    virtual ~CentSchedPktFifoSrc();
};

#endif
