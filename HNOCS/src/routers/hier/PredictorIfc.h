/*
 * PredictorIfc.h
 *
 *  Created on: Aug 29, 2013
 *      Author: doron
 */

#ifndef PREDICTORIFC_H_
#define PREDICTORIFC_H_

#include "CMP_m.h"
#include <string.h>

using std::string;
using std::pair;

typedef pair<simtime_t, simtime_t> PredictionInterval;

class PredictorIfc {
    string m_method;


public:
    // Signal name for Schedulers and in ports
    static const string SignalName() { return "prediction"; }

    // Return prediction delta from t=0, all request pass it
    virtual PredictionInterval predict(NoCFlitMsg *request)  = 0;

    // Implementation of general functions (Im lazy)
    PredictorIfc(const char *method): m_method(method) {}
    const string &getName() const { return m_method; }

    // Use this function to do some adjustments on the fly
    // when we have a prediction miss
    virtual void miss(NoCFlitMsg *response, simtime_t predicted) { }

    static simtime_t Now() { return cSimulation::getActiveSimulation()->getSimTime(); };
};

#endif /* PREDICTORIFC_H_ */
