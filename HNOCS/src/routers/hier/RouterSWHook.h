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

#ifndef ROUTERSWHOOK_H_
#define ROUTERSWHOOK_H_
#include <omnetpp.h>
#include <clistener.h>
#include "cResponseHeadArrived.h"

#include <map>

class RouterSWHook; // Defined below
class IncidenceListener: public ::cListener {
    RouterSWHook    *owner;
public:
    void setHook(RouterSWHook *hook) { owner = hook; };
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

using std::map;
typedef map<simtime_t, int> Ticks;

class RouterSWHook: public cSimpleModule {
	int id;
	cDoubleHistogram    session_deltas;
	cOutVector          phase_graph;    // Capture all types of session which pass by this SW Hook

	cOutVector          phase_graph_l1_l2; // Capture only l1 <--> l2 sessions
	cOutVector          phase_graph_l2_l3; // Capture only l2 <--> l3 sessions
	cOutVector          phase_graph_l3_dram; // Capture only l3 <--> DRAM sessions

	IncidenceListener   listener;

	Ticks ticks;
	void updateStatistics(cResponseHeadArrived *resp);
protected:
    virtual void initialize();
    virtual void finish();
public:
    void packetEvent(cResponseHeadArrived *resp);
};

#endif /* ROUTERSWHOOK_H_ */
