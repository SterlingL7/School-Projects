/* ece4220lab1_isr.c
 *
 * Sterling Labarbera
 * 14096504
 *
 *Interrupt module detecting button presses
 *buttons 1-4 turn on the LEDs individually
 *button 5 turns off all LEDs
 *
 */

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

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/
unsigned long *gpfsel0;
unsigned long *gparen0;
unsigned long *gppud;
unsigned long *gpeds0;

int mydev_id;	// variable needed to identify the handler

// Interrupt handler function. 
static irqreturn_t button_isr(int irq, void *dev_id)
{
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);

	//switch statement uses bit masking of the event status register to
	//determine which button is pressed

	switch(ioread32(gpeds0) & 0x1F0000){
//check which button is pressed based on gpeds0 register bits 16-20

		case(0x10000): //bit 16 set
			
			//if((ioread32((gpfsel0 + 7)) & 0x4) == 0x4){ //if red LED(pin 2) is on, turn off
			//	iowrite32(0x4, gpfsel0 + 10);
			//}else					//else turn on
			iowrite32(0x4, gpfsel0 + 7);
			printk("button 1 pushed\n");
			break;

		case(0x20000): //bit 17 set
			
			//if((ioread32((gpfsel0 + 7)) & 0x8) == 0x8){ //if yellow LED(pin 3) is on, turn off
			//	iowrite32(0x8, gpfsel0 + 10);
			//}else					//else turn on
			iowrite32(0x8, gpfsel0 + 7);
			printk("button 2 pushed\n");
			break;

		case(0x40000): //bit 18 set
			
			//if((ioread32((gpfsel0 + 7)) & 0x10) == 0x10){ //if green LED(pin 4) is on, turn off
			//	iowrite32(0x10, gpfsel0 + 10);
			//}else					//else turn on
			iowrite32(0x10, gpfsel0 + 7);
			printk("button 3 pushed\n");
			break;

		case(0x80000): //bit 19 set
			
			//if((ioread32((gpfsel0 + 7)) & 0x20) == 0x20){ //if blue LED(pin 5) is on, turn off
			//	iowrite32(0x20, gpfsel0 + 10);
			//}else					//else turn on
			iowrite32(0x20, gpfsel0 + 7);
			printk("button 4 pushed\n");
			break;

		case(0x100000): //bit 20 set
			iowrite32(0x3c, gpfsel0 + 10); //set all LEDs off
			printk("button 5 pushed\n");
			break;
		default:
			//printk("nothing pushed");
			break;
	};
	
	// IMPORTANT: Clear the Event Detect status register before leaving.	
	iowrite32(0xFFFFFFFF, gpeds0);
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

int init_module()
{
	int dummy = 0;

	// Map GPIO registers
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers
	gpfsel0 = (unsigned long *) ioremap(0x3f200000, 4096); //gpio select registers
	gparen0 = gpfsel0 + (0x7c/4); //gpio async rising edge detect enable register
	gppud = gpfsel0 + (0x94/4); //gpio pull-up/down register
	gpeds0 = gpfsel0 + (0x40/4); //gpio event detect status register
	
	// Don't forget to configure all ports connected to the push buttons as inputs.
	// button 1-5 = BCM 16-20 need to be inputs
	// LEDs = BCM 2-5 need to be outputs	
	iowrite32(0x9240, gpfsel0); //set LEDs to output
	iowrite32(0x0, gpfsel0 + 1);//ensure buttons 2,3,4 are inputs
	iowrite32(0x0, gpfsel0 + 2);//ensure button 5 is input
	iowrite32(0x3c, gpfsel0 + 10);//gpset = 0 to turn off all lights

	iowrite32(0x01, gppud); //set to pull up control
	udelay(100);
	iowrite32(0x0, gppud + 1); //set clock signals to appropriate pins in GPPUDCLK0
	udelay(100);
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x1F0000, gparen0);		
	
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	printk("Button Detection enabled.\n");
	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module()
{
	// Good idea to clear the Event Detect status register here, just in case.
	iowrite32(0x0, gpeds0);
	iowrite32(0x3c, gpfsel0 + 10);//gpset = 0 to turn off all lights
	udelay(1000);
	iowrite32(0x0, gpfsel0); //set LEDs back to input

	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x0, gparen0);
	iowrite32(0x0, gppud);
	iowrite32(0x0, gppud + 1);
	// Remove the interrupt handler; you need to provide the same identifier
	free_irq(79, &mydev_id);
	
	printk("Button Detection disabled.\n");
}
