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
 
#include "mem_ops_ioctl.h"
 
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

static unsigned long get_pa(int pid, unsigned long va)
{
    struct task_struct *task;
    struct mm_struct *mm;
    struct page* page;
    pgd_t *pgdp, pgd;
    p4d_t *p4dp, p4d;
    pud_t *pudp, pud;
	pmd_t *pmdp, pmd;
	pte_t *ptep, pte;

    void *laddr;
    phys_addr_t paddr, page_addr;
    unsigned long page_offset = 0;

    printk(KERN_INFO "Loading Module...\n");

    for_each_process(task)
    {
        if(task->pid == pid)
            break;
    }
    if(!task)
    {
        printk(KERN_INFO "Process with PID %d not found\n", pid);
        return 0;
    }

    mm = task->mm;
    if(!mm)
    {
        printk(KERN_INFO "Process with PID %d doesn't have a memory map\n", pid);
        return 0;
    }

    pgdp = pgd_offset(mm, va);
	pgd = READ_ONCE(*pgdp);

    if (!(pgd_none(pgd) || pgd_bad(pgd)))
	{
        p4dp = p4d_offset(pgdp, va);
		p4d = READ_ONCE(*p4dp);
        if (!(p4d_none(p4d) || p4d_bad(p4d)))
		{
            pudp = pud_offset(p4dp, va);
		    pud = READ_ONCE(*pudp);
            if (!(pud_none(pud) || pud_bad(pud)))
			{
                pmdp = pmd_offset(pudp, va);
		        pmd = READ_ONCE(*pmdp);
                if (!(pmd_none(pmd) || pmd_bad(pmd)))
                {
                    ptep = pte_offset_map(pmdp, va);
		            pte = READ_ONCE(*ptep);
                    if(!pte_none(pte))
                    {
                        page = pte_page(pte);
                        laddr = page_address(page);
                        page_addr = virt_to_phys(laddr);
                        page_offset = va & ~PAGE_MASK;
                        paddr = page_addr | page_offset;

                        printk(KERN_INFO "PID: [%d], VA: [%lu], PA: [%lu]\n", pid, va, (unsigned long) paddr);
                        return paddr;
                    }
                }
            }
        }
    }
    printk(KERN_INFO "Invalid virtual address [%lu] of PID [%d]\n", va, pid);
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
        case FILL_PA:
            unsigned long va, pa;
            if (copy_from_user(&va, UL_PTR(arg + offsetof(addr_mapping, va)), UL_SIZE))
            {
                return -EACCES;
            }
            pa = get_pa(current->pid, va);
            if (copy_to_user(UL_PTR(arg + offsetof(addr_mapping, pa)), &pa, UL_SIZE))
            {
                return -EACCES;
            }
            break;
        case WRITE_TO_PA:
            mem_data md;
            char* kernel_ptr;
            if (copy_from_user(&md, (mem_data *)arg, sizeof(mem_data)))
            {
                return -EACCES;
            }
            kernel_ptr = phys_to_virt(md.pa);
            *kernel_ptr = md.value;
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
 
static int __init mem_ops_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
    printk(KERN_INFO "Loading Module...\n");
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "mem_ops_ioctl")) < 0)
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
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "mem_ops")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
    return 0;
}
 
static void mem_ops_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    printk(KERN_INFO "Unloading Module...\n");
}
 
module_init(mem_ops_ioctl_init);
module_exit(mem_ops_ioctl_exit);
 
MODULE_DESCRIPTION("Mem ioctl() device driver");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");