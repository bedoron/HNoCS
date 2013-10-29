/*
 * Utils.cpp
 *
 *  Created on: Jan 5, 2013
 *      Author: doron
 */

#include "Utils.h"
#include "ResponseDB.h"
#include <cmodule.h>
#include <csimulation.h>

#include <streambuf>
#include <string>


using std::stringstream;
using std::string;


Utils::Utils() {
    // TODO Auto-generated constructor stub

}

Utils::~Utils() {
    // TODO Auto-generated destructor stub
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

ostream& operator <<(ostream& stream, NoCFlitMsg* msg) {
    long int flitId = msg->getId();
    stream << "**************** Flit " << flitId << " *******************\n";
    stream << "it's type is ";
     switch(msg->getType()) {
     case NOC_START_FLIT:    stream << "NOC_START_FLIT";   break;
     case NOC_END_FLIT:      stream << "NOC_END_FLIT";     break;
     case NOC_MID_FLIT:      stream << "NOC_MID_FLIT";     break;
     }

     stream << "-";
     SessionMeta *meta = ResponseDB::getInstance()->find(flitId);

     if(NULL == meta) {
         stream << "[NOT REGISTERED]";
     } else {
         if(meta->isRequest(flitId)) {
             stream << "[Request]";
         } else { stream << "[Response]"; }
     }
     stream << "\n";


     if(NULL != meta) {
         stream << "Session ID: " << meta->getSessionId();
     } else {
         stream << "Pkt Id:    "<< msg->getPktId() << "\n";
         stream << "Flit Idx:  " << msg->getFlitIdx();
     }

     stream << "\n";

     int src = msg->getSrcId();
     int dst = msg->getDstId();


     stringstream ss_src, ss_dst;

     ss_src << "core[" << src << "]";
     ss_dst << "core[" << dst << "]";


     cModule *srcCore = cSimulation::getActiveSimulation()->getModuleByPath(ss_src.str().c_str());
     cModule *dstCore = cSimulation::getActiveSimulation()->getModuleByPath(ss_dst.str().c_str());

     if(srcCore != 0) {
         ss_src.str(string());
         ss_src << srcCore->par("appType").stringValue();
     }

     if(dstCore != 0) {
         ss_dst.str(string());
         ss_dst << dstCore->par("appType").stringValue();
     }

     stream << "Source:    " << src << " " << ss_src.str() << "\n";
     stream << "Dest:      " << dst << " " << ss_dst.str() << "\n";

     stream << "*********************************************\n";

     return stream;
}
