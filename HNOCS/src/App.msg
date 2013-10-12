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

cplusplus {{
#include <NoCs_m.h>
}}
packet AppMsg;

class NoCFlitMsg;

// An application adds to the basic FLIT metadata about the source and destination APP indexes
// note that the App packet itself will be encapsulated into the last flit of the app message
packet AppFlitMsg extends NoCFlitMsg {
    int srcAppId; // the source application ID
    int dstAppId; // the destination application ID
    int pktIdx;   // packet index within the message
    int appMsgLen;   // message length in packets
    int msgId;    // the message id (unique for the src/dst and src-app/dst-app pairs)
}

// these kind of messages are exchanged between the application and NI 
// every application may subclass to actually add payload
packet AppMsg
{
    int msgId;     // the message id (unique for the src/dst and src-app/dst-app pairs)
    int srcAppId;
    int dstAppId;
    int appMsgLen; // in packets
    int pktLength; // in flits
    int dstId;
    int srcId;
    int VC;        // ??? a requested VC for this message
    int origBytes; // carry the original length in bytes
    simtime_t InjectTime;
}

packet AppTestMsg extends AppMsg
{
    string payload;
}
