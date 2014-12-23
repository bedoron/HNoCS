/*
 * TCPPredictor.h
 *
 *  Created on: Dec 15, 2014
 *      Author: doron
 */

#ifndef TCPPREDICTOR_H_
#define TCPPREDICTOR_H_
#include "predictors/Predictor.h"

class TCPPredictor: public Predictor {
    virtual bool algorithm(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {
        return true;
    }
};




#endif /* TCPPREDICTOR_H_ */
