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

package hnocs.cores;

//
// Message Source with Packet Fifo's
//
simple EchoApp like App_Ifc
{
    parameters:
        int appId; 							// Application Id unique within the Core
        int srcId;                          // local id
        int VC;                             // the VC to be used for packets
        volatile int dstId;                 // the packet destination 
        volatile int dstAppId;              // the application destination    
        volatile int pktLen;                // packet length in FLITs
        volatile int msgLen;                // how many packets will be sent to same dst 
        volatile double msgDelay @unit(s);  // generation delay between messages
        double statStartTime @unit(s);      // start time for recording statistics

        @display("i=block/telnet,#FFFF80");
    gates:
        inout out;
        inout in;
}
