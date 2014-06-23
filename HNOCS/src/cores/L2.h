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

#ifndef __INOC_CMP_L2_H_
#define __INOC_CMP_L2_H_

#include <omnetpp.h>
#include <CMP_m.h>

//
// L2 Cache Module
//
class L2 : public cSimpleModule
{
public:
	// params
    int appId;    // must be unique in the Core
    int srcId;    // local srcId
    int dramId;   // we assume DRAM appId is 0
    double l2HitLatency_s; // time it takes to respond if read hit
    int cacheLinePackets;  // response num packets
    int cacheLinePktFlits; // response packet size
    int cachePagePackets;  // number of packets in cache page
    int cachePacketFlits;  // number of flits in each cache packet

    // state
    int credits;          // to send
    cQueue outQ;          // holds all out messages
    cQueue dramReadReqQ;  // holds read requests waiting for DRAM
    cQueue hitReadReqQ;   // holds read requests waiting for hit latency
    cMessage *popMsg;     // schedule moved from hitReadReqQ to outQ


    int moduleId;

	void handleCreditMsg(NoCCreditMsg *msg);
	void handleHitRead(CMPMsg *msg);
	void handleMissRead(CMPMsg *msg);
	void handleHitWrite(CMPMsg *msg);
	void handleMissWrite(CMPMsg *msg);
	void handleCmpMsg(CMPMsg *msg);
	void handleDramResp(CMPMsg *msg);
	void handlePopMsg();
	virtual ~L2();

 protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

private:
    void logMsg(int modId, CMPMsg* resp, const char* prefix);
};

#endif
