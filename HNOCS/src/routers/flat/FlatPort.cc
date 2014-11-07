//
// This program is free software: you can r8edistribute it and/or modify
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

#include "FlatPort.h"
#include "Utils.h"
#include <algorithm>
#include "CentSchedRouter.h"
#include "InterRouterMatchers.h"
using std::find;
using std::cerr;

/**
 * router - the router which this port resides on.
 * gate - the gate which this port is connected to
 * adjacent - the port which is connected to this port in another router
 * allPort - a vector of all ports in the router which contains this port
 * numVCs - the total amount of VCs on this port
 */
FlatPort::FlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*> &allPorts, int numVcs, int pipelineLatency):
        ports(allPorts), router(router), gate(gate), routerId(router->getIndex()), id(gate->getIndex()),
        pipelineLatency(pipelineLatency) {
    innerActiveVc = 0;
    outerActiveVc = 0;

//    lastReceived = cSimulation::getActiveSimulation()->getSimTime();
//    lastSent= cSimulation::getActiveSimulation()->getSimTime();

    vcs.resize(numVcs);
    for(int i=0; i < numVcs; ++i) {
        vcs[i].id = i;
        vcs[i].outPort = NULL;
        vcs[i].outVC = -1;
        vcs[i].credit = 0;
        vcs[i].pipelineLatency = pipelineLatency;
        vcs[i].predicted = false;
        vcs[i].vcOpen = NULL;
    }
}

vc_t* FlatPort::acceptExternal(NoCCreditMsg* msg) {
    if(msg->getArrivalGate()->getIndex()!=id) {
        throw cRuntimeError("Credit is not mine... [port %d router %d]", id, routerId);
    }

    vc_t &vc = vcs[msg->getVC()];

    vc.credit += msg->getFlits();
    return &vc;
}

vc_t* FlatPort::acceptExternal(NoCFlitMsg* msg) {
    return acceptFlit(ports[router->OPCalc(msg)], msg, EXTERNALY_TAKEN);
}

vc_t* FlatPort::acceptInternal(NoCFlitMsg* msg) {
    return acceptFlit(adjacent, msg, INTERNALY_TAKEN);
}

vector<vcState> FlatPort::getVCStates() {
    vector<vcState> states;
    states.resize(vcs.size());
    for(unsigned int i=0; i < vcs.size(); ++i) {
        states[i] = vcs[i].state;
    }
    return states;
}

bool FlatPort::vcCanAccept(vc_t *vc, NoCFlitMsg* msg) {
    bool canAccept = false;
    canAccept |= Utils::isHead(msg) && (vc->state!=FREE) && (vc->flits.empty());
    canAccept |= (!Utils::isHead(msg) && vc->pktId==msg->getPktId());
    canAccept &= (vc->credit!=0);
    return canAccept;
}

void FlatPort::dumpVC(vc_t* vc) {
    const char *state;
    switch (vc->state) {
    case EXTERNALY_TAKEN: state = "EXTERNALY_TAKEN"; break;
    case FREE: state = "FREE"; break;
    case INTERNALY_TAKEN: state = "INTERNALY_TAKEN"; break;
    }
    cerr << "Contents of VC " << routerId << "." << id << "." <<  vc->id << " : (c:"<< vc->credit<<",oVC:"<< vc->outVC<<") " << state << "\n";
    while (!(vc->flits.empty())) {
        cerr << vc->flits.front() << "\n";
        vc->flits.pop();
    }
}

void FlatPort::failIfCantAccept(vc_t* vc, NoCFlitMsg* msg) {
    bool canAccept = vcCanAccept(vc, msg);
    if (canAccept==false) {
        std::stringstream buff;
        buff << "\nFlit " << msg << " failed.\n";
        buff << "VC " << routerId << "." << id << "." << vc->id <<" was taken ";
        buff << ((vc->state==EXTERNALY_TAKEN)?"EXTERNALLY":(vc->state==FREE?"FREE":"INTERNALLY")) << " ";
        buff << "by packet " << vc->pktId << " and had " << vc->credit << " credits";
        cerr << buff.str() << "\n";
        dumpVC(vc);
        cerr << "***************** REST OF VCS ******************\n";
        dumpAllVCs();
        cerr << "************************************************\n";
        throw cRuntimeError("%s",buff.str().c_str());
    }
}

/**
 * link vc to another vc on outPort.
 */
void FlatPort::setupInternalLinkIfNeeded(NoCFlitMsg* msg, FlatPortIfc* outPort, vc_t& vc) {
    if (Utils::isHead(msg) && (vc.outVC==-1)) {
        router->callPredictor(msg, this, outPort, vc);
        if(--vc.pipelineLatency < 0) {
            // we need to setup a channel because it wasn't setup
            vc.pipelineLatency = pipelineLatency;
            vector<vcState> states = outPort->getVCStates();
            unsigned int outVCId = find(states.begin(), states.end(), FREE) - states.begin();
            if (outVCId != states.size()) {
                vc.outVC = outVCId;
                outPort->reserveVC(outVCId, INTERNALY_TAKEN);
            }
        }
    }
}

void FlatPort::electInnerActiveVc() {
    for (unsigned int i = 0; i < vcs.size(); ++i) {
        int vcId = (innerActiveVc + i) % vcs.size();
        vc_t& vc = vcs[vcId];
        if ((vc.state == EXTERNALY_TAKEN)) {
            innerActiveVc = vcId;
            break;
        }
    }
}

void FlatPort::electOuterActiveVc() {
    for (unsigned int i = 0; i < vcs.size(); ++i) {
        int vcId = (innerActiveVc + i) % vcs.size();
        vc_t& vc = vcs[vcId];
        if ((vc.state == INTERNALY_TAKEN)) {
            outerActiveVc = vcId;
            break;
        }
    }
}

void FlatPort::tickInner() {
    electInnerActiveVc();
    // Router intervention is not required for inner transportation
    vc_t &vc = vcs[innerActiveVc];

    if((vc.state == EXTERNALY_TAKEN) && !vc.flits.empty()) { // We have a sendable packet
        NoCFlitMsg *msg = vc.flits.front();
        FlatPortIfc *outPort = vc.outPort;

        setupInternalLinkIfNeeded(msg, outPort, vc);

        if(!Utils::isHead(msg) && (vc.outVC == -1)) {
            throw cRuntimeError("Head is not in Q and link wasn't setup");
        }

        // Channel + Credits are available
        if((vc.outVC != -1)) {

            if((id==4)&&(msg->getControlInfo()==NULL)) { // Core port internal doesn't have an intial flit info
                inPortFlitInfo *newInfo = new inPortFlitInfo(id, vc.id);
                try {
                    msg->setControlInfo(newInfo);
                } catch (cRuntimeError ex) {
                    ex.prependMessage("tickInner");
                    throw ex;
                }
            }

            if(msg->getControlInfo()==NULL) {
                throw cRuntimeError("Message %s doesn't have control info", msg->getFullName());
            }

            msg->setVC(vc.outVC);

            if(outPort->hasCredits(vc.outVC)) {
                vc.flits.pop();
                outPort->acceptInternal(msg);

                if(Utils::isTail(msg)) {
                    releaseVc(vc);
                }
            }

        }
    }
}

void FlatPort::setupExternalLinkIfNeeded(NoCFlitMsg* msg, vc_t& vc) {
    if (Utils::isHead(msg) && (vc.outVC == -1)) {
        InterRouterMatcher *matcher = InterRouterMatchers::build(router, gate, vc);
        vc.outVC = matcher->getFreeVC();
        vc.pipelineLatency = pipelineLatency;
        delete matcher;
    }
}

void FlatPort::tickOuter() {
    electOuterActiveVc();
    vc_t &vc = vcs[outerActiveVc];



    if((vc.state == INTERNALY_TAKEN) && !vc.flits.empty()) {
        NoCFlitMsg *msg = vc.flits.front();

        // Setup link here
        setupExternalLinkIfNeeded(msg, vc);

        if(!Utils::isHead(msg) && (vc.outVC == -1)) {
            throw cRuntimeError("Head is not in Q and link wasn't setup");
        }

        if((vc.outVC != -1) && vc.credit && (!gate->getTransmissionChannel()->isBusy())) {
            vc.flits.pop();

            inPortFlitInfo *info = NULL;

            try {
                info = Utils::getFlitInfo(msg);
            } catch (cRuntimeError ex) {
                ex.prependMessage("tickOuter");
                throw ex;
            }

            int originVC = info->inVC;
//            int originPort = info->outPort;
            delete info;
            // Info will hold the port id + vc id which this message came from
            info = new inPortFlitInfo(id, vc.id);
            msg->setControlInfo(info);

            // Set destination VC
            msg->setVC(vc.outVC);

            vc.credit--;

            if(Utils::isTail(msg)) {
                releaseVc(vc);
            }
            router->sendCredits(msg->getArrivalGate()->getIndex(), originVC, 1);
            router->send(msg, "out$o", id);
        }
    }
}

void FlatPort::handleVCClaim(vcState state, vc_t *accepting, NoCFlitMsg* msg, FlatPortIfc *outPort)
{
    if(accepting->flits.size()!=1) {
        int numFlits = accepting->flits.size();
        cerr << "Arriving: " << msg << "\n";
        dumpVC(accepting);
        throw cRuntimeError("VC channel was down but %d packets were found in the Q", numFlits);
    }
    if (state == FREE) {
        throw cRuntimeError("VC Wasn't calimed but head arrived");
    }
    accepting->state = state;
    accepting->pktId = msg->getPktId();
    accepting->outPort = outPort;
    accepting->outVC = -1;
    accepting->pipelineLatency = pipelineLatency;
    accepting->predicted = false;
}

bool FlatPort::isCorePort() {
    return id == 4;
}

vc_t* FlatPort::acceptFlit(FlatPortIfc *outPort, NoCFlitMsg* msg, vcState state) {
//    logIfRouterPort(msg, 14, 4);
    if (routerId == 3) {
        logIf(msg, 325371);
    }

    vc_t *accepting = NULL;
    try {
        accepting = &(vcs.at(msg->getVC()));
    } catch(std::out_of_range e) { // Sanity
        throw cRuntimeError("vc %d doesn't exist on port %d", msg->getVC(), id);
    }

    failIfCantAccept(accepting, msg);

    accepting->flits.push(msg);

    if (Utils::isHead(msg)) {
        if ((accepting->vcOpen != NULL) && !isCorePort()) {
            dumpAllVCs();
            throw cRuntimeError("Accepting flit on vc %d which didn't release it's open time", accepting->id);
        }
        accepting->vcOpen = new SimTime(cSimulation::getActiveSimulation()->getSimTime());
    }

    // Flit acceptance logic
    bool arrivedReserved = (Utils::isHead(msg) && (accepting->state==state));

    if((Utils::isHead(msg) && (accepting->state==FREE) && (id==4)) || arrivedReserved) {
        handleVCClaim(state, accepting, msg, outPort);
    }


    return accepting;
}

void FlatPort::releaseVc(vc_t& vc) {
    if(!vc.flits.empty()) {
        throw cRuntimeError("Releasing vc %d on port %d which is not empty", vc.id, id);
    }
    vc.state = FREE;
    vc.pktId = -1;
    vc.outPort = NULL;
    vc.pipelineLatency = pipelineLatency;
    vc.predicted = false;

    if (vc.vcOpen == NULL) {
        throw cRuntimeError("Releasing VC which didn't have a timestamp");
    } else {
        delete vc.vcOpen;
        vc.vcOpen = NULL;
    }
}

vc_t* FlatPort::acceptInternal(int vcNum, int credits) {
    try {
        vc_t &vc = vcs.at(vcNum);
        vc.credit += credits;
        return &vc;
    } catch (std::out_of_range e) {
        throw cRuntimeError("Accessing Virtual channel %d on port %d which isn't in range or doesn't exist", vcNum, id);
    }
    return NULL;
}

void FlatPort::logIf(NoCFlitMsg *msg, int routerId, int portId, int vcId) {
    if(this->routerId==routerId && id==portId && vcId==msg->getVC()) {
        char buff[1024];
        sprintf(buff, "[ByHW] r[%d] p[%d] vc[%d]: Packet %s [%ld]", routerId, portId, vcId, msg->getFullName(), msg->getId());
        cerr << buff << " " << msg << "\n";
    }
}

void FlatPort::logIf(NoCFlitMsg *msg, int flitId) {
    if(msg->getId() == flitId) {
        char buff[1024];
        sprintf(buff, "[ById] r[%d] p[%d] vc[%d]: Packet %s [%ld]", routerId, id, msg->getVC(), msg->getFullName(), msg->getId());
        cerr << buff << " " << msg << "\n";
    }
}

void FlatPort::logIfRouterPort(NoCFlitMsg* msg, int routerId, int port) {
    if(port==id) {
        logIfRouter(msg, routerId);
    }
}

bool FlatPort::reserveVC(int vcNum, vcState intext) {
    if(intext == FREE) {
        throw cRuntimeError("Can't set VC to be FREE");
    }
    try {
        vc_t &vc = vcs.at(vcNum);

        if(vc.state != FREE) {
            throw cRuntimeError("Can't reserve a non FREE vc");
        }

        vc.state = intext;

    } catch(std::out_of_range e) {
        throw cRuntimeError("Invalid vc %d is being reserved", vcNum);
    }
    return true;
}

void FlatPort::dumpAllVCs() {
    vector<vc_t>::iterator vcit =  vcs.begin();
    while(vcit != vcs.end()) {
        dumpVC(&(*vcit));

        ++vcit;
    }
}

bool FlatPort::hasCredits(int vc) {
    try {
    return (vcs.at(vc).credit>0);
    } catch(std::out_of_range ex) {
        throw cRuntimeError("asking if vc %d has credit but router doesn't have suuch VC", vc);
    }
}

FlatPort::~FlatPort() {
    dumpAllVCs();
}

bool FlatPort::hasData() {
    bool data = false;
    for(vector<vc_t>::iterator vc = vcs.begin(); (vc != vcs.end()) && (data == false); ++vc) {
        data |= !(vc->flits.empty());
    }
    return data;
}

void FlatPort::logIfRouter(NoCFlitMsg* msg, int routerId) {
    if(this->routerId==routerId) {
        char buff[1024];
        sprintf(buff, "[ByHW] r[%d] p[%d] vc[%d]: Packet %s [%ld]", routerId, id, msg->getVC(), msg->getFullName(), msg->getId());
        cerr << buff << " " << msg << "\n";
    }
}

int FlatPort::getId() {
    return this->id;
}

void FlatPort::watchdog(SimTime timeout) {
    for (unsigned int i=0; i < vcs.size(); ++i) {
        if (vcs[i].vcOpen != NULL) {
            SimTime diff = cSimulation::getActiveSimulation()->getSimTime() - *(vcs[i].vcOpen);
            if (diff > timeout) {
                cerr << "Watchdog error on " << this->routerId << "." << this->id << "." << i << " - time since last activity " << diff.dbl() << "\n";
                //dumpAllVCs();
                throw cRuntimeError("Watchdog error - time since last activity: %f" , diff.dbl());
            }
        }
    }
}
