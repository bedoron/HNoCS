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
// Changes: 14/11/2012: 
// Ben Elazar Doron b.e.doron@gmail.com, Yevgeny Dibrov yevgenydibrov@gmail.com
// Added boolean tag to mark cmp messages

// This file define the messages traversing a CMP
// 

cplusplus {{
#include "App_m.h"
}}
packet AppMsg;

enum cmpOpType { CMP_OP_READ = 1; CMP_OP_WRITE = 2; };
//
// TODO generated message class
//
packet CMPMsg extends AppMsg {
    int origin;     // 1=L1, 2=L2, 3=DRAM        
    int operation;  // CMP_OP_TYPE
    int l2hit;      // non zero means the L2 is hit
    int address;
    int length;     // in bytes
	bool roundtrip = false; // true if is a closed circle CMPMsg (request->response type) 
}
