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

#ifndef __INOC_CMP_DRAM_H_
#define __INOC_CMP_DRAM_H_

#include <omnetpp.h>
#include <CMP_m.h>
//
// A DRAM Controller App
//
class DRAM : public cSimpleModule
{
public:
	// parameters:
	int srcId;
    int appId;             // must be unique in the Core
    double memLatency_s;   // the latency from request to get mem for one req
    int cachePagePackets;  // number of packets in cache page
    int cachePacketFlits;  // number of flits in each cache packet

    // state
    cQueue reqQ;      // store read and write requests
    cQueue respQ;     // strore out going msgs
    cMessage *popMsg; // used to enable pop from the Q
    int credits;      // number of messages that we can send

    void handlePopMsg();
    void handleCmpMsg(CMPMsg *msg);
	void handleCreditMsg(NoCCreditMsg *msg);
    virtual ~DRAM();
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
