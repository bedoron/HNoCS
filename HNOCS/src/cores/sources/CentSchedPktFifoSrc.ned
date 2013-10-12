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
// Message Source with Packet Fifo's for GANA
//
simple CentSchedPktFifoSrc like CSource_Ifc 
{
    parameters:
        int             srcId;                       // must be globally unique
        volatile int    pktVC;                       // the VC to be used for packets
        volatile int    dstId;                       // the packet destination 
        volatile int    pktLen;                      // packet length in FLITs
        volatile int 	msgLen;                      // how many packets will be sent to same dst     
        volatile double flitArrivalDelay @unit(s);   // Inter Flit delay sec
        int             flitSize @unit(byte);        // FLIT size
        int             maxQueuedPkts;               // Max number of packets that can be queued
    @display("i=block/source");
    gates: 
        inout out;
        inout center;
}
