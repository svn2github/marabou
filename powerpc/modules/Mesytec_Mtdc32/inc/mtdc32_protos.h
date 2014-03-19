#ifndef __MTDC32_PROTOS_H__
#define __MTDC32_PROTOS_H__

#include "mapping_database.h"
#include "mtdc32_database.h"
#include "mtdc32.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mtdc32_protos.h
//! \brief			Definitions for Mesytec Mtdc32 TDC
//! \details		Prototypes for Mesytec Mtdc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.14 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/


struct s_mtdc32 * mtdc32_alloc(char * moduleName, struct s_mapDescr * md, int serial);

void mtdc32_moduleInfo(struct s_mtdc32 * s);
void mtdc32_setPrefix(struct s_mtdc32 * s, char * prefix);

bool_t mtdc32_fillStruct(struct s_mtdc32 * s, char * file);

void mtdc32_loadFromDb(struct s_mtdc32 * s, uint32_t chnPattern);

void mtdc32_initialize(struct s_mtdc32 * s);

void mtdc32_enableBLT(struct s_mtdc32 * s);
bool_t mtdc32_useBLT(struct s_mtdc32 * s);

bool_t mtdc32_dumpRegisters(struct s_mtdc32 * s, char * file);
bool_t mtdc32_dumpRaw(struct s_mtdc32 * s, char * file);
void mtdc32_printDb(struct s_mtdc32 * s);

void mtdc32_soft_reset(struct s_mtdc32 * s);

void mtdc32_initDefaults(struct s_mtdc32 * s);

void mtdc32_setThreshold(struct s_mtdc32 * s, uint16_t channel,  uint16_t thresh);
void mtdc32_setThreshold_db(struct s_mtdc32 * s, uint16_t channel);
uint16_t mtdc32_getThreshold(struct s_mtdc32 * s, uint16_t channel);

void mtdc32_setAddrReg(struct s_mtdc32 * s, uint16_t vmeAddr);
void mtdc32_setAddrReg_db(struct s_mtdc32 * s);
uint16_t mtdc32_getAddrReg(struct s_mtdc32 * s);

void mtdc32_setModuleId(struct s_mtdc32 * s, uint16_t id);
void mtdc32_setModuleId_db(struct s_mtdc32 * s);
uint16_t mtdc32_getModuleId(struct s_mtdc32 * s);

uint16_t mtdc32_getFifoLength(struct s_mtdc32 * s);

void mtdc32_setDataWidth(struct s_mtdc32 * s, uint16_t format);
void mtdc32_setDataWidth_db(struct s_mtdc32 * s);
uint16_t mtdc32_getDataWidth(struct s_mtdc32 * s);

void mtdc32_setMultiEvent(struct s_mtdc32 * s, uint16_t mode);
void mtdc32_setMultiEvent_db(struct s_mtdc32 * s);
uint16_t mtdc32_getMultiEvent(struct s_mtdc32 * s);

void mtdc32_setXferData(struct s_mtdc32 * s, uint16_t words);
void mtdc32_setXferData_db(struct s_mtdc32 * s);
uint16_t mtdc32_getXferData(struct s_mtdc32 * s);

void mtdc32_setMarkingType(struct s_mtdc32 * s, uint16_t type);
void mtdc32_setMarkingType_db(struct s_mtdc32 * s);
uint16_t mtdc32_getMarkingType(struct s_mtdc32 * s);

void mtdc32_setBankOperation(struct s_mtdc32 * s, uint16_t oper);
void mtdc32_setBankOperation_db(struct s_mtdc32 * s);
uint16_t mtdc32_getBankOperation(struct s_mtdc32 * s);

void mtdc32_setAdcResolution(struct s_mtdc32 * s, uint16_t res);
void mtdc32_setAdcResolution_db(struct s_mtdc32 * s);
uint16_t mtdc32_getAdcResolution(struct s_mtdc32 * s);

void mtdc32_setOutputFormat(struct s_mtdc32 * s, uint16_t format);
void mtdc32_setOutputFormat_db(struct s_mtdc32 * s);
uint16_t mtdc32_getOutputFormat(struct s_mtdc32 * s);

void mtdc32_setAdcOverride(struct s_mtdc32 * s, uint16_t over);
void mtdc32_setAdcOverride_db(struct s_mtdc32 * s);
uint16_t mtdc32_getAdcOverride(struct s_mtdc32 * s);

void mtdc32_setSlidingScaleOff(struct s_mtdc32 * s, bool_t flag);
void mtdc32_setSlidingScaleOff_db(struct s_mtdc32 * s);
bool_t mtdc32_getSlidingScaleOff(struct s_mtdc32 * s);

void mtdc32_setSkipOutOfRange(struct s_mtdc32 * s, bool_t flag);
void mtdc32_setSkipOutOfRange_db(struct s_mtdc32 * s);
bool_t mtdc32_getSkipOutOfRange(struct s_mtdc32 * s);

void mtdc32_setIgnoreThresholds(struct s_mtdc32 * s, bool_t flag);
void mtdc32_setIgnoreThresholds_db(struct s_mtdc32 * s);
bool_t mtdc32_getIgnoreThresholds(struct s_mtdc32 * s);

void mtdc32_setHoldDelay(struct s_mtdc32 * s, uint16_t gg, uint16_t delay);
void mtdc32_setHoldDelay_db(struct s_mtdc32 * s, uint16_t gg);
uint16_t mtdc32_getHoldDelay(struct s_mtdc32 * s, uint16_t gg);

void mtdc32_setHoldWidth(struct s_mtdc32 * s, uint16_t gg, uint16_t width);
void mtdc32_setHoldWidth_db(struct s_mtdc32 * s, uint16_t gg);
uint16_t mtdc32_getHoldWidth(struct s_mtdc32 * s, uint16_t gg);

void mtdc32_useGG(struct s_mtdc32 * s, uint16_t gg);
void mtdc32_useGG_db(struct s_mtdc32 * s);
uint16_t mtdc32_getGGUsed(struct s_mtdc32 * s);

void mtdc32_setInputRange(struct s_mtdc32 * s, uint16_t range);
void mtdc32_setInputRange_db(struct s_mtdc32 * s);
uint16_t mtdc32_getInputRange(struct s_mtdc32 * s);

void mtdc32_setEclTerm(struct s_mtdc32 * s, uint16_t term);
void mtdc32_setEclTerm_db(struct s_mtdc32 * s);
uint16_t mtdc32_getEclTerm(struct s_mtdc32 * s);

void mtdc32_setEclG1OrOsc(struct s_mtdc32 * s, uint16_t go);
void mtdc32_setEclG1OrOsc_db(struct s_mtdc32 * s);
uint16_t mtdc32_getEclG1OrOsc(struct s_mtdc32 * s);

void mtdc32_setEclFclOrRts(struct s_mtdc32 * s, uint16_t fr);
void mtdc32_setEclFclOrRts_db(struct s_mtdc32 * s);
uint16_t mtdc32_getEclFclOrRts(struct s_mtdc32 * s);

void mtdc32_setNimG1OrOsc(struct s_mtdc32 * s, uint16_t go);
void mtdc32_setNimG1OrOsc_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNimG1OrOsc(struct s_mtdc32 * s);

void mtdc32_setNimFclOrRts(struct s_mtdc32 * s, uint16_t fr);
void mtdc32_setNimFclOrRts_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNimFclOrRts(struct s_mtdc32 * s);

void mtdc32_setNimBusy(struct s_mtdc32 * s, uint16_t busy);
void mtdc32_setNimBusy_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNimBusy(struct s_mtdc32 * s);

void mtdc32_setBufferThresh_db(struct s_mtdc32 * s);
void mtdc32_setBufferThresh(struct s_mtdc32 * s, uint16_t thresh);
uint16_t mtdc32_getBufferThresh(struct s_mtdc32 * s);

void mtdc32_setTestPulser(struct s_mtdc32 * s, uint16_t mode);
void mtdc32_setTestPulser_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTestPulser(struct s_mtdc32 * s);

void mtdc32_setTsSource(struct s_mtdc32 * s, uint16_t source);
void mtdc32_setTsSource_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTsSource(struct s_mtdc32 * s);

void mtdc32_setTsDivisor(struct s_mtdc32 * s, uint16_t div);
void mtdc32_setTsDivisor_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTsDivisor(struct s_mtdc32 * s);

int mtdc32_readout(struct s_mtdc32 * s, uint32_t * pointer);
bool_t mtdc32_dataReady(struct s_mtdc32 * s);
void mtdc_resetReadout(struct s_mtdc32 * s);

int mtdc32_readTimeB(struct s_mtdc32 * s, uint32_t * pointer);

bool_t mtdc32_testBusError(struct s_mtdc32 * s);
void mtdc32_enableBusError(struct s_mtdc32 * s);
void mtdc32_disableBusError(struct s_mtdc32 * s);

void mtdc32_startAcq(struct s_mtdc32 * s);
void mtdc32_stopAcq(struct s_mtdc32 * s);

void mtdc32_resetFifo(struct s_mtdc32 * s);
void mtdc32_resetTimestamp(struct s_mtdc32 * s);

void mtdc32_setMcstSignature(struct s_mtdc32 * s, unsigned long Signature);
uint16_t mtdc32_getMcstSignature(struct s_mtdc32 * s);

void mtdc32_setCbltSignature(struct s_mtdc32 * s, unsigned long Signature);
uint16_t mtdc32_getCbltSignature(struct s_mtdc32 * s);

void mtdc32_setFirstInChain(struct s_mtdc32 * s);
bool_t mtdc32_isFirstInChain(struct s_mtdc32 * s);

void mtdc32_setLastInChain(struct s_mtdc32 * s);
bool_t mtdc32_isLastInChain(struct s_mtdc32 * s);
void mtdc32_setMiddleOfChain(struct s_mtdc32 * s);
bool_t mtdc32_isMiddleOfChain(struct s_mtdc32 * s);

void mtdc32_setMcstEnable(struct s_mtdc32 * s);
void mtdc32_setMcstDisable(struct s_mtdc32 * s);
bool_t mtdc32_mcstIsEnabled(struct s_mtdc32 * s);
void mtdc32_setCbltEnable(struct s_mtdc32 * s);
void mtdc32_setCbltDisable(struct s_mtdc32 * s);
bool_t mtdc32_cbltIsEnabled(struct s_mtdc32 * s);

void mtdc32_setMcstCblt_db(struct s_mtdc32 * s);

void mtdc32_startAcq_mcst(struct s_mtdc32 * s);
void mtdc32_stopAcq_msct(struct s_mtdc32 * s);
void mtdc32_resetFifo_mcst(struct s_mtdc32 * s);
void mtdc32_resetTimestamp_mcst(struct s_mtdc32 * s);
void mtdc_resetReadout_mcst(struct s_mtdc32 * s);

bool_t mtdc32_updateSettings(struct s_mtdc32 * s, char * updFile);

uint32_t * mtdc32_repairRawData(struct s_mtdc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif
