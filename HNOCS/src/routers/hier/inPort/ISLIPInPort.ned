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

package hnocs.routers.hier.inPort;

//
// ISLIP Input Port
//
simple ISLIPInPort like InPort_Ifc
{
    parameters:
        int numVCs;     // number of supported VCs
        int flitsPerVC; // number of buffers available per VC
        int numParallelSends; //  number of parallel FLITs that can be sent out
    @display("i=block/subqueue");
    gates:
        inout  in;		// inport
        output out[];  	// connected to sw for sending flits
        inout  ctrl[]; 	// connected to sw for sending req, ack and receiving gnt 
        inout calcVc;	// calculates outputVC
        inout calcOp; 	// calculate out port
}
