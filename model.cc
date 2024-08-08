#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <vector>
#include <algorithm>

#include "msgtype_m.h"
#include "queueentry.cc"

#define HEARTBEAT_PERIOD 10

#define MEXTYPE_STD 0
#define MEXTYPE_ACK 1
#define MEXTYPE_SELFHB 2
#define MEXTYPE_HB 3
//...

using namespace omnetpp;

bool is_after_id(int lc1, int id1, int lc2, int id2){
    return lc1 > lc2 || (lc1==lc2 && id1 > id2);
}
bool is_after_msg(const Message *m1, const Message *m2){
    return is_after_id(m1->getL_clock(),m2->getL_clock(),m1->getL_id(),m2->getL_id());
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

    int hb_channel;

    std::vector<QueueEntry> queue;
    std::vector<int> view;
    int num_nodes;

    std::vector<Message> committed_msgs;

  protected:
    virtual void initialize() override;
    virtual void handleStdMessage(Message *m);
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  public:
    Node():queue(),view(),committed_msgs(){ }
};

Define_Module(Node);

void Node::initialize(){
    id = par("id");
    l_clock = 0;

    last_committed_id = -1;
    last_commited_l_clock = -1;

    num_nodes = 0;
    for(int i=0; i< gateSize("out"); i++){
        if(i!=id){
            view.push_back(i);
        }
        num_nodes++;
    }

    hb_channel = (id+1)%num_nodes;

    Message *hb_msg = new Message();
    hb_msg->setMex_type(2);

    //Schedule the event of sending the first HB
    scheduleAt(HEARTBEAT_PERIOD,hb_msg);
}

void Node::handleStdMessage(Message *m){
    //Check if an ACK has already arrived
    QueueEntry *qe = nullptr;
    for(int i=0; i<(int)queue.size(); i++){
        if(queue[i].msg->getL_id() == m->getL_id() && queue[i].msg->getL_clock() == m->getL_clock()){
            qe = &queue[i];
        }
    }

    if(qe == nullptr){
        //INSERT MESSAGE IN QUEUE
        QueueEntry newqe = QueueEntry();
        newqe.setMsg(m);
        //Add own ack
        newqe.acks.insert(id);
        queue.push_back(newqe);
        std::push_heap(queue.begin(),queue.end(),is_after_qe);
    }else{
        //UPDATE MESSAGE IN QUEUE
        qe->setMsg(m);
    }

    return;
}

void Node::handleMessage(cMessage *t_msg){
    Message *m = (Message*) t_msg;

    /*
    if(m->isSelfMessage()){
        //TODO
        return;
    }
    */

    if(m->getMex_type() == MEXTYPE_STD){
        handleStdMessage(m);
        return;
    }
}

void Node::finish(){
    //TODO
}
