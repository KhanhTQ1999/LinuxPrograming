#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "typedef.h"
#include "../CmnBase/cmnbase.h"

bool is_running = true;
mqd_t mqd;

static void handle_signal(int signal);

int main(int argc, char* argv[]) {
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

    ssize_t bytes_read;
    uint32_t prio;
    int32_t flags;
    struct mq_attr attr;

    // Open the message queue
    mqd = mq_open(QUEUE_NAME, O_RDWR | O_CREAT | O_NONBLOCK, 0644, NULL);
    if (mqd == (mqd_t)-1) {
        log_error_ne("mq_open");
        terminate(EXIT_FAILURE, true);
    }

    //Get the attributes of the message queue
    if (mq_getattr(mqd, &attr) == -1) {
        log_error_ne("mq_getattr");
        mq_close(mqd);
        terminate(EXIT_FAILURE, true);
    }

    //Loop to receive and process messages
    char* buffer = (char*)malloc(attr.mq_msgsize);
    if (buffer == NULL){
        log_error_ne("malloc");
        mq_close(mqd);
        terminate(EXIT_FAILURE, true);
    }

    while (is_running) {
        bytes_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
        if (bytes_read < 0) {
            if (errno == EAGAIN) {
                usleep(10000); //no message available, sleep for 10ms
                continue;
            }
            if (errno == EINTR) continue; // Interrupted by signal, retry
            log_error_ne("mq_receive");
            break;
        }   
        buffer[bytes_read] = '\0'; // Null-terminate the string
        printf("Processing task (priority: %d): %s\n", prio, buffer);
        // sleep(5); // Simulate processing time
    }

    free(buffer);
    mq_close(mqd);
    mq_unlink(QUEUE_NAME);
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
    log_info("SIGINT received, shutting down gracefully...");
    is_running = false;
    
    //After caught 2nd SIGINT signal, restore default action for SIGINT and re-raise the signal
    if(signal(SIGINT, SIG_DFL) == SIG_ERR){
        log_error_ne("signal failure");
        terminate(EXIT_FAILURE, true);
    }
}