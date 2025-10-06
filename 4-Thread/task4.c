#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "../CmnBase/cmnbase.h"

#define OR_NUM_THREADS 5
#define OW_NUM_THREADS 2

typedef struct {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int thread_num;       /* Application-defined thread # */
} thread_info;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
uint32_t data; // Shared resource

static void* rt_entry(void* arg){
    thread_info* tinfo = (thread_info*)arg;
    char* ret = malloc(255);  
    if(ret != NULL) 
        sprintf(ret, "READ thread %d", tinfo->thread_num);

    pthread_rwlock_rdlock(&rwlock);
    for(uint32_t i = 0; i < OR_NUM_THREADS; ++i){
        log_info("READ thread %d read data... %d", tinfo->thread_num, data);
        sleep(1);
    }
    pthread_rwlock_unlock(&rwlock);

    return ret;
}

static void* wt_entry(void* arg){
    thread_info* tinfo = (thread_info*)arg;
    char* ret = malloc(255);   
    if(ret != NULL) 
        sprintf(ret, "WRITE thread %d", tinfo->thread_num);
    
    pthread_rwlock_wrlock(&rwlock);
    for(data = 0; data < OW_NUM_THREADS; ++data){
        log_info("WRITE thread %d modify data... %d", tinfo->thread_num, data);
        sleep(1);
    }
    pthread_rwlock_unlock(&rwlock);

    return ret;
}

uint32_t create_threads(thread_info* tinfo, size_t num, void* entry)
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
        if(tret == NULL)
            continue;

        log_info("Joined with thread %d; returned value was \"%s\"", tinfo[tnum].thread_num, (char *) tret);
        free(tret);
    }

    return SUCCESS;
}

int main(int argc, char* argv[]){
    int32_t ret;
    thread_info* wt_info;
    thread_info* rt_info;
    void* tret;

    rt_info = malloc(sizeof(thread_info) * OR_NUM_THREADS);
    if(rt_info == NULL){ 
        log_error("malloc failure");
        terminate(EXIT_FAILURE, true);
    }

    wt_info = malloc(sizeof(thread_info) * OW_NUM_THREADS);
    if(wt_info == NULL){ 
        log_error("malloc failure");
        terminate(EXIT_FAILURE, true);
    }

    //Write thread 
    if(create_threads(rt_info, OR_NUM_THREADS, rt_entry) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }
    //Read thread
    if(create_threads(wt_info, OW_NUM_THREADS, wt_entry) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    if(join_threads(rt_info, OR_NUM_THREADS) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }
   
    if(join_threads(wt_info, OW_NUM_THREADS) == FAILURE){
        terminate(EXIT_FAILURE, true);
    }

    log_info("All threads done");

    free(wt_info);
    free(rt_info);
    pthread_rwlock_destroy(&rwlock);

    return 0;
}