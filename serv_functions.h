#ifndef SERV_FUNCTIONS_H
#define SERV_FUNCTIONS_H

typedef struct {
  char* cmd;
  void (*func_ptr) (char*, int);    
  char* desc;
} Command;

int sendMsg(char* msg, int socket_num);
char* receiveMsg(int socket_num);
char* readCommand(char* client_msg);
char* get_args(char* cmd, char* msg);
Command* init_cmdmap();
int executeCommand(char* cmd, char* msg, Command* cmd_map, int socket_num);


//command exec functions

//echo: echo the argument to client
//void echo_exec(char* args, int client_socket); 

#endif // SERV_FUNCTIONS_H
 
