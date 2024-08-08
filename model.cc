#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <vector>

#include "msgtype_m.h"
#include "queueentry.cc"

using namespace omnetpp;

bool is_after_id(int lc1, int id1, int lc2, int id2){
    return lc1 > lc2 || (lc1==lc2 && id1 > id2);
}
bool is_after_msg(const Message &m1, const Message &m2){
    return is_after_id(m1.getL_clock(),m2.getL_clock(),m1.getL_id(),m2.getL_id());
}
bool is_after_qe(const QueueEntry &qe1, const QueueEntry &qe2){
    return is_after_msg(qe1.msg,qe2.msg);
}


class Node : public cSimpleModule
{
  private:
    int id;
    int l_clock;

    int last_committed_id;
    int last_commited_l_clock;

    std::vector<QueueEntry> queue;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
