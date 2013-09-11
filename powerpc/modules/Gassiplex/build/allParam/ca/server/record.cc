static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/record.cc,v 1.4 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <string.h>
  #include <time.h>
}

#include "record.h"

/******************** 
 * record Functions *
 ********************/

Record::Record(caServer &cas, const char *n, const char *u, aitEnum t) : casPV(cas), units(u), type(t) {
	strcpy (name, n);
	interest = aitFalse;

	alarmStatus = epicsAlarmNone;
	alarmSeverity = epicsSevNone;

	recordScanTimer = new scanTimer(*this);
}

Record::~Record() {
}

/* Misc Functions which are the same in all records */

void Record::destroy() {
}

caStatus Record::interestRegister() {
	interest = aitTrue;
	return S_casApp_success;
}

void Record::interestDelete() {
	interest = aitFalse;
}

const char *Record::getName() const {
	return name;
}

aitEnum Record::bestExternalType() const {
	return type;
}

/* Read Functions which are the same for all records */

gddAppFuncTableStatus Record::readUnits(gdd &value) {
	value.putConvert(*units);
	return S_casApp_success;
}

gddAppFuncTableStatus Record::readStatus(gdd &value) {
	value.putConvert((unsigned int) alarmStatus);
	return S_casApp_success;
}

gddAppFuncTableStatus Record::readSeverity(gdd &value) {
	value.putConvert((unsigned int) alarmSeverity);
	return S_casApp_success;
}

gddAppFuncTableStatus Record::readSeconds(gdd &value) {
	time_t now;
	now = time(NULL);
	value.putConvert((aitUint32) now);
	return S_casApp_success;
}

/***********************
 * scanTimer Functions *
 ***********************/

scanTimer::scanTimer (Record &pv) : osiTimer(1.0), procVar(pv) {
}

void scanTimer::expire() {
	procVar.scan();
}

osiBool scanTimer::again() const {
	return osiTrue;
}

