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

#ifndef INTERROUTERMATCHERS_H_
#define INTERROUTERMATCHERS_H_
#include "InterRouterMatcher.h"
#include "CentSchedRouter.h"

class InterRouterCoreMatcher: public InterRouterMatcher {
    CentSchedRouter *requester;
    cGate *gate;
    vc_t &vc;
public:
    InterRouterCoreMatcher(CentSchedRouter* requester, cGate* gate, vc_t& vc);
    virtual ~InterRouterCoreMatcher();
    virtual int getFreeVC();
};

class InterRouterRouterMatcher: public InterRouterMatcher {
    CentSchedRouter* requestee;
    cGate* otherGate;
public:
    InterRouterRouterMatcher(CentSchedRouter* requester, cGate* gate, vc_t& vc);
    virtual ~InterRouterRouterMatcher();
    virtual int getFreeVC();
};


class InterRouterMatchers {
    static const int CORE_PORT = 4;
public:
    static InterRouterMatcher* build(CentSchedRouter* requester, cGate* gate, vc_t& vc);
};

#endif /* INTERROUTERMATCHERS_H_ */
