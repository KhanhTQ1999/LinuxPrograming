#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../CmnBase/cmnbase.h"

#define EXIT_VALUE 8
#define CMD_SIZE 64
#define COMMAND_ENV "MY_COMMAND"
#define COMMAND_DEFAULT "ls"
#define COMMAND_OVERRIDE 1

void set_cmd(char* cmd){
    if(setenv(COMMAND_ENV, cmd, COMMAND_OVERRIDE) != 0){
        log_error("Set environment variable failed");
    };
}

void get_cmd(char* cmd, size_t size){
    char* command = getenv(COMMAND_ENV);
    if(command != NULL){
        snprintf(cmd, size, "%s", command);
    } else {
       if(size > 0){
           cmd[0] = '\0';
       }
    }
}

void wait_process(pid_t pid){
    int32_t status;
    waitpid(pid, &status, 0);
    if(WIFEXITED(status)){
        log_info("Child process exit normally with exit status = %d", WEXITSTATUS(status));
    } else {
        log_error("Child process exit abnormally");
    }
}
int main(int agrc, char* argv[]){
    //Each process has an environment list, which is a set of environment variables that are maintained within the user-space memory of the process.
    //When a new process is created via fork(), it inherits a copy of its parent’s environment
    set_cmd(agrc > 1 ? argv[1] : COMMAND_DEFAULT);

    pid_t child_id = fork();
    if(child_id > 0){
        //On success, the PID of the child process is returned in the parent
        log_info("This is parent process, parent pid = %d, child pid = %d. ""Wait child process...", getpid(), child_id);
        wait_process(child_id);
    } else if(child_id == 0){
        //0 is returned in the child
        char command[CMD_SIZE];
        get_cmd(command, CMD_SIZE);
        if(command[0] == '\0'){
            log_error("No command to execute");
            terminate(EXIT_FAILURE, false); //Use _exit to terminate child process
        }

        log_info("This is child process, pid = %d, execute command: %s", getpid(), command);
        execlp(command, command, NULL);

        //If execlp return, it means it failed
        log_error("Execute command failed");
        terminate(EXIT_FAILURE, false); //Use _exit to terminate child process
    } else {
        //-1 is returned in the parent, no child process is created, and errno is set appropriately.
        log_error("fork failed\n");
        terminate(EXIT_FAILURE, true); //Use exit to terminate parent process
    }
    
    return 0;
}