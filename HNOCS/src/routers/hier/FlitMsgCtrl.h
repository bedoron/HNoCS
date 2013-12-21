//
// Copyright (C) 2010-2011 Eitan Zahavi, The Technion EE Department
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

#ifndef __HNOCS_FLIT_MSG_CTRL_H_
#define __HNOCS_FLIT_MSG_CTRL_H_
#include <omnetpp.h>
// we need extra info inside the InPort for tracking FLITs
class inPortFlitInfo : public cObject {
public:
	int inVC; // the input VC the FLIT arrived on
	int outPort; // the out port assigned to the FLIT

	inPortFlitInfo(): inVC(-1), outPort(-1) {}

	inPortFlitInfo(const inPortFlitInfo* src) {
	    if(src==NULL) {
	        throw new cRuntimeError("Trying to replicate a null inPortFlitInfo");
	    }
	    inVC = src->inVC;
	    outPort = src->outPort;

	}
};

#endif /* __HNOCS_FLIT_MSG_CTRL_H_ */
