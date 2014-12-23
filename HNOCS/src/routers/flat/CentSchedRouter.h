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
#include "../hier/FlitMsgCtrl.h"
#include "FlatPort.h"
#include "predictors/Predictor.h"
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
	int dataRate;
	const char* routerType;
	const char* coreType;

	cMessage *popMsg; // this is the clock...
	// state
	int westPort, eastPort, northPort, southPort, corePort;
	int rx, ry; // xy coordinates of this router

	// methods

	// my stuff
	vector<FlatPortIfc*> ports;

	unsigned int pipelineDepth;
	bool finishInit;
	unsigned long ticks;

	Predictor *predictor;
	// fill in west... port indexes
	int analyzeMeshTopology();

	void handleCredit(NoCCreditMsg* msg);
	void handleFlitMsg(NoCFlitMsg* msg);
	void handlePop(NoCPopMsg* msg);
//	void deliver(); // Send all pending packets that we can send

	bool hasData();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

public:
    FlatPortIfc* requestPort(int portId);

    void sendCredits(int ip, int otherVC, int numFlits);
    void callPredictor(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC);

    int OPCalc(NoCFlitMsg* msg);


    ~CentSchedRouter();
};

#endif
