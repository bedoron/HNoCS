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

#ifndef SESSIONMETA_H_
#define SESSIONMETA_H_
#include <deque>
#include <simtime.h>
#include <csimulation.h>
#include <iostream>
#include <ostream>
#include <utility>
#include "CMP_m.h"
#include <string>
#include <map>

using std::map;
using std::deque;
using std::pair;
using std::ostream;
using std::string;
using std::cerr;

using OPP::simtime_t;
using OPP::cSimulation;

enum sessionMetaState_t { SESSION_META_REQUEST, SESSION_META_RESPONSE };
enum sessionMetaRoute_t { L1_L2, L2_L3, L3_DRAM };
enum sessionOrigins_t   { ORIGIN_L1=1, ORIGIN_L2=2, ORIGIN_L3=3 }; // this should correspond to origin types defined in cores

typedef pair<simtime_t, simtime_t> Incidents;
typedef long long int MsgId;

class SessionMeta {
	sessionMetaState_t m_state; // false for request
	// store flit's role change (NoC <--> Flit) in both ways
	// this is usually important when flit hops a source/drain to mesh/death
	deque<MsgId>          m_requestIds;
	deque<MsgId>          m_responseIds;
	deque<int> 		    m_traversedRouters;		// Store all the routers this session passed
	deque<simtime_t>    m_traversedRoutersTime;	// Store the corresponding times of routers passed
	deque<int>          m_inPort;
	deque<int>          m_outPort;


	simtime_t m_start;		// Request was generated
	simtime_t m_gen;		// Response was generated
	simtime_t m_end;		// Response was received at Request point

	SessionMeta *dependant; // Session which is waiting for this Session to end
	bool dependantValid;
	sessionMetaRoute_t route;
	void addRouter(int id);
	SessionMeta(MsgId id);

	inline string typeToString() {
	    string ss;
        switch(route) {
        case L1_L2: ss = "L1_L2"; break;
        case L2_L3: ss = "L2_DRAM"; break;
        case L3_DRAM: ss = "L3_DRAM"; break;
        }
        return ss;
	}

	unsigned int m_sessionId;


	static unsigned int m_sessionCounter;

public:
	// Init with a new request - implicit - will take a timestamp
	unsigned int getSessionId();
	sessionMetaState_t getState() { return m_state; }

	SessionMeta(CMPMsg *msg);

	SessionMeta(const SessionMeta& src); // C-Ctor

	void add(MsgId id);				// add according to current state context
	void add(CMPMsg *msg);				// add according to current state context
	void add(NoCFlitMsg* flit); // Router ID will be tagged only for pktIdx==0

	void addRequest(MsgId id);
	void addResponse(MsgId id);
	void addRouter(NoCFlitMsg* flit, int routerId); // Router ID will be tagged only for pktIdx==0

	void addRequest(CMPMsg *msg) { addRequest(msg->getId()); };
	void addResponse(CMPMsg *msg) { addResponse(msg->getId()); };

	void addInPort(int inp);
	void addOutPort(int outp);

	bool isRequest(MsgId id) const;	// check if packet is request
	bool isResponse(MsgId id) const;  // check if packet is response
	bool isSpecial(MsgId id) const; 	// check if packet is request OR response

	bool isRequest(CMPMsg *msg) const { return isRequest(msg->getId()); };	// check if packet is request
	bool isResponse(CMPMsg *msg) const { return isResponse(msg->getId());};  // check if packet is response
	bool isResponse(NoCFlitMsg *msg) const { return isResponse(((AppFlitMsg*)msg)->getId()); };
	bool isSpecial(CMPMsg *msg) const { return isSpecial(msg->getId()); }; 	// check if packet is request OR response

	void traverseRouter(int router_id);

	void end();
	void start();
	void gen();

	const simtime_t& getStartTime() const;

	// Check if packet is Request or Response
	bool operator==(const MsgId id) const; // wrapper to isSpecial
	bool operator==(const CMPMsg *msg) const { return this->operator==(msg->getId()); } // wrapper to isSpecial

	virtual ~SessionMeta();

	SessionMeta& operator=(const SessionMeta& rhs); // Assignment operator
	std::ostream& print(std::ostream &os);
	std::ostream& printIDs(std::ostream &os);

	simtime_t getRouterRequestTime(int router_id) const;
	simtime_t getRouterDelta(int router_id) const; // Get the router delta
	pair<int, int> getRouterRoundtripLocation(int router_id) const;
	Incidents getRouterIncidents(int router_id) const; // Get router incident times

	int getLastRouter() { return m_traversedRouters.front(); }

	int getRouterOccurances(int router_id) const;

	void setParent(SessionMeta* meta);

	SessionMeta* getParent(bool override = false); // Will also unset the parent

	bool hasParent(bool override = false);

	sessionMetaRoute_t getRoute(); // Get type of origin-dest nodes route type for statistics colleciton

	string extractStatistics(int routerId);
	string extractStatistics();

	map<int, int> getUniqueRouters();
};
/* :(
ostream& operator<<(ostream& stream, SessionMeta *meta) {
    meta->print(stream);
    return stream;
}
*/


#endif /* SESSIONMETA_H_ */
