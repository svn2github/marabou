static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramBlobRecord.cc,v 1.13 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <stdio.h>
  #include <string.h>
}

#include "paramBlobRecord.h"

#define EPICS_MAX_INDEX 4092

ParamBlobRecord::ParamBlobRecord(caServer& cas, const Param *p, const char *rn, const char *u) :
  ParamRecord(cas, p, rn, u)
{
	size_t size;
	FILE *tmp;

	pDest = new ArrayIntDestructor;

	if(Param_getBlob(param, name, idx, &size, &tmp) == 0) {
		index = (unsigned int) ((size - 1) / sizeof(aitUint32)) + 2;
		if (index > EPICS_MAX_INDEX) {
			index = 0;
		}
	} else {
		index = 0;
	}
	fclose(tmp);

	funcTable.installReadFunc("units", &Record::readUnits);
	funcTable.installReadFunc("status", &Record::readStatus);
	funcTable.installReadFunc("severity", &Record::readSeverity);
	funcTable.installReadFunc("seconds", &Record::readSeconds);

	funcTable.installReadFunc("alarmLow", &ParamRecord::readAlarmLow);
	funcTable.installReadFunc("alarmHigh", &ParamRecord::readAlarmHigh);
	funcTable.installReadFunc("alarmLowWarning", &ParamRecord::readAlarmLowWarning);
	funcTable.installReadFunc("alarmHighWarning", &ParamRecord::readAlarmHighWarning);
	funcTable.installReadFunc("graphicLow", &ParamRecord::readGraphicLow);
	funcTable.installReadFunc("graphicHigh", &ParamRecord::readGraphicHigh);
	funcTable.installReadFunc("controlLow", &ParamRecord::readControlLow);
	funcTable.installReadFunc("controlHigh", &ParamRecord::readControlHigh);
	funcTable.installReadFunc("precision", &ParamRecord::readPrecision);

	funcTable.installReadFunc("value", &ParamBlobRecord::readValue);
}

ParamBlobRecord::~ParamBlobRecord()
{
}

epicsShareFunc aitEnum ParamBlobRecord::bestExternalType() const
{
	return aitEnumUint32;
}

epicsShareFunc unsigned ParamBlobRecord::maxDimension() const
{
	return 1u;
}

epicsShareFunc aitIndex ParamBlobRecord::maxBound(unsigned dimension) const
{
	aitIndex retVal;
	if(dimension == 0) {
		retVal = index;
	} else {
		retVal = 1u;
	}
	return retVal;
}

gddAppFuncTableStatus ParamBlobRecord::readValue(gdd &value)
{
	FILE *blob;
	size_t size;
	int retVal;

	retVal = Param_getBlob(param, name, idx, &size, &blob);
	index = 2 + (((unsigned int) size - 1) / sizeof(aitUint32));
	if((retVal != 0) || (index == 0) || (index > EPICS_MAX_INDEX)) {
		return S_cas_noRead;
	} else {
		aitUint32 *intValue = new aitUint32[index];
		*intValue = size;
		fread(intValue + 1, sizeof(aitUint32), index - 1, blob);
		fclose(blob);

		value.putRef(intValue);
	}

	return S_casApp_success;
}

caStatus ParamBlobRecord::scan()
{
	FILE *blob;
	size_t size;
	int retVal;

	caServer *pCAS = this->getCAS();

	retVal = Param_getBlob(param, name, idx, &size, &blob);
	index = 2 + (((unsigned int) size - 1) / sizeof(aitUint32));
	if((retVal != 0) || (index == 0) || (index > EPICS_MAX_INDEX)) {
		return S_cas_noRead;
	} else {
		aitUint32 *intValue = new aitUint32[index];
		*intValue = size;
		fread(intValue + 1, sizeof(aitUint32), index - 1, blob);
		fclose(blob);

		val = new gddAtomic(gddAppType_value, aitEnumString, 1, index);

		val->putRef(intValue, pDest);
	}

	val->setStat(epicsAlarmNone);
	val->setSevr(epicsSevNone);

	if (this->interest == aitTrue && pCAS != NULL) {
		casEventMask select(pCAS->valueEventMask|pCAS->logEventMask|pCAS->alarmEventMask);
		this->postEvent (select, *val);
	}

	return S_cas_success;
}

caStatus ParamBlobRecord::read(const casCtx &ctx, gdd &prototype)
{
	return ((scan() == S_cas_success) && funcTable.read(*this, prototype));
}

caStatus ParamBlobRecord::write(const casCtx &ctx, gdd &value)
{
	return S_cas_noWrite;
}

