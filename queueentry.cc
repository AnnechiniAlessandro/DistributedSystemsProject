#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <vector>

#include "msgtype_m.h"

class QueueEntry{
public:
    Message& msg;
    std::vector<int> acks;
    QueueEntry(Message &msg):msg(msg),acks(){ }
};
