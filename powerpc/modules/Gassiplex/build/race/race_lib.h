#ifndef Race_LIB_H
#define Race_LIB_H

#include <allParam.h>

int Race_init(const char *name, const Param *param);
int Race_loadThresholds(const char *name, const Param *param);
int Race_reset(const char *name, const Param *param);
int Race_start(const char *name, const Param *param);
int Race_stop(const char *name, const Param *param);
int Race_initialized(const char *name, const Param *param);
int Race_running(const char *name, const Param *param);
int Race_lvl1Busy(const char *name, const Param *param);
int Race_lvl2Busy(const char *name, const Param *param);
int Race_error(const char *name, const Param *param);
int Race_statusRegister(const char *name, const Param *param, const char *reg);
#ifndef EPICS_RUNCTRL
int Race_status(const char *name, const Param *param);
#endif

#endif
