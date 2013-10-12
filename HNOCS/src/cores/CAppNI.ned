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

import hnocs.cores.sources.CAppSrc;
import hnocs.cores.sinks.AppSink;
import hnocs.cores.App_Ifc;

module CAppNI like CNI_Ifc
{
    parameters:
        int id;
        int numApps;
        string appType;
        @display("i=block/square");
    gates:
        inout in;
        inout out;
        inout center;

    submodules:
        source: CAppSrc {
            parameters:
                srcId = id;
                numApps = numApps;
                @display("p=51,101");
        }
        sink: AppSink {
            @display("p=250,100");
        }
        app[numApps]: <appType> like App_Ifc {
            parameters:
                appId = index;
                srcId = id;
                dstAppId = (index + 1) % numApps;
                @display("p=150,100,c,150");
        }

    connections:
        source.center <--> center;
        source.out <--> out;
        sink.in <--> in;
        for a=0..numApps-1 {
            app[a].out <--> source.in++;
            app[a].in <--> sink.app++;
        }
}
