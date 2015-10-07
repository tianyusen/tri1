#include "command.h"
//#include "command-internals.h"
#include "alloc.h"
//#include <error.h>

bool is_op(char* c, int i) // DO THIS EASY and BASIC ONE FIRST
{
	if (c[i] == ';' ||
		c[i] == '|' ||
		(c[i] == '&' && c[i + 1] == '&'))
	{
		return true;
	}
	else
	{
		return false;
	}

}
operator_type top_operator(operator_node_t op_stack_top) //just read out the type
{
	return op_stack_top->content;
}
bool is_empty_op(operator_node_t top)
{
	return (top == NULL ? true : false);
}
int precedence(operator_type type)
{
	switch (type)
	{
	case PIPE_OP: return 3;
	case AND_OP:
	case OR_OP: return 2;
	case SEQUENCE_OP: return 1;
	default: return 4; //parent
	}
}


void push_operator(operator_node_t* op_stack_top, operator_type type)
{
	size_t size = sizeof(struct operator_node);
	operator_node_t new_op_t = (operator_node_t)checked_malloc(size);
	new_op_t->content = type;
	new_op_t->next = NULL;
	if (*op_stack_top == NULL)
	{
		new_op_t->prev = NULL;
		*op_stack_top = new_op_t;
	}
	else// stack is not empty
	{
		new_op_t->prev = *op_stack_top;
		(*op_stack_top)->next = new_op_t;
		*op_stack_top = new_op_t;
	}
}

operator_type pop_operator(operator_node_t* op_top)
{
	operator_type result = (*op_top)->content;
	operator_node_t temp = (*op_top)->prev;
	free(*op_top);
	*op_top = temp;
	return result;
}
command_t combine_command(command_t* first_command, command_t* second_command, operator_type last_op)
{
	command_type type;
	switch (last_op)
	{
	case AND_OP: 
		type = AND_COMMAND;
		break;
	case SEQUENCE_OP: type = SEQUENCE_COMMAND;
		break;
	case OR_OP:	type = OR_COMMAND;
		break;
	case PIPE_OP: type = PIPE_COMMAND;
	default:
		fprintf(stderr, "%d: Parsing Error, unfit operator positioning", (*first_command)->line);
		break;
	}
	command_t new_command = build_command(type, &((*first_command)->line));
	new_command->u.command[0] = *first_command;
	new_command->u.command[1] = *second_command;
	return new_command;
}
operator_node_t build_operator(char* buffer, int* i)
{
	if (!is_op(buffer, *i))
	{
		perror( "Logic Error 114" );
		abort();
	}
	size_t size = sizeof(struct operator_node);
	operator_node_t new_op = (operator_node_t)checked_malloc(size);
	new_op->next = NULL;
	new_op->prev = NULL;
	switch (buffer[(*i)])
	{
	case ';':
		new_op->content = SEQUENCE_OP;
		break;
	case '|':
		if (buffer[(*i) + 1] == '|')
		{
			(*i)++;
			new_op->content = OR_OP;
		}
		else
		{
			new_op->content = PIPE_OP;
		}
		break;
	case '&':
		(*i)++;
		new_op->content = AND_OP;
		break;
	}
	return new_op;
}
void free_op(operator_node_t op_top)
{
	while (op_top != NULL)
	{
		pop_operator(&op_top);
	}
}

