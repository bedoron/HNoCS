/*
 * ResponseDB.cpp
 *
 *  Created on: Nov 10, 2012
 *      Author: Doron
 */
#include "ResponseDB.h"
#include <algorithm>
#include <iostream>
#include <exception>
#include <string>
#include <deque>

using std::deque;

using std::string;
using std::exception;
using std::find;
using std::cerr;
using std::cout;

ResponseDB* ResponseDB::_instance = 0;
/*
ResponseDB*  ResponseDB::getInstance() {
		if(_instance==0)
			_instance = new ResponseDB();
		return _instance;
}
*/
ResponseDB*  ResponseDB::getInstance() {
    static ResponseDB instance;
    return &instance;
}

ResponseDB::ResponseDB(): lastQuery(0) { }
ResponseDB::~ResponseDB() {
    std::cerr << "Destryoying ResponseDB - this is going to be super slow\n";

    char buffer[500];
    getcwd(buffer, 500);
    printf("%s\n", buffer);
    string fpath = buffer;
    fpath += "/results/responseDB_by_router.txt";
    std::cerr << "Fpath: " << fpath << "\n";
    unlink(fpath.c_str());
    ofstream output(fpath.c_str(), ofstream::out);
    if(!output.is_open()) {
        std::cerr << "Failed creating " << fpath << " for writing, nothing will happen\n";
        goto dtor_end;
    }
    saveSortedByRouter(output);
    output.close();

    fpath = buffer; fpath += "/results/responseDB_by_session.txt";
    unlink(fpath.c_str());
    output.open(fpath.c_str(), ofstream::out);
    if(!output.is_open()) {
        std::cerr << "Failed creating " << fpath << " for writing, nothing will happen\n";
        goto dtor_end;
    }
    saveSortedBySession(output);
    output.close();

dtor_end:
    deque<SessionMeta*>::iterator it;
    deque<SessionMeta*>::iterator sit;
    for(it = metas.begin();it != metas.end(); ++it) {
        SessionMeta *tmp = (*it);
        delete tmp;
    }
    //delete _instance;
}

void ResponseDB::saveSortedBySession(ofstream& output) {
    deque<SessionMeta*>::iterator it;
    for(it = metas.begin(); it != metas.end(); ++it) {
        output << (*it)->extractStatistics();
    }
}


void ResponseDB::saveSortedByRouter(ofstream& output) {
        map<int, deque<SessionMeta*> > router_ids; // order sessions by routers
        map<int, deque<SessionMeta*> >::iterator riit;
        deque<SessionMeta*>::iterator it;

        // Sort Sessions by routers - assuming that a self loop is impossible

        for(it = metas.begin(); it != metas.end(); ++it) { // lookup all routers
            map<int, int> routers = (*it)->getUniqueRouters();
            for(map<int,int>::iterator rit = routers.begin(); rit != routers.end(); ++rit) {
                if(0 == (rit->second%2)) {
                    router_ids[rit->first].push_back((*it));
                } else {
                   // std::cerr << "router[" << rit->first << "] is not in turnaround path - " << rit->second << "\n";
                }
            }
        }

        output << "[\n";

        int routers = router_ids.size();
        for(riit = router_ids.begin(); riit != router_ids.end(); ++riit) {
            deque<SessionMeta*> &dq = riit->second;
            int router_id = riit->first;

            deque<SessionMeta*>::iterator dit = dq.begin();
            int elements = dq.size();

            output << "\t[\n"; // Begin JSON Array
            for(; dit != dq.end(); ++dit) {
                string stats;
                try {
                    stats = (*dit)->extractStatistics(router_id);
                } catch(string e) {
                    cerr << "\n**** Exception router[" << router_id << "] " << e << "\n";
                }
                output << "\t" << stats;
                if((--elements)) {
                    output << ",";
                }
                output << "\n";
            }
            output << "\t]"; // End JSON Array
            if((--routers)) {
                output << ", ";
            }
            output << "\n";
        }

        output << "]\n";
}



bool ResponseDB::isResponse(MsgId id)
{
	SessionMeta* tmp = find(id);
	if(tmp==0)
		return false;
	return tmp->isResponse(id);
}

bool ResponseDB::isRequest(MsgId id)
{
	SessionMeta* tmp = find(id);
	if(tmp==0)
		return false;
	return tmp->isRequest(id);
}

void ResponseDB::add(SessionMeta *meta) {
	metas.push_front(meta);
	m_sessionsById[meta->getSessionId()] = meta;
}


SessionMeta* ResponseDB::add(CMPMsg *msg) {
    if(exists(msg->getId())) {
        cerr << "Cannot add msg " << msg->getId() << ", it already exists!! \n";
        return NULL; // Check there is no ID Multiplicity
    }
    SessionMeta *meta = new SessionMeta(msg);
    add(meta);
    return meta;
}

SessionMeta* ResponseDB::findBySessionId(int sessionId) const{
    map<unsigned int, SessionMeta*>::iterator iter = m_sessionsById.find(sessionId);
    if(iter == m_sessionsById.end())
        return NULL;
    return iter->second;
}


SessionMeta* ResponseDB::find(MsgId id) const {
	if((lastQuery!=0) && ((*lastQuery)==id)) { // Check cache
		return lastQuery;
	}

	deque<SessionMeta*>::const_iterator it = metas.begin();
	for(;it!=metas.end(); it++) {
		if((*it)->isSpecial(id))
			break;
	}
	lastQuery = (it==metas.end())?0:*it;
	return lastQuery;
}

SessionMeta* ResponseDB::find(CMPMsg *msg) const {
	return find(msg->getId());
}

SessionMeta* ResponseDB::find(NoCFlitMsg *msg) const {
    return find(((AppFlitMsg*)msg)->getId());
}



bool ResponseDB::exists(CMPMsg *msg) const {
	return exists(msg->getId());
}

bool ResponseDB::exists(MsgId id) const {
	SessionMeta* tmp = find(id);
	if(tmp==0)
		return false;
	else
		return true;
}

deque<pair<simtime_t, simtime_t> > ResponseDB::getRouterDeltas(int router_id) const {
    deque<pair<simtime_t, simtime_t> > deltas;

    deque<SessionMeta*>::const_iterator it = metas.begin();
    for(;it!=metas.end();it++) {
        try {
            Incidents incident = (*it)->getRouterIncidents(router_id);
            pair<simtime_t, simtime_t> data(incident.second, incident.second - incident.first);
            deltas.push_back(data);
        } catch(string &e){ cerr << e; }
    }
    return deltas;
}
