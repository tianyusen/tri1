
#include "alloc.h"
#include "command.h"




// Build token with: type, pointer of content string
token_t* new_token (enum token_type type, char* content, int line)
{
	token_t* tok = checked_malloc(sizeof(token_t));

	tok->type = type;
	tok->content = content;
	tok->line = line;
	tok->next = NULL;

	return tok;
}

bool is_word (char c)
{
	if (isalnum(c) || strchr("!%+,-./:@^_", c) != NULL)
		return true;
	return false;
}

// Deallocates token stream, token->content is used for read_command, not freed
void free_tokens (token_stream_t* head_stream)
{
	token_stream_t* curr_stream = head_stream;
	token_stream_t* prev_stream;

	while (curr_stream != NULL)
	{
		token_t* curr = curr_stream->head;
		token_t* prev;

		while (curr != NULL)
		{
			prev = curr;
			curr = curr->next;
			free(prev);
		}
		prev_stream = curr_stream;
		curr_stream = curr_stream->next;
		free(prev_stream);
	}
	return;
}

// deallocate an entire command tree
void free_command (command_t cmd)
{
	int i = 1;
	switch(cmd->type)
	{
		case SUBSHELL_COMMAND:
			free(cmd->input); free(cmd->output);
			free_command(cmd->u.subshell_command);
			free(cmd->u.subshell_command);
			break;

		case SIMPLE_COMMAND:
			free(cmd->input); free(cmd->output);
			while(cmd->u.word[i])
			{
				free(cmd->u.word[i]);
				i++;
			}
			break;

		case AND_COMMAND:
		case OR_COMMAND:
		case PIPE_COMMAND:
		case SEQUENCE_COMMAND:
			free_command(cmd->u.command[0]); free(cmd->u.command[0]);
			free_command(cmd->u.command[1]); free(cmd->u.command[1]);
			break;

		default:
			error(3, 0, "Invalid command_Type to free.");
	};
	return ;
}

// convert input to token stream
token_stream_t* make_token_stream (char* script, size_t script_size)
{
	token_t* head_token = new_token(HEAD, NULL, -1);
	token_t* curr_token = head_token;
	token_t* prev_token = NULL;

	token_stream_t* head_stream = checked_malloc(sizeof(token_stream_t));
	token_stream_t* curr_stream = head_stream;
	curr_stream->head = head_token;

	bool newline_to_colon = false;
	int line = 1;
	size_t index = 0;
	char c = *script;
	while (index < script_size)
	{
		if (c == '(') // case '(' merge all in () into a token
		{
			int subshell_line = line;
			int nested = 1;

			size_t count = 0;
			size_t subshell_size = 64;
			char* subshell = checked_malloc(subshell_size);

			// take all content in between () into the sub token
			while (nested > 0)
			{
				script++; index++; c = *script;
				if (index == script_size)//hit end of buffer
				{
					line = subshell_line;
					goto parse_error;
				}

				if (c == '\n')
				{
					// Aggresively consume \n  and ' ' to find the ')'
					while (script[1] == ' ' || script[1] == '\t' || script[1] == '\n')
					{
						if (script[1] == '\n')
							line++;

						script++;
						index++;
					}

					// convert to colon
					c = ';';
					newline_to_colon = true;
					line++;
				}
				else if (c == '(') // count nest subshell level
					nested++;
				else if (c == ')') // decount nest subshell level
				{
					nested--;
					if (nested == 0) // all ( are paired
					{
						script++; index++; c = *script; // consume last close parens
						break;
					}
				}

				// load one char into subshell_buffer
				subshell[count] = c;
				count++;

				// check buffer size and expand
				if (count == subshell_size)
				{
					subshell_size = subshell_size * 2;
					subshell = checked_grow_alloc (subshell, &subshell_size);
				}
			}

			// build subshell token and push
			curr_token->next = new_token(SUBSHELL, subshell, subshell_line);
			curr_token = curr_token->next;
		}
		else if (c == '<') // in
		{
			curr_token->next = new_token(LEFT, NULL, line);
			curr_token = curr_token->next;

			script++; index++; c = *script;
		}
		else if (c == '>') // out
		{
			curr_token->next = new_token(RIGHT, NULL, line);
			curr_token = curr_token->next;

			script++; index++; c = *script;
		}
		else if (c == '|') // check | or ||
		{
			script++; index++; c = *script; // check next char

			if (c== '|') // OR
			{
				curr_token->next = new_token(OR, NULL, line);
				curr_token = curr_token->next;

				script++; index++; c = *script;
			}
			else // PIPE
			{
				curr_token->next = new_token(PIPE, NULL, line);
				curr_token = curr_token->next;
			}
		}
		else if (c == '&') //&& & is invalid
		{
			script++; index++; c = *script;

			if (c == '&')
			{
				curr_token->next = new_token(AND, NULL, line);
				curr_token = curr_token->next;
				script++; index++; c = *script;
			}
			else 
			{
				goto parse_error;
			}
		}
		else if (c == ';')
		{
			curr_token->next = new_token(SEMICOLON, NULL, line);
			curr_token = curr_token->next;

			script++; index++; c = *script;
		}
		else if (c == ' ' || c == '\t')// consume whitespace
		{
			
			script++; index++; c = *script;
		}
		else if (c == '\n')
		{
			line++;
			if (curr_token->type == LEFT ||	curr_token->type == RIGHT)//cannot follow <>
			{
				goto parse_error;
			}
			else if (curr_token->type == WORD || curr_token->type == SUBSHELL)
			{// command completed, start new tree, start next token_stream only if current stream has been used
				
				if (curr_token->type != HEAD)
				{
					curr_stream->next = checked_malloc(sizeof(token_stream_t));
					curr_stream = curr_stream->next;
					curr_stream->head = new_token(HEAD, NULL, -1);
					curr_token = curr_stream->head;
				}
			}
			// curr is operator needing a right operand!!! consume all of the \n 

			script++; index++; c = *script;
		}
		else if (is_word(c))
		{
			size_t count = 0;
			size_t word_size = 8;
			char* word = checked_malloc(word_size);

			do
			{
				// load word buffer
				word[count] = c;
				count++;

				// epxand buffer
				if (count == word_size)
				{
					word_size = word_size * 2;
					word = checked_grow_alloc(word, &word_size);
				}

				script++; index++; c = *script;
			} while (is_word(c) && index < script_size);

			// buil;d word token
			curr_token->next = new_token(WORD, word, line);
			curr_token = curr_token->next;
		}
		//else if (c == ')')
		//{
		//	goto parse_error;
		//}
		else // INVALID character and Exit for error situations
		{
			parse_error:;
			error(2, 0, "%d: Parsing Error", line);
			return NULL;
		}
	}

	return head_stream;
}

// combine then push into operand stack
bool make_branch (stack* ops, stack* operands)
{
	if (size(operands) < 2)// no enough operand found
		return false;
	command_t right_child = pop(operands);
	command_t left_child = pop(operands);
	command_t new_cmd = pop(ops);
	new_cmd->u.command[0] = left_child;
	new_cmd->u.command[1] = right_child;
	push(operands, new_cmd);

	return true;
}

// token to single command tree
command_t make_command_tree (token_t* head_tok)
{
	if(!head_tok)
	{
		return NULL;
	}

	token_t* curr_tok = head_tok;
	int line = curr_tok->line; 

	stack* ops = checked_malloc(sizeof(stack));	
	ops->num_items = 0;
	stack* operands = checked_malloc(sizeof(stack)); 
	operands->num_items = 0;

	command_t prev_cmd = NULL;
	command_t curr_cmd = NULL;


	do
	{
		if( !(curr_tok->type == LEFT || curr_tok->type == RIGHT) )
		{
			// make new command
			curr_cmd = checked_malloc(sizeof( struct command ));
		}

		switch (curr_tok->type)
		{
			case SUBSHELL:
				curr_cmd->type = SUBSHELL_COMMAND;

				curr_cmd->u.subshell_command = make_command_tree(
					make_token_stream(curr_tok->content, strlen(curr_tok->content))->head);

				push(operands, curr_cmd);
				break;

			case LEFT:
				//prev must be subshell or word
				if (prev_cmd == NULL || 
					!(prev_cmd->type == SIMPLE_COMMAND || prev_cmd->type == SUBSHELL_COMMAND) ||
					prev_cmd->input != NULL)
				{
					goto inout_error;
				}
				// if (prev_cmd->output != NULL)
				// {
				// 	error
				// 	return NULL;
				// }
				curr_tok = curr_tok->next;

				if (curr_tok->type == WORD) // followed by a word
				{
					prev_cmd->input = curr_tok->content;
				}
				else
				{
					goto inout_error;
				}
				// just the attri of prev command, dont push operand
				break;

			case RIGHT: //same to LEFT
				if (prev_cmd == NULL ||
					!(prev_cmd->type == SIMPLE_COMMAND || prev_cmd->type == SUBSHELL_COMMAND) ||
					prev_cmd->output != NULL)
				{
					goto inout_error;
				}

				curr_tok = curr_tok->next;

				if (curr_tok->type == WORD) // followed by a word
				{
					prev_cmd->output = curr_tok->content;
				}
				else
				{
					inout_error:
					error(2, 0, "%d: Build Command Tree Error, inout", line);
					return NULL;
				}
				break;

			case AND:
				curr_cmd->type = AND_COMMAND;

				// if AND has lower priority than op on stack top, process
				if (!is_empty(ops) &&
						(peek(ops)->type == PIPE_COMMAND ||
						 peek(ops)->type == OR_COMMAND ||
						 peek(ops)->type == AND_COMMAND))
				{
					if (!make_branch(ops, operands))//pushed new operand to stack
					{
						goto combine_error;
					}
				}

				// push new And op to ops
				push(ops, curr_cmd);
				break;

			case OR:
				curr_cmd->type = OR_COMMAND;

				if (	!is_empty(ops) &&
						(	peek(ops)->type == PIPE_COMMAND ||
							peek(ops)->type == OR_COMMAND ||
							peek(ops)->type == AND_COMMAND
						)
					)
				{
					if (!make_branch(ops, operands))
					{
						goto combine_error;
					}
				}

				// push OR to ops
				push(ops, curr_cmd);
				break;

			case PIPE:
				curr_cmd->type = PIPE_COMMAND;

				// if PIPE has <= priority to operands, pop
				if (!is_empty(ops) && peek(ops)->type == PIPE_COMMAND)
				{
					if (!make_branch(ops, operands))
					{
						goto combine_error;
					}
				}

				// push PIPE to ops
				push(ops, curr_cmd);
				break;

			case SEMICOLON:
				curr_cmd->type = SEQUENCE_COMMAND;

				// always pop since SEMICOLON <= all ops
				if (!is_empty(ops))
				{
					if (!make_branch(ops, operands))
					{
						combine_error:;
						error(2, 0, "%d: Build Command Tree Error, combine", line);
						return NULL;
					}
				}

				// push SEMICOLON to ops
				push(ops, curr_cmd);
				break;

			case WORD:
				curr_cmd->type = SIMPLE_COMMAND;

				// count number of following words
				size_t num_words = 1;
				token_t* ct = curr_tok;
				while (ct->next != NULL && ct->next->type == WORD)
				{
					num_words++;
					ct = ct->next;
				}

				// create string array
				curr_cmd->u.word = checked_malloc((num_words+1) * sizeof(char*));

				// load words
				curr_cmd->u.word[0] = curr_tok->content;
				size_t i;
				for (i = 1; i < num_words; i++)
				{
					curr_tok = curr_tok->next;
					curr_cmd->u.word[i] = curr_tok->content;
				}

				// set last word pointer to NULL
				curr_cmd->u.word[num_words] = NULL;

				// push SIMPLE COMMAND to operands
				push(operands, curr_cmd);
				break;

			default:
				break;
		};

		prev_cmd = curr_cmd; // save previous command for operand checks
	} while(curr_tok != NULL && (curr_tok = curr_tok->next) != NULL);

	// finish tree from existing stack
	while(size(ops) > 0)
	{
		if (!make_branch(ops, operands))
		{
			error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
			return NULL;
		}
	}

	// check for single root
	if (size(operands) != 1)
	{
		error(2, 0, "Line %d: Syntax error. Tree did not converge into single root.", line);
		return NULL;
	}

	command_t root = pop(operands); // the root should be the final tree left in operands

	// free memory
	free(ops); free(operands);

	return root;
}




// Converts a token stream into a command forest
command_stream_t 
make_command_forest (token_stream_t* token_stream)
{
	token_stream_t* curr_stream = token_stream;

	command_stream_t head_tree = NULL;
	command_stream_t curr_tree = NULL;
	command_stream_t prev_tree = NULL;

	while (curr_stream != NULL && curr_stream->head->next != NULL)
	{
		token_t* curr = curr_stream->head->next;	// head is ended with dummy
		command_t cmd = make_command_tree (curr);	// root

		curr_tree = checked_malloc(sizeof(struct command_stream));
		curr_tree->comm = cmd;
		//curr_tree->depends = get_depends(cmd);

		if (!head_tree)
		{
			head_tree = curr_tree;
			prev_tree = head_tree;
		}
		else
		{
			prev_tree->next = curr_tree;
			prev_tree = curr_tree;
		}

		curr_stream = curr_stream->next;
	}

	return head_tree;
}
char* load_buffer(size_t* content, int (*getbyte) (void *), void *arg)
{
   *content = 0;
   size_t buffer_size = 1000;
   char* buffer = checked_malloc(buffer_size);
   char c = 0;

   for(c = getbyte(arg);c != EOF;c = getbyte(arg))
   {
      if (c == '#') //comments
      {
         while ( c != EOF && c != '\n')
         {
            c = getbyte(arg);
         } 
         continue;
      }
     // load into buffer
     buffer[*content] = c;
     (*content)++;

     // expand buffer
     if (*content == INT_MAX)
     {
        error(1, 0, "BUFFER OVERFLOW");
     }
     else if (*content == buffer_size)
     {
        buffer_size = buffer_size * 2;
        buffer = checked_grow_alloc (buffer, &buffer_size);
     }
      
   }
   return buffer;
}

//ALL compartmentalized, just a controller
command_stream_t 
make_command_stream (int (*getbyte) (void *), void *arg)
{
	size_t content = 0;
	char* buffer = load_buffer(&content, getbyte, arg);
	token_stream_t* head = make_token_stream(buffer, content);
	if (head == NULL)
	{
		error(5, 0, "NO token");
		return NULL;
	}
	command_stream_t command_stream = make_command_forest(head);
	free(buffer);
	free_tokens(head);
	return command_stream;
}

// dequeue the first command on the stream
command_t 
read_command_stream (command_stream_t s)
{
	if (s == NULL || s->comm == NULL)
		return NULL;
	command_t result = s->comm;

	if (s->next != NULL)
	{
		command_stream_t next = s->next;
		s->comm = s->next->comm;
		s->next = s->next->next;

		free(next); 
	}
	else
		s->comm = NULL;

	return (result);
}


inline command_t peek (stack *s)
{
   return s->commands[s->num_items - 1];
}

inline command_t pop (stack *s)
{
   s->num_items--;
   return s->commands[s->num_items];
}

inline command_t push (stack *s, command_t cmd)
{
   s->commands[s->num_items] = cmd;
   s->num_items++;

   return cmd;
}

inline int size (stack *s)
{
   return s->num_items;
}

inline bool is_empty (stack *s)
{
   return (s->num_items == 0);
}
