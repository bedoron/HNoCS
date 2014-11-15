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

#ifndef DOUBLEBUFFERFLATPORT_H_
#define DOUBLEBUFFERFLATPORT_H_

#include <FlatPort.h>

class DoubleBufferFlatPort: public FlatPortIfc {
    int id;
    int routerId;
    FlatPort *internalAcceptor;
    FlatPort *externalAcceptor;
    int msgs;
public:
    DoubleBufferFlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*> &allPorts, int numVcs, int pipelineLatency, int flitsPerVC);

    vc_t* acceptExternal(NoCFlitMsg* msg);
    vc_t* acceptExternal(NoCCreditMsg* msg);

    vc_t* acceptInternal(NoCFlitMsg* msg);
    vc_t* acceptInternal(int vcNum, int credits);
    vector<vcState> getVCStates();

    void tickInner();
    void tickOuter();

    bool reserveVC(int vcNum, vcState intext);
    bool hasData();
    virtual bool hasCredits(int vc);

    virtual void handleVCClaim(vcState state, vc_t *accepting, NoCFlitMsg* msg, FlatPortIfc *outPort);
    virtual int getId() { return id; }

    virtual void watchdog(SimTime timeout);

    virtual ~DoubleBufferFlatPort();
    virtual void showDiffs();
};

#endif /* DOUBLEBUFFERFLATPORT_H_ */
