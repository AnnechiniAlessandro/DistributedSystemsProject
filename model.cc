#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <vector>
#include <set>
#include <algorithm>

#include "msgtype_m.h"
#include "queueentry.cc"

#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define HEARTBEAT_PERIOD 10
#define MAX_MEX_WAIT_TIME 0.001

#define MEXOBJTYPE_STD 0
#define MEXOBJTYPE_FAULT 1

#define MEXTYPE_STD 0
#define MEXTYPE_ACK 1
#define MEXTYPE_SELFHB 2
#define MEXTYPE_HB 3
#define MEXTYPE_HB_ACK 4
#define MEXTYPE_FAULT 5
#define MEXTYPE_FSTAGE2 6
//...

#define NODESTATE_STD 0
#define NODESTATE_FAULT 1

using namespace omnetpp;

bool is_after_id(int lc1, int id1, int lc2, int id2){
    return lc1 > lc2 || (lc1==lc2 && id1 > id2);
}
bool is_after_msg(const Message *m1, const Message *m2){
    return is_after_id(m1->getL_clock(),m2->getL_clock(),m1->getL_id(),m2->getL_id());
}
bool is_after_qe(const QueueEntry &qe1, const QueueEntry &qe2){
    return is_after_id(qe1.l_clock,qe1.l_id,qe2.l_clock,qe2.l_id);
}
int id_to_channel(int oth_id, int my_id){
    return oth_id > my_id ? oth_id-1 : oth_id;
}
int channel_to_id(int channel, int my_id){
    return channel >= my_id ? channel+1 : channel;
}

class Node : public cSimpleModule
{
  private:
    int id;
    int l_clock;
    int last_fault_id;

    int last_committed_id;
    int last_committed_l_clock;

    int hb_next_id;
    int hb_channel;

    std::vector<QueueEntry> queue;
    bool still_alive_neighbour;
    int node_state;

    //Stages sets for view change
    std::set<int> stage1;
    std::set<int> stage2;

    //Nodes in the group view
    std::set<int> view;
    int num_nodes;

    std::vector<Message*> committed_msgs;

  protected:
    virtual void initialize() override;
    virtual void send_standard_message(const char *text);
    virtual void checkTopMessage();
    virtual void send_hb(Message *hb_msg);
    virtual void handleStdMessage(Message *m);
    virtual void handleAckMessage(Message *m);
    virtual void handleHBMessage(Message *m);
    virtual void handleHBAckMessage(Message *m);
    virtual void handleMessage(cMessage *msg) override;
    virtual void mergeQueues(std::vector<QueueEntry> otherQueue);
    virtual void set_fault_state();
    virtual void set_std_state();
    virtual void fault_detected();
    virtual void handle_fault_message(FaultMessage *fm);
    virtual void handle_fault_stage2_message(FaultMessage *fm);
    virtual std::vector<MQEntry> prepareFaultQueue(int failed_node);
    virtual void finish() override;

  public:
    Node():queue(),view(),committed_msgs(),stage1(),stage2(){ }
};

Define_Module(Node);

void Node::initialize(){
    id = par("id");
    l_clock = 0;

    last_committed_id = -1;
    last_committed_l_clock = -1;

    num_nodes = 0;
    for(int i=0; i< gateSize("out")+1; i++){
        if(i!=id){
            view.insert(i);
        }
        num_nodes++;
    }

    hb_next_id = (id+1)%num_nodes;
    hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;

    //Schedule the event of sending the first HB
    Message *hb_msg = new Message();
    hb_msg->setMex_type(MEXTYPE_SELFHB);
    scheduleAt(simTime()+ HEARTBEAT_PERIOD,hb_msg);

    if(id==0){
        l_clock++;
        send_standard_message("MESSAGE");
    }

    still_alive_neighbour = true;
    node_state = NODESTATE_STD;
    last_fault_id = 0;

}

void Node::send_standard_message(const char *text){

    for(const int& elem : view){
        if(elem == id) continue;

        Message *msg = new Message();
        msg->setL_clock(l_clock);
        msg->setL_id(id);
        msg->setMex_type(MEXTYPE_STD);
        msg->setSender_id(id);
        msg->setText(text);

        send(msg,"out",id_to_channel(elem,id));
    }

    Message *my_msg = new Message();
    my_msg->setL_clock(l_clock);
    my_msg->setL_id(id);
    my_msg->setMex_type(MEXTYPE_STD);
    my_msg->setSender_id(id);
    my_msg->setText(text);

    QueueEntry qe = QueueEntry();
    qe.setMsg(my_msg);
    qe.acks.insert(id);
    queue.push_back(qe);
    std::push_heap(queue.begin(),queue.end(),is_after_qe);
}

std::vector<MQEntry> Node::prepareFaultQueue(int failed_node){
    std::vector<MQEntry> result = std::vector<MQEntry>();
    for(int i = 0; i<(int)queue.size(); i++){
        MQEntry mqe = MQEntry();
        if(queue[i].msg != nullptr && queue[i].l_id == failed_node){
            mqe.setL_id(queue[i].l_id);
            mqe.setL_clock(queue[i].l_clock);
            mqe.setText(queue[i].msg->getText());
            result.push_back(mqe);
        }
    }

    return result;
}

void Node::set_std_state(){
    if(node_state!=NODESTATE_STD){
        bubble("ENTERED STANDARD STATE!");
        node_state = NODESTATE_STD;
        still_alive_neighbour = true;
        checkTopMessage();
        stage1.clear();
        stage2.clear();
    }
}

void Node::set_fault_state(){
    if(node_state != NODESTATE_FAULT){
        bubble("ENTERED FAULT STATE!");
        node_state = NODESTATE_FAULT;
    }
}

void Node::fault_detected(){

    last_fault_id++;
    int failed_node = hb_next_id;

    hb_next_id = (hb_next_id+1)%num_nodes;
    num_nodes--;
    hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;
    view.erase(failed_node);

    set_fault_state();
    stage1.insert(id);

    for(const int& elem : view){
        if(elem==id) continue;

        //Prepare the queue to be sent
        std::vector<MQEntry> list = prepareFaultQueue(failed_node);

        FaultMessage *fm = new FaultMessage();
        fm->setQueueArraySize(list.size());
        for(int i=0; i<(int)list.size(); i++){
            fm->setQueue(i, list[i]);
        }
        fm->setFault_id(last_fault_id);
        fm->setMex_type(MEXTYPE_FAULT);
        fm->setSender_id(id);
        fm->setFault_node(failed_node);

        send(fm,"out",id_to_channel(elem,id));
    }

    return;
}

void Node::handle_fault_stage2_message(FaultMessage *fm){
    //Ignore if the fault has already been handled
    if(fm->getFault_id() < last_fault_id ||  (fm->getFault_id() == last_fault_id && node_state!=NODESTATE_FAULT)){
        char aaaa[50];
        sprintf(aaaa,"DISCARDED STAGE 2 (%d): %d < %d",NODESTATE_FAULT,fm->getFault_id(),last_fault_id);
        bubble(aaaa);

        delete fm;
        return;
    }

    stage2.insert(fm->getSender_id());

    char aaaa[50];
    sprintf(aaaa,"STAGE 2 %d",(int) stage2.size());
    bubble(aaaa);

    //Check for stage 2 completion
    if(std::includes(stage2.begin(),stage2.end(),view.begin(),view.end())){
        set_std_state();
    }

    delete fm;

}

void Node::handle_fault_message(FaultMessage *fm){

    //Ignore if the fault has already been handled
    if(fm->getFault_id() < last_fault_id ||  (fm->getFault_id() == last_fault_id && node_state!=NODESTATE_FAULT)){
        delete fm;
        return;
    }

    std::vector<QueueEntry> otherQueue = std::vector<QueueEntry>();

    for(int i=0; i<(int)fm->getQueueArraySize(); i++){

        Message *m = new Message();
        m->setMex_type(MEXTYPE_STD);
        m->setText(fm->getQueue(i).getText());
        m->setSender_id(fm->getSender_id());
        m->setL_id(fm->getQueue(i).getL_id());
        m->setL_clock(fm->getQueue(i).getL_clock());

        QueueEntry qe = QueueEntry();
        qe.setMsg(m);
        for(const int& elem : view){
            qe.acks.insert(elem);
        }

        otherQueue.push_back(qe);
    }

    //INSERT UNSTABLE MESSAGES IN QUEUE
    //mergeQueues(otherQueue);

    stage1.insert(fm->getSender_id());

    if(fm->getFault_id() > last_fault_id){
        //First time receiving the fault message
        set_fault_state();

        last_fault_id = fm->getFault_id();
        int failed_node = fm->getFault_node();

        //Prepare the queue to be sent
        std::vector<MQEntry> list = prepareFaultQueue(failed_node);
        view.erase(failed_node);
        num_nodes = (int)view.size();

        stage1.insert(id);
        for(const int& elem : view){
            if(elem==id) continue;

            FaultMessage *fm = new FaultMessage();
            fm->setQueueArraySize(list.size());
            for(int i=0; i<(int)list.size(); i++){
                fm->setQueue(i, list[i]);
            }
            fm->setFault_id(last_fault_id);
            fm->setMex_type(MEXTYPE_FAULT);
            fm->setSender_id(id);
            fm->setFault_node(failed_node);

            send(fm,"out",id_to_channel(elem,id));
        }

    }

    //Check for stage 1 completion
    if(std::includes(stage1.begin(),stage1.end(),view.begin(),view.end())){

        stage2.insert(id);
        for(const int& elem : view){
            if(elem == id) continue;

            FaultMessage *msg = new FaultMessage();
            msg->setMex_type(MEXTYPE_FSTAGE2);
            msg->setSender_id(id);
            msg->setFault_id(last_fault_id);

            send(msg,"out",id_to_channel(elem,id));
        }

        //Check for stage 2 completion
        if(std::includes(stage2.begin(),stage2.end(),view.begin(),view.end())){
            set_std_state();
        }

    }

    delete fm;

}

void Node::send_hb(Message *hb_msg){

    scheduleAt(simTime()+ HEARTBEAT_PERIOD,hb_msg);

    //Postpone if we are handling a fault
    if(node_state == NODESTATE_FAULT){
        return;
    }

    // Any time we send and HB we check if the previous one has been acked
    if(!still_alive_neighbour){
        fault_detected();
        return;
    }

    // id of the following node
    int elem = hb_next_id;

    // create the HB message
    Message *msg = new Message();
    msg->setL_clock(l_clock);
    msg->setL_id(elem);
    msg->setMex_type(MEXTYPE_HB);
    msg->setSender_id(id);
    msg->setText("HEARTBEAT");

    send(msg,"out",hb_channel);

    // set still alive to false
    still_alive_neighbour = false;
}

void Node::checkTopMessage(){

    if(node_state == NODESTATE_FAULT) return;

    bool is_valid;
    if(queue.size()==0) return;
    do{
        is_valid = std::includes(queue[0].acks.begin(),queue[0].acks.end(),view.begin(),view.end());
        if(is_valid){
            char bub[100];
            sprintf(bub,"COMMITTED: %d - %d : %s",queue[0].l_id,queue[0].l_clock,queue[0].msg->getText());
            bubble(bub);
            last_committed_id = queue[0].l_id;
            last_committed_l_clock = queue[0].l_clock;
            committed_msgs.push_back(queue[0].msg);
            std::pop_heap(queue.begin(),queue.end(),is_after_qe);
            queue.pop_back();
        }
    }while(is_valid && queue.size()>0);
}

void Node::handleStdMessage(Message *m){

    l_clock = MAX(l_clock,m->getL_clock()) + 1;

    //Check if an ACK has already arrived
    QueueEntry *qe = nullptr;
    for(int i=0; i<(int)queue.size(); i++){
        if(queue[i].l_id == m->getL_id() && queue[i].l_clock == m->getL_clock()){
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

        //SEND ACKS TO OTHER NODES
        l_clock++;
        for (const int& elem : view) {
            if(elem != id){

                Message *ack = new Message();
                ack->setL_clock(m->getL_clock());
                ack->setL_id(m->getL_id());
                ack->setMex_type(MEXTYPE_ACK);
                ack->setSender_id(id);
                ack->setText("ACK");

                send(ack,"out",id_to_channel(elem,id));
            }
        }

    }else{
        //UPDATE MESSAGE IN QUEUE
        qe->setMsg(m);
    }

    checkTopMessage();

    return;
}

void Node::handleAckMessage(Message *m){

    l_clock = MAX(l_clock,m->getL_clock()) + 1;

    //Check if an ACK has already arrived
    QueueEntry *qe = nullptr;
    for(int i=0; i<(int)queue.size(); i++){
        if(queue[i].l_id == m->getL_id() && queue[i].l_clock == m->getL_clock()){
            qe = &queue[i];
        }
    }

    if(qe == nullptr){
        //INSERT (empty) MESSAGE IN QUEUE
        QueueEntry newqe = QueueEntry();
        newqe.setMsg(nullptr);
        newqe.l_clock = m->getL_clock();
        newqe.l_clock = m->getL_id();

        //Add own ack AND sender ack
        newqe.acks.insert(id);
        newqe.acks.insert(m->getSender_id());

        queue.push_back(newqe);
        std::push_heap(queue.begin(),queue.end(),is_after_qe);

        //SEND ACKS TO OTHER NODES
        l_clock++;
        for (const int& elem : view) {
            if(elem != id){

                Message *ack = new Message();
                ack->setL_clock(m->getL_clock());
                ack->setL_id(m->getL_id());
                ack->setMex_type(MEXTYPE_ACK);
                ack->setSender_id(id);
                ack->setText("ACK");

                send(ack,"out",id_to_channel(elem,id));
            }
        }
    }else{
        //UPDATE ACK LIST
        qe->acks.insert(m->getSender_id());
    }

    checkTopMessage();

    return;
}

void Node::handleHBAckMessage(Message *m){
    still_alive_neighbour = true;
    delete m;
    return;
}

void Node::handleHBMessage(Message *m){
    l_clock = MAX(l_clock,m->getL_clock()) + 1;

    //Postpone if we are handling a fault
    if(node_state == NODESTATE_FAULT){
        return;
    }

    Message *ack = new Message();
    ack->setL_clock(m->getL_clock());
    ack->setL_id(m->getL_id());
    ack->setMex_type(MEXTYPE_HB_ACK);
    ack->setSender_id(id);
    ack->setText("ACK");

    //respond to the HB
    send(ack,"out",id_to_channel(m->getSender_id(),id));

    delete m;
    return;

}

void Node::handleMessage(cMessage *t_msg){

    GenericMessage *gm = (GenericMessage*) t_msg;

    if(gm->getMex_type() == MEXTYPE_SELFHB){
        Message *m = (Message*) gm;
        send_hb(m);
        return;
    }

    if(gm->getMex_type() == MEXTYPE_STD){
        Message *m = (Message*) gm;
        handleStdMessage(m);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_ACK){
        Message *m = (Message*) gm;
        handleAckMessage(m);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_HB){
        Message *m = (Message*) gm;
        handleHBMessage(m);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_HB_ACK){
        Message *m = (Message*) gm;
        handleHBAckMessage(m);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_FAULT){
        FaultMessage *fm = (FaultMessage*) gm;
        handle_fault_message(fm);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_FSTAGE2){
        FaultMessage *fm = (FaultMessage*) gm;
        handle_fault_stage2_message(fm);
        return;
    }

}

void Node::mergeQueues(std::vector<QueueEntry> otherQueue){
    int i = 0;
    int j = 0;
    int lenQ = queue.size();
    int lenOQ = otherQueue.size();
    bool found = false;

    for(i = 0; i < lenOQ; i++){
        if(is_after_id(otherQueue[j].l_clock, otherQueue[j].l_id, last_committed_l_clock, last_committed_id)){ // skip already committed messages
            found = false;
            for(j=0; j<lenQ && !found; j++){ // search if message already in the queue
                found = (queue[j].l_clock == otherQueue[i].l_clock && queue[j].l_id == otherQueue[i].l_id);
            }

            if(found){
                queue[j-1].acks.insert(otherQueue[i].acks.begin(), otherQueue[i].acks.end()); // is the same message -> merge ack
            }else{
                queue.push_back(otherQueue[i]);
                std::push_heap(queue.begin(),queue.end(),is_after_qe);
            }
        }
    }

    checkTopMessage();

    return;

}

void Node::finish(){
    //TODO
}
