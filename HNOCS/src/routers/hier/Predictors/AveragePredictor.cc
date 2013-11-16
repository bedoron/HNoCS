/*
 * AveratePredictor.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: hnocs
 */

#include "AveragePredictor.h"
#include <iostream>
using std::cerr;

const long int AveragePredictor::WARMUP = 50; // Window size
const double AveragePredictor::RADIUS = 10; // This should be in Nano seconds

AveragePredictor::AveragePredictor(): PredictorIfc("AveragePredictor"),
        m_deltas("Miss deltas") {

}

void AveragePredictor::onMiss(AppFlitMsg* msg, SessionMeta* meta) {
    PredictionObject &prediction = getPrediction(msg, meta);
    simtime_t delta = Now() - prediction.timestamp;
    m_deltas.collect(delta);

//    cerr << "Miss handler\n";
}

void AveragePredictor::onHit(AppFlitMsg* msg, SessionMeta* meta) {
    cerr << "Hit handler\n";
}

void AveragePredictor::onDestroy(AppFlitMsg* msg, SessionMeta* meta) {
}

PredictionInterval AveragePredictor::predict(AppFlitMsg* request,
        SessionMeta* meta) {
    PredictionInterval interval(Now(), Now());
    if(m_deltas.getCount()>=WARMUP) {
        simtime_t delta = m_deltas.getStddev();
        interval.first = delta - simtime_t(RADIUS, SIMTIME_NS);
        interval.second = delta + simtime_t(RADIUS, SIMTIME_NS);

        //cerr << "Making prediction\n";
    } else {
        //cerr << "WARMUP TIME: " << m_deltas.getCount() << " \n";
    }
    return interval;
}

AveragePredictor::~AveragePredictor() {
    cerr << "Final STD Dev: " << m_deltas.getStddev() << "\n";


    m_deltas.clearResult();
    // TODO Auto-generated destructor stub
}

