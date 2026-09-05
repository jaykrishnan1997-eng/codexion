
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel = 0;

void *fuel_filling(void *arg) {
	(void)arg;
	for (int i = 0; i < 5; i++) {
		pthread_mutex_lock(&mutexFuel);
		fuel += 15;
		printf("Fillled fuel... %d\n", fuel);
		pthread_mutex_unlock(&mutexFuel);
		// says to all waiting (wait, conFuel) thread on this condition
		// start running the wait 
		pthread_cond_signal(&condFuel);
		sleep(1);
	}
	return NULL;
}

void *car(void *arg){
	(void)arg;
	pthread_mutex_lock(&mutexFuel);
	while (fuel < 40) {
		printf("No fuel. Waiting...\n");
		// this comand wait for a signal from another thread
		// that may or may not change the condition
		// this guy is triggered when he recieves signal
		pthread_cond_wait(&condFuel, &mutexFuel);
		// Equivalent to :
		// pthread_mutex_unlock(&mutexFuel);
		// wait for signal on condFuel
		// pthread_mutex_lock(&mutexFuel);

	}
	fuel -= 40;
	printf("Got fuel. Now left: %d\n", fuel);
	pthread_mutex_unlock(&mutexFuel);
	return NULL;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	pthread_t th[2];
	pthread_mutex_init(&mutexFuel, NULL);
	pthread_cond_init(&condFuel, NULL);

	for (int i = 0; i < 2; i++) {
		if (i == 1) {
			if (pthread_create(&th[i], NULL, &fuel_filling, NULL) != 0) {
				perror("Failed to create thread");
			}
		} else {
			if (pthread_create(&th[i], NULL, &car, NULL) != 0) {
				perror("Failed to create thread");
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		if (pthread_join(th[i], NULL) != 0) {
			perror("Failed to join thread");
		}
	}
	pthread_mutex_destroy(&mutexFuel);
	pthread_cond_destroy(&condFuel)
	return 0;
}
// condition operaator has 3 operation:
// 1. pthread_cond_wait
// : wait on the condition
// 2. pthread_cond_broadcast
// : broadcast on those waiters
// 3. pthread_cond_signal
// : signal on those waiters