#ifndef CHNG_PAR_IOCTL_H
#define CHNG_PAR_IOCTL_H
#include <linux/ioctl.h>
 
#define IOCTL_CHNG_PAR _IOW('c', 1, int)

// command to change the parent of the current process
#define DEVICE_FILE_NAME "/dev/chng_par"

#endif