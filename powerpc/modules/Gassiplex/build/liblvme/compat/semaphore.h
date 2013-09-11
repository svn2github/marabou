/* $Header: /misc/hadaq/cvsroot/compat/semaphore.h,v 1.5 2005/03/10 14:53:03 hadaq Exp $ */
#ifndef OUR_SEMAPHORE_H
#define OUR_SEMAPHORE_H

#ifdef HAVE_LIBCOMPAT

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sem_t;

#define sem_open SEMAPHORE_sem_open
#define sem_close SEMAPHORE_sem_close
#define sem_unlink SEMAPHORE_sem_unlink
#define sem_wait SEMAPHORE_sem_wait
#define sem_trywait SEMAPHORE_sem_trywait
#define sem_post SEMAPHORE_sem_post

sem_t *SEMAPHORE_sem_open(char *name, int oflags, mode_t mode, unsigned int value);
int SEMAPHORE_sem_close(sem_t * sem);
int SEMAPHORE_sem_unlink(char *name);
int SEMAPHORE_sem_wait(sem_t * sem);
int SEMAPHORE_sem_trywait(sem_t * sem);
int SEMAPHORE_sem_post(sem_t * sem);

#ifdef __cplusplus
}
#endif

#else
#include </usr/include/semaphore.h>
#endif

#endif
