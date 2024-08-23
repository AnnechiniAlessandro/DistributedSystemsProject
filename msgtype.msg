// MESSAGE TYPE:
// 0) Normal message (sender_id == l_id)
// 1) Ack (l_id is the sender of the acked message, sender_id is the one sending the ack)
// 2) Self HB Message (to trigger the event of sending the HB message)
// 3) Heartbeat
// 4) HB Ack
// 5) Failure + queue exchange
// 6) Failure stage 2
// ...

class MQEntry{
    int l_id;
    int l_clock;
    string text;
}

message GenericMessage{
    int mex_type;
    int sender_id;
}

message Message extends GenericMessage{
    int l_id;
    int l_clock;
    string text;
}

message FaultMessage extends GenericMessage{
    int fault_id;
    int fault_node;
    MQEntry queue[];
}

message HBAckMessage extends Message{
    int last_l_id;
    int last_l_clock;
}
