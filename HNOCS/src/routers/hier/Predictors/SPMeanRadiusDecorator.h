/*
 * SPMeanRadiusDecorator.h
 *
 *  Created on: Nov 16, 2013
 *      Author: hnocs
 */

#ifndef SPMEANRADIUSDECORATOR_H_
#define SPMEANRADIUSDECORATOR_H_
/*
#include "StatisticalPredictorDecorator.h"

class SPMeanRadiusDecorator : public StatisticalPredictorDecorator {
    cStdDev m_rads;
public:
    virtual void postMiss(AppFlitMsg *msg, SessionMeta *meta) {

        if(m_deltas.getCount()>m_warmup) {
//            simtime_t stat = getStatisticalParameter();
//            PredictionObject prediction = getPrediction(msg, meta);
//            simtime_t now = Now();

            double var = m_deltas.getVariance();
            if(var > 1 && m_radius < 4*RADIUS) {
                m_radius *= 2;
            }
        }
    };
    virtual void postHit(AppFlitMsg *msg, SessionMeta *meta) {
        if(m_radius>RADIUS) {
            m_radius *= 0.9; // Reduce by 10%
        }
    };
    virtual void postDestroy(AppFlitMsg *msg, SessionMeta *meta) { };

    SPMeanRadiusDecorator(StatisticalPredictor *statpred): StatisticalPredictorDecorator(statpred) {}

};

*/
#endif /* SPMEANRADIUSDECORATOR_H_ */
