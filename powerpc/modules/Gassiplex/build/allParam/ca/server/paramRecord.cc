static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramRecord.cc,v 1.9 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <ctype.h>
  #include <stdio.h>
  #include <string.h>
}

#include "paramRecord.h"

ParamRecord::ParamRecord(caServer& cas, const Param *p, const char *rn, const char *u) :
  Record(cas, rn, u, aitEnumString), param(p)
{
	char tmp;
	char buf1[PARAM_MAX_NAME_LEN];
	char buf2[PARAM_MAX_NAME_LEN];
	char buf3[PARAM_MAX_NAME_LEN];

	strcpy(pPVName, rn);
	strcpy(buf1, "");
	strcpy(buf2, "");
	strcpy(buf3, "");
	if(sscanf(pPVName, "HAD:P%c:%[^:]:%[^:]:%[^:]", &tmp, buf1, buf2, buf3) != 4) {
		strcpy(name, buf1);
		strcpy(idx, buf2);
	} else {
		strcpy(name, buf2);
		strcpy(idx, buf3);
	}

	for (unsigned int i = 0 ; i < strlen(name) ; i++) {
		name[i] = tolower(name[i]);
	}
	for (unsigned int i = 0 ; i < strlen(idx) ; i++) {
		idx[i] = tolower(idx[i]);
	}
	interest = aitFalse;
}

ParamRecord::~ParamRecord()
{
	delete pDest;
}

const char *ParamRecord::getPVName()
{
	return pPVName;
}

gddAppFuncTableStatus ParamRecord::readAlarmLow(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readAlarmHigh(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readAlarmLowWarning(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readAlarmHighWarning(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readGraphicLow(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readGraphicHigh(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readControlLow(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readControlHigh(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

gddAppFuncTableStatus ParamRecord::readPrecision(gdd &value)
{
	value.putConvert(0);
	return S_casApp_success;
}

