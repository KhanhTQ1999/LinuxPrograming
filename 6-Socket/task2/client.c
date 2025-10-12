#include "dgram_ipv4.h"

int main(int argc, char* argv[]){
    struct sockaddr_in svaddr;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];
    if (argc < 3 || strcmp(argv[1], "--help") == 0){
        log_error("Usage: %s host-address msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    sfd = socket(AF_INET, SOCK_DGRAM, 0); /* Create client socket */
    if (sfd == -1){
        log_error_ne("socket");
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(PORT_NUM);
    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0){
        log_error("inet_pton failed for address '%s'", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Send messages to server; echo responses on stdout */
    for (j = 2; j < argc; j++) {
        msgLen = strlen(argv[j]);
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(svaddr)) != msgLen){
            log_error_ne("sendto");
            exit(EXIT_FAILURE);
        }
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (numBytes == -1){
            log_error_ne("recvfrom");
            exit(EXIT_FAILURE);
        }
        log_info("Response %d: %.*s\n", j - 1, (int) numBytes, resp);
    }
    return 0;
}