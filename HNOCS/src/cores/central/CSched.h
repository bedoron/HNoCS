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

#ifndef __CENTRAL_SCHED_CSCHED_H_
#define __CENTRAL_SCHED_CSCHED_H_

#include <omnetpp.h>
#include <Central_m.h>

//
// Centralized Scheduler
//
// On incoming scheduling request provide it back only when appropriate
// This scheduler is not fully parallel. It assumes a global Q of requests and
// serve as many as maxParallelReqs on a single cycle.
//
//
class CSched : public cSimpleModule
{
	// parameters
	int rows;
	int columns;
	bool ackEverything;
	double tClk_sec; // the clock of the scheduler
	bool usePathReservation;
	int  maxParallelReqs; // maximal number of reqs that are handled each cycle. 0 = infinite.

	// state
	std::list< CentralRequestMsg *> reqFifo;   // single request by each src
	std::vector< std::list< CentralRequestMsg *>* > ackFifo; // list of acks by tick from now

	std::vector< std::vector<unsigned long long int> > SLinksUsage, NLinksUsage; // by [row][col]
	std::vector< std::vector<unsigned long long int> > WLinksUsage, ELinksUsage; // by [row][col]
	std::vector< std::vector<unsigned long long int> > CLinksUsage;  // in to core by [row][col]
	std::vector< std::vector<unsigned long long int> > SLinksReserve, NLinksReserve; // by [row][col]
	std::vector< std::vector<unsigned long long int> > WLinksReserve, ELinksReserve; // by [row][col]
	std::vector< std::vector<unsigned long long int> > CLinksReserve;  // in to core by [row][col]
	cMessage *clkMsg;
	unsigned long int clkCycle;

	// methods
	void handleReq(CentralRequestMsg *msg);
	void handleClkMsg();
	int getEarliestTickForSrcDst(CentralRequestMsg *req);
	void allocateSchedForReq(unsigned int tick, CentralRequestMsg *req);

	// statistics
	cLongHistogram allocTicksHist;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

  public:
    virtual ~CSched();
};

#endif
