/*
 * Utils.h
 *
 *  Created on: Jan 5, 2013
 *      Author: doron
 */
#include "cmessage.h"
#include "App_m.h"
#include "CMP_m.h"
#include "NoCs_m.h"
#include <iostream>
#include "hier/FlitMsgCtrl.h"
using std::ostream;
#ifndef UTILS_H_
#define UTILS_H_


class Utils {
public:
    Utils();
    virtual ~Utils();

    static bool isHead(NoCFlitMsg *msg);
    static bool isTail(NoCFlitMsg *msg);
    static bool isHead(NoCFlitMsg& msg);
    static bool isTail(NoCFlitMsg& msg);
    static const char* getTypeName(NoCFlitMsg *msg);
    static const char* getTypeName(NoCFlitMsg& msg);

    static inPortFlitInfo* getFlitInfo(NoCFlitMsg* msg);
    static bool hasFlitInfo(NoCFlitMsg* msg);
};

// Global "nice" printers
ostream &operator<<(ostream& stream, cPacket *msg);
ostream &operator<<(ostream& stream, AppMsg *msg);
ostream &operator<<(ostream& stream, CMPMsg *msg);
ostream &operator<<(ostream& stream, NoCFlitMsg *msg);
#endif /* UTILS_H_ */

