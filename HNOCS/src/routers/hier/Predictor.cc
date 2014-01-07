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

#include "Predictor.h"
#include "PredictorFactory.h"
#include "ResponseDB.h"
#include "SessionMeta.h"
//#include "InPortSync.h"
#include <string>
#include <iostream>
#include "Utils.h"
using std::string;
using std::cerr;

Define_Module(Predictor);

bool Predictor::debug = false;

void Predictor::initialize()
{
    string method(par("method").stringValue());
    m_threshold = par("threshold").doubleValue();
    m_numVCs = getParentModule()->getSubmodule("inPort")->par("numVCs");
    m_portIndex = getParentModule()->getIndex();
    m_routerIndex = getParentModule()->getParentModule()->getIndex();

    EV << "Loading " << method << " predictor\n";


    // Build predictor
    m_predictor = PredictorFactory::instance().build(method);
    m_predictor->setThreshold(m_threshold);
    m_predictor->setRouterIndex(m_routerIndex);
    m_predictor->setPortIndex(m_portIndex);

    cModule *opCalc = getParentModule()->getSubmodule("opCalc");
    cModule *vcCalc = getParentModule()->getSubmodule("vcCalc");

    if(NULL == opCalc || NULL == vcCalc ) {
        throw cRuntimeError(getParentModule(), "Can't find one of the main modules for the port");
    }

    m_opCalc = check_and_cast<XYOPCalc*>(opCalc);
    m_vcCalc = check_and_cast<FLUVCCalc*>(vcCalc);

    m_printData = PRINT_DATA;

    m_predictor->setPort(getParentModule());
}

void Predictor::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

XYOPCalc& Predictor::getOpCalc()  {
    return *m_opCalc;
}

/**
 * Call self predictor object to register request with meta
 */
bool Predictor::Predict(NoCFlitMsg* request, SessionMeta* meta) {
//    AppFlitMsg *msg = (AppFlitMsg*)request;
//
//    PredictionInterval interval = m_predictor->predict(request, meta);
//
//    return true;
    throw cRuntimeError("Predict was caleld");
    return false;
}


/**
 * Gets the predictor which belongs to the outPort of this packet
 */
Predictor* Predictor::getTargetPredictor(NoCFlitMsg *msg) {
    int outPort = GetFlitInfo(msg)->outPort;

    cGate *gate = getParentModule()->gate("sw_in",outPort);
    if (!gate) cRuntimeError("outport is weird");
    cGate *remGate = gate->getPathEndGate()->getPreviousGate();
    if (!remGate) cRuntimeError("remote gate is weird");
    cModule *neighbour = remGate->getOwnerModule();
    if(!neighbour) cRuntimeError("remote port is weird");
    Predictor *adjPred = check_and_cast<Predictor*>(neighbour->getSubmodule("predictor"));
    return adjPred;
}

 FLUVCCalc& Predictor::getVcCalc()  {
    return *m_vcCalc;
}

void Predictor::DestroyHit(SessionMeta *meta) {
}

Predictor::~Predictor() {
    delete m_predictor;
}

Predictor* Predictor::GetMyPredictor(cSimpleModule* current) {
    cModule *predictor = current->getParentModule()->getSubmodule("predictor");
    if(NULL == predictor) {
        throw cRuntimeError(current->getParentModule(), "Can't find prediction module for Port");
    }

    return check_and_cast<Predictor *>(predictor);
}

class inPortFlitInfo* Predictor::GetFlitInfo(NoCFlitMsg* msg) {
    cObject *obj = msg->getControlInfo();
    if (obj == NULL) {
        throw cRuntimeError("Moo");
    }

    inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
    return info;
}

Resolution Predictor::checkFlit(NoCFlitMsg* msg, SessionMeta* meta) {

    Resolution res = PREDICTION_IGNORE;

    if(debug) {
        AppFlitMsg *flit = (AppFlitMsg*)msg;
        cerr << "checkFlit: checking flit " << flit->getId();
        if(meta) {
            cerr << " which belongs to meta " << meta->getSessionId();
        }
        cerr << "\n";
    }

    if((NULL != meta) && (NOC_START_FLIT==msg->getType())) {

        if(SESSION_META_RESPONSE==meta->getState()) {
            /* results already have a prediction object which belongs to THIS object */
            res = m_predictor->checkFlit(msg, meta);
        } else {
            /* requests have a designated function which needs to be called *after* the OP result arrives.
             * the reason the need a designated function is because they need to register the message in
             * a remote predictor object and not */
        }

    } else {
        if(debug) {
            cerr << "checkFlit: Flit was not delegated to predictor\n";
            cerr << msg;
        }
    }
    return res;
}

bool Predictor::hasPrediction(NoCFlitMsg* msg) {
    return m_predictor->hasPrediction(msg);
}

bool Predictor::hasPrediction(SessionMeta* meta) {
    return m_predictor->hasPrediction(meta);
}

Resolution Predictor::registerFlit(NoCFlitMsg* msg, SessionMeta* meta) {
    Resolution res = PREDICTION_IGNORE;
    if((meta) && (NOC_START_FLIT==msg->getType())) {
        if(SESSION_META_REQUEST==meta->getState()){
            Predictor *predictor = getTargetPredictor(msg);
            res = predictor->m_predictor->checkFlit(msg, meta);
        } else {

        }
    }
    return res;
}
