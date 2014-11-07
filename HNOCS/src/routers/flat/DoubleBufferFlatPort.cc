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

#include <DoubleBufferFlatPort.h>
#include "SourceFlatPort.h"
#include <iostream>

#include <string>
using std::cerr;
using std::string;

DoubleBufferFlatPort::DoubleBufferFlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*>& allPorts, int numVcs, int pipelineLatency, int flitsPerVC) {
    id = gate->getIndex();
    routerId = ((cModule*)router)->getIndex();
    // Initialize internal acceptor because its not being bootstrapped
    internalAcceptor = new FlatPort(router,gate, allPorts, numVcs, pipelineLatency);

    for(int i=0; i < numVcs; ++i) {
        internalAcceptor->acceptInternal(i, flitsPerVC);
    }

    externalAcceptor = new SourceFlatPort(router,gate, allPorts, numVcs, pipelineLatency);//new FlatPort(router,gate, allPorts, numVcs, pipelineLatency);
}


void DoubleBufferFlatPort::watchdog(SimTime timeout) {
    if (internalAcceptor != NULL) {
        internalAcceptor->watchdog(timeout);
    }
    if (externalAcceptor != NULL) {
        externalAcceptor->watchdog(timeout);
    }
}

vc_t* DoubleBufferFlatPort::acceptExternal(NoCFlitMsg* msg) {
    try {
        return externalAcceptor->acceptExternal(msg);
    } catch (cRuntimeError ex) {
        ex.prependMessage("acceptExt::DB-EXTERNAL:");
        throw ex;
    }
}

vc_t* DoubleBufferFlatPort::acceptExternal(NoCCreditMsg* msg) {
    if(strstr(msg->getFullName(), "[EXT]")!=NULL) {
     try {
        return externalAcceptor->acceptExternal(msg); //Sink's credit message goes to the internal acceptor
    } catch (cRuntimeError ex) {
        ex.prependMessage("DB-EXTERNAL:");
        throw ex;
    }
    } else {
        try {
        return internalAcceptor->acceptExternal(msg); //Sink's credit message goes to the internal acceptor
    } catch (cRuntimeError ex) {
        ex.prependMessage("DB-INTERNAL:");
        throw ex;
    }
    }
}

vc_t* DoubleBufferFlatPort::acceptInternal(NoCFlitMsg* msg) {
 try {
    return internalAcceptor->acceptInternal(msg);
} catch (cRuntimeError ex) {
    ex.prependMessage("DB-INTERNAL:");
    throw ex;
}
}

vc_t* DoubleBufferFlatPort::acceptInternal(int vcNum, int credits) {
 try{
    return externalAcceptor->acceptInternal(vcNum, credits);
} catch (cRuntimeError ex) {
    ex.prependMessage("DB-EXTERNAL:");
    throw ex;
}
}

vector<vcState> DoubleBufferFlatPort::getVCStates() {
 try {
    return internalAcceptor->getVCStates();
} catch (cRuntimeError ex) {
    ex.prependMessage("DB-INTERNAL:");
    throw ex;
}
}

void DoubleBufferFlatPort::tickInner() {
 try {
    externalAcceptor->tickInner();
} catch (cRuntimeError ex) {
    ex.prependMessage("DB-EXTERNAL:");
    throw ex;
}
}

void DoubleBufferFlatPort::tickOuter() {
    try{
    internalAcceptor->tickOuter();
    } catch (cRuntimeError ex) {
        ex.prependMessage("DB-INTERNAL:");
        throw ex;
    }
}

bool DoubleBufferFlatPort::hasData() {
    return internalAcceptor->hasData() || externalAcceptor->hasData();
}

bool DoubleBufferFlatPort::hasCredits(int vc) {
    return internalAcceptor->hasCredits(vc);
}

DoubleBufferFlatPort::~DoubleBufferFlatPort() {
    delete internalAcceptor;
    delete externalAcceptor;
}

bool DoubleBufferFlatPort::reserveVC(int vcNum, vcState intext) {
    // not connected to anything...
    return internalAcceptor->reserveVC(vcNum, intext);
}

void DoubleBufferFlatPort::handleVCClaim(vcState state, vc_t *accepting, NoCFlitMsg* msg, FlatPortIfc *outPort) {
    throw cRuntimeError("Dont use double buffer's flat port vc claim");
}
