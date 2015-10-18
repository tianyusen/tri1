// UCLA CS 111 Lab 1 command execution

#include "command.h"
//#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //+error (1, 0, "command execution not yet implemented");

  //Implementation
  //Get rid of error dummy code
  //	c->status;
  //	time_travel;
  //End-Implementation

     pid_t childpid;

     if(c->type == SIMPLE_COMMAND)
     {
        if(!(childpid = fork())){
          if(c->input != NULL){
            int fd = open(c->input, O_RDWR);
            if(fd == -1){
              goto exe_inout_error;
            }
            if(dup2(fd, STD_IN)==-1){
              goto exe_dup_error;
            }
            close(fd);
          }
          if(c->output != NULL){
            int fd = open(c->output, O_RDWR);
            if(fd == -1){
              goto exe_inout_error;
            }
            if(dup2(fd, STD_OUT)==-1){
              goto exe_dup_error;
            }
            close(fd);
          }
          
          execvp(c->u.word[0], c->u.word);
          goto exe_error;
        }
        else{
          int status=0;
          waitpid(childpid, &stauts,0);
          c->status = status;
        }
     }

     else if(c->type == SUBSHELL_COMMAND){
      execute_command(c->u.subshell_command, time_travel);
      c->status = c->u.subshell_command->status;
     }

     else if(c->type == SEQUENCE_COMMAND)
     {
      execute_command(c->u.command[0], time_travel);
      c->status = c->u.command[0]->status; // in case abort before command[1]
      execute_command(c->u.command[1], time_travel);
      c->status = c->u.command[1]->status;
     }

     else if(c->type == PIPE_COMMAND){
      int pipefd[2];
      pipe(pipefd);
      if(!(childpid=fork()))
      {
        if(dup2(pipefd[1],1)==-1){
              goto exe_dup_error;
            }
        
        close(pipefd[0]);
        execute_command(c->u.command[0], time_travel);
        c->status = c->u.command[0]->status;
        close(pipefd[1]);
        exit(c->command[0]->status); // for parent waitpid
      }
      else{
        int status=0;
        int retpid = waitpid(childpid, &status, 0);
        
        if(dup2(pipefd[0],0)==-1){
              goto exe_dup_error;
            }
        
        close(pipefd[1]);
        execute_command(c->u.command[1], time_travel);
        //if(status == ERROR){ report error}; else ...
        c->status = c->u.command[1]->status;
        close(pipefd[0]);
        //exit(c->command[1]->status);
      }

     }

     else if(c->type == AND_COMMAND || c->type == OR_COMMAND){
      execute_command(c->u.command[0], time_travel);
      c->status = c->u.command[0]->status;
      if (((c->status == 0) && (c->type == AND_COMMAND))|| 
          ((c->status != 0) && (c->type == OR_COMMAND)))

      {
        execute_command(c->u.command[1], time_travel);
        c->status = c->u.command[1]->status;
      }
     }
     else{
      error(7, 0, "Execution Error, invalid type of command");
     }

     if(false)
     {
        exe_error:;
          error(7, 0, "Execution Error, single command failure");
          return;
        exe_dup_error:;
          error(7, 0, "Execution Error, dup2() error");
          return;
        exe_inout_error:;
          error(7, 0, "Execution Error, inout file open error");
          return;
     }
     
     return;
}
