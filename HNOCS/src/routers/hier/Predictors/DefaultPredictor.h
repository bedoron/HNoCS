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
public:
    DefaultPredictor();
    virtual ~DefaultPredictor();

    virtual PredictionInterval predict(NoCFlitMsg *msg);
};

#endif /* DEFAULTPREDICTOR_H_ */
