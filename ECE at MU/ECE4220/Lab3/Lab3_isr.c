
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

unsigned long *gpfsel0;
unsigned long *gparen0;
unsigned long *gppud;
unsigned long *gpeds0;

int mydev_id;

static irqreturn_t button_isr(int irq, void *dev_id){
	disable_irq_nosync(79);

	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

int init_module(){

	int dummy = 0;
	// Map GPIO registers
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers
	gpfsel0 = (unsigned long *) ioremap(0x3f200000, 4096); //gpio select registers
	gparen0 = gpfsel0 + (0x88/4); //gpio async rising edge detect enable register
	gppud = gpfsel0 + (0x94/4); //gpio pull-up/down register
	gpeds0 = gpfsel0 + (0x40/4); //gpio event detect status register
	
	// Don't forget to configure all ports connected to the push buttons as inputs.
	// button 1-5 = BCM 16-20 need to be inputs
	// LEDs = BCM 2-5 need to be outputs	
	iowrite32(0x9240, gpfsel0); //set LEDs to output
	iowrite32(0x0, gpfsel0 + 1);//ensure buttons 2,3,4 are inputs
	iowrite32(0x0, gpfsel0 + 2);//ensure button 5 is input
	iowrite32(0x3c, gpfsel0 + 10);//gpset = 0 to turn off all lights

	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x1F0000, gparen0);		
	
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	//printk("Button Detection enabled.\n");

	return 0;
}

void cleanup_module(){
	// Good idea to clear the Event Detect status register here, just in case.
	iowrite32(0x0, gpeds0);
	iowrite32(0x3c, gpfsel0 + 10);//gpset = 0 to turn off all lights
	udelay(1000);
	iowrite32(0x0, gpfsel0); //set LEDs back to input

	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x0, gparen0);
	iowrite32(0x0, gppud);
	iowrite32(0x0, gppud + 1);
	free_irq(79, &mydev_id);
	
	//printk("Button Detection disabled.\n");
}
