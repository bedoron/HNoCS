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

class AveragePredictor: public PredictorIfc {
    cStdDev m_deltas;
    static const long int WARMUP;
    static const double RADIUS; // Radious around STD DEV in Nanos

    protected:
    // On Miss handler
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta);
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta);
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta);
    // Return prediction delta from t=0, all request pass it, user defined algorithm
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta);

public:
    AveragePredictor();
    virtual ~AveragePredictor();
};

#endif /* AVERATEPREDICTOR_H_ */
