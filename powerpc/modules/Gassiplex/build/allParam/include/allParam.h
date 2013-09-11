#ifndef ALLPARAM_H
#define ALLPARAM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#ifdef __cplusplus
}
#endif

#define PARAM_MAX_ARRAY_LEN 128
#define PARAM_MAX_VALUE_LEN 128
#define PARAM_MAX_NAME_LEN 128

typedef struct ParamS {
	char *basedir;
	char *setup;
	void *specParam;
	char *strerror;
} Param;

int conSetupParam(Param *, const char *);
int conParam(Param *);
void desParam(Param *);

int Param_getInt(const Param *, const char *, const char *, int *, unsigned long int *);
int Param_getString(const Param *, const char *, const char *, int *, char *);
int Param_getFilename(const Param *, const char *, const char *, int *, char *);
int Param_getIntArray(const Param *, const char *, const char *, int, int *, unsigned long int *);
int Param_getStringArray(const Param *, const char *, const char *, int, int *, char **);
int Param_getFilenameArray(const Param *, const char *, const char *, int, int *, char **);
int Param_getBlob(const Param *, const char *, const char *, size_t *, FILE **);

int Param_storeInt(const Param *, const char *, const char *, unsigned long int);
int Param_storeString(const Param *, const char *, const char *, const char *);
void Param_clearCache(const Param *);

const char *Param_getErrStr(const Param *);

#endif /* !ALLPARAM_H */

