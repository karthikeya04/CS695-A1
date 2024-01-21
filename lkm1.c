#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kern_levels.h>


static int init_fn(void)
{
    struct task_struct *task;

    printk(KERN_INFO "Loading Module...\n");
    for_each_process(task)
    {
        if(task->__state == TASK_RUNNING)
            printk(KERN_INFO "PID: [%d]\n", task->pid);
    }
    return 0;
}

static void cleanup_fn(void)
{
    printk(KERN_INFO "Unloading Module...\n");
}

module_init(init_fn);
module_exit(cleanup_fn);

MODULE_DESCRIPTION("LKM to list all processes in a running or runnable state");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");

