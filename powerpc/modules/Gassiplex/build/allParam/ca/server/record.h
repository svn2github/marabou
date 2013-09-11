#ifndef RECORD_H
#define RECORD_H

#include <casdef.h>

#include <osiTimer.h>
#include <aitTypes.h>
#include <aitHelpers.h>

#include <gddApps.h>
#include <gddAppFuncTable.h>

#define NAMELENGTH 28

class scanTimer;

class Record : public casPV {
  protected:
	char name[NAMELENGTH + 1];
	const char *units;
	aitEnum type;
	gdd *val;
	scanTimer *recordScanTimer;
	aitBool interest;
	epicsAlarmCondition alarmStatus;
	epicsAlarmSeverity alarmSeverity;
  public:
	Record(caServer&, const char*, const char*, aitEnum);
	~Record();

	void destroy();
	caStatus interestRegister();
	void interestDelete();
	const char *getName() const;
	aitEnum bestExternalType() const;

	gddAppFuncTableStatus readUnits(gdd &);
	gddAppFuncTableStatus readStatus(gdd &);
	gddAppFuncTableStatus readSeverity(gdd &);
	gddAppFuncTableStatus readSeconds(gdd &);

	virtual gddAppFuncTableStatus readValue(gdd &) = 0;
	virtual caStatus scan() = 0;
	virtual caStatus read(const casCtx &, gdd &) = 0;
	virtual caStatus write(const casCtx &, gdd &) = 0;
};

class scanTimer : public osiTimer {
  private:
	Record &procVar;
  public:
	scanTimer(Record &);
	void expire();
	osiBool again() const;
};

#endif /* !RECORD_H */

