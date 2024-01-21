#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kern_levels.h>

static int pid = 0;

static int init_fn(void)
{
    struct task_struct *task, *parent;

    printk(KERN_INFO "Loading Module...\n");
    for_each_process(task)
    {
        if(task->pid == pid)
            break;
    }
    parent = task;
    printk(KERN_INFO "Printing children of PID: [%d]...\n", pid);
    list_for_each_entry(task, &parent->children, sibling)
    {
        printk(KERN_INFO "PID: [%d], State: [%d]\n", task->pid, task->__state);
    }

    return 0;
}

static void cleanup_fn(void)
{
    printk(KERN_INFO "Unloading Module...\n");
}

module_param(pid, int, 00600);
MODULE_PARM_DESC(pid, "parent's PID");


module_init(init_fn);
module_exit(cleanup_fn);

MODULE_DESCRIPTION("LKM that prints children info given parent's PID");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");

