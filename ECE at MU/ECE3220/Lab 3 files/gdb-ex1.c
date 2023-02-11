#include <stdio.h>
 
int main()
{
    char *fileName;
    printf("Specify file name you would like to print to: \n");
    scanf("%s",&fileName);
    
    FILE* file1 = fopen(fileName, "a+");
 
    printf("Press CTRL+d (end-of-file, EOF) to terminate character entry:");
    char c;
    while ((c=getchar()) != EOF)
    {
        fprintf(file1, "%c", c);

    }
 
    fclose(file1);

    return 0;
}
