static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/file/fileParam.c,v 1.21 2003/01/31 16:41:06 hadaq Exp $";
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

#include "../include/allParam.h"

static int Param_getParamNumber(const Param *, const char *, const char *);
static void Param_strerror(Param *, const char *);

typedef struct FileParamS {
	int nVals;
	char **pname;
	char **value;
	FILE *store;
} FileParam;

int conSetupParam(Param *my, const char *setup)
{
	int retVal = 0;
	char *paramFile;
	char *storageFile;
	FILE *f;
	char buf[2 * (PARAM_MAX_NAME_LEN + 1)];
	int row = 0;
	struct utsname bufferS, *buffer = &bufferS;

	my->strerror = NULL;
	my->specParam = malloc(sizeof(FileParam));
	((FileParam *) (my->specParam))->pname = 0;
	((FileParam *) (my->specParam))->value = 0;
	((FileParam *) (my->specParam))->nVals = 0;

	if (setup != NULL) {
		my->setup = malloc(strlen(setup) + 1);
		strcpy(my->setup, setup);
	} else {
		my->setup = NULL;
	}

	paramFile = malloc(((my->setup == NULL) ? strlen("default") : strlen(setup)) + strlen("_p.tcl") + 1);
	storageFile = malloc(((my->setup == NULL) ? strlen("default") : strlen(setup)) + strlen("_s.tcl") + 1);
	sprintf(paramFile, "%s_p.tcl", (setup == NULL) ? "default" : setup);
	sprintf(storageFile, "%s_s.tcl", (setup == NULL) ? "default" : setup);

	if (NULL == (f = fopen(paramFile, "r"))) {
		desParam(my);
		return -1;
	} else {
		while(fgets(buf, 2 * (PARAM_MAX_NAME_LEN + 1), f) != NULL) {
			if (buf[0] != '#') {
				int items = 0;

				((FileParam *) (my->specParam))->nVals++;
				((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1] = malloc(2 * (PARAM_MAX_NAME_LEN + 1) * sizeof(char));
				((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
				items = sscanf(buf, "set%s %*[\"]%[^\"]\"",
					((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1],
					((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]
				);
				if (items != 2) {
					items = sscanf(buf, "set%s%s",
						((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1],
						((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]
					);
				}
				if (items != 2) {
					free(((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1]);
					free(((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]);
					((FileParam *) (my->specParam))->nVals--;
					((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
					((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				}
			}
		} 
		fclose(f);
	}

	if (NULL == (((FileParam *) (my->specParam))->store = fopen(storageFile, "a+"))) {
		return -1;
	}

	free(paramFile);
	free(storageFile);

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

int conParam(Param *my)
{
	int retVal = 0;
	char *paramFile;
	char *storageFile;
	FILE *f;
	char buf[2 * (PARAM_MAX_NAME_LEN + 1)];
	char *tmp;
	int row = 0;
	struct utsname bufferS, *buffer = &bufferS;

	my->strerror = NULL;
	my->setup = NULL;
	my->specParam = malloc(sizeof(FileParam));
	((FileParam *) (my->specParam))->pname = 0;
	((FileParam *) (my->specParam))->value = 0;
	((FileParam *) (my->specParam))->nVals = 0;

	if((tmp = getenv("DAQSLOW_PARAM_FILE")) != NULL) {
		paramFile = malloc(strlen(tmp) + 1);
		strcpy(paramFile, tmp);
	} else {
		paramFile = malloc(strlen("param.tcl") + 1);
		strcpy(paramFile, "param.tcl");
	}

	if((tmp = getenv("DAQSLOW_STORAGE_FILE")) != NULL) {
		storageFile = malloc(strlen(tmp) + 1);
		strcpy(storageFile, tmp);
	} else {
		storageFile = malloc(strlen("storage.tcl") + 1);
		strcpy(storageFile, "storage.tcl");
	}

	if (NULL == (f = fopen(paramFile, "r"))) {
		desParam(my);
		return -1;
	} else {
		while(fgets(buf, 2 * (PARAM_MAX_NAME_LEN + 1), f) != NULL) {
			if (buf[0] != '#') {
				int items = 0;

				((FileParam *) (my->specParam))->nVals++;
				((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1] = malloc(2 * (PARAM_MAX_NAME_LEN + 1) * sizeof(char));
				((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
				items = sscanf(buf, "set%s %*[\"]%[^\"]\"",
					((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1],
					((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]
				);
				if (items != 2) {
					items = sscanf(buf, "set%s%s",
						((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1],
						((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]
					);
				}
				if (items != 2) {
					free(((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1]);
					free(((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1]);
					((FileParam *) (my->specParam))->nVals--;
					((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
					((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
				}
			}
		} 
		fclose(f);
	}

	if (NULL == (((FileParam *) (my->specParam))->store = fopen(storageFile, "a+"))) {
		return -1;
	}

	free(paramFile);
	free(storageFile);

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
	int i;

	for (i = 0 ; i < ((FileParam *) (my->specParam))->nVals ; i++) {
		free(((FileParam *) (my->specParam))->pname[i]);
		free(((FileParam *) (my->specParam))->value[i]);
	}
	free(((FileParam *) (my->specParam))->pname);
	free(((FileParam *) (my->specParam))->value);
	free((FileParam *) (my->specParam));

	fclose(((FileParam *) (my->specParam))->store);
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
			*row = 1;
		} else {
			strcpy(val, my->basedir);
			strcat(val, value);
			*row = 1;
		}
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
	int i;
	int n = -2;
	char lname[PARAM_MAX_NAME_LEN];
	char lidx[PARAM_MAX_NAME_LEN];

	for(i = 0 ; i <=strlen(name) ; i++) {
		lname[i] = (char) tolower(name[i]);
	}
	for(i = 0 ; i <=strlen(idx) ; i++) {
		lidx[i] = (char) tolower(idx[i]);
	}

	*rows = 0;
	if((n = Param_getParamNumber(my, lname, lidx)) != -1) {
		strcpy(val[0], ((FileParam *) (my->specParam))->value[n]);
		*rows = 1;
	} else {
		char index[PARAM_MAX_NAME_LEN];

		n = -2;
		for (i = 0 ; (i < maxrows) && (n != -1) ; i++) {
			sprintf(index,"%s%d", lidx, i);
			if((n = Param_getParamNumber(my, lname, index)) != -1) {
				strcpy(val[i], ((FileParam *) (my->specParam))->value[n]);
				(*rows)++;
			}
		}
	}
	return 0;
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
	fprintf(((FileParam *) (my->specParam))->store, "set %s(%s)\t%lu\n", name, idx, value);
	fflush(((FileParam *) (my->specParam))->store);
	((FileParam *) (my->specParam))->nVals++;
	((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
	((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
	((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1] = malloc(2 * (PARAM_MAX_NAME_LEN + 1) * sizeof(char));
	((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	sprintf(((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1], "%s(%s)", name, idx);
	sprintf(((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1], "%lu", value);
	return 0;
}

int Param_storeString(const Param *my, const char *name, const char *idx, const char *value)
{
	fprintf(((FileParam *) (my->specParam))->store, "set %s(%s)\t\"%s\"\n", name, idx, value);
	fflush(((FileParam *) (my->specParam))->store);
	((FileParam *) (my->specParam))->nVals++;
	((FileParam *) (my->specParam))->pname = realloc(((FileParam *) (my->specParam))->pname, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
	((FileParam *) (my->specParam))->value = realloc(((FileParam *) (my->specParam))->value, ((FileParam *) (my->specParam))->nVals * sizeof(char *));
	((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1] = malloc(2 * (PARAM_MAX_NAME_LEN + 1) * sizeof(char));
	((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	sprintf(((FileParam *) (my->specParam))->pname[((FileParam *) (my->specParam))->nVals - 1], "%s(%s)", name, idx);
	sprintf(((FileParam *) (my->specParam))->value[((FileParam *) (my->specParam))->nVals - 1], "%s", value);
	return 0;
}

void Param_clearCache(const Param *my)
{
}

const char *Param_getErrStr(const Param *my)
{
	return my->strerror;
}

static int Param_getParamNumber(const Param *my, const char *name, const char *idx)
{
	int retVal = -1;
	int i;
	char fullName[2 * (PARAM_MAX_NAME_LEN + 1)];

	sprintf(fullName, "%s(%s)", name, idx);
	for (i = 0; i < ((FileParam *) (my->specParam))->nVals; i++) {
		if (strcmp(((FileParam *) (my->specParam))->pname[i], fullName) == 0) {
			retVal = i;
		}
	}
	return retVal;
}

static void Param_strerror(Param *my, const char *strerror)
{
	my->strerror = realloc(my->strerror, strlen(strerror) + 1);
	if(my->strerror != NULL) {
		strcpy(my->strerror, strerror);
	}
}

