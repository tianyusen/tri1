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
  
   #include <limits.h>   // INT_MAX
  //  #include <stdbool.h>  // required for boolean functions (ref)

  #include <stdio.h>    // define EOF
  #include <stdlib.h>   // to free memory
    
  #include <string.h>   
  // define strchr() 
  // retunr a pointer to the first occurrence of character in str.
  // If the character is not found, the function returns a null pointer.
//End-Implementation






/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t 
make_command_stream (int (*getbyte) (void *), void *arg)
{

  //Implementation 
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */


  /* FIXME: 
    Implemented, needs testing
    load_buffer function loads the buffer with input and do raw processing:
    1. stripe off comments
    2. compress ' ' and '\t' into a single ' '
    3. initialize the buffer with some small size.
    the return value is the size of the content loaded.
    count==0 means the first read is EOF, buffer starts with NULL
    when the size of the buffer is not big enough, double the size.
    if the size exceeds the INT_MAX return INT_MAX*/
  char* buffer;
  size_t buffer_count = load_buffer(buffer, getbyte, arg);
  if (buffer_count == buffer_size && buffer_size > INT_MAX/2) { perror("Input size too large, read may be incomplete"); };


  // process buffer
  command_stream_t command_stream = parse(buffer);
  // FIXME: check error

  // TODO: deallocate memory
  free(buffer);
  free_tokens(head);

  return command_stream;
}


/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t 
read_command_stream (command_stream_t stream)
{


  //Implementation 

  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;

  //rid of error
  s;


  //End-Implementation 
}


//Addition:

size_t load_buffer(char* buffer, int (*getbyte) (void *), void *arg)
{
  TEST: dump the buffer for every case, to see if the function is okay.
  cases: 
    "asd hd2938hcnch334 348f 829mf 28439f " should not change
    "138e9    12e98   812e     " should "138e9 12e98 812e "
    "1e2 \n \n  \n \t 1d" should "1e2 \n \n \n 1d"
    "    dn     " should " dn "




  //Rules:
  // comsume "#aaaaaaa\n" to " \n" leave out a space in the beginning
  // " \t \t" to " "

  size_t content_count = 0;
  size_t buffer_size = 2048;
  buffer = checked_malloc(buffer_size);
  char c;


  for(c=getbyte(arg); c != EOF;)
  {
    switch ( c )
    {
      case '#': //comsume "#aaaaaaa\n" to "\n"
        for(;c != EOF && c != '\n';)
          {
            c = getbyte(arg);
          }
        if ( c == EOF )
          goto finish;
        //else c == \n
        if (buffer_push(buffer, &buffer_size, &content_count, '\n'))  
          {
            goto finish;
          }
          c= getbyte(arg);    
        break;

      case '\t':
      case ' ':
        for(;c == '\t' || c == ' ';)
          {
            c = getbyte(arg);
          }
        if (c == EOF) goto finish;
        if (buffer_push(buffer, &buffer_size, &content_count, ' '))  
          {
            goto finish;
          }  
        break;

      default:
        //common characters
        if (buffer_push(buffer, &buffer_size, &content_count, c))  
          {
            goto finish;
          }    
          c = getbyte(arg);
          break;
    }//END switch

  }//end for loop when c == EOF

  finish: // when we read EOF in c, come here, or when the content_count+1 == MAX_INT.
    buffer[content_count] = EOF; //it is guaranteed that buffer_size >= content_count+1 by buffer_push
    content_count++; //implied countent_count <= buffer_size
    return buffer_size;
}


//return true if hit max limit -- INT_MAX, used by load_buffer
bool buffer_push(char* buffer, size_t* buffer_size_ptr, size_t* content_count, char c) 
{
  //Double the size of buffer when needed
  //Always leave a empty slot for goto finish
  if (*content_count  +1 == INT_MAX)
  {
    retrun true;
  }
  if(*content_count  +1 == *buffer_size)
  {
    if(*buffer_size > MAX_INT/2)
    {
        perror("Buffer size overflow");
        abort();
    }
    *buffer_size = *buffer_size * 2;
    *buffer = checked_grow_alloc(buffer,buffer_size);
  }

  //Load

  buffer[*buffer_count] = c;
  *buffer_count++;
  return false;
}

bool is_word (char c)
{
  if (isalnum(c) || strchr("!%+,-./:@^_", c) != NULL)
    return true;
  return false;
}


command_stream_t parse(char* buffer)
{
  // Have a string available to store items
  int line = 1;
  char* reading
  size_t reading_size = 256;
  size_t reading_count = 0;

  command_stream_t head = NULL;
  for(int i = 0; buffer[i] != EOF; i++)
  {
    for (;isword(buffer[i]);i++)
    {
      if (buffer_push(reading, &reading_size, &reading_count, buffer[i]))  
        {
          perror("Line %d: Parsing error, simple command with size almost MAX_INT")
        }
    }
    if (buffer[i] == ' ' && buffer[i+1] == )
  }




  For each item in the infix (with parens) expression
    If the item is a number then add it to the string with a space
    if the item is an operator then
      While the stack is not empty and an operator is at the top and the operator at the top is higher priority that the item then
        = Pop the operator on the top of the stack

        = Add the popped operator to the string with a space

      Push the item on the stack
    else if the item is a left paren
      Push the item on the stack
    else if the item is a right paren
      Pop a thing off of the stack.
      while that thing is not a left paren
        = Add the thing to the string with a space

        = Pop a thing off of the stack

  While the stack is not empty
    Pop a thing off the stack.
    Add it to the string with a space.
  Remove the last character (a space) from the string  
}



