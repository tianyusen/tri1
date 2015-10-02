// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
//Implementation

  #include "alloc.h"    
  // safely allocate memory (provided by skeleton)

  #include <ctype.h>    
  // define isalnum(): returns value different from zero (i.e., true) 
  // if indeed c is either a digit or a letter. Zero (i.e., false) otherwise.

  //  #include <limits.h>   // required for INT_MAX check
  //  #include <stdbool.h>  // required for boolean functions (ref)

  #include <stdio.h>    // define EOF
  #include <stdlib.h>   // to free memory
    
  #include <string.h>   
  // define strchr() 
  // retunr a pointer to the first occurrence of character in str.
  // If the character is not found, the function returns a null pointer.
//End-Implementation


/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
//Implementation

  //Linked list for command object
    //Actural structure
    struct command_stream
    {
      command_t m_command;
      filelist_t dependency;   // the files that has dependency for this command object
      command_stream_t next;
    };
//End-Implementation



command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{

  //Implementation 
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
    //Configurations and Initializations
      size_t count =0 
      size_t buffer_size = 2048;
      char next;

    //End-Configurations

    char* buffer = checked_malloc(buffer_size);









  error (1, 0, "command reading not yet implemented");

  //rid of error
  get_next_byte;
  get_next_byte_argument;



  return 0;
  //End-Implementation 
}

command_t
read_command_stream (command_stream_t s)
{


  //Implementation 

  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;

  //rid of error
  s;


  //End-Implementation 
}
