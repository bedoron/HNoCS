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

#ifndef __TRUNK_PARALLELCENTRALSCHED_H_
#define __TRUNK_PARALLELCENTRALSCHED_H_

#include <omnetpp.h>
#include <Central_m.h>

//
// A parallel processor for scheduling. This processor is able to process all
// non contending requests in a single scheduling cycle that takes schedClks cycles.
//
class ParallelCentralSched : public cSimpleModule
{
	enum DIRECTION {WEST, EAST, NORTH, SOUTH, CORE};
	const char *getDirStr(DIRECTION dir);

	// parameters
	int rows;
	int columns;
	bool ackEverything;
	double tClk_sec; // the clock of the scheduler
	int  schedClks;  // clocks between scheduling

	// state
	std::vector<std::vector <CentralRequestMsg *> > reqByRowCol; // single request by each src
	std::vector< std::list< CentralRequestMsg *>* > ackFifo; // list of acks by tick from now

	std::vector< std::vector<unsigned long long int> > SLinksUsage, NLinksUsage; // by [row][col]
	std::vector< std::vector<unsigned long long int> > WLinksUsage, ELinksUsage; // by [row][col]
	std::vector< std::vector<unsigned long long int> > CLinksUsage;  // in to core by [row][col]
	std::vector< std::vector<int> > SLinksOldestReqHops, NLinksOldestReqHops; // by [row][col]
	std::vector< std::vector<int> > WLinksOldestReqHops, ELinksOldestReqHops; // by [row][col]
	std::vector< std::vector<int> > CLinksOldestReqHops;  // in to core by [row][col]
	std::vector<std::vector <CentralRequestMsg *> > NLinksOldestReq; // single request by each src
	std::vector<std::vector <CentralRequestMsg *> > SLinksOldestReq; // single request by each src
	std::vector<std::vector <CentralRequestMsg *> > WLinksOldestReq; // single request by each src
	std::vector<std::vector <CentralRequestMsg *> > ELinksOldestReq; // single request by each src
	std::vector<std::vector <CentralRequestMsg *> > CLinksOldestReq; // single request by each src
	std::vector< std::vector<unsigned long long int> > NLinksOldestUsage;
	std::vector< std::vector<unsigned long long int> > SLinksOldestUsage;
	std::vector< std::vector<unsigned long long int> > WLinksOldestUsage;
	std::vector< std::vector<unsigned long long int> > ELinksOldestUsage;
	std::vector< std::vector<unsigned long long int> > CLinksOldestUsage;
	int assignedReqs; // track for a scheduling cycle the number of assigned reqs
	int pendingReqs;  // track for a scheduling cycle the number of pending reqs
	cMessage *clkMsg;
	unsigned long int clkCycle;

	// methods
	void handleReq(CentralRequestMsg *msg);
	void handleClkMsg();

	bool isPassing(DIRECTION dir, int r, int c, CentralRequestMsg *req);
	int considerReqFromDirToDir(DIRECTION fromDir, DIRECTION toDir, int r, int c);

	// propagate the requests towards the cores as much as possible
	// eventually filling the CLinksOldestReqHops and CLinksOldestReq
	int propagateReqTowardDst(DIRECTION dir, unsigned int row, unsigned int col);

	// treat the given core outstanding request pushing it into first possible ackFifo tick
	int pushCoreReqIntoFifo(unsigned int row, unsigned int col);

	// statistics
	cStdDev assignedPerCycle;
	cStdDev unAssignedPerCycle;
	cLongHistogram allocTicksHist;
	cLongHistogram assignedPerCycleHist;   // track number of assigned requests per assignment cycle
	cLongHistogram unAssignedPerCycleHist; // track pending but unserviced requests per assignment cycle

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

  public:
    virtual ~ParallelCentralSched();
};

#endif
