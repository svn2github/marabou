static char *rcsId = "$Header: /misc/hadaq/cvsroot/compat/semaphore.c,v 1.2 2004/08/27 13:00:33 hadaq Exp $";


#define _XOPEN_SOURCE
#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>

#if !(defined(__Lynx__) || (defined(__linux) && __GNUC_MINOR__ < 91))
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

#endif

#define HAVE_LIBCOMPAT
#include "semaphore.h"

static char *name2Path(char *path, char *name)
{
	char *prefix;

	if (NULL == (prefix = getenv("OUR_SEMAPHORE_PREFIX"))) {
		prefix = "";
	}
	if (strlen(prefix) + strlen(name) + 1 > _POSIX_PATH_MAX) {
		errno = ENAMETOOLONG;
		path = NULL;
	} else {
		strcpy(path, prefix);
		strcat(path, name);
	}
	return path;
}

sem_t *SEMAPHORE_sem_open(char *name, int oflags, mode_t mode, unsigned int value)
{
	int *sem;
	char path[_POSIX_PATH_MAX];
	key_t key;
	int semflg;
	int fd;
	union semun arg;

	if (NULL == name2Path(path, name)) {
		return (sem_t *) -1;
	}
	if (0 > (fd = open(path, oflags, mode))) {
		return (sem_t *) -1;
	}
	if ((key_t) - 1 == (key = ftok(path, 'A'))) {
		close(fd);
		return (sem_t *) -1;
	}
	close(fd);

	if (NULL == (sem = malloc(sizeof(int)))) {
		return (sem_t *) -1;
	}
	semflg = mode;				/* BUGBUG This depends on ipc modes having
                                   the same */
	/* semantics as file modes */
	if ((oflags & O_CREAT) != 0) {
		semflg |= IPC_CREAT;
	}
	if ((oflags & O_EXCL) != 0) {
		semflg |= IPC_EXCL;
	}
	if (semget(key, 1, 0) != -1) {	/* semaphore existst already */
		if (0 > (*sem = semget(key, 1, semflg))) {
			free(sem);
			return (sem_t *) -1;
		}
	} else {
		arg.val = value;
		if (0 > (*sem = semget(key, 1, semflg))
			|| 0 > semctl(*sem, 0, SETVAL, arg)
			) {
			free(sem);
			return (sem_t *) -1;
		}
	}
	return sem;
}

int SEMAPHORE_sem_close(sem_t *sem)
{
	free(sem);
	return 0;
}

int SEMAPHORE_sem_unlink(char *name)
{
	int retVal;
	sem_t semS, *sem = &semS;
	char path[_POSIX_PATH_MAX];
	key_t key;
	union semun arg;

	retVal = 0;

	arg.val = 0;
	if (
		   NULL == name2Path(path, name)
		   || (key_t) - 1 == (key = ftok(path, 'A'))
		   || 0 > (*sem = semget(key, 1, 0))
		   || 0 > semctl(*sem, 0, IPC_RMID, arg)
		) {
		retVal = -1;
	}
	if (-1 == unlink(path)) {
		retVal = -1;
	}
	return retVal;
}

int SEMAPHORE_sem_wait(sem_t *sem)
{
	static struct sembuf waitOpS =
	{0, -1, 0}, *waitOp = &waitOpS;

	return semop(*sem, waitOp, 1);
}

int SEMAPHORE_sem_trywait(sem_t *sem)
{
	static struct sembuf trywaitOpS =
	{0, -1, IPC_NOWAIT}, *trywaitOp = &trywaitOpS;

	return semop(*sem, trywaitOp, 1);
}

int SEMAPHORE_sem_post(sem_t *sem)
{
	static struct sembuf postOpS =
	{0, +1, 0}, *postOp = &postOpS;

	return semop(*sem, postOp, 1);
}
