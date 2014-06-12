/*
 * Utils.cpp
 *
 *  Created on: Jan 5, 2013
 *      Author: doron
 */

#include "Utils.h"

Utils::Utils() {
    // TODO Auto-generated constructor stub

}

Utils::~Utils() {
    // TODO Auto-generated destructor stub
}

bool Utils::isHead(NoCFlitMsg* msg) {
        NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg->getType();
        return type==NOC_START_FLIT;
    }

    bool Utils::isTail(NoCFlitMsg* msg) {
        NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg->getType();
        return type==NOC_END_FLIT;
    }

    bool Utils::isHead(NoCFlitMsg& msg) {
        NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg.getType();
        return type==NOC_START_FLIT;
    }


    bool Utils::isTail(NoCFlitMsg& msg) {
        NOC_FLIT_TYPES type = (NOC_FLIT_TYPES)msg.getType();
        return type==NOC_END_FLIT;
    }

    inPortFlitInfo* Utils::getFlitInfo(NoCFlitMsg* msg) {
        cObject *obj = msg->getControlInfo();
        if (obj == NULL) {
            throw cRuntimeError("-E- BUG - No Control Info for FLIT: %s",
                    msg->getFullName());
        }

        inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
        return info;
    }


/**
 * Class generated from <tt>CMP.msg</tt> by opp_msgc.
 * <pre>
 * packet CMPMsg extends AppMsg {
 *     int origin;
 *     int operation;
 *     int l2hit;
 *     int address;
 *     int length;
 *  bool roundtrip = false;
 * }
 * </pre>
 */
/**
 * Class generated from <tt>App.msg</tt> by opp_msgc.
 * <pre>
 * packet AppMsg
 * {
 *     int msgId;
 *     int srcAppId;
 *     int dstAppId;
 *     int appMsgLen;
 *     int pktLength;
 *     int dstId;
 *     int srcId;
 *     int VC;
 *     int origBytes;
 *     simtime_t InjectTime;
 * }
 * </pre>
 */
/* \code
* packet Datagram
* {
*     int destAddr = -1; // destination address
*     int srcAddr = -1;  // source address
*     int ttl =  32;     // time to live
* }
* \endcode */
ostream &operator<<(ostream& stream, cPacket *msg) {
    stream << "pktId: " << msg->getId() << "\n";
    return stream;
}

ostream &operator<<(ostream& stream, AppMsg *msg) {
    stream << "TO BE IMPLEMENTED";
    return stream;
}

ostream &operator<<(ostream& stream, CMPMsg *msg) {
    stream << "CMPMsg\n";
    stream << "CMPMsg ID: " << msg->getId() << " MSGId: "<< msg->getMsgId() << "\n";
    stream << "Origin: " << msg->getOrigin() << " OriginSrcAppId: " <<  msg->getSrcAppId() <<" SrcId: " << msg->getSrcId()<< "\n";
    stream << "DstId: " << msg->getDstId() << "\n";
    stream << "AppMsgLen (how many packets): " << msg->getAppMsgLen() << "\n";
    stream << "PktLength (how many flits per packet): " << msg->getPktLength() << "\n";
    stream << "VC: " << msg->getVC() ;
    return stream;
}
