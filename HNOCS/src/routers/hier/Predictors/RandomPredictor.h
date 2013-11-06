/*
 * RandomPredictor.h
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#ifndef RANDOMPREDICTOR_H_
#define RANDOMPREDICTOR_H_

#include "PredictorIfc.h"

class RandomPredictor: public PredictorIfc {
    string m_method;

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
    RandomPredictor();
    virtual ~RandomPredictor();

    virtual PredictionInterval predict(NoCFlitMsg *msg);
};

#endif /* RANDOMPREDICTOR_H_ */
