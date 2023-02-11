



int matrix[m][n]; //initial matrix
int kernel[3]; //1x3 kernel
int convolution[m][n]; //output convolved matrix
int xtime = 0; // keep track of processing time
int num_ops = 0; // keep track of number of operations

for (int i = 0; i < m; ++i){    // loop through every element of input 
    for (int j = 0; j < n; ++j){// matrix and apply filter
        if(j == 0){ // if column 1, do not use j - 1
            convolution[i][j] = convolution[i][j] * kernel[1] +
                                convolution[i][j+1] * kernel[2];
        }
        else if(j == n - 1){ //if leftmost column, do not use j + 1
            convolution[i][j] = convolution[i][j-1] * kernel[0] +
                                convolution[i][j] * kernel[1];
        }
        else{ // non edge cases
            convolution[i][j] = convolution[i][j-1] * kernel[0] +
                                convolution[i][j] * kernel[1] + 
                                convolution[i][j+1] * kernel[2];
        }
        ++num_ops; //increment number of operations on every inner loop pass
    }
}

printf("*******Results*******\n");
printf("time taken: %d\n", xtime);
printf("number of operations: %d\n\n", num_ops); 
printf("result matrix: \n");
for (int i = 0; i < m; ++i){ //loop through array and print 1 row per line
    for (int j = 0; j < n; ++j){
        printf("%d ", convolution[i][j];
    }
    printf("\n");
}
