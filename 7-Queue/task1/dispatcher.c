#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "typedef.h"
#include "../../CmnBase/cmnbase.h"

int main(int argc, char *argv[]) {
    if (argc != 3 || argv[2][0] < '0' || argv[2][0] > '9') {
        log_error("Usage: %s <priority> <task description>", argv[0]);
        return 1;
    }

    uint32_t prio = argv[2] - '0';
    char *task_desc = argv[2];
    if (strlen(task_desc) >= MSG_SIZE) {
        log_error("Task description too long");
        return 1;
    }

    mqd_t mqd = mq_open(QUEUE_NAME, O_WRONLY);
    if (mqd == (mqd_t)-1) {
        log_error_ne("mq_open");
        return 1;
    }

    if (mq_send(mqd, task_desc, strlen(task_desc), prio) == -1) {
        log_error_ne("mq_send");
        mq_close(mqd);
        return 1;
    }

    mq_close(mqd);
    return 0;
}
