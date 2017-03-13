/* Initialization of the module */
#include <linux/init.h> 
/* To recognize that this is a module */
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/mutex.h> 

/*
Defining Semaphores
*/
static DEFINE_SEMAPHORE(empty);
static DEFINE_SEMAPHORE(full);
static DEFINE_SEMAPHORE(mutex);

MODULE_LICENSE("DUAL BSD/GPL");
/*
Input Parameter for Buffer length/size of character device
*/
int bufferLimit = 0;
module_param(bufferLimit, int, S_IRUSR | S_IWUSR);

/*
Function declarations
*/
static ssize_t mypipe_read(struct file *, char __user *, size_t , loff_t *);
static int mypipe_open(struct inode *,struct file *);
static int mypipe_close(struct inode *,struct file *);
ssize_t mypipe_write(struct file *, const char __user * , size_t , loff_t *);

char *devbuffer;
static int tail=0;

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = mypipe_open,
	.read = mypipe_read,
	.write = mypipe_write,
	.release = mypipe_close
};

static struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mypipe",
	.fops = &my_fops
};

/*
static function since we need to use its instance of this function.
*/
static int mypipe_open(struct inode *inode,struct file *file){ 
	printk(KERN_ALERT "mypipe open successful\n");
	return 0;
}

static int mypipe_close(struct inode *inode,struct file *file)
{
	printk(KERN_ALERT "mypipe close successful\n");	
	return 0;
}

static ssize_t mypipe_read(struct file *file, char __user * buffer, size_t size, loff_t * off) {
	
	int result;
	
	if( down_interruptible(&full) < 0){ /* Decrement full by 1 and notify blocked producers. Block consumer if full is 0. */
		printk("Consumer Exit.. Error:1. No filled slot in Buffer\n");
		return -1;
	}
	if( down_interruptible(&mutex) < 0){ /* Entered in Critical Section */
		printk("Consumer Exit.. Error:2. No filled slot in Buffer\n");
		return -1;
	}
	result = copy_to_user(buffer,devbuffer, size);
	if(result<0){
		printk(KERN_ALERT "Error in copy to user\n");		
	}
	else{
		int i;

		for(i=0;i < tail-10; i++)
		{
			devbuffer[i]=devbuffer[i+10];
		}
		tail = tail-10;
		printk(KERN_ALERT "Consumer consumed a String. (BUFFER FILLED:%d)\n",tail/10);
	}
	up(&mutex); /* EXIT from  Critical Section */
	up(&empty); /* Increment empty by 1 */
	if(result<0){
		return -1;	
	}
	return tail/10; /* Return current buffer status */
}

ssize_t mypipe_write(struct file *file, const char __user * buffer, size_t size, loff_t * off) {
		
	int result;
	if( down_interruptible(&empty) < 0){ /* Decrement empty by 1 and notify blocked consumers. Block producer if empty is 0. */
		printk("Producer Exit.. Error:1. No empty slot in Buffer\n");
		return -1;
	} 
	if( down_interruptible(&mutex) < 0){ /* Entered in Critical Section */
		printk("Producer Exit.. Error:2. No empty slot in Buffer\n");
		return -1;
	}
	result=copy_from_user(devbuffer+tail, buffer, size);
	
	if(result<0){
		printk(KERN_ALERT "Error in copy from user\n");
	}
	else{
		tail = tail+10;
		printk(KERN_ALERT "Producer Produced a String. (BUFFER FILLED:%d)\n", tail/10);
	}
	
	up(&mutex); /* EXIT from  Critical Section */
	up(&full); /* Increment full by 1 */
	if(result<0){
		return -1;	
	}
	return tail/10;	/* Return current buffer status */
}

int __init kernel_module_init(void) { 
	int regStat = misc_register(&my_misc_device);
	if(regStat == 0) /* Check for register status */
	{
		printk(KERN_ALERT "Character Device Registered\n");	
	}	
	else
	{
		return regStat;	
	}
	sema_init(&full, 0); 
    	sema_init(&empty, bufferLimit); 
    	sema_init(&mutex, 1); 	
	/* Allocating memory as required at runtime */
	devbuffer = kmalloc(10*bufferLimit, GFP_KERNEL);	
	if (!devbuffer){
		printk(KERN_ALERT "Error in allocating memory using kmalloc\n");
		return 0;
	}
	printk(KERN_ALERT "Kernel Module init successfully\n");
	return 0;
}

void __exit kernel_module_exit(void) {
	misc_deregister(&my_misc_device);
	printk(KERN_ALERT "Character Devided Deregisted\n");
	printk(KERN_ALERT "Kernel Module exit successfully\n");
	kfree(devbuffer);
}

module_init(kernel_module_init); //To tell kernel to start executing linux module. Whenever start this module jump execution to hello_init function.
module_exit(kernel_module_exit); //Whenever module is removed or execution is done jump execution to hello_exit function and deallocate any resources used.
