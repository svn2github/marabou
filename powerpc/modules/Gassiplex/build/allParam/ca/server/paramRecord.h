#ifndef PARAMRECORD_H
#define PARAMRECORD_H

#include <gdd.h>

extern "C" {
  #include <allParam.h>
}

#include "record.h"
#include "arrayDest.h"

class ParamRecord : public Record {
  protected:
	ArrayDestructor *pDest;
	const Param *param;
	char pPVName[PARAM_MAX_VALUE_LEN];
	char name[PARAM_MAX_VALUE_LEN];
	char idx[PARAM_MAX_VALUE_LEN];

  public:
	ParamRecord(caServer&, const Param *, const char *, const char *);
	~ParamRecord();

	const char *getPVName();

	virtual gddAppFuncTableStatus readAlarmLow(gdd &);
	virtual gddAppFuncTableStatus readAlarmHigh(gdd &);
	virtual gddAppFuncTableStatus readAlarmLowWarning(gdd &);
	virtual gddAppFuncTableStatus readAlarmHighWarning(gdd &);
	virtual gddAppFuncTableStatus readGraphicLow(gdd &);
	virtual gddAppFuncTableStatus readGraphicHigh(gdd &);
	virtual gddAppFuncTableStatus readControlLow(gdd &);
	virtual gddAppFuncTableStatus readControlHigh(gdd &);
	virtual gddAppFuncTableStatus readPrecision(gdd &);

	virtual gddAppFuncTableStatus readValue(gdd &) = 0;

	virtual caStatus scan() = 0;
	virtual caStatus read(const casCtx &, gdd &) = 0;
	virtual caStatus write(const casCtx &, gdd &) = 0;
};

#endif /* !PARAMRECORD_H */

