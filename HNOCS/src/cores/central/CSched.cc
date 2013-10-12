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

#include "CSched.h"

Define_Module(CSched)
;
using std::hex;
using std::showbase;
using std::ios_base;

void CSched::initialize() {
	rows = par("rows");
	columns = par("columns");
	ackEverything = par("ackEverything");
	tClk_sec = par("tClk");
	usePathReservation = par("usePathReservation");
	maxParallelReqs = par("maxParallelReqs");

	// initialize link usage matrixes
	// NOTE: we add one extra row or column as it makes indexing simpler
	WLinksUsage.resize(rows);
	ELinksUsage.resize(rows);
	NLinksUsage.resize(rows);
	SLinksUsage.resize(rows);
	CLinksUsage.resize(rows);
	WLinksReserve.resize(rows);
	ELinksReserve.resize(rows);
	NLinksReserve.resize(rows);
	SLinksReserve.resize(rows);
	CLinksReserve.resize(rows);
	for (int r = 0; r < rows; r++) {
		WLinksUsage[r].resize(columns, 0);
		ELinksUsage[r].resize(columns, 0);
		NLinksUsage[r].resize(columns, 0);
		SLinksUsage[r].resize(columns, 0);
		CLinksUsage[r].resize(columns, 0);
		WLinksReserve[r].resize(columns, 0);
		ELinksReserve[r].resize(columns, 0);
		NLinksReserve[r].resize(columns, 0);
		SLinksReserve[r].resize(columns, 0);
		CLinksReserve[r].resize(columns, 0);

	}

	// we must start with at least one list of ackFifo
	std::list<CentralRequestMsg *> * reqList = new std::list<
			CentralRequestMsg *>;
	ackFifo.push_back(reqList);

	clkCycle = 0;

	clkMsg = new cMessage("clk");
	clkMsg->setSchedulingPriority(5);
	scheduleAt(simTime() + tClk_sec, clkMsg);

	allocTicksHist.setName("tick-used-in-alloc");
	allocTicksHist.setRangeAutoUpper(0);
	allocTicksHist.setCellSize(1.0);
}

// scan through the path from src to dst and obtain the earliest time that
// a path is free for that entire packet. Return that time or -1 if could not
// find any
// HACK: assume a mesh and XY routing
int CSched::getEarliestTickForSrcDst(CentralRequestMsg *req) {
	int srcId = req->getSrcId();
	int dstId = req->getDstId();
	int flits = req->getFlits();
	int sr = srcId / columns;
	int sc = srcId % columns;
	int dr = dstId / columns;
	int dc = dstId % columns;
	int hop = 0;
	// accumulate the used ticks on the path
	unsigned long long int accumUsedTicks = 0;
	// HACK: assume XY Routing
	int r = sr;
	int c = sc;
	while ((c != dc) || (r != dr)) {
		unsigned long long int stepUsedTicks;
		unsigned long long int stepReservedTicks;
		// This is where the XY order is enforced
		if (c < dc) {
			stepReservedTicks = WLinksReserve[r][c];
			stepUsedTicks = WLinksUsage[r][c++];
		} else if (c > dc) {
			stepReservedTicks = ELinksReserve[r][c];
			stepUsedTicks = ELinksUsage[r][c--];
		} else if (r < dr) {
			stepReservedTicks = NLinksReserve[r][c];
			stepUsedTicks = NLinksUsage[r++][c];
		} else if (r > dr) {
			stepReservedTicks = SLinksReserve[r][c];
			stepUsedTicks = SLinksUsage[r--][c];
		} else {
			throw cRuntimeError("-E- BUG in loop on steps");
		}

		// shift since a tick on step 1 marks usage of in 0...
		accumUsedTicks |= (stepUsedTicks >> hop);
		if (usePathReservation)
			accumUsedTicks |= (stepReservedTicks >> hop);
		hop++;
	}
	// the final link to the core
	accumUsedTicks |= (CLinksUsage[r][c] >> hop);
	if (usePathReservation)
		accumUsedTicks |= (CLinksReserve[r][c] >> hop);

	// look for req->flits number of continuous zeros in the accumUsedTicks
	// since we calculate at the source and then mark all the way to the sink
	// we can not allocate if at the sink it goes over the width
#define LONG_INT_BITS 64
	unsigned long long int mask = (1 << flits) - 1;
	int t;
	for (t = 0; t < LONG_INT_BITS - flits - hop; t++) {
		// see if all these bits are zero
		if (!(accumUsedTicks & mask)) {
			char s[128], a[128], u[128];
			sprintf(s, "0x%x,%x", (unsigned int) (mask >> 32),
					(unsigned int) (mask & 0xffffffff));
			sprintf(u, "0x%x,%x", (unsigned int) (CLinksUsage[r][c] >> 32),
					(unsigned int) (CLinksUsage[r][c] & 0xffffffff));
			sprintf(a, "0x%x,%x", (unsigned int) (accumUsedTicks >> 32),
					(unsigned int) (accumUsedTicks & 0xffffffff));

			EV<< "-I- at clk: " << clkCycle << " found empty window of size: " <<s
			   << " accum: " << a << " lastStep: " << u
			   << " for src: " << srcId << " dst: " << dstId
			   << " starting: " << t << " ticks away" << endl;
			return(t);
		}
		mask = mask << 1;
	}

	if (!usePathReservation) return(-1);

	// if could not find a window big enough: make sure next request can not use the tail of the path
	// mask holds now the last tried mask. So now try up to the last bit
	for (; t < LONG_INT_BITS; t++) {
		if (! (accumUsedTicks & mask)) {
			// t is the earliest possible assignment for this t
			// mark the "reserved" flags on the following links
			unsigned long long int reserveMask = mask;
			int r = sr;
			int c = sc;
			while ((c != dc) || (r != dr)) {
				// This is where the XY order is enforced
				if (c < dc) {
					WLinksReserve[r][c++] |= reserveMask;
				} else if (c > dc) {
					ELinksReserve[r][c--] |= reserveMask;
				} else if (r < dr) {
					NLinksReserve[r++][c] |= reserveMask;
				} else if (r > dr) {
					SLinksReserve[r--][c] |= reserveMask;
				} else {
					throw cRuntimeError("-E- BUG in loop on steps");
				}

				// shift since a tick on step 1 marks usage of in 0...
				reserveMask |= (reserveMask << 1);
			}
			// the final link to the core
			CLinksReserve[r][c] = reserveMask;

			return(-1);
		}
		mask = mask << 1;
	}
	return(-1);
}

// mark on the path the provided allocation and place the req on the ackFifo
void CSched::allocateSchedForReq(unsigned int tick, CentralRequestMsg *creq) {
	int srcId = creq->getSrcId();
	int dstId = creq->getDstId();
	int flits = creq->getFlits();
	int sr = srcId / columns;
	int sc = srcId % columns;
	int dr = dstId / columns;
	int dc = dstId % columns;
	int hop = 0;
	unsigned long long int mask = ((unsigned long long int) (1 << flits) - 1)
			<< tick;
	int r = sr;
	int c = sc;
	// HACK: Mesh XY routing assumed
	while ((c != dc) || (r != dr)) {
		// This is where the XY order is enforced
		if (c < dc) {
			if (WLinksUsage[r][c] & mask) {
				throw cRuntimeError(
						"-E- going W from %d to %d on hop %d at %d,%d "
							"WLinksUsage=0x%x and mask=0x%x", srcId, dstId,
						hop, r, c, WLinksUsage[r][c], mask);
			}
			WLinksUsage[r][c++] |= mask;
		} else if (c > dc) {
			if (ELinksUsage[r][c] & mask) {
				throw cRuntimeError(
						"-E- going E from %d to %d on hop %d at %d,%d "
							"ELinksUsage=0x%x and mask=0x%x", srcId, dstId,
						hop, r, c, ELinksUsage[r][c], mask);
			}
			ELinksUsage[r][c--] |= mask;
		} else if (r < dr) {
			if (NLinksUsage[r][c] & mask) {
				throw cRuntimeError(
						"-E- going N from %d to %d on hop %d at %d,%d "
							"NLinksUsage=0x%x and mask=0x%x for tick:%d",
						srcId, dstId, hop, r, c, NLinksUsage[r][c], mask, tick);
			}
			NLinksUsage[r++][c] |= mask;
		} else if (r > dr) {
			if (SLinksUsage[r][c] & mask) {
				throw cRuntimeError(
						"-E- going S from %d to %d on hop %d at %d,%d "
							"SLinksUsage=0x%x and mask=0x%x", srcId, dstId,
						hop, r, c, SLinksUsage[r][c], mask);
			}
			SLinksUsage[r--][c] |= mask;
		} else {
			throw cRuntimeError("-E- BUG in loop on steps");
		}

		mask = mask << 1;
		hop++;
	}
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
	EV<< "-I- " << getFullPath() << " clk: " << clkCycle <<" scheduling into tick:"
			     << tick << " src:" << srcId << " dst:" << dstId << endl;
	ackFifo[tick]->push_front(creq);
	allocTicksHist.collect(tick);
}

void CSched::handleReq(CentralRequestMsg *msg) {
	if (ackEverything) {
		int ip = msg->getArrivalGate()->getIndex();
		// for now we simply return it
		EV<< "-I- " << getFullPath() << " clk: " << clkCycle << " approving flow from src:" << msg->getSrcId()
		<< " dst:" << msg->getDstId() << " pkt:" << msg->getPktId()
		<< " flits:" << msg->getFlits() << endl;
		send(msg, "center$o", ip);
		return;
	}

	// push the request into the request FIFO
	reqFifo.push_back(msg);
}

// perform the actual work of deciding which requests are going to be
void CSched::handleClkMsg() {
	clkCycle++;

	// shift the clock tick on all the links
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < columns; c++) {
			WLinksUsage[r][c] = WLinksUsage[r][c] >> 1;
			ELinksUsage[r][c] = ELinksUsage[r][c] >> 1;
			NLinksUsage[r][c] = NLinksUsage[r][c] >> 1;
			SLinksUsage[r][c] = SLinksUsage[r][c] >> 1;
			CLinksUsage[r][c] = CLinksUsage[r][c] >> 1;
			WLinksReserve[r][c] = 0;
			ELinksReserve[r][c] = 0;
			NLinksReserve[r][c] = 0;
			SLinksReserve[r][c] = 0;
			CLinksReserve[r][c] = 0;
		}

	// go over all the request FIFO entries (until there was no success in routing?)
	// in this ideal view we write back the allocation for every request so we can
	// safely do the next one
	std::list<CentralRequestMsg *>::iterator rI;
	int reqIdx = 0;
	for (rI = reqFifo.begin(); rI != reqFifo.end();) {
		CentralRequestMsg *creq = *rI;
		int availTick = getEarliestTickForSrcDst(creq);
		if (availTick < 0) {
			EV<< "-W- " << getFullPath() << " failed to find any time for path from "
			<< creq->getSrcId() << " to:" << creq->getDstId() << endl;
			rI++;
		} else {
			// set the usage flags and Q or send the req back
			allocateSchedForReq(availTick, creq);
			// remove the request from the reqFifo
			rI = reqFifo.erase(rI);
		}
		if (maxParallelReqs && (maxParallelReqs <= ++reqIdx))
		break;
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

	// next clock
	scheduleAt(simTime()+tClk_sec, clkMsg);
}

void CSched::handleMessage(cMessage *msg) {
	if (msg == clkMsg) {
		handleClkMsg();
	} else {
		handleReq((CentralRequestMsg *) msg);
	}
}

void CSched::finish() {
	allocTicksHist.record();
}

CSched::~CSched() {
	std::list<CentralRequestMsg *>::iterator rI;
	for (rI = reqFifo.begin(); rI != reqFifo.end(); rI++) {
		if (*rI)
			delete *rI;
	}
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
