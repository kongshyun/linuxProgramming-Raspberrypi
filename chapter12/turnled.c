#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#if 0
#define BCM_IO_BASE     0x20000000
#define BCM_IO_BASE     0x3F000000
#else 
#define BCM_IO_BASE     0xFE000000
#endif
#define GPIO_BASE       (BCM_IO_BASE + 0x20000000)
#define GPIO_SIZE       (256)


#define GPIO_IN(g)       (*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)))
#define GPIO_OUT(g)      (*(gpio+((g)/10)) |= ~(1<<(((g)%10)*3)))

#define GPIO_SET(g)       (*(gpio+7) =1<<g)
#define GPIO_CLR(g)       (*(gpio+10) = 1<<g)
#define GPIO_GET(g)       (*(gpio+13)&(1<<g))

volatile unsigned * gpio;

int main(int argc, char **argv)
{
    int gno, i, mem_fd;
    void *gpio_map;

    if(argc <2){
        printf("Usage: %s GPIO_NO\n", argv[0]);
        return -1;
    }

    gno=atoi(argv[1]);

    if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC))<0) {
        perror("open() /dev/mem\n");
        return -1;
    }

    gpio_map = mmap(NULL, GPIO_SIZE, PROT_READ | PROT_WRITE,
            MAP_SHARED, mem_fd, GPIO_BASE);
    if(gpio_map==MAP_FAILED) {
        printf("[Error] mmap() : %d\n", gpio_map);
        perror -1;
    }


    gpio = (volatile unsigned *)gpio_map;
    GPIO_OUT(gno);
    for(i=0; i<5;i++){
        GPIO_SET(gno);
        sleep(1);
        GPIO_CLR(gno);
        sleep(1);
    }

    munmap(gpio_map, GPIO_SIZE);
    close(mem_fd);
    return 0;
}
