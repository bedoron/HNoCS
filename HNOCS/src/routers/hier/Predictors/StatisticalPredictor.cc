/*
 * AveratePredictor.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: hnocs
 */

#include "StatisticalPredictor.h"
#include <iostream>
using std::cerr;

const long int StatisticalPredictor::WARMUP = 50; // How many measurements to make until starting to predict
const double StatisticalPredictor::RADIUS = 10; // This should be in Nano seconds

StatisticalPredictor::StatisticalPredictor(): PredictorIfc("StatisticalPredictor"),
        m_deltas("Miss deltas"), m_hits(0), m_misses(0), m_radius(RADIUS), m_warmup(WARMUP) {
}

void StatisticalPredictor::onMiss(AppFlitMsg* msg, SessionMeta* meta) {
    PredictionObject &prediction = getPrediction(msg, meta);
    simtime_t now = Now();

    simtime_t delta = now - prediction.timestamp;
    m_deltas.collect(delta);

    if(m_deltas.getCount()>= m_warmup) {
        ++m_misses; // We count misses only after warmup period

        simtime_t stats = getStatisticalParameter();
//        cerr << "Mean: " << m_deltas.getMean() << " Std: " << m_deltas.getStddev() << " Stats: "  << stats << "\n";

        cerr << "["<< Now() << "] R:" << port->getParentModule()->getIndex() << " S:" << meta->getSessionId() << " ";
        cerr << "Flow arrived after " << delta << " ns, ";
        cerr << "prediction was: [" << prediction.interval.first << "," << prediction.interval.second<< "]";
        cerr << "[Hits: " << m_hits << ", Misses: " << m_misses << "] - " << m_deltas.getCount() << " samples,";
        cerr << "StdDev: " << stats << " (err: " <<  (delta-stats) << ") rad: "<< m_radius<< "\n";
    }
}

void StatisticalPredictor::onHit(AppFlitMsg* msg, SessionMeta* meta) {
    cerr << "Hit handler\n";
    ++m_hits;
}

void StatisticalPredictor::onDestroy(AppFlitMsg* msg, SessionMeta* meta) {
}

PredictionInterval StatisticalPredictor::predict(AppFlitMsg* request,
        SessionMeta* meta) {
    PredictionInterval interval(Now(), Now());
    if(m_deltas.getCount()>=m_warmup) {
        simtime_t delta = getStatisticalParameter();
        simtime_t radius = simtime_t(m_radius, SIMTIME_NS);
//        cerr << "RADIUS USED FOR PREDICTION IS " << radius.dbl() << "Actual: " << m_radius << "\n";
        if(delta > radius) {
            interval.first += delta - radius;
        } else {
            interval.first += delta;
        }

        interval.second += delta + radius;
    }
    return interval;
}

StatisticalPredictor::~StatisticalPredictor() {
    cerr << "Final STD Dev: " << m_deltas.getStddev() << "\n";


    m_deltas.clearResult();
    // TODO Auto-generated destructor stub
}

