#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../CmnBase/cmnbase.h"

typedef struct {
    int id;
    int price;
    char name[256];
} ProductInfo;

int main(int argc, char* argv[]) {
    int32_t fd;
    ProductInfo product;
    struct stat sb;
    int32_t len = sizeof(ProductInfo);
    char *addr;
    const char *shm_name = "my_shared_memory";
    
    fd = shm_open(shm_name, O_RDONLY, 0); /* Open existing object */
    if (fd == -1){
        log_error_ne("shm_open");
        terminate(EXIT_FAILURE, true);
    }

    /* Use shared memory object size as length argument for mmap()
        and as number of bytes to write() */
    if (fstat(fd, &sb) == -1){
        log_error_ne("fstat");
        terminate(EXIT_FAILURE, true);        
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED){
        log_error_ne("mmap");
        terminate(EXIT_FAILURE, true);
    }
    
    if (close(fd) == -1) /* 'fd' is no longer needed */{
        log_error_ne("close");
        terminate(EXIT_FAILURE, true);
    }

    memcpy(&product, addr, len);

    printf("Product ID: %d\n", product.id);
    printf("Price     : %d\n", product.price);
    printf("Name      : %s\n", product.name);

    return 0;
}