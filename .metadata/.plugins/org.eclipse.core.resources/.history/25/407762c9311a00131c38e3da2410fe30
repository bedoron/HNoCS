/*
 * PredictorFactory.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: doron
 */

#include "PredictorFactory.h"

// ----- Predictors
#include "DefaultPredictor.h"

PredictorFactory::PredictorFactory() {
    // TODO Auto-generated constructor stub

}

PredictorFactory& PredictorFactory::instance() {
    static PredictorFactory instance;
    return instance;
}

PredictorIfc* PredictorFactory::build(const string& method) {
    return new DefaultPredictor();
}

PredictorFactory::~PredictorFactory() {
    // TODO Auto-generated destructor stub
}

