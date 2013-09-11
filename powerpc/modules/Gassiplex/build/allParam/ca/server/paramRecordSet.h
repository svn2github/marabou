#ifndef PARAMRECORDSET_H
#define PARAMRECORDSET_H

#include <casdef.h>

extern "C" {
  #include <allParam.h>
}

#include "paramRecord.h"

#define MAX_PARAM_SRC 128
#define MAX_NUM_PV 1024

class ParamRecordSet : public caServer {
  private:
    int numPv;
    ParamRecord *pvs[MAX_NUM_PV];

    int numParamSrc;
    Param *param[MAX_PARAM_SRC];
	Param *pParam(const char *);

  public:
	ParamRecordSet(unsigned int);
	~ParamRecordSet();
	pvExistReturn pvExistTest(const casCtx &, const char *);
	pvCreateReturn createPV(const casCtx &, const char *);
};

#endif /* !PARAMRECORDSET_H */

