static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramIntRecord.cc,v 1.12 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <string.h>
}

#include "paramIntRecord.h"

ParamIntRecord::ParamIntRecord(caServer& cas, const Param *p, const char *rn, const char *u) :
  ParamRecord(cas, p, rn, u)
{
	int rows;
	unsigned long int tmp[PARAM_MAX_ARRAY_LEN];

	pDest = new ArrayIntDestructor;

	if(Param_getIntArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, tmp) == 0) {
		if ((rows == 0) || (rows == 1)) {
			index = 1;
		} else {
			index = (unsigned int) rows + 2;
		}
	} else {
		index = 1;
	}
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

	funcTable.installReadFunc("value", &ParamIntRecord::readValue);
}

ParamIntRecord::~ParamIntRecord()
{
}

epicsShareFunc aitEnum ParamIntRecord::bestExternalType() const
{
	return aitEnumUint32;
}

epicsShareFunc unsigned ParamIntRecord::maxDimension() const
{
	return 1u;
}

epicsShareFunc aitIndex ParamIntRecord::maxBound(unsigned dimension) const
{
	aitIndex retVal;
	if(dimension == 0) {
		retVal = index;
	} else {
		retVal = 1u;
	}
	return retVal;
}

gddAppFuncTableStatus ParamIntRecord::readValue(gdd &value)
{
	int rows;
	int retVal;

	retVal = Param_getIntArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, ourValue);
	if((retVal != 0) || (rows == 0)) {
		index = 1;
		return S_cas_noRead;
	} else if(rows == 1) {
		index = 1;
		aitUint32 intValue;

		intValue = ourValue[0];

		value.putConvert(intValue);
	} else {
		index = (unsigned int) rows + 2;
		aitUint32 *intValue = new aitUint32[index];
		memcpy(intValue + 2, ourValue, sizeof(aitUint32) * (index - 2));

		value.putRef(intValue);
		delete [] intValue;
	}

	return S_casApp_success;
}

caStatus ParamIntRecord::scan()
{
	caServer *pCAS = this->getCAS();
	int rows;
	int retVal;

	retVal = Param_getIntArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, ourValue);
	if((retVal != 0) || (rows == 0)) {
		index = 1;
		return S_cas_noRead;
	} else if(rows == 1) {
		index = 1;
		aitUint32 intValue;

		val = new gddScalar(gddAppType_value, aitEnumUint32);

		intValue = ourValue[0];

		val->putConvert(intValue);
	} else {
		index = (unsigned int) rows + 2;
		aitUint32 *intValue = new aitUint32[index];
		memcpy(intValue + 2, ourValue, sizeof(aitUint32) * (index - 2));

		val = new gddAtomic(gddAppType_value, aitEnumUint32, 1, index);

		val->putRef(intValue, pDest);
		delete [] intValue;
	}

	val->setStat(epicsAlarmNone);
	val->setSevr(epicsSevNone);

	if (this->interest == aitTrue && pCAS != NULL) {
		casEventMask select(pCAS->valueEventMask|pCAS->logEventMask|pCAS->alarmEventMask);
		this->postEvent (select, *val);
	}

	return S_cas_success;
}

caStatus ParamIntRecord::read(const casCtx &ctx, gdd &prototype)
{
	return ((scan() == S_cas_success) && funcTable.read(*this, prototype));
}

caStatus ParamIntRecord::write(const casCtx &ctx, gdd &value)
{
	aitUint32 intValue;

	value.getConvert(intValue);

	Param_storeInt(param, name, idx, (unsigned long int) intValue);

	return S_cas_success;
}

