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
    printf("[OP] Allocating 1 byte of memory on the heap...\n");
    // allocate byte-size memory on the heap
    char *ptr = malloc(sizeof(char));

    // assign value 6 to the memory
    *ptr = 6;
    printf("[INFO] ptr of type char* points to the allocated memory\n");

    // prints the address (virtual address) and the value of the allocated memory
    printf("[INFO] VA (i.e the value of ptr) is %lu\n", (unsigned long) ptr);
    printf("[INFO] The value of (*ptr) is %d\n", (int) *ptr);

    // setting up the argument for ioctl call
    addr_mapping am;
    am.va = (unsigned long) ptr;
    printf("[OP] Making an ioctl call to get the physical address...\n");

    // Make an ioctl call to get the physical address of the allocated memory
    if (ioctl(fd, FILL_PA, &am) == -1)
    {
        perror("mem_ops ioctl_1");
        return;
    }
    printf("[INFO] PA received from the ioctl call is %lu\n", am.pa);

    // setting up the argument for ioctl call
    mem_data md = { 
        .pa = am.pa, // physical address
        .value = 5 // new value
    };
    printf("[OP] Making an ioctl call to update the value to 5 using PA...\n");

    // Make another ioctl call to change the value of the memory to 5 using physical address
    if (ioctl(fd, WRITE_AT_PA, &md) == -1)
    {
        perror("mem_ops ioctl_1");
        return;
    }

    // check if the value is modified to 5 using the user-space virtual address
    printf("[INFO] The value of (*ptr) is %d\n", (int) *ptr);
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

