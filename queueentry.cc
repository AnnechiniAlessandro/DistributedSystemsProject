#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <set>

#include "msgtype_m.h"

class QueueEntry{

public:
    int l_id;
    int l_clock;
    Message *msg;
    std::set<int> acks;

    QueueEntry():acks(){ }

    void setMsg(Message *imsg){
       msg = imsg;
       //imsg == nullptr means we received an ack without receiving the original message
       if(imsg == nullptr) return;
       l_id = msg->getL_id();
       l_clock = msg->getL_clock();
       acks.insert(l_id);
    }
};
