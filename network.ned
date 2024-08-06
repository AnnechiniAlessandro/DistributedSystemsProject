
// IN OUR CASE: 5 NODES

simple Node
{
	parameters:
		int id;
    gates:
        input in[4];
        output out[4];
}

network Net
{
    submodules:
        node0: Node {
        	id = 0;
        }
        node1: Node {
        	id = 1;
        }
        node2: Node {
        	id = 2;
        }
        node3: Node {
        	id = 3;
        }
        node4: Node {
        	id = 4;
        }
    connections:
    	node0.out[0] --> {  delay = 100ms; } --> node1.in[0];
    	node0.out[1] --> {  delay = 100ms; } --> node2.in[0];
    	node0.out[2] --> {  delay = 100ms; } --> node3.in[0];
    	node0.out[3] --> {  delay = 100ms; } --> node4.in[0];
    	
    	node1.out[0] --> {  delay = 100ms; } --> node0.in[0];
    	node1.out[1] --> {  delay = 100ms; } --> node2.in[1];
    	node1.out[2] --> {  delay = 100ms; } --> node3.in[1];
    	node1.out[3] --> {  delay = 100ms; } --> node4.in[1];
    	
    	node2.out[0] --> {  delay = 100ms; } --> node0.in[1];
    	node2.out[1] --> {  delay = 100ms; } --> node1.in[1];
    	node2.out[2] --> {  delay = 100ms; } --> node3.in[2];
    	node2.out[3] --> {  delay = 100ms; } --> node4.in[2];
    	
    	node3.out[0] --> {  delay = 100ms; } --> node0.in[2];
    	node3.out[1] --> {  delay = 100ms; } --> node1.in[2];
    	node3.out[2] --> {  delay = 100ms; } --> node2.in[2];
    	node3.out[3] --> {  delay = 100ms; } --> node4.in[3];
    	
    	node4.out[0] --> {  delay = 100ms; } --> node0.in[3];
    	node4.out[1] --> {  delay = 100ms; } --> node1.in[3];
    	node4.out[2] --> {  delay = 100ms; } --> node2.in[3];
    	node4.out[3] --> {  delay = 100ms; } --> node3.in[3];
}