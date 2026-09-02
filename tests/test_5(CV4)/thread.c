//  from Unix thread in C by CodeVault
// Using mutex in C and Race condition
// its similar to the use of check-flag in Python, where
// the specific function is executed unless the flag disagrees.
// eg: printing: A -> B -> C etc. after last character dont print "->"
// this lock will make sure a thread is finish in that loop before
// the next thread starts
// here we get double the value, exactly as it is suppposed to work.
// mutex protects the thread from other thread from executing at same time

// without mutex its however faster, so we try to avoid them if possible
// Race condition only happens in a multi-core processor, in a single core
// we never encounter this.

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int mails = 0;
pthread_mutex_t mutex;

void *routine() {
    for (int i = 0; i < 1000000; i++){
        // one fun to set lock to wait and not wait etc.
        // equalent to if lock == 1: wait till lock is 0.
        pthread_mutex_lock(&mutex);
        // this code will be protected by mutex
        mails++;
        // one fun to set the lock to 0, ie unlocking mutex:
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void){
    pthread_t t1, t2, t3, t4;
    // initializing mutex
    pthread_mutex_init(&mutex, NULL);

    if (pthread_create(&t1, NULL, &routine, NULL))
        return 1;

    if (pthread_create(&t2, NULL, &routine, NULL))
        return 2;

    if (pthread_create(&t3, NULL, &routine, NULL))
        return 3;

    if (pthread_create(&t4, NULL, &routine, NULL))
        return 4;

    if(pthread_join(t1, NULL))
        return 5;

    if(pthread_join(t2, NULL))
        return 6;

    if(pthread_join(t3, NULL))
        return 7;

    if(pthread_join(t4, NULL))
        return 8;

    // destroying mutex
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);
    return 0;
}
// gcc -S thread.c
// to see what each line does behind