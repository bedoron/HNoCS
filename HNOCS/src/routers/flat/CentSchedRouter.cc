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
#include "App_m.h"
#include <iostream>
using std::cerr;

Define_Module(CentSchedRouter)
;

void CentSchedRouter::initialize() {
    numPorts = gateSize("in");
	numCols = par("columns");
	id = par("id");
	rx = id % numCols;
	ry = id / numCols;
	coreType = par("coreType");
	routerType = par("routerType");
	double data_rate = par("dataRate");

    int numVCs = par("numVCs");
    int flitSize_B = par("flitSize");
    //int arbitration_type = par("arbitration_type");

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

	m_ports.resize(numPorts);


	for (int ip = 0; ip < numPorts; ip++) {
	    sendCredits(ip, 2);

	    cGate *g = gate("out$o", ip);
	    m_ports[ip].connected = (g->getPathEndGate()->getType()!= cGate::INPUT);

	    if(m_ports[ip].connected) {
	        m_ports[ip].m_vcs.resize(numVCs);
	        m_ports[ip].gate = g;
	    } else {
	        m_ports[ip].gate = NULL;
	    }

	}

	double data_rate = chan->getDatarate();
	tClk_s = (8 * flitSize_B) / data_rate;

    popMsg = new cMessage("pop");
    popMsg->setKind(NOC_POP_MSG);
    popMsg->setSchedulingPriority(5);
    scheduleAt(simTime()+tClk_s, popMsg);
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

void CentSchedRouter::handlePop(NoCPopMsg* msg) {
    if (!popMsg->isScheduled()) {
        scheduleAt(simTime() + tClk_s, popMsg);
        // EV<< "-I" << getFullPath() << "popMsg is scheduled to:" <<simTime() + tClk_s << endl;
    }
    deliver();
}

bool CentSchedRouter::isHead(NoCFlitMsg* msg) {
    NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg->getType();
    return type==NOC_START_FLIT;
}

bool CentSchedRouter::isTail(NoCFlitMsg* msg) {
    NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg->getType();
    return type==NOC_END_FLIT;
}

bool CentSchedRouter::isHead(NoCFlitMsg& msg) {
    NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg.getType();
    return type==NOC_START_FLIT;
}

void CentSchedRouter::deliver() {
    // Iterate all out-ports and try to deliver messages
}

void CentSchedRouter::handleCredit(NoCCreditMsg* msg) {
}

bool CentSchedRouter::isTail(NoCFlitMsg& msg) {
    NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg.getType();
    return type==NOC_END_FLIT;
}

inPortFlitInfo* CentSchedRouter::getFlitInfo(NoCFlitMsg* msg) {
    cObject *obj = msg->getControlInfo();
    if (obj == NULL) {
        throw cRuntimeError("-E- BUG - No Control Info for FLIT: %s",
                msg->getFullName());
    }

    inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
    return info;
}

void CentSchedRouter::handleMessage(cMessage *msg) {

    NOC_MSGS type = (NOC_MSGS) msg->getKind();
    NoCFlitMsg *flit = dynamic_cast<NoCFlitMsg*>(msg);

    switch(type) {
    case NOC_FLIT_MSG: handleFlitMsg(flit); break;
    case NOC_POP_MSG: handlePop((NoCPopMsg*)flit); break;
    case NOC_CREDIT_MSG: handleCredit((NoCCreditMsg*)flit); break;
    default:
        throw cRuntimeError("Unsupported message arrival");
    }
}

// VC Accepting Flit
bool CentSchedRouter::vc_t::accept(NoCFlitMsg& flit) {
    AppFlitMsg &appFlit = *(dynamic_cast<AppFlitMsg*>(&flit));
    int msgId = appFlit.getMsgId();
    bool canAccept = false;

    // Check if packet fits
    if(!empty() && (m_activeMessage==msgId)) {
        canAccept = true;
    } else if(empty()) {
        if(isHead(flit)) { // Only heads can "open" empty VCs
            canAccept = true;
        }
    }
    // Check if packet queueable and queue it
    if(canAccept) {
        if(m_credits<=0) { // not enough credits
            return false;
        } else {
            if(empty()) {
                m_activeMessage = msgId;
                m_activePacket = flit.getPktId();
            }
            m_flits.push(&flit);
            m_credits--;
        }
    }
    return canAccept;
}

// Check if current VC is empty
bool CentSchedRouter::vc_t::empty() {
    return m_flits.empty();
}

NoCFlitMsg& CentSchedRouter::vc_t::release() {
    if(empty()) {
        throw cRuntimeError("Trying to release a flit from an empty VC");
    }

    NoCFlitMsg *msg = m_flits.front();
    m_flits.pop();
    ++m_credits;

    NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg->getKind();
    if(type == NOC_END_FLIT) {
        // Release this Virtual channel
        m_activeMessage = -1;
        m_activePacket = -1;
    }

    return *msg;
}

struct CentSchedRouter::vc_t& CentSchedRouter::port_t::getVC(NoCFlitMsg* msg) {
    msg->getControlInfo();
    if(isHead(msg)) {
        // look for a free VC

        for(uint i=0; i < m_vcs.size(); ++i) {
            if(m_vcs[i].m_flits.empty())
                return m_vcs[i];
        }
        cerr << "Head flit arrived but no free VC available";
        throw new cRuntimeError("Head flit arrived but no free VC available");
    } else {
        // Find the VC this flit belongs to
        inPortFlitInfo *info = getFlitInfo(msg);
        int flitVc = info->inVC;
        try {
            vc_t &vc = m_vcs.at(flitVc);
            return vc;
        } catch(std::out_of_range ex) {
            throw cRuntimeError("Flit arrived with invalid VC");
        }
    }
}
