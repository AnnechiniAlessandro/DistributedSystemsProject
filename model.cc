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

#define PRINT_RESTORE_LENGTH 0

#define MEXTYPE_STD 0
#define MEXTYPE_ACK 1
#define MEXTYPE_SELFHB 2
#define MEXTYPE_HB 3
#define MEXTYPE_HB_ACK 4
#define MEXTYPE_FAULT 5
#define MEXTYPE_NEWNODE 6
#define MEXTYPE_RECOVERYINFO 7
#define MEXTYPE_OLDNODE 8

#define NODESTATE_STD 0
#define NODESTATE_FAULT 1
#define NODESTATE_CRASHED 2
#define NODESTATE_NEWNODE 3

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
omnetpp::SimTime next_hb_time(){
    return simTime()+ HEARTBEAT_PERIOD + (rand()%10 - 5)*0.01;
}

class Node : public cSimpleModule
{
  private:
    int id;
    int l_clock;
    //int last_fault_id;

    int hb_next_id;
    int hb_channel;

    std::vector<QueueEntry> queue;
    bool still_alive_neighbour;
    int node_state;

    //Stages sets for view change
    std::set<int> received_fm;

    std::set<int> received_nn;

    //Nodes in the group view
    std::set<int> view;
    std::set<int> original_view;
    int num_nodes;
    int original_num_nodes;

    std::vector<Message*> committed_msgs;

    std::vector<std::vector<Message*>> restoreQueues;
    std::vector<int> last_committed_id_vector;
    std::vector<int> last_committed_l_clock_vector;

    //Buffered recovery message
    std::vector<NewNodeMessage*> buffered_nnm;

  protected:
    virtual void initialize() override;
    virtual void send_standard_message(const char *text);
    virtual void setMsgLastCommittedVector(Message *msg);
    virtual void commitMessage(Message *m);
    virtual void checkTopMessage();
    virtual void checkLastCommittedVector(Message *m);
    virtual void send_hb(Message *hb_msg);
    virtual void handleStdMessage(Message *m);
    virtual void handleAckMessage(Message *m);
    virtual void handleHBMessage(Message *m);
    virtual void handleHBAckMessage(HBAckMessage *m);
    virtual void handleMessage(cMessage *msg) override;
    virtual void mergeQueues(std::vector<QueueEntry> otherQueue);
    virtual void set_fault_state();
    virtual void set_std_state();
    virtual void set_newnode_state();
    virtual void revive();
    virtual void fault_detected();
    virtual void handle_fault_message(FaultMessage *fm);
    virtual void handle_oldnode_message(OldNodeMessage *m);
    virtual void handleNewNodeS1Message(NewNodeMessage *m);
    virtual void handleNewNodeInfoMessage(NewNodeInfoMessage *m);
    virtual std::vector<MQEntry> prepareFaultQueue(int failed_node);
    virtual std::vector<MQEntry> prepareNewNodeQueue();
    virtual void handleParameterChange(const char *parname) override;
    virtual void finish() override;

  public:
    Node():queue(),view(),committed_msgs(),received_fm(),received_nn(),original_view(),buffered_nnm(){ }
};

Define_Module(Node);

void Node::initialize(){
    id = par("id");
    l_clock = 0;

    num_nodes = 0;
    for(int i=0; i< gateSize("out")+1; i++){
        view.insert(i);
        original_view.insert(i);
        num_nodes++;
    }
    original_num_nodes = num_nodes;

    last_committed_id_vector = std::vector<int>();
    last_committed_l_clock_vector = std::vector<int>();
    restoreQueues = std::vector<std::vector<Message*>>();
    for(int i=0; i<num_nodes; i++){
        restoreQueues.push_back(std::vector<Message*>());
        last_committed_id_vector.push_back(-1);
        last_committed_l_clock_vector.push_back(-1);
    }

    hb_next_id = (id+1)%num_nodes;
    hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;

    //Schedule the event of sending the first HB
    Message *hb_msg = new Message();
    hb_msg->setMex_type(MEXTYPE_SELFHB);
    scheduleAt(next_hb_time(),hb_msg);

    still_alive_neighbour = true;
    node_state = NODESTATE_STD;

    //buffered_nnm = nullptr;
}

void Node::setMsgLastCommittedVector(Message *msg){
    msg->setLast_cidArraySize(last_committed_id_vector.size());
    msg->setLast_cclockArraySize(last_committed_l_clock_vector.size());
    for(int i=0; i<last_committed_id_vector.size(); i++){
        msg->setLast_cid(i,last_committed_id_vector[i]);
        msg->setLast_cclock(i,last_committed_l_clock_vector[i]);
    }
}

void Node::send_standard_message(const char *text){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    for(const int& elem : view){
        if(elem == id) continue;

        Message *msg = new Message();
        msg->setL_clock(l_clock);
        msg->setL_id(id);
        msg->setMex_type(MEXTYPE_STD);
        msg->setSender_id(id);
        msg->setText(text);

        setMsgLastCommittedVector(msg);

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

    checkTopMessage();
}

std::vector<MQEntry> Node::prepareFaultQueue(int failed_node){
    std::vector<MQEntry> result = std::vector<MQEntry>();
    for(int i = 0; i<(int)queue.size(); i++){
        MQEntry mqe = MQEntry();
        if(queue[i].msg != nullptr && queue[i].l_id == failed_node){
            queue[i].acks.insert(original_view.begin(), original_view.end());
            mqe.setL_id(queue[i].l_id);
            mqe.setL_clock(queue[i].l_clock);
            mqe.setText(queue[i].msg->getText());
            result.push_back(mqe);
        }
    }
    return result;
}

std::vector<MQEntry> Node::prepareNewNodeQueue(){
    std::vector<MQEntry> result = std::vector<MQEntry>();
    for(int i = 0; i<(int)queue.size(); i++){
        MQEntry mqe = MQEntry();
        if(queue[i].msg != nullptr){
            queue[i].acks.insert(original_view.begin(), original_view.end());
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
        getDisplayString().setTagArg("i", 1, "white");
        node_state = NODESTATE_STD;
        still_alive_neighbour = true;
        checkTopMessage();
        received_fm.clear();
        received_nn.clear();
    }
}

void Node::set_fault_state(){
    if(node_state != NODESTATE_FAULT){
        bubble("ENTERED FAULT STATE!");
        getDisplayString().setTagArg("i", 1, "yellow");
        node_state = NODESTATE_FAULT;
    }
}

void Node::set_newnode_state(){
    if(node_state != NODESTATE_NEWNODE){
        bubble("ENTERED NEW NODE STATE!");
        getDisplayString().setTagArg("i", 1, "blue");
        node_state = NODESTATE_NEWNODE;
    }
}

void Node::revive(){
    //FLUSH THE QUEUE
    for(int i=0; i<(int)queue.size(); i++){
        if(queue[i].msg != nullptr){
            delete queue[i].msg;
            queue[i].msg = nullptr;
        }
    }
    queue.clear();

    //FLUSH RESTORE QUEUES
    for(int i=0; i<(int)restoreQueues.size(); i++){
        for(int j=0; j<(int)restoreQueues[i].size(); j++){
            delete restoreQueues[i][j];
        }
        restoreQueues[i].clear();
    }

    //RESET VIEW
    view.clear();
    for(int i=0; i<original_num_nodes; i++){
        view.insert(i);
    }
    num_nodes = original_num_nodes;

    set_newnode_state();

    received_nn.insert(id);

    if(view.find(id) == view.end()){
        view.insert(id);
        num_nodes = (int)view.size();
    }

    l_clock++;
    for(const int& elem : view){
        if(elem == id) continue;
        NewNodeMessage *nnm = new NewNodeMessage();
        nnm->setMex_type(MEXTYPE_NEWNODE);
        nnm->setSender_id(id);
        nnm->setSender_clock(l_clock);
        nnm->setNew_node_id(id);
        nnm->setQueueArraySize(0);

        send(nnm,"out",id_to_channel(elem,id));
    }
    return;
}

void Node::handleNewNodeS1Message(NewNodeMessage *m){

    if(node_state == NODESTATE_CRASHED){
        delete m;
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    //If the fault was not detected yet, trigger the Fault protocol first
    if(view.find(m->getNew_node_id()) != view.end() && node_state == NODESTATE_STD){

        buffered_nnm.push_back(m);
        if(m->getNew_node_id() == hb_next_id){
            fault_detected();
        }
        return;
    }

    //If the fault has already been detected, wait for the Fault protocol to finish
    if(node_state == NODESTATE_FAULT){
        buffered_nnm.push_back(m);
        return;
    }

    if(view.find(m->getNew_node_id()) == view.end() || node_state != NODESTATE_NEWNODE){

        set_newnode_state();

        received_nn.insert(id);
        received_nn.insert(m->getSender_id());

        view.insert(m->getNew_node_id());
        num_nodes = (int)view.size();

        std::vector<QueueEntry> otherQueue = std::vector<QueueEntry>();
        for(int i=0; i<(int)m->getQueueArraySize(); i++){
            Message *qm = new Message();
            qm->setMex_type(MEXTYPE_STD);
            qm->setText(m->getQueue(i).getText());
            qm->setSender_id(m->getSender_id());
            qm->setL_id(m->getQueue(i).getL_id());
            qm->setL_clock(m->getQueue(i).getL_clock());

            QueueEntry qe = QueueEntry();
            qe.setMsg(qm);
            for(const int& elem : view){
                qe.acks.insert(elem);
            }
            otherQueue.push_back(qe);
        }
        mergeQueues(otherQueue);

        //Check if the node revived is the next HB hop
        int prev_hop = (m->getNew_node_id()+num_nodes-1)%original_num_nodes;
        while(view.find(prev_hop)==view.end() && prev_hop!=id)
            prev_hop = (prev_hop+num_nodes-1)%original_num_nodes;

        l_clock++;
        for(const int& elem : view){
            if(elem == id) continue;

            std::vector<MQEntry> list = prepareNewNodeQueue();

            if(prev_hop == id && elem == m->getNew_node_id()){

                NewNodeInfoMessage *nnm = new NewNodeInfoMessage();
                nnm->setMex_type(MEXTYPE_RECOVERYINFO);

                nnm->setQueueArraySize(list.size());
                for(int i=0; i<(int)list.size(); i++){
                    nnm->setQueue(i, list[i]);
                }

                nnm->setSender_id(id);
                nnm->setSender_clock(l_clock);
                nnm->setNew_node_id(m->getNew_node_id());

                nnm->setNew_viewArraySize(view.size());
                int put = 0;
                for(const int& el : view){
                    nnm->setNew_view(put,el);
                    put++;
                }

                nnm->setNew_hb_next_id(hb_next_id);

                hb_next_id = m->getNew_node_id();
                hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;

                nnm->setQueueArraySize(restoreQueues[m->getNew_node_id()].size());
                for(int i=0; i<(int)restoreQueues[m->getNew_node_id()].size(); i++){
                    MQEntry mqe = MQEntry();
                    mqe.setL_id(restoreQueues[m->getNew_node_id()][i]->getL_id());
                    mqe.setL_clock(restoreQueues[m->getNew_node_id()][i]->getL_clock());
                    mqe.setText(restoreQueues[m->getNew_node_id()][i]->getText());
                    nnm->setQueue(i, mqe);
                }

                send(nnm,"out",id_to_channel(elem,id));

            }else{

                NewNodeMessage *nnm = new NewNodeMessage();
                nnm->setMex_type(MEXTYPE_NEWNODE);

                nnm->setQueueArraySize(list.size());
                for(int i=0; i<(int)list.size(); i++){
                    nnm->setQueue(i, list[i]);
                }

                nnm->setSender_id(id);
                nnm->setSender_clock(l_clock);
                nnm->setNew_node_id(m->getNew_node_id());

                send(nnm,"out",id_to_channel(elem,id));
            }
        }
    }else{

        received_nn.insert(m->getSender_id());

        std::vector<QueueEntry> otherQueue = std::vector<QueueEntry>();
        for(int i=0; i<(int)m->getQueueArraySize(); i++){
            Message *qm = new Message();
            qm->setMex_type(MEXTYPE_STD);
            qm->setText(m->getQueue(i).getText());
            qm->setSender_id(m->getSender_id());
            qm->setL_id(m->getQueue(i).getL_id());
            qm->setL_clock(m->getQueue(i).getL_clock());

            QueueEntry qe = QueueEntry();
            qe.setMsg(qm);
            for(const int& elem : view){
                qe.acks.insert(elem);
            }
            otherQueue.push_back(qe);
        }
        mergeQueues(otherQueue);
    }

    //Check for completion
    if(std::includes(received_nn.begin(),received_nn.end(),view.begin(),view.end())){

        set_std_state();

    }

    delete m;

}

void Node::handleNewNodeInfoMessage(NewNodeInfoMessage *m){

    if(node_state == NODESTATE_CRASHED){
        delete m;
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    view.clear();
    for(int i=0; i<(int)m->getNew_viewArraySize()>0; i++){
        view.insert(m->getNew_view(i));
    }
    num_nodes = (int)view.size();

    received_nn.insert(m->getSender_id());

    std::vector<QueueEntry> otherQueue = std::vector<QueueEntry>();
    for(int i=0; i<(int)m->getQueueArraySize(); i++){
        Message *qm = new Message();
        qm->setMex_type(MEXTYPE_STD);
        qm->setText(m->getQueue(i).getText());
        qm->setSender_id(m->getSender_id());
        qm->setL_id(m->getQueue(i).getL_id());
        qm->setL_clock(m->getQueue(i).getL_clock());

        QueueEntry qe = QueueEntry();
        qe.setMsg(qm);
        for(const int& elem : view){
            qe.acks.insert(elem);
        }
        otherQueue.push_back(qe);
    }
    mergeQueues(otherQueue);

    hb_next_id = m->getNew_hb_next_id();
    hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;

    if(std::includes(received_nn.begin(),received_nn.end(),view.begin(),view.end())){
        set_std_state();
    }

    delete m;

}

void Node::fault_detected(){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    int failed_node = hb_next_id;

    view.erase(failed_node);
    num_nodes = (int)view.size();

    if(num_nodes == 1){

        hb_next_id = id;

        set_std_state();

        while(buffered_nnm.size() > 0){
            NewNodeMessage *nnm = buffered_nnm[0];
            buffered_nnm.erase(buffered_nnm.begin());
            handleNewNodeS1Message(nnm);
        }
        return;

    }

    hb_next_id = (hb_next_id+1)%original_num_nodes;
    while(view.find(hb_next_id) == view.end()){
        hb_next_id = (hb_next_id+1)%original_num_nodes;
    }
    hb_channel = hb_next_id > id ? hb_next_id-1 : hb_next_id;

    set_fault_state();
    received_fm.insert(id);

    l_clock++;
    for(const int& elem : view){
        if(elem==id) continue;

        //Prepare the queue to be sent
        std::vector<MQEntry> list = prepareFaultQueue(failed_node);

        FaultMessage *fm = new FaultMessage();
        fm->setQueueArraySize(list.size());
        for(int i=0; i<(int)list.size(); i++){
            fm->setQueue(i, list[i]);
        }
        fm->setMex_type(MEXTYPE_FAULT);
        fm->setSender_id(id);
        fm->setSender_clock(l_clock);
        fm->setFault_node(failed_node);

        send(fm,"out",id_to_channel(elem,id));
    }

    return;
}

void Node::handle_fault_message(FaultMessage *fm){

    if(node_state == NODESTATE_CRASHED){
        delete fm;
        return;
    }

    l_clock = MAX(l_clock,fm->getSender_clock()) + 1;

    if(node_state == NODESTATE_NEWNODE){
        delete fm;
        return;
    }

    //Ignore if the fault has already been handled
    if(view.find(fm->getFault_node())==view.end() && node_state!=NODESTATE_FAULT){
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
    mergeQueues(otherQueue);

    received_fm.insert(fm->getSender_id());

    if(view.find(fm->getFault_node()) != view.end()){
        //First time receiving the fault message
        set_fault_state();

        int failed_node = fm->getFault_node();

        //Prepare the queue to be sent
        std::vector<MQEntry> list = prepareFaultQueue(failed_node);
        view.erase(failed_node);
        num_nodes = (int)view.size();

        received_fm.insert(id);

        l_clock++;
        for(const int& elem : view){
            if(elem==id) continue;

            FaultMessage *fm = new FaultMessage();
            fm->setQueueArraySize(list.size());
            for(int i=0; i<(int)list.size(); i++){
                fm->setQueue(i, list[i]);
            }
            fm->setMex_type(MEXTYPE_FAULT);
            fm->setSender_id(id);
            fm->setSender_clock(l_clock);
            fm->setFault_node(failed_node);

            send(fm,"out",id_to_channel(elem,id));
        }
    }

    //Check for completion
    if(std::includes(received_fm.begin(),received_fm.end(),view.begin(),view.end())){

        set_std_state();

        while(buffered_nnm.size() > 0){
            NewNodeMessage *nnm = buffered_nnm[0];
            buffered_nnm.erase(buffered_nnm.begin());
            handleNewNodeS1Message(nnm);
        }

    }

    delete fm;

}

void Node::handle_oldnode_message(OldNodeMessage *m){
    if(node_state == NODESTATE_CRASHED){
        delete m;
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    if(node_state == NODESTATE_FAULT){
        delete m;
        return;
    }

    if(node_state == NODESTATE_NEWNODE){
        delete m;
        return;
    }

    delete m;

    revive();

    return;
}

void Node::send_hb(Message *hb_msg){

    scheduleAt(next_hb_time(),hb_msg);

    if(num_nodes == 1){
        return;
    }

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    //Postpone if we are handling a fault
    if(node_state == NODESTATE_FAULT || node_state == NODESTATE_NEWNODE){
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
    l_clock++;
    Message *msg = new HBMessage();
    msg->setL_clock(l_clock);
    msg->setL_id(elem);
    msg->setMex_type(MEXTYPE_HB);
    msg->setSender_id(id);
    msg->setSender_clock(l_clock);
    msg->setText("HEARTBEAT");

    setMsgLastCommittedVector(msg);

    send(msg,"out",hb_channel);

    // set still alive to false
    still_alive_neighbour = false;
}

void Node::commitMessage(Message *m){
    if(is_after_id(m->getL_clock(),m->getL_id(),last_committed_l_clock_vector[id],last_committed_id_vector[id])){
        last_committed_id_vector[id] = m->getL_id();
        last_committed_l_clock_vector[id] = m->getL_clock();
        committed_msgs.push_back(m);
    }
}

void Node::checkTopMessage(){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    if(node_state == NODESTATE_FAULT || node_state == NODESTATE_NEWNODE) return;

    bool is_valid;
    bool is_invalid;
    if(queue.size()==0) return;
    do{
        is_valid = std::includes(queue[0].acks.begin(),queue[0].acks.end(),view.begin(),view.end());
        is_invalid = !is_after_id(queue[0].l_clock,queue[0].l_id,last_committed_l_clock_vector[id],last_committed_id_vector[id]);

        if(is_valid && !is_invalid){
            char bub[100];
            sprintf(bub,"COMMITTED: %d - %d : %s",queue[0].l_id,queue[0].l_clock,queue[0].msg->getText());
            bubble(bub);

            commitMessage(queue[0].msg);

            for(int i=0; i<(int)restoreQueues.size(); i++){
                if(i == id) continue;

                Message *rm = new Message();
                rm->setL_clock(queue[0].msg->getL_clock());
                rm->setL_id(queue[0].msg->getL_id());
                rm->setMex_type(MEXTYPE_STD);
                rm->setSender_id(queue[0].msg->getSender_id());
                rm->setText(queue[0].msg->getText());

                restoreQueues[i].push_back(rm);
            }

            std::pop_heap(queue.begin(),queue.end(),is_after_qe);
            queue.pop_back();
        }
        if(is_invalid){
            std::pop_heap(queue.begin(),queue.end(),is_after_qe);
            queue.pop_back();
        }
    }while((is_valid || is_invalid) && queue.size()>0);
}

void Node::checkLastCommittedVector(Message *m){
    for(int i=0; i<(int)last_committed_id_vector.size(); i++){
        if(i == id) continue;
        if( is_after_id(m->getLast_cclock(i),m->getLast_cid(i),last_committed_l_clock_vector[i],last_committed_id_vector[i]) ){

            last_committed_l_clock_vector[i] = m->getLast_cclock(i);
            last_committed_id_vector[i] =  m->getLast_cid(i);

            while((int)restoreQueues[i].size() > 0 &&
                    !is_after_id(restoreQueues[i][0]->getL_clock(),restoreQueues[i][0]->getL_id(),
                            last_committed_l_clock_vector[i],last_committed_id_vector[i])){
                restoreQueues[i].erase(restoreQueues[i].begin());
            }

        }
    }

    //PRINT THE SIZE OF THE RESTORE QUEUES
    if(PRINT_RESTORE_LENGTH){
        char aaaa[1024];
        sprintf(aaaa,"RESTORE items: ");
        for(int i=0; i<original_num_nodes; i++){
            sprintf(aaaa,"%s %d",aaaa,restoreQueues[i].size());
        }
        bubble(aaaa);
    }
}

void Node::handleStdMessage(Message *m){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    checkLastCommittedVector(m);

    if(!is_after_id(m->getL_clock(),m->getL_id(),last_committed_l_clock_vector[id],last_committed_id_vector[id])){
        delete m;
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

    //SEND ACKS TO OTHER NODES
    l_clock++;
    for (const int& elem : view) {
        if(elem != id){

            Message *ack = new AckMessage();
            ack->setL_clock(m->getL_clock());
            ack->setL_id(m->getL_id());
            ack->setMex_type(MEXTYPE_ACK);
            ack->setSender_id(id);
            ack->setSender_clock(l_clock);
            ack->setText("ACK");

            setMsgLastCommittedVector(ack);

            send(ack,"out",id_to_channel(elem,id));
        }
    }

    if(node_state == NODESTATE_STD)
        checkTopMessage();

    return;
}

void Node::handleAckMessage(Message *m){

    if(node_state == NODESTATE_CRASHED){
        delete m;
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    checkLastCommittedVector(m);

    //Check if an ACK has already arrived
    QueueEntry *qe = nullptr;
    for(int i=0; i<(int)queue.size(); i++){
        if(queue[i].l_id == m->getL_id() && queue[i].l_clock == m->getL_clock()){
            qe = &queue[i];
        }
    }

    if(qe == nullptr){

        if(is_after_id(m->getL_clock(),m->getL_id(),last_committed_l_clock_vector[id],last_committed_id_vector[id])){
            //INSERT (empty) MESSAGE IN QUEUE
            QueueEntry newqe = QueueEntry();
            newqe.setMsg(nullptr);
            newqe.l_clock = m->getL_clock();
            newqe.l_id = m->getL_id();

            //Add own ack AND sender ack
            newqe.acks.insert(id);
            newqe.acks.insert(m->getSender_id());

            queue.push_back(newqe);
            std::push_heap(queue.begin(),queue.end(),is_after_qe);
        }
    }else{
        //UPDATE ACK LIST
        qe->acks.insert(m->getSender_id());
    }

    if(node_state == NODESTATE_STD)
        checkTopMessage();

    delete m;
    return;
}

void Node::handleHBAckMessage(HBAckMessage *m){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    still_alive_neighbour = true;

    checkLastCommittedVector(m);

    delete m;
    return;
}

void Node::handleHBMessage(Message *m){

    if(node_state == NODESTATE_CRASHED){
        return;
    }

    l_clock = MAX(l_clock,m->getSender_clock()) + 1;

    checkLastCommittedVector(m);

    //Postpone if we are handling a fault
    if(node_state == NODESTATE_FAULT || node_state == NODESTATE_NEWNODE){
        return;
    }

    l_clock++;
    HBAckMessage *ack = new HBAckMessage();
    ack->setL_clock(m->getL_clock());
    ack->setL_id(m->getL_id());
    ack->setMex_type(MEXTYPE_HB_ACK);
    ack->setSender_id(id);
    ack->setSender_clock(l_clock);
    ack->setText("ACK");

    setMsgLastCommittedVector(ack);

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

    if(node_state == NODESTATE_CRASHED){
        delete t_msg;
        return;
    }

    if(gm->getMex_type() == MEXTYPE_NEWNODE){
        NewNodeMessage *nnm = (NewNodeMessage*) gm;
        handleNewNodeS1Message(nnm);
        return;
    }

    //Handle if Sender is not in the view
    if(view.find(gm->getSender_id()) == view.end()){
        l_clock++;
        OldNodeMessage *onm = new OldNodeMessage();
        onm->setMex_type(MEXTYPE_OLDNODE);
        onm->setSender_id(id);
        onm->setSender_clock(l_clock);
        send(onm,"out",id_to_channel(gm->getSender_id(),id));
        delete gm;
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
        HBAckMessage *m = (HBAckMessage*) gm;
        handleHBAckMessage(m);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_FAULT){
        FaultMessage *fm = (FaultMessage*) gm;
        handle_fault_message(fm);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_RECOVERYINFO){
        NewNodeInfoMessage *nnm = (NewNodeInfoMessage*) gm;
        handleNewNodeInfoMessage(nnm);
        return;
    }
    if(gm->getMex_type() == MEXTYPE_OLDNODE){
        OldNodeMessage *nnm = (OldNodeMessage*) gm;
        handle_oldnode_message(nnm);
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
        if(is_after_id(otherQueue[i].l_clock, otherQueue[i].l_id, last_committed_l_clock_vector[id], last_committed_id_vector[id])){ // skip already committed messages

            found = false;
            for(j=0; j<lenQ && !found; j++){ // search if message already in the queue
                found = (queue[j].l_clock == otherQueue[i].l_clock && queue[j].l_id == otherQueue[i].l_id);
            }

            if(found){
                queue[j-1].acks.insert(original_view.begin(), original_view.end());
                if(queue[j-1].msg == nullptr){
                    queue[j-1].setMsg(otherQueue[i].msg);
                }else{
                    if(otherQueue[i].msg!=nullptr)
                        delete otherQueue[i].msg;
                }
            }else{
                otherQueue[i].acks.insert(original_view.begin(), original_view.end());
                queue.push_back(otherQueue[i]);
                std::push_heap(queue.begin(),queue.end(),is_after_qe);
            }
        }else{
            if(otherQueue[i].msg!=nullptr)
                delete otherQueue[i].msg;
        }
    }

    return;
}

void Node::handleParameterChange(const char *parname) {
    if (strcmp(parname, "alive") == 0) {
        if(par("alive").boolValue()){
            if(node_state == NODESTATE_CRASHED){
                revive();
            }
            return;
        }else{
            bubble("CRASHED :(");
            node_state = NODESTATE_CRASHED;
            getDisplayString().setTagArg("i", 1, "red");
        }
    }

    if (strcmp(parname, "new_mex") == 0) {
        if(par("new_mex").intValue() > 0){
            if(node_state == NODESTATE_STD || node_state == NODESTATE_FAULT || node_state == NODESTATE_NEWNODE){
                l_clock++;
                char message_text[1024];
                sprintf(message_text,"Message{%d,%d}",l_clock,id);
                send_standard_message(message_text);
                par("new_mex").setIntValue(par("new_mex").intValue()-1);
                bubble(message_text);
            }else if(node_state == NODESTATE_CRASHED){
                bubble("Cannot send messages while crashed :(");
                par("new_mex").setIntValue(0);
            }else if(node_state == NODESTATE_NEWNODE){
                bubble("Cannot send messages during node addition...");
                par("new_mex").setIntValue(0);
            }
        }else if(par("new_mex").intValue() < 0){
            par("new_mex").setIntValue(0);
        }
    }
}

void Node::finish(){
    char final_message[1024];
    sprintf(final_message,"COMMITTED MESSAGES by %d: ",id);
    for(int i=0; i<(int)committed_msgs.size(); i++){
        sprintf(final_message,"%s (%d,%s) ",final_message,committed_msgs[i]->getL_id(),committed_msgs[i]->getText());
    }

    cCanvas *canvas = getParentModule()->getCanvas();
    cTextFigure *textFigure = new cTextFigure("text");
    textFigure->setText(final_message);
    textFigure->setPosition(cFigure::Point(600, 100+30*id));
    canvas->addFigure(textFigure);
}
