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

#ifndef __HNOCS_PREDICTOR_H_
#define __HNOCS_PREDICTOR_H_

#include <omnetpp.h>
#include <map>
#include <vector>
#include "PredictorIfc.h"
#include "SessionMeta.h"
//#include "InPortSync.h"
#include "XYOPCalc.h"
#include "SchedSync.h"
#include "FLUVCCalc.h"

using std::map;
using std::vector;

//class Predictor : public cSimpleModule
//{
//    typedef map<SessionMeta*, PredictionInterval> PredictionTable;
//
//
//    PredictionTable      m_predictionTable;
//    PredictorIfc         *m_predictor;
//    vector<SessionMeta*> m_VCHit; // if NULL there is not hit for this current VC
//
//    int m_numVCs;
//
////    InPortSync  *m_inPort;
//    XYOPCalc    *m_opCalc;
////    Sched       *m_sched;
//    FLUVCCalc   *m_vcCalc;
//
//    bool CheckIfHit(SessionMeta *meta);
//
//    void HitFlowStart(NoCFlitMsg* msg, SessionMeta *meta);
//    void HitFlowEnd(NoCFlitMsg* msg, SessionMeta *meta);
//
//  protected:
//    virtual void initialize();
//    virtual void handleMessage(cMessage *msg);
//
//public:
//    bool Hit(NoCFlitMsg* msg);
//    bool Hit(int inVC);
//
//    void DestroyHit(int inVC);
//
//    bool Predict(NoCFlitMsg *request);
//    bool Predict(NoCFlitMsg *request, SessionMeta *meta);
//
//    bool PredictIfRequest(NoCFlitMsg *msg, int outPort);
//
//
////    void RegisterRemoteTable(SessionMeta *meta, PredictionInterval predInt);
//
////    InPortSync& getInPort();
//    XYOPCalc& getOpCalc();
////    SchedSync& getSched();
//    FLUVCCalc& getVcCalc();
//};

#endif