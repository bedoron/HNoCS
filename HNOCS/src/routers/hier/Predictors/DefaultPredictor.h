/*
 * DefaultPredictor.h
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#ifndef DEFAULTPREDICTOR_H_
#define DEFAULTPREDICTOR_H_

#include "PredictorIfc.h"
#include <string>
using std::string;

class DefaultPredictor: public PredictorIfc {
    string m_method;
protected:
    // On Miss handler
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta,PredictionInterval predictedInterval);
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta, PredictionInterval predictedInterval);
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta);
    // Return prediction delta from t=0, all request pass it, user defined algorithm
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta);

public:
    DefaultPredictor();
    virtual ~DefaultPredictor();

};

#endif /* DEFAULTPREDICTOR_H_ */
