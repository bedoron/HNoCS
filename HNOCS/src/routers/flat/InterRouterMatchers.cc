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

#include <InterRouterMatchers.h>
#include <algorithm>
using std::find;
using std::cerr;

InterRouterMatcher* InterRouterMatchers::build(CentSchedRouter* requester, cGate* gate, vc_t& vc) {
    if(gate->getIndex()==CORE_PORT) {
        return new InterRouterCoreMatcher(requester, gate, vc);
    } else {
        return new InterRouterRouterMatcher(requester, gate, vc);
    }
}

InterRouterCoreMatcher::InterRouterCoreMatcher(CentSchedRouter* requester, cGate* gate, vc_t& vc): requester(requester), gate(gate), vc(vc) {
}

InterRouterCoreMatcher::~InterRouterCoreMatcher() {
}

int InterRouterCoreMatcher::getFreeVC() {
//    int connectedGate = gate->getIndex();
//    vector<vcState> states = requester->requestPort(connectedGate)->getVCStates();
//    unsigned int outVCId = find(states.begin(), states.end(), FREE)
//            - states.begin();
//
//    cerr << "Size of states for router " << requester->getIndex() << " is " << states.size() << "\n";
//
//    return (outVCId==states.size())?-1:outVCId;
    return vc.id;
}

InterRouterRouterMatcher::InterRouterRouterMatcher(CentSchedRouter* requester, cGate* gate, vc_t& vc) {
    requestee = (CentSchedRouter*)(gate->getPathEndGate()->getOwnerModule());
    otherGate = gate->getPathEndGate();
}

InterRouterRouterMatcher::~InterRouterRouterMatcher() {
}

int InterRouterRouterMatcher::getFreeVC() {
    int connectedGate = otherGate->getIndex();
    vector<vcState> states = requestee->requestPort(connectedGate)->getVCStates();
    unsigned int outVCId = find(states.begin(), states.end(), FREE)
            - states.begin();
    int selectedVC = (outVCId==states.size())?-1:outVCId;

    if(selectedVC != -1) {
        requestee->requestPort(connectedGate)->reserveVC(selectedVC);
    }

    return selectedVC;
}
