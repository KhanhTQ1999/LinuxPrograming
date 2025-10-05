#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../CmnBase/cmnbase.h"

#define EXIT_VALUE 20

int main(int agrc, char* argv[]){
    pid_t child_id = fork();
    if(child_id > 0){
        //On success, the PID of the child process is returned in the parent
        log_info(
            "This is parent process, parent pid = %d, child pid = %d. "
            "Wait child process..."
            , getpid(), child_id);

        int32_t status;
        waitpid(child_id, &status, 0);

        if( WIFEXITED(status)){
            log_info("Child process exit normally with exit status = %d", WEXITSTATUS(status));
        } else {
            log_error("Child process exit abnormally");
        }

    } else if(child_id == 0){
        //0 is returned in the child
        log_info("This is child process, pid = %d", getpid());
        terminate(EXIT_VALUE, false); //_exit to terminate child process
    } else {
        //-1 is returned in the parent, no child process is created, and errno is set appropriately.
        log_error("Fork error");
        terminate(EXIT_FAILURE, true); //exit to terminate parent process
    }
    
    return 0;
}