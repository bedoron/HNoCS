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

#ifndef ALWAYSHIT_H_
#define ALWAYSHIT_H_

#include "PredictorIfc.h"

class AlwaysHit: public PredictorIfc {
protected:
    // On Miss handler
    virtual void onMiss(AppFlitMsg *msg, SessionMeta *meta,PredictionInterval predictedInterval);
    // On Hit handler
    virtual void onHit(AppFlitMsg *msg, SessionMeta *meta, PredictionInterval predictedInterval);
    // On Destroy session (last tail flit) handler
    virtual void onDestroy(AppFlitMsg *msg, SessionMeta *meta);
    // Return prediction delta from t=0, all request pass it, user defined algorithm
    virtual PredictionInterval predict(AppFlitMsg *request, SessionMeta *meta);


public:
    AlwaysHit();
    virtual ~AlwaysHit();
};

#endif /* ALWAYSHIT_H_ */
