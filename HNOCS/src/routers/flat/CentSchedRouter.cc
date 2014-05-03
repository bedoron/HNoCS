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
	flitsPerVC = par("flitsPerVC");
	dataRate = par("dataRate");
    int numVCs = par("numVCs");
    int flitSize_B = par("flitSize");

    unsigned int pipelineDepth = par("pipelineDepth"); //40; // TODO: get this as a parameter

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


	for (int ip = 0; ip < numPorts; ip++) { // Initialize Ports
        cGate *g = gate("in$o", ip);
        m_ports[ip].connected = g->isConnectedOutside();

	    m_ports[ip].m_transmittingVC = -1; // no VC is transmitting
	    if(m_ports[ip].connected) {
	        m_ports[ip].m_vcs.resize(numVCs);
	        m_ports[ip].gate = g;
	        for(int vc = 0; vc < numVCs; ++vc) { //  Initialize current port's VCs
	            m_ports[ip].m_vcs[vc].m_linkCredits = flitsPerVC;
	            m_ports[ip].m_vcs[vc].m_credits = 0; // Hopefully, this will happen before receiving any credits...
	            m_ports[ip].m_vcs[vc].m_id = vc;
	            m_ports[ip].m_vcs[vc].m_pipelineDepth = pipelineDepth;
	            m_ports[ip].m_vcs[vc].m_pipelineStage = 0;
	            sendCredits(ip, vc, flitsPerVC);
	            m_ports[ip].m_vcs[vc].m_portId = ip;
	            m_ports[ip].m_vcs[vc].m_routerId = getIndex();
	        }
	    } else {
	        m_ports[ip].gate = NULL;
	    }

	}

	tClk_s = (8 * flitSize_B) / dataRate;

    popMsg = new cMessage("pop");
    popMsg->setKind(NOC_POP_MSG);
    popMsg->setSchedulingPriority(5);
}

	// send back a credit on the in port
void CentSchedRouter::sendCredits(int ip, int otherVC, int numFlits) {
	EV<< "-I- " << getFullPath() << " sending " << numFlits
	<< " credits on VC=" << 0 << endl;

	char credName[64];
	sprintf(credName, "cred-%d-%d", 0, numFlits);
	NoCCreditMsg *crd = new NoCCreditMsg(credName);
	crd->setKind(NOC_CREDIT_MSG);
	crd->setVC(otherVC);
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
    OPCalc(msg);

	if (msg->getFirstNet()) {
		msg->setFirstNetTime(simTime());
		msg->setFirstNet(false);
	}

	// Queue packet on appropriate VC/Port
	int inPort = msg->getArrivalGate()->getIndex();
	struct vc_t &vc = m_ports[inPort].getVC(msg);
	if(!vc.accept(msg)) { // Queue flit on VC
	    // if this code would have seen daylight, we should have sent a NACK...
	    throw cRuntimeError("Couldn't accept flit in VC, not enough credits");
	}

	if(msg->getId()==179) {
	    cerr << getIndex() << "." << inPort << "." << vc.m_id << " c:" << vc.m_credits << "\n";
	}

	if(!popMsg->isScheduled()) {
	    scheduleAt(simTime()+tClk_s, popMsg);
	}
}

void CentSchedRouter::handlePop(NoCPopMsg* msg) {
    if (hasData() && (!popMsg->isScheduled())) {
        scheduleAt(simTime() + tClk_s, popMsg);
    }
    deliver();
}

bool CentSchedRouter::isLinked(vc_t& source) {
    bool linked = false;
    vc_t* to = source.m_linkedTo;
    if(to!=NULL) {
        if(m_linker[to] == &source) {
            linked = true;
        } else {
            throw cRuntimeError("Source VC is linked to a VC which isn't linked back to it");
        }
    }
    return linked;
}

bool CentSchedRouter::tryToLink(vc_t& source, int outPort) {
    bool linked = false;
    if(isLinked(source)) {
        cerr << "Source is already linked";
        linked = true;
    } else {
        for(unsigned int i = 0; i < m_ports[outPort].m_vcs.size(); ++i) {
            vc_t *vc = &(m_ports[outPort].m_vcs[i]);
            if(!vc->used()) {
                source.m_linkedTo = vc;
                m_linker[vc] = &source;
                vc->m_used = true; // Avoid electing it
                vc->m_activePacket = source.m_activePacket;
                linked = true;
                break;
            }
        }
    }
    return linked;
}

bool CentSchedRouter::port_t::hasData() {
    bool hasData = false;
    for(unsigned int i = 0; i < m_vcs.size(); ++i) {
        hasData |= !(m_vcs[i].empty());
    }
    return hasData;
}

bool CentSchedRouter::hasData() {
    bool hasData = false;
    for(unsigned int i = 0; i < m_ports.size(); ++i) {
        hasData |= m_ports[i].hasData();
    }
    return hasData;
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

/**
 * Deliver flits for each gate that has flits to send. using winner-takes-all method.
 * This function is being called for each TICK in the system.
 */
void CentSchedRouter::deliver() {
    // Iterate all in-ports and try to deliver messages. this method implements winner-takes-all method
    for(unsigned int ip = 0; ip < m_ports.size(); ++ip) {
        if(!m_ports[ip].hasElectedVC()) { // Try to elect a VC
            m_ports[ip].electVC();
        }

        if(m_ports[ip].hasElectedVC()) { // If we have an elected VC, send stuff
            struct vc_t &vc = m_ports[ip].getElectedVC();
            NoCFlitMsg *msg = vc.m_flits.front();
            int swOutPortIdx = OPCalc(msg); // Get output port which would reach the flit's destination
            bool linked = isLinked(vc);

            if(!linked) {
                linked = tryToLink(vc, swOutPortIdx);
            }

            if(linked) {
                if(vc.canRelease()) {
                    bool busy = this->gate("out$o", swOutPortIdx)->getTransmissionChannel()->isBusy();
                    if(!busy) {
                        msg = vc.release();
                        int prevVC = msg->getVC();
                        msg->setVC(vc.m_linkedTo->m_id);
                        int incommingPort = msg->getArrivalGate()->getIndex();

                        sendCredits(incommingPort,prevVC, 1);
                        send(msg, "out$o", swOutPortIdx);
                    }
                }
            } else {
                continue;
            }


//            if(vc.canRelease()) {
//                NoCFlitMsg *msg = vc.m_flits.front();
//                int swOutPortIdx = OPCalc(msg); // Get output port which would reach the flit's destination
//                bool busy = this->gate("out$o", swOutPortIdx)->getTransmissionChannel()->isBusy();
//                if(busy) {
////                    cerr << "Channel busy, skipping round\n";
//                } else {
//                    msg = vc.release();
//                    int prevVC = msg->getVC();
//                    int incommingPort = msg->getArrivalGate()->getIndex();
//
//                    msg->setVC(vc.m_id);
//
////                    const char* objType = msg->getArrivalGate()->getPreviousGate()->getOwnerModule()->getClassName();
////                    if(strcmp("CentSchedRouter", objType)==0) {
////                        CentSchedRouter* rt = (CentSchedRouter*)msg->getArrivalGate()->getPreviousGate()->getOwnerModule();
////                        int fromPort = msg->getArrivalGate()->getPreviousGate()->getIndex();
////                        if(rt->getIndex()==7 && fromPort==1 && prevVC==0) {
////                            cerr << "router["<< getIndex() <<"]["<< ip <<"]["<< vc.m_id <<"] Flit: " << msg << " " << msg->getId()<<" **\n";
////                        }
////                    }
////
////                    if(msg->getId() == 179) {
////                        cerr << vc.m_credits << "\n";
////                    }
////
//                    sendCredits(incommingPort,prevVC, 1);
//                    send(msg, "out$o", swOutPortIdx);
//                }
//            }
        }
    }
}

void CentSchedRouter::handleCredit(NoCCreditMsg* msg) {
    int creditVc = msg->getVC();
    int numFlits = msg->getFlits();
    int port = msg->getArrivalGate()->getIndex();
    vc_t &vc = m_ports[port].m_vcs[creditVc];
    vc.m_credits += numFlits;
    // I don't know why the sink is sending 100 credits but be it as it may be, we will w/a it.
    if((vc.m_credits > vc.m_linkCredits) && (port != corePort)) {
        throw cRuntimeError("VC %d.%d has more credits than it should have maximum is %d",
                port, vc.m_id, vc.m_linkCredits);
    }
    delete msg;
}

int CentSchedRouter::OPCalc(NoCFlitMsg* msg) {
    int dx, dy;
    dy = msg->getDstId() / numCols;
    dx = msg->getDstId() % numCols;
    int swOutPortIdx = -1;
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

    return swOutPortIdx;
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
//    NoCFlitMsg *flit = dynamic_cast<NoCFlitMsg*>(msg);

    switch(type) {
    case NOC_FLIT_MSG: handleFlitMsg((NoCFlitMsg*)msg); break;
    case NOC_POP_MSG: handlePop((NoCPopMsg*)msg); break;
    case NOC_CREDIT_MSG: handleCredit((NoCCreditMsg*)msg); break;
    default:
        throw cRuntimeError("Unsupported message arrival");
    }
}

/*
 * Checks if a flit "belongs" to this VC. belonging of a flit is defined
 * as one of the following criterion:
 *  1. VC is empty & flit is Head
 *  2. flit belongs to a packet which belongs to the same message
 *  * Credits are not considered *
 *
 *  @return true if flit "belongs" to vc.
 */
bool CentSchedRouter::vc_t::belongs(NoCFlitMsg* flit) {
    bool canAccept = false;

    // Check if packet fits
    /**
     * Heads can be accepted without scrutiny iff Q is empty, otherwise
     * check if VC is owned by current flow
     */
    canAccept = m_activePacket == flit->getPktId();

    if(!canAccept && isHead(flit)) { // In case VC is not being used, heads can claim them
        canAccept = !used();
    }

    return canAccept;
}

/*
 * make this VC try and accept given flit. if flit was accepted,
 */
bool CentSchedRouter::vc_t::accept(NoCFlitMsg* flit) {
//    AppFlitMsg *appFlit = dynamic_cast<AppFlitMsg*>(flit);
    bool canAccept = belongs(flit);
    // TODO: Assert if m_linkCredits is non negative
    // Check if packet queueable and queue it
    if(canAccept) {
        if(m_flits.size()>=((size_t)m_linkCredits)) { // not enough credits
            canAccept = false;
        } else {
            if(!used()) {
              if(isHead(flit)) {
                  m_activePacket = flit->getPktId();
                  m_used = true;
              } else {
                  throw cRuntimeError("Non head flit arrived on empty VC which it doesn't belong to");
              }
            }
            m_flits.push(flit);
//            cerr << "vc << " << m_id <<" Empty after insertion: " << m_flits.empty() << " size " << m_flits.size() << "\n";
//            cerr << "Inserting on vc #" << this << " queue #" << (&(m_flits)) << "\n";
        }
    }
    return canAccept;
}

// Check if current VC is empty
bool CentSchedRouter::vc_t::empty() {
//    cerr << "Queue #" << &m_flits << "\n";
    return m_flits.empty();
}
bool CentSchedRouter::vc_t::used() {
    return (m_activePacket!=-1)||m_used;
}

void CentSchedRouter::vc_t::takeOwnership(NoCFlitMsg* flit) {
    if(!isHead(flit)) {
        throw cRuntimeError("Non head flit can't take ownership of VCs");
    }
    AppFlitMsg *appFlit = dynamic_cast<AppFlitMsg*>(flit);
    if(appFlit==NULL) {
        throw cRuntimeError("I want you to take me to funky towwwwnnnn");
    }
    m_activePacket = flit->getPktId();
    m_used = true;
}

/* Tells a VC to release a flit from the head of it's Queue. if no flits are found
 * throw an exception
 * @return Flit to transmit
 */
NoCFlitMsg* CentSchedRouter::vc_t::release() {
    if(empty()) {
        throw cRuntimeError("Trying to release a flit from an empty VC");
    }

    if(m_credits == 0) {
        throw cRuntimeError("Trying to release a flit when no credits are available");
    }

    NoCFlitMsg *msg = m_flits.front();
    m_flits.pop();
    --m_credits;

    if(isHead(msg)) {
        takeOwnership(msg);
    } else if(isTail(msg)) {
        m_activePacket = -1;
        m_used = false;
        if(!m_flits.empty()) {
            takeOwnership(m_flits.front());
        }
    } else { // Regular packet - check if it was supposed to be here
        if(m_activePacket!=msg->getPktId()) {
            throw cRuntimeError("Sending a flit which doesn't belong to the packet which holds the VC");
        }
    }

    return msg;
}

/**
 * checks if we can release a flit from the current VC. we can release if
 * there are flits AND there is enough credit on the channel AND pipeline
 * stage has reached maximal depth
 * @return true if we can release a flit on this channel, otherwise false
 */
bool CentSchedRouter::vc_t::canRelease() {
    bool release = false;
    if((!empty() && (m_credits!=0))) {
        if(m_pipelineStage==(m_pipelineDepth-1)) {
            release = true;
            m_pipelineStage = 0;
        } else {
            // TODO: Prediction will cause m_pipelineStage = m_pipelineDepth-1
            ++m_pipelineStage;
        }
    }
    return release;
}

/**
 * Sets the next VC which has data to send as active. if no VC found, set sending VC
 * to none. electing sending VC will select the next VC which is after last sending VC
 * if none found search will wrap around.
 */
void CentSchedRouter::port_t::electVC() {
    unsigned int startAt = 0;
    if(m_transmittingVC!=-1) {
        startAt = m_transmittingVC;
        m_transmittingVC = -1;
    }

    for(unsigned int i = 0; i < m_vcs.size(); ++i) {
        unsigned vc = (i + startAt) % m_vcs.size();
        if(!m_vcs[vc].empty()) {
            m_transmittingVC = vc;
            break;
        }
    }
}

/**
 * Gets the currently elected VC. calling this when no VC is elected will result
 * in an exception
 */
struct CentSchedRouter::vc_t& CentSchedRouter::port_t::getElectedVC() {
    return m_vcs[m_transmittingVC];
}

/**
 * check if we've elected a VC. if VC was previously elected and it got nothing to send
 * function will return that there is no elected vc
 */
bool CentSchedRouter::port_t::hasElectedVC() {
    bool elected = false;
    if(m_transmittingVC!=-1) {
        if(!m_vcs[m_transmittingVC].empty()) {
            elected = true;
        }
    }
    return elected;
}


/* A port's method to get a VC given a NoCFlitMsg. this method will check the type of
 * Flit and search for a matching VC.
 * if Flit is a head flit - look for an empty VC, otherwise, look for a VC which holds
 * the Packet which this flit belongs to.
 * THIS METHOD WILL NOT ADD THE FLIT TO THE GIVEN VC
 *
 * @return vc_t struct which can accept this flit
 * @throws cRuntimeError if flit can't be assigned to any of the vc's this port holds
 */
struct CentSchedRouter::vc_t& CentSchedRouter::port_t::getVC(NoCFlitMsg* msg) {
    for(unsigned int i=0; i <= m_vcs.size(); ++i) {
        if(m_vcs[i].belongs(msg)) {
            return m_vcs[i];
        }
    }
    cerr << "=========================================\n";
    cerr << msg << " " << msg->getPktId()<< "\n";
    cerr << "Port " << gate->getIndex() << "\n";
    for(unsigned int i = 0; i < m_vcs.size(); ++i) {
        cerr << "vc:" << i << " " << "active: " << m_vcs[i].m_activePacket <<
                " credits: " << m_vcs[i].m_credits << " queue: " << m_vcs[i].m_flits.size() <<"\n";
    }
    throw new cRuntimeError("Flit doesn't fit in any VC on this port");
}


CentSchedRouter::~CentSchedRouter() {
    cancelAndDelete(popMsg);
}
