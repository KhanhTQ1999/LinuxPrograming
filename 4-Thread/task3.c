#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "../CmnBase/cmnbase.h"

#define READY 1
#define NOT_READY 0

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
uint32_t data;
bool data_ready = false;

static void* producer(void* arg){
    // acquire a lock
    for(uint32_t i = 0; i < 10; ++i){
        pthread_mutex_lock(&lock);
        data = rand() % 100;
        log_info("Producer modify data... %d", data);
        data_ready = READY;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    return NULL;
}

static void* customer(void* arg){
    for(uint32_t i = 0; i < 10; ++i){
        // acquire a lock
        log_info("Customer is waiting data...");
        pthread_mutex_lock(&lock);
        while(!data_ready){
            pthread_cond_wait(&cond, &lock);
        }
        log_info("Data: %d", data);
        data_ready = NOT_READY;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    int32_t ret;
    pthread_t thread_producer, thread_customer;

    ret = pthread_create(&thread_customer, NULL, customer, NULL);
    if(ret != 0){
        errno = ret;
        log_error_ne("pthread_create failure");
    }

    //Create producer thread after 1s
    sleep(1);
    ret = pthread_create(&thread_producer, NULL, producer, NULL);
    if(ret != 0){
        errno = ret;
        log_error_ne("pthread_create failure");
    }

    ret = pthread_join(thread_producer, NULL);
    if(ret != 0){
        errno = ret;
        log_error_ne("pthread_join failure");
    }

    ret = pthread_join(thread_customer, NULL);
    if(ret != 0){
        errno = ret;
        log_error_ne("pthread_join failure");
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}