static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/tcl/tclParam.c,v 1.23 2003/01/31 16:40:18 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include <tcl.h>

#include "../include/allParam.h"

static void Param_strerror(Param *, const char *);

typedef struct TclParamS {
	Tcl_Interp *interp;
	FILE *store;
} TclParam;

int conSetupParam(Param *my, const char *setup)
{
	int retVal = 0;
	char *paramFile;
	char *storageFile;
	int code;
	int row = 0;
	struct utsname bufferS, *buffer = &bufferS;

	my->strerror = NULL;
	my->specParam = malloc(sizeof(TclParam));

	if (setup != NULL) {
		my->setup = malloc(strlen(setup) + 1);
		strcpy(my->setup, setup);
	} else {
		my->setup = NULL;
	}
	paramFile = malloc(((my->setup == NULL) ? strlen("default") : strlen(setup)) + strlen("_p.tcl") + 1);
	storageFile = malloc(((my->setup == NULL) ? strlen("default") : strlen(setup)) + strlen("_s.tcl") + 1);
	sprintf(paramFile, "%s_p.tcl", (my->setup == NULL) ? "default" : setup);
	sprintf(storageFile, "%s_s.tcl", (my->setup == NULL) ? "default" : setup);

	((TclParam *) (my->specParam))->interp = Tcl_CreateInterp();
	code = Tcl_EvalFile(((TclParam *) (my->specParam))->interp, paramFile);
	if (code != TCL_OK) {
		if (* ((TclParam *) (my->specParam))->interp->result != 0) {
			Param_strerror(my, ((TclParam *) (my->specParam))->interp->result);
		} else {
			Param_strerror((Param *) my, "Tcl interpreter cannot read file correctly and does not deliver an error string.");
		}
		retVal = -1;
	}

	if (NULL == (((TclParam *) (my->specParam))->store = fopen(storageFile, "a+"))) {
		retVal = -1;
	}

	if(retVal == 0) {
		my->basedir = malloc(PARAM_MAX_VALUE_LEN);
		uname(buffer);
		if(Param_getString(my, buffer->nodename, "basedir", &row, my->basedir) || (row != 1)) {
			strcpy(my->basedir, "");
		} else if (strlen(my->basedir)) {
			strcat(my->basedir, "/");
		}
	}

	free(paramFile);
	free(storageFile);

	return retVal;
}

int conParam(Param *my)
{
	int retVal = 0;
	char *paramFile;
	char *storageFile;
	int code;
	int row = 0;
	struct utsname bufferS, *buffer = &bufferS;

	my->strerror = NULL;
	my->setup = NULL;
	my->specParam = malloc(sizeof(TclParam));

	paramFile = getenv("DAQSLOW_PARAM_FILE");
	if (paramFile == NULL) {
		paramFile = "param.tcl";
	}
	storageFile = getenv("DAQSLOW_STORAGE_FILE");
	if (storageFile == NULL) {
		storageFile = "storage.tcl";
	}

	((TclParam *) (my->specParam))->interp = Tcl_CreateInterp();
	code = Tcl_EvalFile(((TclParam *) (my->specParam))->interp, paramFile);
	if (code != TCL_OK) {
		if (* ((TclParam *) (my->specParam))->interp->result != 0) {
			Param_strerror(my, ((TclParam *) (my->specParam))->interp->result);
		} else {
			Param_strerror((Param *) my, "Tcl interpreter cannot read file correctly and does not deliver an error string.");
		}
		retVal = -1;
	}

	if (NULL == (((TclParam *) (my->specParam))->store = fopen(storageFile, "a+"))) {
		return -1;
	}

	if(retVal == 0) {
		my->basedir = malloc(PARAM_MAX_VALUE_LEN);
		uname(buffer);
		if(Param_getString(my, buffer->nodename, "basedir", &row, my->basedir) || (row != 1)) {
			strcpy(my->basedir, "");
		} else if (strlen(my->basedir)) {
			strcat(my->basedir, "/");
		}
	}

	return retVal;
}

void desParam(Param *my)
{
	Tcl_DeleteInterp(((TclParam *) (my->specParam))->interp);
	fclose(((TclParam *) (my->specParam))->store);

	free((TclParam *) (my->specParam));
	free(my->basedir);
	if(my->setup != NULL) {
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
	int retVal = 0;
	int rows = 0;
	char value[PARAM_MAX_VALUE_LEN];

	if (((retVal = Param_getString(my, name, idx, &rows, value)) == 0) && (rows == 1)) {
		if (value[0] == '/') {
			strcpy(val, value);
		} else {
			strcpy(val, my->basedir);
			strcat(val, value);
		}
		*row = 1;
	} else {
		*row = 0;
	}
	return retVal;
}

int Param_getIntArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, unsigned long int *val)
{
	int retVal = 0;
	int i;
	char *endptr;
	char *strval[PARAM_MAX_ARRAY_LEN];

	for (i = 0 ; i < maxrows ; i++) {
		strval[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}
	*rows = 0;
	if((retVal |= Param_getStringArray(my, name, idx, maxrows, rows, strval)) == 0) {
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

int Param_getStringArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, char **val)
{
	int retVal = 0;
	int i;
	char lname[PARAM_MAX_NAME_LEN];
	char lidx[PARAM_MAX_NAME_LEN];

	for(i = 0 ; i <=strlen(name) ; i++) {
		lname[i] = tolower(name[i]);
	}
	for(i = 0 ; i <=strlen(idx) ; i++) {
		lidx[i] = tolower(idx[i]);
	}

	*rows = 0;
	if(Tcl_GetVar2(((TclParam *) (my->specParam))->interp, lname, lidx, 0) != 0) {
		strcpy(val[0], Tcl_GetVar2(((TclParam *) (my->specParam))->interp, lname, lidx, 0));
		*rows = 1;
	} else {
		char index[PARAM_MAX_NAME_LEN];
		for (i = 0 ; i < maxrows ; i++) {
			sprintf(index,"%s%d", lidx, i);
			if(Tcl_GetVar2(((TclParam *) (my->specParam))->interp, lname, index, 0) != 0) {
				strcpy(val[i], Tcl_GetVar2(((TclParam *) (my->specParam))->interp, lname, index, 0));
				(*rows)++;
			} else {
				i = maxrows;
			}
		}
	}
	return retVal;
}

int Param_getFilenameArray(const Param *my, const char *name, const char *idx, int maxrows, int *rows, char **val)
{
	int retVal = 0;
	int i;
	char *value[PARAM_MAX_ARRAY_LEN];

	for (i = 0 ; i < maxrows ; i++) {
		value[i] = malloc(PARAM_MAX_VALUE_LEN);
	}

	if (((retVal = Param_getStringArray(my, name, idx, maxrows, rows, value)) == 0) && (*rows > 0)) {
		for (i = 0 ; i < *rows ; i++) {
			if (value[i][0] != '/') {
				strcpy(val[i], my->basedir);
				strcat(val[i], value[i]);
			} else {
				strcpy(val[i], value[i]);
			}
		}
	} else {
		*rows = 0;
	}

	for (i = 0 ; i < maxrows ; i++) {
		free(value[i]);
	}

	return retVal;
}

int Param_getBlob(const Param *my, const char *name, const char *idx, size_t *size, FILE **val)
{
	int retVal = 0;
	char filename[PARAM_MAX_VALUE_LEN];
	int rows;
	FILE *stream;

	retVal = Param_getFilename(my, name, idx, &rows, filename);
	if ((rows == 0) || (stream = fopen(filename, "r")) == NULL) {
		Param_strerror((Param *) my, strerror(errno));
		*size = 0;
		retVal = -1;
	} else {
		struct stat fileS, *file = &fileS;
		stat(filename, file);
		*size = (size_t) file->st_size;
		*val = stream;
	}
	return retVal;
}

int Param_storeInt(const Param *my, const char *name, const char *idx, unsigned long int value)
{
	char buf[7 + 2 * PARAM_MAX_NAME_LEN + PARAM_MAX_VALUE_LEN];
	sprintf(buf, "set %s(%s)\t%lu\n", name, idx, value);
	fprintf(((TclParam *) (my->specParam))->store, buf);
	fflush(((TclParam *) (my->specParam))->store);
	Tcl_Eval(((TclParam *) (my->specParam))->interp, buf);
	return 0;
}

int Param_storeString(const Param *my, const char *name, const char *idx, const char *value)
{
	char buf[7 + 2 * PARAM_MAX_NAME_LEN + PARAM_MAX_VALUE_LEN];
	sprintf(buf, "set %s(%s)\t\"%s\"\n", name, idx, value);
	fprintf(((TclParam *) (my->specParam))->store, buf);
	fflush(((TclParam *) (my->specParam))->store);
	Tcl_Eval(((TclParam *) (my->specParam))->interp, buf);
	return 0;
}

void Param_clearCache(const Param *my)
{
}

const char *Param_getErrStr(const Param *my)
{
	return my->strerror;
}

static void Param_strerror(Param *my, const char *strerror)
{
	my->strerror = realloc(my->strerror, strlen(strerror) + 1);
	if(my->strerror != NULL) {
		strcpy(my->strerror, strerror);
	}
}

