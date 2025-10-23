#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>

#include "../CmnBase/cmnbase.h"

#define MAX_ITEM 10
#define VEGAN_SHM_IDX 0
#define NON_VEGAN_SHM_IDX 1
#define VEGAN_SHM_NAME     "/vegan"
#define NON_VEGAN_SHM_NAME "/non_vegan"

typedef struct {
    const char *roleDesc;           // Description of the child process role
    void (*parentHandler)(pid_t);   // Parent process handler
    void (*childHandler)(void);     // Child process handler
} ForkHandler;

typedef struct {
    char name[255];
    sem_t *sem;
} Semaphore;
typedef struct {
    const char *name;
    int32_t fd;
    uint32_t len;
    char *addr;
    Semaphore semaphore;
} ShareMemory;

typedef struct {
    int32_t id;
    time_t time;
    char dish[255];
} Food;

typedef struct {
    uint32_t count;
    Food items[MAX_ITEM];
} ShareFoodBuffer;

bool shmInit(ShareMemory* shm_ptr);
bool spawnChildProcesses();
pid_t forkWithHandlers(
    const char *roleDesc,
    void (*parentHandler)(pid_t),
    void (*childHandler)(void)
);
void cleanup();
void waitForChildren();
void chefDonatello();
void chefPortecelli();
void nonVeganCustomer();
void veganCustomer();
void hybridCustomer();

pid_t cid[5]; // Child process IDs
bool is_running;
ShareMemory food_shm[2]; //0: vegan food, 1: non-vegan food
char *non_vegan_name[] = {
    "Fettuccine Chicken Alfredo",
    "Garlic Sirloin Steak"
};
char *vegan_name[] = {
    "Pistachio Pesto Pasta",
    "Avocado Fruit Salad"
};
ForkHandler handlers[] = { // Handlers for child processes
    { "Chef Donatello", NULL, chefDonatello },
    { "Chef Portecelli", NULL, chefPortecelli },
    { "Non-Vegan Customer", NULL, nonVeganCustomer },
    { "Vegan Customer", NULL, veganCustomer },
    { "Hybrid Customer", NULL, hybridCustomer }
};

int main(int argc, char* argv[]){
    is_running = true;
    food_shm[NON_VEGAN_SHM_IDX] = (ShareMemory) {
        .name = NON_VEGAN_SHM_NAME,
        .len = sizeof(ShareFoodBuffer)
    };

    food_shm[VEGAN_SHM_IDX] = (ShareMemory) {
        .name = VEGAN_SHM_NAME,
        .len = sizeof(ShareFoodBuffer)
    };

    if(!shmInit(&food_shm[VEGAN_SHM_IDX])){
        terminate(EXIT_FAILURE, true);
    }

    if(!shmInit(&food_shm[NON_VEGAN_SHM_IDX])){
        terminate(EXIT_FAILURE, true);
    }



    if(!spawnChildProcesses()){
        terminate(EXIT_FAILURE, true);
    }

    waitForChildren();
    cleanup();

    return 0;
}

bool shmInit(ShareMemory* shm_ptr) {
    char sem_name[64];
    snprintf(sem_name, sizeof(sem_name), "%s_sem", shm_ptr->name);

    snprintf(shm_ptr->semaphore.name, sizeof(shm_ptr->semaphore.name), "%s", sem_name);
    shm_ptr->semaphore.sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (shm_ptr->semaphore.sem == SEM_FAILED) {
        log_error("sem_open failed for %s", sem_name);
        return false;
    }

    shm_ptr->fd = shm_open(shm_ptr->name, O_CREAT | O_RDWR, 0666);
    if (shm_ptr->fd == -1) {
        log_error_ne("shm_open");
        return false;
    }

    if (ftruncate(shm_ptr->fd, shm_ptr->len) == -1) {
        log_error_ne("ftruncate");
        return false;
    }

    shm_ptr->addr = mmap(NULL, shm_ptr->len, PROT_READ | PROT_WRITE, MAP_SHARED, shm_ptr->fd, 0);
    if (shm_ptr->addr == MAP_FAILED) {
        log_error_ne("mmap");
        return false;
    }

    return true;
}

bool spawnChildProcesses() {
    // Fork child processes based on the handlers defined
    for (size_t i = 0; i < sizeof(handlers)/sizeof(handlers[0]); i++) {
        // Ensure we do not exceed the cid array size
        if(i >=  sizeof(cid)/sizeof(cid[0])){
            log_error("Insufficient space in cid array");
            return false;
        }

        // Fork with specified handlers
        cid[i] = forkWithHandlers(
            handlers[i].roleDesc,
            handlers[i].parentHandler,
            handlers[i].childHandler
        );

        //Any fork failure, retrun false
        if (cid[i] == -1) {
            return false;
        }
    }
    return true;
}

pid_t forkWithHandlers(
    const char *roleDesc,
    void (*parentHandler)(pid_t),
    void (*childHandler)(void)
) {
    pid_t cid = fork();
    if (cid == 0) {
        // Child process
        log_info("Child process for '%s' started", roleDesc);
        if (childHandler) {
            childHandler();
        }
        //End child process
        exit(EXIT_SUCCESS);
    } else if (cid < 0) {
        log_error("Fork failed for %s", roleDesc);
        return -1;
    } else {
        //Parent process
        log_info("Child process for '%s' created with pid = %d", roleDesc, cid);
        if (parentHandler) {
            parentHandler(cid);
        }
        return cid;
    }
}

void cleanup() {
    for(int32_t i = 0; i < sizeof(food_shm) / sizeof(food_shm[0]); ++i){
        close(food_shm[i].fd);
        shm_unlink(food_shm[i].name);

        sem_close(food_shm[i].semaphore.sem);
        sem_unlink(food_shm[i].semaphore.name);
    }
}

void waitForChildren() {
    for (size_t i = 0; i < sizeof(cid) / sizeof(cid[0]); i++) {
        int status;
        pid_t wpid = waitpid(cid[i], &status, 0);

        if (wpid == -1) {
            log_error("Failed to wait for child process with pid %d", cid[i]);
            continue;
        }

        if (WIFEXITED(status)) {
            log_info("Child process %d exited with status %d", wpid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            log_warn("Child process %d terminated by signal %d", wpid, WTERMSIG(status));
        } else {
            log_warn("Child process %d exited abnormally", wpid);
        }
    }
}

void chefDonatello(){
    srand(time(NULL));
    uint32_t second_sleep;
    uint32_t food_idx;
    ShareFoodBuffer *buffer = (ShareFoodBuffer *)food_shm[NON_VEGAN_SHM_IDX].addr;
    
    while(is_running){
        sem_wait(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
        uint32_t count = buffer->count;
        if(count >= MAX_ITEM){
            sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
            usleep(100 * 1000);
            continue;
        }

        second_sleep = rand() % 5 + 1;
        food_idx = rand() % 2;

        Food food = {
            .id = count,
            .time = time(NULL),
        };
        snprintf(food.dish, sizeof(food.dish), "%s", non_vegan_name[food_idx]);
        buffer->items[count] = food;
        buffer->count = count + 1;
        sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
        log_info("Donatello cooked: %s (ID: %d)", food.dish, food.id);

        sleep(second_sleep);
    }
    log_info("Donatello come back home.");
}

void chefPortecelli(){
    srand(time(NULL));
    uint32_t second_sleep;
    uint32_t food_idx;
    ShareFoodBuffer *buffer = (ShareFoodBuffer *)food_shm[VEGAN_SHM_IDX].addr;
    
    while(is_running){
        sem_wait(food_shm[VEGAN_SHM_IDX].semaphore.sem);
        uint32_t count = buffer->count;
        if(count >= MAX_ITEM){
            sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);
            usleep(100 * 1000);
            continue;
        }

        second_sleep = rand() % 5 + 1;
        food_idx = rand() % 2;

        Food food = {
            .id = count,
            .time = time(NULL),
        };
        snprintf(food.dish, sizeof(food.dish), "%s", vegan_name[food_idx]);
        buffer->items[count] = food;
        buffer->count = count + 1;
        sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);
        log_info("Portecelli cooked: %s (ID: %d)", food.dish, food.id);
        
        sleep(second_sleep);
    }
    log_info("Portecelli come back home.");
}

void nonVeganCustomer(){
    srand(time(NULL));
    uint32_t second_sleep;
    uint32_t food_idx;
    uint32_t count;
    ShareFoodBuffer *buffer = (ShareFoodBuffer *)food_shm[NON_VEGAN_SHM_IDX].addr;
    while(is_running){
        sem_wait(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
        count = buffer->count;
        if(count <= 0){
            sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
            usleep(100 * 1000);
            continue;
        }

        second_sleep = rand() % 6 + 10;
        food_idx = rand() % count;
        
        Food item = buffer->items[food_idx];
        for(int32_t idx = food_idx; idx < count; ++idx){
            buffer->items[idx] = buffer->items[idx+1];
        }
        memset(&buffer->items[count], 0x00, sizeof(Food));
        buffer->count = count-1;
        sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
        log_info("Non-Vegan customer get: %s (ID: %d)", item.dish, item.id);

        sleep(second_sleep);
    }
    log_info("Non-Vegan customer come back home.");
}

void veganCustomer(){
    srand(time(NULL));
    uint32_t second_sleep;
    uint32_t food_idx;
    uint32_t count;
    ShareFoodBuffer *buffer = (ShareFoodBuffer *)food_shm[VEGAN_SHM_IDX].addr;
    while(is_running){
        sem_wait(food_shm[VEGAN_SHM_IDX].semaphore.sem);
        count = buffer->count;
        if(count <= 0){
            sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);
            usleep(100 * 1000);
            continue;
        }

        second_sleep = rand() % 6 + 10;
        food_idx = rand() % count;
        
        Food item = buffer->items[food_idx];
        for(int32_t idx = food_idx; idx < count; ++idx){
            buffer->items[idx] = buffer->items[idx+1];
        }
        memset(&buffer->items[count], 0x00, sizeof(Food));
        buffer->count = count-1;
        sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);
        log_info("Vegan customer get: %s (ID: %d)", item.dish, item.id);

        sleep(second_sleep);
    }
    log_info("Vegan customer come back home.");
}

void hybridCustomer(){
    srand(time(NULL));
    uint32_t second_sleep;
    uint32_t vegan_idx;
    uint32_t non_vegan_idx;
    uint32_t vegan_cnt;
    uint32_t non_vegan_cnt;

    ShareFoodBuffer *vegan_buffer = (ShareFoodBuffer *)food_shm[VEGAN_SHM_IDX].addr;
    ShareFoodBuffer *non_vegan_buffer = (ShareFoodBuffer *)food_shm[NON_VEGAN_SHM_IDX].addr;

    while(is_running){
        sem_wait(food_shm[VEGAN_SHM_IDX].semaphore.sem);
        sem_wait(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
        vegan_cnt = vegan_buffer->count;
        non_vegan_cnt = non_vegan_buffer->count;
        if(vegan_cnt <= 0 || non_vegan_cnt <= 0){
            sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);
            sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);
            usleep(100 * 1000);
            continue;
        }

        second_sleep = rand() % 6 + 10;

        vegan_idx = rand() % vegan_cnt;
        Food vegan_item = vegan_buffer->items[vegan_idx];
        for(int32_t idx = vegan_idx; idx < vegan_cnt; ++idx){
            vegan_buffer->items[idx] = vegan_buffer->items[idx+1];
        }
        memset(&vegan_buffer->items[vegan_cnt], 0x00, sizeof(Food));
        vegan_buffer->count = vegan_cnt-1;
        sem_post(food_shm[VEGAN_SHM_IDX].semaphore.sem);

        non_vegan_idx = rand() % non_vegan_cnt;
        Food non_vegan_item = non_vegan_buffer->items[non_vegan_idx];
        for(int32_t idx = non_vegan_idx; idx < non_vegan_cnt; ++idx){
            non_vegan_buffer->items[idx] = non_vegan_buffer->items[idx+1];
        }
        memset(&non_vegan_buffer->items[non_vegan_cnt], 0x00, sizeof(Food));
        non_vegan_buffer->count = non_vegan_cnt-1;
        sem_post(food_shm[NON_VEGAN_SHM_IDX].semaphore.sem);

        log_info("Hybrid customer get: %s (ID: %d) and %s (ID: %d)", vegan_item.dish, vegan_item.id, non_vegan_item.dish, non_vegan_item.id);
        sleep(second_sleep);
    }
}