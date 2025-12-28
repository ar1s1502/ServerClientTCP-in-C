**TO USE:**
Have at least two terminal tabs open. Run  `./server` on one first, then run `./client1` from the other terminal. Enter commands from client terminal. The `help` command lists usable commands. 
The server receives the input from client, parses it (sometimes via regex capture groups), and then sends a response back. 
Can have multiple client connections to the server if you open more terminal tabs and run `./client1`. The server process will fork to handle each new connection. 

Sometimes after exiting, the new connection will fail; wait a couple seconds before re-running programs to ensure that the connection works.

To compile the the server.c program, run 
```
gcc server.c serv_functions.c -o server
```


Written in terminal vim, using [Leak Sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer)
