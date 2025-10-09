// Reference link: https://gist.github.com/aspyct/3462238

#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>
#include <unistd.h>

#include "../CmnBase/cmnbase.h"

void handle_signal(int signal);
void handle_sigalrm();

// struct sigaction {
//     union {
//          void (*sa_handler)(int);
//          void (*sa_sigaction)(int, siginfo_t *, void *);
//     } __sigaction_handler;
//     sigset_t sa_mask; /* Signals blocked during handler
//     invocation */
//     int sa_flags; /* Flags controlling handler invocation */
//     void (*sa_restorer)(void); /* Not for application use */
// };

int seconds = 0;

int main(int argc, char* argv[]){
    struct sigaction sa;

    // Print pid, so that we can send signals from other shells
    log_info("Process %d is running...", getpid());

    // Setup the sighub handler
    sa.sa_handler = &handle_signal;

    // Restart the system call, if at all possible
    sa.sa_flags = SA_RESTART;

    // Block every signal during the handler
    sigfillset(&sa.sa_mask);

    // Intercept SIGHUP and SIGINT
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        log_error_ne("Cannot handle SIGALRM"); // Should not happen
    }

    // Schedule an alarm signal in 1 second
    alarm(1);
    while(seconds < 10){
        usleep(10000); // Sleep for 0.1 seconds
    }
    return 0;
}

void handle_signal(int signal) {
    if (signal == SIGALRM) {
        handle_sigalrm();
    } else {
        log_info("Received unexpected signal %d", signal);
    }
}

void handle_sigalrm() {
    log_info("Timer: %d seconds", ++seconds);
    // Schedule an alarm signal in 1 second
    alarm(1);
}