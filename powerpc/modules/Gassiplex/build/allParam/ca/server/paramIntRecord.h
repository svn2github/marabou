#ifndef PARAMINTRECORD_H
#define PARAMINTRECORD_H

#include "paramRecord.h"
#include "arrayDest.h"

class ParamIntRecord : public ParamRecord {
  private:
	aitIndex index;
	unsigned long int ourValue[PARAM_MAX_ARRAY_LEN];
	gddAppFuncTable<ParamIntRecord> funcTable;

  public:
	ParamIntRecord(caServer&, const Param *, const char *, const char *);
	~ParamIntRecord();

	epicsShareFunc aitEnum bestExternalType() const;
	epicsShareFunc unsigned maxDimension() const;
	epicsShareFunc aitIndex maxBound(unsigned int) const;
	gddAppFuncTableStatus readValue(gdd &);
	caStatus scan();
	caStatus read(const casCtx &, gdd &);
	caStatus write(const casCtx &, gdd &);
};

#endif /* !PARAMINTRECORD_H */

