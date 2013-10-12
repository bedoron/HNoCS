/*
 * RandomPredictor.cc
 *
 *  Created on: Aug 30, 2013
 *      Author: doron
 */

#include "RandomPredictor.h"

RandomPredictor::RandomPredictor():
    PredictorIfc("Random") {

}

RandomPredictor::~RandomPredictor() {
}

PredictionInterval RandomPredictor::predict(NoCFlitMsg* msg) {
    // random time between 100 to 150 NS with random delta of 5-10 NS
    simtime_t center  = Now() + SimTime(100 + rand() % 50, SIMTIME_NS);
    simtime_t delta = SimTime(5 + rand() % 10, SIMTIME_NS);

    return PredictionInterval(center-delta, center+delta);
}

