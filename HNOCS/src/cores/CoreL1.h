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
// Changes: 01/12/2012:
// Ben Elazar Doron b.e.doron@gmail.com, Yevgeny Dibrov yevgenydibrov@gmail.com
// Added Histogram data collection for tagged packets. (Req-Resp pair)

#ifndef __INOC_CMP_COREL1_H_
#define __INOC_CMP_COREL1_H_

#include <omnetpp.h>
#include <CMP_m.h>
#include <fstream>

//
// A CPU Core and its Attached L1 Application
//
class CoreL1 : public cSimpleModule
{
public:
	// parameters
    int appId;    		  // must be unique in the Core
    int srcId;    		  // local srcId
    const char *traceFileName; // the name of the trace file to parse
    const char *l2IdsStr; // a list of L2 module IDs
    int l2AppId; // L2 destination app
    int cacheLinePackets;  // response num packets
	int cacheLinePktFlits; // response packet size
    double tClk_s;
    int cachePageSize; // result of FlitSize * cacheLinePackets * cacheLinePktFlits

    // state
    bool outstandingMemAccess; // if true need to wait for response
    std::ifstream trace;      // the trace stream - assuming format: t-from-prev r-or-w l2hit-0-or-1 address size
    double curDelayToNextMem_s;
    int curOp;
    int curL2Hit;
    int curAddr;
    int curSize_b;
    int curL2Id;
    cMessage *popMsg;
    cQueue outQ;          // holds all out messages
    int credits;
    int curMsgId;
    std::vector<int> l2Ids;
    int lineNum; // the trace file line number
    simtime_t curReadStartTime;
    bool finished; // is trace finished ?
    // statistics
	cDoubleHistogram l2ReadTimeHist;
	cDoubleHistogram requestResponseHist;


    bool readNextMemAccess();
	void handleCreditMsg(NoCCreditMsg *msg);
	void handleCmpMsg(CMPMsg *msg);
	void handlePopMsg();
	virtual ~CoreL1();

//	int send(cMessage *msg, const char *gatename, int gateindex=-1); // Override regular send
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
	virtual void finish();
};

#endif
