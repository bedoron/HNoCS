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
//#ifndef PREDICTIONOBJECT_H_
//#define PREDICTIONOBJECT_H_
//#include "SessionMeta.h"
//
//#include "CMP_m.h"
//
//class PredictionObject: public cObject {
//    SessionMeta *m_meta;
//
//    // Terminology is with reference to the Request
//    uint32_t    m_vc_local;
//    uint32_t    m_vc_remote;
//    uint32_t    m_port_local;
//    uint32_t    m_port_remote;
//
//    simtime_t   m_delta; // Prediction
//
//    cObject     *m_originalControl;
//public:
//    PredictionObject(NoCFlitMsg *msg, uint32_t vc, uint32_t port);
//    // Set prediction
//    void setDelta(const simtime_t& delta);
//    // Remote port
//    void setRemotePort(uint32_t port);
//    // Remote VC
//    void setRemoteVc(uint32_t vc);
//
//    const bool operator==(const simtime_t& time) const;
//    const bool operator<(const simtime_t& time) const;
//
//    virtual ~PredictionObject();
//
//    cObject* getOriginalControl();
//    // make a composite out of the original control message of the request
//    // and return the request itself
//    NoCReqMsg* attachPrediction(NoCReqMsg *req);
//    NoCReqMsg* detachPrediction(NoCReqMsg *req);
//
//    // Extracts the prediction object and detaches it from the request
//    // if no prediction exists, return 0
//    static PredictionObject* ExtractPrediction(NoCReqMsg *req);
//};
//
//#endif /* PREDICTIONOBJECT_H_ */