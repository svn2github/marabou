#ifndef __MESY_PROTOS_H__
#define __MESY_PROTOS_H__

#include "mesy_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mesy_protos.h
//! \brief			Definitions for Mesytec modules
//! \details		Prototypes for Mesytec modules
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

s_mesy * mesy_alloc(char * moduleName, char * moduleType, int size, s_mapDescr * md, int serial);
int mesy_initBLT(s_mesy * m, int flag);
bool_t mesy_useBLT(s_mesy * m);
void mesy_soft_reset(s_mesy * m);
void mesy_setAddrReg_db(s_mesy * m);
void mesy_setAddrReg(s_mesy * m, uint16_t vmeAddr);
uint16_t mesy_getAddrReg(s_mesy * m);
void mesy_setModuleId_db(s_mesy * m);
void mesy_setModuleId(s_mesy * m, uint16_t id);
uint16_t mesy_getModuleId(s_mesy * m);
uint16_t mesy_getFifoLength(s_mesy * m);
void mesy_setDataWidth_db(s_mesy * m);
void mesy_setDataWidth(s_mesy * m, uint16_t width);
uint16_t mesy_getDataWidth(s_mesy * m);
void mesy_setMultiEvent_db(s_mesy * m);
void mesy_setMultiEvent(s_mesy * m, uint16_t mode);
uint16_t mesy_getMultiEvent(s_mesy * m);
void mesy_setXferData_db(s_mesy * m);
void mesy_setXferData(s_mesy * m, uint16_t wc);
uint16_t mesy_getXferData(s_mesy * m);
void mesy_setMarkingType_db(s_mesy * m);
void mesy_setMarkingType(s_mesy * m, uint16_t type);
uint16_t mesy_getMarkingType(s_mesy * m);
void mesy_setTsSource_db(s_mesy * m);
void mesy_setTsSource(s_mesy * m, uint16_t source);
uint16_t mesy_getTsSource(s_mesy * m);
void mesy_setTsDivisor_db(s_mesy * m);
void mesy_setTsDivisor(s_mesy * m, uint16_t div);
uint16_t mesy_getTsDivisor(s_mesy * m);
void mesy_moduleInfo(s_mesy * m);
void mesy_setPrefix(s_mesy * m, char * prefix);
bool_t mesy_dumpRaw(s_mesy * m, char * file);
int mesy_readout(s_mesy * s, uint32_t * pointer);
int mesy_readTimeB(s_mesy * m, uint32_t * pointer);
bool_t mesy_dataReady(s_mesy * m);
void mesy_resetReadout(s_mesy * m);
void mesy_resetTimestamp(s_mesy * m);
bool_t mesy_testBusError(s_mesy * m);
void mesy_enableBusError(s_mesy * m);
void mesy_disableBusError(s_mesy * m);
void mesy_startAcq(s_mesy * m, bool_t v);
void mesy_stopAcq(s_mesy * m, bool_t v);
void mesy_resetFifo(s_mesy * m);
void mesy_initMCST(s_mesy * m);
void mesy_setMcstCblt_db(s_mesy * m);
void mesy_setMcstSignature(s_mesy * m, unsigned long Signature);
uint16_t mesy_getMcstSignature(s_mesy * m);
bool_t mesy_isMcstMaster(s_mesy * m);
void mesy_setMcstEnable(s_mesy * m);
void mesy_setMcstDisable(s_mesy * m);
bool_t mesy_mcstIsEnabled(s_mesy * m);
void mesy_setCbltSignature(s_mesy * m, unsigned long Signature);
uint16_t mesy_getCbltSignature(s_mesy * m);
void mesy_setCbltEnable(s_mesy * m);
void mesy_setCbltDisable(s_mesy * m);
bool_t mesy_cbltIsEnabled(s_mesy * m);
void mesy_setFirstInCbltChain(s_mesy * m);
bool_t mesy_isFirstInCbltChain(s_mesy * m);
void mesy_setLastInCbltChain(s_mesy * m);
bool_t mesy_isLastInCbltChain(s_mesy * m);
void mesy_setMiddleOfCbltChain(s_mesy * m);
bool_t mesy_isMiddleOfCbltChain(s_mesy * m);
void mesy_resetFifo_mcst(s_mesy * m);
void mesy_resetReadout_mcst(s_mesy * m);
void mesy_resetTimestamp_mcst(s_mesy * m);
void mesy_reportReadErrors(s_mesy * m);
uint32_t * mesy_repairRawData(s_mesy * m, uint32_t * pointer, uint32_t * dataStart);
#endif
