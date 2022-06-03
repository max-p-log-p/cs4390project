CS 4390 Project

Evaluates simple mathematical expressions using a server and two clients.

Compiled on gcc (Gentoo Hardened 11.2.1_p20220115 p4) 11.2.1 20220115

To build, run make

`$ make`

Testing

Start the server with a destination and port.
./sv 127.0.0.1 4444

Start 2 clients with a destination and port that corresponds to the server.
./cl 127.0.0.1 4444
./cl 127.0.0.1 4444
