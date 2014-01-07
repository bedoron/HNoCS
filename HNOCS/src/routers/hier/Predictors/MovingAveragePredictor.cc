//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <iostream>
#include "MovingAveragePredictor.h"

using std::cerr;

MovingAveragePredictor::MovingAveragePredictor(): PredictorIfc("MovingAverage"), predictions(0), hits(0), negatives(0) {
    errorThreshold = 0; // TODO: get it from somewhere....
    simulation.getActiveEnvir()->
}

MovingAveragePredictor::~MovingAveragePredictor() {

}

void MovingAveragePredictor::onMiss(AppFlitMsg *msg, SessionMeta *meta, PredictionInterval predictedInterval) {
    handleResponse(msg, meta, predictedInterval);
}

// On Hit handler
void MovingAveragePredictor::onHit(AppFlitMsg *msg, SessionMeta *meta, PredictionInterval predictedInterval) {
    handleResponse(msg, meta, predictedInterval);
}
// On Destroy session (last tail flit) handler
void MovingAveragePredictor::onDestroy(AppFlitMsg *msg, SessionMeta *meta) {

}

void MovingAveragePredictor::handleResponse(AppFlitMsg* msg, SessionMeta* meta,
        PredictionInterval predictedInterval) {
    ++responses;
    PredictionTable entry;
    // entry.request_arrival = Now();
    // TODO: put a variable in the PredictorIfc which tells which router this is and add a query method
    // to session which allows to get the time of the request passing this router.
    entry.response_prediction = SimTime(actualPrediction);
    entry.response_arrival = Now();

    table.push_back(entry);

    if(table.size()>=TABLE_SIZE) {
       table.pop_front();
    }
}

// Return prediction delta from t=0, all request pass it, user defined algorithm
PredictionInterval MovingAveragePredictor::predict(AppFlitMsg *request, SessionMeta *meta)  {
    PredictionInterval interval(Now(),Now()); // Generic
    ++predictions;

    if(table.size()>=TABLE_SIZE) { // Warmup is done
        double totalDelta = 0;
        double totalError = 0;
        for(deque<PredictionTable>::iterator it = table.begin(); it != table.end(); ++it) {
            double actualTime = (it->response_arrival.dbl()-it->response_prediction.dbl());
            double error = (it->response_prediction.dbl() - it->response_arrival.dbl());

            totalDelta += actualTime;
            totalError += error;
        }
        totalDelta /= (double)table.size();
        totalError /= (double)table.size();

        if(totalError < errorThreshold) {
            double halfError = totalError/2.0;
            if( ((totalDelta - halfError)>0) && ((totalDelta + halfError)>0) ) {
                interval.first += SimTime(totalDelta - halfError);
                interval.second+= SimTime(totalDelta + halfError);
            } else {
                ++negatives;
                cerr << negatives << " negative deltas so far\n";
            }
        }
    }
    return interval;
}
