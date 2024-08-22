// MESSAGE OBJECT TYPE:
// 0) Message
// 1) FaultMessage
// ...

// MESSAGE TYPE:
// 0) Normal message (sender_id == l_id)
// 1) Ack (l_id is the sender of the acked message, sender_id is the one sending the ack)
// 2) Self HB Message (to trigger the event of sending the HB message)
// 3) Heartbeat
// 4) Failure + queue exchange
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
