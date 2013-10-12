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
public:
    RandomPredictor();
    virtual ~RandomPredictor();

    virtual PredictionInterval predict(NoCFlitMsg *msg);
};

#endif /* RANDOMPREDICTOR_H_ */