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

#include <SourceFlatPort.h>
#include "Utils.h"
#include "CentSchedRouter.h"

bool SourceFlatPort::vcCanAccept(vc_t* vc, NoCFlitMsg* msg) {
    return (vc->credit!=0);
}

SourceFlatPort::SourceFlatPort(CentSchedRouter* router, cGate* gate,
        vector<FlatPortIfc*>& allPorts, int numVcs, int pipelineLatency): FlatPort(router, gate, allPorts, numVcs, pipelineLatency),
                ports(allPorts), theRouter(router), id(gate->getIndex()), pipelineLatency(pipelineLatency), routerId(router->getIndex()) {
}

void SourceFlatPort::releaseVc(vc_t& vc) {
    if(!vc.flits.empty() && !Utils::isHead(vc.flits.front())) {
        throw cRuntimeError("Releasing vc %d on port %d which is not empty and the next packet isn't a head", vc.id, id);
    }
    if(!vc.flits.empty()) {
        NoCFlitMsg *msg = vc.flits.front();
        vc.state = EXTERNALY_TAKEN;
        vc.pktId = msg->getPktId();
        vc.outPort = ports[theRouter->OPCalc(msg)];
    } else {
        FlatPort::releaseVc(vc);
    }
    vc.outVC = -1;
    vc.pipelineLatency = pipelineLatency;
}

SourceFlatPort::~SourceFlatPort() {
    // TODO Auto-generated destructor stub
}

void SourceFlatPort::handleVCClaim(vcState state, vc_t *accepting, NoCFlitMsg* msg, FlatPortIfc *outPort) {
    if(accepting->flits.size() == 1) {
        FlatPort::handleVCClaim(state, accepting, msg,outPort);
    }
}
