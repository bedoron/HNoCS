/*
 * RandomPredictor.cc
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#include "RandomPredictor.h"

#include <iostream>
using std::cerr;

RandomPredictor::RandomPredictor():
    PredictorIfc("Random") {

}

RandomPredictor::~RandomPredictor() {
}

void RandomPredictor::onMiss(AppFlitMsg* msg, SessionMeta* meta,PredictionInterval predictedInterval) {
    cerr << "Missed session " << meta->getSessionId() << "\n";
}

void RandomPredictor::onHit(AppFlitMsg* msg, SessionMeta* meta, PredictionInterval predictedInterval) {
    cerr << "Hit session " << meta->getSessionId() << "\n";
}

void RandomPredictor::onDestroy(AppFlitMsg* msg, SessionMeta* meta) {
    cerr << "Destroying session " << meta->getSessionId() << "\n";
}

PredictionInterval RandomPredictor::predict(AppFlitMsg* request,
        SessionMeta* meta) {
    // random time between 250 to 300 NS with random delta of 5-10 NS
    simtime_t center  = Now() + SimTime(250 + rand() % 50, SIMTIME_NS);
    simtime_t delta = SimTime(5 + rand() % 10, SIMTIME_NS);

    return PredictionInterval(center-delta, center+delta);
}
