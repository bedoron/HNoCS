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

#include "ParallelCentralSched.h"

Define_Module(ParallelCentralSched)
;
using std::hex;
using std::showbase;
using std::ios_base;
#define LONG_INT_BITS 64

const char *ParallelCentralSched::getDirStr(DIRECTION dir) {
	if (dir == NORTH)
		return ("NORTH");
	else if (dir == SOUTH)
		return ("SOUTH");
	else if (dir == WEST)
		return ("WEST");
	else if (dir == EAST)
		return ("EAST");
	else if (dir == CORE)
		return ("CORE");
	else {
		throw cRuntimeError("-E- Unknown Direction: %d", dir);
	}
}

void ParallelCentralSched::initialize() {
	rows = par("rows");
	columns = par("columns");
	ackEverything = par("ackEverything");
	tClk_sec = par("tClk");
	schedClks = par("schedClks");

	// init the requests matrix
	reqByRowCol.resize(rows);

	// initialize link usage matrixes
	// NOTE: we add one extra row or column as it makes indexing simpler
	WLinksUsage.resize(rows);
	ELinksUsage.resize(rows);
	NLinksUsage.resize(rows);
	SLinksUsage.resize(rows);
	CLinksUsage.resize(rows);
	WLinksOldestReqHops.resize(rows);
	ELinksOldestReqHops.resize(rows);
	NLinksOldestReqHops.resize(rows);
	SLinksOldestReqHops.resize(rows);
	CLinksOldestReqHops.resize(rows);
	NLinksOldestReq.resize(rows);
	SLinksOldestReq.resize(rows);
	WLinksOldestReq.resize(rows);
	ELinksOldestReq.resize(rows);
	CLinksOldestReq.resize(rows);
	NLinksOldestUsage.resize(rows);
	SLinksOldestUsage.resize(rows);
	WLinksOldestUsage.resize(rows);
	ELinksOldestUsage.resize(rows);
	CLinksOldestUsage.resize(rows);

	for (int r = 0; r < rows; r++) {
		reqByRowCol[r].resize(columns, NULL);
		WLinksUsage[r].resize(columns, 0);
		ELinksUsage[r].resize(columns, 0);
		NLinksUsage[r].resize(columns, 0);
		SLinksUsage[r].resize(columns, 0);
		CLinksUsage[r].resize(columns, 0);
		WLinksOldestReqHops[r].resize(columns, 0);
		ELinksOldestReqHops[r].resize(columns, 0);
		NLinksOldestReqHops[r].resize(columns, 0);
		SLinksOldestReqHops[r].resize(columns, 0);
		CLinksOldestReqHops[r].resize(columns, 0);
		NLinksOldestReq[r].resize(columns, NULL);
		SLinksOldestReq[r].resize(columns, NULL);
		WLinksOldestReq[r].resize(columns, NULL);
		ELinksOldestReq[r].resize(columns, NULL);
		CLinksOldestReq[r].resize(columns, NULL);
		NLinksOldestUsage[r].resize(columns, 0);
		SLinksOldestUsage[r].resize(columns, 0);
		WLinksOldestUsage[r].resize(columns, 0);
		ELinksOldestUsage[r].resize(columns, 0);
		CLinksOldestUsage[r].resize(columns, 0);
	}

	// we must start with at least one list of ackFifo
	std::list<CentralRequestMsg *> * reqList = new std::list<
			CentralRequestMsg *>;
	ackFifo.push_back(reqList);

	clkCycle = 0;

	clkMsg = new cMessage("clk");
	clkMsg->setSchedulingPriority(3);
	scheduleAt(simTime() + tClk_sec, clkMsg);

	allocTicksHist.setName("tick-used-in-alloc-hist");
	allocTicksHist.setRangeAutoUpper(0);
	allocTicksHist.setCellSize(1.0);

	assignedPerCycleHist.setName("assigned-reqs-per-cycle-hist");
	assignedPerCycleHist.setRange(0, rows * columns);
	assignedPerCycleHist.setCellSize(1.0);

	unAssignedPerCycleHist.setName("un-assigned-reqs-per-cycle-hist");
	unAssignedPerCycleHist.setRange(0, rows * columns);
	unAssignedPerCycleHist.setCellSize(1.0);

	assignedPerCycle.setName("assigned-reqs-per-cycle");
	unAssignedPerCycle.setName("un-assigned-reqs-per-cycle");
}

// consider a request from west router (on its east going port)
bool ParallelCentralSched::isPassing(DIRECTION dir, int r, int c,
		CentralRequestMsg *req) {
	int dc = req->getDstId() % columns;
	int dr = req->getDstId() / columns;
	if (dir == CORE) {
		if ((dc == c) && (dr == r))
			return (true);
	} else if (dir == NORTH) {
		if ((dc == c) && (dr > r))
			return (true);
	} else if (dir == SOUTH) {
		if ((dc == c) && (dr < r))
			return (true);
	} else if (dir == WEST) {
		if (dc > c)
			return (true);
	} else if (dir == EAST) {
		if (dc < c)
			return (true);
	}
	return (false);
}

// given the r and c of local port
int ParallelCentralSched::considerReqFromDirToDir(DIRECTION fromDir,
		DIRECTION toDir, int r, int c) {
	// some basic checks ... and setting of prev age and hop

	CentralRequestMsg **prevReqPtr = NULL;
	int *prevHopPtr = NULL;
	unsigned long long int *prevOldestUsagePtr = NULL;
	unsigned long long int *prevCommitedUsagePtr = NULL;

	if (toDir == WEST) {
		if (fromDir == WEST) {
			throw cRuntimeError("-E- going WEST can't consider WEST traffic");
		} else if (fromDir == NORTH) {
			throw cRuntimeError(
					"-E- going WEST can't consider NORTH traffic on XY routing");
		} else if (fromDir == SOUTH) {
			throw cRuntimeError(
					"-E- going WEST can't consider SOUTH traffic on XY routing");
		}
		prevReqPtr = &WLinksOldestReq[r][c];
		prevHopPtr = &WLinksOldestReqHops[r][c];
		prevOldestUsagePtr = &WLinksOldestUsage[r][c];
		prevCommitedUsagePtr = &WLinksUsage[r][c];
	} else if (toDir == EAST) {
		if (fromDir == EAST) {
			throw cRuntimeError("-E- going EAST can't consider WEST traffic");
		} else if (fromDir == NORTH) {
			throw cRuntimeError(
					"-E- going EAST can't consider NORTH traffic on XY routing");
		} else if (fromDir == SOUTH) {
			throw cRuntimeError(
					"-E- going EAST can't consider SOUTH traffic on XY routing");
		}
		prevReqPtr = &ELinksOldestReq[r][c];
		prevHopPtr = &ELinksOldestReqHops[r][c];
		prevOldestUsagePtr = &ELinksOldestUsage[r][c];
		prevCommitedUsagePtr = &ELinksUsage[r][c];
	} else if (toDir == NORTH) {
		if (fromDir == NORTH) {
			throw cRuntimeError(
					"-E- going NORTH can't consider NORTH traffic on XY routing");
		}
		prevReqPtr = &NLinksOldestReq[r][c];
		prevHopPtr = &NLinksOldestReqHops[r][c];
		prevOldestUsagePtr = &NLinksOldestUsage[r][c];
		prevCommitedUsagePtr = &NLinksUsage[r][c];
	} else if (toDir == SOUTH) {
		if (fromDir == SOUTH) {
			throw cRuntimeError(
					"-E- going SOUTH can't consider SOUTH traffic on XY routing");
		}
		prevReqPtr = &SLinksOldestReq[r][c];
		prevHopPtr = &SLinksOldestReqHops[r][c];
		prevOldestUsagePtr = &SLinksOldestUsage[r][c];
		prevCommitedUsagePtr = &SLinksUsage[r][c];
	} else if (toDir == CORE) {
		if (fromDir == CORE) {
			throw cRuntimeError(
					"-E- going CORE can't consider CORE traffic on XY routing");
		}
		prevReqPtr = &CLinksOldestReq[r][c];
		prevHopPtr = &CLinksOldestReqHops[r][c];
		prevOldestUsagePtr = &CLinksOldestUsage[r][c];
		prevCommitedUsagePtr = &CLinksUsage[r][c];
	}

	// based on the fromDir get the req:
	CentralRequestMsg *req = NULL;
	int hops = 0;
	unsigned long long int usage = 0;
	if (fromDir == CORE) {
		req = reqByRowCol[r][c];
	} else if (fromDir == WEST) {
		if (c < columns - 1) {
			req = ELinksOldestReq[r][c + 1];
			hops = ELinksOldestReqHops[r][c + 1];
			usage = ELinksOldestUsage[r][c + 1];
		}
	} else if (fromDir == EAST) {
		if (c > 0) {
			req = WLinksOldestReq[r][c - 1];
			hops = WLinksOldestReqHops[r][c - 1];
			usage = WLinksOldestUsage[r][c - 1];
		}
	} else if (fromDir == NORTH) {
		if (r < rows - 1) {
			req = SLinksOldestReq[r + 1][c];
			hops = SLinksOldestReqHops[r + 1][c];
			usage = SLinksOldestUsage[r + 1][c];
		}
	} else if (fromDir == SOUTH) {
		if (r > 0) {
			req = NLinksOldestReq[r - 1][c];
			hops = NLinksOldestReqHops[r - 1][c];
			usage = NLinksOldestUsage[r - 1][c];
		}
	}
	if (!req)
		return (0);

	// make sure it is passing through out port
	if (!isPassing(toDir, r, c, req))
		return (0);

	// compare the age
	if (!(*prevReqPtr) || (req->getAge() >= (*prevReqPtr)->getAge())) {
		char s[128];
		unsigned long long int newUsage = (usage << 1) | *prevCommitedUsagePtr;
		sprintf(s, "0x%08x,%08x", (unsigned int) (newUsage >> 32),
				(unsigned int) (newUsage & 0xffffffff));
		EV<< "-I- New oldest request:" << req->getName() << " srcId:" << req->getSrcId()
           << " at: " << r << "," << c << " " << getDirStr(toDir) << " usage: " << s << endl;
		if (*prevReqPtr) {
			sprintf(s, "0x%08x,%08x", (unsigned int)(*prevOldestUsagePtr >> 32),
					(unsigned int)(*prevOldestUsagePtr & 0xffffffff));
			EV << "-I- Replacing: " << getDirStr(toDir) << " srcId:" << (*prevReqPtr)->getSrcId()
			<< " with hops:" << (*prevHopPtr) << " usage:" << s << endl;
		}
		*prevReqPtr = req;
		*prevHopPtr = 1 + hops;
		*prevOldestUsagePtr = newUsage;
	}
	return(0);
}

// Given a port r,c and the direction. Take one step of consolidating
// all possible inputs to that port and select the oldest or farthest one
// Eventually filling the CLinksOldestReqHops and CLinksOldestReq
// NOTE: This is where XY is implemented!
int ParallelCentralSched::propagateReqTowardDst(DIRECTION dir, unsigned int r,
		unsigned int c) {
	// filter out the requests based on their destination
	if (dir == WEST) {
		// can have either core or east request
		WLinksOldestReq[r][c] = NULL;
		WLinksOldestReqHops[r][c] = 0;
		considerReqFromDirToDir(CORE, WEST, r, c);
		considerReqFromDirToDir(EAST, WEST, r, c);
	} else if (dir == EAST) {
		// can have either core or request from the west router east port
		ELinksOldestReq[r][c] = NULL;
		ELinksOldestReqHops[r][c] = 0;
		considerReqFromDirToDir(CORE, EAST, r, c);
		considerReqFromDirToDir(WEST, EAST, r, c);
	} else if (dir == NORTH) {
		// for a north port we may have traffic from center or west or east or south
		NLinksOldestReq[r][c] = NULL;
		NLinksOldestReqHops[r][c] = 0;
		considerReqFromDirToDir(CORE, NORTH, r, c);
		considerReqFromDirToDir(WEST, NORTH, r, c);
		considerReqFromDirToDir(EAST, NORTH, r, c);
		considerReqFromDirToDir(SOUTH, NORTH, r, c);
	} else if (dir == SOUTH) {
		// for a north port we may have traffic from center or west or east or north
		SLinksOldestReq[r][c] = NULL;
		SLinksOldestReqHops[r][c] = 0;
		considerReqFromDirToDir(CORE, SOUTH, r, c);
		considerReqFromDirToDir(WEST, SOUTH, r, c);
		considerReqFromDirToDir(EAST, SOUTH, r, c);
		considerReqFromDirToDir(NORTH, SOUTH, r, c);
	} else {
		// core ports get from all other ports
		CLinksOldestReq[r][c] = NULL;
		CLinksOldestReqHops[r][c] = 0;
		considerReqFromDirToDir(WEST, CORE, r, c);
		considerReqFromDirToDir(EAST, CORE, r, c);
		considerReqFromDirToDir(SOUTH, CORE, r, c);
		considerReqFromDirToDir(NORTH, CORE, r, c);
	}
	return (0);
}

// treat the given core outstanding request pushing it into first possible ackFifo tick
int ParallelCentralSched::pushCoreReqIntoFifo(unsigned int row,
		unsigned int col) {
	CentralRequestMsg *req = CLinksOldestReq[row][col];
	if (!req)
		return (1);

	int srcId = req->getSrcId();
	int dstId = req->getDstId();
	int flits = req->getFlits();
	int sr = srcId / columns;
	int sc = srcId % columns;
	int dr = dstId / columns;
	int dc = dstId % columns;

	// first we need to find an empty window on the core to push this packet in
	unsigned long long int usedTicks = CLinksOldestUsage[row][col];
	int hops = CLinksOldestReqHops[row][col] - 1;
	unsigned long long int mask = (((unsigned long long int) 1 << flits) - 1)
			<< hops;
	int t;
	bool found = false;
	for (t = hops; !found && (t < LONG_INT_BITS - flits); t++) {
		// see if all these bits are zero
		if (!(usedTicks & mask)) {
			found = true;
		} else {
			mask = mask << 1;
		}
	}
	if (!found) {
		EV<< "-I- at clk: " << clkCycle <<" could not find empty window of size: " << flits
		   << " for src: " << srcId << " dst: " << dstId << endl;
		return(1);
	}

	// mask must be ready here for usage
	char s[128], a[128];
	sprintf(s, "0x%08x,%08x", (unsigned int)(mask >> 32), (unsigned int)(mask & 0xffffffff));
	sprintf(a, "0x%08x,%08x", (unsigned int)(usedTicks >> 32), (unsigned int)(usedTicks & 0xffffffff));

	EV << "-I- at clk: " << clkCycle << " found empty window of size: " << s
	<< " used: " << a
	<< " for src: " << srcId << " dst: " << dstId
	<< " starting: " << t << " ticks away at destination which is "
	<< hops + 1 << " hops away from src"<< endl;

	// go back marking usage...
	if (t - hops < 1) {
		throw cRuntimeError("-E- BUG in tick calc. hops: %d t: %d", hops, t);
	}
	unsigned int tick = t - hops - 1;

	// get the mask back to the src
	mask = mask >> hops;
	int r = sr;
	int c = sc;
	int hop = 1;
	// HACK: Mesh XY routing assumed
	while ((c != dc) || (r != dr)) {
		sprintf(s, "0x%08x,%08x", (unsigned int)(mask >> 32), (unsigned int)(mask & 0xffffffff));

		EV << "-I- Marking reached: " << r << "," << c << " with mask: " << s << " hop:" << hop << endl;

		// This is where the XY order is enforced
		if (c < dc) {
			sprintf(a, "0x%08x,%08x", (unsigned int)(WLinksUsage[r][c] >> 32),
					(unsigned int)(WLinksUsage[r][c] & 0xffffffff));
			EV << "-I- going WEST from " << srcId << " to " << dstId << " on hop " << hop
			<< " at " << r << "," << c << " WLinksUsage=" << a << " and mask=" << s << endl;
			if (WLinksUsage[r][c] & mask) {
				throw cRuntimeError("-E- going W from %d to %d on hop %d at %d,%d "
						"WLinksUsage=0x%x and mask=0x%x", srcId, dstId, hop, r, c,
						WLinksUsage[r][c], mask);
			}
			WLinksUsage[r][c++] |= mask;
		} else if (c > dc) {
			sprintf(a, "0x%08x,%08x", (unsigned int)(ELinksUsage[r][c] >> 32),
					(unsigned int)(ELinksUsage[r][c] & 0xffffffff));
			EV << "-I- going EARTH from " << srcId << " to " << dstId << " on hop " << hop
			<< " at " << r << "," << c << " ELinksUsage=" << a << " and mask=" << s << endl;
			if (ELinksUsage[r][c] & mask) {
				throw cRuntimeError("-E- going E from %d to %d on hop %d at %d,%d "
						"ELinksUsage=0x%x and mask=0x%x", srcId, dstId, hop, r, c,
						ELinksUsage[r][c], mask);
			}
			ELinksUsage[r][c--] |= mask;
		} else if (r < dr) {
			sprintf(a, "0x%08x,%08x", (unsigned int)(NLinksUsage[r][c] >> 32),
					(unsigned int)(NLinksUsage[r][c] & 0xffffffff));
			EV << "-I- going NORTH from " << srcId << " to " << dstId << " on hop " << hop
			<< " at " << r << "," << c << " NLinksUsage=" << a << " and mask=" << s << endl;
			if (NLinksUsage[r][c] & mask) {
				throw cRuntimeError("-E- going N from %d to %d on hop %d at %d,%d "
						"NLinksUsage=0x%x and mask=0x%x for tick:%d", srcId, dstId, hop, r, c,
						NLinksUsage[r][c], mask, tick);
			}
			NLinksUsage[r++][c] |= mask;
		} else if (r > dr) {
			sprintf(a, "0x%08x,%08x", (unsigned int)(SLinksUsage[r][c] >> 32),
					(unsigned int)(SLinksUsage[r][c] & 0xffffffff));
			EV << "-I- going SOUTH from " << srcId << " to " << dstId << " on hop " << hop
			<< " at " << r << "," << c << " SLinksUsage=" << a << " and mask=" << s << endl;
			if (SLinksUsage[r][c] & mask) {
				throw cRuntimeError("-E- going S from %d to %d on hop %d at %d,%d "
						"SLinksUsage=0x%x and mask=0x%x", srcId, dstId, hop, r, c,
						SLinksUsage[r][c], mask);
			}
			SLinksUsage[r--][c] |= mask;
		} else {
			throw cRuntimeError("-E- BUG in loop on steps");
		}

		mask = mask << 1;
		hop++;
	}
	sprintf(a, "0x%08x,%08x", (unsigned int)(CLinksUsage[r][c] >> 32),
			(unsigned int)(CLinksUsage[r][c] & 0xffffffff));
	sprintf(s, "0x%08x,%08x", (unsigned int)(mask >> 32), (unsigned int)(mask & 0xffffffff));
	EV << "-I- going CORE from " << srcId << " to " << dstId << " on hop " << hop
	<< " at " << r << "," << c << " CLinksUsage=" << a << " and mask=" << s << endl;

	if (CLinksUsage[r][c] & mask) {
		throw cRuntimeError("-E- going C from %d to %d on hop %d at %d,%d "
				"CLinksUsage=0x%x and mask=0x%x", srcId, dstId, hop, r, c,
				CLinksUsage[r][c], mask);
	}
	CLinksUsage[r][c] |= mask;

	// place on the ackFifo
	if (ackFifo.size() <= tick) {
		for (unsigned int t = ackFifo.size(); t <= tick; t++) {
			ackFifo.push_back(new std::list<CentralRequestMsg *>);
		}
	}
	EV << "-I- " << getFullPath() << " clk: " << clkCycle << " scheduling into tick:"
	<< tick << " src:" << srcId << " dst:" << dstId << endl;
	ackFifo[tick]->push_front(req);
	reqByRowCol[sr][sc] = NULL;
	allocTicksHist.collect(tick);
	assignedReqs++;
	return(0);
}

// perform the actual work of deciding which requests are going to be serviced when
void ParallelCentralSched::handleClkMsg() {
	clkCycle++;

	// shift the clock tick on all the links
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < columns; c++) {
			WLinksUsage[r][c] = WLinksUsage[r][c] >> 1;
			ELinksUsage[r][c] = ELinksUsage[r][c] >> 1;
			NLinksUsage[r][c] = NLinksUsage[r][c] >> 1;
			SLinksUsage[r][c] = SLinksUsage[r][c] >> 1;
			CLinksUsage[r][c] = CLinksUsage[r][c] >> 1;
		}

	if ((clkCycle % schedClks) == 0) {
		assignedReqs = 0;
		pendingReqs = 0;
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				if (reqByRowCol[r][c])
					pendingReqs++;

		// in order to visit every port only once - and take the right decision
		// we rely on network topology and sort in topological order:
		// For West Ports - go from col 0 and up
		// For East Ports - go from last column down
		// For North Ports - once all East and West done - go from row 0 up
		// For South Ports - once all East and West done - go from last row down
		// So the following procedures are being used...
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++)
				propagateReqTowardDst(WEST, r, c);
			for (int c = columns - 1; c >= 0; c--)
				propagateReqTowardDst(EAST, r, c);
		}
		for (int c = 0; c < columns; c++) {
			for (int r = 0; r < rows; r++)
				propagateReqTowardDst(NORTH, r, c);
			for (int r = rows - 1; r >= 0; r--)
				propagateReqTowardDst(SOUTH, r, c);
		}
		for (int c = 0; c < columns; c++)
			for (int r = 0; r < rows; r++)
				propagateReqTowardDst(CORE, r, c);

		// calculate a first window (if any) for the request at each dst
		// if found move the req into the ackFifo
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				if (CLinksOldestReq[r][c])
					pushCoreReqIntoFifo(r, c);
		assignedPerCycle.collect(assignedReqs);
		assignedPerCycleHist.collect(assignedReqs);
		unAssignedPerCycle.collect(pendingReqs - assignedReqs);
		unAssignedPerCycleHist.collect(pendingReqs - assignedReqs);
	} else {
		// we only schedule every schedClks clocks
		EV<< "-I- " << getFullPath() << " scheduling only once every " << schedClks
		<< " clks" << " cycle: " << clkCycle << endl;
	}

	// send the list of acks for this clock and pop that FIFO
	std::list<CentralRequestMsg*>::iterator lI;
	for (lI = ackFifo[0]->begin(); lI != ackFifo[0]->end(); lI++) {
		CentralRequestMsg *creq = *lI;
		int ip = creq->getArrivalGate()->getIndex();
		// for now we simply return it
		EV << "-I- " << getFullPath() << " clk: " << clkCycle << " approving queued flow from src:"
		<< creq->getSrcId() << " dst:" << creq->getDstId() << " pkt:"
		<< creq->getPktId() << " flits:" << creq->getFlits() << endl;
		send(creq, "center$o", ip);
	}

	// pop the fifo and allocate a new last tick list
	delete ackFifo[0];
	unsigned int t;
	for (t = 1; t < ackFifo.size(); t++)
	ackFifo[t-1] = ackFifo[t];
	ackFifo[t-1] = new std::list<CentralRequestMsg *>;

	// shift the age of all requsts
	for (int r = 0; r < rows; r++)
	for (int c = 0; c < columns; c++)
	if (reqByRowCol[r][c])
	reqByRowCol[r][c]->setAge(reqByRowCol[r][c]->getAge()+1);
	// next clock
	scheduleAt(simTime()+tClk_sec, clkMsg);
}

void ParallelCentralSched::handleReq(CentralRequestMsg *msg) {
	if (ackEverything) {
		int ip = msg->getArrivalGate()->getIndex();
		// for now we simply return it
		EV<< "-I- " << getFullPath() << " clk: " << clkCycle << " approving flow from src:" << msg->getSrcId()
		<< " dst:" << msg->getDstId() << " pkt:" << msg->getPktId()
		<< " flits:" << msg->getFlits() << endl;
		send(msg, "center$o", ip);
		return;
	}

	int srcId = msg->getSrcId();
	int sr = srcId / columns;
	int sc = srcId % columns;
	if (reqByRowCol[sr][sc]) {
		throw cRuntimeError("-E- request from src %d at %d,%d but previous request exists",
				srcId, sr, sc);
	}
	reqByRowCol[sr][sc] = msg;
}

void ParallelCentralSched::handleMessage(cMessage *msg) {
	if (msg == clkMsg) {
		handleClkMsg();
	} else {
		handleReq((CentralRequestMsg *) msg);
	}
}

void ParallelCentralSched::finish() {
	allocTicksHist.record();
	assignedPerCycle.record();
	unAssignedPerCycle.record();
	assignedPerCycleHist.record();
	unAssignedPerCycleHist.record();
}

ParallelCentralSched::~ParallelCentralSched() {
	// clear the requests
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < columns; c++)
			if (reqByRowCol[r][c])
				delete reqByRowCol[r][c];

	// clear the outstanding acked reqs fifo
	for (unsigned int t = 0; t < ackFifo.size(); t++) {
		if (ackFifo[t]) {
			std::list<CentralRequestMsg*>::iterator lI;
			for (lI = ackFifo[t]->begin(); lI != ackFifo[t]->end(); lI++) {
				delete *lI;
			}
			delete ackFifo[t];
		}
	}
	cancelAndDelete(clkMsg);
}
