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

#include "AlwaysHit.h"

AlwaysHit::AlwaysHit(): PredictorIfc("AlwaysHit") {
}

void AlwaysHit::onMiss(AppFlitMsg* msg, SessionMeta* meta,PredictionInterval predictedInterval) {
//    cerr << "Miss " << meta->getSessionId() << "\n";
}

void AlwaysHit::onHit(AppFlitMsg* msg, SessionMeta* meta, PredictionInterval predictedInterval) {
//    cerr << "Hit " << meta->getSessionId() << "\n";
}

void AlwaysHit::onDestroy(AppFlitMsg* msg, SessionMeta* meta) {
    cerr << "Destroy " << meta->getSessionId() << "\n";
}

AlwaysHit::~AlwaysHit() {
}


PredictionInterval AlwaysHit::predict(AppFlitMsg* request, SessionMeta* meta) {
    simtime_t left      = Now();
    simtime_t right  = left +  SimTime(3600, SIMTIME_S); // one hour == inf.

    return PredictionInterval(left, right);
}
