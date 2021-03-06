////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU Lesser General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU Lesser General Public License for more details.
////
//// You should have received a copy of the GNU Lesser General Public License
//// along with this program.  If not, see http://www.gnu.org/licenses/.
////
//
//#include "PredictionObject.h"
//#include "ResponseDB.h"
//
//PredictionObject::PredictionObject(NoCFlitMsg *msg, uint32_t vc, uint32_t port):
//    m_vc_local(vc), m_port_local(port), m_originalControl(0) {
//    m_meta = ResponseDB::getInstance()->find(msg);
//}
//
//PredictionObject::~PredictionObject() {
//    // TODO Auto-generated destructor stub
//}
//
//cObject* PredictionObject::getOriginalControl() {
//    return m_originalControl;
//}
//
//NoCReqMsg* PredictionObject::attachPrediction(NoCReqMsg *req) {
//    m_originalControl = req->getControlInfo();
//    req->setControlInfo((cMessage*)this);
//    return req;
//}
//
//NoCReqMsg* PredictionObject::detachPrediction(NoCReqMsg *req) {
//    req->setControlInfo(m_originalControl);
//    return req;
//}
//
//PredictionObject* PredictionObject::ExtractPrediction(NoCReqMsg *req)  {
//    PredictionObject *predObj = dynamic_cast<PredictionObject*>(req->getControlInfo());
//    if(0 != predObj) {
//        predObj->detachPrediction(req);
//    }
//    return predObj;
//}
