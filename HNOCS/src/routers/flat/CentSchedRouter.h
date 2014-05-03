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
#include <map>
#include <FlitMsgCtrl.h>
using std::queue;
using std::vector;
using std::map;
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
	int flitsPerVC;
	double tClk_s;
	double dataRate;
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
	    vc_t* m_linkedTo;
	    int m_activePacket; // Current NoC packet being sent
	    int m_vcSize; // Size of VC
	    int m_id; // ID of this VC
	    int m_credits; // Current availability of credits
	    int m_linkCredits; // number of credits available by configuration (upper bound of m_credits)
	    bool m_used;

	    // Simulation of Pipeline delay
        unsigned int m_pipelineDepth; // Setting - nr. of clocks to delay a packet on this VC
        unsigned int m_pipelineStage; // Counter for current pipeline stage

        // parent path
        int m_portId;
        int m_routerId;

	    bool accept(NoCFlitMsg* flit); // Specify rules to accept flits
	    bool belongs(NoCFlitMsg* flit); // Check if flit "belongs"
	    bool empty(); // True if VC is empty
	    bool used(); // True if VC belongs to some flow
	    NoCFlitMsg* release(); // release one flit from Q and update state, throw exception if empty
	    void takeOwnership(NoCFlitMsg* flit);
	    bool canRelease(); // returns true if VC can release a FLIT - enough credits are available

	    vc_t(const vc_t& src) {
//	        std::cerr << "VC Copy CTOR Invoked on [" << src.m_routerId << "][" << src.m_portId << "][" << src.m_id << "]\n";
            m_activePacket = src.m_activePacket ; // Current NoC packet being sent
            m_vcSize = src.m_vcSize; // Size of VC
            m_id = src.m_id; // ID of this VC
            m_credits = src.m_credits; // Current availability of credits
            m_linkCredits = src.m_linkCredits; // number of credits available by configuration (upper bound of m_credits)

            // Simulation of Pipeline delay
            m_pipelineDepth = src.m_pipelineDepth; // Setting - nr. of clocks to delay a packet on this VC
            m_pipelineStage = src.m_pipelineStage; // Counter for current pipeline stage

            m_portId = src.m_portId;
            m_routerId = src.m_routerId;

            m_linkedTo = src.m_linkedTo;
            m_used = src.m_used;
	    }

	    vc_t() {
	        //m_flits; // Flits Queue
            m_activePacket = -1; // Current NoC packet being sent
            m_vcSize = -1; // Size of VC
            m_id = -1; // ID of this VC
            m_credits = -1; // Current availability of credits
            m_linkCredits = -1; // number of credits available by configuration (upper bound of m_credits)

            // Simulation of Pipeline delay
            m_pipelineDepth = 0; // Setting - nr. of clocks to delay a packet on this VC
            m_pipelineStage = 0; // Counter for current pipeline stage

            m_portId = -1;
            m_routerId = -1;

            m_linkedTo = NULL;
            m_used = false;
	    }
	};

	struct port_t {
	    int m_transmittingVC; // which VC should transmit
	    cGate *gate; // Associated gate
	    vector<vc_t> m_vcs; // vcs for this gate
	    int m_size; // maximum number of VC's for this gate
	    bool connected;
	    void electVC();
	    bool hasElectedVC();
	    bool hasData();
	    struct CentSchedRouter::vc_t& getElectedVC();
	    vc_t& getVC(NoCFlitMsg* msg); // Returns an available vc or an existing one if this flit is part of it

	    port_t(const port_t& src) {
//	        std::cerr << "Port Copy CTOR Invoked on [X]\n";
            m_transmittingVC = src.m_transmittingVC;
            gate = src.gate;
            m_vcs = src.m_vcs;
            m_size = src.m_size;
            connected = src.connected;
	    }

	    port_t() {
            m_transmittingVC = -1;
            gate = NULL;
            m_vcs.resize(0);
            m_size = 0;
            connected = false;
	    }
	};

	/**
	 * Maps the outgoing channel to the incoming channel. using this map the router
	 * can associate credits comming from one end to the other end and create a pass-by
	 * effect where incoming credit reaches the ingress port and not the egress one.
	 */
	map<vc_t*, vc_t*> m_linker; // Maps the outgoing channel to the incoming channel

	vector<port_t> m_ports; // In ports of this router. in ports deliver messages to next router

	// fill in west... port indexes
	int analyzeMeshTopology();

	void sendCredits(int ip, int otherVC, int numFlits);

	void handleCredit(NoCCreditMsg* msg);
	void handleFlitMsg(NoCFlitMsg* msg);
	void handlePop(NoCPopMsg* msg);
	void deliver(); // Send all pending packets that we can send
	int OPCalc(NoCFlitMsg* msg);

	bool hasData();

	bool isLinked(vc_t& source); // Checks if input vc has an output vc linked to it
	bool tryToLink(vc_t& srource, int outPort);

	static inPortFlitInfo* getFlitInfo(NoCFlitMsg* msg);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

public:
    static bool isHead(NoCFlitMsg *msg);
    static bool isTail(NoCFlitMsg *msg);
    static bool isHead(NoCFlitMsg& msg);
    static bool isTail(NoCFlitMsg& msg);

    ~CentSchedRouter();
};

#endif
