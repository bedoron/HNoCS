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

// Test two EchoApp back to back
package hnocs.simulations.app2app;

import hnocs.cores.App_Ifc;

network App2app
{
    parameters:
        string appType;
    submodules:
        App1: <appType> like App_Ifc {
            @display("p=126,96");
        }
        App2: <appType> like App_Ifc {
            @display("p=288,96");
        }
    connections:
        App1.out <--> App2.in;
        App1.in <--> App2.out;
}
