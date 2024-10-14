#include <linux/fs.h>

#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/mutex.h>

struct cdev gpio_cdev;
static int switch_irq;
static struct timer_list timer;
static DEFINE_MUTEX(led_mutex);



