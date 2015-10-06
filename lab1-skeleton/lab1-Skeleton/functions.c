//
//  functions.c
//  tri1_tianye003
//
//  Created by YeTian on 10/4/15.
//  Copyright (c) 2015 YeTian. All rights reserved.
//


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

char* read_word(char* buffer, int *i);
command_t build_command(enum command_type type, int* line);
command_t pop_command_stream(command_stream_t stream);
void push_word(char* new_word, int* num_word, size_t* buffer_size, command_t current_command);
void push_command_stream(command_stream_t* top, command_t current_command);
void set_input(command_t current_command, char* inword);
void set_output(command_t current_command, char* outword);

char* read_word(char* buffer, int *i){
    size_t buffer_size = 2048;
    char* new_word = checked_malloc(buffer_size);
    char c = buffer[*i];
    size_t count = 0;
    
    buffer[count] = c;
    count=count+1;
    *i = *i+1;
    
    
    while ((is_word(c = buffer[*i]))&&(c != ' ')) {
        if(count+1 == buffer_size)
        {
            buffer_size = buffer_size * 2;
            *new_word = checked_grow_alloc(buffer,buffer_size);
        }
        buffer[count] = c;
        *i = *i+1;
        count++;
    }
    
    return new_word;
}

command_t build_command(enum command_type type, int* line){
    
    command_t new_command;
    new_command->type = type;
    new_command->status = -1;
    new_command->line = *line;
    
    return new_command;
    
}

command_t pop_command_stream(command_stream_t stream){
    if (stream->prev != NULL) {
        stream->prev = stream->next;
    }
    
    if (stream->next != NULL) {
        stream->next = stream->prev;
    }
    
    command_t new_command;
    new_command->type = stream->m_command->type;
    new_command->status = stream->m_command->status;
    new_command->input = stream->m_command->input;
    new_command->output = stream->m_command->output;
    new_command->line = stream->m_command->line;
    new_command->u = stream->m_command->u;
    
    free(stream->m_command);
    free(stream);
    
    return new_command;
}

void push_word(char* new_word, int* num_word, size_t* buffer_size, command_t current_command){
    if (current_command->u.word == NULL) {
        //size_t buffer_size = 2*sizeof(char*);
        current_command->u.word = checked_malloc(buffer_size);
    }
    else
    {
        if ((*num_word*(sizeof(char*))+1) >= INT_MAX)
        {
            //perror("Buffer size overflow");
            //abort();
        }
        if (((*num_word)*(sizeof(char*))+1) >= *buffer_size) {
            if ((*num_word*(sizeof(char*))+1) >= INT_MAX/2) {
                //perror("Buffer size overflow");
                //abort();
            }
            *buffer_size = *buffer_size * 2;
            *current_command->u.word = checked_grow_alloc(current_command->u.word,buffer_size);
        }
    }
    
    char* copy = checked_malloc(strlen(new_word)+1);
    strcpy(copy,new_word);
    
    current_command->u.word[*num_word]= copy;
    *num_word = *num_word+1;
    
    free(new_word);
    
}

void push_command_stream(command_stream_t* top, command_t current_command){
    command_stream_t new_stream;
    new_stream->m_command = current_command;
    new_stream->prev = NULL;
    new_stream->next = *top;
    new_stream->next->prev = new_stream;
    *top = new_stream;
}

void set_input(command_t current_command, char* inword){
    char* copy = checked_malloc(strlen(inword)+1);
    strcpy(copy, inword);
    current_command->input = copy;
    free(inword);
}

void set_output(command_t current_command, char* outword){
    char* copy = checked_malloc(strlen(outword)+1);
    strcpy(copy, outword);
    current_command->output = copy;
    free(outword);
}

