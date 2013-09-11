#ifndef PARAMBLOBRECORD_H
#define PARAMBLOBRECORD_H

#include "paramRecord.h"

class ParamBlobRecord : public ParamRecord {
  private:
	aitIndex index;
	gddAppFuncTable<ParamBlobRecord> funcTable;

  public:
	ParamBlobRecord(caServer&, const Param *, const char *, const char *);
	~ParamBlobRecord();

	epicsShareFunc aitEnum bestExternalType() const;
	epicsShareFunc unsigned maxDimension() const;
	epicsShareFunc aitIndex maxBound(unsigned int) const;
	gddAppFuncTableStatus readValue(gdd &);
	caStatus scan();
	caStatus read(const casCtx &, gdd &);
	caStatus write(const casCtx &, gdd &);
};

#endif /* !PARAMBLOBRECORD_H */

