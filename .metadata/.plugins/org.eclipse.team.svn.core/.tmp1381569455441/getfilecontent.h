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

#ifndef DEFAULTPREDICTOR_H_
#define DEFAULTPREDICTOR_H_

#include "PredictorIfc.h"
#include <string>
using std::string;

class DefaultPredictor: public PredictorIfc {
    string m_method;

public:
    DefaultPredictor(const string& method);
    virtual ~DefaultPredictor();

    // Interface
    SimTime predict(CMPMsg *msg);
    const string &getName() const;
};

#endif /* DEFAULTPREDICTOR_H_ */
