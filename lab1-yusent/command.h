// UCLA CS 111 Lab 1 command interface
#include <stdbool.h>
#include <stdio.h>
 // safely allocate memory (provided by skeleton)
#include <error.h>
#include <ctype.h>
// define isalnum(): returns value different from zero (i.e., true)
// if indeed c is either a digit or a letter. Zero (i.e., false) otherwise.
#include <limits.h>   // INT_MAX
//  #include <stdbool.h>  // required for boolean functions (ref)
#include <stdlib.h>   // to free memory
#include <string.h>
#include "command-internals.h"
#include <fcntl.h>//o_rdwr
#include <sys/types.h>//for fork
#include <sys/wait.h>//for fork
#include <unistd.h>
typedef struct command *command_t;
typedef struct command command;
typedef struct command_stream *command_stream_t;
typedef struct command_stream command_stream;
typedef struct element_stream element_stream;
typedef struct element element;
typedef struct queue queue;

enum ref_type
{
   And,First,in_put,Or,out_put,Pipe,Sequence,Subshell,Word
};


struct element
{
   element* prev;
   char* content;
   element* next;

   enum ref_type type;
   int line;
};



struct element_stream
{
      element_stream* prev;
   element* tree_head;

   element_stream* next;

};



struct command_stream
{
   command_t commend;
   command_stream_t next;
};


struct queue
{

   int num;
   command_t commands[4]; // only 4 tier of proproity of operators, only need 4 positions
};


bool is_word (char c);
inline command_t pop (queue *top);
inline command_t push (queue *top, command_t commend);
void print_command (command_t);
void execute_command (command_t, int);
element_stream* parselement (char* buffer, size_t buffer_size);
command_t read_command_stream (command_stream_t stream);
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);
void ElementFree (element_stream* first);
void CommandFree (command_t commend);
bool combine (queue* operator, queue* token);
element* build_element ( char* content, int type, int line);
bool noless_prio(command_t left,command_t right);
char* load_buffer(size_t* content, int (*getbyte) (void *), void *arg);
command_t build_command (command_type type);
int command_status (command_t);
command_t plant (element* head_atom);