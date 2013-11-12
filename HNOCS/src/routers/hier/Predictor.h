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
#include "PredictorApiIfc.h"

using std::vector;

class Predictor : public cSimpleModule, public PredictorApiIfc
{
    static const bool PRINT_DATA = false;
    static bool debug;

    PredictorIfc    *m_predictor;
    int             m_numVCs;

    XYOPCalc    *m_opCalc;
    FLUVCCalc   *m_vcCalc;

    int m_routerIndex;
    int m_portIndex;

    bool m_printData;


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

    /* Registers flit in local  - THIS FUNCTION MUST BE
     * called **AFTER** we have OP Calc result
     */
    //Resolution registerFlit(NoCFlitMsg *msg, SessionMeta *meta = 0);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    ~Predictor();
    bool Hit(NoCFlitMsg* msg);

    void DestroyHit(SessionMeta *meta);


    bool PredictIfRequest(NoCFlitMsg *msg);

    XYOPCalc& getOpCalc();
    FLUVCCalc& getVcCalc();

    // Get the Predictor associated with current hierarchy
    static Predictor* GetMyPredictor(cSimpleModule *current);

    virtual Resolution checkFlit(NoCFlitMsg *msg, SessionMeta *meta = 0);
    // Check if an object has prediction
    virtual bool hasPrediction(NoCFlitMsg *msg);
    virtual bool hasPrediction(SessionMeta *meta);

    /* Registers flit in its appropriate predictor - THIS FUNCTION MUST BE
     * called **AFTER** we have OP Calc result
     */
    Resolution registerFlit(NoCFlitMsg *msg, SessionMeta *meta = 0);

};

#endif
