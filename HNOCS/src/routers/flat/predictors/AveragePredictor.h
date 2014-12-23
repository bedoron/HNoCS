/*
 * AveragePredictor.h
 *
 *  Created on: Dec 16, 2014
 *      Author: doron
 */

#ifndef AVERAGEPREDICTOR_H_
#define AVERAGEPREDICTOR_H_

#include <cmodule.h>
#include <CentSchedRouter.h>
#include <CMP_m.h>
#include <NoCs_m.h>
#include <ResponseDB.h>
#include <SessionMeta.h>
#include <map>
#include <utility>
#include <deque>
#include <bits/stl_numeric.h>
#include "Predictor.h"

using std::map;
using std::deque;
using std::pair;

class AveragePredictor: public Predictor {
    const unsigned int warmUp = 1;

    map<int, deque<double>> delayVectorPerRoute;
    map<SessionMeta*, double> prediction;

    double calculateAverage(int destination) {
        double sum = std::accumulate(delayVectorPerRoute[destination].begin(), delayVectorPerRoute[destination].end(), 0.0);
        return sum / (double)delayVectorPerRoute[destination].size();
    }

protected:
    virtual bool shouldPredict(NoCFlitMsg* msg, FlatPortIfc* inPort, FlatPortIfc* outPort, vc_t& inVC) {
        SessionMeta *meta = ResponseDB::getInstance()->find((CMPMsg*)msg);
        if (Predictor::isRequest(msg)) {
            if (delayVectorPerRoute[msg->getDstId()].size() == warmUp) {
                double predicted = calculateAverage(msg->getDstId());
                prediction[meta] = cSimulation::getActiveSimulation()->getSimTime().dbl() + predicted;
                cerr << "Predicting: " << predicted << "\n";
            }
        } else { // Accumulate data
            if (delayVectorPerRoute[msg->getDstId()].size() < warmUp) {
                SimTime delta = meta->getRouterDelta(this->router->getIndex());
                delayVectorPerRoute[msg->getDstId()].push_back(delta.dbl());
            }
        }

        return Predictor::isResponse(msg);
    }

    virtual bool algorithm(NoCFlitMsg *msg, FlatPortIfc *inPort, FlatPortIfc *outPort, vc_t &inVC)
    {
        SessionMeta *meta = ResponseDB::getInstance()->find((CMPMsg*)msg);
        SimTime delta = meta->getRouterDelta(this->router->getIndex());

        bool skipPipeline = false;
        int destination = msg->getDstId();

        if (delayVectorPerRoute[msg->getDstId()].size() == warmUp) {
            delayVectorPerRoute[destination].push_back(delta.dbl());
            delayVectorPerRoute[destination].pop_front();
        }

        // Get prediction if one was made
        auto predicted = prediction.find(meta);
        if (predicted != prediction.end()) {
            double predictedTime = predicted->second;
            double currentTime = cSimulation::getActiveSimulation()->getSimTime().dbl();
            skipPipeline = (predictedTime == currentTime);
            prediction.erase(meta);

            if (!skipPipeline) {
                cerr << "[" << this->router->getIndex() << "]" << " Prediction missed, predicted: " << predictedTime << " but arrived on: " << currentTime << "\n";
            }

        }

        return skipPipeline;

    }
};

#endif /* AVERAGEPREDICTOR_H_ */
