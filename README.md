# CS 4390 Project

Evaluates simple mathematical expressions using a server and two clients.

## Building

`$ make`

## Testing

Start the server with a destination and port.

`$ ./sv 127.0.0.1 4444`

Start 2 clients with a destination and port that corresponds to the server.

`$ ./cl 127.0.0.1 4444`

`$ ./cl 127.0.0.1 4444`
