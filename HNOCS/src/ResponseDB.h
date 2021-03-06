/*
 * ResponseDB.h
 *
 *  Created on: Nov 10, 2012
 *      Author: Doron
 */

#ifndef RESPONSEDB_H_
#define RESPONSEDB_H_
#include <deque>
#include <simtime.h>
#include <csimulation.h>
#include <iostream>
#include <utility>
#include "SessionMeta.h"
#include "CMP_m.h"
#include <fstream>
#include <map>

using OPP::simtime_t;
using std::deque;
using std::pair;
using std::map;

using std::cerr;
using std::ofstream;

// This is not thread safe.
class ResponseDB {
	static ResponseDB *_instance;
//	deque<int> responses;	// Backward compatibility
	deque<SessionMeta*> metas;
	mutable SessionMeta* lastQuery;
	map<unsigned int, SessionMeta*> m_sessionsById;

public:
	void saveSortedByRouter(ofstream& output);
	void saveSortedBySession(ofstream& output);
	ResponseDB();
	static ResponseDB* getInstance();

	bool isResponse(MsgId id);			// legacy - to remove
	bool isRequest(MsgId id);

	void add(SessionMeta *meta);	// will be added per NoC request-response pair
	SessionMeta* add(CMPMsg *msg);          // will be added per NoC request-response pair, use packet as basis

	SessionMeta* find(MsgId id) const;		// get SessionMeta associated by packet id, null if not
	SessionMeta* find(NoCFlitMsg *msg) const;      // get SessionMeta associated by packet id, null if not
	SessionMeta* find(CMPMsg *msg) const; // use get id

	SessionMeta* findBySessionId(int sessionId);


	bool exists(MsgId id) const;		// check if flit id exists in DB
	bool exists(CMPMsg *msg) const; // use getId

	virtual ~ResponseDB();
	//deque<simtime_t> getRouterDeltas(int router_id) const;
	/*
	 * first:   last time session traversed router
	 * second:  delta between Incidents of session
	 */
	deque<pair<simtime_t, simtime_t> > getRouterDeltas(int router_id) const;


};

#endif /* RESPONSEDB_H_ */
