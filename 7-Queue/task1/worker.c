#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "typedef.h"
#include "../../CmnBase/cmnbase.h"

int main(int argc, char* argv[]) {
    int32_t flags, bytes_read;
    uint32_t prio;
    mqd_t mqd;
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = MSG_SIZE;
    flags = O_CREAT | O_RDONLY;

    // Open the message queue
    mqd = mq_open("/task_queue", flags, NULL, NULL);
    if (mqd == (mqd_t)-1) {
        log_error_ne("mq_open");
        return 1;
    }

    // Get the attributes of the message queue
    if (mq_getattr(mqd, &attr) == -1) {
        log_error_ne("mq_getattr");
        mq_close(mqd);
        return 1;
    }

    // Continuously receive messages
    char buffer[attr.mq_msgsize + 1]; // +1 for null terminator
    while (1) {
        bytes_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
        if (bytes_read < 0) {
            log_error_ne("mq_receive");
            break;
        }

        buffer[bytes_read] = '\0'; // Null-terminate the string
        printf("Processing task (priority: %d): %s\n", prio, buffer);
        sleep(1); // Simulate processing time
    }

    mq_close(mqd);
    return 0;
}
