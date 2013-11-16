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
protected:
    virtual simtime_t getStatisticalParameter() {
        long int count = m_deltas.getCount();

        if(count!=0) {
            return simtime_t((m_deltas.getSum()/count));
        }
        return Now();
    }

public:
    AveragePredictor(): StatisticalPredictor(), m_name("Average predictor") { }

    virtual const string &getName() const {
        return m_name;
    }
};

#endif /* AVERAGEPREDICTOR_H_ */
