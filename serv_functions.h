#ifndef SERV_FUNCTIONS_H
#define SERV_FUNCTIONS_H

typedef struct {
  char* cmd;
  int (*func_ptr) (char*, int);    
} Command;

int sendMsg(char* msg, int socket_num);
char* receiveMsg(int socket_num);
size_t read_whitespace(char* msg);
char* readCommand(char* client_msg);
char* get_args(char* cmd, char* msg);
int executeCommand(char* cmd, char* msg, int socket_num);


//command exec functions

//echo: echo the argument to client
int echo_exec(char* args, int client_socket); 

#endif // SERV_FUNCTIONS_H
 
