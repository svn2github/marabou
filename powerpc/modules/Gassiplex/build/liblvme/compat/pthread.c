static const char rcsId[] = "$Header$";

#define _POSIX_C_SOURCE 199506L
#include <unistd.h>

#include </usr/include/pthread.h>

int PTHREAD_pthread_detach(pthread_t thread)
{
	return pthread_detach(&thread);
}

int PTHREAD_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	return pthread_mutex_init(mutex, mutexattr == NULL ? pthread_mutexattr_default : mutexattr);
}

int PTHREAD_pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *condattr)
{
	return pthread_cond_init(cond, condattr == NULL ? pthread_condattr_default : condattr);
}

int PTHREAD_pthread_attr_init(pthread_attr_t *attr)
{
	pthread_attr_create(attr);
}

int PTHREAD_pthread_attr_destroy(pthread_attr_t *attr)
{
	pthread_attr_delete(attr);
}

int PTHREAD_pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
	pthread_attr_t P_attr;

	P_attr = attr == NULL ? pthread_attr_default : *attr;
	return pthread_create(thread, P_attr, start_routine, arg);
}

int PTHREAD_pthread_key_create(pthread_key_t *key, void (*destr_function) (void 
*))
{
	return pthread_keycreate(key, destr_function);
}

int PTHREAD_key_delete(pthread_key_t key)
{
	return 0;
}

void *PTHREAD_pthread_getspecific(pthread_key_t key)
{
	void *pointer;

    return -1 == pthread_getspecific(key,  &pointer) ? NULL : pointer;
}

