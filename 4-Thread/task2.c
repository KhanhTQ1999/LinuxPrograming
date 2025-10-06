#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#include "../CmnBase/cmnbase.h"

#define NUM_THREADS 3

typedef struct {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int thread_num;       /* Application-defined thread # */
} thread_info;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;       /* Mutex for synchronizing access */
uint32_t shared_counter = 0; // Shared resource

static void* entry(void* arg){
    thread_info* tinfo = (thread_info*)arg;
    log_info("Thread %d is running... with tid %ld", tinfo->thread_num, tinfo->thread_id);

    for(uint32_t i = 0; i < 1000000; ++i){
        pthread_mutex_lock(&lock);
        shared_counter++; // Safely increment the shared counter
        pthread_mutex_unlock(&lock);
    }

    log_info("Thread %d finished execution, share_counter: %d", tinfo->thread_num, shared_counter);
    return NULL;
}

uint32_t create_threads(thread_info* tinfo, size_t num)
{
    int32_t ret;
    for(uint32_t tnum = 0; tnum < num; ++tnum){
        tinfo[tnum].thread_num = tnum;
        ret = pthread_create(&tinfo[tnum].thread_id, NULL, entry, &tinfo[tnum]);
        if(ret != 0){
            errno = ret;
            log_error_ne("pthread_create failure");
            return FAILURE;
        }
    }
    return SUCCESS;
}

uint32_t join_threads(thread_info* tinfo, size_t num){
    int32_t ret;
    void* tret;
    for(uint32_t tnum = 0; tnum < num; ++tnum){
        ret = pthread_join(tinfo[tnum].thread_id, &tret);
        if(ret != 0){
            errno = ret;
            log_error_ne("pthread_join failure");
            return FAILURE;
        }
    }
    return SUCCESS;
}

uint32_t create_mutex(pthread_mutex_t* mutex){
    //nothing to do, mutex is initialized with PTHREAD_MUTEX_INITIALIZER
    return SUCCESS;
}

void destroy_mutex(pthread_mutex_t* mutex){
    pthread_mutex_destroy(mutex);
}

int main(int argc, char* argv[]){
    int8_t ret;
    thread_info* tinfo;
    void* tret;
    tinfo = malloc(sizeof(thread_info) * NUM_THREADS);
    if(tinfo == NULL){ 
        log_error("malloc failure");
        terminate(EXIT_FAILURE, true);
    }

    if(create_mutex(&lock) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    if(create_threads(tinfo, NUM_THREADS) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    if(join_threads(tinfo, NUM_THREADS) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    destroy_mutex(&lock);
    free(tinfo);

    log_info("Final value of shared_counter: %u", shared_counter);

    return 0;
}