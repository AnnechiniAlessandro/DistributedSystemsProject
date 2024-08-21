#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omnetpp.h>
#include <vector>
#include <set>
#include <algorithm>

#include "msgtype_m.h"
#include "queueentry.cc"

#define HEARTBEAT_PERIOD 10
#define MAX_MEX_WAIT_TIME 0.001

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
    return is_after_id(qe1.l_clock,qe1.l_id,qe2.l_clock,qe2.l_id);
}
int id_to_channel(int oth_id, int my_id){
    return oth_id > my_id ? oth_id-1 : oth_id;
}
int channel_to_id(int channel, int my_id){
    return channel >= my_id ? channel+1 : channel;
}
void fault_detected(){
    // TODO
}

class Node : public cSimpleModule
{
  private:
    int id;
    int l_clock;

    int last_committed_id;
    int last_commited_l_clock;

    int hb_next_id;
    int hb_channel;

    std::vector<QueueEntry> queue;
    QueueEntry heart_beat;

    //Nodes in the group view
    std::set<int> view;
    int num_nodes;

    std::vector<Message*> committed_msgs;

  protected:
    virtual void initialize() override;
    virtual void send_standard_message(const char *text);
    virtual void checkTopMessage();
    virtual void send_hb();
    virtual void handleStdMessage(Message *m);
    virtual void handleAckMessage(Message *m);
    virtual void handleHBMessage(Message *m);
    virtual void handleMessage(cMessage *msg) override;
    virtual void mergeQueues(const std::vector<std::vector<QueueEntry>>& otherQueues);
    virtual void finish() override;

  public:
    Node():queue(),view(),committed_msgs(){ }
};

Define_Module(Node);

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

void Node::send_hb(){

    // Any time we send and HB we check if the previous one has been acked
    if(heart_beat.l_id != -1){
        fault_detected();
        return;
    }

    // id of the following node
    int elem = (id + 1) % (int)view.size();

    // create the HB message
    Message *msg = new Message();
    msg->setL_clock(l_clock);
    msg->setL_id(elem);
    msg->setMex_type(MEXTYPE_HB);
    msg->setSender_id(id);
    msg->setText("");

    send(msg,"out",id_to_channel(elem,id));

    // save the last HB sent
    heart_beat.setMsg(msg);
}

void Node::initialize(){
    id = par("id");
    l_clock = 0;

    last_committed_id = -1;
    last_commited_l_clock = -1;

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
    hb_msg->setMex_type(2);
    scheduleAt(HEARTBEAT_PERIOD,hb_msg);

    if(id==0){
        l_clock++;
        send_standard_message("MESSAGE");
    }
}

void Node::checkTopMessage(){
    bool is_valid;
    if(queue.size()==0) return;
    do{
        is_valid = std::includes(queue[0].acks.begin(),queue[0].acks.end(),view.begin(),view.end());
        if(is_valid){
            char bub[100];
            sprintf(bub,"COMMITTED: %d - %d : %s",queue[0].l_id,queue[0].l_clock,queue[0].msg->getText());
            bubble(bub);
            last_committed_id = queue[0].l_id;
            last_commited_l_clock = queue[0].l_clock;
            committed_msgs.push_back(queue[0].msg);
            std::pop_heap(queue.begin(),queue.end(),is_after_qe);
            queue.pop_back();
        }
    }while(is_valid && queue.size()>0);
}

void Node::handleStdMessage(Message *m){

    l_clock++;

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

    l_clock++;

    // Remove the HB message
    if(heart_beat.l_id == m->getL_id() && heart_beat.l_clock == m->getL_clock()){
        heart_beat.clear();
        return;
    }

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

void Node::handleHBMessage(Message *m){
    l_clock++;

    Message *ack = new Message();
    ack->setL_clock(m->getL_clock());
    ack->setL_id(m->getL_id());
    ack->setMex_type(MEXTYPE_ACK);
    ack->setSender_id(id);
    ack->setText("ACK");

    //respond to the HB
    send(ack,"out",id_to_channel(m->getSender_id(),id));

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
    if(m->getMex_type() == MEXTYPE_ACK){
        handleAckMessage(m);
        return;
    }
    if(m->getMex_type() == MEXTYPE_HB){
        handleHBMessage(m);
        return;
    }
}

void Node::mergeQueues(const std::vector<std::vector<QueueEntry>>& otherQueues){
    std::vector<QueueEntry> mergedQueue;

    // min-heap: vector of pairs <QueueEntry, index of the queue it came from>
    std::vector<std::pair<QueueEntry, int>> minHeap;

    // Initialize the heap with the first element of each queue
    if (!queue.empty()) {
        minHeap.push_back(std::make_pair(queue.front(), 0));
    }
    for (int i = 0; i < otherQueues.size(); i++) {
        if (!otherQueues[i].empty()) {
            minHeap.push_back(std::make_pair(otherQueues[i].front(), i + 1));
        }
    }

    // Turn the vector into a heap
    std::make_heap(minHeap.begin(), minHeap.end(), [](const std::pair<QueueEntry, int>& lhs, const std::pair<QueueEntry, int>& rhs) {
        return is_after_qe(lhs.first, rhs.first);
    });

    // Create and initialize iterators to track the position in each queue
    std::vector<int> indices(otherQueues.size() + 1, 0);

    while (!minHeap.empty()) {
        // Extract the smallest element from the heap
        std::pop_heap(minHeap.begin(), minHeap.end(), [](const std::pair<QueueEntry, int>& lhs, const std::pair<QueueEntry, int>& rhs) {
            return is_after_qe(lhs.first, rhs.first);
        });
        std::pair<QueueEntry, int> topElement = minHeap.back();
        minHeap.pop_back();

        QueueEntry smallestEntry = topElement.first;
        int queueIdx = topElement.second;

        // Add the smallest entry to the merged queue
        mergedQueue.push_back(smallestEntry);

        // Advance the iterator for the queue from which the element was taken
        if (queueIdx == 0) { // localQueue
            if (++indices[0] < queue.size()) {
                minHeap.push_back(std::make_pair(queue[indices[0]], 0));
                std::push_heap(minHeap.begin(), minHeap.end(), [](const std::pair<QueueEntry, int>& lhs, const std::pair<QueueEntry, int>& rhs) {
                    return is_after_qe(lhs.first, rhs.first);
                });
            }
        } else { // otherQueues
            int otherQueueIdx = queueIdx - 1;
            if (++indices[queueIdx] < otherQueues[otherQueueIdx].size()) {
                minHeap.push_back(std::make_pair(otherQueues[otherQueueIdx][indices[queueIdx]], queueIdx));
                std::push_heap(minHeap.begin(), minHeap.end(), [](const std::pair<QueueEntry, int>& lhs, const std::pair<QueueEntry, int>& rhs) {
                    return is_after_qe(lhs.first, rhs.first);
                });
            }
        }
    }

    // Replace the local queue with the merged queue
    queue = std::move(mergedQueue);

    // Merge acks of duplicated messages in the sorted queue
    std::vector<QueueEntry>::iterator it = queue.begin();
    while (it != queue.end()) {
        // Find the range of duplicates
        std::pair<std::vector<QueueEntry>::iterator, std::vector<QueueEntry>::iterator> range = std::equal_range(
            it + 1, queue.end(), *it, [](const QueueEntry& qe1, const QueueEntry& qe2) {
                return qe1.l_clock == qe2.l_clock && qe1.l_id == qe2.l_id;
            }
        );

        if (range.first != range.second) {
            // Merge the acks
            for (std::vector<QueueEntry>::iterator mergeIt = range.first; mergeIt != range.second; mergeIt++) {
                it->acks.insert(mergeIt->acks.begin(), mergeIt->acks.end());
            }
            queue.erase(range.first, range.second);
        }
        it++;
    }

    // Remove committed messages - TODO - We need to see how to handle it

}

void Node::finish(){
    //TODO
}
