#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <time.h>
#include <pthread.h>
#include <wiringPi.h>
#include <semaphore.h>

#define _GNU_SOURCE

void* light1(void* arg);
void* light2(void* arg);
void* plight(void* arg);

int fd;
uint32_t *mptr;
uint32_t *gpeds;

static sem_t mysem;

struct Args{
	struct sched_param* p;
	pthread_t threadid[3];
	
};

int main(){
	if(sem_init(&mysem, 0, 1) == -1){
		printf("sem init failed");
		return 0;
	};
	
	int val;
	sem_getvalue(&mysem, &val);
	printf("%d- ", val);

	wiringPiSetupGpio();
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(16, INPUT);
	struct Args args;
	struct Args* a = &args;
	struct sched_param pa;
	args.p = &pa;
	pa.sched_priority = 51;	

	fd = open("/dev/mem", O_RDWR|O_SYNC);
	mptr = mmap(NULL,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,fd, 0x3f200000);
	gpeds = mptr + 16; //16 = 0x40/4 = 0x10
	

	int err = 0;

	err = pthread_create(&args.threadid[0], NULL, light1, (void*) a);
	//printf("%d", args.threadid[0]);

	err = pthread_create(&args.threadid[1], NULL, light2, (void*) a);
	if(err) printf("thread failed to init\n");

	err = pthread_create(&args.threadid[2], NULL, plight, (void*) a);
	if(err) printf("thread failed to init\n");
	

	pthread_join(args.threadid[0], NULL);
	pthread_join(args.threadid[1], NULL);
	pthread_join(args.threadid[2], NULL);
	return err;
}

void* light1(void* arg){
	struct Args* a = arg;
	a->p->sched_priority = 54;
	int er = sched_setscheduler(0, SCHED_FIFO, a->p);
	if(er) printf("priority failed\n");

	while(1){
		if(sem_wait(&mysem) == -1) perror("l1 ");
	int val;
	sem_getvalue(&mysem, &val);
	printf("%d\n", val);
		//perror("e1 ");
		printf("light1\n");
		digitalWrite(2, 1);
		usleep(1000000);
		digitalWrite(2, 0);
		if(sem_post(&mysem) == -1) perror("l1 post ");
	sem_getvalue(&mysem, &val);
	printf("%d:\n\n", val);
		usleep(10000);//prevent retaking sem
	}
}
void* light2(void* arg){
	struct Args* a = arg;
	a->p->sched_priority = 53;
	int er = sched_setscheduler(0, SCHED_FIFO, a->p);
	if(er) printf("priority failed\n");

	while(1){
		if(sem_wait(&mysem) == -1) perror("l1 ");
	int val;
	sem_getvalue(&mysem, &val);
	printf("%d|\n", val);
		printf("light2\n");
		digitalWrite(4, 1);
		usleep(1000000);
		digitalWrite(4, 0);
		if(sem_post(&mysem) == -1) perror("l2 post ");
	sem_getvalue(&mysem, &val);
	printf("%d|\n\n", val);
		usleep(10000);//prevent retaking sem
	}
}
void* plight(void* arg){
	struct Args* a = arg;
	a->p->sched_priority = 99;
	int er = sched_setscheduler(0, SCHED_FIFO, a->p);
	if(er) printf("priority failed\n");

	while(1){
		
		if(*gpeds & 0x00010000){
			if(sem_wait(&mysem) == -1) perror("l1 ");
	int val;
	sem_getvalue(&mysem, &val);
	printf("%d/\n", val);
			printf("light3\n");
			digitalWrite(3, 1);
			sleep(1);
			digitalWrite(3, 0);
			*gpeds = 0x00010000;	
		}
		if(sem_post(&mysem) == -1) perror("l3 post ");
		usleep(10000);
	}	
		
}

