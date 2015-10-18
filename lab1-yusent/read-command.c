
#include "alloc.h"
#include "command.h"



//ALL compartmentalized, just a controller
command_stream_t 
make_command_stream (int (*getbyte) (void *), void *arg)
{	
	element_stream* head;
	size_t content = 0;
	char* buffer = load_buffer(&content, getbyte, arg);
	
	if (!(head = parselement(buffer, content)))
	{ 	error(5, 0, "NO element"); 
		return NULL;}


	element_stream* present_stream = head;
	command_stream_t first_tree = NULL;
	command_stream_t present_tree = NULL;
	command_stream_t previous_tree = NULL;

	for (;present_stream->tree_head->next != NULL && present_stream != NULL;present_stream = present_stream->next )
	{
		element* present = present_stream->tree_head->next;	// head is ended with dummy
		command_t commend = plant (present);	// root
		present_tree = checked_malloc(sizeof(struct command_stream));
		present_tree->commend = commend;
		//
		if (!first_tree)
			previous_tree = first_tree = present_tree;
		else
			previous_tree = previous_tree->next = present_tree;
	}

	ElementFree(head);
	free(buffer);
	return first_tree;
}

// dequeue the first command on the stream
command_t 
read_command_stream (command_stream_t stream)
{
	command_stream_t next;
	command_t result = NULL;
	if (stream == NULL)
	{
		return result;
	}


	if( !(result = stream->commend))
	{
		return result;
	}

	if (stream->next)
	{
		next = stream->next;
		stream->commend = next->commend;
		stream->next = next->next;
		free(next); 
		return result;
	}
	else
	{
		stream->commend = NULL;
		return result;
	}	


}


//NEW functions


// convert input to element stream
element_stream* parselement (char* buffer, size_t buffer_size)
{
	element* first_element = build_element( NULL,2, -1);
	element* present_element = first_element;
	element_stream* top = checked_malloc(sizeof(element_stream));
	element_stream* present_stream = top;
	present_stream->tree_head = first_element;

	bool newline_to_colon = false;
	char c = buffer[0];
	element* previous_element = NULL;
	int line = 1;
	size_t i = 0;
	while (i < buffer_size)
	{
		if (c == ' ' || c == '\t')// consume whitespace
		{
			buffer++; 
			i++; 
			c = buffer[0];
		}
		else if (c == '\n')
		{
			line++;
			if (present_element->type == in_put ||	present_element->type == out_put)//cannot follow <>
			{
				goto parse_error;
			}
			else if (present_element->type == Subshell || present_element->type == Word)
			{// command completed, start new tree, start next element_stream only if presentent stream has been used
				
				if (present_element->type == First)
				{
					goto parse_error;
				}
				else
				{
					present_stream = present_stream->next = checked_malloc(sizeof(element_stream));
					present_element = present_stream->tree_head = build_element( NULL,2, -1);
				}
			}
			// present is operator needing a right token!!! consume all of the \n 

			buffer++; 
			i++; 
			c = buffer[0];
		}
		else if (is_word(c))
		{
			
			size_t word_size = 100;
			unsigned int count = 0;
			char* word = checked_malloc(word_size);

			next_word:;
			
			word[count] = c;
			count++;

			// epxand buffer
			if (count < word_size)
			{
				
				// next_word:;
				
				// word[count] = c;
				count;
				// count++;
			}
			else
			{
				word_size = word_size * 2;
				word = checked_grow_alloc(word, &word_size);
			}
			i++; 
			buffer++; 
			
			c = buffer[0];

			if (is_word(c) && i < buffer_size)
			{
				goto next_word;
			}
			// buil;d word element
			present_element->next = build_element( word,9, line);
			present_element = present_element->next;
		}
		else if (c == '>') // out
		{
			present_element->next = build_element(NULL,5, line);
			present_element = present_element->next;

			buffer++; 
			i++; 
			c = buffer[0];
		}
		else if (c == '<') // in
		{
			present_element->next = build_element( NULL,3, line);
			present_element = present_element->next;

			buffer++; 
			i++; 
			c = buffer[0];
		}
		else if (c == '|') // check | or ||
		{
			buffer++; 
			i++; 
			c = buffer[0]; // check next char

			if (c== '|') // OR
			{
				present_element->next = build_element( NULL,4, line);
				present_element = present_element->next;

				buffer++; 
				i++; 
				c = buffer[0];
			}
			else // PIPE
			{
				present_element->next = build_element( NULL,6, line);
				present_element = present_element->next;
			}
		}
		else if (c == '&') //&& & is invalid
		{
			buffer++; 
			i++; 
			c = buffer[0];

			if (c == '&')
			{
				present_element->next = build_element( NULL,1, line);
				present_element = present_element->next;
				buffer++; 
				i++; 
				c = buffer[0];
			}
			else 
			{
				goto parse_error;
			}
		}
		else if (c == ';')
		{
			present_element->next = build_element( NULL,7, line);
			present_element = present_element->next;

			buffer++; 
			i++; 
			c = buffer[0];
		}
		else if (c == '(') // case '(' merge all in () into a element
		{
			int layer = 1;
			int subshell_line = line;
			size_t count = 0;
			size_t subshell_size = 2;
			char* subshell = checked_malloc(subshell_size);

			// take all content in between () into the sub element
			while (layer > 0)
			{
				buffer++; 
				i++; 
				c = buffer[0];
				if (i == buffer_size)//hit end of buffer
				{
					line = subshell_line;
					goto parse_error;
				}

				switch(c) 
				{
					case '\n':
						// Aggresively consume \n  and ' ' to find the ')'
						for (;buffer[1] == '\n' || 
							buffer[1] == ' ' || 
							buffer[1] == '\t'  ;
							(i++ && buffer++))
						{
							buffer[1] == '\n'? line++: line;		
						}
						// convert to colon
						c = ';';
						newline_to_colon = true;
						line++;
					break;

					case '(':
						layer++;
					break;

					case ')':
						layer--;
						if (layer == 0) // all ( are paired
						{
							buffer++; 
							i++; 
							c = buffer[0]; // consume last close parens
							break;
						}
					break;

				}
				subshell[count] = c;



				count = count +1;
				if (!(count < subshell_size))
				{
					subshell_size *= 4;
					subshell = checked_grow_alloc (subshell, &subshell_size);
				}
			}


			
			present_element = present_element->next = build_element( subshell,8, subshell_line);
		}
		//else if (c == ')')
		//{
		//	goto parse_error;
		//}
		else // INVALID character and Exit for error situations
		{
			parse_error:;
			error(1, 0, "%d: Parsing Error", line);
			return NULL;
		}
	}

	return top;
}

// element to single command tree
command_t plant (element* head_atom)
{
	if(!head_atom)
	{
		return NULL;
	}

	element* present_atom = head_atom;
	int line = present_atom->line; 
	queue* operator = checked_malloc(sizeof(queue));	
	operator->num = 0;
	queue* token = checked_malloc(sizeof(queue)); 
	command_t present_commend = NULL;
	command_t previous_commend = NULL;
	token->num = 0;


	do
	{
		switch (present_atom->type)
		{

			case out_put:
				if (previous_commend == NULL ||
					(previous_commend->type != SIMPLE_COMMAND && previous_commend->type != SUBSHELL_COMMAND) ||
					previous_commend->output != NULL)
				{
					
					goto inout_error;
				}

				present_atom = present_atom->next;

				if (present_atom->type == Word) // followed by a word
				{
					previous_commend->output = present_atom->content;
				}
				else
				{
					goto inout_error;
				}
				break;	

			case in_put:
				//previous must be subshell or word
				if (previous_commend == NULL || 
					(previous_commend->type != SIMPLE_COMMAND && previous_commend->type != SUBSHELL_COMMAND) ||
					previous_commend->input != NULL)
				{
					goto inout_error;
				}

				present_atom = present_atom->next;

				if (present_atom->type == Word) // followed by a word
				{
					previous_commend->input = present_atom->content;
				}
				else
				{
					goto inout_error;
				}

				// just the attri of previous command, dont push token
				if(false)
				{
					inout_error:
					error(1, 0, "%d: Build Command Tree Error, inout", line);
					return NULL;
				}

				break;

			case Word:
				
				{
					size_t num_words = 1;
					present_commend = build_command (SIMPLE_COMMAND);
					unsigned int i = 1;
					
					element* ct = present_atom;
					for(;ct->next != NULL && ct->next->type == Word;num_words++)
					{
						
						ct = ct->next;
					}

					// load word
					present_commend->u.word = checked_malloc( (sizeof(char*)) * (num_words+1) );
					present_commend->u.word[0] = present_atom->content;
					while (i < num_words)
					{
						present_atom = present_atom->next;
						present_commend->u.word[i] = present_atom->content;
						i++;
					}
					present_commend->u.word[num_words] = NULL;

					push(token, present_commend);
				}
				break;
			

			case And: //ALL the OPS
			case Or:
			case Pipe:
			case Sequence:

				switch( present_atom->type )
				{
					case And:
						present_commend = build_command (AND_COMMAND);
						break;
					case Or:
						present_commend = build_command (OR_COMMAND);
						break;
					case Pipe:
						present_commend = build_command (PIPE_COMMAND);
						break;
					case Sequence:
						present_commend = build_command (SEQUENCE_COMMAND);
						break;
					default:
						break;
				}

				if (!(operator->num == 0) && (noless_prio(operator->commands[operator->num - 1], present_commend)))
				{
					bool detect = combine(operator, token);
					if (!detect)
						goto finish_error;
				}
				push(operator, present_commend);
				break;


			case Subshell:

				present_commend = build_command (SUBSHELL_COMMAND);
				present_commend->u.subshell_command = plant(
					parselement(present_atom->content, strlen(present_atom->content))->tree_head);

				push(token, present_commend);
				break;
			default:
				break;
		};

		previous_commend = present_commend; // save previouscommend for check
	} while(present_atom != NULL && (present_atom = present_atom->next) != NULL);


	while(operator->num > 0)//pop all operator
	{
		if (!combine(operator, token))
		{
			goto finish_error;
		}
	}

	if (token->num != 1)
	{
		finish_error:;
		error(1, 0, "%d: Build Tree Errer, ending queue abnormal", line);
		return NULL;
	}

	command_t root = pop(token); // get the last token
	free(operator); 
	free(token);

	return root;
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

// Build element with: type, pointer of content string
element* build_element ( char* content, int type, int line)
{
	element* atom = checked_malloc(sizeof(element));
	atom->next = atom->prev = NULL;
	atom->line = line;
	switch (type)
	{
		case 1:
			atom->type = And;
		break;
		case 2:
			atom->type = First;
		break;
		case 3:
			atom->type = in_put;
		break;
		case 4:
			atom->type = Or;
		break;
		case 5:
			atom->type = out_put;
		break;
		case 6:
			atom->type = Pipe;
		break;
		case 7:
			atom->type = Sequence;
		break;
		case 8:
			atom->type = Subshell;
		break;
		case 9:
			atom->type = Word;
		break;
		default:
		break;
	}
	atom->content = content;
	return atom;
}



inline command_t pop (queue *top)
{
   --(top->num);
   return top->commands[top->num];
}

inline command_t push (queue *top, command_t commend)
{
   top->commands[top->num] = commend;
   ++(top->num);
   return commend;
}


command_t build_command (command_type type)
{
   command_t result = (command_t)checked_malloc(sizeof( struct command ));
   result->type = type;
   result->status= -1;
   result->input = NULL;
   result->output = NULL;
   return result;
}

bool is_word (char c)
{
	if ( strchr("1234567890", c) != NULL ||strchr("!%+,-./:@^_", c) != NULL || isalnum(c))
		return true;
	else
		return false;
}


// deallocate an entire command tree
void CommandFree (command_t commend)
{
	int i;
	switch(commend->type)
	{
		case PIPE_COMMAND:
		case AND_COMMAND:
		case OR_COMMAND:
		case SEQUENCE_COMMAND:
			if(commend->u.command != NULL)
			{
				if(commend->u.command[1] != NULL)
				{
					CommandFree(commend->u.command[1]);
					free(commend->u.command[1]);
				}
				if(commend->u.command[1] != NULL)
				{	
					CommandFree(commend->u.command[0]); 
					free(commend->u.command[0]);
				}

			}
			break;

		case SUBSHELL_COMMAND:
			if(commend->input!= NULL)
			{
				free(commend->input); 
			}
			if(commend->output != NULL)
			{
				free(commend->output);
			}
			if(commend->u.subshell_command != NULL)
			CommandFree(commend->u.subshell_command);
			free(commend->u.subshell_command);
			break;

		case SIMPLE_COMMAND:
			free(commend->input); 
			free(commend->output);
			for(i = 1;commend->u.word[i];i++)
			{
				free(commend->u.word[i]);
			}
			break;
		default:
			error(1, 0, "Invalid command_Type to free.");
			break;
	}
	return ;
}


// Deallocates element stream, element->content is used for read_command, not freed
void ElementFree (element_stream* first)
{
	element_stream* present_stream = first;
	element_stream* previous_stream;

	for (;present_stream != NULL;free(previous_stream))
	{
		element* present = present_stream->tree_head;
		element* previous;

		for (;present != NULL;free(previous))
		{
			previous = present;
			present = present->next;
		}
		previous_stream = present_stream;
		present_stream = present_stream->next;
		
	}
	return;
}


// combine then push into token queue
bool combine (queue* operator, queue* token)
{
	if (token->num >= 2)// no enough token found
	{
		command_t right_operand = pop(token);
		command_t new_commend = pop(operator);	
		command_t left_operand = pop(token);
		if(right_operand != NULL)
		{
			new_commend->u.command[1] = right_operand;
		}
		if(left_operand != NULL)
		{
			new_commend->u.command[0] = left_operand;
		}
		push(token, new_commend);
		return true;
	}
	else
	{
				return false;
	}
}

bool noless_prio(command_t left,command_t right)
{
	int leftv = 0;//impled ; has 0 priority
   int rightv = 0;
   switch (left->type)
   {
      case AND_COMMAND: 
      case OR_COMMAND:
         leftv= 2;
         break;
      case PIPE_COMMAND:
         leftv = 3;
         break;
      case SEQUENCE_COMMAND:
         leftv = 1;
         break;
      default:
      break;
   }
   switch (right->type)
   {
      case AND_COMMAND: 
      case OR_COMMAND:
         rightv= 2;
         break;
      case PIPE_COMMAND:
         rightv = 3;
         break;
      case SEQUENCE_COMMAND:
         rightv = 1;
         break;
      default:
      	break;
   }
   return (leftv >= rightv);
}