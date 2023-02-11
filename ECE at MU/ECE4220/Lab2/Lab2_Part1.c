#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <pthread.h>

struct values {
    int matrix_dim[2];
    int *matrix;
    int *out;
    int *kernel;
    int x,y;//row number x and element number y
};

void *conv(void* args);	//process convolution via loop
void *convRow(void* args); //process covolution with 1 thread per row
void *convElement(void *args); //process convolution with 1 thread per element
 

int main(int argc, char* argv[]){


	//load matrix info into arrays
	FILE *fp;
	
	struct values args;

    if(argc == 2){
		fp = fopen(argv[1], "r");
		if(!fp){
			printf("file failed to open\n");
			exit(-1);
		}
	}
	else{
		printf("format: ./Lab2_Part1 <matrix_file>\n");
		exit(-1);
	}
	//get matrix rows and columns
	fscanf(fp, "%d", &args.matrix_dim[0]);
	fscanf(fp, "%d", &args.matrix_dim[1]);

	//create matrix using dimensions, point to it from args
	int matrix[args.matrix_dim[0]][args.matrix_dim[1]];
	args.matrix = &matrix[0][0];
	
	//different convolution matrices for each test
	int conv1[args.matrix_dim[0]][args.matrix_dim[1]];
	int conv2[args.matrix_dim[0]][args.matrix_dim[1]];
   	int conv3[args.matrix_dim[0]][args.matrix_dim[1]];
	
	//point to output 1 from args
   	args.out = &conv1[0][0];
	
	//fill values from file
	for(int i = 0; i < args.matrix_dim[0]; ++i){
		for(int j = 0; j < args.matrix_dim[1]; ++j){
			fscanf(fp, "%d", &matrix[i][j]);
			printf("%d\t", matrix[i][j]); 
		}
		printf("\n");
	}
	
	//kernel is only 1x3 in this lab
	//load kernel dimensions
	int kernel_dim[2];
	fscanf(fp, "%d", &kernel_dim[0]);
	fscanf(fp, "%d", &kernel_dim[1]);
	

	//create kernel using the dimensions
	int kernel[kernel_dim[0]][kernel_dim[1]];
	args.kernel = &kernel[0][0];
	//fill values from file
	for(int i = 0; i < kernel_dim[0]; ++i){
		for(int j = 0; j < kernel_dim[1]; ++j){
			fscanf(fp, "%d", &kernel[i][j]);
			printf("%d\t", kernel[i][j]); 
		}
		printf("\n\n");
	}

    //make void pointer to args for thread arguments
    struct values* pt = &args;
    pthread_t tid;
    //process singly threaded covolution
	clock_t t = clock();
	pthread_create(&tid, NULL, conv, (void*)pt);
	pthread_join(tid, NULL);
	t = clock() - t;

	for(int i = 0; i < args.matrix_dim[0]; ++i){
		for(int j = 0; j < args.matrix_dim[1]; ++j){
			printf("%d\t", conv1[i][j]);
			if(j == args.matrix_dim[1] - 1) printf("\n");
		}
	}
    printf("%ld time for single thread calculation\n\n", t);
	
	
	args.out = &conv2[0][0];
    //convolution via 1 thread per row
	//create matrix_dim[0] threads, loop through rows
	t = clock();
	
    pthread_t row_threads[args.matrix_dim[0]];
	int ret;
	args.x = 0;
	args.y = 0;
	for(int i = 0; i < args.matrix_dim[0]; ++i){
		ret = pthread_create(&row_threads[i], NULL, convRow, (void*)pt);
		if(ret){
			printf("creation failed error: %d\n", ret);
			exit(-1);
		}
	}
    
    for(int i = 0; i < args.matrix_dim[0]; ++i){
		pthread_join(row_threads[i], NULL);
	}
	
    t = clock() - t;

	for(int i = 0; i < args.matrix_dim[0]; ++i){
		for(int j = 0; j < args.matrix_dim[1]; ++j){
			printf("%d\t", conv2[i][j]);
			if(j == args.matrix_dim[1] - 1) printf("\n");
		}
	}
    printf("%ld time for thread per row calculation\n\n", t);
	
	//via 1 thread per element
	//create matrix_dim[0] * matrix_dim[1] threads
	//change output target for 3rd processing
	args.out = &conv3[0][0];
	args.x = 0;
	
	t = clock();
	
	pthread_t ele_threads[args.matrix_dim[0]][args.matrix_dim[1]];
	

	for(int i = 0; i < args.matrix_dim[0]; ++i){
        for(int j = 0; j < args.matrix_dim[1]; ++j){
            ret = pthread_create(&ele_threads[i][j], NULL, convElement, (void*)pt);
            if(ret){
                printf("creation failed error: %d\n", ret);
                exit(-1);
            }
        }
	}
	for(int i = 0; i < args.matrix_dim[0]; ++i){
		for(int j = 0; j < args.matrix_dim[1]; ++j){
			pthread_join(ele_threads[i][j], NULL);
		}
	}
	
	t = clock() - t;

	for(int i = 0; i < args.matrix_dim[0]; ++i){
		for(int j = 0; j < args.matrix_dim[1]; ++j){
			printf("%d\t", conv3[i][j]);
			if(j == args.matrix_dim[1] - 1) printf("\n");
		}
	}
	printf("%ld time for thread per row calculation\n", t);
    
	fclose(fp);
	pthread_exit(NULL);
	return 0;
}


void* conv(void* arg){	//process convolution via loop
	struct values* args = (struct values*)arg;

	for(int i = 0; i < args->matrix_dim[0]*args->matrix_dim[1]; ++i){

		if(i == 0 || i == 10){// if first element, do not process j-1 element
			*(args->out + i) = 
				*(args->matrix + i) * args->kernel[1] +
				*(args->matrix + i + 1) * args->kernel[2];
		}
		else if((i == 9) || (i == 19)){// if last element, do not process j+1 element
			*(args->out + i) = 
				*(args->matrix + i - 1) * args->kernel[0] +
				*(args->matrix + i) * args->kernel[1];
		}
		else{
			*(args->out + i) = 
				*(args->matrix + i - 1) * args->kernel[0] +
				*(args->matrix + i) * args->kernel[1] +
				*(args->matrix + i + 1) * args->kernel[2];
		}

	}
	pthread_exit(NULL);
}

void* convRow(void* arg){ //process covolution with 1 thread per row
	struct values* args = (struct values*)arg;

	for(int i = 0; i < args->matrix_dim[1]; ++i){
		if(i == 0){
            *(args->out + args->x*args->matrix_dim[1] + i) = 
			*(args->matrix + args->x*args->matrix_dim[1] + i)*args->kernel[1] + 
            		*(args->matrix + args->x*args->matrix_dim[1] + i + 1)*args->kernel[2];
        }
		else if(i == (args->matrix_dim[1] - 1)){
            *(args->out + args->x*args->matrix_dim[1] + i) = 
			*(args->matrix + args->x*args->matrix_dim[1] + i - 1)*args->kernel[0] + 
            		*(args->matrix + args->x*args->matrix_dim[1] + i)*args->kernel[1];
        }
        else{
            *(args->out + args->x*args->matrix_dim[1] + i) = 
			*(args->matrix + args->x*args->matrix_dim[1] + i - 1)*args->kernel[0] + 
            		*(args->matrix + args->x*args->matrix_dim[1] + i)*args->kernel[1] + 
            		*(args->matrix + args->x*args->matrix_dim[1] + i + 1)*args->kernel[2];
        }
	}
	args->x++;
	pthread_exit(NULL);
}

void* convElement(void* arg){ //process convolution with 1 thread per element
	struct values* args = (struct values*)arg;
    if(args->y == 0){
            *(args->out + args->x*args->matrix_dim[1] + args->y) = 
			*(args->matrix + args->x*args->matrix_dim[1] + args->y)*args->kernel[1] + 
           	*(args->matrix + args->x*args->matrix_dim[1] + args->y + 1)*args->kernel[2];

			args->y++;

    }
    else if(args->y == (args->matrix_dim[1] - 1)){
            *(args->out + args->x*args->matrix_dim[1] + args->y) = 
			*(args->matrix + args->x*args->matrix_dim[1] + args->y - 1)*args->kernel[0] + 
           	*(args->matrix + args->x*args->matrix_dim[1] + args->y)*args->kernel[1];

			args->y = 0;
			args->x++;
			
    }
    else{
            *(args->out + args->x*args->matrix_dim[1] + args->y) = 
			(*(args->matrix + args->x) + args->y - 1)*args->kernel[0] + 
           	 	*(args->matrix + args->x*args->matrix_dim[1] + args->y)*args->kernel[1] + 
           		*(args->matrix + args->x*args->matrix_dim[1] + args->y + 1)*args->kernel[2];

				args->y++;
    }
    pthread_exit(NULL);
}
