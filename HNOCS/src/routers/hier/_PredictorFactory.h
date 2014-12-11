/*
 * PredictorFactory.h
 *
 *  Created on: Aug 29, 2013
 *      Author: doron
 */

#ifndef PREDICTORFACTORY_H_
#define PREDICTORFACTORY_H_

#include "PredictorIfc.h"
#include <string>

#include <map>

using std::map;
using std::string;

class PredictorFactory {

    PredictorFactory();
public:
    static PredictorFactory& instance();

    PredictorIfc *build(const string &method);

    virtual ~PredictorFactory();
};

#endif /* PREDICTORFACTORY_H_ */
