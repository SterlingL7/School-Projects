#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <time.h>
#include <pthread.h>
#include <wiringPi.h>
#include <semaphore.h>

int main(){

	wiringPiSetupGpio();
	pullUpDnControl(16, PUD_DOWN);
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	uint32_t *mptr = mmap(NULL,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,fd, 0x3f200000);
	uint32_t *gpeds = mptr + 16;

	while(1){
		//light 1		
		digitalWrite(2, 1);
		usleep(1000000);
		digitalWrite(2, 0);
		usleep(100);//prevent retaking sem
		//light 2
		digitalWrite(4, 1);
		usleep(1000000);
		digitalWrite(4, 0);
		usleep(100);//prevent retaking sem
		//check for pedestrian button
		if(*gpeds & 0x00010000){
			digitalWrite(3, 1);
			usleep(1000000);
			digitalWrite(3, 0);
			*gpeds = 0x00010000;
			//clear EDS		
		}
	}

	return 0;
}

