#ifndef DGRAM_IPV4_H
#define DGRAM_IPV4_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>

#include "../../CmnBase/cmnbase.h"

#define BUF_SIZE 10 /* Maximum size of messages exchanged between client and server */
#define PORT_NUM 50002 /* Server port number */

#endif /* DGRAM_IPV4_H */