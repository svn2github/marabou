#ifndef PARAMSTRINGRECORD_H
#define PARAMSTRINGRECORD_H

#include "paramRecord.h"

class ParamStringRecord : public ParamRecord {
  private:
	aitIndex index;
	char *ourValue[PARAM_MAX_ARRAY_LEN];
	gddAppFuncTable<ParamStringRecord> funcTable;

  public:
	ParamStringRecord(caServer&, const Param *, const char *, const char *);
	~ParamStringRecord();

	epicsShareFunc aitEnum bestExternalType() const;
	epicsShareFunc unsigned maxDimension() const;
	epicsShareFunc aitIndex maxBound(unsigned int) const;
	gddAppFuncTableStatus readValue(gdd &);
	caStatus scan();
	caStatus read(const casCtx &, gdd &);
	caStatus write(const casCtx &, gdd &);
};

#endif /* !PARAMSTRINGRECORD_H */

