#ifndef ALLPARAM_H
#define ALLPARAM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#ifdef __cplusplus
}
#endif

/* limitations */
#define PARAM_MAX_ARRAY_LEN 128
#define PARAM_MAX_VALUE_LEN 128
#define PARAM_MAX_NAME_LEN 128

/* class */
typedef struct ParamS {
	char *basedir;
	char *setup;
	void *specParam;
	char *strerror;
} Param;

void Param_registerLogMsg(void (*f) (int, const char *, ...));

/* constructors and destructor */
int conSetupParam(Param *param, const char *setup);
int conParam(Param *param);
void desParam(Param *param);

/* get data */
int Param_getInt(const Param *param, const char *name, const char *idx,
				 int *rows, unsigned long int *value);
int Param_getString(const Param *param, const char *name, const char *idx,
					int *rows, char *value);
int Param_getFilename(const Param *param, const char *name, const char *idx,
					  int *rows, char *value);
int Param_getIntArray(const Param *param, const char *name, const char *idx,
					  int maxrows, int *rows, unsigned long int *value);
int Param_getStringArray(const Param *param, const char *name,
						 const char *idx, int maxrows, int *rows,
						 char **value);
int Param_getFilenameArray(const Param *param, const char *name,
						   const char *idx, int maxrows, int *rows,
						   char **value);
int Param_getBlob(const Param *param, const char *name, const char *idx,
				  size_t * size, FILE ** value);

/* store data */
int Param_storeInt(const Param *param, const char *name, const char *idx,
				   unsigned long int value);
int Param_storeString(const Param *param, const char *name, const char *idx,
					  const char *value);

/* misc */
void Param_clearCache(const Param *param);

const char *Param_getErrStr(const Param *param);

#endif							/* !ALLPARAM_H */
