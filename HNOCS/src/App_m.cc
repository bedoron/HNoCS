//
// Generated file, do not edit! Created by opp_msgc 4.3 from App.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "App_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(AppFlitMsg);

AppFlitMsg::AppFlitMsg(const char *name, int kind) : NoCFlitMsg(name,kind)
{
    this->srcAppId_var = 0;
    this->dstAppId_var = 0;
    this->pktIdx_var = 0;
    this->appMsgLen_var = 0;
    this->msgId_var = 0;
    this->sessionId_var = 0;
}

AppFlitMsg::AppFlitMsg(const AppFlitMsg& other) : NoCFlitMsg(other)
{
    copy(other);
}

AppFlitMsg::~AppFlitMsg()
{
}

AppFlitMsg& AppFlitMsg::operator=(const AppFlitMsg& other)
{
    if (this==&other) return *this;
    NoCFlitMsg::operator=(other);
    copy(other);
    return *this;
}

void AppFlitMsg::copy(const AppFlitMsg& other)
{
    this->srcAppId_var = other.srcAppId_var;
    this->dstAppId_var = other.dstAppId_var;
    this->pktIdx_var = other.pktIdx_var;
    this->appMsgLen_var = other.appMsgLen_var;
    this->msgId_var = other.msgId_var;
    this->sessionId_var = other.sessionId_var;
}

void AppFlitMsg::parsimPack(cCommBuffer *b)
{
    NoCFlitMsg::parsimPack(b);
    doPacking(b,this->srcAppId_var);
    doPacking(b,this->dstAppId_var);
    doPacking(b,this->pktIdx_var);
    doPacking(b,this->appMsgLen_var);
    doPacking(b,this->msgId_var);
    doPacking(b,this->sessionId_var);
}

void AppFlitMsg::parsimUnpack(cCommBuffer *b)
{
    NoCFlitMsg::parsimUnpack(b);
    doUnpacking(b,this->srcAppId_var);
    doUnpacking(b,this->dstAppId_var);
    doUnpacking(b,this->pktIdx_var);
    doUnpacking(b,this->appMsgLen_var);
    doUnpacking(b,this->msgId_var);
    doUnpacking(b,this->sessionId_var);
}

int AppFlitMsg::getSrcAppId() const
{
    return srcAppId_var;
}

void AppFlitMsg::setSrcAppId(int srcAppId)
{
    this->srcAppId_var = srcAppId;
}

int AppFlitMsg::getDstAppId() const
{
    return dstAppId_var;
}

void AppFlitMsg::setDstAppId(int dstAppId)
{
    this->dstAppId_var = dstAppId;
}

int AppFlitMsg::getPktIdx() const
{
    return pktIdx_var;
}

void AppFlitMsg::setPktIdx(int pktIdx)
{
    this->pktIdx_var = pktIdx;
}

int AppFlitMsg::getAppMsgLen() const
{
    return appMsgLen_var;
}

void AppFlitMsg::setAppMsgLen(int appMsgLen)
{
    this->appMsgLen_var = appMsgLen;
}

int AppFlitMsg::getMsgId() const
{
    return msgId_var;
}

void AppFlitMsg::setMsgId(int msgId)
{
    this->msgId_var = msgId;
}

unsigned int AppFlitMsg::getSessionId() const
{
    return sessionId_var;
}

void AppFlitMsg::setSessionId(unsigned int sessionId)
{
    this->sessionId_var = sessionId;
}

class AppFlitMsgDescriptor : public cClassDescriptor
{
  public:
    AppFlitMsgDescriptor();
    virtual ~AppFlitMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(AppFlitMsgDescriptor);

AppFlitMsgDescriptor::AppFlitMsgDescriptor() : cClassDescriptor("AppFlitMsg", "NoCFlitMsg")
{
}

AppFlitMsgDescriptor::~AppFlitMsgDescriptor()
{
}

bool AppFlitMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<AppFlitMsg *>(obj)!=NULL;
}

const char *AppFlitMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int AppFlitMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int AppFlitMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *AppFlitMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "srcAppId",
        "dstAppId",
        "pktIdx",
        "appMsgLen",
        "msgId",
        "sessionId",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int AppFlitMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAppId")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstAppId")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "pktIdx")==0) return base+2;
    if (fieldName[0]=='a' && strcmp(fieldName, "appMsgLen")==0) return base+3;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgId")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "sessionId")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *AppFlitMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "unsigned int",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *AppFlitMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int AppFlitMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    AppFlitMsg *pp = (AppFlitMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string AppFlitMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    AppFlitMsg *pp = (AppFlitMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSrcAppId());
        case 1: return long2string(pp->getDstAppId());
        case 2: return long2string(pp->getPktIdx());
        case 3: return long2string(pp->getAppMsgLen());
        case 4: return long2string(pp->getMsgId());
        case 5: return ulong2string(pp->getSessionId());
        default: return "";
    }
}

bool AppFlitMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    AppFlitMsg *pp = (AppFlitMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcAppId(string2long(value)); return true;
        case 1: pp->setDstAppId(string2long(value)); return true;
        case 2: pp->setPktIdx(string2long(value)); return true;
        case 3: pp->setAppMsgLen(string2long(value)); return true;
        case 4: pp->setMsgId(string2long(value)); return true;
        case 5: pp->setSessionId(string2ulong(value)); return true;
        default: return false;
    }
}

const char *AppFlitMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *AppFlitMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    AppFlitMsg *pp = (AppFlitMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(AppMsg);

AppMsg::AppMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->msgId_var = 0;
    this->srcAppId_var = 0;
    this->dstAppId_var = 0;
    this->appMsgLen_var = 0;
    this->pktLength_var = 0;
    this->dstId_var = 0;
    this->srcId_var = 0;
    this->VC_var = 0;
    this->origBytes_var = 0;
    this->InjectTime_var = 0;
}

AppMsg::AppMsg(const AppMsg& other) : cPacket(other)
{
    copy(other);
}

AppMsg::~AppMsg()
{
}

AppMsg& AppMsg::operator=(const AppMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void AppMsg::copy(const AppMsg& other)
{
    this->msgId_var = other.msgId_var;
    this->srcAppId_var = other.srcAppId_var;
    this->dstAppId_var = other.dstAppId_var;
    this->appMsgLen_var = other.appMsgLen_var;
    this->pktLength_var = other.pktLength_var;
    this->dstId_var = other.dstId_var;
    this->srcId_var = other.srcId_var;
    this->VC_var = other.VC_var;
    this->origBytes_var = other.origBytes_var;
    this->InjectTime_var = other.InjectTime_var;
}

void AppMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgId_var);
    doPacking(b,this->srcAppId_var);
    doPacking(b,this->dstAppId_var);
    doPacking(b,this->appMsgLen_var);
    doPacking(b,this->pktLength_var);
    doPacking(b,this->dstId_var);
    doPacking(b,this->srcId_var);
    doPacking(b,this->VC_var);
    doPacking(b,this->origBytes_var);
    doPacking(b,this->InjectTime_var);
}

void AppMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgId_var);
    doUnpacking(b,this->srcAppId_var);
    doUnpacking(b,this->dstAppId_var);
    doUnpacking(b,this->appMsgLen_var);
    doUnpacking(b,this->pktLength_var);
    doUnpacking(b,this->dstId_var);
    doUnpacking(b,this->srcId_var);
    doUnpacking(b,this->VC_var);
    doUnpacking(b,this->origBytes_var);
    doUnpacking(b,this->InjectTime_var);
}

int AppMsg::getMsgId() const
{
    return msgId_var;
}

void AppMsg::setMsgId(int msgId)
{
    this->msgId_var = msgId;
}

int AppMsg::getSrcAppId() const
{
    return srcAppId_var;
}

void AppMsg::setSrcAppId(int srcAppId)
{
    this->srcAppId_var = srcAppId;
}

int AppMsg::getDstAppId() const
{
    return dstAppId_var;
}

void AppMsg::setDstAppId(int dstAppId)
{
    this->dstAppId_var = dstAppId;
}

int AppMsg::getAppMsgLen() const
{
    return appMsgLen_var;
}

void AppMsg::setAppMsgLen(int appMsgLen)
{
    this->appMsgLen_var = appMsgLen;
}

int AppMsg::getPktLength() const
{
    return pktLength_var;
}

void AppMsg::setPktLength(int pktLength)
{
    this->pktLength_var = pktLength;
}

int AppMsg::getDstId() const
{
    return dstId_var;
}

void AppMsg::setDstId(int dstId)
{
    this->dstId_var = dstId;
}

int AppMsg::getSrcId() const
{
    return srcId_var;
}

void AppMsg::setSrcId(int srcId)
{
    this->srcId_var = srcId;
}

int AppMsg::getVC() const
{
    return VC_var;
}

void AppMsg::setVC(int VC)
{
    this->VC_var = VC;
}

int AppMsg::getOrigBytes() const
{
    return origBytes_var;
}

void AppMsg::setOrigBytes(int origBytes)
{
    this->origBytes_var = origBytes;
}

simtime_t AppMsg::getInjectTime() const
{
    return InjectTime_var;
}

void AppMsg::setInjectTime(simtime_t InjectTime)
{
    this->InjectTime_var = InjectTime;
}

class AppMsgDescriptor : public cClassDescriptor
{
  public:
    AppMsgDescriptor();
    virtual ~AppMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(AppMsgDescriptor);

AppMsgDescriptor::AppMsgDescriptor() : cClassDescriptor("AppMsg", "cPacket")
{
}

AppMsgDescriptor::~AppMsgDescriptor()
{
}

bool AppMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<AppMsg *>(obj)!=NULL;
}

const char *AppMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int AppMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 10+basedesc->getFieldCount(object) : 10;
}

unsigned int AppMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<10) ? fieldTypeFlags[field] : 0;
}

const char *AppMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "msgId",
        "srcAppId",
        "dstAppId",
        "appMsgLen",
        "pktLength",
        "dstId",
        "srcId",
        "VC",
        "origBytes",
        "InjectTime",
    };
    return (field>=0 && field<10) ? fieldNames[field] : NULL;
}

int AppMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAppId")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstAppId")==0) return base+2;
    if (fieldName[0]=='a' && strcmp(fieldName, "appMsgLen")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "pktLength")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstId")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcId")==0) return base+6;
    if (fieldName[0]=='V' && strcmp(fieldName, "VC")==0) return base+7;
    if (fieldName[0]=='o' && strcmp(fieldName, "origBytes")==0) return base+8;
    if (fieldName[0]=='I' && strcmp(fieldName, "InjectTime")==0) return base+9;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *AppMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "simtime_t",
    };
    return (field>=0 && field<10) ? fieldTypeStrings[field] : NULL;
}

const char *AppMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int AppMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    AppMsg *pp = (AppMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string AppMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    AppMsg *pp = (AppMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgId());
        case 1: return long2string(pp->getSrcAppId());
        case 2: return long2string(pp->getDstAppId());
        case 3: return long2string(pp->getAppMsgLen());
        case 4: return long2string(pp->getPktLength());
        case 5: return long2string(pp->getDstId());
        case 6: return long2string(pp->getSrcId());
        case 7: return long2string(pp->getVC());
        case 8: return long2string(pp->getOrigBytes());
        case 9: return double2string(pp->getInjectTime());
        default: return "";
    }
}

bool AppMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    AppMsg *pp = (AppMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgId(string2long(value)); return true;
        case 1: pp->setSrcAppId(string2long(value)); return true;
        case 2: pp->setDstAppId(string2long(value)); return true;
        case 3: pp->setAppMsgLen(string2long(value)); return true;
        case 4: pp->setPktLength(string2long(value)); return true;
        case 5: pp->setDstId(string2long(value)); return true;
        case 6: pp->setSrcId(string2long(value)); return true;
        case 7: pp->setVC(string2long(value)); return true;
        case 8: pp->setOrigBytes(string2long(value)); return true;
        case 9: pp->setInjectTime(string2double(value)); return true;
        default: return false;
    }
}

const char *AppMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<10) ? fieldStructNames[field] : NULL;
}

void *AppMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    AppMsg *pp = (AppMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(AppTestMsg);

AppTestMsg::AppTestMsg(const char *name, int kind) : AppMsg(name,kind)
{
    this->payload_var = 0;
}

AppTestMsg::AppTestMsg(const AppTestMsg& other) : AppMsg(other)
{
    copy(other);
}

AppTestMsg::~AppTestMsg()
{
}

AppTestMsg& AppTestMsg::operator=(const AppTestMsg& other)
{
    if (this==&other) return *this;
    AppMsg::operator=(other);
    copy(other);
    return *this;
}

void AppTestMsg::copy(const AppTestMsg& other)
{
    this->payload_var = other.payload_var;
}

void AppTestMsg::parsimPack(cCommBuffer *b)
{
    AppMsg::parsimPack(b);
    doPacking(b,this->payload_var);
}

void AppTestMsg::parsimUnpack(cCommBuffer *b)
{
    AppMsg::parsimUnpack(b);
    doUnpacking(b,this->payload_var);
}

const char * AppTestMsg::getPayload() const
{
    return payload_var.c_str();
}

void AppTestMsg::setPayload(const char * payload)
{
    this->payload_var = payload;
}

class AppTestMsgDescriptor : public cClassDescriptor
{
  public:
    AppTestMsgDescriptor();
    virtual ~AppTestMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(AppTestMsgDescriptor);

AppTestMsgDescriptor::AppTestMsgDescriptor() : cClassDescriptor("AppTestMsg", "AppMsg")
{
}

AppTestMsgDescriptor::~AppTestMsgDescriptor()
{
}

bool AppTestMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<AppTestMsg *>(obj)!=NULL;
}

const char *AppTestMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int AppTestMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int AppTestMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *AppTestMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "payload",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int AppTestMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *AppTestMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *AppTestMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int AppTestMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    AppTestMsg *pp = (AppTestMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string AppTestMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    AppTestMsg *pp = (AppTestMsg *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getPayload());
        default: return "";
    }
}

bool AppTestMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    AppTestMsg *pp = (AppTestMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setPayload((value)); return true;
        default: return false;
    }
}

const char *AppTestMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *AppTestMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    AppTestMsg *pp = (AppTestMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


