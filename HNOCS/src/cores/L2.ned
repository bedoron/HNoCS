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

package hnocs.cores;

//
// L2 Cache Module
//
simple L2 like App_Ifc
{
    parameters:
        int appId;    // must be unique in the Core
        int srcId;    // local srcId
        int dramId;   // we assume DRAM appId is 0 
        volatile int dstAppId;      // destination app
        double l2HitLatency @unit(s);   
        int cacheLinePackets;  // response num packets
    	int cacheLinePktFlits; // response packet size
        int cachePagePackets;  // number of packets in cache page
        int cachePagePktFlits;  // number of flits in each cache packet
        
        @display("i=block/telnet");
    gates:
        inout out;
        inout in;
}
