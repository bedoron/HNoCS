/*
 * PredictorFactory.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: doron
 */
#include <iostream>
#include <algorithm>
#include "PredictorFactory.h"
using std::transform;
using std::cerr;
// ----- Predictors
#include "Predictors/DefaultPredictor.h"
#include "Predictors/RandomPredictor.h"

PredictorFactory::PredictorFactory() { }

PredictorFactory& PredictorFactory::instance() {
    static PredictorFactory instance;
    return instance;
}

PredictorIfc* PredictorFactory::build(const string &method) {
    string predictorName;
    transform(method.begin(), method.end(),predictorName.begin(), tolower);

    if(predictorName == "random")
        return new RandomPredictor();
    if(predictorName == "default")
        return new DefaultPredictor();



    cerr << "No " << method << " predictor, using default\n";
    //ev << "No " << method << " predictor, using default";
    return new DefaultPredictor();
}

PredictorFactory::~PredictorFactory() { }

