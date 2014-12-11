/*
 * Predictor.h
 *
 *  Created on: Nov 23, 2014
 *      Author: doron
 */

#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include "App_m.h"
#include "FlatPort.h"
#include "ResponseDB.h"

class Predictor {
public:
    bool predict(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {
        // Do not predict if we already predicted this vc or user asked not to
        if (!shouldPredict(msg, inPort, outPort, inVC) || inVC.predicted) {
            return false;
        }

        bool predictionSuccess = algorithm(msg, inPort, outPort, inVC);

        if (predictionSuccess) {
            inVC.pipelineLatency = 0;
        }

        inVC.predicted = true;
    }
protected:
    // Default behavior - predict only responses
    virtual bool shouldPredict(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {
        ResponseDB* respDB = ResponseDB::getInstance();
        bool predict = respDB->exists(msg->getId()) && respDB->isResponse(msg->getId());
        return predict;
    }

    // Return true to bump packets
    virtual bool algorithm(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) = 0;
};



#endif /* PREDICTOR_H_ */