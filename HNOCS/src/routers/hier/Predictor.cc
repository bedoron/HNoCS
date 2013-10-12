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
//#include "InPortSync.h"
#include <string>
#include <iostream>
using std::string;
using std::cerr;

Define_Module(Predictor);

void Predictor::initialize()
{
    string method(par("method").stringValue());

    EV << "Loading " << method << " predictor\n";

    // Build predictor
    m_predictor = PredictorFactory::instance().build(method);

    m_numVCs = getParentModule()->getSubmodule("inPort")->par("numVCs");

    m_VCHit.resize(m_numVCs);
    for(int i=0; i < m_numVCs; ++i) {
        m_VCHit[i] = NULL;
    }

//    cModule *opCalc = getParentModule()->getSubmodule("inPort");
    cModule *opCalc = getParentModule()->getSubmodule("opCalc");
    cModule *vcCalc = getParentModule()->getSubmodule("vcCalc");
    cModule *sched  = getParentModule()->getSubmodule("sched");


    if(NULL == opCalc || NULL == vcCalc || NULL == sched) {
        throw cRuntimeError(getParentModule(), "Can't find one of the main modules for the port");
    }

//    m_inPort = check_and_cast<InPortSync*>(inPort);
    m_opCalc = check_and_cast<XYOPCalc*>(opCalc);
    m_vcCalc = check_and_cast<FLUVCCalc*>(vcCalc);
//    m_sched = check_and_cast<Sched*>(sched);
}

void Predictor::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

bool Predictor::CheckIfHit(SessionMeta *meta) {
    bool isHit = false;

    PredictionTable::iterator prediction = m_predictionTable.find(meta);
    if(prediction != m_predictionTable.end()) {
        PredictionInterval interval = prediction->second;
        simtime_t now = cSimulation::getActiveSimulation()->getSimTime();

        if((now >= interval.first)  && (now <= interval.second)) {
            isHit = true;
        } else {
            isHit = false;
        }
    }
    return isHit;
}

void Predictor::HitFlowStart(NoCFlitMsg* msg, SessionMeta* meta) {
    int inVC = msg->getVC(); // We assume this is still the inVC
    m_VCHit[inVC] = meta;
}

bool Predictor::Hit(NoCFlitMsg* msg) {
    bool isHit = false;
    int msgId = msg->getId();
    SessionMeta *meta = ResponseDB::getInstance()->find(msgId);
    if(meta->isResponse(msgId)) {
        isHit = CheckIfHit(meta);

        if(true == isHit) { // Start Hit flow
            HitFlowStart(msg, meta);
        }

    } else {
        cerr << "Trying to match a hit to a non response message :( \n";
    }
    return isHit;
}

//InPortSync& Predictor::getInPort() {
//    return *m_inPort;
//}

 XYOPCalc& Predictor::getOpCalc()  {
    return *m_opCalc;
}

// SchedSync& Predictor::getSched()  {
//    return *m_sched;
//}

bool Predictor::Hit(int inVC) {
    return (NULL != m_VCHit[inVC]);
}


bool Predictor::Predict(NoCFlitMsg* request, SessionMeta* meta) {

    if(m_predictionTable.find(meta)!=m_predictionTable.end()) {
        cRuntimeError("Trying to add a prediction for a session which already exists");
        return false;
    }

    m_predictionTable[meta] = m_predictor->predict(request);
    return true;
}

bool Predictor::Predict(NoCFlitMsg* request) {
    bool retVal = false;
    SessionMeta *meta = ResponseDB::getInstance()->find(request->getId()); // TODO: check if ok
    if((NULL != meta) && (meta->isRequest(request->getId()))) {

        retVal = Predict(request, meta);
    } else {
        cerr << "Predictor error: can't find session meta, not a request/response?\n";
    }

    return retVal;
}

//void Predictor::RegisterRemoteTable(SessionMeta* meta,
//        PredictionInterval predInt) {
//    Predict(request, meta);
//}

bool Predictor::PredictIfRequest(NoCFlitMsg* msg, int outPort) {
    bool isPredicted = false;
    if(msg->getFlitIdx()!=0) {
        cerr << " Trying to Predict for a non head flit, ignoring\n";
        isPredicted = false;
    } else {

        SessionMeta *meta = ResponseDB::getInstance()->find(msg->getId());
        if(meta->isRequest(msg->getId())) {
            cGate *gate = getParentModule()->gate("sw_in",outPort);
            if (!gate) cRuntimeError("outport is weird");
            cGate *remGate = gate->getPathEndGate()->getPreviousGate();
            if (!remGate) cRuntimeError("remote gate is weird");
            cModule *neighbour = remGate->getOwnerModule();
            if(!neighbour) cRuntimeError("remote port is weird");
            Predictor *adjPred = check_and_cast<Predictor*>(neighbour->getSubmodule("predictor"));
            adjPred->Predict(msg, meta);
        }

    }

    return isPredicted;
}

 FLUVCCalc& Predictor::getVcCalc()  {
    return *m_vcCalc;
}

void Predictor::DestroyHit(int inVC) {
    if(NULL != m_VCHit[inVC]) {
        SessionMeta *meta = m_VCHit[inVC];
        m_VCHit[inVC] = NULL;
        PredictionTable::iterator prediction = m_predictionTable.find(meta);
        if(prediction != m_predictionTable.end()) {
            m_predictionTable.erase(prediction);
        } else {
            throw cRuntimeError("Trying to remove a meta that was marked as a hit but didn't have a prediction");
        }
    }
}
