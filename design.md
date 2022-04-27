Design and Analysis

Message Format
The overarching goal was to reduce complexity. We used the same structure of message for both requests and replies and used a fixed length format in order to simplify parsing. The result needed to be able to represent the maximum and minimum possible result: a signed 32-bit integer was chosen since the result could be negative and multiplying two 8-bit numbers would result in a minimum and maximum number of about 16 bits, with the result multiplied by two for safety. An 8-bit value was enough to represent all operations supported and was sufficient for demonstrating mathematical operations.

We used TCP's existing connection management services and did not design a new message format for joining and terminating connections. TCP's joining and terminating connection protocol uses 3-phase commit in order to atomically decide wheter or not to create a connection or terminate a connection. 3-phase commit is non-blocking when there is a single site failure but requires an extra message propagation delay compared to 2-phase commit.

Client Logs
The server printed out any logs to standard output, as standard output is easily redirected to a file by the shell. The server prints out the Internet Protocol address of the client, Unix Timestamp, and the number 0 when the client first connects to the server. The same message is printed except the number 0 is replaced with the number 1 when the client terminates the connection. Using the Unix timestamp allows the duration of the client connection to be calculated easily by taking the difference of the timestamp when the client connected and the timestamp when the client terminated. The Unix timestamp can also be converted to an absolute time if necessary. The request sent by the client is also printed by the server along with the client's Internet Protocol address for identification.

Performance
The number of message propagation delays needed to request and receive the result of a math operation is 2 assuming that there are no retransmits. 2 messages are sent for each math operation to be computed, so the number of messages sent is linearly proportional to the number of operations requested.

Data Structures
The server's threads append to a circular queue in order to ensure that it will respond to clients in the order of requests it gets from different clients. This is implemented simply by using an array with a capacity of MAX_QUEUE entries, a head index, and a tail index. Each struct entry contains a socket file descriptor to allow the server to send a reply and the reply to be sent. The head and tail are incremented modulo the size of the queue. The queue is empty when the head == tail and full when tail + 1 % sizeof(queue) == head. Each enqueueing and dequeueing operation has a time complexity of O(1). An analysis of the space requirements shows that the space required scales linearly with the number of client requests.

Distributed Mutual Exclusion
We implemented a special case of the distributed mutual exclusion algorithms where the number of servers is one, and the shared resource(s) is the memory to which the results of the operations are read from and written to and the central processing unit(s). There is no need to implement mechanisms to reach consensus among the servers as to who is allowed to enter the critical section because there is only one server.

Mutual Exclusion
We used mutexes because multiple threads were writing to a global queue as well as its indexes. Condition variables were needed since the queueing thread must block if the queue is full and the replier thread must block if the queue is empty.

Fault Tolerance
Division by zero and any invalid opcodes sent by the client result in an ERR opcode being returned. This ensures correctness of the protocol execution by the server even if there is electromagnetic noise which causes the opcode to be modified. We are unable to handle server failures since there is only one server. The application is resilient to NUM_CLIENTS - 1 crashes in queuer threads or replier threads since only one thread is needed, however depending on the timing of the failure deadlock could occur or a reply may not be sent to a client.

Testing
The client was used as a testing component by causing it to send a random number of requests in the range [3, RAND_MAX + 3] as well as randomly assigning opcodes and arguments to each request. Additionally, each client sleeps a for a random number of microseconds to ensure testing is done with multiple timing differences between requests to make sure deadlock or other race conditions do not occur.
