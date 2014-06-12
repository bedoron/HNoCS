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
using std::find;

/**
 * router - the router which this port resides on.
 * gate - the gate which this port is connected to
 * adjacent - the port which is connected to this port in another router
 * allPort - a vector of all ports in the router which contains this port
 * numVCs - the total amount of VCs on this port
 */
FlatPort::FlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPort*> &allPorts, int numVcs, int pipelineLatency):
        ports(allPorts), router(router), gate(gate), routerId(router->getIndex()), id(gate->getIndex()),
        pipelineLatency(pipelineLatency) {
    innerActiveVc = 0;
    outerActiveVc = 0;

    vcs.resize(numVcs);
    for(int i=0; i < numVcs; ++i) {
        vcs[i].id = i;
        vcs[i].outPort = NULL;
        vcs[i].outVC = -1;
        vcs[i].credit = 0;
        vcs[i].pipelineLatency = pipelineLatency;
    }
}

vc_t* FlatPort::acceptExternal(NoCCreditMsg* msg) {

    if(msg->getArrivalGate()->getIndex()!=id) {
        throw cRuntimeError("Credit is not mine... [port %d router %d]", id, routerId);
    }

    vc_t &vc = vcs[msg->getVC()];

    if(vc.state == INTERNALY_TAKEN) {
        throw cRuntimeError("Accepted credit on a VC which is internally taken.");
    }

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
    bool canAccept =
            ((Utils::isHead(msg) && vc->state==FREE) ||
             (!Utils::isHead(msg) && vc->pktId==msg->getPktId())) && (vc->credit!=0);
    return canAccept;
}

/**
 * link vc to another vc on outPort.
 */
void FlatPort::setupInternalLinkIfNeeded(NoCFlitMsg* msg, FlatPort* outPort, vc_t& vc) {
    if (Utils::isHead(msg) && (vc.outVC==-1) && (--vc.pipelineLatency == 0)) {
        // we need to setup a channel because it wasn't setup
        vc.pipelineLatency = pipelineLatency;
        vector<vcState> states = outPort->getVCStates();
        unsigned int outVCId = find(states.begin(), states.end(), FREE) - states.begin();
        if (outVCId != states.size()) {
            vc.outVC = outVCId;
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
        FlatPort *outPort = vc.outPort;

        setupInternalLinkIfNeeded(msg, outPort, vc);

        if(!Utils::isHead(msg) && (vc.outVC == -1)) {
            throw new cRuntimeError("Head is not in Q and link wasn't setup");
        }

        // Channel + Credits are available
        if((vc.outVC != -1) && (vc.credit > 0)) {
            vc.flits.pop();

            inPortFlitInfo *oldInfo = Utils::getFlitInfo(msg);
            inPortFlitInfo *newInfo = new inPortFlitInfo(id, vc.id);

            int prevVc = oldInfo->inVC;
            delete oldInfo;

            msg->setControlInfo(newInfo);
            msg->setVC(vc.outVC);
            outPort->acceptInternal(msg);

            router->sendCredits(id, prevVc, 1);
            --vc.credit;

            if(Utils::isTail(msg)) {
                releaseVc(vc);
            }
        }
    }
}

void FlatPort::tickOuter() {
    electOuterActiveVc();
    vc_t &vc = vcs[outerActiveVc];

    if((vc.state == INTERNALY_TAKEN) && !vc.flits.empty()) {
        NoCFlitMsg *msg = vc.flits.front();

        FlatPort *adjPort = router->getAdjacent(gate);

        // Setup link here
        if(Utils::isHead(msg) && (vc.outVC == -1)) {
            vc.pipelineLatency = pipelineLatency;
            vector<vcState> states = adjPort->getVCStates();
            unsigned int outVCId = find(states.begin(), states.end(), FREE) - states.begin();
            if (outVCId != states.size()) {
                vc.outVC = outVCId;
            }
        }

        if(!Utils::isHead(msg) && (vc.outVC == -1)) {
            throw new cRuntimeError("Head is not in Q and link wasn't setup");
        }

        if((vc.outVC != -1)&&vc.credit) {
            if(!gate->getTransmissionChannel()->isBusy()) {
                inPortFlitInfo *info = Utils::getFlitInfo(msg);
                int originVC = info->inVC;
                int originPort = info->outPort;
                delete info;
                info = new inPortFlitInfo(id, vc.id);
                msg->setControlInfo(info);
                ports[originPort]->acceptInternal(originVC, 1);

                router->send(msg, "in$o", id);
                vc.credit--;

                if(Utils::isTail(msg)) {
                    releaseVc(vc);
                }
            }
        }
    }
}

vc_t* FlatPort::acceptFlit(FlatPort *outPort, NoCFlitMsg* msg, vcState state) {
    vc_t *accepting = NULL;
    try {
        accepting = &(vcs.at(msg->getVC()));
    } catch(std::out_of_range e) { // Sanity
        throw new cRuntimeError("vc %d doesn't exist on port %d", msg->getVC(), id);
    }

    if(!vcCanAccept(accepting, msg)) {
        throw new cRuntimeError("vc %d can't accept flit", accepting->id);
    }

    accepting->flits.push(msg);

    // Flit acceptance logic
    if(Utils::isHead(msg)) { // Initiate a new channel
        if(accepting->flits.size()!=1) {
            throw cRuntimeError("Q wasn't empty when head was added");
        }
        accepting->state = state;
        accepting->pktId = msg->getPktId();
        accepting->outPort = outPort;
        accepting->outVC = -1;
        accepting->pipelineLatency = pipelineLatency;
    }
    return accepting;
}

void FlatPort::releaseVc(vc_t& vc) {
    vc.state = FREE;
    vc.pktId = -1;
    vc.outPort = NULL;
    vc.pipelineLatency = pipelineLatency;

    if(!vc.flits.empty()) {
        throw new cRuntimeError("Releasing vc %d on port %d which is not empty", vc.id, id);
    }
}

vc_t* FlatPort::acceptInternal(int vcNum, int credits) {
    try {
        vc_t &vc = vcs.at(vcNum);
        vc.credit += credits;
        return &vc;
    } catch (std::out_of_range e) {
        throw cRuntimeError("Accessing Virtual channel which isn't in range");
    }
    return NULL;
}

FlatPort::~FlatPort() {
}

bool FlatPort::hasData() {
    bool data = false;
    for(vector<vc_t>::iterator vc = vcs.begin(); (vc != vcs.end()) && (data == false); ++vc) {
        data |= !(vc->flits.empty());
    }
    return data;
}
