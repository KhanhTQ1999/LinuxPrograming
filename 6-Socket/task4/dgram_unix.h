#ifndef DGRAM_UNIX_H
#define DGRAM_UNIX_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>

#define BUF_SIZE 10 /* Maximum size of messages exchanged between client to server */
#define SV_SOCK_PATH "/tmp/ud_ucase" /* Server's socket pathname */

#endif // DGRAM_UNIX_H
