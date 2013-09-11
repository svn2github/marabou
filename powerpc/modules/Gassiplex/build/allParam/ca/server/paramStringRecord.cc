static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramStringRecord.cc,v 1.11 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <string.h>
}

#include "paramStringRecord.h"

ParamStringRecord::ParamStringRecord(caServer& cas, const Param *p, const char *rn, const char *u) :
  ParamRecord(cas, p, rn, u)
{
	int rows;
	char *tmp[PARAM_MAX_ARRAY_LEN];

	pDest = new ArrayStringDestructor;

	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		tmp[i] = new char[PARAM_MAX_VALUE_LEN];
	}
	if(Param_getStringArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, tmp) == 0) {
		index = rows == 0 ? 1 : (unsigned int) rows;
	} else {
		index = 1;
	}
	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		delete [] tmp[i];
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

	funcTable.installReadFunc("value", &ParamStringRecord::readValue);
}

ParamStringRecord::~ParamStringRecord()
{
	for(unsigned int k = 0 ; k < index ; k++) {
		delete ourValue[k];
	}
}

epicsShareFunc aitEnum ParamStringRecord::bestExternalType() const
{
	return aitEnumString;
}

epicsShareFunc unsigned ParamStringRecord::maxDimension() const
{
	return 1u;
}

epicsShareFunc aitIndex ParamStringRecord::maxBound(unsigned dimension) const
{
	aitIndex retVal;
	if(dimension == 0) {
		retVal = index;
	} else {
		retVal = 1u;
	}
	return retVal;
}

gddAppFuncTableStatus ParamStringRecord::readValue(gdd &value)
{
	gddAppFuncTableStatus retVal;
	int pretVal;
	int rows;

	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		ourValue[i] = new char[PARAM_MAX_VALUE_LEN];
	}

	pretVal = Param_getStringArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, ourValue);
	if((pretVal != 0) || ((index = (unsigned int) rows ) == 0)) {
		index = 1;
		retVal = S_cas_noRead; 
	} else if(index == 1) {
		aitString stringValue = ourValue[0];

		value.putConvert(stringValue);
		retVal = S_casApp_success;
	} else {
		aitString *stringValue;
		stringValue = new aitString[index];
		for (unsigned int k = 0 ; k < index ; k++) {
			stringValue[k] = ourValue[k];
		}

		value.putRef(stringValue);
		retVal = S_casApp_success;
	}

	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		delete [] ourValue[i];
	}

	return retVal;
}

caStatus ParamStringRecord::scan()
{
	caStatus retVal;
	caServer *pCAS = this->getCAS();
	int pretVal;
	int rows;

	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		ourValue[i] = new char[PARAM_MAX_VALUE_LEN];
	}

	pretVal = Param_getStringArray(param, name, idx, PARAM_MAX_ARRAY_LEN, &rows, ourValue);
	if((pretVal != 0) || ((index = (unsigned int) rows ) == 0)) {
		index = 1;
		retVal = S_cas_noRead; 
	} else if(index == 1) {
		aitString stringValue = ourValue[0];

		val = new gddScalar(gddAppType_value, aitEnumString);
		val->putConvert(stringValue);
		retVal = S_casApp_success;
	} else {
		aitString *stringValue;
		stringValue = new aitString[index];
		for (unsigned int k = 0 ; k < index ; k++) {
			stringValue[k] = ourValue[k];
		}

		val = new gddAtomic(gddAppType_value, aitEnumString, 1, index);

		val->putRef(stringValue, pDest);

		retVal = S_casApp_success;
	}

	for (int i = 0 ; i < PARAM_MAX_ARRAY_LEN ; i++) {
		delete [] ourValue[i];
	}

	if (this->interest == aitTrue && pCAS != NULL) {
		casEventMask select(pCAS->valueEventMask|pCAS->logEventMask|pCAS->alarmEventMask);
		this->postEvent (select, *val);
	}

	return retVal;
}

caStatus ParamStringRecord::read(const casCtx &ctx, gdd &prototype)
{
	return ((scan() == S_cas_success) && funcTable.read(*this, prototype));
}

caStatus ParamStringRecord::write(const casCtx &ctx, gdd &value)
{
	aitString stringValue;

	value.getConvert(stringValue);
	Param_storeString(param, name, idx, (const char *) stringValue);

	return S_cas_success;
}

