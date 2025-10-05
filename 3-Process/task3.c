#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../CmnBase/cmnbase.h"

#define EXIT_VALUE 8

int main(int agrc, char* argv[]){
    pid_t child_id = fork();
    if(child_id > 0){
        //the PID of the child process is returned in the parent
        sleep(10);
    } else if(child_id == 0){
        //0 is returned in the child
        terminate(EXIT_FAILURE, false); //Use _exit to terminate child process
    } else {
        //-1 is returned in the parent, no child process is created, and errno is set appropriately.
        log_error("fork failed\n");
        terminate(EXIT_FAILURE, true); //Use exit to terminate parent process
    }

    return 0;
}