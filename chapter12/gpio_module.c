#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YoungJin Suh");
MODULE_DESCRIPTION("Raspberry Pi GPIO LED Device Module");

#if 0
#define BCM_IO_BASE
#else
#define BCM_TO_BASE
#endif
#define GPIO_BASE   (BCM_IO_BASE + 0x200000)
#define GPIO_SIZE   (256)

#define GPIO_IN(g)  
#define GPIO_OUT(g)

#define GPIO_SET(g)
#define GPIO_CLR(g)
#define GPIO_GET(g)
