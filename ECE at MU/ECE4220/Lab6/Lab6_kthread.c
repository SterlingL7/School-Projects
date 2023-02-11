/* Based on example from: http://tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html
   Modified and commented by: Luis Rivera			
   
   Compile using the Makefile
*/

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
   
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
//for cdev
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define MSG_SIZE 50
#define CDEV_NAME "Lab6"

// structure for the kthread.
static struct task_struct *kthread1;

static int major; 
static char msg[MSG_SIZE];

unsigned long *gpfsel0;
unsigned long *gparen0;
unsigned long *gppud;
unsigned long *gpeds0;
char Case = '0';
int mydev_id;	// variable needed to identify the handler


static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
	// Whatever is in msg will be placed into buffer, which will be copied into user space
	ssize_t dummy = copy_to_user(buffer, msg, length);	// dummy will be 0 if successful
	
	
	// msg should be protected (e.g. semaphore). Not implemented here, but you can add it.
	msg[0] = '\0';	// "Clear" the message, in case the device is read again.
					// This way, the same message will not be read twice.
					// Also convenient for checking if there is nothing new, in user space.
	
	return length;
}

// Function called when the user space program writes to the Character Device.
// Some arguments not used here.
// buff: data that was written to the Character Device will be there, so it can be used
//       in Kernel space.
// In this example, the data is placed in the same global variable msg used above.
// That is not needed. You could place the data coming from user space in a different
// string, and use it as needed...
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
	ssize_t dummy;
	
	if(len > MSG_SIZE)
		return -EINVAL;
	
	// unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
	dummy = copy_from_user(msg, buff, len);	// Transfers the data from user space to kernel space
	if(len == MSG_SIZE)
		msg[len-1] = '\0';	// will ignore the last character received.
	else
		msg[len] = '\0';
	
	// You may want to remove the following printk in your final version.
	printk("Message from user space: %s\n", msg);
	
	if(msg[0] == 'A') Case = 'A';
	else if(msg[0] == 'B') Case = 'B';
	else if(msg[0] == 'C') Case = 'C';
	else if(msg[0] == 'D') Case = 'D';
	else if(msg[0] == 'E') Case = 'E';
	//Case = msg[0];

	return len;		// the number of bytes that were written to the Character Device.
}

// structure needed when registering the Character Device. Members are the callback
// functions when the device is read from or written to.
static struct file_operations fops = {
	.read = device_read, 
	.write = device_write,
};

// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.
static irqreturn_t button_isr(int irq, void *dev_id)
{
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);
	
	switch(ioread32(gpeds0) & 0x1F0000){
		case 0x100000:
			
			Case = 'A';
			break;
		case 0x080000:
			
			Case = 'B';
			break;
		case 0x040000:
			
			Case = 'C';
			break;
		case 0x020000:
			
			Case = 'D';
			break;
		case 0x010000:
			
			Case = 'E';
			break;
	}
	printk("button pressed %c", Case);

	// IMPORTANT: Clear the Event Detect status register before leaving.	
	iowrite32(0xFFFFFFFF, gpeds0);
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
	int halfpd = 100000;
	printk("In kthread1\n");

	//printk("Before loop\n");
	//printk("%p\n", gpfsel0);
	// The ktrhead does not need to run forever. It can execute something
	// and then leave.
	while(1){
		switch(Case){
			case 'A':
				halfpd = 500;
				break;
			case 'B':
				halfpd = 600;
				break;
			case 'C':
				halfpd = 700;
				break;
			case 'D':
				halfpd = 800;
				break;
			case 'E':
				halfpd = 900;
				break;
			default:
				break;
		}
		//printk("case\n");

		udelay(halfpd);	// good for > 10 ms
		iowrite32(0x40, gpfsel0+7);
		udelay(halfpd);	// good for > 10 ms
		//msleep_interruptible(1000); // good for > 10 ms
		//udelay(unsigned long usecs);	// good for a few us (micro s)
		//usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
		iowrite32(0x40, gpfsel0+10);
		
		
		// In an infinite loop, you should check if the kthread_stop
		// function has been called (e.g. in clean up module). If so,
		// the kthread should exit. If this is not done, the thread
		// will persist even after removing the module.
		if(kthread_should_stop()) {
			do_exit(0);
		}
	}
	return 0;
}

int init_module(void)
{

	int isr_dummy = 0;
	gpfsel0 = (unsigned long *) ioremap(0x3f200000, 4096);//gpio function select register
	gpeds0 = gpfsel0 + (0x40/4); //gpio event detect status register
	gparen0 = gpfsel0 + (0x7c/4); //gpio async rising edge detect enable register
	gppud = gpfsel0 + (0x94/4); //gpio pull-up/down register

	iowrite32(0x0, gpfsel0 + 1);//ensure buttons 2,3,4 are inputs
	iowrite32(0x0, gpfsel0 + 2);//ensure button 5 is input
	iowrite32(0x040000, gpfsel0); //set speaker to output
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x1F0000, gparen0);	
	
	iowrite32(0x155, gppud);//configure pull down resistors
	udelay(100); //delay before writing again
	iowrite32(0x1F0000, gppud + 1);
	udelay(100);
	
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	isr_dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	printk("Button Detection enabled.\n");
	char kthread_name[11]="my_kthread";	// try running  ps -ef | grep my_kthread
						// when the thread is active.
	printk("In init module\n");

	major = register_chrdev(0, CDEV_NAME, &fops);
	if (major < 0) {
     		printk("Registering the character device failed with %d\n", major);
	     	return major;
	}

	printk("character device, assigned major: %d\n", major);
	printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);

    kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
    if((kthread1))	// true if kthread creation is successful
    {
        printk("Inside if\n");
		// kthread is dormant after creation. Needs to be woken up
        wake_up_process(kthread1);
    }

    return 0;
}

void cleanup_module(void) {
	int ret;

	unregister_chrdev(major, CDEV_NAME);
	// the following doesn't actually stop the thread, but signals that
	// the thread should stop itself (with do_exit above).
	// kthread should not be called if the thread has already stopped.
	iowrite32(0x0, gpeds0);
	iowrite32(0x40, gpfsel0 + 10);//gpset = 0 to turn off speaker
	udelay(1000);
	iowrite32(0x0, gpfsel0); //set LEDs back to input
	iowrite32(0x0, gppud);//reset pull down resistors
	udelay(100); //delay before writing again
	iowrite32(0x0, gppud + 1);
	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x0, gparen0);
	iowrite32(0x0, gppud);
	iowrite32(0x0, gppud + 1);

	// Remove the interrupt handler; you need to provide the same identifier
	free_irq(79, &mydev_id);

	ret = kthread_stop(kthread1);
								
	if(!ret)
		printk("Kthread stopped\n");
}


