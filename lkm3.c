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

static int pid = 0;
static unsigned long va = 0;

static int init_fn(void)
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
                        return 0;
                    }
                }
            }
        }
    }
    printk(KERN_INFO "Invalid virtual address [%lu] of PID [%d]\n", va, pid);
    return 0;
}

static void cleanup_fn(void)
{
    printk(KERN_INFO "Unloading Module...\n");
}

module_param(pid, int, 00600);
MODULE_PARM_DESC(pid, "parent's PID");

module_param(va, ulong, 00600);
MODULE_PARM_DESC(va, "Virtual address");

module_init(init_fn);
module_exit(cleanup_fn);

MODULE_DESCRIPTION("LKM that prints PA given VA and PID");
MODULE_AUTHOR("Karthikeya");
MODULE_LICENSE("GPL");

