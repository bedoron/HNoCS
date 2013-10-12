//
// Copyright (C) 2011 Eitan Zahavi, The Technion EE Department
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

package hnocs.simulations.app2app;
import hnocs.cores.AppNI;
import ned.DelayChannel;

// we use same Link channel for the mesh
channel Link extends ned.DatarateChannel
{
    datarate = 16Gbps; // 32 lines of 2ns clock
    delay = 0us;
}

network Apps2NoC2Apps
{
    submodules:
        AppNI1: AppNI {
            parameters:
                id = 0;
                numApps = 2;
            @display("p=126,96");
        }
        AppNI2: AppNI {
            parameters:
                id = 1;
                numApps = 2;
            @display("p=288,96");
        }
    connections:
        AppNI1.out <--> Link <--> AppNI2.in;
        AppNI1.in <--> Link <--> AppNI2.out;
}