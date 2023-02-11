/*Lab 1 part 1
* Sterling L
* spl2q2
* 14096504
*
* basic module to turn on and off GPIO pins
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

static int __init light_init_module(void){
	
	unsigned long *gpfsel0 = (unsigned long*) ioremap(0x3f200000, 4096);
	iowrite32(0x9240, gpfsel0); //001001001001000000 set pins 2-5 to output
	iowrite32(0x3c, gpfsel0+7); //set bits 2-5 to 1 turning on lights
	gpfsel0 = NULL;
	printk("Lights on");
	return 0;
}

static void __exit light_cleanup_module(void){
	unsigned long *gpfsel0 = (unsigned long*) ioremap(0x3f200000, 4096);
	iowrite32(0x3c, gpfsel0 + 10); //set bits 2-5 to 0 turning off lights
	//gpclr0 = NULL;
	printk("lights off");
}

module_init(light_init_module);
module_exit(light_cleanup_module);
