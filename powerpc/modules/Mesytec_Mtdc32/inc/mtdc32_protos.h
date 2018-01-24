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


bool_t mtdc32_fillStruct(s_mtdc32 * s, char * file);
void mtdc32_loadFromDb(s_mtdc32 * s, uint32_t chnPattern);
bool_t mtdc32_dumpRegisters(s_mtdc32 * s, char * file);
void mtdc32_printDb(s_mtdc32 * s);
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
void mtdc32_setMultLimit(s_mtdc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim);
void mtdc32_setMultLimit_db(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultHighLimit(s_mtdc32 * s, uint16_t bnk);
uint16_t mtdc32_getMultLowLimit(s_mtdc32 * s, uint16_t bnk);
#endif
