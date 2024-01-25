#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h>
#include <linux/mm.h>
#include <linux/vm_event_item.h>
#include <linux/vmstat.h>
#include <linux/slab.h>
 
#define procfs_name "get_pgfaults" 
 
static struct proc_dir_entry *our_proc_file; 

int get_pgfaults_open(struct inode *sp_inode, struct file *sp_file)
{
    printk("proc called open\n");
    return 0;
}
int hello_proc_release(struct inode *sp_inode, struct file *sp_file)
{
    printk("proc called release\n");
    return 0;
}

static unsigned long get_pgfaults(void)
{
    unsigned long ev[NR_VM_EVENT_ITEMS];
    all_vm_events(ev);
    return ev[PGFAULT];
}

static char* convert_to_string(unsigned long n)
{
    int len = 0, temp = n, idx;
    char* ret;
    while(temp)
        len++, temp/=10;
    ret = kmalloc(len + 2, GFP_KERNEL);
    idx = len - 1;
    ret[len] = '\n';
    ret[len + 1] = '\0';
    while(n)
        ret[idx] = (char)('0' + n%10), n/=10, idx--;
    return ret;
}

static ssize_t get_pgfaults_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
    unsigned long num_pgfaults = get_pgfaults();
    char* s = convert_to_string(num_pgfaults);
    int len = sizeof(s);
    ssize_t ret = len;
    printk(KERN_INFO "#page faults : [%lu]\n",num_pgfaults);
    if (*offset >= len) { 
        return 0;
    }
    if(copy_to_user(buffer, s, len))
    {
        printk(KERN_INFO "copy_to_user failed\n");
        return 0;
    }
    else {
        printk(KERN_INFO "procfile read %s\n", file_pointer->f_path.dentry->d_name.name); 
        *offset += len;
    }
    return ret; 
} 
 
static const struct proc_ops proc_file_fops = { 
    .proc_open = get_pgfaults_open,
    .proc_release = hello_proc_release,
    .proc_read = get_pgfaults_read, 
};
 
static int __init procfs1_init(void) 
{ 
    our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops); 
    if (NULL == our_proc_file) { 
        proc_remove(our_proc_file); 
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_name); 
        return -ENOMEM; 
    } 
 
    printk(KERN_INFO "/proc/%s created\n", procfs_name); 
    return 0; 
} 
 
static void __exit procfs1_exit(void) 
{ 
    proc_remove(our_proc_file); 
    printk(KERN_INFO "/proc/%s removed\n", procfs_name); 
} 
 
module_init(procfs1_init); 
module_exit(procfs1_exit); 
 
MODULE_DESCRIPTION("/proc/get_pgfaults");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");