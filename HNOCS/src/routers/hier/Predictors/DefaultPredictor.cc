/*
 * DefaultPredictor.cc
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#include "DefaultPredictor.h"

DefaultPredictor::DefaultPredictor():
    PredictorIfc("Default") {
}

DefaultPredictor::~DefaultPredictor() {

}

PredictionInterval DefaultPredictor::predict(NoCFlitMsg* msg) {
    return PredictionInterval(SimTime(0),SimTime(0)); // This would always miss
}

