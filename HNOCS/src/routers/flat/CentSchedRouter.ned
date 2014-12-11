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

package hnocs.routers.flat;

//
// A simple router assuming all scheduling done elsewhere.
// Forward packets by their destination using XY routing only 
//
module CentSchedRouter like hnocs.routers.Router_Ifc
{
    parameters:
        int numPorts; // number of ports on this router
        int id; // serve as a global identifier for routing etc
        int rows;
        int columns;
        int dataRate = 16000; // 16 Gigabits per second - speed is in MBps
        int flitsPerVC;
        int numVCs = default(1);
        int flitSize @unit(byte);   // flitSize [bytes]
        int pipelineDepth = default(1);
        string coreType;
        string routerType;
        string predictorType = default("Random");
 	@display("i=block/broadcast");   
    gates:
        inout in[];
        inout out[];
}
