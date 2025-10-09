#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#include "../CmnBase/cmnbase.h"

#define EXIT_VALUE 8

void handle_signal(int signal);
void handle_usr1();
void parent_entry();
void child_entry();
    
uint32_t count = 0;

int main(int agrc, char* argv[]){
    pid_t child_id = fork();

    if(child_id > 0){
        //On success, the PID of the child process is returned in the parent
        log_info(
            "This is parent process, parent pid = %d, child pid = %d. "
            "Wait child process..."
            , getpid(), child_id);

        parent_entry(child_id);
    } else if(child_id == 0){
        //0 is returned in the child
        log_info("This is child process, pid = %d", getpid());
        child_entry();
    } else {
        //-1 is returned in the parent, no child process is created, and errno is set appropriately.
        log_error_ne("Fork failure");
        return -1;
    }
    
    return 0;
}

void parent_entry(pid_t child){
    int32_t status;

    //Send SIGUSR1 to child process every 2 seconds, 5 times
    while(count < 5){
        sleep(2);
        log_info("Parent send the SIGUSR1... %d", ++count);
        kill(child, SIGUSR1); // Send SIGUSR1 to child process
    }

    // After doing something else, wait for child process to exit
    waitpid(child, &status, 0);
    if(WIFEXITED(status)){
        log_info("Child process exit normally with exit status = %d", WEXITSTATUS(status));
    } else {
        log_info("Child process exit abnormally");
    }
}

void child_entry(){
    //0 is returned in the child
    struct sigaction sa;

    // Setup the sighub handler
    sa.sa_handler = &handle_signal;

    // Restart the system call, if at all possible
    sa.sa_flags = SA_RESTART;

    // Block every signal during the handler
    sigfillset(&sa.sa_mask);

    // Intercept SIGUSR1
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        log_error_ne("Cannot handle SIGUSR1"); // Should not happen
    }

    while(count < 5){
        // Sleep until a signal is received
        pause(); 
    }

    exit(EXIT_VALUE);
}

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        handle_usr1();
    } else {
        log_info("Received unexpected signal %d\n", signal);
    }
}

void handle_usr1() {
    log_info("Received signal from parent %d times\n", ++count);
}