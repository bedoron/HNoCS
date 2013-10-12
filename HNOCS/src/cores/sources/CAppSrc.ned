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

package hnocs.cores.sources;

//
// Central Scheduler NoC Application Source
//
simple CAppSrc
{
    parameters:
        int srcId;                      // must be globally unique
        int numVCs;
        int numApps;
        int arbitration_type;			// 0 = winner takes all 1 = change
        int flitSize @unit(byte);       // FLIT size
        double statStartTime @unit(s);  // start time for recording statistics
    @display("i=block/join");
    gates:
        inout in[];   // connected to application - receiving msgs
        inout out;    // drives the links between the AppSrc to the router
        inout center; // requests and grants to the central scheduler         
}
