#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "mem_ops_ioctl.h"

void test(int fd)
{
    char *ptr = malloc(sizeof(char));
    *ptr = 6;
    printf("VA is %lu and the value is %d\n", (unsigned long) ptr,(int) *ptr);
    addr_mapping am;
    am.va = (unsigned long) ptr;

    printf("Making an ioctl call to get the physical address....\n");
    if (ioctl(fd, FILL_PA, &am) == -1)
    {
        perror("mem_ops ioctl_1");
        return;
    }
    printf("PA is %lu\n", am.pa);

    mem_data md = { 
        .pa = am.pa,
        .value = 5
    };
    printf("Making an ioctl call to update the value to 5 using PA....\n");
    if (ioctl(fd, WRITE_TO_PA, &md) == -1)
    {
        perror("mem_ops ioctl_1");
        return;
    }
    printf("Modified value is %d\n", (int) *ptr);
}

int main(int argc, char *argv[])
{
    int fd = open(DEVICE_FILE_NAME, O_RDWR);
    if (fd == -1)
    {
        perror("mem_ops open");
        return 2;
    }
    test(fd);

    if (close(fd) == -1) {
        perror("ERROR: could not close driver");
    }
}

