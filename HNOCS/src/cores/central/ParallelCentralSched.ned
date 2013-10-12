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
// Same interface as CentralSched - but with some parameters differences
//
simple ParallelCentralSched like Center_Ifc
{
    parameters:
        int rows;
        int columns;
        double tClk @unit(s); // the clock cycle time of the scheduler
        bool ackEverything;   // make the sched immediatly approve every request
        int schedClks;        // number of clocks between scheduling (or how many clocks it takes)
    @display("i=misc/sun");
    gates:
        inout center[]; 
}
