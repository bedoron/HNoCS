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

#ifndef PREDICTIONOBJECT_H_
#define PREDICTIONOBJECT_H_

#include <cmodule.h>
typedef pair<simtime_t, simtime_t> PredictionInterval;

class PredictionObject {
public:
    PredictionInterval  interval;
    Resolution          resolution;
    simtime_t           timestamp;

    PredictionObject(PredictionInterval interval):
        interval(interval), resolution(PREDICTION_IDLE) {
        timestamp = cSimulation::getActiveSimulation()->getSimTime();

    }

    PredictionObject(): resolution(PREDICTION_IDLE) {}
};

#endif /* PREDICTIONOBJECT_H_ */
