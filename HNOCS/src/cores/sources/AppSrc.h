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

#ifndef __HNOCS_APPSOURCE_H_
#define __HNOCS_APPSOURCE_H_

#include <omnetpp.h>
#include <NoCs_m.h>
#include <App_m.h>

//
// A source driven by App vector - that is each App can inject messages and the
// source provides arbitration and segmentation into FLITs.
// The App message is then encapsulated into the last FLIT of the message
//
class AppSrc: public cSimpleModule {
private:
	// parameters:
    int arbitration_type;     // 0- winner takes all , 1- round robin ,
	int numVCs;
	int numApps;
	int srcId;
	int flitSize_B;
	simtime_t statStartTime;  // in sec

	// state
	int arbiterStarOnNextVCandApp; // if 0 we are in winner takes all if 1 start with next VC
	int curVC;                     // last VC sent
	int curApp;                    // last App sent
	std::vector<AppMsg*> vcCurMsg; // the current msg (last one arbitrated on a vc)
	std::vector<int> pktIdx;       // packet index in message need to be sent on cur VC
	std::vector<int> flitIdx;      // flit index in packet index.

	int curPktLen;
	int curPktId;
	int curPktVC;
	int curMsgDst;			       // the destination of the current msg
	int curMsgDstApp;              // the destination app of current sent message
	int curMsgLen;			       // length in packets of current msg
	int curPktIdx;                 // the packet index in the msg

	std::vector< std::vector<int> > numSentMsgs;// number of sent messages, assume that there is only single destination
	std::vector< std::vector<AppMsg*> > msgPerAppVC;  // App messages by app and VCout
	NoCPopMsg *popMsg;                          // used to pop packets modeling the wire BW
	std::vector<int> credits;                   // number of credits on all VCs
	double tClk_s;                              // clk extracted from output channel

	// Statistics
	cLongHistogram dstIdHist;
	cOutVector dstIdVec;
	cStdDev numSentPkt; // number of sent packets, assume that there is only single destination

	// methods
	void sendFlitFromQ();
	void sendFlit(int vc);
	void handleCreditMsg(NoCCreditMsg *msg);
	void handleAppMsg(AppMsg *msg);
	void handlePopMsg(cMessage *msg);
	bool arbitrate();


protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

public:
    virtual ~AppSrc();
};

#endif
