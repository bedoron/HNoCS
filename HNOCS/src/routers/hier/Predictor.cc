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
#include "Utils.h"
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

    m_portIndex = getParentModule()->getIndex();
    m_routerIndex = getParentModule()->getParentModule()->getIndex();


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

    m_printData = PRINT_DATA;
}

void Predictor::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

bool Predictor::CheckIfHit(SessionMeta *meta, NoCFlitMsg* nocMsg) {
//    bool isHit = false;
//
//    PredictionTable::iterator prediction = m_predictionTable.find(meta);
//    if(prediction != m_predictionTable.end()) {
//        PredictionInterval interval = prediction->second;
//        simtime_t now = cSimulation::getActiveSimulation()->getSimTime();
//
//        EV << "[" << now << "] Predicted interval is " << interval.first << "-" << interval.second << "\n";
//
//        if((now >= interval.first)  && (now <= interval.second)) {
//            isHit = true;
////            cerr << "Hit\n";
//        } else {
//            isHit = false;
////            if(m_predictor->getName()!="Default") {
////                cerr << "Miss\n";
////            }
//        }
//
//    } else {
//        cerr << "*********** CheckIfHit FAILURE **************\n";
//        cerr << "Flit doesn't have a prediction object but all\n";
//        cerr << "responses should have one, aborting violently\n";
//        cerr << "     THROWING EXCEPTION AND QUITTING\n";
//        cerr << "*********************************************\n";
//        cerr << nocMsg;
//
//        throw cRuntimeError("Predictor::CheckIfHit - Meta not found in prediction table");
//    }
//
//    return isHit;
}


bool Predictor::Hit(NoCFlitMsg* nocMsg) {
    AppFlitMsg* msg = dynamic_cast<AppFlitMsg*>(nocMsg);
    bool isHit = false;
    int msgId = msg->getId(); //msg->getId();
    SessionMeta *meta = ResponseDB::getInstance()->find(msgId);
    // meta could be null if its a write request

    if((NULL != meta) && meta->isResponse(msgId)) {
        simtime_t now = cSimulation::getActiveSimulation()->getSimTime();
        EV << "[" << now << "] Checking response prediction for pkt " << msg->getPktId() << "\n";
        isHit = CheckIfHit(meta, nocMsg);

        if(true == isHit) { // Start Hit flow
            EV << "[" << now << "] HIT!\n";
            HitFlowStart(msg, meta);
        } else {
            EV << "[" << now << "] MISS :-(\n";
        }
    } else {
        // Requests are always a miss
        isHit = false;
    }
    return isHit;
}

 XYOPCalc& Predictor::getOpCalc()  {
    return *m_opCalc;
}

bool Predictor::Hit(SessionMeta* meta) {

}


/**
 * Call self predictor object to register request with meta
 */
bool Predictor::Predict(NoCFlitMsg* request, SessionMeta* meta) {
//    AppFlitMsg *msg = (AppFlitMsg*)request;

    PredictionInterval interval = m_predictor->predict(request, meta);

    return true;
}

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


bool Predictor::PredictIfRequest(NoCFlitMsg* msg) {
    AppFlitMsg *flit = (AppFlitMsg*)msg;
    bool isPredicted = false;

    int flitIdx = flit->getFlitIdx();
    int pktIdx = flit->getPktIdx();
    int flitType = flit->getType();

    if(flitType!=NOC_START_FLIT) { /* Flit isn't a start flit */
        cerr << msg;
        throw cRuntimeError("PredictIfRequest received a non start flit, aborting");
    } else { /* Flit is a start flit */
        if(pktIdx==0) { /* Flit is first of flow */
            SessionMeta *meta = ResponseDB::getInstance()->find(msg->getId());
            if(NULL != meta) { /* Flit is part of a request-response flow*/
                if(meta->isRequest(msg)) {
                    Predictor *target = getTargetPredictor(msg);
                    isPredicted = target->Predict(msg, meta);;
                }
            } else {
                /* NOP */
            }
        }
    }

    return isPredicted;
}

 FLUVCCalc& Predictor::getVcCalc()  {
    return *m_vcCalc;
}

void Predictor::DestroyHit(SessionMeta *meta) {
//    PredictionTable::iterator iter = m_predictionTable.find(meta);
//    if(iter != m_predictionTable.end()) {
//        m_predictionTable.erase(iter);
//    } else {
//        cerr << "Trying to destroy session " << meta->getSessionId() << " which isn't registered\n";
//        throw cRuntimeError("Trying to remove a meta that was marked as a hit but didn't have a prediction");
//    }
}

Predictor::~Predictor() {
    if(m_printData) {
        cerr << "Destroying predictor for router " << m_routerIndex << " on port " << m_portIndex << "\n";
        cerr << "The contents of its Database is: \n";
        PredictionTable::iterator it = m_predictionTable.begin();
        for(;it != m_predictionTable.end(); ++it) {

            cerr << "Session ID - " << (it->first->getSessionId()) << "\n";
        }
        cerr << "***************************************\n";
    }
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
        throw cRuntimeError("-E- %s BUG - No Control Info for FLIT: %s",
                getFullPath().c_str(), msg->getFullName());
    }

    inPortFlitInfo *info = dynamic_cast<inPortFlitInfo*> (obj);
    return info;
}

SessionMeta* Predictor::getVCHit(int vc) {
    return m_VCHit[vc];
}
