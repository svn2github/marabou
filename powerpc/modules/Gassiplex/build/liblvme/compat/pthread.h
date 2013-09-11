/* $Header$ */
#ifndef OUR_PTHREAD_H
#define OUR_PTHREAD_H

#ifdef HAVE_LIBCOMPAT

/*
Translate pthread calls conforming to the final POSIX standard (1003.1c)
to the older draft version 1003.4b.  The latter one is e.g. used by
LynxOS 2.5.  The translation is done according to Appendix B in
"Nichols, Buttlar, Farrell: Pthreads Programming (O'Reilly)".
*/

#include </usr/include/pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Detaching a Thread */
#ifdef pthread_detach
#undef pthread_detach
#endif
#define pthread_detach PTHREAD_pthread_detach

/* Mutex Variables */
#ifdef pthread_mutex_init
#undef pthread_mutex_init
#endif
#define pthread_mutex_init PTHREAD_pthread_mutex_init

/* Condition Variables */
#ifdef pthread_cond_init
#undef pthread_cond_init
#endif
#define pthread_cond_init PTHREAD_pthread_cond_init

/* Thread Attributes */
#ifdef pthread_create
#undef pthread_create
#endif
#define pthread_create PTHREAD_pthread_create

#ifdef pthread_attr_init
#undef pthread_attr_init
#endif
#define pthread_attr_init PTHREAD_pthread_attr_init

#ifdef pthread_attr_destroy
#undef pthread_attr_destroy
#endif
#define pthread_attr_destroy PTHREAD_pthread_attr_destroy

/* Initializes a static mutex with default attributes */
#ifdef PTHREAD_MUTEX_INITIALIZER
#undef PTHREAD_MUTEX_INITIALIZER
#endif
#define PTHREAD_MUTEX_INITIALIZER {1, 0, 0, NULL}

/* The pthread_once Function */
#define PTHREAD_ONCE_INIT pthread_once_init

/* Keys */
#ifdef pthread_key_create
#undef pthread_key_create
#endif
#define pthread_key_create PTHREAD_pthread_key_create

#ifdef pthread_key_delete
#undef pthread_key_delete
#endif
#define pthread_key_delete PTHREAD_pthread_key_delete

#ifdef pthread_getspecific
#undef pthread_getspecific
#endif
#define pthread_getspecific PTHREAD_pthread_getspecific

int PTHREAD_pthread_detach(pthread_t thread);
int PTHREAD_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int PTHREAD_pthread_cond_init(pthread_cond_t *mutex, const pthread_condattr_t *condattr);
int PTHREAD_pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
int PTHREAD_pthread_attr_init(pthread_attr_t *attr);
int PTHREAD_pthread_attr_destroy(pthread_attr_t *attr);
int PTHREAD_pthread_key_create(pthread_key_t *key, void (*destr_function) (void *));
int PTHREAD_pthread_key_delete(pthread_key_t key);
void *PTHREAD_pthread_getspecific(pthread_key_t key);

#ifdef __cplusplus
}
#endif

#else
#include </usr/include/pthread.h>
#endif

#endif
