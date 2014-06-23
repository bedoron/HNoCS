//
// Copyright (C) 2010-2011 Eitan Zahav8i, The Technion EE Department
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
#include "DoubleBufferFlatPort.h"
#include <iostream>
#include "ResponseDB.h"
using std::cerr;
using std::out_of_range;

Define_Module(CentSchedRouter)
;

void CentSchedRouter::initialize() {
    finishInit = false;
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

    int pipelineDepth = par("pipelineDepth"); //40; // TODO: get this as a parameter
    tClk_s = 1.0/(dataRate*1000000.0/(8.0 * flitSize_B));

    if( tClk_s < 0 ) {
        throw cRuntimeError("Clock frequency of %f is negative " , tClk_s);
    }

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

	ports.resize(numPorts);
	for(int i = 0; i < numPorts; ++i) {
	    cGate *g = gate("out$o", i);
	    FlatPortIfc *p = NULL;
	    if(g->isConnectedOutside()) {
	        if(i==corePort) {
	            p = new DoubleBufferFlatPort(this, g, ports, numVCs, pipelineDepth, flitsPerVC);
	        } else {
	            p = new FlatPort(this, g, ports, numVCs, pipelineDepth);
	        }
	    }
	    ports[i] = p;

	    if(p!=NULL && (i!=corePort)) {
            for(int vc=0; vc < numVCs; ++vc) {
                sendCredits(i, vc, flitsPerVC);
            }
	    }
	}

	sendCredits(corePort, 0, flitsPerVC);

    popMsg = new cMessage("pop");
    popMsg->setKind(NOC_POP_MSG);
    popMsg->setSchedulingPriority(5);
    finishInit = true;
}

	// send back a credit on the in port
void CentSchedRouter::sendCredits(int ip, int otherVC, int numFlits) {
	EV<< "-I- " << getFullPath() << " sending " << numFlits
	<< " credits on VC=" << 0 << endl;

	char credName[64];
	const char *prefix = (!finishInit)?"INIT ":"";
	sprintf(credName, "%scred-%d-%d-%d", prefix, ip, otherVC, numFlits);
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
	if (msg->getFirstNet()) {
		msg->setFirstNetTime(simTime());
		msg->setFirstNet(false);
	}

	int inPort = msg->getArrivalGate()->getIndex();

	if(ports.at(inPort) == NULL) {
	    throw cRuntimeError("Flit arrived on a disconnected port :(");
	}

	bool tagged = ResponseDB::getInstance()->exists(msg->getId());
    if (tagged) {
        SessionMeta* meta = ResponseDB::getInstance()->find(msg->getId());
        AppFlitMsg* moomsg = dynamic_cast<AppFlitMsg*>(msg);

        if (moomsg->getPktIdx() == 0) { // This is the first flit of the AppMsg packet
            meta->addRouter(msg, this->getIndex());
            meta->addOutPort(OPCalc(msg));
            meta->addInPort(msg->getArrivalGate()->getIndex());
        }
    }

	ports[inPort]->acceptExternal(msg);
}

void CentSchedRouter::handlePop(NoCPopMsg* msg) {

    if (hasData() && (!popMsg->isScheduled())) {
        scheduleAt(simTime() + tClk_s, popMsg);
    }

    for(int i=0; i < numPorts; ++i) {
        if(ports[i] != NULL)
            ports[i]->tickInner();
    }
    // Maintain this order !
    for(int i=0; i < numPorts; ++i) {
        if(ports[i] != NULL)
            ports[i]->tickOuter();
    }
}


void CentSchedRouter::handleCredit(NoCCreditMsg* msg) {
    ports[msg->getArrivalGate()->getIndex()]->acceptExternal(msg);
    delete msg;
}

int CentSchedRouter::OPCalc(NoCFlitMsg* msg) {
    int dx, dy;
    dy = msg->getDstId() / numCols;
    dx = msg->getDstId() % numCols;
    int swOutPortIdx = -1;

    bool response = ResponseDB::getInstance()->isResponse(msg->getId());
    // Select routing as needed
    if (response) {
        if ((dx == rx) && (dy == ry)) {
            swOutPortIdx = corePort;
        } else if (dy > ry) {
            swOutPortIdx = northPort;
        } else if (dy < ry) {
            swOutPortIdx = southPort;
        } else if (dx > rx) {
            swOutPortIdx = eastPort;
        } else {
            swOutPortIdx = westPort;
        }
    } else {
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
    }

    if(swOutPortIdx<0) {
        throw cRuntimeError("Dead end routing");
    }

    return swOutPortIdx;
}

void CentSchedRouter::handleMessage(cMessage *msg) {
    NOC_MSGS type = (NOC_MSGS) msg->getKind();

    switch(type) {
    case NOC_FLIT_MSG: handleFlitMsg((NoCFlitMsg*)msg); break;
    case NOC_POP_MSG: handlePop((NoCPopMsg*)msg); break;
    case NOC_CREDIT_MSG: handleCredit((NoCCreditMsg*)msg); break;
    default:
        throw cRuntimeError("Unsupported message arrival");
    }

    if (hasData() && (!popMsg->isScheduled())) {
        scheduleAt(simTime() + tClk_s, popMsg);
    }
}

FlatPortIfc* CentSchedRouter::requestPort(int portId) {
    FlatPortIfc *p = NULL;
    try {
        p = ports.at(portId);
    } catch (std::out_of_range e) {
        throw cRuntimeError("Port %d doesn't exit");
    }
    return p;
}

bool CentSchedRouter::hasData() {
    bool data = false;
    for(int port = 0; (port < numPorts) && (data == false); ++port) {
        data |= (ports[port]==NULL)?false:ports[port]->hasData();
    }
    return data;
}

void CentSchedRouter::callPredictor(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {

    ResponseDB* respDB = ResponseDB::getInstance();
    bool predict = respDB->exists(msg->getId()) && respDB->isResponse(msg->getId()) && (!inVC.predicted);

    // Random predictor proof of concept
    if(predict) {
        inVC.predicted = true;
        if(rand()%2) {
            inVC.pipelineLatency = 0;
            cerr << "Router " << getIndex() << " predicted pass from " << inPort->getId() << " to " << outPort->getId() << "\n";
        } else {
            cerr << "Router " << getIndex() << " missed pass from " << inPort->getId() << " to " << outPort->getId() << "\n";
        }
    }


}

CentSchedRouter::~CentSchedRouter() {
    cancelAndDelete(popMsg);
    for(int i=0;i < numPorts; ++i) {
        delete ports[i];
    }
}
