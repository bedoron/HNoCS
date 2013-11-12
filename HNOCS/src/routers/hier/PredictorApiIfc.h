/*
 * PredictorApiIfc.h
 *
 *  Created on: Nov 12, 2013
 *      Author: hnocs
 */

#ifndef PREDICTORAPIIFC_H_
#define PREDICTORAPIIFC_H_
#include <string>
using std::string;

typedef enum { PREDICTION_MISS, PREDICTION_HIT, PREDICTION_IGNORE, PREDICTION_CREATE, PREDICTION_DESTROY, PREDICTION_IDLE } Resolution;

class PredictorApiIfc {
public:
    virtual Resolution checkFlit(NoCFlitMsg *msg, SessionMeta *meta = 0) = 0;

    // Check if an object has prediction
    virtual bool hasPrediction(NoCFlitMsg *msg) = 0;
    virtual bool hasPrediction(SessionMeta *meta) = 0;

    static string ResolutionToString(Resolution res) {
        switch(res) {
        case PREDICTION_MISS:       return "PREDICTION MISS";
        case PREDICTION_HIT:        return "PREDICTION HIT";
        case PREDICTION_IGNORE:     return "PREDICTION IGNORE";
        case PREDICTION_CREATE:     return "PREDICTION CREATE";
        case PREDICTION_DESTROY:    return "PREDICTION DESTROY";
        case PREDICTION_IDLE:       return "PREDICTION IDLE";
        }
        return "UNKNOWN";
    }
};

#endif /* PREDICTORAPIIFC_H_ */
