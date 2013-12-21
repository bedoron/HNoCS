//
// Generated file, do not edit! Created by opp_msgc 4.3 from NoCs.msg.
//

#ifndef _NOCS_M_H_
#define _NOCS_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0403
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * enum NOC_MSGS
 * {
 *   NOC_FLIT_MSG    = 1; 
 *   NOC_CREDIT_MSG  = 2; 
 *   NOC_REQ_MSG     = 3;
 *   NOC_GNT_MSG     = 4;
 *   NOC_ACK_MSG     = 5;
 *   NOC_POP_MSG     = 7;
 *   NOC_GEN_MSG     = 8;
 *   NOC_APP_MSG 	  = 9;
 *   
 * }
 * </pre>
 */
enum NOC_MSGS {
    NOC_FLIT_MSG = 1,
    NOC_CREDIT_MSG = 2,
    NOC_REQ_MSG = 3,
    NOC_GNT_MSG = 4,
    NOC_ACK_MSG = 5,
    NOC_POP_MSG = 7,
    NOC_GEN_MSG = 8,
    NOC_APP_MSG = 9
};

/**
 * Enum generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * enum NOC_FLIT_TYPES
 * {
 *     NOC_START_FLIT = 1;
 *     NOC_MID_FLIT   = 2;
 *     NOC_END_FLIT   = 3;
 * }
 * </pre>
 */
enum NOC_FLIT_TYPES {
    NOC_START_FLIT = 1,
    NOC_MID_FLIT = 2,
    NOC_END_FLIT = 3
};

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * packet NoCFlitMsg
 * {
 *   int type;    
 *   int VC;      
 *   int SL;      
 *   int pktId;   
 *   int flits;   
 *   int flitIdx; 
 *   int srcId;
 *   int dstId;
 *   bool firstNet; 
 *   simtime_t InjectTime; 
 *   simtime_t FirstNetTime; 
 *   
 *   bool replicated = false; 
 *    
 * }
 * </pre>
 */
class NoCFlitMsg : public ::cPacket
{
  protected:
    int type_var;
    int VC_var;
    int SL_var;
    int pktId_var;
    int flits_var;
    int flitIdx_var;
    int srcId_var;
    int dstId_var;
    bool firstNet_var;
    simtime_t InjectTime_var;
    simtime_t FirstNetTime_var;
    bool replicated_var;

  private:
    void copy(const NoCFlitMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCFlitMsg&);

  public:
    NoCFlitMsg(const char *name=NULL, int kind=0);
    NoCFlitMsg(const NoCFlitMsg& other);
    virtual ~NoCFlitMsg();
    NoCFlitMsg& operator=(const NoCFlitMsg& other);
    virtual NoCFlitMsg *dup() const {return new NoCFlitMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getType() const;
    virtual void setType(int type);
    virtual int getVC() const;
    virtual void setVC(int VC);
    virtual int getSL() const;
    virtual void setSL(int SL);
    virtual int getPktId() const;
    virtual void setPktId(int pktId);
    virtual int getFlits() const;
    virtual void setFlits(int flits);
    virtual int getFlitIdx() const;
    virtual void setFlitIdx(int flitIdx);
    virtual int getSrcId() const;
    virtual void setSrcId(int srcId);
    virtual int getDstId() const;
    virtual void setDstId(int dstId);
    virtual bool getFirstNet() const;
    virtual void setFirstNet(bool firstNet);
    virtual simtime_t getInjectTime() const;
    virtual void setInjectTime(simtime_t InjectTime);
    virtual simtime_t getFirstNetTime() const;
    virtual void setFirstNetTime(simtime_t FirstNetTime);
    virtual bool getReplicated() const;
    virtual void setReplicated(bool replicated);
};

inline void doPacking(cCommBuffer *b, NoCFlitMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCFlitMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * message NoCCreditMsg extends cMessage
 * {
 *     int VC;
 *     int flits;
 *     int msgs;
 * }
 * </pre>
 */
class NoCCreditMsg : public ::cMessage
{
  protected:
    int VC_var;
    int flits_var;
    int msgs_var;

  private:
    void copy(const NoCCreditMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCCreditMsg&);

  public:
    NoCCreditMsg(const char *name=NULL, int kind=0);
    NoCCreditMsg(const NoCCreditMsg& other);
    virtual ~NoCCreditMsg();
    NoCCreditMsg& operator=(const NoCCreditMsg& other);
    virtual NoCCreditMsg *dup() const {return new NoCCreditMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getVC() const;
    virtual void setVC(int VC);
    virtual int getFlits() const;
    virtual void setFlits(int flits);
    virtual int getMsgs() const;
    virtual void setMsgs(int msgs);
};

inline void doPacking(cCommBuffer *b, NoCCreditMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCCreditMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * message NoCReqMsg extends cMessage 
 * {
 *     int outVC;      
 *     int inVC;       
 *     int outPortNum; 
 *     int pktId;      
 *     int numFlits;   
 *     int numGranted; 
 *     int numAcked;   
 *     bool prediction; 
 * }
 * </pre>
 */
class NoCReqMsg : public ::cMessage
{
  protected:
    int outVC_var;
    int inVC_var;
    int outPortNum_var;
    int pktId_var;
    int numFlits_var;
    int numGranted_var;
    int numAcked_var;
    bool prediction_var;

  private:
    void copy(const NoCReqMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCReqMsg&);

  public:
    NoCReqMsg(const char *name=NULL, int kind=0);
    NoCReqMsg(const NoCReqMsg& other);
    virtual ~NoCReqMsg();
    NoCReqMsg& operator=(const NoCReqMsg& other);
    virtual NoCReqMsg *dup() const {return new NoCReqMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getOutVC() const;
    virtual void setOutVC(int outVC);
    virtual int getInVC() const;
    virtual void setInVC(int inVC);
    virtual int getOutPortNum() const;
    virtual void setOutPortNum(int outPortNum);
    virtual int getPktId() const;
    virtual void setPktId(int pktId);
    virtual int getNumFlits() const;
    virtual void setNumFlits(int numFlits);
    virtual int getNumGranted() const;
    virtual void setNumGranted(int numGranted);
    virtual int getNumAcked() const;
    virtual void setNumAcked(int numAcked);
    virtual bool getPrediction() const;
    virtual void setPrediction(bool prediction);
};

inline void doPacking(cCommBuffer *b, NoCReqMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCReqMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * message NoCGntMsg extends cMessage 
 * {
 *     int outVC;      
 *     int inVC;       
 *     int outPortNum; 
 * }
 * </pre>
 */
class NoCGntMsg : public ::cMessage
{
  protected:
    int outVC_var;
    int inVC_var;
    int outPortNum_var;

  private:
    void copy(const NoCGntMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCGntMsg&);

  public:
    NoCGntMsg(const char *name=NULL, int kind=0);
    NoCGntMsg(const NoCGntMsg& other);
    virtual ~NoCGntMsg();
    NoCGntMsg& operator=(const NoCGntMsg& other);
    virtual NoCGntMsg *dup() const {return new NoCGntMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getOutVC() const;
    virtual void setOutVC(int outVC);
    virtual int getInVC() const;
    virtual void setInVC(int inVC);
    virtual int getOutPortNum() const;
    virtual void setOutPortNum(int outPortNum);
};

inline void doPacking(cCommBuffer *b, NoCGntMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCGntMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * message NoCAckMsg extends cMessage 
 * {
 *     bool OK;        
 *     int outVC;      
 *     int inVC;
 *     int outPortNum;
 * }
 * </pre>
 */
class NoCAckMsg : public ::cMessage
{
  protected:
    bool OK_var;
    int outVC_var;
    int inVC_var;
    int outPortNum_var;

  private:
    void copy(const NoCAckMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCAckMsg&);

  public:
    NoCAckMsg(const char *name=NULL, int kind=0);
    NoCAckMsg(const NoCAckMsg& other);
    virtual ~NoCAckMsg();
    NoCAckMsg& operator=(const NoCAckMsg& other);
    virtual NoCAckMsg *dup() const {return new NoCAckMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual bool getOK() const;
    virtual void setOK(bool OK);
    virtual int getOutVC() const;
    virtual void setOutVC(int outVC);
    virtual int getInVC() const;
    virtual void setInVC(int inVC);
    virtual int getOutPortNum() const;
    virtual void setOutPortNum(int outPortNum);
};

inline void doPacking(cCommBuffer *b, NoCAckMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCAckMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>NoCs.msg</tt> by opp_msgc.
 * <pre>
 * message NoCPopMsg extends cMessage
 * {
 *     int VC;
 *     int outPortNum;
 * }
 * </pre>
 */
class NoCPopMsg : public ::cMessage
{
  protected:
    int VC_var;
    int outPortNum_var;

  private:
    void copy(const NoCPopMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NoCPopMsg&);

  public:
    NoCPopMsg(const char *name=NULL, int kind=0);
    NoCPopMsg(const NoCPopMsg& other);
    virtual ~NoCPopMsg();
    NoCPopMsg& operator=(const NoCPopMsg& other);
    virtual NoCPopMsg *dup() const {return new NoCPopMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getVC() const;
    virtual void setVC(int VC);
    virtual int getOutPortNum() const;
    virtual void setOutPortNum(int outPortNum);
};

inline void doPacking(cCommBuffer *b, NoCPopMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NoCPopMsg& obj) {obj.parsimUnpack(b);}


#endif // _NOCS_M_H_
