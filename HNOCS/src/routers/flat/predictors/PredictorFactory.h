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

#ifndef PERDICTORFACTORY_H_
#define PERDICTORFACTORY_H_
#include "Random.h"
#include "NoPredictor.h"
#include "TCPPredictor.h"
#include "AveragePredictor.h"

class PredictorFactory {
    PredictorFactory();
public:
    static Predictor* generatePredictor(const string& predictor) {
        if (predictor.compare("Random") == 0) {
                return new Random();
        }
        if (predictor.compare("TCP") == 0) {
            return new TCPPredictor();
        }
        if (predictor.compare("Average") == 0) {
            return new AveragePredictor();
        }


        return new NoPredictor();
    }

    virtual ~PredictorFactory();
};

#endif /* PERDICTORFACTORY_H_ */
