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


struct s_mqdc32 * mqdc32_alloc(char * moduleName, struct s_mapDescr * md, int serial);

void mqdc32_moduleInfo(struct s_mqdc32 * s);
void mqdc32_setPrefix(struct s_mqdc32 * s, char * prefix);

bool_t mqdc32_fillStruct(struct s_mqdc32 * s, char * file);

void mqdc32_loadFromDb(struct s_mqdc32 * s, uint32_t chnPattern);

void mqdc32_initialize(struct s_mqdc32 * s);

void mqdc32_enableBLT(struct s_mqdc32 * s);
bool_t mqdc32_useBLT(struct s_mqdc32 * s);


bool_t mqdc32_dumpRegisters(struct s_mqdc32 * s, char * file);
bool_t mqdc32_dumpRaw(struct s_mqdc32 * s, char * file);
void mqdc32_printDb(struct s_mqdc32 * s);

void mqdc32_soft_reset(struct s_mqdc32 * s);

void mqdc32_initDefaults(struct s_mqdc32 * s);

void mqdc32_setThreshold(struct s_mqdc32 * s, uint16_t channel,  uint16_t thresh);
void mqdc32_setThreshold_db(struct s_mqdc32 * s, uint16_t channel);
uint16_t mqdc32_getThreshold(struct s_mqdc32 * s, uint16_t channel);

void mqdc32_setAddrReg(struct s_mqdc32 * s, uint16_t vmeAddr);
void mqdc32_setAddrReg_db(struct s_mqdc32 * s);
uint16_t mqdc32_getAddrReg(struct s_mqdc32 * s);

void mqdc32_setMcstCblt_db(struct s_mqdc32 * s);

void mqdc32_setModuleId(struct s_mqdc32 * s, uint16_t id);
void mqdc32_setModuleId_db(struct s_mqdc32 * s);
uint16_t mqdc32_getModuleId(struct s_mqdc32 * s);

uint16_t mqdc32_getFifoLength(struct s_mqdc32 * s);

void mqdc32_setDataWidth(struct s_mqdc32 * s, uint16_t format);
void mqdc32_setDataWidth_db(struct s_mqdc32 * s);
uint16_t mqdc32_getDataWidth(struct s_mqdc32 * s);

void mqdc32_setMultiEvent(struct s_mqdc32 * s, uint16_t mode);
void mqdc32_setMultiEvent_db(struct s_mqdc32 * s);
uint16_t mqdc32_getMultiEvent(struct s_mqdc32 * s);

void mqdc32_setXferData(struct s_mqdc32 * s, uint16_t words);
void mqdc32_setXferData_db(struct s_mqdc32 * s);
uint16_t mqdc32_getXferData(struct s_mqdc32 * s);

void mqdc32_setMarkingType(struct s_mqdc32 * s, uint16_t type);
void mqdc32_setMarkingType_db(struct s_mqdc32 * s);
uint16_t mqdc32_getMarkingType(struct s_mqdc32 * s);

void mqdc32_setBankOperation(struct s_mqdc32 * s, uint16_t oper);
void mqdc32_setBankOperation_db(struct s_mqdc32 * s);
uint16_t mqdc32_getBankOperation(struct s_mqdc32 * s);

void mqdc32_setAdcResolution(struct s_mqdc32 * s, uint16_t res);
void mqdc32_setAdcResolution_db(struct s_mqdc32 * s);
uint16_t mqdc32_getAdcResolution(struct s_mqdc32 * s);

void mqdc32_setBankOffset(struct s_mqdc32 * s, uint16_t bank, uint16_t offset);
void mqdc32_setBankOffset_db(struct s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getBankOffset(struct s_mqdc32 * s, uint16_t bank);

void mqdc32_setGateLimit(struct s_mqdc32 * s, uint16_t bank, uint16_t limit);
void mqdc32_setGateLimit_db(struct s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getGateLimit(struct s_mqdc32 * s, uint16_t bank);

void mqdc32_setSlidingScaleOff(struct s_mqdc32 * s, bool_t flag);
void mqdc32_setSlidingScaleOff_db(struct s_mqdc32 * s);
bool_t mqdc32_getSlidingScaleOff(struct s_mqdc32 * s);

void mqdc32_setSkipOutOfRange(struct s_mqdc32 * s, bool_t flag);
void mqdc32_setSkipOutOfRange_db(struct s_mqdc32 * s);
bool_t mqdc32_getSkipOutOfRange(struct s_mqdc32 * s);

void mqdc32_setIgnoreThresholds(struct s_mqdc32 * s, bool_t flag);
void mqdc32_setIgnoreThresholds_db(struct s_mqdc32 * s);
bool_t mqdc32_getIgnoreThresholds(struct s_mqdc32 * s);

void mqdc32_setInputCoupling(struct s_mqdc32 * s, uint16_t range);
void mqdc32_setInputCoupling_db(struct s_mqdc32 * s);
uint16_t mqdc32_getInputCoupling(struct s_mqdc32 * s);

void mqdc32_setEclTerm(struct s_mqdc32 * s, uint16_t term);
void mqdc32_setEclTerm_db(struct s_mqdc32 * s);
uint16_t mqdc32_getEclTerm(struct s_mqdc32 * s);

void mqdc32_setEclG1OrOsc(struct s_mqdc32 * s, uint16_t go);
void mqdc32_setEclG1OrOsc_db(struct s_mqdc32 * s);
uint16_t mqdc32_getEclG1OrOsc(struct s_mqdc32 * s);

void mqdc32_setEclFclOrRts(struct s_mqdc32 * s, uint16_t fr);
void mqdc32_setEclFclOrRts_db(struct s_mqdc32 * s);
uint16_t mqdc32_getEclFclOrRts(struct s_mqdc32 * s);

void mqdc32_setGateSelect(struct s_mqdc32 * s, uint16_t nimOrEcl);
void mqdc32_setGateSelect_db(struct s_mqdc32 * s);
uint16_t mqdc32_getGateSelect(struct s_mqdc32 * s);

void mqdc32_setNimG1OrOsc(struct s_mqdc32 * s, uint16_t go);
void mqdc32_setNimG1OrOsc_db(struct s_mqdc32 * s);
uint16_t mqdc32_getNimG1OrOsc(struct s_mqdc32 * s);

void mqdc32_setNimFclOrRts(struct s_mqdc32 * s, uint16_t fr);
void mqdc32_setNimFclOrRts_db(struct s_mqdc32 * s);
uint16_t mqdc32_getNimFclOrRts(struct s_mqdc32 * s);

void mqdc32_setNimBusy(struct s_mqdc32 * s, uint16_t busy);
void mqdc32_setNimBusy_db(struct s_mqdc32 * s);
uint16_t mqdc32_getNimBusy(struct s_mqdc32 * s);

void mqdc32_setTestPulser(struct s_mqdc32 * s, uint16_t mode);
void mqdc32_setTestPulser_db(struct s_mqdc32 * s);
uint16_t mqdc32_getTestPulser(struct s_mqdc32 * s);

void mqdc32_setTsSource(struct s_mqdc32 * s, uint16_t source);
void mqdc32_setTsSource_db(struct s_mqdc32 * s);
uint16_t mqdc32_getTsSource(struct s_mqdc32 * s);

void mqdc32_setTsDivisor(struct s_mqdc32 * s, uint16_t div);
void mqdc32_setTsDivisor_db(struct s_mqdc32 * s);
uint16_t mqdc32_getTsDivisor(struct s_mqdc32 * s);

int mqdc32_readout(struct s_mqdc32 * s, uint32_t * pointer);
bool_t mqdc32_dataReady(struct s_mqdc32 * s);
void mqdc_resetReadout(struct s_mqdc32 * s);

int mqdc32_readTimeB(struct s_mqdc32 * s, uint32_t * pointer);

bool_t mqdc32_testBusError(struct s_mqdc32 * s);
void mqdc32_enableBusError(struct s_mqdc32 * s);
void mqdc32_disableBusError(struct s_mqdc32 * s);

void mqdc32_startAcq(struct s_mqdc32 * s);
void mqdc32_stopAcq(struct s_mqdc32 * s);

void mqdc32_resetFifo(struct s_mqdc32 * s);
void mqdc32_resetTimestamp(struct s_mqdc32 * s);

void mqdc32_setMcstSignature(struct s_mqdc32 * s, unsigned long Signature);
uint16_t mqdc32_getMcstSignature(struct s_mqdc32 * s);

void mqdc32_setCbltSignature(struct s_mqdc32 * s, unsigned long Signature);
uint16_t mqdc32_getCbltSignature(struct s_mqdc32 * s);

void mqdc32_setFirstInChain(struct s_mqdc32 * s);
bool_t mqdc32_isFirstInChain(struct s_mqdc32 * s);

void mqdc32_setLastInChain(struct s_mqdc32 * s);
bool_t mqdc32_isLastInChain(struct s_mqdc32 * s);
void mqdc32_setMiddleOfChain(struct s_mqdc32 * s);
bool_t mqdc32_isMiddleOfChain(struct s_mqdc32 * s);

void mqdc32_setMcstEnable(struct s_mqdc32 * s);
void mqdc32_setMcstDisable(struct s_mqdc32 * s);
bool_t mqdc32_mcstIsEnabled(struct s_mqdc32 * s);
void mqdc32_setCbltEnable(struct s_mqdc32 * s);
void mqdc32_setCbltDisable(struct s_mqdc32 * s);
bool_t mqdc32_cbltIsEnabled(struct s_mqdc32 * s);

void mqdc32_setMcstCblt_db(struct s_mqdc32 * s);

void mqdc32_startAcq_mcst(struct s_mqdc32 * s);
void mqdc32_stopAcq_msct(struct s_mqdc32 * s);
void mqdc32_resetFifo_mcst(struct s_mqdc32 * s);
void mqdc32_resetTimestamp_mcst(struct s_mqdc32 * s);
void mqdc_resetReadout_mcst(struct s_mqdc32 * s);


bool_t mqdc32_updateSettings(struct s_mqdc32 * s, char * updFile);
#endif
