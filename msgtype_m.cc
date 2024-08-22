//
// Generated file, do not edit! Created by opp_msgtool 6.0 from msgtype.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "msgtype_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

MQEntry::MQEntry()
{
}

MQEntry::MQEntry(const MQEntry& other)
{
    copy(other);
}

MQEntry::~MQEntry()
{
}

MQEntry& MQEntry::operator=(const MQEntry& other)
{
    if (this == &other) return *this;
    copy(other);
    return *this;
}

void MQEntry::copy(const MQEntry& other)
{
    this->l_id = other.l_id;
    this->l_clock = other.l_clock;
    this->text = other.text;
}

void MQEntry::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->l_id);
    doParsimPacking(b,this->l_clock);
    doParsimPacking(b,this->text);
}

void MQEntry::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->l_id);
    doParsimUnpacking(b,this->l_clock);
    doParsimUnpacking(b,this->text);
}

int MQEntry::getL_id() const
{
    return this->l_id;
}

void MQEntry::setL_id(int l_id)
{
    this->l_id = l_id;
}

int MQEntry::getL_clock() const
{
    return this->l_clock;
}

void MQEntry::setL_clock(int l_clock)
{
    this->l_clock = l_clock;
}

const char * MQEntry::getText() const
{
    return this->text.c_str();
}

void MQEntry::setText(const char * text)
{
    this->text = text;
}

class MQEntryDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_l_id,
        FIELD_l_clock,
        FIELD_text,
    };
  public:
    MQEntryDescriptor();
    virtual ~MQEntryDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(MQEntryDescriptor)

MQEntryDescriptor::MQEntryDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MQEntry)), "")
{
    propertyNames = nullptr;
}

MQEntryDescriptor::~MQEntryDescriptor()
{
    delete[] propertyNames;
}

bool MQEntryDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MQEntry *>(obj)!=nullptr;
}

const char **MQEntryDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *MQEntryDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int MQEntryDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int MQEntryDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_l_id
        FD_ISEDITABLE,    // FIELD_l_clock
        FD_ISEDITABLE,    // FIELD_text
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *MQEntryDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "l_id",
        "l_clock",
        "text",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int MQEntryDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "l_id") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "l_clock") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "text") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *MQEntryDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_l_id
        "int",    // FIELD_l_clock
        "string",    // FIELD_text
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **MQEntryDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MQEntryDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MQEntryDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void MQEntryDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'MQEntry'", field);
    }
}

const char *MQEntryDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MQEntryDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: return long2string(pp->getL_id());
        case FIELD_l_clock: return long2string(pp->getL_clock());
        case FIELD_text: return oppstring2string(pp->getText());
        default: return "";
    }
}

void MQEntryDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: pp->setL_id(string2long(value)); break;
        case FIELD_l_clock: pp->setL_clock(string2long(value)); break;
        case FIELD_text: pp->setText((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MQEntry'", field);
    }
}

omnetpp::cValue MQEntryDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: return pp->getL_id();
        case FIELD_l_clock: return pp->getL_clock();
        case FIELD_text: return pp->getText();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'MQEntry' as cValue -- field index out of range?", field);
    }
}

void MQEntryDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: pp->setL_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_l_clock: pp->setL_clock(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_text: pp->setText(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MQEntry'", field);
    }
}

const char *MQEntryDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr MQEntryDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void MQEntryDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    MQEntry *pp = omnetpp::fromAnyPtr<MQEntry>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MQEntry'", field);
    }
}

Register_Class(GenericMessage)

GenericMessage::GenericMessage(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

GenericMessage::GenericMessage(const GenericMessage& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

GenericMessage::~GenericMessage()
{
}

GenericMessage& GenericMessage::operator=(const GenericMessage& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void GenericMessage::copy(const GenericMessage& other)
{
    this->mex_type = other.mex_type;
    this->sender_id = other.sender_id;
}

void GenericMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->mex_type);
    doParsimPacking(b,this->sender_id);
}

void GenericMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->mex_type);
    doParsimUnpacking(b,this->sender_id);
}

int GenericMessage::getMex_type() const
{
    return this->mex_type;
}

void GenericMessage::setMex_type(int mex_type)
{
    this->mex_type = mex_type;
}

int GenericMessage::getSender_id() const
{
    return this->sender_id;
}

void GenericMessage::setSender_id(int sender_id)
{
    this->sender_id = sender_id;
}

class GenericMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_mex_type,
        FIELD_sender_id,
    };
  public:
    GenericMessageDescriptor();
    virtual ~GenericMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(GenericMessageDescriptor)

GenericMessageDescriptor::GenericMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(GenericMessage)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

GenericMessageDescriptor::~GenericMessageDescriptor()
{
    delete[] propertyNames;
}

bool GenericMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<GenericMessage *>(obj)!=nullptr;
}

const char **GenericMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *GenericMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int GenericMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int GenericMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_mex_type
        FD_ISEDITABLE,    // FIELD_sender_id
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *GenericMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "mex_type",
        "sender_id",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int GenericMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "mex_type") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "sender_id") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *GenericMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_mex_type
        "int",    // FIELD_sender_id
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **GenericMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *GenericMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int GenericMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void GenericMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'GenericMessage'", field);
    }
}

const char *GenericMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string GenericMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        case FIELD_mex_type: return long2string(pp->getMex_type());
        case FIELD_sender_id: return long2string(pp->getSender_id());
        default: return "";
    }
}

void GenericMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        case FIELD_mex_type: pp->setMex_type(string2long(value)); break;
        case FIELD_sender_id: pp->setSender_id(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GenericMessage'", field);
    }
}

omnetpp::cValue GenericMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        case FIELD_mex_type: return pp->getMex_type();
        case FIELD_sender_id: return pp->getSender_id();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'GenericMessage' as cValue -- field index out of range?", field);
    }
}

void GenericMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        case FIELD_mex_type: pp->setMex_type(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sender_id: pp->setSender_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GenericMessage'", field);
    }
}

const char *GenericMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr GenericMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void GenericMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    GenericMessage *pp = omnetpp::fromAnyPtr<GenericMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GenericMessage'", field);
    }
}

Register_Class(Message)

Message::Message(const char *name, short kind) : ::GenericMessage(name, kind)
{
}

Message::Message(const Message& other) : ::GenericMessage(other)
{
    copy(other);
}

Message::~Message()
{
}

Message& Message::operator=(const Message& other)
{
    if (this == &other) return *this;
    ::GenericMessage::operator=(other);
    copy(other);
    return *this;
}

void Message::copy(const Message& other)
{
    this->l_id = other.l_id;
    this->l_clock = other.l_clock;
    this->text = other.text;
}

void Message::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::GenericMessage::parsimPack(b);
    doParsimPacking(b,this->l_id);
    doParsimPacking(b,this->l_clock);
    doParsimPacking(b,this->text);
}

void Message::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::GenericMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->l_id);
    doParsimUnpacking(b,this->l_clock);
    doParsimUnpacking(b,this->text);
}

int Message::getL_id() const
{
    return this->l_id;
}

void Message::setL_id(int l_id)
{
    this->l_id = l_id;
}

int Message::getL_clock() const
{
    return this->l_clock;
}

void Message::setL_clock(int l_clock)
{
    this->l_clock = l_clock;
}

const char * Message::getText() const
{
    return this->text.c_str();
}

void Message::setText(const char * text)
{
    this->text = text;
}

class MessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_l_id,
        FIELD_l_clock,
        FIELD_text,
    };
  public:
    MessageDescriptor();
    virtual ~MessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(MessageDescriptor)

MessageDescriptor::MessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Message)), "GenericMessage")
{
    propertyNames = nullptr;
}

MessageDescriptor::~MessageDescriptor()
{
    delete[] propertyNames;
}

bool MessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Message *>(obj)!=nullptr;
}

const char **MessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *MessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int MessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int MessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_l_id
        FD_ISEDITABLE,    // FIELD_l_clock
        FD_ISEDITABLE,    // FIELD_text
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *MessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "l_id",
        "l_clock",
        "text",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int MessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "l_id") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "l_clock") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "text") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *MessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_l_id
        "int",    // FIELD_l_clock
        "string",    // FIELD_text
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **MessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void MessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Message'", field);
    }
}

const char *MessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: return long2string(pp->getL_id());
        case FIELD_l_clock: return long2string(pp->getL_clock());
        case FIELD_text: return oppstring2string(pp->getText());
        default: return "";
    }
}

void MessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: pp->setL_id(string2long(value)); break;
        case FIELD_l_clock: pp->setL_clock(string2long(value)); break;
        case FIELD_text: pp->setText((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message'", field);
    }
}

omnetpp::cValue MessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: return pp->getL_id();
        case FIELD_l_clock: return pp->getL_clock();
        case FIELD_text: return pp->getText();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Message' as cValue -- field index out of range?", field);
    }
}

void MessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        case FIELD_l_id: pp->setL_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_l_clock: pp->setL_clock(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_text: pp->setText(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message'", field);
    }
}

const char *MessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr MessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void MessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Message *pp = omnetpp::fromAnyPtr<Message>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Message'", field);
    }
}

Register_Class(FaultMessage)

FaultMessage::FaultMessage(const char *name, short kind) : ::GenericMessage(name, kind)
{
}

FaultMessage::FaultMessage(const FaultMessage& other) : ::GenericMessage(other)
{
    copy(other);
}

FaultMessage::~FaultMessage()
{
    delete [] this->queue;
}

FaultMessage& FaultMessage::operator=(const FaultMessage& other)
{
    if (this == &other) return *this;
    ::GenericMessage::operator=(other);
    copy(other);
    return *this;
}

void FaultMessage::copy(const FaultMessage& other)
{
    this->fault_id = other.fault_id;
    this->fault_node = other.fault_node;
    delete [] this->queue;
    this->queue = (other.queue_arraysize==0) ? nullptr : new MQEntry[other.queue_arraysize];
    queue_arraysize = other.queue_arraysize;
    for (size_t i = 0; i < queue_arraysize; i++) {
        this->queue[i] = other.queue[i];
    }
}

void FaultMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::GenericMessage::parsimPack(b);
    doParsimPacking(b,this->fault_id);
    doParsimPacking(b,this->fault_node);
    b->pack(queue_arraysize);
    doParsimArrayPacking(b,this->queue,queue_arraysize);
}

void FaultMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::GenericMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->fault_id);
    doParsimUnpacking(b,this->fault_node);
    delete [] this->queue;
    b->unpack(queue_arraysize);
    if (queue_arraysize == 0) {
        this->queue = nullptr;
    } else {
        this->queue = new MQEntry[queue_arraysize];
        doParsimArrayUnpacking(b,this->queue,queue_arraysize);
    }
}

int FaultMessage::getFault_id() const
{
    return this->fault_id;
}

void FaultMessage::setFault_id(int fault_id)
{
    this->fault_id = fault_id;
}

int FaultMessage::getFault_node() const
{
    return this->fault_node;
}

void FaultMessage::setFault_node(int fault_node)
{
    this->fault_node = fault_node;
}

size_t FaultMessage::getQueueArraySize() const
{
    return queue_arraysize;
}

const MQEntry& FaultMessage::getQueue(size_t k) const
{
    if (k >= queue_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)queue_arraysize, (unsigned long)k);
    return this->queue[k];
}

void FaultMessage::setQueueArraySize(size_t newSize)
{
    MQEntry *queue2 = (newSize==0) ? nullptr : new MQEntry[newSize];
    size_t minSize = queue_arraysize < newSize ? queue_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        queue2[i] = this->queue[i];
    delete [] this->queue;
    this->queue = queue2;
    queue_arraysize = newSize;
}

void FaultMessage::setQueue(size_t k, const MQEntry& queue)
{
    if (k >= queue_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)queue_arraysize, (unsigned long)k);
    this->queue[k] = queue;
}

void FaultMessage::insertQueue(size_t k, const MQEntry& queue)
{
    if (k > queue_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)queue_arraysize, (unsigned long)k);
    size_t newSize = queue_arraysize + 1;
    MQEntry *queue2 = new MQEntry[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        queue2[i] = this->queue[i];
    queue2[k] = queue;
    for (i = k + 1; i < newSize; i++)
        queue2[i] = this->queue[i-1];
    delete [] this->queue;
    this->queue = queue2;
    queue_arraysize = newSize;
}

void FaultMessage::appendQueue(const MQEntry& queue)
{
    insertQueue(queue_arraysize, queue);
}

void FaultMessage::eraseQueue(size_t k)
{
    if (k >= queue_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)queue_arraysize, (unsigned long)k);
    size_t newSize = queue_arraysize - 1;
    MQEntry *queue2 = (newSize == 0) ? nullptr : new MQEntry[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        queue2[i] = this->queue[i];
    for (i = k; i < newSize; i++)
        queue2[i] = this->queue[i+1];
    delete [] this->queue;
    this->queue = queue2;
    queue_arraysize = newSize;
}

class FaultMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_fault_id,
        FIELD_fault_node,
        FIELD_queue,
    };
  public:
    FaultMessageDescriptor();
    virtual ~FaultMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(FaultMessageDescriptor)

FaultMessageDescriptor::FaultMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(FaultMessage)), "GenericMessage")
{
    propertyNames = nullptr;
}

FaultMessageDescriptor::~FaultMessageDescriptor()
{
    delete[] propertyNames;
}

bool FaultMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FaultMessage *>(obj)!=nullptr;
}

const char **FaultMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *FaultMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int FaultMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int FaultMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_fault_id
        FD_ISEDITABLE,    // FIELD_fault_node
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISRESIZABLE,    // FIELD_queue
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *FaultMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "fault_id",
        "fault_node",
        "queue",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int FaultMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "fault_id") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "fault_node") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "queue") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *FaultMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_fault_id
        "int",    // FIELD_fault_node
        "MQEntry",    // FIELD_queue
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **FaultMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FaultMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FaultMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_queue: return pp->getQueueArraySize();
        default: return 0;
    }
}

void FaultMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_queue: pp->setQueueArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'FaultMessage'", field);
    }
}

const char *FaultMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FaultMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_fault_id: return long2string(pp->getFault_id());
        case FIELD_fault_node: return long2string(pp->getFault_node());
        case FIELD_queue: return "";
        default: return "";
    }
}

void FaultMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_fault_id: pp->setFault_id(string2long(value)); break;
        case FIELD_fault_node: pp->setFault_node(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FaultMessage'", field);
    }
}

omnetpp::cValue FaultMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_fault_id: return pp->getFault_id();
        case FIELD_fault_node: return pp->getFault_node();
        case FIELD_queue: return omnetpp::toAnyPtr(&pp->getQueue(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'FaultMessage' as cValue -- field index out of range?", field);
    }
}

void FaultMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_fault_id: pp->setFault_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_fault_node: pp->setFault_node(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FaultMessage'", field);
    }
}

const char *FaultMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_queue: return omnetpp::opp_typename(typeid(MQEntry));
        default: return nullptr;
    };
}

omnetpp::any_ptr FaultMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_queue: return omnetpp::toAnyPtr(&pp->getQueue(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void FaultMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    FaultMessage *pp = omnetpp::fromAnyPtr<FaultMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FaultMessage'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

