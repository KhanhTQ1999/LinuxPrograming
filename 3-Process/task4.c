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
        terminate(EXIT_FAILURE, true); //Use exit to terminate parent process
    } else if(child_id == 0){
        //0 is returned in the child
        sleep(10);
        for(int cnt = 0; cnt < 10; ++cnt){
            printf("Parent process ID: %d\n", getppid());
            sleep(1);
        }
    }else{
        //-1 is returned in the parent, no child process is created, and errno is set appropriately.
        log_error("fork failed\n");
        terminate(EXIT_FAILURE, true); //Use exit to terminate parent process
    }

    return 0;
}