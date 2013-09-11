#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void *thrTest0(void *arg)
{
	struct timespec tS, *t = &tS;

	t->tv_sec = rand() / (RAND_MAX / 3);
	t->tv_nsec = 0;
	nanosleep(t, NULL);

	puts("thread 0");
	return NULL;
}

void *thrTest1(void *arg)
{
	struct timespec tS, *t = &tS;

	t->tv_sec = rand() / (RAND_MAX / 3);
	t->tv_nsec = 0;
	nanosleep(t, NULL);
	puts("thread 1");
	return NULL;
}

void *thrTest2(void *arg)
{
	struct timespec tS, *t = &tS;

	t->tv_sec = rand() / (RAND_MAX / 3);
	t->tv_nsec = 0;
	nanosleep(t, NULL);
	puts("thread 2");
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t t0;
	pthread_t t1;
	pthread_t t2;
	int i;

	for (i = 0; i < 100; i++) {
		pthread_create(&t0, NULL, thrTest0, NULL);
		pthread_create(&t1, NULL, thrTest1, NULL);
		pthread_create(&t2, NULL, thrTest2, NULL);

		pthread_join(t0, NULL);
		pthread_join(t1, NULL);
		pthread_join(t2, NULL);
	}

	exit(0);
}
