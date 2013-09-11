static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/client/caParam.c,v 1.22 2003/01/31 16:40:17 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include <cadef.h>
#include <tcl.h>

#include "../../include/allParam.h"

#define TIMEOUTEVT 1.0E-09
#define TIMEOUTIO 10.0

static void Param_copyStringsToAllocMem(struct event_handler_args args);
static void Param_copyIntsToAllocMem(struct event_handler_args args);
static void Param_copyBlobToAllocMem(struct event_handler_args args);
static void Param_confirmStore(struct event_handler_args args);
static void Param_returnPVName(const Param *my, const char *, const char *, const char *, char *);
static void Param_strerror(Param *, const char *);
static int Param_getIntArrayCache(const Param *, const char *, const char *, int, int *, unsigned long int *);
static int Param_getStringArrayCache(const Param *, const char *, const char *, int, int *, char **);
static int Param_storeIntCache(const Param *, const char *, const char *, unsigned long int);
static int Param_storeStringCache(const Param *, const char *, const char *, const char *);

typedef struct CaParamS {
	Tcl_Interp *cache;
	pthread_mutex_t *cacheLock;
	pthread_mutex_t *socketLock;
} CaParam;

typedef struct ParamStringResultS {
	char **val;
	int *rows;
	int maxrows;
	const Param *my;
	int *retVal;
	int *isReady;
} ParamStringResult;

typedef struct ParamIntResultS {
	unsigned long int *val;
	int *rows;
	int maxrows;
	const Param *my;
	int *retVal;
	int *isReady;
} ParamIntResult;

typedef struct ParamBlobResultS {
	FILE **val;
	size_t *size;
	const Param *my;
	int *retVal;
	int *isReady;
} ParamBlobResult;

typedef struct ParamConfirmStoreStructS {
	int *isReady;
} ParamConfirmStoreStruct;

int conSetupParam(Param *my, const char *setup)
{
	struct utsname bufferS, *buffer = &bufferS;
	int retVal = 0;
	int row = 0;
	int status = ECA_NORMAL;
#if ( _POSIX_VERSION < 199506L )
	int *attr;
#endif
	my->strerror = NULL;
	my->specParam = malloc(sizeof(CaParam));

#if ( _POSIX_VERSION < 199506L )
	((CaParam *) (my->specParam))->cacheLock = malloc(sizeof(pthread_mutex_t));
	pthread_mutexattr_create(attr);
	pthread_mutex_init(((CaParam *) (my->specParam))->cacheLock, *attr);
	pthread_mutexattr_delete(attr);

	((CaParam *) (my->specParam))->socketLock = malloc(sizeof(pthread_mutex_t));
	pthread_mutexattr_create(attr);
	pthread_mutex_init(((CaParam *) (my->specParam))->socketLock, *attr);
	pthread_mutexattr_delete(attr);
#else
	((CaParam *) (my->specParam))->cacheLock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(((CaParam *) (my->specParam))->cacheLock, NULL);

	((CaParam *) (my->specParam))->socketLock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(((CaParam *) (my->specParam))->socketLock, NULL);
#endif

	((CaParam *) (my->specParam))->cache = Tcl_CreateInterp();

	if ((status = ca_task_initialize()) != ECA_NORMAL) {
		Param_strerror((Param *) my, ca_message(status));
		pthread_mutex_destroy(((CaParam *) (my->specParam))->cacheLock);
		free(((CaParam *) (my->specParam))->cacheLock);
		pthread_mutex_destroy(((CaParam *) (my->specParam))->socketLock);
		free(((CaParam *) (my->specParam))->socketLock);
		retVal = -1;
	} else {
		if (setup != NULL) {
			my->setup = malloc(strlen(setup) + 1);
			strcpy(my->setup, setup);
		} else {
			my->setup = NULL;
		}
		my->basedir = malloc(PARAM_MAX_VALUE_LEN);
		uname(buffer);
		if (Param_getString(my, buffer->nodename, "basedir", &row, my->basedir) || (row != 1)) {
			strcpy(my->basedir, "");
		} else if (strlen(my->basedir)) {
			strcat(my->basedir, "/");
		}
	}

	return retVal;
}

int conParam(Param *my)
{
	return conSetupParam(my, NULL);
}

void desParam(Param *my)
{
	ca_task_exit();

	Tcl_DeleteInterp(((CaParam *) (my->specParam))->cache);
	pthread_mutex_destroy(((CaParam *) (my->specParam))->cacheLock);
	free(((CaParam *) (my->specParam))->cacheLock);
	pthread_mutex_destroy(((CaParam *) (my->specParam))->socketLock);
	free(((CaParam *) (my->specParam))->socketLock);
	free(my->basedir);
	if (my->setup != NULL) {
		free(my->setup);
	}
}

int Param_getInt(const Param *my, const char *name, const char *idx, int *row, unsigned long int *val)
{
	return Param_getIntArray(my, name, idx, 1, row, val);
}

int Param_getString(const Param *my, const char *name, const char *idx, int *row, char *val)
{
	return Param_getStringArray(my, name, idx, 1, row, &val);
}

int Param_getFilename(const Param *my, const char *name, const char *idx, int *row, char *val)
{
	return Param_getFilenameArray(my, name, idx, 1, row, &val);
}

int Param_getIntArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, unsigned long int *val)
{
	int isReady;
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;
	int i;
	char buf[PARAM_MAX_NAME_LEN];

	*rows = 0;
	if (Param_getIntArrayCache(my, name, idx, maxrows, rows, val) || (*rows != maxrows)) {
		pPVName = malloc((strlen("HAD:PI:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
		Param_returnPVName(my, "PI", name, idx, pPVName);

		if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
			if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
			} else if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				retVal = -1;
			} else {
				do {
					status = ca_pend_io(TIMEOUTIO);
				} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
				if (status != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
					found = 0;
					retVal = 0;
				}
				if (found == 1) {
					ParamIntResult resultS, *result = &resultS;
					result->val = val;
					result->rows = rows;
					result->maxrows = maxrows;
					result->my = my;
					result->retVal = &retVal;
					result->isReady = &isReady;
					isReady = 1;

					status = ca_array_get_callback(ca_field_type(chan), ca_element_count(chan), chan, Param_copyIntsToAllocMem, result);
					Param_strerror((Param *) my, ca_message(status));
					do {
						ca_pend_event(TIMEOUTEVT);
					} while(isReady == 1);
				}
				if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
				}
			}
			pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
		}
		free(pPVName);

		if (maxrows == 1) {
			Param_storeIntCache(my, name, idx, *val);
		} else {
			for (i = 0 ; i < *rows ; i++) {
				sprintf(buf, "%s%d", idx, i);
				Param_storeIntCache(my, name, buf, val[i]);
			}
		}
	}

	return retVal;
}

int Param_getStringArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, char **val)
{
	int isReady;
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;
	int i;
	char buf[PARAM_MAX_NAME_LEN];

	*rows = 0;
	if (Param_getStringArrayCache(my, name, idx, maxrows, rows, val) || (*rows != maxrows)) {
		pPVName = malloc((strlen("HAD:PS:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
		Param_returnPVName(my, "PS", name, idx, pPVName);

		if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
			if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
			} else if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				retVal = -1;
			} else {
				do {
					status = ca_pend_io(TIMEOUTIO);
				} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
				if (status != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
					found = 0;
					retVal = 0;
				}
				if (found == 1) {
					ParamStringResult resultS, *result = &resultS;
					result->val = val;
					result->rows = rows;
					result->maxrows = maxrows;
					result->my = my;
					result->retVal = &retVal;
					result->isReady = &isReady;
					isReady = 1;

					status = ca_array_get_callback(ca_field_type(chan), ca_element_count(chan), chan, Param_copyStringsToAllocMem, result);
					Param_strerror((Param *) my, ca_message(status));
					do {
						ca_pend_event(TIMEOUTEVT);
					} while(isReady == 1);
				}

				if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
				}
			}
			pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
		}

		free(pPVName);

		if (maxrows == 1) {
			Param_storeStringCache(my, name, idx, *val);
		} else {
			for (i = 0 ; i < *rows ; i++) {
				sprintf(buf, "%s%d", idx, i);
				Param_storeStringCache(my, name, buf, val[i]);
			}
		}
	}

	return retVal;
}

int Param_getFilenameArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, char **val)
{
	int isReady;
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;
	int i;
	char *prefixVal[PARAM_MAX_ARRAY_LEN];
	char buf[PARAM_MAX_NAME_LEN];
	*rows = 0;

	if (Param_getStringArrayCache(my, name, idx, maxrows, rows, val) || (*rows != maxrows)) {
		pPVName = malloc((strlen("HAD:PS:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
		Param_returnPVName(my, "PS", name, idx, pPVName);

		if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
			if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
			} else if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				retVal = -1;
			} else {
				do {
					status = ca_pend_io(TIMEOUTIO);
				} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
				if (status != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
					found = 0;
					retVal = 0;
				}
				if (found == 1) {
					ParamStringResult resultS, *result = &resultS;
					result->val = prefixVal;
					result->rows = rows;
					result->maxrows = maxrows;
					result->my = my;
					result->retVal = &retVal;
					result->isReady = &isReady;
					isReady = 1;

					for (i = 0 ; i < maxrows ; i++) {
						prefixVal[i] = malloc(PARAM_MAX_VALUE_LEN);
					}
					status = ca_array_get_callback(ca_field_type(chan), ca_element_count(chan), chan, Param_copyStringsToAllocMem, result);
					Param_strerror((Param *) my, ca_message(status));
					do {
						ca_pend_event(TIMEOUTEVT);
					} while(isReady == 1);

					for (i = 0 ; i < *rows ; i++) {
						if (*prefixVal[i] == '/') {
							strcpy(val[i], prefixVal[i]);
						} else {
							strcpy(val[i], my->basedir);
							strcat(val[i], prefixVal[i]);
						}
					}
					for (i = 0 ; i < maxrows ; i++) {
						free(prefixVal[i]);
					}
				}
				if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
					Param_strerror((Param *) my, ca_message(status));
				}
			}
			pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
		}

		free(pPVName);

		if (maxrows == 1) {
			Param_storeStringCache(my, name, idx, *val);
		} else {
			for (i = 0 ; i < *rows ; i++) {
				sprintf(buf, "%s%d", idx, i);
				Param_storeStringCache(my, name, buf, val[i]);
			}
		}
	}

	return retVal;
}

int Param_getBlob(const Param *my, const char *name, const char *idx, size_t *size, FILE **val)
{
	int isReady;
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;

	pPVName = malloc((strlen("HAD:PB:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
	Param_returnPVName(my, "PB", name, idx, pPVName);

	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
		if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
			Param_strerror((Param *) my, ca_message(status));
			found = 0;
		} else if (status != ECA_NORMAL) {
			Param_strerror((Param *) my, ca_message(status));
			retVal = -1;
		} else {
			do {
				status = ca_pend_io(TIMEOUTIO);
			} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
			if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
				retVal = 0;
			}
			if ((*val = tmpfile()) == NULL) {
				Param_strerror((Param *) my, strerror(errno));
				found = 0;
				retVal = -1;
			}

			if (found == 1) {
				ParamBlobResult resultS, *result = &resultS;
				result->val = val;
				result->size = size;
				result->my = my;
				result->retVal = &retVal;
				result->isReady = &isReady;
				isReady = 1;

				status = ca_array_get_callback(ca_field_type(chan), ca_element_count(chan), chan, Param_copyBlobToAllocMem, result);
				Param_strerror((Param *) my, ca_message(status));
					do {
						ca_pend_event(TIMEOUTEVT);
					} while(isReady == 1);
			}
			if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
			}
		}
		pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
	}

	free(pPVName);

	return retVal;
}

int Param_storeInt(const Param *my, const char *name, const char *idx, unsigned long int value)
{
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;

	pPVName = malloc((strlen("HAD:PI:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
	Param_returnPVName(my, "PI", name, idx, pPVName);

	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
		if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
			Param_strerror((Param *) my, ca_message(status));
			found = 0;
		} else if (status != ECA_NORMAL) {
			Param_strerror((Param *) my, ca_message(status));
			retVal = -1;
		} else {
			do {
				status = ca_pend_io(TIMEOUTIO);
			} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
			if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
				retVal = 0;
			}
			if (found == 1) {
				ParamConfirmStoreStruct confirmS, *confirm = &confirmS;
				int isReady = 1;
				confirm->isReady = &isReady;
				
				status = ca_array_put_callback(DBR_LONG, 1, chan, &value, Param_confirmStore, confirm);
				Param_strerror((Param *) my, ca_message(status));
				do {
					ca_pend_event(TIMEOUTEVT);
				} while(isReady == 1);
			}

			if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
			}
		}
		pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
	}

	free(pPVName);

	return retVal;
}

int Param_storeString(const Param *my, const char *name, const char *idx, const char *value)
{
	int retVal = 0;
	int status;
	int found = 1;
	chid chan;
	char *pPVName;

	pPVName = malloc((strlen("HAD:PS:") + (my->setup == NULL ? 0 : strlen(my->setup) + 1) + strlen(name) + 1 + strlen(idx) + 1) * sizeof(char));
	Param_returnPVName(my, "PS", name, idx, pPVName);

	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->socketLock)) {
		if ((status = ca_search(pPVName, &chan)) == ECA_GETFAIL) {
			Param_strerror((Param *) my, ca_message(status));
			found = 0;
		} else if (status != ECA_NORMAL) {
			Param_strerror((Param *) my, ca_message(status));
			retVal = -1;
		} else {
			do {
				status = ca_pend_io(TIMEOUTIO);
			} while ((ca_state(chan) != cs_conn) && (status != ECA_TIMEOUT));
			if (status != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
				found = 0;
				retVal = 0;
			}
			if (found == 1) {
				ParamConfirmStoreStruct confirmS, *confirm = &confirmS;
				int isReady = 1;
				confirm->isReady = &isReady;

				status = ca_array_put_callback(DBR_STRING, 1, chan, value, Param_confirmStore, confirm);
				Param_strerror((Param *) my, ca_message(status));
				do {
					ca_pend_event(TIMEOUTEVT);
				} while(isReady == 1);
			}

			if ((status = ca_clear_channel(chan)) != ECA_NORMAL) {
				Param_strerror((Param *) my, ca_message(status));
			}
		}
		pthread_mutex_unlock(((CaParam *) (my->specParam))->socketLock);
	}

	free(pPVName);

	return retVal;
}

void Param_clearCache(const Param *my)
{
	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->cacheLock)) {
		Tcl_DeleteInterp(((CaParam *) (my->specParam))->cache);
		((CaParam *) (my->specParam))->cache = Tcl_CreateInterp();
		 pthread_mutex_unlock(((CaParam *) (my->specParam))->cacheLock);
	}
}

const char *Param_getErrStr(const Param *my)
{
	return my->strerror;
}

static void Param_copyStringsToAllocMem(struct event_handler_args args)
{
	int i;
	ParamStringResult *result = args.usr;
	dbr_string_t *buffer = (dbr_string_t *) args.dbr;
	
	*(result->rows) = (args.count > result->maxrows) ?
		result->maxrows :
		args.count;
	if (args.type == DBR_STRING && args.status == ECA_NORMAL) {
		for(i = 0 ; i < *(result->rows) ; i++) {
			strcpy((result->val)[i], buffer[i]);
		}
	} else {
		Param_strerror((Param *) result->my, ca_message(args.status));
		*(result->rows) = 0;
	}
	*(result->isReady) = 0;
}

static void Param_copyIntsToAllocMem(struct event_handler_args args)
{
	ParamIntResult *result = args.usr;
	dbr_long_t *buffer = (dbr_long_t *) args.dbr;

	if (args.count == 1) {
		*(result->rows) = 1;
		if (args.type == DBR_LONG && args.status == ECA_NORMAL) {
			memcpy(result->val, buffer, sizeof(dbr_long_t));
		} else {
			Param_strerror((Param *) result->my, ca_message(args.status));
			*(result->rows) = 0;
		}
	} else {
		if (args.count - 2 <= result->maxrows) {
			*(result->rows) = args.count - 2;
		} else {
			*(result->rows) = result->maxrows;
		}
		if (args.type == DBR_LONG && args.status == ECA_NORMAL) {
			memcpy(result->val, buffer + 2, (sizeof(dbr_long_t) * *(result->rows)));
		} else {
			Param_strerror((Param *) result->my, ca_message(args.status));
			*(result->rows) = 0;
		}
	}
	*(result->isReady) = 0;
}

static void Param_copyBlobToAllocMem(struct event_handler_args args)
{
	ParamBlobResult *result = args.usr;
	dbr_long_t *buffer = (dbr_long_t *) args.dbr;
	if (args.type == DBR_LONG && args.status == ECA_NORMAL) {
		*(result->size) = *buffer;
		fwrite(buffer + 1, 1, *(result->size), *(result->val));
		fflush(*(result->val));
		rewind(*(result->val));
	} else {
		Param_strerror((Param *) result->my, ca_message(args.status));
		*(result->size) = 0;
	}
	*(result->isReady) = 0;
}

static void Param_confirmStore(struct event_handler_args args)
{
	ParamConfirmStoreStruct *result = args.usr;
	*(result->isReady) = 0;
}

static void Param_returnPVName(const Param *my, const char *type, const char *name, const char *idx, char *pPVName)
{
	int i;

	if (my->setup == NULL) {
		sprintf(pPVName ,"HAD:%s:%s:%s", type, name, idx);
	} else {
		sprintf(pPVName ,"HAD:%s:%s:%s:%s", type, my->setup, name, idx);
	}
	for (i = 0 ; i < strlen(pPVName) ; i++) {
		pPVName[i] = toupper(pPVName[i]);
	}
}

static void Param_strerror(Param *my, const char *strerror)
{
	my->strerror = realloc(my->strerror, strlen(strerror) + 1);
	if (my->strerror != NULL) {
		strcpy(my->strerror, strerror);
	}
}

static int Param_getIntArrayCache(const Param *my, const char *name, const char *idx, int maxrows, int *rows, unsigned long int *val)
{
	int retVal = 0;
	int i;
	char *endptr;
	char *strval[PARAM_MAX_ARRAY_LEN];

	for (i = 0 ; i < maxrows ; i++) {
		strval[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}
	*rows = 0;
	if ((retVal |= Param_getStringArrayCache(my, name, idx, maxrows, rows, strval)) == 0) {
		for (i = 0 ; i < *rows ; i++) {
			val[i] = strtoul(strval[i], &endptr, 0);
			if (*endptr != '\0') {
				*rows = 0;
				retVal = -1;
				Param_strerror((Param *) my, "Value seems to be no integer.");
			}
		}
	}
	for (i = 0 ; i < maxrows ; i++) {
		free(strval[i]);
	}
	return retVal;
}

static int Param_getStringArrayCache(const Param *my, const char *name, const char *idx, int maxrows, int *rows, char **val)
{
	int retVal = 0;
	int i;
	char lname[PARAM_MAX_NAME_LEN];
	char lidx[PARAM_MAX_NAME_LEN];

	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->cacheLock)) {
		for(i = 0 ; i <=strlen(name) ; i++) {
			lname[i] = tolower(name[i]);
		}
		for(i = 0 ; i <=strlen(idx) ; i++) {
			lidx[i] = tolower(idx[i]);
		}

		*rows = 0;
		if (Tcl_GetVar2(((CaParam *) (my->specParam))->cache, lname, lidx, 0) != 0) {
			strcpy(val[0], Tcl_GetVar2(((CaParam *) (my->specParam))->cache, lname, lidx, 0));
			*rows = 1;
		} else {
			char index[PARAM_MAX_NAME_LEN];
			for (i = 0 ; i < maxrows ; i++) {
				sprintf(index,"%s%d", lidx, i);
				if (Tcl_GetVar2(((CaParam *) (my->specParam))->cache, lname, index, 0) != 0) {
					strcpy(val[i], Tcl_GetVar2(((CaParam *) (my->specParam))->cache, lname, index, 0));
					(*rows)++;
				} else {
					i = maxrows;
				}
			}
		}
		pthread_mutex_unlock(((CaParam *) (my->specParam))->cacheLock);
	} else {
		retVal = -1;
	}
	return retVal;
}

static int Param_storeIntCache(const Param *my, const char *name, const char *idx, unsigned long int value)
{
	char buf[7 + 2 * PARAM_MAX_NAME_LEN + PARAM_MAX_VALUE_LEN];
	sprintf(buf, "set %s(%s)\t%lu\n", name, idx, value);
	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->cacheLock)) {
		Tcl_Eval(((CaParam *) (my->specParam))->cache, buf);
		pthread_mutex_unlock(((CaParam *) (my->specParam))->cacheLock);
	}
	return 0;
}

static int Param_storeStringCache(const Param *my, const char *name, const char *idx, const char *value)
{
	char buf[7 + 2 * PARAM_MAX_NAME_LEN + PARAM_MAX_VALUE_LEN];
	sprintf(buf, "set %s(%s)\t\"%s\"\n", name, idx, value);
	if (0 == pthread_mutex_lock(((CaParam *) (my->specParam))->cacheLock)) {
		Tcl_Eval(((CaParam *) (my->specParam))->cache, buf);
		pthread_mutex_unlock(((CaParam *) (my->specParam))->cacheLock);
	}
	return 0;
}

