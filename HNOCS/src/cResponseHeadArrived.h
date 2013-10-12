#ifndef _CRESPONSEHEADARRIVAL_H
#define _CRESPONSEHEADARRIVAL_H
#include <cobject.h>

// Forward declerations
class SessionMeta;
class NoCFlitMsg;

class cResponseHeadArrived: public cObject, noncopyable {
public:
    cResponseHeadArrived(cModule *rtr, NoCFlitMsg* flt, SessionMeta* meta, int inp = -1, int outp = -1):
        router(rtr), flit(flt), session(meta), inPort(inp), outPort(outp) { };

    cModule     *router;
    NoCFlitMsg  *flit;
    SessionMeta *session;

    int inPort;
    int outPort;
};





#endif
