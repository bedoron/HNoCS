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

#include "CentSchedRouter.h"

Define_Module(CentSchedRouter)
;

void CentSchedRouter::initialize() {
	numPorts = par("numPorts");
	numCols = par("columns");
	id = par("id");
	rx = id % numCols;
	ry = id / numCols;
	coreType = par("coreType");
	routerType = par("routerType");

	// calculate the routing information
	analyzeMeshTopology();
	EV<< "-I- " << getFullPath() << " Found N/W/S/E/C ports:" << northPort
	<< "/" << westPort << "/" << southPort << "/"
	<< eastPort << "/" << corePort << endl;
	WATCH(northPort);
	WATCH(westPort);
	WATCH(eastPort);
	WATCH(southPort);
	WATCH(corePort);

	for (int ip = 0; ip < numPorts; ip++)
	sendCredits(ip, 2);
}

	// send back a credit on the in port
void CentSchedRouter::sendCredits(int ip, int numFlits) {
	if (gate("in$o", ip)->getPathEndGate()->getType() != cGate::INPUT) {
		return;
	}
	EV<< "-I- " << getFullPath() << " sending " << numFlits
	<< " credits on VC=" << 0 << endl;

	char credName[64];
	sprintf(credName, "cred-%d-%d", 0, numFlits);
	NoCCreditMsg *crd = new NoCCreditMsg(credName);
	crd->setKind(NOC_CREDIT_MSG);
	crd->setVC(0);
	crd->setFlits(numFlits);
	send(crd, "in$o", ip);
}

	// Analyze the topology of this router and obtain the port numbers
	// connected to the 4 directions
int CentSchedRouter::analyzeMeshTopology() {
	// if not found the port numbers will be -1
	northPort = -1;
	westPort = -1;
	southPort = -1;
	eastPort = -1;
	corePort = -1;

	// try to go through all ports and see what is out there
	cGate *og = gate("out$o", 0);
	for (int op = 0; op < og->getVectorSize(); op++) {
		cGate *g = gate("out$o", op);
		// must be connected on the other side
		if (!g->isConnectedOutside())
			continue;

		// get the module on the other side of the gate
		cGate *rg = g->getNextGate();
		if (!rg)
			continue;

		cModule *neighbour = rg->getOwnerModule();
		if (neighbour == this)
			continue;

		// it may only allowed to be a core or another router
		if (neighbour->getModuleType() == cModuleType::get(routerType)) {
			// it is a router
			int remId = neighbour->par("id");
			int x = remId % numCols;
			int y = remId / numCols;
			if ((rx == x) && (ry == y)) {
				throw cRuntimeError(
						"Ports: %s and %s share the same x:%d and y:%d",
						getFullPath().c_str(),
						neighbour->getFullPath().c_str(), x, y);
			} else if ((rx == x) && (ry == y + 1)) {
				// remPort is south port
				if (southPort != -1) {
					throw cRuntimeError(
							"Already found a south port: %d for ports: %s."
								" %s is miss-configured", southPort,
							getFullPath().c_str(),
							neighbour->getFullPath().c_str());
				} else {
					EV<< "-I- " << getFullPath()
					<< " connected through out[" << op
					<< "] to South port: " << neighbour->getFullPath() << endl;
					southPort = op;
				}
			} else if ((rx == x) && (ry == y - 1)) {
				// remPort is north port
				if (northPort != -1) {
					throw cRuntimeError("Already found a north port: %d for ports: %s."
							" %s is miss-configured",
							northPort, getFullPath().c_str(),
							neighbour->getFullPath().c_str());
				} else {
					EV << "-I- " << getFullPath()
					<< " connected through out[" << op
					<< "] to North port: " << neighbour->getFullPath() << endl;
					northPort = op;
				}
			} else if ((rx == x + 1) && (ry == y)) {
				// remPort is west port
				if (westPort != -1) {
					throw cRuntimeError("Already found a west port: %d for ports: %s."
							" %s is miss-configured",
							westPort, getFullPath().c_str(),
							neighbour->getFullPath().c_str());
				} else {
					EV << "-I- " << getFullPath()
					<< " connected through out[" << op
					<< "] to West port: " << neighbour->getFullPath() << endl;
					westPort = op;
				}
			} else if ((rx == x - 1) && (ry == y)) {
				// remPort is east port
				if (eastPort != -1) {
					throw cRuntimeError("Already found an east port: %d for ports: %s."
							" %s is miss-configured",
							eastPort, getFullPath().c_str(),
							neighbour->getFullPath().c_str());
				} else {
					EV << "-I- " << getFullPath()
					<< " connected through out[" << op
					<< "] to East port: " << neighbour->getFullPath() << endl;
					eastPort = op;
				}
			} else {
				throw cRuntimeError("Found a non Mesh connection between %s (%d,%d) and %s (%d,%d)", " %s is miss-configured",
						getFullPath().c_str(), rx,ry,
						neighbour->getFullPath().c_str(),x,y);
			}
		} else if (neighbour->getModuleType() == cModuleType::get(coreType)) {
			// it is a core
			if (corePort >= 0) {
				throw cRuntimeError("Already found core on port: %d but now on %d",
						corePort, op);
			}
			corePort = op;
		} else {
			throw cRuntimeError("Port: %d is connected to module %s of unknown type %s (not %s or %s)",
					op, neighbour->getFullName(), neighbour->getClassName(), routerType, coreType);
		}
	}

	if (corePort < 0) {
		EV << "-W- " << getFullPath()
		<< " could not find corePort (of coreType:" << coreType << ")" << endl;
	}
	return(0);
}

void CentSchedRouter::handleFlitMsg(NoCFlitMsg *msg) {
	int dx, dy;
	dy = msg->getDstId() / numCols;
	dx = msg->getDstId() % numCols;
	int swOutPortIdx;
	if ((dx == rx) && (dy == ry)) {
		swOutPortIdx = corePort;
	} else if (dx > rx) {
		swOutPortIdx = eastPort;
	} else if (dx < rx) {
		swOutPortIdx = westPort;
	} else if (dy > ry) {
		swOutPortIdx = northPort;
	} else {
		swOutPortIdx = southPort;
	}
	if (swOutPortIdx < 0) {
		throw cRuntimeError("Routing dead end at %s (%d,%d) "
			"for destination %d (%d,%d)", getFullPath().c_str(), rx, ry,
				msg->getDstId(), dx, dy);
	}

	if (msg->getFirstNet()) {
		msg->setFirstNetTime(simTime());
		msg->setFirstNet(false);
	}

	// send back credits
	sendCredits(msg->getArrivalGate()->getIndex(), 1);

	// check if the out port is busy and report
	if (gate("out$o", swOutPortIdx)->getTransmissionChannel()->isBusy()) {
		simtime_t
				txfinishTime =
						gate("out$o", swOutPortIdx)->getTransmissionChannel()->getTransmissionFinishTime();
		EV<< "-E- " << getFullPath() << " port " << swOutPortIdx << " busy until:"
		<< txfinishTime << " now: " << simTime() << endl;
	}

	// send the packet out - will cause exception if another packet already there
	send(msg, "out$o", swOutPortIdx);
}

void CentSchedRouter::handleReq(NoCReqMsg* msg) {
}

void CentSchedRouter::handleGnt(NoCGntMsg* msg) {
}

void CentSchedRouter::handlePop(NoCPopMsg* msg) {
}

void CentSchedRouter::handleMessage(cMessage *msg) {
    /*
    NOC_MSGS type = msg->getKind();
    NoCFlitMsg flit = *(dynamic_cast<NoCFlitMsg*>(msg));

    switch(type) {
    case NOC_FLIT_MSG: handleFlitMsg(flit); break;
    case NOC_REQ_MSG: handleReq(flit); break;
    case NOC_GNT_MSG: handleGnt(flit); break;
    case NOC_POP_MSG: handlePop(flit); break;
    default:
        throw cRuntimeError("Unsupported message arrival");
    }
    */
}
/*
bool CentSchedRouter::vc_t::accept(NoCFlitMsg& flit) {
}

bool CentSchedRouter::vc_t::empty() {
}

NoCFlitMsg& CentSchedRouter::vc_t::release() {
}

vc_t& CentSchedRouter::port_t::getVC(NoCFlitMsg* msg) {
}
*/
