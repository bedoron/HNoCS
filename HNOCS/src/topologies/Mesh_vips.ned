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

// Created: 2010-2-30 for project iNoC

package hnocs.topologies;

import hnocs.routers.Router_Ifc;
import hnocs.cores.NI_Ifc;

//import ned.DelayChannel;

//
// A generated network with grid topology.
//
    network Mesh_vips
{
    parameters:
        string routerType;
        string coreType;
        int columns = default(4);
        int rows = default(4);
    submodules:
        router[columns*rows]: <routerType> like Router_Ifc {
            parameters:
                numPorts = 5;
                id = index;
                @display("p=100,100,matrix,$columns,150,150");
            gates:
                in[5];
                out[5];
        }
        core[columns*rows]: <coreType> like NI_Ifc {
            parameters:
                id = index;
                @display("p=150,150,matrix,$columns,150,150");
        }

    connections allowunconnected:

    router[0].in[4] <-->{datarate=16Gbps;} <--> core[0].out;
router[0].out[4] <-->{datarate=16Gbps;} <--> core[0].in;
router[0].out[3] <-->{datarate=16Gbps;}<-->router[1].in[1];
router[0].out[2] <-->{datarate=16Gbps;}<-->router[4].in[0];
router[4].in[4] <-->{datarate=16Gbps;} <--> core[4].out;
router[4].out[4] <-->{datarate=16Gbps;} <--> core[4].in;
router[4].out[0] <-->{datarate=16Gbps;}<-->router[0].in[2];
router[4].out[3] <-->{datarate=16Gbps;}<-->router[5].in[1];
router[4].out[2] <-->{datarate=16Gbps;}<-->router[8].in[0];
router[8].in[4] <-->{datarate=16Gbps;} <--> core[8].out;
router[8].out[4] <-->{datarate=16Gbps;} <--> core[8].in;
router[8].out[0] <-->{datarate=16Gbps;}<-->router[4].in[2];
router[8].out[3] <-->{datarate=16Gbps;}<-->router[9].in[1];
router[8].out[2] <-->{datarate=16Gbps;}<-->router[12].in[0];
router[12].in[4] <-->{datarate=16Gbps;} <--> core[12].out;
router[12].out[4] <-->{datarate=16Gbps;} <--> core[12].in;
router[12].out[0] <-->{datarate=16Gbps;}<-->router[8].in[2];
router[12].out[3] <-->{datarate=16Gbps;}<-->router[13].in[1];
router[1].in[4] <-->{datarate=16Gbps;} <--> core[1].out;
router[1].out[4] <-->{datarate=16Gbps;} <--> core[1].in;
router[1].out[1] <-->{datarate=16Gbps;}<-->router[0].in[3];
router[1].out[3] <-->{datarate=16Gbps;}<-->router[2].in[1];
router[1].out[2] <-->{datarate=16Gbps;}<-->router[5].in[0];
router[5].in[4] <-->{datarate=16Gbps;} <--> core[5].out;
router[5].out[4] <-->{datarate=16Gbps;} <--> core[5].in;
router[5].out[0] <-->{datarate=16Gbps;}<-->router[1].in[2];
router[5].out[1] <-->{datarate=16Gbps;}<-->router[4].in[3];
router[5].out[3] <-->{datarate=16Gbps;}<-->router[6].in[1];
router[5].out[2] <-->{datarate=16Gbps;}<-->router[9].in[0];
router[9].in[4] <-->{datarate=16Gbps;} <--> core[9].out;
router[9].out[4] <-->{datarate=16Gbps;} <--> core[9].in;
router[9].out[0] <-->{datarate=16Gbps;}<-->router[5].in[2];
router[9].out[1] <-->{datarate=16Gbps;}<-->router[8].in[3];
router[9].out[3] <-->{datarate=16Gbps;}<-->router[10].in[1];
router[9].out[2] <-->{datarate=16Gbps;}<-->router[13].in[0];
router[13].in[4] <-->{datarate=16Gbps;} <--> core[13].out;
router[13].out[4] <-->{datarate=16Gbps;} <--> core[13].in;
router[13].out[0] <-->{datarate=16Gbps;}<-->router[9].in[2];
router[13].out[1] <-->{datarate=16Gbps;}<-->router[12].in[3];
router[13].out[3] <-->{datarate=16Gbps;}<-->router[14].in[1];
router[2].in[4] <-->{datarate=16Gbps;} <--> core[2].out;
router[2].out[4] <-->{datarate=16Gbps;} <--> core[2].in;
router[2].out[1] <-->{datarate=16Gbps;}<-->router[1].in[3];
router[2].out[3] <-->{datarate=16Gbps;}<-->router[3].in[1];
router[2].out[2] <-->{datarate=16Gbps;}<-->router[6].in[0];
router[6].in[4] <-->{datarate=16Gbps;} <--> core[6].out;
router[6].out[4] <-->{datarate=16Gbps;} <--> core[6].in;
router[6].out[0] <-->{datarate=16Gbps;}<-->router[2].in[2];
router[6].out[1] <-->{datarate=16Gbps;}<-->router[5].in[3];
router[6].out[3] <-->{datarate=16Gbps;}<-->router[7].in[1];
router[6].out[2] <-->{datarate=16Gbps;}<-->router[10].in[0];
router[10].in[4] <-->{datarate=16Gbps;} <--> core[10].out;
router[10].out[4] <-->{datarate=16Gbps;} <--> core[10].in;
router[10].out[0] <-->{datarate=16Gbps;}<-->router[6].in[2];
router[10].out[1] <-->{datarate=16Gbps;}<-->router[9].in[3];
router[10].out[3] <-->{datarate=16Gbps;}<-->router[11].in[1];
router[10].out[2] <-->{datarate=16Gbps;}<-->router[14].in[0];
router[14].in[4] <-->{datarate=16Gbps;} <--> core[14].out;
router[14].out[4] <-->{datarate=16Gbps;} <--> core[14].in;
router[14].out[0] <-->{datarate=16Gbps;}<-->router[10].in[2];
router[14].out[1] <-->{datarate=16Gbps;}<-->router[13].in[3];
router[14].out[3] <-->{datarate=16Gbps;}<-->router[15].in[1];
router[3].in[4] <-->{datarate=16Gbps;} <--> core[3].out;
router[3].out[4] <-->{datarate=16Gbps;} <--> core[3].in;
router[3].out[1] <-->{datarate=16Gbps;}<-->router[2].in[3];
router[3].out[2] <-->{datarate=16Gbps;}<-->router[7].in[0];
router[7].in[4] <-->{datarate=16Gbps;} <--> core[7].out;
router[7].out[4] <-->{datarate=16Gbps;} <--> core[7].in;
router[7].out[0] <-->{datarate=16Gbps;}<-->router[3].in[2];
router[7].out[1] <-->{datarate=16Gbps;}<-->router[6].in[3];
router[7].out[2] <-->{datarate=16Gbps;}<-->router[11].in[0];
router[11].in[4] <-->{datarate=16Gbps;} <--> core[11].out;
router[11].out[4] <-->{datarate=16Gbps;} <--> core[11].in;
router[11].out[0] <-->{datarate=16Gbps;}<-->router[7].in[2];
router[11].out[1] <-->{datarate=16Gbps;}<-->router[10].in[3];
router[11].out[2] <-->{datarate=16Gbps;}<-->router[15].in[0];
router[15].in[4] <-->{datarate=16Gbps;} <--> core[15].out;
router[15].out[4] <-->{datarate=16Gbps;} <--> core[15].in;
router[15].out[0] <-->{datarate=16Gbps;}<-->router[11].in[2];
router[15].out[1] <-->{datarate=16Gbps;}<-->router[14].in[3];
}