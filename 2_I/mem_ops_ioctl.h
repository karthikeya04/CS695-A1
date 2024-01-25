#ifndef MEM_IOCTL_H
#define MEM_IOCTL_H
#include <linux/ioctl.h>
 
typedef struct
{
    unsigned long va, pa;
} addr_mapping;

typedef struct
{
    unsigned long pa;
    char value;
} mem_data;

// command to get the physical address for a virtual address
#define FILL_PA _IOWR('q', 1, addr_mapping *)

// command to write the given value at the given physical address
#define WRITE_AT_PA _IOW('q', 2, mem_data *)

#define DEVICE_FILE_NAME "/dev/mem_ops"

#endif