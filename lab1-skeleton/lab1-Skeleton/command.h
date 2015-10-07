// UCLA CS 111 Lab 1 command interface
#include <stdbool.h>
#include <stdio.h>
 // safely allocate memory (provided by skeleton)
//+#include <error.h>
#include <ctype.h>
// define isalnum(): returns value different from zero (i.e., true)
// if indeed c is either a digit or a letter. Zero (i.e., false) otherwise.
#include <limits.h>   // INT_MAX
//  #include <stdbool.h>  // required for boolean functions (ref)
#include <stdlib.h>   // to free memory
#include <string.h>

#include "command-internals.h"

typedef struct command *command_t;
typedef struct command_stream *command_stream_t;
typedef struct operator_node *operator_node_t;

//enum command_type;
//enum operator_type;


//Implementation
	//Linked list for file names
		//Pointer type of filelist called filelist_t
		typedef struct filelist *filelist_t;
		//Actural structure
		struct filelist
		{
			char* file;
			filelist_t next;
		};
  //Linked list for command object
	    struct command_stream
	    {
	      command_t m_command;
	      //FIXME: filelist_t is set to NULL at all time for 1A
	      filelist_t dependency;   // the files that has dependency for this command object
	      command_stream_t next;
	      command_stream_t prev;
	    };
	//operator stack for parse()    
	typedef enum
	{
	    AND_OP,         // A && B
	    SEQUENCE_OP,    // A ; B
	    OR_OP,          // A || B
	    PIPE_OP,        // A | B
	    LPAR_OP,      // (
	    RPAR_OP,    // ) 
    } operator_type;
	struct operator_node 
	{
	  operator_node_t next;
	  operator_node_t prev;
	  operator_type content;
	};

//End-Implementation



/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);
//Implement in read-command

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t* stream);
//implement in read-command

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the integer flag is
   nonzero.  */
void execute_command (command_t, int);

/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);


//new-from the reference
/* Deallocates all allocated memory associated with a command tree  */
void free_command (command_t cmd);

/* Traverses a command tree and returns a list of all redirect files.  */
filelist_t get_depends (command_t c);

/* Returns 1 if a filelist shares dependencies with a command_stream  */
int is_dependent (filelist_t f1, filelist_t f2);

/* Executes command_stream with time travel parallelism.  */
int execute_time_travel (command_stream_t stream);


//Addition In read-command.c

/* FIXME: load_buffer function loads the buffer with input and do raw process:
1. stripe off comments
2. compress ' ' and '\t' into a single ' '
3. initialize the buffer with some small size.
the return value is the size of the content loaded.
count==0 means the first read is EOF, buffer starts with NULL
when the size of the buffer is not big enough, double the size.
if the size exceeds the INT_MAX return INT_MAX*/
size_t load_buffer(char** buffer, int (*getbyte) (void *), void *arg);


//return true if hit max limit -- INT_MAX
bool buffer_push(char* buffer, size_t* buffer_size_ptr, size_t* content_count, char c);


//command_stream_t parse(char* buffer, int* line_number);


//Small functions
char* read_word(char* buffer, int *i);
command_t build_command(command_type type, int* line);
command_t pop_command_stream(command_stream_t stream);
void push_word(char* new_word, int* num_word, size_t* buffer_size, command_t current_command);

void push_command_stream(command_stream_t* top, command_t current_command);
//void push_command_stream(command_stream_t, command_t command);
void set_input(command_t current_command, char* inword);
void set_output(command_t current_command, char* outword);

//DONE in functions2
bool is_op(char* c, int i); // DO THIS EASY and BASIC ONE FIRST
operator_type top_operator(operator_node_t op_stack_top); //just read out the type
bool is_empty_op(operator_node_t top);
int precedence(operator_type type);

//DONE in fucntions2
void push_operator(operator_node_t* op_stack_top, operator_type type);
operator_type pop_operator(operator_node_t* op_top);
command_t combine_command(command_t* first_conmmand, command_t* second_conmmand, operator_type last_op);
operator_node_t build_operator(char* buffer, int* i);
void free_op(operator_node_t op_top);


command_stream_t parse(char* buffer, int* line_number);
