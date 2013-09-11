#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "sa_int_available.h"

#define NUM2 32

int sa_int_available(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tinta";
	char *result[NUM2];
	int rows;
	int retVal;
	int myRetVal;
	int i;

#ifdef VERBOSE
	printf("%s : Param_getStringArray: String is wanted but Int-like values found.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM2 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getStringArray(param, name, idx, NUM2, &rows, result);
	if ((rows == NUM2) &&
		result[0] && (strcmp(result[0], "1") == 0) &&
		result[1] && (strcmp(result[1], "0") == 0) &&
		result[2] && (strcmp(result[2], "3") == 0) &&
		result[3] && (strcmp(result[3], "2") == 0) &&
		result[4] && (strcmp(result[4], "5") == 0) &&
		result[5] && (strcmp(result[5], "4") == 0) &&
		result[6] && (strcmp(result[6], "7") == 0) &&
		result[7] && (strcmp(result[7], "6") == 0) &&
		result[8] && (strcmp(result[8], "9") == 0) &&
		result[9] && (strcmp(result[9], "8") == 0) &&
		result[10] && (strcmp(result[10], "11") == 0) &&
		result[11] && (strcmp(result[11], "10") == 0) &&
		result[12] && (strcmp(result[12], "13") == 0) &&
		result[13] && (strcmp(result[13], "12") == 0) &&
		result[14] && (strcmp(result[14], "15") == 0) &&
		result[15] && (strcmp(result[15], "14") == 0) &&
		result[16] && (strcmp(result[16], "17") == 0) &&
		result[17] && (strcmp(result[17], "16") == 0) &&
		result[18] && (strcmp(result[18], "19") == 0) &&
		result[19] && (strcmp(result[19], "18") == 0) &&
		result[20] && (strcmp(result[20], "21") == 0) &&
		result[21] && (strcmp(result[21], "20") == 0) &&
		result[22] && (strcmp(result[22], "23") == 0) &&
		result[23] && (strcmp(result[23], "22") == 0) &&
		result[24] && (strcmp(result[24], "25") == 0) &&
		result[25] && (strcmp(result[25], "24") == 0) &&
		result[26] && (strcmp(result[26], "27") == 0) &&
		result[27] && (strcmp(result[27], "26") == 0) &&
		result[28] && (strcmp(result[28], "29") == 0) &&
		result[29] && (strcmp(result[29], "28") == 0) &&
		result[30] && (strcmp(result[30], "31") == 0) &&
		result[31] && (strcmp(result[31], "30") == 0) &&
		(retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, return Value: %d.\n", testid, rows, retVal);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}
	for (i = 0 ; i < NUM2 ; i++) {
		free(result[i]);
	}

	desParam(param);
	free(param);

	return myRetVal;
}

