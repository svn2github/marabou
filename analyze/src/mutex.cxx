#include <pthread.h>

// global pthread mutex to protect TMapped data

pthread_mutex_t global_data_mutex = PTHREAD_MUTEX_INITIALIZER;
