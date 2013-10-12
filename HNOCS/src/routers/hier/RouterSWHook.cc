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
#include "RouterSWHook.h"
#include <omnetpp.h>

Define_Module(RouterSWHook);

#include "ResponseDB.h"
#include <App_m.h>
#include <iostream>
#include <deque>
#include <simtime.h>
#include <assert.h>
using std::deque;
using std::cerr;
using OPP::simtime_t;

// Event Handler
void IncidenceListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) {
    cResponseHeadArrived *data = 0;
    if((data = dynamic_cast<cResponseHeadArrived*>(obj))) {
        owner->packetEvent(data);
    }
}

void RouterSWHook::initialize() {
	id = par("id");
	session_deltas.setName("session-deltas-in-router");
//session_deltas.setNumFirstVals(100000);
//	session_deltas.setRange(0.0, 8.0e-7);
	//session_deltas.setRangeAuto(100, 10);
	session_deltas.setRangeAutoUpper(0, 10000); // Avoid starting negative tests
	phase_graph.setName("session-phases-in-router_global");

	phase_graph_l1_l2.setName("session-phases-in-router_l1-l2");
	phase_graph_l2_l3.setName("session-phases-in-router_l2-l3");
	phase_graph_l3_dram.setName("session-phases-in-router_l3-dram");

//	phase_graph.setMax(5e-5);
//	phase_graph.setMin(0);

	listener.setHook(this);
	getParentModule()->subscribe("flit-second-incidence", &listener); // Register to router level event
}

void RouterSWHook::finish() {
    session_deltas.record();
    getParentModule()->unsubscribe("flit-second-incidence", &listener);

#ifdef DNY_DEBUG
    if(getParentModule()->getId() == 10) {
        Ticks::iterator it = ticks.begin();
        for(;it != ticks.end(); ++it) {
            cerr << "[" << it->first << ", " << it->second << "]\n";
        }
        //ticks[times.second-times.first] += 1;
    }
#endif
}

void RouterSWHook::packetEvent(cResponseHeadArrived *resp) {
    if(resp->router->getIndex()!=getParentModule()->getIndex()) {
        cerr << "Event Router and Own Router Differ :( ^^^^^^^^^^^\n";
    }
    AppFlitMsg* appFlit = dynamic_cast<AppFlitMsg*>(resp->flit);
    assert(appFlit!=0);

    bool isFirstPkt = (appFlit->getPktIdx()==0);
    if(isFirstPkt) updateStatistics(resp); // Collect data for the first head in the first packet ONLY!
}

void RouterSWHook::updateStatistics(cResponseHeadArrived *resp) {
    Incidents times = resp->session->getRouterIncidents(resp->router->getIndex());
    simtime_t delta     = times.second-times.first;
    simtime_t incident  = times.second;
    sessionMetaRoute_t  route = resp->session->getRoute();

    phase_graph.recordWithTimestamp(incident,delta); // Vector - global
    session_deltas.collect(delta);   // Histogram

    // fine grained statistical coverage for route specific packets
    switch(route) {
    case L1_L2:     phase_graph_l1_l2.recordWithTimestamp(incident, delta);     break;
    case L2_L3:     phase_graph_l2_l3.recordWithTimestamp(incident, delta);     break;
    case L3_DRAM:   phase_graph_l3_dram.recordWithTimestamp(incident, delta);   break;
    }

#ifdef DNY_DEBUG
    AppFlitMsg* appFlit = dynamic_cast<AppFlitMsg*>(resp->flit);
    if(getParentModule()->getId() == 10) {
           ticks[times.second-times.first] += 1;
    }
    session_deltas.collect(cSimulation::getActiveSimulation()->getSimTime());
    if(times.second < 0) {
        cerr << "Turnaround of flit " << appFlit->getPktId() <<" is in negative time " << times.second << "\n";
    }
    if(times.first < 0) {
        cerr << "First Incident of flit " << appFlit->getPktId() <<" is in negative time " << times.first << "\n";
    }
#endif

}

