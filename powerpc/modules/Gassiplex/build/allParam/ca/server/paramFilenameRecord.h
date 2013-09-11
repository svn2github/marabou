#ifndef PARAMFILENAMERECORD_H
#define PARAMFILENAMERECORD_H

#include "paramRecord.h"

class ParamFilenameRecord : public ParamRecord {
  private:
	aitIndex index;
	char *ourValue[PARAM_MAX_ARRAY_LEN];
	gddAppFuncTable<ParamFilenameRecord> funcTable;

  public:
	ParamFilenameRecord(caServer&, const Param *, const char *, const char *);
	~ParamFilenameRecord();

	epicsShareFunc aitEnum bestExternalType() const;
	epicsShareFunc unsigned maxDimension() const;
	epicsShareFunc aitIndex maxBound(unsigned int) const;
	gddAppFuncTableStatus readValue(gdd &);
	caStatus scan();
	caStatus read(const casCtx &, gdd &);
	caStatus write(const casCtx &, gdd &);
};

#endif /* !PARAMFILENAMERECORD_H */

