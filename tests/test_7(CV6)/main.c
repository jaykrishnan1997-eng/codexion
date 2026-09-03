//  from Unix thread in C by CodeVault
// in prev exercises we had to create, destroy and use threads.
// now we try to get result from them

// pthread_join create a refernce to a pointer or a double pointer
// and it will set the value from within it to whatever we set
// from roll_dice 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void *roll_dice() {

    int *value = malloc(sizeof(int));
    
    if (value == NULL)
        return NULL;

    *value = (rand() % 6) + 1;

    return value;
}

int main(void){

    int* res;

    srand(time(NULL));

    pthread_t th;

    if (pthread_create(&th, NULL, roll_dice, NULL))
        return 1;

    if(pthread_join(th, (void**) &res))
        return 2;

    printf("Result: %d\n", *res);

    free(res);

    return 0;
}
