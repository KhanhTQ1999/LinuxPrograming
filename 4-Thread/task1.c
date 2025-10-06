#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#include "../CmnBase/cmnbase.h"

#define NUM_THREADS 2

typedef struct {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int thread_num;       /* Application-defined thread # */
} thread_info;

static void* entry(void* arg){
    thread_info* tinfo = (thread_info*)arg;
    log_info("Thread %d is running... with tid %ld", tinfo->thread_num, tinfo->thread_id);

    //Allocate memory to hold return value
    char* ret = malloc(255);
    if(ret != NULL){
        sprintf(ret, "Thread %d end", tinfo->thread_num);
    }
   
    return ret;
}

uint32_t create_threads(thread_info* tinfo)
{
    int32_t ret;
    for(uint32_t tnum = 0; tnum < NUM_THREADS; ++tnum){
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

uint32_t join_threads(thread_info* tinfo){
    int32_t ret;
    void* tret;
    for(uint32_t tnum = 0; tnum < NUM_THREADS; ++tnum){
        ret = pthread_join(tinfo[tnum].thread_id, &tret);
        if(ret != 0){
            errno = ret;
            log_error_ne("pthread_join failure");
            return FAILURE;
        }

        if(tret == NULL)
            continue;

        log_info("Joined with thread %d; returned value was \"%s\"", tinfo[tnum].thread_num, (char *) tret);
        free(tret);
    }
    return SUCCESS;
}

int main(int argc, char* argv[]){
    thread_info* tinfo;

    tinfo = malloc(sizeof(thread_info) * NUM_THREADS);
    if(tinfo == NULL){ 
        log_error("malloc failure");
        terminate(EXIT_FAILURE, true);
    }
    
    if(create_threads(tinfo) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    if(join_threads(tinfo) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }
    
    log_info("All threads done");
    free(tinfo);
    return 0;
}