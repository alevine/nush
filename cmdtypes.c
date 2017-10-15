#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "arrlist.h"
#include "cmdtypes.h"

void make_command_help(command* cmd, arrlist* tokens, int index, execType type);

command* make_command(arrlist* tokens) {
	command* cmd = malloc(sizeof(command));

	int pipe;
	int semi;
	int and;
	int or;
	int back;
	int in;
	int out;

	pipe = arrlist_contains(tokens, "|");

	// case of pipe operator, make mult 
	if (pipe != -1) {
		make_command_help(cmd, tokens, pipe, PIPE);
		return cmd;
	}

	
	semi = arrlist_contains(tokens, ";");

	// case of ;
	if (semi != -1) {
		make_command_help(cmd, tokens, semi, SEMI);
		return cmd;
	}

	and = arrlist_contains(tokens, "&&");

	// case of &&
	if (and != -1) {
		make_command_help(cmd, tokens, and, AND);
		return cmd;
	}

	or = arrlist_contains(tokens, "||");

	// case of ||
	if (or != -1) {
		make_command_help(cmd, tokens, or, OR);
		return cmd;
	}

	back = arrlist_contains(tokens, "&");

	// case of &
	if (back != -1) {
		if (back == tokens->size) {
			make_command_help(cmd, tokens, back, BACKGROUND);
		}
		else {
			arrlist* back_args = arrlist_copy(tokens, 0, back);
			cmd->type = SINGLE;
			cmd->single = make_exec(BACKGROUND, back_args);
		}
		return cmd;
	}

	in = arrlist_contains(tokens, "<");

	// case of >
	if (in != -1) {
		make_command_help(cmd, tokens, in, REDIRECT_IN);
		return cmd;
	}

	out = arrlist_contains(tokens, ">");

	// case of <
	if (out != -1) {
		make_command_help(cmd, tokens, out, REDIRECT_OUT);
		return cmd;
	}
	// finally, just a single program to run
	else {
		cmd->type = SINGLE;

		char* program = tokens->data[0];

		execType type;

		if (strcmp(program, "cd") == 0) {
			type = CD;
		}
		else if (strcmp(program, "exit") == 0) {
			type = EXIT;
		}
		else {
			type = PROGRAM;
		}

		cmd->single = make_exec(type, tokens);

		return cmd;
	}
}

void make_command_help(command* cmd, arrlist* tokens, int index, execType type) {
	cmd->type = MULT;
	arrlist* left  = arrlist_copy(tokens, 0, index);
	arrlist* right = arrlist_copy(tokens, index + 1, tokens->size);
	cmd->mult = make_mult(type, make_command(left), make_command(right));
	free_arrlist(tokens);
}


exec_program* make_exec(execType type, arrlist* args) {
	exec_program* exec = malloc(sizeof(exec_program));

	// initialize single exec program
	exec->args = args;
	exec->type = type;

	return exec;
}

mult_program* make_mult(execType type, command* left, command* right) {
	mult_program* mult = malloc(sizeof(mult_program));

	// initialize multiple part program
	mult->left  = left;
	mult->right = right;
	mult->type  = type;

	return mult;
}

// for an exec program, free the args and then free the program itself
void free_exec(exec_program* exec) {
	free_arrlist(exec->args);
	free(exec);
}

// for a mult program, free the left and right then free the program itself
void free_mult(mult_program* mult) {
	free_command(mult->left);
	free_command(mult->right);
	free(mult);
}

// to free a command, free either its mult or single
void free_command(command* cmd) {
	switch(cmd->type) {
		case MULT:
			free_mult(cmd->mult);
			break;
		case SINGLE:
			free_exec(cmd->single);
			break;
	}
	free(cmd);
}

// print a command (for debugging)
void print_command(command* cmd) {
	printf("Command is of type: %d\n", cmd->type);

	switch(cmd->type) {
		case MULT:
			printf("Mult is of type: %d\n", cmd->mult->type);
			printf("In left part of cmd.\n");
			print_command(cmd->mult->left);
			printf("In right part of cmd.\n");
			print_command(cmd->mult->right);
			break;
		case SINGLE:
			printf("Single of of type: %d\n", cmd->single->type);
			printf("Single args as follows: \n");
			arrlist_print(cmd->single->args);
			break;

	}
}
