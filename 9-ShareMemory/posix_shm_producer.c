#include "stdint.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "../CmnBase/cmnbase.h"

typedef struct {
    int id;
    int price;
    char name[256];
} ProductInfo;

int main(int argc, char* argv[]) {
    int32_t fd;
    ProductInfo product = {
        .id = 1,
        .price = 100,
        .name = "Sample Product"
    };
    int32_t len = sizeof(ProductInfo);
    char *addr;
    const char *shm_name = "my_shared_memory";
    
    fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666); /* Open existing object */
    if (fd == -1) {
        log_error_ne("shm_open");
        terminate(EXIT_FAILURE, true);
    }

    if (ftruncate(fd, len) == -1) { /* Resize object to hold string */
        log_error_ne("ftruncate");
        terminate(EXIT_FAILURE, true);
    }

    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED){
        log_error_ne("mmap");
        terminate(EXIT_FAILURE, true);
    }

    if (close(fd) == -1){
        log_error_ne("close");
        terminate(EXIT_FAILURE, true);
    } 
    memcpy(addr, &product, len); /* Copy string to shared memory */
    return 0;
}