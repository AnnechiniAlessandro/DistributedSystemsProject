
// MESSAGE TYPE:
// 0) Normal message (sender_id == l_id)
// 1) Ack (l_id is the sender of the acked message, sender_id is the one sending the ack)
// 2) Self HB Message (to trigger the event of sending the HB message)
// 3) Heartbeat
// ...

message Message {
    int sender_id;
    int l_id;
    int l_clock;
    string text;
    int mex_type;
}
