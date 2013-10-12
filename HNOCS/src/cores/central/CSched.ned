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

package hnocs.cores.central;

//
// Centralized Scheduler
//
simple CSched like Center_Ifc
{
    parameters:
        int rows;
        int columns;
        double tClk @unit(s); // the clock cycle time of the scheduler
        bool ackEverything = false; // make the sched immediatly approve every request
        bool usePathReservation = true; // if true marks un-allocated paths as 
                                        // reserves preventing starvation of long paths
        int maxParallelReqs; // the number of reqs that can be handled in one cycle 0 = inf.
    @display("i=misc/sun");
    gates:
        inout center[]; 
}
