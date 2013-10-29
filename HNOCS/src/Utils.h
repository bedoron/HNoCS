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
using std::ostream;
#ifndef UTILS_H_
#define UTILS_H_


class Utils {
public:
    Utils();
    virtual ~Utils();
};

// Global "nice" printers
ostream &operator<<(ostream& stream, cPacket *msg);
ostream &operator<<(ostream& stream, AppMsg *msg);
ostream &operator<<(ostream& stream, CMPMsg *msg);
ostream &operator<<(ostream& stream, NoCFlitMsg *msg);




#endif /* UTILS_H_ */
