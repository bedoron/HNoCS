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

struct vc_t {
   int id;
   int credit;
   bool delivering; // Is it currently delivering data, relevant only is state is not free
   int pktId;
   FlatPort *outPort;
   int outVC;
   int pipelineLatency;
   queue<NoCFlitMsg*> flits;
   vcState state;
};

class FlatPort {
    vector<FlatPort*> &ports; // ports[id] is me!
    CentSchedRouter *router;
    cGate *gate;
    int routerId;
    FlatPort *adjacent;
    int id;
    int pipelineLatency;
    vector<vc_t> vcs;

    int innerActiveVc;
    int outerActiveVc;

    bool vcCanAccept(vc_t *vc, NoCFlitMsg* msg);
    vc_t* acceptFlit(FlatPort *outPort, NoCFlitMsg* msg, vcState state);
    void setupInternalLinkIfNeeded(NoCFlitMsg* msg, FlatPort* outPort, vc_t& vc);
    void electInnerActiveVc();
    void electOuterActiveVc();

    void releaseVc(vc_t& vc);
public:

    FlatPort(CentSchedRouter* router, cGate* gate, vector<FlatPort*> &allPorts, int numVcs, int pipelineLatency);
    vc_t* acceptExternal(NoCFlitMsg* msg);
    vc_t* acceptExternal(NoCCreditMsg* msg);

    vc_t* acceptInternal(NoCFlitMsg* msg);
    vc_t* acceptInternal(int vcNum, int credits);
    vector<vcState> getVCStates();

    void tickInner();
    void tickOuter();


    bool hasData();
    virtual ~FlatPort();

};

#endif /* FLATPORT_H_ */
