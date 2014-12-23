/*
 * NoPredictor.h
 *
 *  Created on: Nov 24, 2014
 *      Author: doron
 */

#ifndef NOPREDICTOR_H_
#define NOPREDICTOR_H_

#include "Predictor.h"

class NoPredictor: public Predictor {
protected:
    virtual bool algorithm(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {
        return false;
    };
};

#endif /* NOPREDICTOR_H_ */
