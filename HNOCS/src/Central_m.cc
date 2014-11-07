//
// Generated file, do not edit! Created by opp_msgc 4.5 from Central.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Central_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(CentralRequestMsg);

CentralRequestMsg::CentralRequestMsg(const char *name, int kind) : ::cMessage(name,kind)
{
    this->srcId_var = 0;
    this->dstId_var = 0;
    this->pktId_var = 0;
    this->flits_var = 0;
    this->age_var = 0;
}

CentralRequestMsg::CentralRequestMsg(const CentralRequestMsg& other) : ::cMessage(other)
{
    copy(other);
}

CentralRequestMsg::~CentralRequestMsg()
{
}

CentralRequestMsg& CentralRequestMsg::operator=(const CentralRequestMsg& other)
{
    if (this==&other) return *this;
    ::cMessage::operator=(other);
    copy(other);
    return *this;
}

void CentralRequestMsg::copy(const CentralRequestMsg& other)
{
    this->srcId_var = other.srcId_var;
    this->dstId_var = other.dstId_var;
    this->pktId_var = other.pktId_var;
    this->flits_var = other.flits_var;
    this->age_var = other.age_var;
}

void CentralRequestMsg::parsimPack(cCommBuffer *b)
{
    ::cMessage::parsimPack(b);
    doPacking(b,this->srcId_var);
    doPacking(b,this->dstId_var);
    doPacking(b,this->pktId_var);
    doPacking(b,this->flits_var);
    doPacking(b,this->age_var);
}

void CentralRequestMsg::parsimUnpack(cCommBuffer *b)
{
    ::cMessage::parsimUnpack(b);
    doUnpacking(b,this->srcId_var);
    doUnpacking(b,this->dstId_var);
    doUnpacking(b,this->pktId_var);
    doUnpacking(b,this->flits_var);
    doUnpacking(b,this->age_var);
}

int CentralRequestMsg::getSrcId() const
{
    return srcId_var;
}

void CentralRequestMsg::setSrcId(int srcId)
{
    this->srcId_var = srcId;
}

int CentralRequestMsg::getDstId() const
{
    return dstId_var;
}

void CentralRequestMsg::setDstId(int dstId)
{
    this->dstId_var = dstId;
}

int CentralRequestMsg::getPktId() const
{
    return pktId_var;
}

void CentralRequestMsg::setPktId(int pktId)
{
    this->pktId_var = pktId;
}

int CentralRequestMsg::getFlits() const
{
    return flits_var;
}

void CentralRequestMsg::setFlits(int flits)
{
    this->flits_var = flits;
}

int CentralRequestMsg::getAge() const
{
    return age_var;
}

void CentralRequestMsg::setAge(int age)
{
    this->age_var = age;
}

class CentralRequestMsgDescriptor : public cClassDescriptor
{
  public:
    CentralRequestMsgDescriptor();
    virtual ~CentralRequestMsgDescriptor();

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

Register_ClassDescriptor(CentralRequestMsgDescriptor);

CentralRequestMsgDescriptor::CentralRequestMsgDescriptor() : cClassDescriptor("CentralRequestMsg", "cMessage")
{
}

CentralRequestMsgDescriptor::~CentralRequestMsgDescriptor()
{
}

bool CentralRequestMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<CentralRequestMsg *>(obj)!=NULL;
}

const char *CentralRequestMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int CentralRequestMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int CentralRequestMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *CentralRequestMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "srcId",
        "dstId",
        "pktId",
        "flits",
        "age",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int CentralRequestMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcId")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstId")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "pktId")==0) return base+2;
    if (fieldName[0]=='f' && strcmp(fieldName, "flits")==0) return base+3;
    if (fieldName[0]=='a' && strcmp(fieldName, "age")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *CentralRequestMsgDescriptor::getFieldTypeString(void *object, int field) const
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
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *CentralRequestMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int CentralRequestMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    CentralRequestMsg *pp = (CentralRequestMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string CentralRequestMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    CentralRequestMsg *pp = (CentralRequestMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSrcId());
        case 1: return long2string(pp->getDstId());
        case 2: return long2string(pp->getPktId());
        case 3: return long2string(pp->getFlits());
        case 4: return long2string(pp->getAge());
        default: return "";
    }
}

bool CentralRequestMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    CentralRequestMsg *pp = (CentralRequestMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcId(string2long(value)); return true;
        case 1: pp->setDstId(string2long(value)); return true;
        case 2: pp->setPktId(string2long(value)); return true;
        case 3: pp->setFlits(string2long(value)); return true;
        case 4: pp->setAge(string2long(value)); return true;
        default: return false;
    }
}

const char *CentralRequestMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *CentralRequestMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    CentralRequestMsg *pp = (CentralRequestMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


