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


s_mtdc32 * mtdc32_alloc(char * moduleName, s_mapDescr * md, int serial);

void mtdc32_moduleInfo(s_mtdc32 * s);
void mtdc32_setPrefix(s_mtdc32 * s, char * prefix);

bool_t mtdc32_fillStruct(s_mtdc32 * s, char * file);

void mtdc32_loadFromDb(s_mtdc32 * s, uint32_t chnPattern);

int mtdc32_initBLT(s_mtdc32 * s, int flag);
void mtdc32_enableBLT(s_mtdc32 * s);
bool_t mtdc32_useBLT(s_mtdc32 * s);

bool_t mtdc32_dumpRegisters(s_mtdc32 * s, char * file);
bool_t mtdc32_dumpRaw(s_mtdc32 * s, char * file);
void mtdc32_printDb(s_mtdc32 * s);

void mtdc32_soft_reset(s_mtdc32 * s);

void mtdc32_initDefaults(s_mtdc32 * s);

void mtdc32_setAddrReg(s_mtdc32 * s, uint16_t vmeAddr);
void mtdc32_setAddrReg_db(s_mtdc32 * s);
uint16_t mtdc32_getAddrReg(s_mtdc32 * s);

void mtdc32_setModuleId(s_mtdc32 * s, uint16_t id);
void mtdc32_setModuleId_db(s_mtdc32 * s);
uint16_t mtdc32_getModuleId(s_mtdc32 * s);

uint16_t mtdc32_getFifoLength(s_mtdc32 * s);

void mtdc32_setDataWidth(s_mtdc32 * s, uint16_t format);
void mtdc32_setDataWidth_db(s_mtdc32 * s);
uint16_t mtdc32_getDataWidth(s_mtdc32 * s);

void mtdc32_setMultiEvent(s_mtdc32 * s, uint16_t mode);
void mtdc32_setMultiEvent_db(s_mtdc32 * s);
uint16_t mtdc32_getMultiEvent(s_mtdc32 * s);

void mtdc32_setXferData(s_mtdc32 * s, uint16_t words);
void mtdc32_setXferData_db(s_mtdc32 * s);
uint16_t mtdc32_getXferData(s_mtdc32 * s);

void mtdc32_setMarkingType(s_mtdc32 * s, uint16_t type);
void mtdc32_setMarkingType_db(s_mtdc32 * s);
uint16_t mtdc32_getMarkingType(s_mtdc32 * s);

void mtdc32_setBankOperation(s_mtdc32 * s, uint16_t oper);
void mtdc32_setBankOperation_db(s_mtdc32 * s);
uint16_t mtdc32_getBankOperation(s_mtdc32 * s);

void mtdc32_setTdcResolution(s_mtdc32 * s, uint16_t res);
void mtdc32_setTdcResolution_db(s_mtdc32 * s);
uint16_t mtdc32_getTdcResolution(s_mtdc32 * s);

void mtdc32_setOutputFormat(s_mtdc32 * s, uint16_t format);
void mtdc32_setOutputFormat_db(s_mtdc32 * s);
uint16_t mtdc32_getOutputFormat(s_mtdc32 * s);

void mtdc32_setWinStart(s_mtdc32 * s, uint16_t bnk, int16_t offset);
void mtdc32_setWinStart_db(s_mtdc32 * s, uint16_t bnk);
int16_t mtdc32_getWinStart(s_mtdc32 * s, uint16_t bnk);

void mtdc32_setWinWidth(s_mtdc32 * s, uint16_t bnk, int16_t width);
void mtdc32_setWinWidth_db(s_mtdc32 * s, uint16_t bnk);
int16_t mtdc32_getWinWidth(s_mtdc32 * s, uint16_t bnk);

void mtdc32_setTrigSource(s_mtdc32 * s, uint16_t bnk, uint16_t trig, uint16_t chan, uint16_t bank);
void mtdc32_setTrigSource_db(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSource(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcTrig(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcChan(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcBank(s_mtdc32 * s, uint16_t bnk);

void mtdc32_setFirstHit(s_mtdc32 * s, uint16_t range);
void mtdc32_setFirstHit_db(s_mtdc32 * s);
uint16_t mtdc32_getFirstHit(s_mtdc32 * s);

void mtdc32_setNegEdge(s_mtdc32 * s, uint16_t edge);
void mtdc32_setNegEdge_db(s_mtdc32 * s);
uint16_t mtdc32_getNegEdge(s_mtdc32 * s);

void mtdc32_setEclTerm(s_mtdc32 * s, uint16_t term);
void mtdc32_setEclTerm_db(s_mtdc32 * s);
uint16_t mtdc32_getEclTerm(s_mtdc32 * s);

void mtdc32_setEclT1Osc(s_mtdc32 * s, uint16_t to);
void mtdc32_setEclT1Osc_db(s_mtdc32 * s);
uint16_t mtdc32_getEclT1Osc(s_mtdc32 * s);

void mtdc32_setTrigSelect(s_mtdc32 * s, uint16_t select);
void mtdc32_setTrigSelect_db(s_mtdc32 * s);
uint16_t mtdc32_getTrigSelect(s_mtdc32 * s);

void mtdc32_setNimT1Osc(s_mtdc32 * s, uint16_t to);
void mtdc32_setNimT1Osc_db(s_mtdc32 * s);
uint16_t mtdc32_getNimT1Osc(s_mtdc32 * s);

void mtdc32_setNimBusy(s_mtdc32 * s, uint16_t busy);
void mtdc32_setNimBusy_db(s_mtdc32 * s);
uint16_t mtdc32_getNimBusy(s_mtdc32 * s);

void mtdc32_setPulserStatus(s_mtdc32 * s, uint16_t mode);
void mtdc32_setPulserStatus_db(s_mtdc32 * s);
uint16_t mtdc32_getPulserStatus(s_mtdc32 * s);

void mtdc32_setPulserPattern(s_mtdc32 * s, uint16_t pattern);
void mtdc32_setPulserPattern_db(s_mtdc32 * s);
uint16_t mtdc32_getPulserPattern(s_mtdc32 * s);

void mtdc32_setInputThresh_db(s_mtdc32 * s, uint16_t bnk);
void mtdc32_setInputThresh(s_mtdc32 * s, uint16_t bnk, uint16_t thresh);
uint16_t mtdc32_getInputThresh(s_mtdc32 * s, uint16_t bnk);

void mtdc32_setTsSource(s_mtdc32 * s, uint16_t source);
void mtdc32_setTsSource_db(s_mtdc32 * s);
uint16_t mtdc32_getTsSource(s_mtdc32 * s);

void mtdc32_setTsDivisor(s_mtdc32 * s, uint16_t div);
void mtdc32_setTsDivisor_db(s_mtdc32 * s);
uint16_t mtdc32_getTsDivisor(s_mtdc32 * s);

void mtdc32_setMultLimit(s_mtdc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim);
void mtdc32_setMultLimit_db(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultHighLimit(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultLowLimit(s_mtdc32 * s, uint16_t bnk);

int mtdc32_readout(s_mtdc32 * s, uint32_t * pointer);
bool_t mtdc32_dataReady(s_mtdc32 * s);

int mtdc32_readTimeB(s_mtdc32 * s, uint32_t * pointer);

bool_t mtdc32_testBusError(s_mtdc32 * s);
void mtdc32_enableBusError(s_mtdc32 * s);
void mtdc32_disableBusError(s_mtdc32 * s);

void mtdc32_startAcq(s_mtdc32 * s);
void mtdc32_stopAcq(s_mtdc32 * s);

void mtdc32_resetFifo(s_mtdc32 * s);
void mtdc32_resetTimestamp(s_mtdc32 * s);
void mtdc_resetReadout(s_mtdc32 * s);

void mtdc32_resetFifo_mcst(s_mtdc32 * s);
void mtdc32_resetTimestamp_mcst(s_mtdc32 * s);
void mtdc_resetReadout_mcst(s_mtdc32 * s);

void mtdc32_initMCST(s_mtdc32 * s);

void mtdc32_setMcstSignature(s_mtdc32 * s, unsigned long Signature);
uint16_t mtdc32_getMcstSignature(s_mtdc32 * s);

bool_t mtdc32_isMcstMaster(s_mtdc32 * s);

void mtdc32_setCbltSignature(s_mtdc32 * s, unsigned long Signature);
uint16_t mtdc32_getCbltSignature(s_mtdc32 * s);

void mtdc32_setFirstInCbltChain(s_mtdc32 * s);
bool_t mtdc32_isFirstInCbltChain(s_mtdc32 * s);

void mtdc32_setLastInCbltChain(s_mtdc32 * s);
bool_t mtdc32_isLastInCbltChain(s_mtdc32 * s);
void mtdc32_setMiddleOfCbltChain(s_mtdc32 * s);
bool_t mtdc32_isMiddleOfCbltChain(s_mtdc32 * s);

void mtdc32_setMcstEnable(s_mtdc32 * s);
void mtdc32_setMcstDisable(s_mtdc32 * s);
bool_t mtdc32_mcstIsEnabled(s_mtdc32 * s);
void mtdc32_setCbltEnable(s_mtdc32 * s);
void mtdc32_setCbltDisable(s_mtdc32 * s);
bool_t mtdc32_cbltIsEnabled(s_mtdc32 * s);

void mtdc32_setMcstCblt_db(s_mtdc32 * s);

void mtdc32_reportReadErrors(s_mtdc32 * s);

uint32_t * mtdc32_repairRawData(s_mtdc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif
