/**
 * @Filename: eth2.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/27/2017 03:29:55 PM
 */
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

#define RESEVE_SPACE 0xf0000000

static int __init eth2_init(void)
{
    printk("eth module init\n");
    char *ptr;
    
    //ptr = phys_to_virt(RESEVE_SPACE);
    memcpy(RESEVE_SPACE, "hello world\n", 11);

    return 0;
}

static void __exit eth2_exit(void)
{
    printk("eth2 module exit\n");

    return;
}

module_init(eth2_init);
module_exit(eth2_exit);
