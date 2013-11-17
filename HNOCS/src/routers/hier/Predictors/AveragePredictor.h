/*
 * AveragePredictor.h
 *
 *  Created on: Nov 16, 2013
 *      Author: hnocs
 */

#ifndef AVERAGEPREDICTOR_H_
#define AVERAGEPREDICTOR_H_
#include "StatisticalPredictor.h"

class AveragePredictor: public StatisticalPredictor {
    string m_name;
    cStdDev radiusErr;
protected:
    virtual simtime_t getStatisticalParameter() {
        long int count = m_deltas.getCount();

        if(count!=0) {
            return simtime_t((m_deltas.getSum()/count));
        }
        return Now();
    }

    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta) {
        StatisticalPredictor::onMiss(msg, meta);

        PredictionObject &prediction = getPrediction(msg, meta);
        simtime_t now = Now();

        double old_radius = (prediction.interval.first -prediction.interval.second).dbl();
        double old_centroid = (prediction.interval.first + prediction.interval.second).dbl()/2.0;

        double new_radius = now.dbl() - old_centroid;
        new_radius *= (new_radius<0)?-1:1;
        old_radius *= (old_radius<0)?-1:1;

        radiusErr.collect(new_radius);

        double mean_radius = radiusErr.getMean();

//        cerr << "MEAN: " << mean_radius << " OLD: " << old_radius << " \n";

        double ratio = (old_radius>0)?1.1*(mean_radius/old_radius):1;

        if((m_radius<RADIUS*5)) {
            m_radius *= ratio;
        }
    }

    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta) {
        StatisticalPredictor::onHit(msg, meta);

        if(m_radius>2*RADIUS) {
            m_radius*=0.9;
        }
    }

    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta) {



        return StatisticalPredictor::predict(request, meta);
    }

public:
    AveragePredictor(): StatisticalPredictor(), m_name("Average predictor"), radiusErr("Radius err") { }
    ~AveragePredictor() { radiusErr.clearResult(); }

    virtual const string &getName() const {
        return m_name;
    }
};

#endif /* AVERAGEPREDICTOR_H_ */
