#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>//file operations like open/close; read/write
#include <linux/cdev.h>//char device driver header
#include <linux/semaphore.h>//for synchronization behaviors
#include <asm/uaccess.h>//userspace to kernalspace & kernalspace to userspace


