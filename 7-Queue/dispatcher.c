#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "typedef.h"
#include "../CmnBase/cmnbase.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        log_error("Usage: %s <priority> <task description>", argv[0]);
        terminate(EXIT_FAILURE, false);
    }

    uint32_t prio = str_to_int(argv[1], strlen(argv[1]));
    if(prio == INT32_MAX){
        log_error("Invalid priority value");
        terminate(EXIT_FAILURE, false);
    }

    mqd_t mqd = mq_open(QUEUE_NAME, O_WRONLY);
    if (mqd == (mqd_t)-1) {
        log_error_ne("mq_open");
        return 1;
    }

    if (mq_send(mqd, argv[2], strlen(argv[2]), prio) == -1) {
        log_error_ne("mq_send");
        mq_close(mqd);
        return 1;
    }

    mq_close(mqd);
    return 0;
}
