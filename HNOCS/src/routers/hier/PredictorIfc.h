/*
 * PredictorIfc.h
 *
 *  Created on: Aug 29, 2013
 *      Author: doron
 */

#ifndef PREDICTORIFC_H_
#define PREDICTORIFC_H_

#include "CMP_m.h"
#include "ResponseDB.h"
#include <string.h>
#include <map>

using std::string;
using std::pair;
using std::map;

typedef pair<simtime_t, simtime_t> PredictionInterval;
typedef enum { PREDICTION_MISS, PREDICTION_HIT, PREDICTION_IGNORE, PREDICTION_CREATE, PREDICTION_DESTROY, PREDICTION_IDLE } Resolution;

class PredictorIfc {
    string m_method;


    void callHandler(AppFlitMsg *msg, SessionMeta *meta, Resolution resolution);
protected:
    map<SessionMeta*, PredictionInterval> m_predictionTable;

    /**
     * Checks for HIT/MISS for request, if request doesn't have a prediction function
     * will return PREDICTION_IGNORE.
     */
    virtual Resolution checkPrediction(AppFlitMsg *request, SessionMeta *meta);

    /**
     * Add prediction to the Database. function will throw an exception if we are
     * adding a prediction for a request which already exists
     */
    virtual void addPrediction(AppFlitMsg *request, SessionMeta *meta,
            const PredictionInterval& interval);
    /*
     * True if prediction exists. function output is the interval reference
     * parameter
     */
    virtual bool getPrediction(AppFlitMsg *request, SessionMeta *meta,
            PredictionInterval& interval);
    virtual void removePrediction(AppFlitMsg *request, SessionMeta *meta);

    /**
     * A "FLOW" consists of a request-response CMP messages. each and every
     * CMP message can be broken into App packets and App packets are further
     * broke down into flits.
     *
     * The functions below are being called for each head+tail flits which are
     * part of a Request-Response transaction.
     *
     * Start flow will be called for the head+tail flits which belongs
     * to the FIRST App packet.
     *
     * End flow will be called for the head+tail flits which belongs to
     * the LAST App packet
     *
     * Mid flow is called for any other head+tail flits which are in between
     * the end + start flow
     *
     * This means that if a request consists of a single App message then
     * onStartFlow will be called and immediately afterwards onEndFlow will
     * be called, without called the onMidFlow event
     */
    virtual Resolution onStartFlow(AppFlitMsg *msg, SessionMeta *meta);
    virtual Resolution onEndFlow(AppFlitMsg *msg, SessionMeta *meta);
    virtual Resolution onMidFlow(AppFlitMsg *msg, SessionMeta *meta);

    virtual Resolution onFlit(AppFlitMsg *msg, SessionMeta *meta);

    // On Miss handler
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta) = 0;
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta) = 0;
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta) = 0;
    // Return prediction delta from t=0, all request pass it, user defined algorithm
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta)  = 0;
public:
    PredictorIfc(const char *method);
    const string &getName() const;

    static simtime_t Now();

    // Call this function when passing a flit
    Resolution checkFlit(NoCFlitMsg *msg, SessionMeta *meta = 0);
};

#endif /* PREDICTORIFC_H_ */
