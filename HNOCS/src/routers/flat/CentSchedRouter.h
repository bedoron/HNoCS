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

#ifndef __HNOCS_CENTRAL_SCHED_CENTSCHEDROUTER_H_
#define __HNOCS_CENTRAL_SCHED_CENTSCHEDROUTER_H_

#include <omnetpp.h>
#include <NoCs_m.h>

#include <queue>
#include <vector>
#include <FlitMsgCtrl.h>
using std::queue;
using std::vector;
//
// Central Scheduling Router - it's main task is to
// send the input flits to the correct output port
// It implements XY routing only
//
class CentSchedRouter : public cSimpleModule
{
private:
	// parameters
	int id;
	int numPorts;
	int numCols;
	int numRows;
	double tClk_s;
	const char* routerType;
	const char* coreType;

	cMessage *popMsg; // this is the clock...
	// state
	int westPort, eastPort, northPort, southPort, corePort;
	int rx, ry; // xy coordinates of this router

	// methods

	// my stuff
	struct vc_t {
	    queue<NoCFlitMsg*> m_flits; // Flits Queue
	    int m_activeMessage; // Current CMP message being sent
	    int m_activePacket; // Current NoC packet being sent
	    int m_vcSize;
	    int m_credits;
	    bool accept(NoCFlitMsg& flit); // Specify rules to accept flits
	    bool empty(); // True if VC is empty
	    NoCFlitMsg& release(); // release one flit from Q and update state, throw exception if empty
	};

	struct port_t {
	    cGate *gate; // Associated gate
	    vector<vc_t> m_vcs; // vcs for this gate
	    int m_size; // maximum number of VC's for this gate
	    bool connected;
	    vc_t& getVC(NoCFlitMsg* msg); // Returns an available vc or an existing one if this flit is part of it
	};

	vector<port_t> m_ports;


	// fill in west... port indexes
	int analyzeMeshTopology();

	void sendCredits(int ip, int numFlits);

	void handleCredit(NoCCreditMsg *msg);
	void handleFlitMsg(NoCFlitMsg *msg);
	void handlePop(NoCPopMsg *msg);
	void deliver(); // Send all pending packets that we can send

	static inPortFlitInfo* getFlitInfo(NoCFlitMsg *msg);
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    static bool isHead(NoCFlitMsg *msg);
    static bool isTail(NoCFlitMsg *msg);
    static bool isHead(NoCFlitMsg& msg);
    static bool isTail(NoCFlitMsg& msg);
};

#endif
