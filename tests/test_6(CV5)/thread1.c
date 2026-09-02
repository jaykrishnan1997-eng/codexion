//  from Unix thread in C by CodeVault
// in prev exercises we had too many create and join function
// here we do what we need for a N thread

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
    // array to store 4 thread handles
    int N = 4;
    int i;
    pthread_t th[N];
    // initializing mutex
    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < N; i++){
        // &th[i] == th + i
        if (pthread_create(&th[i], NULL, &routine, NULL))
            return 1;
        printf("Thread %d have started\n", i);
    }
    for (i = 0; i < N; i++){
        if(pthread_join(th[i], NULL))
            return 2;
        printf("Thread %d have ended\n", i);
    }

    // destroying mutex
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);
    return 0;
}
// gcc -S thread.c
// to see what each line does behind