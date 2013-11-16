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
#include "Predictors/AlwaysHit.h"
#include "Predictors/AveragePredictor.h"
#include "Predictors/MeanPredictor.h"
//#include "Predictors/SPMeanRadiusDecorator.h"

PredictorFactory::PredictorFactory() { }

PredictorFactory& PredictorFactory::instance() {
    static PredictorFactory instance;
    return instance;
}

PredictorIfc* PredictorFactory::build(const string &method) {
//    string predictorName;
//
//    transform(method.begin(), method.end(),predictorName.begin(), tolower);


    if(method == "random")
        return new RandomPredictor();
    if(method == "default")
        return new DefaultPredictor();
    if(method == "always-hit")
        return new AlwaysHit();
    if(method == "mean")
        return new MeanPredictor();
    if(method == "avg")
        return new AveragePredictor();
/*
    if(method == "mean-rad")
        return new SPMeanRadiusDecorator(new MeanPredictor());
    if(method == "mean-avg")
        return new SPMeanRadiusDecorator(new AveragePredictor());
*/
    cerr << "No " << method << " predictor, using default\n";
    //ev << "No " << method << " predictor, using default";
    return new DefaultPredictor();
}

PredictorFactory::~PredictorFactory() { }

