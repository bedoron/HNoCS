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

#ifndef FLATPORT_H_
#define FLATPORT_H_
#include <cgate.h>
#include <queue>
#include <vector>
#include "Utils.h"
using std::queue;
using std::vector;


#define PIPELINE_LATENCY 1
class FlatPort;
class CentSchedRouter;

enum vcState { FREE, INTERNALY_TAKEN, EXTERNALY_TAKEN };

class FlatPortIfc;

struct vc_t {
   int id;
   int credit;
   bool delivering; // Is it currently delivering data, relevant only is state is not free
   int pktId;
   FlatPortIfc *outPort;
   int outVC;
   int pipelineLatency;
   queue<NoCFlitMsg*> flits;
   vcState state;
};

class FlatPortIfc {
public:
    virtual vc_t* acceptExternal(NoCFlitMsg* msg) = 0;
    virtual vc_t* acceptExternal(NoCCreditMsg* msg) = 0;

    virtual vc_t* acceptInternal(NoCFlitMsg* msg) = 0;
    virtual vc_t* acceptInternal(int vcNum, int credits) = 0;
    virtual vector<vcState> getVCStates() = 0;

    virtual void tickInner() = 0;
    virtual void tickOuter() = 0;

    virtual bool hasData() = 0;

    virtual ~FlatPortIfc() {};
};


class FlatPort: public FlatPortIfc {
    vector<FlatPortIfc*> &ports; // ports[id] is me!
    CentSchedRouter *router;
    cGate *gate;
    int routerId;
    FlatPortIfc *adjacent;
    int id;
    int pipelineLatency;
    vector<vc_t> vcs;

    int innerActiveVc;
    int outerActiveVc;

    void failIfCantAccept(vc_t *vc, NoCFlitMsg* msg);
    vc_t* acceptFlit(FlatPortIfc *outPort, NoCFlitMsg* msg, vcState state);


    void logIf(NoCFlitMsg *msg, int routerId, int portId, int vcId);
    void logIf(NoCFlitMsg *msg, int flitId);
    void logIfRouter(NoCFlitMsg *msg, int routerId);
    void logIfRouterPort(NoCFlitMsg *msg, int routerId, int port);

protected:
    virtual void electInnerActiveVc();
    virtual void electOuterActiveVc();

    virtual void setupExternalLinkIfNeeded(NoCFlitMsg* msg, vc_t& vc);
    virtual void setupInternalLinkIfNeeded(NoCFlitMsg* msg, FlatPortIfc* outPort, vc_t& vc);

    virtual bool vcCanAccept(vc_t *vc, NoCFlitMsg* msg);
    virtual void releaseVc(vc_t& vc);
public:
    FlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPortIfc*> &allPorts, int numVcs, int pipelineLatency);
    virtual vc_t* acceptExternal(NoCFlitMsg* msg);
    virtual vc_t* acceptExternal(NoCCreditMsg* msg);

    virtual vc_t* acceptInternal(NoCFlitMsg* msg);
    virtual vc_t* acceptInternal(int vcNum, int credits);
    virtual vector<vcState> getVCStates();

    virtual void tickInner();
    virtual void tickOuter();


    virtual bool hasData();
    virtual ~FlatPort();

};

#endif /* FLATPORT_H_ */
