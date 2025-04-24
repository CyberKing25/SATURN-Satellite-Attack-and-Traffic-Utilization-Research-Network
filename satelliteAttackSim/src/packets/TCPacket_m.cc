//
// Generated file, do not edit! Created by opp_msgtool 6.1 from packets/TCPacket.msg.
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
#include "TCPacket_m.h"

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

Register_Class(TC_Packet)

TC_Packet::TC_Packet(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

TC_Packet::TC_Packet(const TC_Packet& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

TC_Packet::~TC_Packet()
{
    delete [] this->util_buffer;
}

TC_Packet& TC_Packet::operator=(const TC_Packet& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void TC_Packet::copy(const TC_Packet& other)
{
    this->scid = other.scid;
    this->max_fdu_len = other.max_fdu_len;
    this->max_frame_len = other.max_frame_len;
    this->rx_fifo_len = other.rx_fifo_len;
    this->vcid = other.vcid;
    this->crc_flag = other.crc_flag;
    this->seg_hdr_flag = other.seg_hdr_flag;
    this->bypass = other.bypass;
    this->ctrl_cmd = other.ctrl_cmd;
    delete [] this->util_buffer;
    this->util_buffer = (other.util_buffer_arraysize==0) ? nullptr : new uint8_t[other.util_buffer_arraysize];
    util_buffer_arraysize = other.util_buffer_arraysize;
    for (size_t i = 0; i < util_buffer_arraysize; i++) {
        this->util_buffer[i] = other.util_buffer[i];
    }
}

void TC_Packet::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->scid);
    doParsimPacking(b,this->max_fdu_len);
    doParsimPacking(b,this->max_frame_len);
    doParsimPacking(b,this->rx_fifo_len);
    doParsimPacking(b,this->vcid);
    doParsimPacking(b,this->crc_flag);
    doParsimPacking(b,this->seg_hdr_flag);
    doParsimPacking(b,this->bypass);
    doParsimPacking(b,this->ctrl_cmd);
    b->pack(util_buffer_arraysize);
    doParsimArrayPacking(b,this->util_buffer,util_buffer_arraysize);
}

void TC_Packet::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->scid);
    doParsimUnpacking(b,this->max_fdu_len);
    doParsimUnpacking(b,this->max_frame_len);
    doParsimUnpacking(b,this->rx_fifo_len);
    doParsimUnpacking(b,this->vcid);
    doParsimUnpacking(b,this->crc_flag);
    doParsimUnpacking(b,this->seg_hdr_flag);
    doParsimUnpacking(b,this->bypass);
    doParsimUnpacking(b,this->ctrl_cmd);
    delete [] this->util_buffer;
    b->unpack(util_buffer_arraysize);
    if (util_buffer_arraysize == 0) {
        this->util_buffer = nullptr;
    } else {
        this->util_buffer = new uint8_t[util_buffer_arraysize];
        doParsimArrayUnpacking(b,this->util_buffer,util_buffer_arraysize);
    }
}

int TC_Packet::getScid() const
{
    return this->scid;
}

void TC_Packet::setScid(int scid)
{
    this->scid = scid;
}

int TC_Packet::getMax_fdu_len() const
{
    return this->max_fdu_len;
}

void TC_Packet::setMax_fdu_len(int max_fdu_len)
{
    this->max_fdu_len = max_fdu_len;
}

int TC_Packet::getMax_frame_len() const
{
    return this->max_frame_len;
}

void TC_Packet::setMax_frame_len(int max_frame_len)
{
    this->max_frame_len = max_frame_len;
}

int TC_Packet::getRx_fifo_len() const
{
    return this->rx_fifo_len;
}

void TC_Packet::setRx_fifo_len(int rx_fifo_len)
{
    this->rx_fifo_len = rx_fifo_len;
}

int TC_Packet::getVcid() const
{
    return this->vcid;
}

void TC_Packet::setVcid(int vcid)
{
    this->vcid = vcid;
}

bool TC_Packet::getCrc_flag() const
{
    return this->crc_flag;
}

void TC_Packet::setCrc_flag(bool crc_flag)
{
    this->crc_flag = crc_flag;
}

bool TC_Packet::getSeg_hdr_flag() const
{
    return this->seg_hdr_flag;
}

void TC_Packet::setSeg_hdr_flag(bool seg_hdr_flag)
{
    this->seg_hdr_flag = seg_hdr_flag;
}

bool TC_Packet::getBypass() const
{
    return this->bypass;
}

void TC_Packet::setBypass(bool bypass)
{
    this->bypass = bypass;
}

bool TC_Packet::getCtrl_cmd() const
{
    return this->ctrl_cmd;
}

void TC_Packet::setCtrl_cmd(bool ctrl_cmd)
{
    this->ctrl_cmd = ctrl_cmd;
}

size_t TC_Packet::getUtil_bufferArraySize() const
{
    return util_buffer_arraysize;
}

uint8_t TC_Packet::getUtil_buffer(size_t k) const
{
    if (k >= util_buffer_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)util_buffer_arraysize, (unsigned long)k);
    return this->util_buffer[k];
}

void TC_Packet::setUtil_bufferArraySize(size_t newSize)
{
    uint8_t *util_buffer2 = (newSize==0) ? nullptr : new uint8_t[newSize];
    size_t minSize = util_buffer_arraysize < newSize ? util_buffer_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        util_buffer2[i] = this->util_buffer[i];
    for (size_t i = minSize; i < newSize; i++)
        util_buffer2[i] = 0;
    delete [] this->util_buffer;
    this->util_buffer = util_buffer2;
    util_buffer_arraysize = newSize;
}

void TC_Packet::setUtil_buffer(size_t k, uint8_t util_buffer)
{
    if (k >= util_buffer_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)util_buffer_arraysize, (unsigned long)k);
    this->util_buffer[k] = util_buffer;
}

void TC_Packet::insertUtil_buffer(size_t k, uint8_t util_buffer)
{
    if (k > util_buffer_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)util_buffer_arraysize, (unsigned long)k);
    size_t newSize = util_buffer_arraysize + 1;
    uint8_t *util_buffer2 = new uint8_t[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        util_buffer2[i] = this->util_buffer[i];
    util_buffer2[k] = util_buffer;
    for (i = k + 1; i < newSize; i++)
        util_buffer2[i] = this->util_buffer[i-1];
    delete [] this->util_buffer;
    this->util_buffer = util_buffer2;
    util_buffer_arraysize = newSize;
}

void TC_Packet::appendUtil_buffer(uint8_t util_buffer)
{
    insertUtil_buffer(util_buffer_arraysize, util_buffer);
}

void TC_Packet::eraseUtil_buffer(size_t k)
{
    if (k >= util_buffer_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)util_buffer_arraysize, (unsigned long)k);
    size_t newSize = util_buffer_arraysize - 1;
    uint8_t *util_buffer2 = (newSize == 0) ? nullptr : new uint8_t[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        util_buffer2[i] = this->util_buffer[i];
    for (i = k; i < newSize; i++)
        util_buffer2[i] = this->util_buffer[i+1];
    delete [] this->util_buffer;
    this->util_buffer = util_buffer2;
    util_buffer_arraysize = newSize;
}

class TC_PacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_scid,
        FIELD_max_fdu_len,
        FIELD_max_frame_len,
        FIELD_rx_fifo_len,
        FIELD_vcid,
        FIELD_crc_flag,
        FIELD_seg_hdr_flag,
        FIELD_bypass,
        FIELD_ctrl_cmd,
        FIELD_util_buffer,
    };
  public:
    TC_PacketDescriptor();
    virtual ~TC_PacketDescriptor();

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

Register_ClassDescriptor(TC_PacketDescriptor)

TC_PacketDescriptor::TC_PacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TC_Packet)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

TC_PacketDescriptor::~TC_PacketDescriptor()
{
    delete[] propertyNames;
}

bool TC_PacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TC_Packet *>(obj)!=nullptr;
}

const char **TC_PacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TC_PacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TC_PacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 10+base->getFieldCount() : 10;
}

unsigned int TC_PacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_scid
        FD_ISEDITABLE,    // FIELD_max_fdu_len
        FD_ISEDITABLE,    // FIELD_max_frame_len
        FD_ISEDITABLE,    // FIELD_rx_fifo_len
        FD_ISEDITABLE,    // FIELD_vcid
        FD_ISEDITABLE,    // FIELD_crc_flag
        FD_ISEDITABLE,    // FIELD_seg_hdr_flag
        FD_ISEDITABLE,    // FIELD_bypass
        FD_ISEDITABLE,    // FIELD_ctrl_cmd
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_util_buffer
    };
    return (field >= 0 && field < 10) ? fieldTypeFlags[field] : 0;
}

const char *TC_PacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "scid",
        "max_fdu_len",
        "max_frame_len",
        "rx_fifo_len",
        "vcid",
        "crc_flag",
        "seg_hdr_flag",
        "bypass",
        "ctrl_cmd",
        "util_buffer",
    };
    return (field >= 0 && field < 10) ? fieldNames[field] : nullptr;
}

int TC_PacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "scid") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "max_fdu_len") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "max_frame_len") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "rx_fifo_len") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "vcid") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "crc_flag") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "seg_hdr_flag") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "bypass") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "ctrl_cmd") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "util_buffer") == 0) return baseIndex + 9;
    return base ? base->findField(fieldName) : -1;
}

const char *TC_PacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_scid
        "int",    // FIELD_max_fdu_len
        "int",    // FIELD_max_frame_len
        "int",    // FIELD_rx_fifo_len
        "int",    // FIELD_vcid
        "bool",    // FIELD_crc_flag
        "bool",    // FIELD_seg_hdr_flag
        "bool",    // FIELD_bypass
        "bool",    // FIELD_ctrl_cmd
        "uint8_t",    // FIELD_util_buffer
    };
    return (field >= 0 && field < 10) ? fieldTypeStrings[field] : nullptr;
}

const char **TC_PacketDescriptor::getFieldPropertyNames(int field) const
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

const char *TC_PacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int TC_PacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_util_buffer: return pp->getUtil_bufferArraySize();
        default: return 0;
    }
}

void TC_PacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_util_buffer: pp->setUtil_bufferArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TC_Packet'", field);
    }
}

const char *TC_PacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TC_PacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_scid: return long2string(pp->getScid());
        case FIELD_max_fdu_len: return long2string(pp->getMax_fdu_len());
        case FIELD_max_frame_len: return long2string(pp->getMax_frame_len());
        case FIELD_rx_fifo_len: return long2string(pp->getRx_fifo_len());
        case FIELD_vcid: return long2string(pp->getVcid());
        case FIELD_crc_flag: return bool2string(pp->getCrc_flag());
        case FIELD_seg_hdr_flag: return bool2string(pp->getSeg_hdr_flag());
        case FIELD_bypass: return bool2string(pp->getBypass());
        case FIELD_ctrl_cmd: return bool2string(pp->getCtrl_cmd());
        case FIELD_util_buffer: return ulong2string(pp->getUtil_buffer(i));
        default: return "";
    }
}

void TC_PacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_scid: pp->setScid(string2long(value)); break;
        case FIELD_max_fdu_len: pp->setMax_fdu_len(string2long(value)); break;
        case FIELD_max_frame_len: pp->setMax_frame_len(string2long(value)); break;
        case FIELD_rx_fifo_len: pp->setRx_fifo_len(string2long(value)); break;
        case FIELD_vcid: pp->setVcid(string2long(value)); break;
        case FIELD_crc_flag: pp->setCrc_flag(string2bool(value)); break;
        case FIELD_seg_hdr_flag: pp->setSeg_hdr_flag(string2bool(value)); break;
        case FIELD_bypass: pp->setBypass(string2bool(value)); break;
        case FIELD_ctrl_cmd: pp->setCtrl_cmd(string2bool(value)); break;
        case FIELD_util_buffer: pp->setUtil_buffer(i,string2ulong(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TC_Packet'", field);
    }
}

omnetpp::cValue TC_PacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_scid: return pp->getScid();
        case FIELD_max_fdu_len: return pp->getMax_fdu_len();
        case FIELD_max_frame_len: return pp->getMax_frame_len();
        case FIELD_rx_fifo_len: return pp->getRx_fifo_len();
        case FIELD_vcid: return pp->getVcid();
        case FIELD_crc_flag: return pp->getCrc_flag();
        case FIELD_seg_hdr_flag: return pp->getSeg_hdr_flag();
        case FIELD_bypass: return pp->getBypass();
        case FIELD_ctrl_cmd: return pp->getCtrl_cmd();
        case FIELD_util_buffer: return (omnetpp::intval_t)(pp->getUtil_buffer(i));
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TC_Packet' as cValue -- field index out of range?", field);
    }
}

void TC_PacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        case FIELD_scid: pp->setScid(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_max_fdu_len: pp->setMax_fdu_len(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_max_frame_len: pp->setMax_frame_len(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rx_fifo_len: pp->setRx_fifo_len(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_vcid: pp->setVcid(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_crc_flag: pp->setCrc_flag(value.boolValue()); break;
        case FIELD_seg_hdr_flag: pp->setSeg_hdr_flag(value.boolValue()); break;
        case FIELD_bypass: pp->setBypass(value.boolValue()); break;
        case FIELD_ctrl_cmd: pp->setCtrl_cmd(value.boolValue()); break;
        case FIELD_util_buffer: pp->setUtil_buffer(i,omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TC_Packet'", field);
    }
}

const char *TC_PacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr TC_PacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TC_PacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TC_Packet *pp = omnetpp::fromAnyPtr<TC_Packet>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TC_Packet'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

