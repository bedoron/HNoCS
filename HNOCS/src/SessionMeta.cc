// Author: Doron Ben Elazar, b.e.doron@gmail.com
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
#include <string>
#include "SessionMeta.h"
#include <iterator>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <strstream>
#include <unistd.h>

using std::string;
using std::find;
using std::cerr;
using std::stringstream;

unsigned int SessionMeta::m_sessionCounter = 0; // Initialize session


SessionMeta::SessionMeta(MsgId id): m_state(SESSION_META_REQUEST), m_start(0),
		m_gen(0), m_end(0), dependant(0), dependantValid(false), m_sessionId(++m_sessionCounter) {
    cerr << "USING DEPRECATED METHOD SessionMeta::SessionMeta(int id)\n";

	m_start = cSimulation::getActiveSimulation()->getSimTime();
	addRequest(id);
}

SessionMeta::SessionMeta(CMPMsg *msg): m_state(SESSION_META_REQUEST), m_start(0),
		m_gen(0), m_end(0), dependant(0), dependantValid(false), m_sessionId(++m_sessionCounter) {
	m_start = cSimulation::getActiveSimulation()->getSimTime();

	sessionOrigins_t origin = static_cast<sessionOrigins_t>(msg->getOrigin());
	switch(origin) {
	case ORIGIN_L1: route = L1_L2; break;
	case ORIGIN_L2: route = L2_L3; break;
	case ORIGIN_L3: route = L3_DRAM; break;
	default:
	    cerr << "Origin of " << msg->getOrigin() << " is not supported, statistics are not to be trusted\n";
	    break;
	}
	addRequest(msg->getId());
}

unsigned int SessionMeta::getSessionId()  {
    return m_sessionId;
}

void SessionMeta::addRequest(MsgId id) {
	m_state = SESSION_META_REQUEST;
	m_requestIds.push_front(id);
}

void SessionMeta::addResponse(MsgId id) {
	m_state = SESSION_META_RESPONSE;
	if(m_responseIds.empty())
		m_gen = cSimulation::getActiveSimulation()->getSimTime();
	m_responseIds.push_front(id);
}

void SessionMeta::add(MsgId id) {
	switch(m_state) {
	case SESSION_META_REQUEST:	addRequest(id); 	break;
	case SESSION_META_RESPONSE: addResponse(id); 	break;
	default:	break;
	}
}

void SessionMeta::add(CMPMsg *msg) {
	add(msg->getId());
}

void SessionMeta::add(NoCFlitMsg* flit) {
    AppFlitMsg* msg = dynamic_cast<AppFlitMsg*>(flit);
    assert(msg!=0); // We support only AppFlitMsg
    add(flit->getId());
}

void SessionMeta::addInPort(int inp) {
    m_inPort.push_front(inp);
}

void SessionMeta::addOutPort(int outp) {
    m_outPort.push_front(outp);
}




bool SessionMeta::isRequest(MsgId id) const {
    if(m_requestIds.empty()) return false;
    deque<MsgId>::const_iterator it = find(m_requestIds.begin(), m_requestIds.end(), id);
    if((it)!= m_requestIds.end())
        return true;
    return false;
    //return id==m_requestIds.front();
}
bool SessionMeta::isResponse(MsgId id) const {
    if(m_responseIds.empty()) return false;
    deque<MsgId>::const_iterator it = find(m_responseIds.begin(), m_responseIds.end(), id);
    if((it)!= m_responseIds.end())
        return true;
    return false;
    //return id==m_responseIds.front();
}

bool SessionMeta::isSpecial(MsgId id) const { return (isRequest(id) || isResponse(id)); }

void SessionMeta::traverseRouter(int router_id) {
//	if(m_traversedRouters.front()!=router_id) {  -- Signal system crashes because of odd palyndrom length
		m_traversedRouters.push_front(router_id);
		m_traversedRoutersTime.push_front(cSimulation::getActiveSimulation()->getSimTime());
//	}
}

void SessionMeta::addRouter(int id)
{
	traverseRouter(id);
}

void SessionMeta::addRouter(NoCFlitMsg* flit, int routerId) {
    AppFlitMsg* msg = dynamic_cast<AppFlitMsg*>(flit);
    assert(msg!=0); // We support only AppFlitMsg for the time being
    if(msg->getPktIdx()==0)
        addRouter(routerId);
}

void SessionMeta::end() { m_end = cSimulation::getActiveSimulation()->getSimTime(); } // Route collection ended
void SessionMeta::start() { m_start = cSimulation::getActiveSimulation()->getSimTime(); }
void SessionMeta::gen() { m_gen = cSimulation::getActiveSimulation()->getSimTime(); }


bool SessionMeta::operator==(const MsgId id) const { return isSpecial(id); }

SessionMeta& SessionMeta::operator=(const SessionMeta& rhs) {
	if(&rhs==this)
		return *this;
	m_requestIds.resize(rhs.m_requestIds.size());
	m_responseIds.resize(rhs.m_responseIds.size());
	m_traversedRouters.resize(rhs.m_traversedRouters.size());
	copy(rhs.m_requestIds.begin(), rhs.m_requestIds.end(), m_requestIds.begin());
	copy(rhs.m_responseIds.begin(), rhs.m_responseIds.end(), m_responseIds.begin());
	copy(rhs.m_traversedRouters.begin(), rhs.m_traversedRouters.end(), m_traversedRouters.begin());
	m_start = rhs.m_start;
	m_end = rhs.m_end;
	m_gen = rhs.m_gen;
	m_state = rhs.m_state;
	dependant = rhs.dependant;
	dependantValid = rhs.dependantValid;
	return *this;
}

SessionMeta::SessionMeta(const SessionMeta& src): m_state(src.m_state), m_start(src.m_start),
		m_gen(src.m_start), m_end(src.m_end), dependant(src.dependant), dependantValid(src.dependantValid) {
	m_requestIds.resize(src.m_requestIds.size());
	m_responseIds.resize(src.m_responseIds.size());
	m_traversedRouters.resize(src.m_traversedRouters.size());

	copy(src.m_requestIds.begin(), src.m_requestIds.end(), m_requestIds.begin());
	copy(src.m_responseIds.begin(), src.m_responseIds.end(), m_responseIds.begin());
	copy(src.m_traversedRouters.begin(), src.m_traversedRouters.end(), m_traversedRouters.begin());
}

SessionMeta::~SessionMeta() {
    // print into a file whatever
//    std::cerr << "Session meta killed\n";
//    std::cerr << this->extraStatistics();
//    std::cerr << "\n";
}

std::ostream& SessionMeta::printIDs(std::ostream &os) {
	os << "Resp: ";
	deque<MsgId>::const_iterator itReq = m_requestIds.begin();
	deque<MsgId>::const_iterator itRes = m_responseIds.begin();
	for(;itRes != m_responseIds.end(); ++itRes)
		os << "[" << (*itRes) << "] " << " ";
	os << "\nReq:  ";
	for(;itReq!=m_requestIds.end(); ++itReq)
		os << "[" << (*itReq) << "] " << " ";
	os << "\n";
	return os;
}

std::ostream& SessionMeta::print(std::ostream &os) {
	os << "R: ";

	deque<int>::const_iterator it = m_traversedRouters.end() - 1;
	for(; it >= m_traversedRouters.begin(); it--) {
		os << " " << (*it);
	}
	os << "\nT: ";
	deque<simtime_t>::const_iterator it_t = m_traversedRoutersTime.end() -1;
	for(; it_t >= m_traversedRoutersTime.begin(); it_t--) {
		os << " " << (*it_t);
	}

	os << "\n";
	return os;
}

const simtime_t& SessionMeta::getStartTime() const {
	return m_start;
}

int SessionMeta::getRouterOccurances(int router_id) const {
	return -1; // To be continued
}

simtime_t SessionMeta::getRouterRequestTime(int router_id) const {
    // NOTE: t1 > t2 !!!!!
    deque<int>::const_reverse_iterator it = m_traversedRouters.rbegin();
    for(;it != m_traversedRouters.rend(); ++it) {
        if((*it)==router_id)
            break;
    }
    if(it==m_traversedRouters.rend()) {
        stringstream ss;
        ss << "Router " << router_id << " wasn't found in session " << m_sessionId;
        throw new string(ss.str());
    }

    int index = it-m_traversedRouters.rbegin();
    return m_traversedRouters.at(index);
}

pair<int, int> SessionMeta::getRouterRoundtripLocation(int router_id) const {
    // NOTE: t1 > t2 !!!!!
    deque<int>::const_iterator it1 = find(m_traversedRouters.begin(), m_traversedRouters.end(), router_id);
    deque<int>::const_iterator it2 = it1;

    if((it1 + 1)!=m_traversedRouters.end())
        it2 = find(it1+1, m_traversedRouters.end(), router_id);

    if(it2 == m_traversedRouters.end()) {
        throw string("Router not in turnaround path");
    }
    int first = it2 - m_traversedRouters.begin();
    int second = it1 - m_traversedRouters.begin();

    return pair<int, int>(first, second);
}

Incidents SessionMeta::getRouterIncidents(int router_id) const {
    pair<int, int> incidents = getRouterRoundtripLocation(router_id);

    simtime_t t1 = m_traversedRoutersTime.at(incidents.first);
    simtime_t t2 = m_traversedRoutersTime.at(incidents.second);

    return Incidents(t1, t2);
}

simtime_t SessionMeta::getRouterDelta(int router_id) const {
    Incidents times = getRouterIncidents(router_id);
    return (times.second - times.first);
}

void SessionMeta::setParent(SessionMeta* meta) {
	//printIDs(cerr);
	dependantValid = true;
	dependant = meta;
}

SessionMeta* SessionMeta::getParent(bool override) {
	//printIDs(cerr);
	if(override) return dependant;
	dependantValid = false;
	return dependant;
}

bool SessionMeta::hasParent(bool override) {
	if(override) return (dependant != 0);
	return dependantValid;
}

sessionMetaRoute_t SessionMeta::getRoute() {
    return route;
}

map<int, int> SessionMeta::getUniqueRouters() {
    map<int, int> uniqueRouters;
    deque<int>::iterator it = m_traversedRouters.begin();
    for(;it != m_traversedRouters.end(); ++it) {
        uniqueRouters[(*it)] += 1;
    }
    return uniqueRouters;
}

string SessionMeta::extractStatistics(int routerId) {
    stringstream ss;
    pair<int, int> incidents = getRouterRoundtripLocation(routerId);
//    Incidents times = getRouterIncidents(router_id);

    ss << "{";
        ss << "\"router_id\": " << routerId << ", ";
        ss << "\"session_id\": " << m_sessionId << ", ";
        ss << "\"session_type\": \"" << typeToString() << "\", ";
        ss << "\"response_ts\": " << m_traversedRoutersTime.at(incidents.second) << ", ";
        ss << "\"request_ts\": " << m_traversedRoutersTime.at(incidents.first) << ", ";
//        ss << "\"request_out_port\": " << m_outPort[incidents.first] << ", ";
//        ss << "\"response_out_port\": " << m_outPort[incidents.second];
        ss << "\"request_out_port\": " << m_inPort[incidents.second] << ", ";
        ss << "\"response_out_port\": " << m_inPort[incidents.first];
    ss << "}";
    return ss.str();
}

string SessionMeta::extractStatistics() {
    stringstream ss;
    deque<int>::iterator it = m_traversedRouters.begin();
    pair<int, int> prevIncidents;

    for(;it != m_traversedRouters.end(); ++it) {
        pair<int, int> incidents;
        ss << "router["<< (*it) << "] ,";
        try {
            incidents = getRouterRoundtripLocation((*it));
        } catch(string &s) {
            ss << "0, " << m_outPort[prevIncidents.first+1] << "," <<  m_outPort[prevIncidents.first+1];
            ss << typeToString() << "*";
            ss << "\n";
            break;
        }
        ss << extractStatistics((*it)) << "\n";
        prevIncidents = incidents;
    }
    return ss.str();
}
