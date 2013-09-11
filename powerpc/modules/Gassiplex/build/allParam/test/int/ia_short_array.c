#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "ia_short_array.h"

#define NUM2 32
#define NUM4 64

int ia_short_array(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tinta";
	unsigned long int result[NUM4];
	int rows;
	int resultsum = 1;
	int i;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getIntArray: To few array members available.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getIntArray(param, name, idx, NUM4, &rows, result);
	for(i = 0 ; i < NUM2 ; i++) {
		resultsum &= (result[i] == i + 1 - 2*(i%2));
	}
	if ((rows == NUM2) && resultsum && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result array:\n", testid, rows);
		for(i = 0 ; i < NUM2 ; i++) {
			printf("result[%d]: %ld.\n", i, result[i]);
		}
		printf("return Value: %d.\n", retVal);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}
	desParam(param);
	free(param);

	return myRetVal;
}

