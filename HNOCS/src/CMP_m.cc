//
// Generated file, do not edit! Created by opp_msgc 4.3 from CMP.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "CMP_m.h"

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




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("cmpOpType");
    if (!e) enums.getInstance()->add(e = new cEnum("cmpOpType"));
    e->insert(CMP_OP_READ, "CMP_OP_READ");
    e->insert(CMP_OP_WRITE, "CMP_OP_WRITE");
);

Register_Class(CMPMsg);

CMPMsg::CMPMsg(const char *name, int kind) : AppMsg(name,kind)
{
    this->origin_var = 0;
    this->operation_var = 0;
    this->l2hit_var = 0;
    this->address_var = 0;
    this->length_var = 0;
    this->roundtrip_var = false;
}

CMPMsg::CMPMsg(const CMPMsg& other) : AppMsg(other)
{
    copy(other);
}

CMPMsg::~CMPMsg()
{
}

CMPMsg& CMPMsg::operator=(const CMPMsg& other)
{
    if (this==&other) return *this;
    AppMsg::operator=(other);
    copy(other);
    return *this;
}

void CMPMsg::copy(const CMPMsg& other)
{
    this->origin_var = other.origin_var;
    this->operation_var = other.operation_var;
    this->l2hit_var = other.l2hit_var;
    this->address_var = other.address_var;
    this->length_var = other.length_var;
    this->roundtrip_var = other.roundtrip_var;
}

void CMPMsg::parsimPack(cCommBuffer *b)
{
    AppMsg::parsimPack(b);
    doPacking(b,this->origin_var);
    doPacking(b,this->operation_var);
    doPacking(b,this->l2hit_var);
    doPacking(b,this->address_var);
    doPacking(b,this->length_var);
    doPacking(b,this->roundtrip_var);
}

void CMPMsg::parsimUnpack(cCommBuffer *b)
{
    AppMsg::parsimUnpack(b);
    doUnpacking(b,this->origin_var);
    doUnpacking(b,this->operation_var);
    doUnpacking(b,this->l2hit_var);
    doUnpacking(b,this->address_var);
    doUnpacking(b,this->length_var);
    doUnpacking(b,this->roundtrip_var);
}

int CMPMsg::getOrigin() const
{
    return origin_var;
}

void CMPMsg::setOrigin(int origin)
{
    this->origin_var = origin;
}

int CMPMsg::getOperation() const
{
    return operation_var;
}

void CMPMsg::setOperation(int operation)
{
    this->operation_var = operation;
}

int CMPMsg::getL2hit() const
{
    return l2hit_var;
}

void CMPMsg::setL2hit(int l2hit)
{
    this->l2hit_var = l2hit;
}

int CMPMsg::getAddress() const
{
    return address_var;
}

void CMPMsg::setAddress(int address)
{
    this->address_var = address;
}

int CMPMsg::getLength() const
{
    return length_var;
}

void CMPMsg::setLength(int length)
{
    this->length_var = length;
}

bool CMPMsg::getRoundtrip() const
{
    return roundtrip_var;
}

void CMPMsg::setRoundtrip(bool roundtrip)
{
    this->roundtrip_var = roundtrip;
}

class CMPMsgDescriptor : public cClassDescriptor
{
  public:
    CMPMsgDescriptor();
    virtual ~CMPMsgDescriptor();

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

Register_ClassDescriptor(CMPMsgDescriptor);

CMPMsgDescriptor::CMPMsgDescriptor() : cClassDescriptor("CMPMsg", "AppMsg")
{
}

CMPMsgDescriptor::~CMPMsgDescriptor()
{
}

bool CMPMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<CMPMsg *>(obj)!=NULL;
}

const char *CMPMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int CMPMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int CMPMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *CMPMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "origin",
        "operation",
        "l2hit",
        "address",
        "length",
        "roundtrip",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int CMPMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "origin")==0) return base+0;
    if (fieldName[0]=='o' && strcmp(fieldName, "operation")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "l2hit")==0) return base+2;
    if (fieldName[0]=='a' && strcmp(fieldName, "address")==0) return base+3;
    if (fieldName[0]=='l' && strcmp(fieldName, "length")==0) return base+4;
    if (fieldName[0]=='r' && strcmp(fieldName, "roundtrip")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *CMPMsgDescriptor::getFieldTypeString(void *object, int field) const
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
        "bool",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *CMPMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int CMPMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    CMPMsg *pp = (CMPMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string CMPMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    CMPMsg *pp = (CMPMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getOrigin());
        case 1: return long2string(pp->getOperation());
        case 2: return long2string(pp->getL2hit());
        case 3: return long2string(pp->getAddress());
        case 4: return long2string(pp->getLength());
        case 5: return bool2string(pp->getRoundtrip());
        default: return "";
    }
}

bool CMPMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    CMPMsg *pp = (CMPMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setOrigin(string2long(value)); return true;
        case 1: pp->setOperation(string2long(value)); return true;
        case 2: pp->setL2hit(string2long(value)); return true;
        case 3: pp->setAddress(string2long(value)); return true;
        case 4: pp->setLength(string2long(value)); return true;
        case 5: pp->setRoundtrip(string2bool(value)); return true;
        default: return false;
    }
}

const char *CMPMsgDescriptor::getFieldStructName(void *object, int field) const
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

void *CMPMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    CMPMsg *pp = (CMPMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


