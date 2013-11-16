/*
 * MeanPredictor.h
 *
 *  Created on: Nov 16, 2013
 *      Author: hnocs
 */

#ifndef MEANPREDICTOR_H_
#define MEANPREDICTOR_H_

class MeanPredictor: public StatisticalPredictor {
    string m_name;
protected:
    virtual simtime_t getStatisticalParameter() {
        simtime_t stats = simtime_t(m_deltas.getMean());
        return stats;
    }

public:
    MeanPredictor(): StatisticalPredictor(), m_name("Mean predictor") { }

    virtual const string &getName() const {
        return m_name;
    }
};
#endif /* MEANPREDICTOR_H_ */
