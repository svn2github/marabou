#ifndef __MQDC32_PROTOS_H__
#define __MQDC32_PROTOS_H__

#include "mapping_database.h"
#include "mqdc32_database.h"
#include "mqdc32.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mqdc32_protos.h
//! \brief			Definitions for Mesytec mqdc32 ADC
//! \details		Prototypes for Mesytec mqdc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.14 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/


bool_t mqdc32_fillStruct(s_mqdc32 * s, char * file);
void mqdc32_loadFromDb(s_mqdc32 * s, uint32_t chnPattern);
bool_t mqdc32_dumpRegisters(s_mqdc32 * s, char * file);
void mqdc32_printDb(s_mqdc32 * s);
void mqdc32_setThreshold(s_mqdc32 * s, uint16_t channel,  uint16_t thresh);
void mqdc32_setThreshold_db(s_mqdc32 * s, uint16_t channel);
uint16_t mqdc32_getThreshold(s_mqdc32 * s, uint16_t channel);
void mqdc32_setBankOperation(s_mqdc32 * s, uint16_t oper);
void mqdc32_setBankOperation_db(s_mqdc32 * s);
uint16_t mqdc32_getBankOperation(s_mqdc32 * s);
void mqdc32_setAdcResolution(s_mqdc32 * s, uint16_t res);
void mqdc32_setAdcResolution_db(s_mqdc32 * s);
uint16_t mqdc32_getAdcResolution(s_mqdc32 * s);
void mqdc32_setBankOffset(s_mqdc32 * s, uint16_t bank, uint16_t offset);
void mqdc32_setBankOffset_db(s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getBankOffset(s_mqdc32 * s, uint16_t bank);
void mqdc32_setGateLimit(s_mqdc32 * s, uint16_t bank, uint16_t limit);
void mqdc32_setGateLimit_db(s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getGateLimit(s_mqdc32 * s, uint16_t bank);
void mqdc32_setSlidingScaleOff(s_mqdc32 * s, bool_t flag);
void mqdc32_setSlidingScaleOff_db(s_mqdc32 * s);
bool_t mqdc32_getSlidingScaleOff(s_mqdc32 * s);
void mqdc32_setSkipOutOfRange(s_mqdc32 * s, bool_t flag);
void mqdc32_setSkipOutOfRange_db(s_mqdc32 * s);
bool_t mqdc32_getSkipOutOfRange(s_mqdc32 * s);
void mqdc32_setIgnoreThresholds(s_mqdc32 * s, bool_t flag);
void mqdc32_setIgnoreThresholds_db(s_mqdc32 * s);
bool_t mqdc32_getIgnoreThresholds(s_mqdc32 * s);
void mqdc32_setInputCoupling(s_mqdc32 * s, uint16_t range);
void mqdc32_setInputCoupling_db(s_mqdc32 * s);
uint16_t mqdc32_getInputCoupling(s_mqdc32 * s);
void mqdc32_setEclTerm(s_mqdc32 * s, uint16_t term);
void mqdc32_setEclTerm_db(s_mqdc32 * s);
uint16_t mqdc32_getEclTerm(s_mqdc32 * s);
void mqdc32_setEclG1Osc(s_mqdc32 * s, uint16_t go);
void mqdc32_setEclG1Osc_db(s_mqdc32 * s);
uint16_t mqdc32_getEclG1Osc(s_mqdc32 * s);
void mqdc32_setEclFclRts(s_mqdc32 * s, uint16_t fr);
void mqdc32_setEclFclRts_db(s_mqdc32 * s);
uint16_t mqdc32_getEclFclRts(s_mqdc32 * s);
void mqdc32_setGateSelect(s_mqdc32 * s, uint16_t nimOrEcl);
void mqdc32_setGateSelect_db(s_mqdc32 * s);
uint16_t mqdc32_getGateSelect(s_mqdc32 * s);
void mqdc32_setNimG1Osc(s_mqdc32 * s, uint16_t go);
void mqdc32_setNimG1Osc_db(s_mqdc32 * s);
uint16_t mqdc32_getNimG1Osc(s_mqdc32 * s);
void mqdc32_setNimFclRts(s_mqdc32 * s, uint16_t fr);
void mqdc32_setNimFclRts_db(s_mqdc32 * s);
uint16_t mqdc32_getNimFclRts(s_mqdc32 * s);
void mqdc32_setNimBusy(s_mqdc32 * s, uint16_t busy);
void mqdc32_setNimBusy_db(s_mqdc32 * s);
uint16_t mqdc32_getNimBusy(s_mqdc32 * s);
void mqdc32_setPulserStatus(s_mqdc32 * s, uint16_t mode);
void mqdc32_setulserStatus_db(s_mqdc32 * s);
uint16_t mqdc32_getPulserStatus(s_mqdc32 * s);
void mqdc32_setMultLimit(s_mqdc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim);
void mqdc32_setMultLimit_db(s_mqdc32 * s, uint16_t bnk);
uint16_t mqdc32_getMultHighLimit(s_mqdc32 * s, uint16_t bnk);
uint16_t mqdc32_getMultLowLimit(s_mqdc32 * s, uint16_t bnk);
#endif
