**TO USE:**
Have at least two terminal tabs open. Run  `./server` on one first, then run `./client1` from the other terminal. Enter commands from client terminal. The `help` command lists usable commands. 
The server receives the input from client, parses it (sometimes via regex capture groups), and then sends a response back. 
Can have multiple client connections to the server if you open more terminal tabs and run `./client1`. The server process will fork to handle each new connection. 

Sometimes after exiting, the new connection will fail; wait a couple seconds before re-running programs to ensure that the connection works.

To compile the the server.c program, run 
```
gcc server.c serv_functions.c -o server
```

**Cool Features:**
- Custom C header file to import my serv_functions.c library into the server.c file
- Server stores commands in a "command map." It's technically a list of Command structs, not a map, but the idea is that the command struct maps the command name string to its corresponding execute command function, in a function pointer
- Server can fork to handle multiple client connections at once. Server logs printed out to the terminal window with the server process.

Written in terminal vim; used [Leak Sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer) to ensure memory safety
