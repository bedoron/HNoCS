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
    Resolution res = PREDICTION_IDLE;
    switch(meta->getState()) {
    case SESSION_META_REQUEST:
        if(NOC_START_FLIT == msg->getType()) {
            PredictionInterval interval = predict(msg, meta);
            addPrediction(msg, meta, interval);
            res = PREDICTION_CREATE;
        }
        break;
    case SESSION_META_RESPONSE:
        if(NOC_START_FLIT == msg->getType()) {
            res = checkPrediction(msg, meta);
        }
        break;
    }
    return res;
}

Resolution PredictorIfc::onEndFlow(AppFlitMsg* msg, SessionMeta* meta) {
    Resolution res = PREDICTION_IDLE;
    switch(meta->getState()) {
    case SESSION_META_REQUEST:
        /* Do nothing */
        break;
    case SESSION_META_RESPONSE:
        if(NOC_START_FLIT == msg->getType()) {
            res = checkPrediction(msg, meta);
        } else if(NOC_END_FLIT == msg->getType()) {
            /* Actual removal occurs in the event handler */
            res = PREDICTION_DESTROY;
        }
        break;
    }
    return res;
}

Resolution PredictorIfc::onMidFlow(AppFlitMsg* msg, SessionMeta* meta) {
    Resolution res = PREDICTION_IDLE;
    switch(meta->getState()) {
    case SESSION_META_REQUEST: break;
    case SESSION_META_RESPONSE:
        if(NOC_START_FLIT == msg->getType()) {
            res = checkPrediction(msg, meta);
        }
        break;
    }
    return res;
}

Resolution PredictorIfc::onFlit(AppFlitMsg* msg, SessionMeta* meta) {
    return PREDICTION_IDLE;
}

void PredictorIfc::callHandler(AppFlitMsg* msg, SessionMeta* meta,
        Resolution resolution) {
    switch(resolution) {
    case PREDICTION_HIT:    onHit(msg, meta); break;
    case PREDICTION_MISS:   onMiss(msg, meta); break;
    case PREDICTION_DESTROY:onDestroy(msg, meta); removePrediction(msg, meta); break;
    case PREDICTION_CREATE:
    case PREDICTION_IGNORE:
    case PREDICTION_IDLE:   break;
    default:
        cerr << "Unknown handler: " << resolution << "\n";
    }
}

Resolution PredictorIfc::checkPrediction(AppFlitMsg* request,
        SessionMeta* meta) {
    Resolution res = PREDICTION_IGNORE;
    PredictionInterval interval;
    if(getPrediction(request, meta, interval)) {
        simtime_t now = Now();
        if(now >= interval.first && now <= interval.second) {
            res = PREDICTION_HIT;
        } else {
            res = PREDICTION_MISS;
        }
    }
    return res;
}

void PredictorIfc::addPrediction(AppFlitMsg* request, SessionMeta* meta,
        const PredictionInterval& interval) {
    map<SessionMeta*, PredictionInterval>::iterator iter = m_predictionTable.find(meta);
    if(iter != m_predictionTable.end()) {
        cerr << request;
        throw cRuntimeError("Trying to add prediction which already exists");
    }
    m_predictionTable[meta] = interval;
}

bool PredictorIfc::getPrediction(AppFlitMsg* request, SessionMeta* meta,
        PredictionInterval& interval) {
    bool isValueValid = false;
    try {
        interval = m_predictionTable.at(meta);
        isValueValid = true;
    } catch(std::out_of_range &e) {
        cerr << "Trying to get a prediction which doesn't exist for\n";
        cerr << request;
    }
    return isValueValid;
}

void PredictorIfc::removePrediction(AppFlitMsg* request, SessionMeta* meta) {
    map<SessionMeta*, PredictionInterval>::iterator iter = m_predictionTable.find(meta);
    if(iter == m_predictionTable.end()) {
        cerr << request;
        throw cRuntimeError("Trying to remove a prediction which dosn't exists");
    }
    m_predictionTable.erase(iter);
}

Resolution PredictorIfc::checkFlit(NoCFlitMsg *msg, SessionMeta *meta) {
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
                    callHandler(flit, meta, res);
                }
                if(lastPacket) {
                    res = onEndFlow(flit, meta);
                    callHandler(flit, meta, res);
                }
                if(!(firstPacket || lastPacket)) {
                    res = onMidFlow(flit, meta);
                    callHandler(flit, meta, res);
                }
            }
        } else { /* Maybe its a write request ? */ }
    } else {
        cerr << "MID FLIT in PREDICTOR OBJECT\n";
    }

    return res;
}
