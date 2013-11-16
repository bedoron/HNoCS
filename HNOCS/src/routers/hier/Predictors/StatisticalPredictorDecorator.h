/*
 * StatisticalPredictorDecorator.h
 *
 *  Created on: Nov 16, 2013
 *      Author: hnocs
 */

#ifndef STATISTICALPREDICTORDECORATOR_H_
#define STATISTICALPREDICTORDECORATOR_H_
/*
class StatisticalPredictorDecorator: public StatisticalPredictor {
    StatisticalPredictor &m_predictor;
    string m_name;
protected:
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta) {
        m_predictor.onMiss(msg, meta);
        postMiss(msg, meta);
    }
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta) {
        m_predictor.onHit(msg, meta);
        postHit(msg, meta);
    }
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta) {
        m_predictor.onDestroy(msg, meta);
        postDestroy(msg, meta);
    }



    virtual void postMiss(AppFlitMsg *msg, SessionMeta *meta) = 0;
    virtual void postHit(AppFlitMsg *msg, SessionMeta *meta) = 0;
    virtual void postDestroy(AppFlitMsg *msg, SessionMeta *meta) = 0;

    virtual simtime_t getStatisticalParameter() {
        return m_predictor.getStatisticalParameter();
    }

public:

    StatisticalPredictorDecorator(StatisticalPredictor *predictor): m_predictor(*predictor), m_name("Decorator") {
        m_name = m_predictor.getName() + m_name;
    }

    ~StatisticalPredictorDecorator() {
        delete &m_predictor;
    }

    // Check if an object has prediction
    virtual bool hasPrediction(NoCFlitMsg *msg) {
        return m_predictor.hasPrediction(msg);
    }
    virtual bool hasPrediction(SessionMeta *meta) {
        return m_predictor.hasPrediction(meta);
    }

    Resolution checkFlit(NoCFlitMsg *msg, SessionMeta *meta = 0) {
        return m_predictor.checkFlit(msg, meta);
    }

    virtual const string &getName() const {
        return m_name;
    }

    // Return prediction delta from t=0, all request pass it, user defined algorithm
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta) {
        return m_predictor.predict(request, meta);
    }
};

*/
#endif /* STATISTICALPREDICTORDECORATOR_H_ */
