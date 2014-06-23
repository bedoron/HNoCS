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

#ifndef SOURCEFLATPORT_H_
#define SOURCEFLATPORT_H_
#include "FlatPort.h"

class SourceFlatPort: public FlatPort {
    vector<FlatPortIfc*> &ports; // ports[id] is me!
    CentSchedRouter *theRouter;
    int id;
    int pipelineLatency;
    int routerId;

protected:
    virtual bool vcCanAccept(vc_t *vc, NoCFlitMsg* msg);
    virtual void releaseVc(vc_t& vc);
public:
    SourceFlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*> &allPorts, int numVcs, int pipelineLatency);
    virtual void handleVCClaim(vcState state, vc_t *accepting, NoCFlitMsg* msg, FlatPortIfc *outPort);
    virtual int getId() { return id; };
    virtual ~SourceFlatPort();
};

#endif /* SOURCEFLATPORT_H_ */
