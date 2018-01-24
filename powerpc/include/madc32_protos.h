#ifndef __MADC32_PROTOS_H__
#define __MADC32_PROTOS_H__

#include "mapping_database.h"
#include "madc32_database.h"
#include "madc32.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32_protos.h
//! \brief			Definitions for Mesytec Madc32 ADC
//! \details		Prototypes for Mesytec Madc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/


bool_t madc32_fillStruct(s_madc32 * s, char * file);

void madc32_loadFromDb(s_madc32 * s, uint32_t chnPattern);
bool_t madc32_dumpRegisters(s_madc32 * s, char * file);
void madc32_printDb(s_madc32 * s);

void madc32_setThreshold(s_madc32 * s, uint16_t channel,  uint16_t thresh);
void madc32_setThreshold_db(s_madc32 * s, uint16_t channel);
uint16_t madc32_getThreshold(s_madc32 * s, uint16_t channel);

void madc32_setBankOperation(s_madc32 * s, uint16_t oper);
void madc32_setBankOperation_db(s_madc32 * s);
uint16_t madc32_getBankOperation(s_madc32 * s);

void madc32_setAdcResolution(s_madc32 * s, uint16_t res);
void madc32_setAdcResolution_db(s_madc32 * s);
uint16_t madc32_getAdcResolution(s_madc32 * s);

void madc32_setOutputFormat(s_madc32 * s, uint16_t format);
void madc32_setOutputFormat_db(s_madc32 * s);
uint16_t madc32_getOutputFormat(s_madc32 * s);

void madc32_setAdcOverride(s_madc32 * s, uint16_t over);
void madc32_setAdcOverride_db(s_madc32 * s);
uint16_t madc32_getAdcOverride(s_madc32 * s);

void madc32_setSlidingScaleOff(s_madc32 * s, bool_t flag);
void madc32_setSlidingScaleOff_db(s_madc32 * s);
bool_t madc32_getSlidingScaleOff(s_madc32 * s);

void madc32_setSkipOutOfRange(s_madc32 * s, bool_t flag);
void madc32_setSkipOutOfRange_db(s_madc32 * s);
bool_t madc32_getSkipOutOfRange(s_madc32 * s);

void madc32_setIgnoreThresholds(s_madc32 * s, bool_t flag);
void madc32_setIgnoreThresholds_db(s_madc32 * s);
bool_t madc32_getIgnoreThresholds(s_madc32 * s);

void madc32_setHoldDelay(s_madc32 * s, uint16_t gg, uint16_t delay);
void madc32_setHoldDelay_db(s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldDelay(s_madc32 * s, uint16_t gg);

void madc32_setHoldWidth(s_madc32 * s, uint16_t gg, uint16_t width);
void madc32_setHoldWidth_db(s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldWidth(s_madc32 * s, uint16_t gg);

void madc32_useGG(s_madc32 * s, uint16_t gg);
void madc32_useGG_db(s_madc32 * s);
uint16_t madc32_getGGUsed(s_madc32 * s);

void madc32_setInputRange(s_madc32 * s, uint16_t range);
void madc32_setInputRange_db(s_madc32 * s);
uint16_t madc32_getInputRange(s_madc32 * s);

void madc32_setEclTerm(s_madc32 * s, uint16_t term);
void madc32_setEclTerm_db(s_madc32 * s);
uint16_t madc32_getEclTerm(s_madc32 * s);

void madc32_setEclG1Osc(s_madc32 * s, uint16_t go);
void madc32_setEclG1Osc_db(s_madc32 * s);
uint16_t madc32_getEclG1Osc(s_madc32 * s);

void madc32_setEclFclRts(s_madc32 * s, uint16_t fr);
void madc32_setEclFclRts_db(s_madc32 * s);
uint16_t madc32_getEclFclRts(s_madc32 * s);

void madc32_setNimG1Osc(s_madc32 * s, uint16_t go);
void madc32_setNimG1Osc_db(s_madc32 * s);
uint16_t madc32_getNimG1Osc(s_madc32 * s);

void madc32_setNimFclRts(s_madc32 * s, uint16_t fr);
void madc32_setNimFclRts_db(s_madc32 * s);
uint16_t madc32_getNimFclRts(s_madc32 * s);

void madc32_setNimBusy(s_madc32 * s, uint16_t busy);
void madc32_setNimBusy_db(s_madc32 * s);
uint16_t madc32_getNimBusy(s_madc32 * s);

void madc32_setPulserStatus(s_madc32 * s, uint16_t mode);
void madc32_setPulserStatus_db(s_madc32 * s);
uint16_t madc32_getPulserStatus(s_madc32 * s);

#endif
