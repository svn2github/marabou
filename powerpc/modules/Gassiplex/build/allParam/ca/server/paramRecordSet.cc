static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramRecordSet.cc,v 1.10 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <ctype.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <syslog.h>
}

#include "paramRecordSet.h"
#include "paramIntRecord.h"
#include "paramStringRecord.h"
#include "paramFilenameRecord.h"
#include "paramBlobRecord.h"

ParamRecordSet::ParamRecordSet(unsigned int pvCountEstimate) :
  caServer(pvCountEstimate)
{
	numParamSrc = 0;
	numPv = 0;
}

ParamRecordSet::~ParamRecordSet()
{
	for (int i = 0 ; i < numParamSrc ; i++) {
		desParam(param[i]);
		delete param[i];
	}
	for (int i = 0 ; i < numPv ; i++) {
		delete pvs[i];
	}
}

Param *ParamRecordSet::pParam(const char *setup)
{
	if(setup != NULL) {
		for (int i = 0 ; i < numParamSrc ; i++) {
			if (param[i]->setup != NULL) {
				if (strcmp(param[i]->setup, setup) == 0) {
					return param[i];
				}
			} else {
				if (setup == NULL) {
					return param[i];
				}
			}
		}
	} else {
		for (int i = 0 ; i < numParamSrc ; i++) {
			if (param[i]->setup == NULL) {
				return param[i];
			}
		}
	}
	return NULL;
}

pvExistReturn ParamRecordSet::pvExistTest(const casCtx &ctx, const char *pPVName)
{
	char *setup;
	char buf1[PARAM_MAX_NAME_LEN];
	char buf2[PARAM_MAX_NAME_LEN];
	char buf3[PARAM_MAX_NAME_LEN];

	strcpy(buf1, "");
	strcpy(buf2, "");
	strcpy(buf3, "");

	for (int i = 0 ; i < numPv ; i++) {
		if(strcmp(pPVName, pvs[i]->getPVName()) == 0) {
			return pverExistsHere;
		}
	}

	if(strncmp(pPVName, "HAD:P", strlen("HAD:P")) == 0) {
		if (sscanf(pPVName, "HAD:P%*c:%[^:]:%[^:]:%[^:]", buf1, buf2, buf3) != 3) {
			setup = NULL;
		} else {
			setup = buf1;
			for (unsigned int i = 0 ; i < strlen(setup) ; i++) {
				setup[i] = tolower(setup[i]);
			}
		}
		if(pParam(setup) == NULL) {
			param[numParamSrc] = new Param;
			if (conSetupParam(param[numParamSrc], setup) == 0) {
				syslog(LOG_INFO, "Constructed new param source: %s", setup);
				numParamSrc++;
			} else {
				syslog(LOG_INFO, "Failed to construct new param source: %s", setup);
				desParam(param[numParamSrc]);
				delete param[numParamSrc];
			}
		}
		if(pParam(setup) != NULL) {
			return pverExistsHere;
		} else {
			return pverDoesNotExistHere;
		}
	}
	return pverDoesNotExistHere;
}

pvCreateReturn ParamRecordSet::createPV(const casCtx &ctx, const char *pPVName)
{
	char *setup;
	char type;
	char buf1[PARAM_MAX_NAME_LEN];
	char buf2[PARAM_MAX_NAME_LEN];
	char buf3[PARAM_MAX_NAME_LEN];

	pvCreateReturn retVal(S_casApp_pvNotFound);

	for (int i = 0 ; i < numPv ; i++) {
		if(strcmp(pPVName, pvs[i]->getPVName()) == 0) {
			retVal = pvs[i];
			return retVal;
		}
	}

	if(strncmp(pPVName, "HAD:P", strlen("HAD:P")) == 0) {
		if(sscanf(pPVName, "HAD:P%c:%[^:]:%[^:]:%[^:]", &type, buf1, buf2, buf3) != 4) {
			setup = NULL;
		} else {
			setup = buf1;
			for (unsigned int i = 0 ; i < strlen(setup) ; i++) {
				setup[i] = tolower(setup[i]);
			}
		}
		if(pParam(setup) != NULL) {
			switch (type) {
				case('I'):
					retVal = (pvs[numPv++] = new ParamIntRecord(*this, pParam(setup), pPVName, "Integer"));
					syslog(LOG_DEBUG, "Created Integer Record %s", pPVName);
					break;
				case('S'):
					retVal = (pvs[numPv++] = new ParamStringRecord(*this, pParam(setup), pPVName, "String"));
					syslog(LOG_DEBUG, "Created String Record %s", pPVName);
					break;
				case('F'):
					retVal = (pvs[numPv++] = new ParamFilenameRecord(*this, pParam(setup), pPVName, "Filename"));
					syslog(LOG_DEBUG, "Created Filename Record %s", pPVName);
					break;
				case('B'):
					retVal = (pvs[numPv++] = new ParamBlobRecord(*this, pParam(setup), pPVName, "Binary Large OBject"));
					syslog(LOG_DEBUG, "Created Blob Record %s", pPVName);
					break;
			}
			return retVal;
		} else {
			return S_casApp_pvNotFound;
		}
	}
	return S_casApp_pvNotFound;
}

