/*
 * PredictorIfc.cc
 *
 *  Created on: Nov 2, 2013
 *      Author: doron
 */
#include "PredictorIfc.h"

simtime_t PredictorIfc::Now() {
    return cSimulation::getActiveSimulation()->getSimTime();
}


PredictorIfc::PredictorIfc(const char *method): m_method(method) {}
const string &PredictorIfc::getName() const { return m_method; }

Resolution PredictorIfc::onStartFlow(AppFlitMsg* msg, SessionMeta* meta) {
}

Resolution PredictorIfc::onEndFlow(AppFlitMsg* msg, SessionMeta* meta) {
}

Resolution PredictorIfc::onMidFlow(AppFlitMsg* msg, SessionMeta* meta) {
}

Resolution PredictorIfc::onFlit(AppFlitMsg* msg, SessionMeta* meta) {
}

Resolution PredictorIfc::checkFlit(NoCFlitMsg *msg, SessionMeta *meta = 0) {
    Resolution res = PREDICTION_IGNORE;
    AppFlitMsg *flit = (AppFlitMsg*)msg;

    int flitType = flit->getType();
    int pktIdx   = flit->getPktIdx();
    int totalPkt = flit->getAppMsgLen();

    if(flitType!=NOC_MID_FLIT) {

        if(NULL == meta) {
            if(NOC_START_FLIT == flitType) {
                meta = ResponseDB::getInstance()->find(msg);
            } else {
                meta = ResponseDB::getInstance()->findBySessionId(flit->getSessionId());
            }
        }

        if(NULL != meta) {
            bool firstPacket = (0 == pktIdx);
            bool lastPacket = ((totalPkt-1) == pktIdx);

            if(PREDICTION_IGNORE != onFlit(flit, meta)) {
                if(firstPacket) {
                    res = onStartFlow(flit, meta);
                }
                if(lastPacket) {
                    res = onEndFlow(flit, meta);
                }
                if(!(firstPacket || lastPacket)) {
                    res = onMidFlow(flit, meta);
                }
            }
        } else { /* Maybe its a write request ? */ }
    } else {
        cerr << "MID FLIT in PREDICTOR OBJECT\n";
    }

    return res;
}
