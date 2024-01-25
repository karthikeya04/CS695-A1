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
#include <linux/pagewalk.h>
#include <linux/maple_tree.h>
#include <linux/mmap_lock.h>

static int pid = 0;

// returns the size of the virtual address space
static unsigned long get_virt_size(struct mm_struct *mm)
{
    // reference : https://elixir.bootlin.com/linux/v6.1/source/arch/parisc/kernel/cache.c#L662
    struct vm_area_struct *vma;
    unsigned long virt_size = 0;
    // traverse through all the vmas to find the total size of the virtual address space
    VMA_ITERATOR(vmi, mm, 0);
    for_each_vma(vmi, vma) {
        virt_size += vma->vm_end - vma->vm_start;
    }
    return virt_size;
}

// returns true if there exists a physical page mapped to the given virtual address (va)
bool mapping_exist(struct mm_struct *mm, unsigned long va)
{
    pgd_t *pgdp, pgd;
    p4d_t *p4dp, p4d;
    pud_t *pudp, pud;
	pmd_t *pmdp, pmd;
	pte_t *ptep, pte;

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
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

static unsigned long get_phys_size(struct mm_struct *mm)
{
    unsigned long phys_size = 0;
    struct vm_area_struct *vma;
    unsigned long vmpage;
    VMA_ITERATOR(vmi, mm, 0);
    for_each_vma(vmi, vma) {
        // iterate through all the pages in the current vma
        // NOTE: memory in a single vma is contigous
        for(vmpage = vma->vm_start; vmpage < vma->vm_end; vmpage += PAGE_SIZE)
        {
            // check if the mapping exist
            if(mapping_exist(mm, vmpage))
                phys_size += PAGE_SIZE;
        }
    }
    return phys_size;
}

static int init_fn(void)
{
    struct task_struct *task = NULL;
    struct mm_struct *mm;
    unsigned long virt_size = 0, phys_size = 0;

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

    virt_size = get_virt_size(mm);
    phys_size = get_phys_size(mm);
    printk(KERN_INFO "Size of the allocated virtual address space: [%lu]\n", virt_size);
    printk(KERN_INFO "Size of the mapped physical address space  : [%lu]\n", phys_size);
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

MODULE_DESCRIPTION("LKM that prints the size of Virtual address space and mapped physical address space");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");

