/*
 * DefaultPredictor.cc
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#include "DefaultPredictor.h"

DefaultPredictor::DefaultPredictor():
    PredictorIfc("Default") {
}

DefaultPredictor::~DefaultPredictor() {

}

void DefaultPredictor::onMiss(AppFlitMsg* msg, SessionMeta* meta,PredictionInterval predictedInterval) {
    cerr << "Miss " << meta->getSessionId() << "\n";
}

void DefaultPredictor::onHit(AppFlitMsg* msg, SessionMeta* meta, PredictionInterval predictedInterval) {
    cerr << "Hit " << meta->getSessionId() << "\n";
}

void DefaultPredictor::onDestroy(AppFlitMsg* msg, SessionMeta* meta) {
    cerr << "Destroy " << meta->getSessionId() << "\n";
}

PredictionInterval DefaultPredictor::predict(AppFlitMsg* request,
        SessionMeta* meta) {
    return PredictionInterval(SimTime(0),SimTime(0)); // This would always miss
}

