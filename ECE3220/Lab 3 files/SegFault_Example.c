#include <stdio.h>
#include <string.h>
#include <math.h>

void tester(int* c, int k) {
	printf("x[%d] = %d\n", k, c[k]);
	c[k]=1309;
}

int main(int argc, char* argv[]) {
	int i, j, k;
	int x[1000];
	
	while(i < 1000){
		x[i] = i;
		i++;
	}

    while (1) {
        printf("\nenter an integer between 0 to 999 (ctrl+c to quit): ");
        fflush(stdout);
        scanf("%d",&k);
		
	while(k < 0 || k > 999)
		{
			printf("that is not valid. Must be between 0 and 999: ");
			scanf("%d",&k);
		}
        tester(x, k);
    }

    return 0;
}
