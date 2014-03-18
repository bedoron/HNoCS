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

#include "XYOPCalc.h"
#include <NoCs_m.h>
#include <CMP_m.h>	// recognize CMPMsg
#include "ResponseDB.h"
#include "ctimestampedvalue.h"
#include "cResponseHeadArrived.h"

#include <iostream>
#include "Utils.h"

using std::cerr;

Define_Module(XYOPCalc);

int XYOPCalc::rowColByID(int id, int &x, int &y)
{
	y = id / numCols;
	x = id % numCols;
	return(0);
}

// return true if the provided cModule pointer is a Port
bool
XYOPCalc::isPortModule(cModule *mod)
{
	return(mod->getModuleType() == cModuleType::get(portType));
}

// return the pointer to the port on the other side of the given port or NULL
cModule *
XYOPCalc::getPortRemotePort(cModule *port)
{
	cGate *gate = port->gate("out$o");
	if (!gate) return NULL;
	cGate *remGate = gate->getPathEndGate()->getPreviousGate();
	if (!remGate) return NULL;
	cModule *neighbour = remGate->getOwnerModule();
	if (!isPortModule(neighbour)) return NULL;
	if (neighbour == port) return NULL;
	return neighbour;
}

// return true if the provided cModule pointer is a Core
bool
XYOPCalc::isCoreModule(cModule *mod)
{
	return(mod->getModuleType() == cModuleType::get(coreType));
}

// return the pointer to the Core on the other side of the given port or NULL
cModule *
XYOPCalc::getPortRemoteCore(cModule *port)
{
	cGate *gate = port->gate("out$o");
	if (!gate) return NULL;
	cGate *remGate = gate->getPathEndGate()->getPreviousGate();
	if (!remGate) return NULL;
	cModule *neighbour = remGate->getOwnerModule();
	if (!isCoreModule(neighbour)) return NULL;
	if (neighbour == port) return NULL;
	return neighbour;
}

// Given the port pointer find the index idx such that sw_out[idx] 
// connect to that port
int
XYOPCalc::getIdxOfSwPortConnectedToPort(cModule *port)
{
	for (int i=0; i< getParentModule()->gateSize("sw_in"); i++) {
		cGate *oGate = getParentModule()->gate("sw_in", i);
		if (!oGate) return -1;
		cGate *remGate = oGate->getPathEndGate()->getPreviousGate();
		if (!remGate) return -1;
		cModule *neighbour = remGate->getOwnerModule();
		if (neighbour == port)
			return i;
	}
	return -1;
}

// Analyze the topology of this router and obtain the port numbers
// connected to the 4 directions
int
XYOPCalc::analyzeMeshTopology()
{
	// if not found the port numbers will be -1
	northPort = -1;
	westPort  = -1;
	southPort = -1;
	eastPort  = -1;
	corePort  = -1;
	cModule *router = getParentModule()->getParentModule();
	// go over all the router ports and check their remote side if they are of type "Port"
	for (cModule::SubmoduleIterator iter(router); !iter.end(); iter++) {
		if (! isPortModule(iter())) continue;
		cModule *port = iter();

	    // Get the remote port which this port is connected. remote port is on a different router than port.
		cModule *remPort = getPortRemotePort(port);
		cModule *remCore = getPortRemoteCore(port);

		// obtain the idx of this port sw_out[idx] that connects to port
		int portIdx = getIdxOfSwPortConnectedToPort(port);

		// Save the port Id which this OPCalc is bound to
		//bool isSelf = false;
        if(port == this->getParentModule()) {
          //isSelf = true;
          boundPort = portIdx;
          portIdx = getIdxOfSwPortConnectedToPort(port);
        }


		if (remCore) {
			// remote side is the core connected to the router
			int x,y;
			rowColByID((int)(remCore->par("id")), x, y);
			if ((rx == x) && (ry == y)) {
				EV << "-I- " << getParentModule()->getFullPath()
					<< " connected through sw_out[" << portIdx
					<< "] to Core port: " << port->getFullPath() << endl;



				corePort = portIdx;




			} else {
				throw cRuntimeError("Port: %s and connected Core %s do not share the same x:%d and y:%d",
						port->getFullPath().c_str(), remCore->getFullPath().c_str(),
						x, y);
			}
		} else if (remPort) {
			// remote side is another router port
			// get the remote port x,y
			int x,y;
			rowColByID((int)(remPort->getParentModule()->par("id")), x, y);
			if ((rx == x) && (ry == y)) {
				throw cRuntimeError("Ports: %s and %s share the same x:%d and y:%d",
						port->getFullPath().c_str(), remPort->getFullPath().c_str(), x, y);
			} else if ((rx == x) && (ry == y + 1)) {
				// remPort is south port
				if (southPort != -1) {
					throw cRuntimeError("Already found a south port: %d for ports: %s."
							" %s is miss-configured",
							southPort, port->getFullPath().c_str(),
							remPort->getFullPath().c_str());
				} else {
					EV << "-I- " << getParentModule()->getFullPath()
						<< " connected through sw_out[" << portIdx
						<< "] to South port: " << port->getFullPath() << endl;
					southPort = portIdx;
				}
			} else if ((rx == x) && (ry == y - 1)) {
				// remPort is north port
				if (northPort != -1) {
					throw cRuntimeError("Already found a north port: %d for ports: %s."
							" %s is miss-configured",
							northPort, port->getFullPath().c_str(),
							remPort->getFullPath().c_str());
				} else {
					EV << "-I- " << getParentModule()->getFullPath()
						<< " connected through sw_out[" << portIdx
						<< "] to North port: " << port->getFullPath() << endl;
					northPort = portIdx;
				}
			} else if ((rx == x + 1) && (ry == y)) {
				// remPort is west port
				if (westPort != -1) {
					throw cRuntimeError("Already found a west port: %d for ports: %s."
							" %s is miss-configured",
							westPort, port->getFullPath().c_str(),
							remPort->getFullPath().c_str());
				} else {
					EV << "-I- " << getParentModule()->getFullPath()
						<< " connected through sw_out[" << portIdx
						<< "] to West port: " << port->getFullPath() << endl;
					westPort = portIdx;
				}
			} else if ((rx == x - 1) && (ry == y)) {
				// remPort is east port
				if (eastPort != -1) {
					throw cRuntimeError("Already found an east port: %d for ports: %s."
							" %s is miss-configured",
							eastPort, port->getFullPath().c_str(),
							remPort->getFullPath().c_str());
				} else {
					EV << "-I- " << getParentModule()->getFullPath()
						<< " connected through sw_out[" << portIdx
						<< "] to East port: " << port->getFullPath() << endl;
					eastPort = portIdx;
				}
			} else {
				throw cRuntimeError("Found a non Mesh connection between %s (%d,%d) and %s (%d,%d)",    			" %s is miss-configured",
						port->getFullPath().c_str(), rx,ry,
						remPort->getFullPath().c_str(),x,y);
			}
		}
	}

	if (corePort < 0) {
		EV << "-W- " << getParentModule()->getFullPath()
			<< " could not find corePort (of coreType:" << coreType << ")" << endl;
	}


//	cerr << getFullPath() << " - Core prot " << corePort << "\n";

	return(0);
}

void XYOPCalc::initialize()
{
    coreType = par("coreType");
    portType = par("portType");


    flitSecondIncidence = registerSignal("flit-second-incidence"); // Register Signal

    // the id is supposed to be on the router
    cModule *router = getParentModule()->getParentModule();
    int id = router->par("id");
    numCols = router->getParentModule()->par("columns");

    rowColByID(id, rx, ry);
    // Analyze the connections of this port building the port number to be used for routing
    // north, south, west and east. if there is no way to go on some direction the
    analyzeMeshTopology();
    EV << "-I- " << getFullPath() << " Found N/W/S/E/C ports:" << northPort
    		<< "/" << westPort << "/" << southPort << "/"
    		<< eastPort << "/" << corePort << endl;

    m_meshIndex = getParentModule()->getParentModule()->getIndex();


    WATCH(northPort);
    WATCH(westPort);
    WATCH(eastPort);
    WATCH(southPort);
    WATCH(corePort);
}

void XYOPCalc::SetOutPort(NoCFlitMsg* msg) {
    bool tagged = ResponseDB::getInstance()->exists(msg->getId());
    bool response = ResponseDB::getInstance()->isResponse(msg->getId());
    int swOutPortIdx;

    SessionMeta *session = ResponseDB::getInstance()->find(msg);

    if((session != NULL) && session->getSessionId()==36) {
        if(msg->getPktId()==262144 && (getParentModule()->getParentModule()->getIndex()==4) ) {
            cerr << "Packet 262144 detected\n";
            cerr << msg;
        }
    }


//    if(dstPort!=m_meshIndex) {

        int dx, dy;
        rowColByID(msg->getDstId(), dx, dy);
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
        if (swOutPortIdx < 0) {
            throw cRuntimeError(
                    "Routing dead end at %s (%d,%d) for destination %d (%d,%d)",
                    getParentModule()->getFullPath().c_str(), rx, ry,
                    msg->getDstId(), dx, dy);
        }
//    } else {
//        swOutPortIdx = 4;
//    }
    // TODO - move into a common header for msgs ?
    cObject* obj = msg->getControlInfo();
    if (obj == NULL) {
        throw cRuntimeError("-E- %s BUG - No Control Info for FLIT: %s",
                getFullPath().c_str(), msg->getFullName());
    }
    inPortFlitInfo* info = dynamic_cast<inPortFlitInfo*>(obj);
    info->outPort = swOutPortIdx;
    // Emit packet event & tag this router
    if (tagged) {
        //int srcPort = boundPort;//getIdxOfSwPortConnectedToPort(getParentModule());
        int srcPort = this->getParentModule()->getIndex();
        SessionMeta* meta = ResponseDB::getInstance()->find(msg->getId());
        cModule* router = getParentModule()->getParentModule();
        AppFlitMsg* moomsg = dynamic_cast<AppFlitMsg*>(msg);
        // This is the first flit of the NoC packet
        if (moomsg->getPktIdx() == 0) {
            meta->addRouter(msg, router->getIndex());
            meta->addOutPort(swOutPortIdx);
            meta->addInPort(srcPort);
            //            std::cerr << srcPort << "->" << swOutPortIdx << "\n";
        }
        if (response & mayHaveListeners(flitSecondIncidence)) {
            cResponseHeadArrived data(router, msg, meta);
            emit(flitSecondIncidence, &data);
        }
    }

//    NoCFlitMsg *flit = (NoCFlitMsg*)msg;
//    if(flit->getPktId()==262144 && (getParentModule()->getParentModule()->getIndex()==4)) {
//        cerr << "=-=-=-=-=-= Decided that 262144 will go out on port: " << swOutPortIdx << "\n";
//      cerr << "----------------------------\n";
//    }
}

void XYOPCalc::handlePacketMsg(NoCFlitMsg* msg)
{
//    if(getParentModule()->getParentModule()->getIndex()==9) {
//        cerr << "Router 9 handlePacketMsg start for \n";
//        cerr << msg;
//        cerr << "\n";
//    }

//    NoCFlitMsg *flit = (NoCFlitMsg*)msg;
//    if(flit->getPktId()==262144) {
//       cerr << "OPCLALC!!!\n";
//       cerr << "Caught flit " << 262144 << " in router " << getParentModule()->getParentModule()->getIndex() << " port " << getParentModule()->getIndex() << "\n";
//       cerr << flit;
//       cerr << "----------------------------\n";
//    }

    if(msg->getType()==NOC_START_FLIT) {
        SetOutPort(msg);
    }
    send(msg, "calc$o");

//    if(getParentModule()->getParentModule()->getIndex()==9) {
//        cerr << "Router 9 handlePacketMsg end\n";
//    }
}

void XYOPCalc::handleMessage(cMessage *msg)
{
    int msgType = msg->getKind();
    EV << "XYOPCalc::handleMessage - we got packet " << msg->getId() << "\n";
    if ( msgType == NOC_FLIT_MSG ) {
    	handlePacketMsg((NoCFlitMsg*)msg);
    } else {
    	throw cRuntimeError("Does not know how to handle message of type %d", msg->getKind());
    	delete msg;
    }
}

void XYOPCalc::PredictorSetOutPort(NoCFlitMsg* msg) {
    Enter_Method_Silent();
    take(msg);
    SetOutPort(msg);
    drop(msg);
}
