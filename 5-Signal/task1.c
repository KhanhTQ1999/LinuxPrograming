#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#include "../CmnBase/cmnbase.h"

static void sigint_handler();
static void handle_signal(int signal);

uint32_t signal_count = 0;

int main(int argc, char* argv[]){
    struct sigaction sa = {
        .sa_handler = handle_signal, // Setup the sighub handler
        .sa_flags = SA_RESTART // Restart the system call, if at all possible
    };

    // Block every signal during the handler
    sigfillset(&sa.sa_mask);

    //Register the SIGINT handler
    if(sigaction(SIGINT, &sa, NULL) == -1){
        log_error_ne("Error: cannot handle SIGINT"); // Should not happen
        terminate(EXIT_FAILURE, true);
    }

    for (;;) {
        log_info("Program processing...");
        sleep(3);
    }

    return 0;
}

void handle_signal(int sig_num){
    if(sig_num == SIGINT){
        sigint_handler();
    }else{
        log_warn("Unexpected signal %d", sig_num);
    }
}

void sigint_handler(){
    signal_count++;
    log_info("SIGINT received %d", signal_count);
    if(signal_count < 2){
        return;
    }
    //After caught 2nd SIGINT signal, restore default action for SIGINT and re-raise the signal
    if(signal(SIGINT, SIG_DFL) == SIG_ERR){
        log_error_ne("signal failure");
        terminate(EXIT_FAILURE, true);
    }
}
