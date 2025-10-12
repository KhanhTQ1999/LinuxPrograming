#include "dgram_ipv4.h"

int main(int argc, char* argv[]){
    struct sockaddr_in svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddrStr[INET_ADDRSTRLEN];
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1){
        log_error_ne("socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svaddr.sin_port = htons(PORT_NUM);
    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1){
        log_error_ne("bind");
        exit(EXIT_FAILURE);
    }
    /* Receive messages, convert to uppercase, and return to client */
    for (;;) {
        len = sizeof(struct sockaddr_in6);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);

        if (numBytes == -1){
            log_error_ne("recvfrom");
            exit(EXIT_FAILURE);
        }

        if (inet_ntop(AF_INET, &claddr.sin_addr, claddrStr, INET_ADDRSTRLEN) == NULL)
            log_info("Couldn't convert client address to string\n");
        else
            log_info("Server received %ld bytes from (%s, %u)\n", (long) numBytes, claddrStr, ntohs(claddr.sin_port));
        for (j = 0; j < numBytes; j++)
            buf[j] = toupper((unsigned char) buf[j]);
        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes){
            log_error_ne("sendto");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}