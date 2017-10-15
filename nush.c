#include "tokenize.h"
#include "cmdtypes.h"
// ATTRIBUTION: compiler was giving me warnings about using kill, 
// added this line I found on stackoverflow.
#define _POSIX_SOURCE
// END ATTRIBUTION
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void execute_command(command* cmd);
void execute_mult(execType type, mult_program* cmd);
void execute_single(exec_program* cmd);

// executes a single cd command
void execute_cd(exec_program* cmd) {
    char path[256];

    strcpy(path, arrlist_get(cmd->args, 1)); // copy path into buffer

    chdir(path); // change dir
}

// exits the shell
void execute_exit(exec_program* cmd) {
    _exit(0); // pretty sure this is all I need haha
}

// executes a single shell program, with arguments
// if background = 1, runs it in the background instead of foreground
void execute_program(exec_program* cmd, int background) {
    int cpid;

    arrlist* args = cmd->args;

    if ((cpid = fork())) {
        int status;

        if (!background) {
            waitpid(cpid, &status, 0);
        } else {
            signal(SIGCHLD, SIG_IGN);
        }

    } else {
        if (background) {
            setpgid(0, 0);
        }

        execvp(arrlist_get(args, 0), args->data);
    }
}

// executes a single and/or statement
// if and_p = 1 then it is assumed to be an and block, or otherwise
void exec_and_or(mult_program* cmd, int and_p) {
    exec_program* left = cmd->left->single;

    int cpid;

    if ((cpid = fork())) {
        int status;

        waitpid(cpid, &status, 0);

        if (WIFEXITED(status)) {
            if (and_p == 1) {
                if (status == 0) {
                    exec_program* right = cmd->right->single;
                    execute_single(right);
                }
            } else {
                if (status != 0) {
                    exec_program* right = cmd->right->single;
                    execute_single(right);
                }
            }
        }

    } else {
        if (left->args->data != NULL) {
            execvp(arrlist_get(left->args, 0), left->args->data);
        }
    }
}

// executes a single program
void execute_single(exec_program* cmd) {
    switch(cmd->type) {
        case CD:
            execute_cd(cmd);
            break;
        case EXIT:
            execute_exit(cmd);
            break;
        case BACKGROUND:
            execute_program(cmd, 1);
            break;
        case PROGRAM:
            execute_program(cmd, 0);
            break;
        default:
            // here be demons
            break;
    }
}

// cmd->left is either a mult or single program to run
// cmd->right is a single that is the input for left
void execute_redir_in(mult_program* cmd) {
    exec_program* right = cmd->right->single;
    command* left = cmd->left;

    char* to_read = arrlist_get(right->args, 0);

    int cpid;

    if ((cpid = fork())) {
        int status;

        waitpid(cpid, &status, 0);
    } else {
        int read_file = open(to_read, O_RDONLY);

        int save_stdin = dup(STDIN_FILENO); // save temp stdout

        fflush(stdin);

        dup2(read_file, STDIN_FILENO); // overwrite stdout with the write file

        close(read_file);

        execute_command(left);

        kill(getpid(), SIGTERM); // kill the process, don't need it after forking 

        dup2(save_stdin, STDIN_FILENO);

        close(save_stdin);
    }
}

// cmd->left is either a mult or single program to run
// cmd->right is a single that is the file to write to
void execute_redir_out(mult_program* cmd) {
    exec_program* right = cmd->right->single;
    command* left = cmd->left;

    char* to_write = arrlist_get(right->args, 0);

    int cpid;

    if ((cpid = fork())) {
        int status;

        waitpid(cpid, &status, 0);
    } else {
        int write_file = open(to_write, O_CREAT | O_TRUNC | O_WRONLY, 0666);

        int save_stdout = dup(STDOUT_FILENO); // save temp stdout

        fflush(stdout);

        dup2(write_file, STDOUT_FILENO); // overwrite stdout with the write file

        close(write_file);

        execute_command(left);

        kill(getpid(), SIGTERM); // kill the process, don't need it after forking 

        dup2(save_stdout, STDOUT_FILENO);

        close(save_stdout);
    }

}

// handles a pipe command, where the left side is a single command and the right
// side can be either single or mult
void execute_pipe(mult_program* cmd) {
    command* right = cmd->right;
    exec_program* left = cmd->left->single;

    int pipe_fds[2];

    pipe(pipe_fds); // create a pipe

    int pipe_r = pipe_fds[0];

    int pipe_w = pipe_fds[1];

    int cpid;

    if ((cpid = fork())) {  
        // parent, right process to read from pipe
        int status;
        waitpid(cpid, &status, 0);

        close(0); // close stdout
        close(pipe_w); // don't need write

        dup2(pipe_r, 0); // put pipe_r in stdin

        close(pipe_r); // close original fd slot

        execute_command(right);

    } else {
        // child, left process to write to pipe
        
        close(1); // close stdout
        close(pipe_r); // don't need read

        dup2(pipe_w, 1); // put pipe_w in stdout

        close(pipe_w); // close original fd slot

        arrlist* args = left->args;

        execvp(arrlist_get(args, 0), args->data); // execute the left
    }

}

// semicolon is simple: execute the left, then execute the right
void execute_semi(mult_program* cmd) {
    execute_command(cmd->left);
    execute_command(cmd->right);
}

// execute a multiple part command
void execute_mult(execType type, mult_program* cmd) {
    switch(type) {
        case REDIRECT_IN:
            execute_redir_in(cmd);
            break;
        case REDIRECT_OUT:
            execute_redir_out(cmd);
            break;  
        case PIPE:
            execute_pipe(cmd);
            break;
        case SEMI:
            execute_semi(cmd);
            break;
        case AND:
            exec_and_or(cmd, 1);
            break;
        case OR:
            exec_and_or(cmd, 0);
            break;
        default:
            break;
    }
}

// execute a command
void execute_command(command* cmd) {
    switch(cmd->type) {
        case SINGLE:
            execute_single(cmd->single);
            break;
        case MULT:
            execute_mult(cmd->mult->type, cmd->mult);
            break;
    }
}

int main(int argc, char* argv[]) {
    char cmd[256];

    if (argc == 1) {
        while(1) {
            printf("nush$ ");

            char* read_line = fgets(cmd, 256, stdin);

            if (!read_line) {
                break;
            }

            arrlist* tokens = tokenize(read_line);

            command* command = make_command(tokens);

            execute_command(command);
            
            free_command(command);
        }
    }
    else {
        char* file_path = argv[1];

        FILE* read_file = fopen(file_path, "r");

        if (read_file == NULL) {
            printf("File opening failed.\n");
        }

        while (fgets(cmd, 256, read_file) != NULL) {
            arrlist* tokens = tokenize(cmd);

            command* file_command = make_command(tokens);

            execute_command(file_command);

            free_command(file_command);
        }

        fclose(read_file);
    }

    return 0;
}
