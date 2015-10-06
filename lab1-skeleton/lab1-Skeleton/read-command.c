// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
//#include <error.h>

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
  int lineN = 0;
  size_t buffer_count = load_buffer(buffer, getbyte, arg);
  //if (buffer_count == buffer_size && buffer_size > INT_MAX/2) { perror("Input size too large, read may be incomplete"); };


  // process buffer
    
  
  command_stream_t command_stream = parse(buffer, &lineN);
   
    
  // FIXME: check error

  // TODO: deallocate memory
  free(buffer);
  //free_tokens(head);

  return command_stream;
}


/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t 
read_command_stream (command_stream_t stream)
{


  //Implementation 

  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  return 0;

  //rid of error


  //End-Implementation 
}


//Addition:

size_t load_buffer(char* buffer, int (*getbyte) (void *), void *arg)
{
/*  TO YeTian:

  TEST: dump the buffer for every case, to see if the function is okay.
  cases: 
    "asd hd2938hcnch334 348f 829mf 28439f " should not change
    "138e9    12e98   812e     " should "138e9 12e98 812e "
    "1e2 \n \n  \n \t 1d" should "1e2 \n \n \n 1d"
    "    dn     " should " dn "
    "#asdbasjdhkas   \n   \n \n asd" -> "\n \n \n asd"

  The above are short cases, you can make longer ones,
  MOST IMPORTANTLY: check if each case end with EOF,
  check if the buffer_push saves space for EOF to be inserted (including corner cases: buffer_size reaches INT_MAX, and the case where buffer_size -1 == countent_count)
  you can check INT_MAX case by replacing INT_MAX in the implementation with some small number
  ALSO: check if there are buffer overflow in corner cases*/


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
bool buffer_push(char* buffer, size_t* buffer_size, size_t* content_count, char c)
{
/*  TO YeTian:
  Check this if buffer overflow is reported correctly, if the load_buffer() passes all cases, this is the only thing left to check for this one.

  BY THE WAY, write deallocate functions for buffer if you got time */

  //Double the size of buffer when needed
  //Always leave a empty slot for goto finish
  if (*content_count  +1 >= INT_MAX)
  {
    return true;
  }
  if(*content_count  +1 == *buffer_size)
  {
    if(*buffer_size > INT_MAX/2)
    {
        //perror("Buffer size overflow");
        //abort();
        return true;
    }
    *buffer_size = *buffer_size * 2;
    buffer = checked_grow_alloc(buffer,buffer_size);
  }

  //Load

  buffer[*content_count] = c;
  *content_count=*content_count+1;
  return false;
}

bool is_word (char c)
{
  if (isalnum(c) || strchr("!%+,-./:@^_", c) != NULL)
    return true;
  return false;
}



command_stream_t parse(char* buffer, int* line_number)
{
  // Have a string available to store items
  int* line = line_number;
  command_stream_t top = NULL;
  int prev_newline = 2; 
  operator_node_t op_top = NULL;
  command_t current_command = NULL;


  for(int i = 0; buffer[i] != EOF; i++) //each loop is a newline
  {
    if(buffer[i] == ' ')//EXPERIMENTAL:consume space
    {
      continue;
    }

    if(buffer[i] == '\n' )
    {
      (*line)++;
      if(i>0 && (buffer[i-1] == '<' || buffer[i-1]) )
      {
        perror("%d: Parsing Error, in violation of (Newlines may follow any special token other than < and >)");
      }
      switch (prev_newline)
      {
        case 0: //the first \n after a command
          prev_newline++; 
          continue;

        case 1://a \n following 1 \n
          prev_newline++;
          continue;

        case 2:
          continue;
      }
    }
    //else// not a \n which is implied since all case 0,1,2 have continue to intercept, 
    //{
      //prev_newline = 0;// this should be done by the end of the loop, but not in the for(;;"here"), since continue should not proc it.
    //}
    if(is_word(buffer[i]))//meet a simple command, record this into a command object and push to stack, it should finish when meeting <,>,;,\n\n
    {
      if (prev_newline == 1){ buffer[i] = ';'; goto colon; }
	  int count_word = 0;
      size_t buffer_size = 2*sizeof(char*);

      current_command = build_command(SIMPLE_COMMAND,line); //TODO(y) return a empty command that is properly initialized.
      //int word_count = 0;
	  nextword:;
      //INTERFACE:   
      char* new_word = read_word(buffer,&i); // TODO read in a word and allocate space for it and return a pointer to that space, end with EOF, modify i so that it points to the first character not in the word.
	  push_word(new_word, &count_word, &buffer_size, current_command); //TODO, simply append new_word in the current_command's structure
      //Now buffer[i] points to something not is_word()

      // for (;isword(buffer[i]);i++)
      // {
      //   if (buffer_push(reading, &reading_size, &reading_count, buffer[i]))  
      //     {
      //       perror(":%d: Parsing error, simple command with size almost MAX_INT",line);
      //     }
      // }
      inout://THIS part actually is better off being a independent function
      switch (buffer[i])
      {
        case '<': goto word_IN; break;
        case '>': goto word_OUT; break;
        case ' ':
          if(is_word(buffer[i+1]))
            {
              i++;
              goto nextword;
            }
          if (buffer[i+1] == '<') 
            { 
              i++;
              word_IN:
              if(buffer[i+1] == ' ' && is_word(buffer[i+2]))
                { i++; } 
              if(is_word(buffer[i+1]))
                { i++; } 
              else
                {
                  perror("%d: Parsing Error, invalid input file name");
                } 
              char* inword = read_word(buffer,&i);
              set_input(current_command,inword);//TODO
            }
          if (buffer[i+1] == '>') 
            {
			  i++; //move to '>'
              word_OUT:
              if(buffer[i+1] == ' ' && is_word(buffer[i+2]))
                { i++; } //consume one white space after '<'
              if(is_word(buffer[i+1]))
                { i++; } //move to the start of next word
              else// no word detected, either after a white space or immediately after '>' 
                {
                  perror("%d: Parsing Error, invalid output file name");
                } 
              char* outword = read_word(buffer,&i);
              set_output(current_command,outword);//TODO
            }
          case EOF: goto pares_EOF;
      }
      push_command_stream(top, current_command);//TODO(y) move top to pointing to current_command, and link up the last command
    }
    else if(buffer[i]=='(')
    {
      push_operator(op_top, LPAR_OP);//TODO
    }

    // else if the item is a right paren
    //   Pop a thing off of the stack.
    //   while that thing is not a left paren
    //     = Add the thing to the string with a space

    //     = Pop a thing off of the stack

    else if(buffer[i]==')')
    {
      if(is_empty_op(op_top)){ perror("%d: Parsing Error, unparied right parenthesis");}
      enum operator_type last_op =  pop_operator(op_top);
      while(last_op != LPAR_OP)
      {
        command_t second_conmmand = pop_command_stream(top);//TODO pop a command object off top, and return command_t, report error on line (line) if trying to pop empty steam
        command_t first_conmmand = pop_command_stream(top);
        command_t command_cb = combine_command(first_conmmand, second_conmmand, last_op);
          // TODO generate a new command based on two command and a connecting op, line number take the first command's 
        push_command_stream(top, command_cb);
        last_op =  pop_operator(op_top);
        if(is_empty_op(op_top)){ perror("%d: Parsing Error, unparied right parenthesis");}
      }
      current_command = build_command(SUBSHELL_COMMAND,line);

      current_command->u.subshell_command = pop_command_stream(top);
      //push_command_stream(top,current_command); this shall be done in the goto, which is at the last part of simple command ^^^
      i++;
	  if (buffer[i] == EOF){ goto pares_EOF; }
	  goto inout;
    }
    else if(prev_newline > 0)
    {
		perror("%d: Parsing Error, in violation of (the only tokens that newlines can appear before are (, ), and the ﬁrst words of simple commands)");
    }
    else if(is_op(buffer[i]))//TODO, evaluate buffer[i] first to avoid buffer[i+1] cause segmentation fault
    {//prev_newline == 0, and this one is not a word, a (, or a ), then this must be a operator other than ( and ).
      colon://buffer[i] == ';' by conversion from \n
      operator_node_t current_op = build_operator(buffer, &i); //TODO, return a pointer
      if(is_empty_op(op_top))
      {
        push_operator(op_top, current_op->type);
      }
      else
      {
        if (precedence(current_op)>precedence(top_operator(op_top)) //TODO precedence just return a corresponding int for different ops, top_operator just peek the top element of the op_stack.
        {
          push_operator(op_top, current_op->type);
        }
        else
        {
          while(top_operator(op_top)->type != LPAR_OP && precedence(current_op) <= precedence(top_operator(op_top))
          {
            operator_type op_cb = pop_operator(op_top)->type;  //cb is short for combine
            command_t second_conmmand = pop_command_stream(top);
            command_t first_conmmand = pop_command_stream(top);
            command_t command_cb = combine_command(first_conmmand, second_conmmand, op_cb);
              // TODO generate a new command based on two command and a connecting op, line number take the first command's 
            push_command_stream(top, command_cb);
            if is_empty_op(op_top)
            {
              break;
            }
          }
          push_operator(op_top, current_op);
        }
      } 

    }
    else// not a legit character
    {
      perror("%d: Parsing Error, non-standard character.");
    }
    prev_newline = 0;
    if(false)//EMERGENCY EXIT buffer[i] == EOF
      {
        pares_EOF:
        i--;
      }
  }

  while(!is_empty_op(op_top))
  {
    operator_type op_cb = pop_operator(op_top)->type;  //cb is short for combine
    if( op_cb ==  LPAR_OP ||    
        op_cb ==  RPAR_OP) 
        {
          (*line)--;//EXPERIMENTAL to adjust this line numer to fit vvv
          perror("%d: Parsing Error, unpaired parenthesis by the EOF") //TOCHECK line number should be the line of EOF 
        } 
    command_t second_conmmand = pop_command_stream(top);
    command_t first_conmmand = pop_command_stream(top);
    command_t command_cb = combine_command(first_conmmand, second_conmmand, op_cb);
      // TODO generate a new command based on two command and a connecting op, line number take the first command's 
    push_command_stream(top, command_cb);
  }


/*  For each item in the infix (with parens) expression
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
  Remove the last character (a space) from the string  */

  free_op(op_top) = NULL;
  return top;
}



