//
// Generated file, do not edit! Created by opp_msgtool 6.0 from msgtype.msg.
//

#ifndef __MSGTYPE_M_H
#define __MSGTYPE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class MQEntry;
class GenericMessage;
class Message;
class FaultMessage;
class HBAckMessage;
/**
 * Class generated from <tt>msgtype.msg:11</tt> by opp_msgtool.
 * <pre>
 * class MQEntry
 * {
 *     int l_id;
 *     int l_clock;
 *     string text;
 * }
 * </pre>
 */
class MQEntry
{
  protected:
    int l_id = 0;
    int l_clock = 0;
    omnetpp::opp_string text;

  private:
    void copy(const MQEntry& other);

  protected:
    bool operator==(const MQEntry&) = delete;

  public:
    MQEntry();
    MQEntry(const MQEntry& other);
    virtual ~MQEntry();
    MQEntry& operator=(const MQEntry& other);
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    virtual int getL_id() const;
    virtual void setL_id(int l_id);

    virtual int getL_clock() const;
    virtual void setL_clock(int l_clock);

    virtual const char * getText() const;
    virtual void setText(const char * text);
};

/**
 * Class generated from <tt>msgtype.msg:17</tt> by opp_msgtool.
 * <pre>
 * message GenericMessage
 * {
 *     int mex_type;
 *     int sender_id;
 * }
 * </pre>
 */
class GenericMessage : public ::omnetpp::cMessage
{
  protected:
    int mex_type = 0;
    int sender_id = 0;

  private:
    void copy(const GenericMessage& other);

  protected:
    bool operator==(const GenericMessage&) = delete;

  public:
    GenericMessage(const char *name=nullptr, short kind=0);
    GenericMessage(const GenericMessage& other);
    virtual ~GenericMessage();
    GenericMessage& operator=(const GenericMessage& other);
    virtual GenericMessage *dup() const override {return new GenericMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getMex_type() const;
    virtual void setMex_type(int mex_type);

    virtual int getSender_id() const;
    virtual void setSender_id(int sender_id);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const GenericMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, GenericMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>msgtype.msg:22</tt> by opp_msgtool.
 * <pre>
 * message Message extends GenericMessage
 * {
 *     int l_id;
 *     int l_clock;
 *     string text;
 * }
 * </pre>
 */
class Message : public ::GenericMessage
{
  protected:
    int l_id = 0;
    int l_clock = 0;
    omnetpp::opp_string text;

  private:
    void copy(const Message& other);

  protected:
    bool operator==(const Message&) = delete;

  public:
    Message(const char *name=nullptr, short kind=0);
    Message(const Message& other);
    virtual ~Message();
    Message& operator=(const Message& other);
    virtual Message *dup() const override {return new Message(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getL_id() const;
    virtual void setL_id(int l_id);

    virtual int getL_clock() const;
    virtual void setL_clock(int l_clock);

    virtual const char * getText() const;
    virtual void setText(const char * text);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Message& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Message& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>msgtype.msg:28</tt> by opp_msgtool.
 * <pre>
 * message FaultMessage extends GenericMessage
 * {
 *     int fault_id;
 *     int fault_node;
 *     MQEntry queue[];
 * }
 * </pre>
 */
class FaultMessage : public ::GenericMessage
{
  protected:
    int fault_id = 0;
    int fault_node = 0;
    MQEntry *queue = nullptr;
    size_t queue_arraysize = 0;

  private:
    void copy(const FaultMessage& other);

  protected:
    bool operator==(const FaultMessage&) = delete;

  public:
    FaultMessage(const char *name=nullptr, short kind=0);
    FaultMessage(const FaultMessage& other);
    virtual ~FaultMessage();
    FaultMessage& operator=(const FaultMessage& other);
    virtual FaultMessage *dup() const override {return new FaultMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getFault_id() const;
    virtual void setFault_id(int fault_id);

    virtual int getFault_node() const;
    virtual void setFault_node(int fault_node);

    virtual void setQueueArraySize(size_t size);
    virtual size_t getQueueArraySize() const;
    virtual const MQEntry& getQueue(size_t k) const;
    virtual MQEntry& getQueueForUpdate(size_t k) { return const_cast<MQEntry&>(const_cast<FaultMessage*>(this)->getQueue(k));}
    virtual void setQueue(size_t k, const MQEntry& queue);
    virtual void insertQueue(size_t k, const MQEntry& queue);
    [[deprecated]] void insertQueue(const MQEntry& queue) {appendQueue(queue);}
    virtual void appendQueue(const MQEntry& queue);
    virtual void eraseQueue(size_t k);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const FaultMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, FaultMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>msgtype.msg:34</tt> by opp_msgtool.
 * <pre>
 * message HBAckMessage extends Message
 * {
 *     int last_l_id;
 *     int last_l_clock;
 * }
 * </pre>
 */
class HBAckMessage : public ::Message
{
  protected:
    int last_l_id = 0;
    int last_l_clock = 0;

  private:
    void copy(const HBAckMessage& other);

  protected:
    bool operator==(const HBAckMessage&) = delete;

  public:
    HBAckMessage(const char *name=nullptr, short kind=0);
    HBAckMessage(const HBAckMessage& other);
    virtual ~HBAckMessage();
    HBAckMessage& operator=(const HBAckMessage& other);
    virtual HBAckMessage *dup() const override {return new HBAckMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getLast_l_id() const;
    virtual void setLast_l_id(int last_l_id);

    virtual int getLast_l_clock() const;
    virtual void setLast_l_clock(int last_l_clock);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const HBAckMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, HBAckMessage& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

inline any_ptr toAnyPtr(const MQEntry *p) {if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);}
template<> inline MQEntry *fromAnyPtr(any_ptr ptr) { return ptr.get<MQEntry>(); }
template<> inline GenericMessage *fromAnyPtr(any_ptr ptr) { return check_and_cast<GenericMessage*>(ptr.get<cObject>()); }
template<> inline Message *fromAnyPtr(any_ptr ptr) { return check_and_cast<Message*>(ptr.get<cObject>()); }
template<> inline FaultMessage *fromAnyPtr(any_ptr ptr) { return check_and_cast<FaultMessage*>(ptr.get<cObject>()); }
template<> inline HBAckMessage *fromAnyPtr(any_ptr ptr) { return check_and_cast<HBAckMessage*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __MSGTYPE_M_H

