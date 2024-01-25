#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kern_levels.h>
#include <linux/pgtable.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/pid.h>
#include <asm/io.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/list.h>
 
#include "chng_par_ioctl.h"
 
#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

#define UL_PTR (unsigned long *)
#define UL_SIZE sizeof(unsigned long)
 
static int my_open(struct inode *i, struct file *f)
{
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int pid;
    struct task_struct *task = NULL, *new_parent;
    switch (cmd)
    {
        case IOCTL_CHNG_PAR:
            if (copy_from_user(&pid, (int *)arg, sizeof(int)))
            {
                return -EFAULT;
            }
            for_each_process(task)
            {
                if(task->pid == pid)
                    break;
            }
            if(!task)
            {
                printk(KERN_INFO "Process with PID %d not found\n", pid);
                return -ESRCH;
            }
            new_parent = task;

            // change the parent of the current process
            current->real_parent = new_parent;
            current->parent = new_parent;
            list_del(&current->sibling);
            list_add(&current->sibling,&new_parent->children);

            printk(KERN_INFO "Parent of process %d changed to %d\n", current->pid, new_parent->pid);
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}
 
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};
 
static int __init chng_par_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
    printk(KERN_INFO "Loading Module...\n");
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "chng_par_ioctl")) < 0)
    {
        return ret;
    }
    cdev_init(&c_dev, &query_fops);
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "chng_par")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
    return 0;
}
 
static void chng_par_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    printk(KERN_INFO "Unloading Module...\n");
}
 
module_init(chng_par_ioctl_init);
module_exit(chng_par_ioctl_exit);
 
MODULE_DESCRIPTION("change parent ioctl() device driver");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");