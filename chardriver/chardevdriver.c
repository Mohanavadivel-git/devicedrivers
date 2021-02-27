#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>//file operations like open/close; read/write
#include <linux/cdev.h>//char device driver header
#include <linux/semaphore.h>//for synchronization behaviors
#include <linux/uaccess.h>//userspace to kernalspace & kernalspace to userspace

MODULE_LICENSE("GPL");

//(1) Create a structure for a fake device
struct fake_device{
	char data[100];
	struct semaphore sem;
} virtual_device;


//(2) To register our fake device we need a cdev object and some other varialbes
struct cdev *mcdev;  //my char device
int major_number;    //will store our device major number - extracted from dev_t using macro - mknod /director/file c major minor
int ret;             //will be used to hold return values of functions; this is because the kernel stack is very small, 
                     //so declaring variables all over the pass in our module functions eats up the stack very fast 
dev_t dev_num;       //will hold the major number that kernel gives us,
                     //name--> appears in /proc/device
#define DEVICE_NAME "MYTESTDEVICE"


//file operation declaration
int device_open(struct inode *, struct file *);
int device_close(struct inode *, struct file *);
ssize_t device_read(struct file *, char *, size_t, loff_t *);
ssize_t device_write(struct file *, const char *, size_t, loff_t *);


//(6) Tell the kernel which functions to call when user operates on our device file
static struct file_operations fops = {
	.owner = THIS_MODULE, //prevent unloading of this module when operation in use
	.open = device_open, //points to the method to call when opening a device
	.release = device_close, //points to the method to call when closing a device
	.write = device_write, //points to the method to call when writing to the device
	.read = device_read, //points to the method to call when reading a device
};


//(7)called on device_file open
//	inode reference to the file on disk
//	and contains information about the file
//	struct file is represents an abstract open file
int device_open(struct inode *inode, struct file *filp){

	//only allow one process to open this device by using semaphore as mutual exclusive lock - mutex
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "MYTESTDEVICE: could not lock device during open\n");
		return -1;
	}

	printk(KERN_INFO "MYTESTDEVICE: opened device\n");
	return 0;
}

//(8) called when user wants to get information from the device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t *curOffset){
	//take data from kernel space(device) to user space(process)
	//copy_to_user(desitnation, source, sizeToTransfer)
	printk(KERN_INFO "MYTESTDEVICE: Reading from device\n");
	ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

//(9) called when user wants to send information to the device
ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//send data from user to kernel
	//copy_from_user (dest, source, count)
	
	printk(KERN_INFO "MYTESTDEVICE: writing to device\n");
	ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);

	return bufCount;
}

//(10) called upon user close
int device_close(struct inode *inode, struct file *filp){
	//by calling up, which is opposite of down for semaphore, we release the mutex that we obtained at device open
	//this has the effect of allowing other process to use the device now
	up(&virtual_device.sem);
	printk(KERN_INFO "MYTESTDEVICE: closed device\n");
	return 0;
}



//device driver entry point
//(1). Register the device - Two step process
//	(a).use dynamic allocation to assign our device
//	      a major number - alloc_chrdev_region(dev_t*,uint fminor,uint count, char* name)

static int driver_entry(void){
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	
	if(ret < 0){
		printk(KERN_ALERT "MYTESTDEVICE: failed to allocate a major number\n");
		return ret;
	}
	major_number = MAJOR(dev_num); //extracts the major number and store in our variable(MACRO)
	printk(KERN_INFO "MYTESTDEVICE: major number is %d\n", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file\n",DEVICE_NAME,major_number);

	//step(2)
	mcdev = cdev_alloc(); //create our cdev structure, initialized our cdev
	mcdev->ops = &fops; //struct file_operations
	mcdev->owner = THIS_MODULE;
	//now that we created cdev, we have to add it to the kernel
	//int cdev_add(struct cdev* dev, dev_t num, unsigned int count)
	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0){
		printk(KERN_ALERT "MYTESTDEVICE: unable to add cdev to kernel\n");
		return ret;
	}
	//step (4) Initialize our semaphore
	sema_init(&virtual_device.sem,1); //initial value of one
	return 0;
}

static void driver_exit(void){
	//(5) unregister everything in reverse order
	//(a)
	cdev_del(mcdev);

	//(b)
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "MYTESTDEVICE: unloaded module\n");
}

//Inform the kernel where to start and stop with our module/driver
module_init(driver_entry);
module_exit(driver_exit);
