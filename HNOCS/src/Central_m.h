//
// Generated file, do not edit! Created by opp_msgc 4.3 from Central.msg.
//

#ifndef _CENTRAL_M_H_
#define _CENTRAL_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0403
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>Central.msg</tt> by opp_msgc.
 * <pre>
 * message CentralRequestMsg {
 *     int srcId;
 *     int dstId;
 *     int pktId;
 *     int flits;
 *     int age;
 * }
 * </pre>
 */
class CentralRequestMsg : public ::cMessage
{
  protected:
    int srcId_var;
    int dstId_var;
    int pktId_var;
    int flits_var;
    int age_var;

  private:
    void copy(const CentralRequestMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CentralRequestMsg&);

  public:
    CentralRequestMsg(const char *name=NULL, int kind=0);
    CentralRequestMsg(const CentralRequestMsg& other);
    virtual ~CentralRequestMsg();
    CentralRequestMsg& operator=(const CentralRequestMsg& other);
    virtual CentralRequestMsg *dup() const {return new CentralRequestMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSrcId() const;
    virtual void setSrcId(int srcId);
    virtual int getDstId() const;
    virtual void setDstId(int dstId);
    virtual int getPktId() const;
    virtual void setPktId(int pktId);
    virtual int getFlits() const;
    virtual void setFlits(int flits);
    virtual int getAge() const;
    virtual void setAge(int age);
};

inline void doPacking(cCommBuffer *b, CentralRequestMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, CentralRequestMsg& obj) {obj.parsimUnpack(b);}


#endif // _CENTRAL_M_H_
