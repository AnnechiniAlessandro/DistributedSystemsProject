// MESSAGE TYPE:
// 0) Normal message (sender_id == l_id)
// 1) Ack (l_id is the sender of the acked message, sender_id is the one sending the ack)
// 2) Self HB Message (to trigger the event of sending the HB message)
// 3) Heartbeat
// 4) HB Ack
// 5) Failure + queue exchange
// 6) New Node Message
// 7) New Node Message (recovery info)
// 8) Old Node (a node has been excluded from the view)
// ...

class MQEntry{
    int l_id;
    int l_clock;
    string text;
}

message GenericMessage{
    int mex_type;
    int sender_id;
    int sender_clock;
}

message Message extends GenericMessage{
    int l_id;
    int l_clock;
    string text;
    int last_cid[];
    int last_cclock[];
}

message FaultMessage extends GenericMessage{
    int fault_node;
    MQEntry queue[];
}

message AckMessage extends Message{ }

message HBMessage extends Message{ }

message HBAckMessage extends Message{ }

message NewNodeMessage extends GenericMessage{
    int new_node_id;
    MQEntry queue[];
}

message NewNodeStage2Message extends NewNodeMessage{}

message NewNodeInfoMessage extends NewNodeMessage{
	int new_hb_next_id;
	MQEntry restore[];
	int new_view[];
	MQEntry restoreQueues[];
	int restoreQueuesLengths[];
}

message OldNodeMessage extends GenericMessage{}
