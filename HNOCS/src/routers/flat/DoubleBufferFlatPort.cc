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
#include <iostream>
using std::cerr;

DoubleBufferFlatPort::DoubleBufferFlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*>& allPorts, int numVcs, int pipelineLatency) {
    id = gate->getIndex();
    routerId = -1;
    internalAcceptor = new FlatPort(router,gate, allPorts, numVcs, pipelineLatency);
    externalAcceptor = new FlatPort(router,gate, allPorts, numVcs, pipelineLatency);
}

vc_t* DoubleBufferFlatPort::acceptExternal(NoCFlitMsg* msg) {
    cerr << "Accepting message " << msg->getFullName() << " on router " << routerId << " port " << id << "\n";
    return externalAcceptor->acceptExternal(msg);
}

vc_t* DoubleBufferFlatPort::acceptExternal(NoCCreditMsg* msg) {
    return internalAcceptor->acceptExternal(msg); //Sink's credit message goes to the internal acceptor
}

vc_t* DoubleBufferFlatPort::acceptInternal(NoCFlitMsg* msg) {
    return internalAcceptor->acceptInternal(msg);
}

vc_t* DoubleBufferFlatPort::acceptInternal(int vcNum, int credits) {
    return internalAcceptor->acceptInternal(vcNum, credits);
}

vector<vcState> DoubleBufferFlatPort::getVCStates() {
    return internalAcceptor->getVCStates();
}

void DoubleBufferFlatPort::tickInner() {
    externalAcceptor->tickInner();
}

void DoubleBufferFlatPort::tickOuter() {
    internalAcceptor->tickOuter();
}

bool DoubleBufferFlatPort::hasData() {
    return internalAcceptor->hasData() || externalAcceptor->hasData();
}

DoubleBufferFlatPort::~DoubleBufferFlatPort() {
    delete internalAcceptor;
    delete externalAcceptor;
}
