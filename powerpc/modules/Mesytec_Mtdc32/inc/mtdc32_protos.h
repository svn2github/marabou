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

void mtdc32_setTdcResolution(struct s_mtdc32 * s, uint16_t res);
void mtdc32_setTdcResolution_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTdcResolution(struct s_mtdc32 * s);

void mtdc32_setOutputFormat(struct s_mtdc32 * s, uint16_t format);
void mtdc32_setOutputFormat_db(struct s_mtdc32 * s);
uint16_t mtdc32_getOutputFormat(struct s_mtdc32 * s);

void mtdc32_setWinStart(struct s_mtdc32 * s, uint16_t bnk, uint16_t offset);
void mtdc32_setWinStart_db(struct s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getWinStart(struct s_mtdc32 * s, uint16_t bnk);

void mtdc32_setWinWidth(struct s_mtdc32 * s, uint16_t bnk, uint16_t width);
void mtdc32_setWinWidth_db(struct s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getWinWidth(struct s_mtdc32 * s, uint16_t bnk);

void mtdc32_setTrigSource(struct s_mtdc32 * s, uint16_t bnk, uint16_t trig, uint16_t chan, uint16_t bnk);
void mtdc32_setTrigSource_db(struct s_mtdc32 * s, uint16_t bnk);
void mtdc32_getTrigSource(struct s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcTrig(struct s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcChan(struct s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getTrigSrcBank(struct s_mtdc32 * s, uint16_t bnk);

void mtdc32_setFirstHit(struct s_mtdc32 * s, uint16_t range);
void mtdc32_setFirstHit_db(struct s_mtdc32 * s);
uint16_t mtdc32_getFirstHit(struct s_mtdc32 * s);

void mtdc32_setNegEdge(struct s_mtdc32 * s, uint16_t edge);
void mtdc32_setNegEdge_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNegEdge(struct s_mtdc32 * s);

void mtdc32_setEclTerm(struct s_mtdc32 * s, uint16_t term);
void mtdc32_setEclTerm_db(struct s_mtdc32 * s);
uint16_t mtdc32_getEclTerm(struct s_mtdc32 * s);

void mtdc32_setEclT1Osc(struct s_mtdc32 * s, uint16_t go);
void mtdc32_setEclT1Osc_db(struct s_mtdc32 * s);
uint16_t mtdc32_getEclT1Osc(struct s_mtdc32 * s);

void mtdc32_setNimT1Osc(struct s_mtdc32 * s, uint16_t go);
void mtdc32_setNimT1Osc_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNimT1Osc(struct s_mtdc32 * s);

void mtdc32_setNimBusy(struct s_mtdc32 * s, uint16_t busy);
void mtdc32_setNimBusy_db(struct s_mtdc32 * s);
uint16_t mtdc32_getNimBusy(struct s_mtdc32 * s);

void mtdc32_setPulserStatus(struct s_mtdc32 * s, uint16_t mode);
void mtdc32_setPulserStatus_db(struct s_mtdc32 * s);
uint16_t mtdc32_getPulserStatus(struct s_mtdc32 * s);

void mtdc32_setPulserPattern(struct s_mtdc32 * s, uint16_t pattern);
void mtdc32_setPulserPattern_db(struct s_mtdc32 * s);
uint16_t mtdc32_getPulserPattern(struct s_mtdc32 * s);

void mtdc32_setTsSource(struct s_mtdc32 * s, uint16_t source);
void mtdc32_setTsSource_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTsSource(struct s_mtdc32 * s);

void mtdc32_setTsDivisor(struct s_mtdc32 * s, uint16_t div);
void mtdc32_setTsDivisor_db(struct s_mtdc32 * s);
uint16_t mtdc32_getTsDivisor(struct s_mtdc32 * s);

void mtdc32_setMultLimit(struct s_madc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim);
void mtdc32_setMultLimit_db(struct s_madc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultHighLimit(struct s_madc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultLowLimit(struct s_madc32 * s, uint16_t bnk);

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

uint32_t * mtdc32_repairRawData(struct s_mtdc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif
