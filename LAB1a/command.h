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

enum token_type
{
   HEAD,    // used for dummy head of token lists
   SUBSHELL,
   LEFT,
   RIGHT,
   AND,
   OR,
   PIPE,
   SEMICOLON,
   WORD
};

// Linked list of tokens. Stores type and content (for subshells and words)
typedef struct token token_t;
struct token
{
   enum token_type type;
   char* content;
   int line;
   token_t* next;
};

// Linked list of token-lists.
typedef struct token_stream token_stream_t;
struct token_stream
{
   token_t* head;
   token_stream_t* next;
};


typedef struct filelist *filelist_t;
struct filelist
{
   char* file;
   filelist_t next;
};
typedef struct command *command_t;
typedef struct command_stream *command_stream_t;
struct command_stream
{
   command_t comm;
   filelist_t depends;     // command file dependencies
   command_stream_t next;
};

typedef struct stack stack;
struct stack
{
   command_t commands[4];
   int num_items;
};

inline command_t peek (stack *s);
inline command_t pop (stack *s);
inline command_t push (stack *s, command_t cmd);
inline int size (stack *s);
inline bool is_empty (stack *s);


/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the integer flag is
   nonzero.  */
void execute_command (command_t, int);

/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);

token_t* new_token (enum token_type type, char* content, int line);
bool is_word (char c);
void free_tokens (token_stream_t* head_stream);
void free_command (command_t cmd);
token_stream_t* make_token_stream (char* script, size_t script_size);
bool make_branch (stack* ops, stack* operands);
//bool more_prio()
char* load_buffer(size_t* content, int (*getbyte) (void *), void *arg);