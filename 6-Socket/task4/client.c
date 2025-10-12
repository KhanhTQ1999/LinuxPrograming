#include "dgram_unix.h"
#include "../CmnBase/cmnbase.h"

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];
    if (argc < 2 || strcmp(argv[1], "--help") == 0){
        log_error("Usage: %s msg...", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Create client socket; bind to unique pathname (based on PID) */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1){
        log_error_ne("socket");
        exit(EXIT_FAILURE);
    }
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long) getpid());
    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1){
        log_error_ne("bind");
        exit(EXIT_FAILURE);
    }
    /* Construct address of server */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);
    /* Send messages to server; echo responses on stdout */
    for (j = 1; j < argc; j++) {
        msgLen = strlen(argv[j]); /* May be longer than BUF_SIZE */
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen){
            log_error_ne("sendto");
            exit(EXIT_FAILURE);
        }
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (numBytes == -1){
            log_error_ne("recvfrom");
            exit(EXIT_FAILURE);
        }
        log_info("Response %d: %.*s", j, (int) numBytes, resp);
    }
    remove(claddr.sun_path); /* Remove client socket pathname */
    exit(EXIT_SUCCESS);
}