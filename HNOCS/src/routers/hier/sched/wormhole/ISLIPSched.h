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

#ifndef __HNOCS_ISLIP_SCHED_H_
#define __HNOCS_ISLIP_SCHED_H_

#include <omnetpp.h>
#include <NoCs_m.h>
#include <HierRouter.h>

//
// Crossbar Scheduler
//
// Ports:
//  inout in - the input of data and requests from the switch crossbar
//  inout out - the NoC router output
//
// Event:
//   Req - Received on the "in" port carry the request of a InPort to send data on specific
//         VC through that OutPort
//   ACK - accept to a provided grant - i.e. the InPort is using that grant
//   FLIT/Packet - the data being provided from the InPort
//   Credit - received from the other side of the out port
//
// The scheduler in general can implemented as a single module for the
// entire router but this implementation is for a single router output port
// This scheduler supports multiple VCs per port and provides round robin
// between input ports and round robin between VCs
//
// NOTE: for every input port there is a single packet that is granted by the
// scheduler. This means that there is no bypassing of VC traffic during packet
// transmission (from the same port).
//
class ISLIPSched : public Sched
{
private:
	// parameters
	int numVCs;
	double tClk_s;

	bool speculativeGntOnCompltedReq; // if true allow sending Gnt on next Req while waiting for complted Req Acks
	bool winnerTakesAll; // if true then the last arbitrated packet will not switch to other VC until EoP

	// state
	int numInPorts;
	int numReqs;  // total number of requests
	std::vector< std::vector< std::list<NoCReqMsg*> > > HoQ; // active requests by [ip][vc]
	std::vector< int > credits; // credits per VC
	std::vector< int > vcUsage; // count number of pending reqs per VC
	cMessage *clkMsg; // this is the clock...

	// bool waitForAck; // true if a grant was provided and not ACK/NAK yet
	bool isDisconnected; // if true means there is no InPort or Core on the other side

	// arbitration state
	int curVC; // last VC sent
	std::vector<int> vcCurInPort;  // last port sending on this VC
	std::vector<NoCReqMsg*> vcCurReq; // the current Req (last one arbitrated on a vc)

	// statistics
	cOutVector vc0Credits;

	// methods
	void handleFlitMsg(NoCFlitMsg *msg );
	void handleReqMsg(NoCReqMsg *msg);
	void handleAckMsg(NoCAckMsg *msg);
	void handleClkMsg(cMessage *msg);
	void handleCreditMsg(NoCCreditMsg *msg);
	void arbitrate();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    const std::vector<int> *getCredits() const {return &credits;};
    const std::vector<int> *getVCUsage() const {return &vcUsage;};
    virtual void incrVCUsage(int vc) { vcUsage[vc]++ ; } ;

    virtual ~ISLIPSched();
};

#endif
