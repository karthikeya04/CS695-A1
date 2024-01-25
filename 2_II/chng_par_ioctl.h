#ifndef CHNG_PAR_IOCTL_H
#define CHNG_PAR_IOCTL_H
#include <linux/ioctl.h>
 
#define IOCTL_CHNG_PAR _IOW('c', 1, int)

#define DEVICE_FILE_NAME "/dev/chng_par"

#endif