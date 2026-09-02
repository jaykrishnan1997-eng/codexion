//  from Unix thread in C by CodeVault
//  shared variable incremented with sleep
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void){
    int x = 2;
    int pid = fork();
    if (pid == -1)
        return 1;
    
    if (pid == 0)
        x++;
    sleep(2);
    printf("value of x %d\n", x);
    if (pid != 0)
        wait(NULL);

    return 0;
}