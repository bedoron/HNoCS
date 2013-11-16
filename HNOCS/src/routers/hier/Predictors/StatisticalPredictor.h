/*
 * AveratePredictor.h
 *
 *  Created on: Nov 14, 2013
 *      Author: hnocs
 */

#ifndef AVERATEPREDICTOR_H_
#define AVERATEPREDICTOR_H_

#include "PredictorIfc.h"
#include <cstddev.h>

class StatisticalPredictor: public PredictorIfc {
protected:
    cStdDev m_deltas;
    int m_hits;
    int m_misses;
    double m_radius;
    double m_warmup;

    static const long int WARMUP;
    static const double RADIUS; // Radius around STD DEV in Nanos

    // On Miss handler
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta);
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta);
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta);

    virtual simtime_t getStatisticalParameter() = 0;
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta);
    void adjustRadius(double radius) { m_radius = radius; }
public:
    StatisticalPredictor();
    // Return prediction delta from t=0, all request pass it, user defined algorithm

    virtual ~StatisticalPredictor();
};

#endif /* AVERATEPREDICTOR_H_ */
