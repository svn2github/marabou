#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "ia_uppercase.h"

#define NUM2 32

int ia_uppercase(const char *testid) {
	Param *param;
	char *name = "TNAME";
	char *idx = "TINTA";
	unsigned long int result[NUM2];
	int rows;
	int resultsum = 1;
	int i;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getIntArray: Uppercase input on correct statements.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getIntArray(param, name, idx, NUM2, &rows, result);
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

