//  from Unix thread in C by CodeVault
// Memory is shared between thread, similar to
// nonlocal memory in python
// we increase iteration to 100, 200, 1000, 100000 but at 
// 1000000 we get not twice but 1073746. this is race Condition.

// in one thread under a for loop we have : read mails, increment,
// and write mails. so under a parallel thread work say t1 and t2,
//  one thread (eg: t2) could wait because the CPU doesnt allow that thread
// the operation continue for t1 muliple times. This breaks the to and fro addition
// resulting in garbage value creation, eg five times t1 and one times t2. 
// in our case we missed nearly 50% of increment. This is race condition.
// for 100 thread all the 100 iteration from thread one will be interwowen 
// with thread 2. No gaps in betweeen
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int mails = 0;

void *routine() {
    for (int i = 0; i < 1000000; i++)
        mails++;
    return NULL;
}

int main(void){
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, &routine, NULL))
        return 1;

    if (pthread_create(&t2, NULL, &routine, NULL))
        return 2;

    if(pthread_join(t1, NULL))
        return 3;

    if(pthread_join(t2, NULL))
        return 4;

    printf("Number of mails: %d\n", mails);
    return 0;
}
// gcc -S thread.c
// to see what each line does behind