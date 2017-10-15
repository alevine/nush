/**
 * ATTRIBUTION notice: all of this code is my own, however the concepts of enums and unions
 * were not known to me so I looked up examples on StackOverflow for help. Again, nothing
 * was copy pasted, and the structure was thought up by me, I'm just putting this here
 * in case (because Dr. Tuck makes this seem scary).
 */
#include "arrlist.h"

#ifndef CMDTYPES_H
#define CMDTYPES_H

struct exec_program;
struct mult_program;

typedef enum commandType {
	MULT,
	SINGLE
} commandType;

// enum for the type of command
typedef enum execType {
	REDIRECT_IN,    // 0
	REDIRECT_OUT,   // 1
	PIPE,			// 2
	BACKGROUND,		// 3
	SEMI,			// 4
	AND,			// 5
	OR,				// 6
	CD,				// 7
	EXIT,			// 8
	PROGRAM 		// 9
} execType;

// a command is one of: program to execute (be it built-in or not) or
// a program that has a left and right
typedef struct Command {
	commandType type;
	union {
		struct exec_program*   single;
		struct mult_program*   mult;
	};
} command;

// programs to execute have arguments
typedef struct exec_program {
	execType type;
	arrlist* args; // arrlist of tokens to use as arguments/program names
} exec_program;

// a multiple part program has left and right commands as well as an op type
typedef struct mult_program {
	execType type;
	command* left;
	command* right;
} mult_program;


command* 		make_command(arrlist* tokens);

exec_program*   make_exec(execType type, arrlist* args);

mult_program*   make_mult(execType type, command* left, command* right);

void 			free_command(command* cmd);

void			print_command(command* cmd);

#endif