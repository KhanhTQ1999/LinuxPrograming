#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <sys/select.h>

#include "../CmnBase/cmnbase.h"

void handle_signal(int signal);
void handle_sigint();
void signal_cfg();
void multiplexing_cfg();

sigset_t emptyset;
sig_atomic_t got_term_sig = 0;
sig_atomic_t got_int_sig = 0;

// struct signalfd_siginfo {
//     uint32_t ssi_signo; /* Signal number */
//     int32_t ssi_errno; /* Error number (generally unused) */
//     int32_t ssi_code; /* Signal code */
//     uint32_t ssi_pid; /* Process ID of sending process */
//     uint32_t ssi_uid; /* Real user ID of sender */
//     int32_t ssi_fd; /* File descriptor (SIGPOLL/SIGIO) */
//     uint32_t ssi_tid; /* Kernel timer ID (POSIX timers) */
//     uint32_t ssi_band; /* Band event (SIGPOLL/SIGIO) */
//     uint32_t ssi_tid; /* (Kernel-internal) timer ID (POSIX timers) */
//     uint32_t ssi_overrun; /* Overrun count (POSIX timers) */
//     uint32_t ssi_trapno; /* Trap number */
//     int32_t ssi_status; /* Exit status or signal (SIGCHLD) */
//     int32_t ssi_int; /* Integer sent by sigqueue() */
//     uint64_t ssi_ptr; /* Pointer sent by sigqueue() */
//     uint64_t ssi_utime; /* User CPU time (SIGCHLD) */
//     uint64_t ssi_stime; /* System CPU time (SIGCHLD) */
//     uint64_t ssi_addr; /* Address that generated signal
//     (hardware-generated signals only) */
// };

int main(int argc, char* argv[]){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);

    // Block signal to use signalfd
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        log_error_ne("sigprocmask");
        terminate(EXIT_FAILURE, true);
    }

    // Create fd to receive signal
    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
        log_error_ne("signalfd");
        terminate(EXIT_FAILURE, true);
    }

    log_info("Program is running...");
    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); //stdin
        FD_SET(sfd, &readfds);          //signal

        int nfds = (STDIN_FILENO > sfd ? STDIN_FILENO : sfd) + 1;

        int ret = select(nfds, &readfds, NULL, NULL, NULL);
        if (ret == -1) {
            log_error_ne("select");
            terminate(EXIT_FAILURE, true);
        }

        if (FD_ISSET(sfd, &readfds)) {
            struct signalfd_siginfo fdsi;
            ssize_t s = read(sfd, &fdsi, sizeof(fdsi));
            if (s != sizeof(fdsi)) {
                log_error_ne("read");
                terminate(EXIT_FAILURE, true);
            }

            if (fdsi.ssi_signo == SIGINT) {
                log_info("SIGINT received.");
            } else if (fdsi.ssi_signo == SIGTERM) {
                log_info("SIGTERM received. Exit program");
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), stdin)) {
                printf("Your input: %s", buffer);
            }
        }
    }
    printf("Ending program.\n");
    close(sfd);
    return 0;
}