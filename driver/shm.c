/**
 * @Filename: shm.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/27/2017 03:29:55 PM
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/io.h>

#define PROC_SHM_FILE "shmfile"

struct proc_dir_entry *g_proc_file = NULL;
unsigned long g_shm_addr = 0;

int shm_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long page;

    page = virt_to_phys((void *)g_shm_addr) >> PAGE_SHIFT;

    if(remap_pfn_range(vma, vma->vm_start, page, (vma->vm_end - vma->vm_start), vma->vm_page_prot)) {
        return -1;
    }
    vma->vm_flags |= VM_RESERVED;
    printk("remap_pfn_rang page:[%lu] ok.\n", page);

    return 0;
}

static const struct file_operations g_shm_fops ={
    .owner = THIS_MODULE,
    .mmap = shm_mmap,
};

static int __init shm_init(void)
{
    /*build proc dir "memshare"and two proc files: phymem_addr, phymem_size in the dir*/
    g_proc_file = proc_create(PROC_SHM_FILE, 0, NULL, &g_shm_fops);
    if (NULL == g_proc_file) {
        printk("proc create failed\n");
        return -1;
    }

    g_shm_addr = __get_free_page(GFP_KERNEL);
    if(!g_shm_addr) {
        printk("Allocate memory failure!/n");
        return -1;
    } else {
        SetPageReserved(virt_to_page(g_shm_addr));
        printk("Allocate memory success!. The phy mem addr=%lx\n", __pa(g_shm_addr));
    }
    strcpy((void *)g_shm_addr, "hello, world\n");

    return 0;
}

static void __exit shm_fini(void)
{
    ClearPageReserved(virt_to_page(g_shm_addr));
    free_page(g_shm_addr);
    remove_proc_entry(PROC_SHM_FILE, NULL);

    return;
}
module_init(shm_init);
module_exit(shm_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("accelink");
MODULE_DESCRIPTION("Kernel memory share module.");
