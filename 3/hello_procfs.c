#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
 
 
#define procfs_name "hello_procfs" 
 
static struct proc_dir_entry *our_proc_file; 

int hello_procfs_open(struct inode *sp_inode, struct file *sp_file)
{
    printk("proc called open\n");
    return 0;
}
int hello_proc_release(struct inode *sp_inode, struct file *sp_file)
{
    printk("proc called release\n");
    return 0;
}

// called when a read operation is performed on /proc/hello_procfs
// NOTE: This function could be called multiple times
static ssize_t hello_procfs_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
    char s[14] = "Hello World!\n"; 
    int len = sizeof(s); 
    ssize_t ret = len;
 
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
    .proc_open = hello_procfs_open,
    .proc_release = hello_proc_release,
    .proc_read = hello_procfs_read, 
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
 
MODULE_DESCRIPTION("/proc/hello_procfs");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");