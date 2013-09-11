#ifndef Dtu_LIB_H
#define Dtu_LIB_H

#include <allParam.h>

int Dtu_init(const char *name, const Param * param);
int Dtu_loadThresholds(const char *name, const Param * param);
int Dtu_reset(const char *name, const Param * param);
int Dtu_start(const char *name, const Param * param);
int Dtu_stop(const char *name, const Param * param);
int Dtu_initialized(const char *name, const Param * param);
int Dtu_running(const char *name, const Param * param);
int Dtu_lvl1Busy(const char *name, const Param * param);
int Dtu_lvl2Busy(const char *name, const Param * param);
int Dtu_error(const char *name, const Param * param);
int Dtu_statusRegister(const char *name, const Param * param, const char *reg);
#ifndef EPICS_RUNCTRL
int Dtu_status(const char *name, const Param * param);
#endif

#endif
