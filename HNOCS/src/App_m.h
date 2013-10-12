//
// Generated file, do not edit! Created by opp_msgc 4.3 from App.msg.
//

#ifndef _APP_M_H_
#define _APP_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0403
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include <NoCs_m.h>
// }}



/**
 * Class generated from <tt>App.msg</tt> by opp_msgc.
 * <pre>
 * packet AppFlitMsg extends NoCFlitMsg {
 *     int srcAppId; 
 *     int dstAppId; 
 *     int pktIdx;   
 *     int appMsgLen;   
 *     int msgId;    
 * }
 * </pre>
 */
class AppFlitMsg : public ::NoCFlitMsg
{
  protected:
    int srcAppId_var;
    int dstAppId_var;
    int pktIdx_var;
    int appMsgLen_var;
    int msgId_var;

  private:
    void copy(const AppFlitMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AppFlitMsg&);

  public:
    AppFlitMsg(const char *name=NULL, int kind=0);
    AppFlitMsg(const AppFlitMsg& other);
    virtual ~AppFlitMsg();
    AppFlitMsg& operator=(const AppFlitMsg& other);
    virtual AppFlitMsg *dup() const {return new AppFlitMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSrcAppId() const;
    virtual void setSrcAppId(int srcAppId);
    virtual int getDstAppId() const;
    virtual void setDstAppId(int dstAppId);
    virtual int getPktIdx() const;
    virtual void setPktIdx(int pktIdx);
    virtual int getAppMsgLen() const;
    virtual void setAppMsgLen(int appMsgLen);
    virtual int getMsgId() const;
    virtual void setMsgId(int msgId);
};

inline void doPacking(cCommBuffer *b, AppFlitMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, AppFlitMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>App.msg</tt> by opp_msgc.
 * <pre>
 * packet AppMsg
 * {
 *     int msgId;     
 *     int srcAppId;
 *     int dstAppId;
 *     int appMsgLen; 
 *     int pktLength; 
 *     int dstId;
 *     int srcId;
 *     int VC;        
 *     int origBytes; 
 *     simtime_t InjectTime;
 * }
 * </pre>
 */
class AppMsg : public ::cPacket
{
  protected:
    int msgId_var;
    int srcAppId_var;
    int dstAppId_var;
    int appMsgLen_var;
    int pktLength_var;
    int dstId_var;
    int srcId_var;
    int VC_var;
    int origBytes_var;
    simtime_t InjectTime_var;

  private:
    void copy(const AppMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AppMsg&);

  public:
    AppMsg(const char *name=NULL, int kind=0);
    AppMsg(const AppMsg& other);
    virtual ~AppMsg();
    AppMsg& operator=(const AppMsg& other);
    virtual AppMsg *dup() const {return new AppMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getMsgId() const;
    virtual void setMsgId(int msgId);
    virtual int getSrcAppId() const;
    virtual void setSrcAppId(int srcAppId);
    virtual int getDstAppId() const;
    virtual void setDstAppId(int dstAppId);
    virtual int getAppMsgLen() const;
    virtual void setAppMsgLen(int appMsgLen);
    virtual int getPktLength() const;
    virtual void setPktLength(int pktLength);
    virtual int getDstId() const;
    virtual void setDstId(int dstId);
    virtual int getSrcId() const;
    virtual void setSrcId(int srcId);
    virtual int getVC() const;
    virtual void setVC(int VC);
    virtual int getOrigBytes() const;
    virtual void setOrigBytes(int origBytes);
    virtual simtime_t getInjectTime() const;
    virtual void setInjectTime(simtime_t InjectTime);
};

inline void doPacking(cCommBuffer *b, AppMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, AppMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>App.msg</tt> by opp_msgc.
 * <pre>
 * packet AppTestMsg extends AppMsg
 * {
 *     string payload;
 * }
 * </pre>
 */
class AppTestMsg : public ::AppMsg
{
  protected:
    opp_string payload_var;

  private:
    void copy(const AppTestMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AppTestMsg&);

  public:
    AppTestMsg(const char *name=NULL, int kind=0);
    AppTestMsg(const AppTestMsg& other);
    virtual ~AppTestMsg();
    AppTestMsg& operator=(const AppTestMsg& other);
    virtual AppTestMsg *dup() const {return new AppTestMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getPayload() const;
    virtual void setPayload(const char * payload);
};

inline void doPacking(cCommBuffer *b, AppTestMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, AppTestMsg& obj) {obj.parsimUnpack(b);}


#endif // _APP_M_H_
