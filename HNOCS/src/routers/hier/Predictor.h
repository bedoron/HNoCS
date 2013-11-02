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

class Predictor : public cSimpleModule
{
    static const bool PRINT_DATA = false;

    typedef map<SessionMeta*, PredictionInterval> PredictionTable;


    PredictionTable      m_predictionTable;
    PredictorIfc         *m_predictor;
    vector<SessionMeta*> m_VCHit; // if NULL there is not hit for this current VC

    int m_numVCs;

//    InPortSync  *m_inPort;
    XYOPCalc    *m_opCalc;
//    Sched       *m_sched;
    FLUVCCalc   *m_vcCalc;
    int m_routerIndex;
    int m_portIndex;

    bool m_printData;

    /*
     * ?!
     */
    bool CheckIfHit(SessionMeta *meta, NoCFlitMsg* nocMsg);

    /**
     * Gets the predictor which belongs to the outPort of this packet
     */
    Predictor* getTargetPredictor(NoCFlitMsg *msg);

    /**
     * Helper function to extract the flitInfo class for a flit
     * this function is used to extract the out port from the packet
     */
    static class inPortFlitInfo* GetFlitInfo(NoCFlitMsg *msg);

    /**
     * Call self predictor object to register request with meta
     */
    bool Predict(NoCFlitMsg *request, SessionMeta *meta);


  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    ~Predictor();
    bool Hit(NoCFlitMsg* msg);



//    bool Hit(SessionMeta *meta);
//    bool Hit(int inVC);

    void DestroyHit(SessionMeta *meta);

//    bool Predict(NoCFlitMsg *request);



    /**
     * this function will create a prediction for flit *msg iff
     * flit is first head flit of it's flow and a request flit
     *
     * first of its flow means - PktIdx == 0 && FlitIdx == 0
     */
    bool PredictIfRequest(NoCFlitMsg *msg);


//    void RegisterRemoteTable(SessionMeta *meta, PredictionInterval predInt);

//    InPortSync& getInPort();
    XYOPCalc& getOpCalc();
//    SchedSync& getSched();
    FLUVCCalc& getVcCalc();

    // Get the Predictor associated with current hierarchy
    static Predictor* GetMyPredictor(cSimpleModule *current);

    // Test method

    SessionMeta *getVCHit(int vc);

};

#endif
