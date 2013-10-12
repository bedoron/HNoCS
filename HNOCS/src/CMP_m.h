//
// Generated file, do not edit! Created by opp_msgc 4.3 from CMP.msg.
//

#ifndef _CMP_M_H_
#define _CMP_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0403
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "App_m.h"
// }}



/**
 * Enum generated from <tt>CMP.msg</tt> by opp_msgc.
 * <pre>
 * enum cmpOpType { CMP_OP_READ = 1; CMP_OP_WRITE = 2; };
 * </pre>
 */
enum cmpOpType {
    CMP_OP_READ = 1,
    CMP_OP_WRITE = 2
};

/**
 * Class generated from <tt>CMP.msg</tt> by opp_msgc.
 * <pre>
 * packet CMPMsg extends AppMsg {
 *     int origin;     
 *     int operation;  
 *     int l2hit;      
 *     int address;
 *     int length;     
 * 	bool roundtrip = false; 
 * }
 * </pre>
 */
class CMPMsg : public ::AppMsg
{
  protected:
    int origin_var;
    int operation_var;
    int l2hit_var;
    int address_var;
    int length_var;
    bool roundtrip_var;

  private:
    void copy(const CMPMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CMPMsg&);

  public:
    CMPMsg(const char *name=NULL, int kind=0);
    CMPMsg(const CMPMsg& other);
    virtual ~CMPMsg();
    CMPMsg& operator=(const CMPMsg& other);
    virtual CMPMsg *dup() const {return new CMPMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getOrigin() const;
    virtual void setOrigin(int origin);
    virtual int getOperation() const;
    virtual void setOperation(int operation);
    virtual int getL2hit() const;
    virtual void setL2hit(int l2hit);
    virtual int getAddress() const;
    virtual void setAddress(int address);
    virtual int getLength() const;
    virtual void setLength(int length);
    virtual bool getRoundtrip() const;
    virtual void setRoundtrip(bool roundtrip);
};

inline void doPacking(cCommBuffer *b, CMPMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, CMPMsg& obj) {obj.parsimUnpack(b);}


#endif // _CMP_M_H_