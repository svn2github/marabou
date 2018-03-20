/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			mcommonfct.c
//! \brief			Common code for all Mesytec modules
//! \details		Implements functions to handle Mesytec modules
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "gd_readout.h"

#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#include "mapping_database.h"
#include "bmaErrlist.h"

#include "mesy.h"
#include "mesy_database.h"
#include "mesy_protos.h"

char msg[256];

bool_t busError;

void mesy_catchBerr() { busError = TRUE; }

s_mesy * mesy_alloc(char * moduleName, char * moduleType, int size, s_mapDescr * md, int serial)
{
	s_mesy * m;
	int firmware, mainRev;

	m = (s_mesy *) calloc(1, size);
	if (m != NULL) {
		m->md = md;
		strcpy(m->moduleName, moduleName);
		strcpy(m->prefix, "m_read_meb");
		strcpy(m->mpref, moduleType);
		strcat(m->mpref, ": ");
		m->serial = serial;
		m->verbose = FALSE;
		m->dumpRegsOnInit = FALSE;

		m->mcstSignature = 0x0;
		m->mcstMaster = FALSE;
		m->cbltSignature = 0x0;
		m->firstInCbltChain = FALSE;
		m->lastInCbltChain = FALSE;
		
		m->mcstAddr = 0;
		m->cbltAddr = 0;

		firmware = GET16(m->md->vmeBase, MESY_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		m->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
		
		busError = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate mesy struct", m->mpref, m->moduleName);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return m;
}

int mesy_initBLT(s_mesy * m, int flag)
{
	if (!m->blockXfer) return flag;			/* not using BLT -> nothing to do */
	
	if (flag == kBLTInitError) {			/* BLT not available */
		m->blockXfer = FALSE;
		return kBLTInitError;
	}
		
	if (flag == kBLTInitNotDone) {			/* we have to call InitBLT() once */
		if (!initBLT()) {
			m->blockXfer = FALSE;			/* not successful: turn off BLT */
			return kBLTInitError;
		}
	}
	
	if (m->md->mappingBLT == kVMEMappingUndef) {	/* already mapped? */
		if (mapBLT(m->md, 0xb) == NULL) {			/* no, map BLT page */
			m->blockXfer = FALSE;					/* no succedss, turn BLT off */
			return kBLTInitDone;					/* BLT status remains unchanged */
		}
	}

	setBLTMode(m->md, BMA_M_Vsz32, BMA_M_WzD32, TRUE);
	return kBLTInitDone;
}

bool_t mesy_useBLT(s_mesy * m) { return m->blockXfer; }

void mesy_soft_reset(s_mesy * m)
{
  SET16(m->md->vmeBase, MESY_SOFT_RESET, 0x1);
}

void mesy_setAddrReg_db(s_mesy * m) { mesy_setAddrReg(m, m->addrReg); }

void mesy_setAddrReg(s_mesy * m, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(m->md->vmeBase, MESY_ADDR_REG, vmeAddr);
		SET16(m->md->vmeBase, MESY_ADDR_SOURCE, MESY_ADDR_SOURCE_REG);
		m->md->vmeBase = vmeAddr;
	}
}

uint16_t mesy_getAddrReg(s_mesy * m)
{
	uint16_t source = GET16(m->md->vmeBase, MESY_ADDR_SOURCE);
	if (source & MESY_ADDR_SOURCE_REG) return GET16(m->md->vmeBase, MESY_ADDR_REG);
	else return 0;
}

void mesy_setModuleId_db(s_mesy * m) { mesy_setModuleId(m, m->moduleId); }

void mesy_setModuleId(s_mesy * m, uint16_t id)
{
	SET16(m->md->vmeBase, MESY_MODULE_ID, id & MESY_MODULE_ID_MASK);
}

uint16_t mesy_getModuleId(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_MODULE_ID) & MESY_MODULE_ID_MASK;
}

uint16_t mesy_getFifoLength(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_BUFFER_DATA_LENGTH) & MESY_BUFFER_DATA_LENGTH_MASK;
}

void mesy_setDataWidth_db(s_mesy * m) { mesy_setDataWidth(m, m->dataWidth); }

void mesy_setDataWidth(s_mesy * m, uint16_t width)
{
	SET16(m->md->vmeBase, MESY_DATA_LENGTH_FORMAT, width & MESY_DATA_LENGTH_FORMAT_MASK);
}

uint16_t mesy_getDataWidth(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_DATA_LENGTH_FORMAT) & MESY_DATA_LENGTH_FORMAT_MASK;
}

void mesy_setMultiEvent_db(s_mesy * m) { mesy_setMultiEvent(m, m->multiEvent); }

void mesy_setMultiEvent(s_mesy * m, uint16_t mode)
{
	SET16(m->md->vmeBase, MESY_MULTI_EVENT, mode & MESY_MULTI_EVENT_MASK);
}

uint16_t mesy_getMultiEvent(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_MULTI_EVENT) & MESY_MULTI_EVENT_MASK;
}

void mesy_setXferData_db(s_mesy * m) { mesy_setXferData(m, m->xferData); }

void mesy_setXferData(s_mesy * m, uint16_t wc)
{
	SET16(m->md->vmeBase, MESY_MAX_XFER_DATA, wc & MESY_MAX_XFER_DATA_MASK);
}

uint16_t mesy_getXferData(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_MAX_XFER_DATA) & MESY_MAX_XFER_DATA_MASK;
}

void mesy_setMarkingType_db(s_mesy * m) { mesy_setMarkingType(m, m->markingType); }

void mesy_setMarkingType(s_mesy * m, uint16_t type)
{
	SET16(m->md->vmeBase, MESY_MARKING_TYPE, type & MESY_MARKING_TYPE_MASK);
}

uint16_t mesy_getMarkingType(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_MARKING_TYPE) & MESY_MARKING_TYPE_MASK;
}

void mesy_setTsSource_db(s_mesy * m) { mesy_setTsSource(m, m->ctraTsSource); }

void mesy_setTsSource(s_mesy * m, uint16_t source)
{
	SET16(m->md->vmeBase, MESY_CTRA_TS_SOURCE, source & MESY_CTRA_TS_SOURCE_MASK);
}

uint16_t mesy_getTsSource(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_CTRA_TS_SOURCE) & MESY_CTRA_TS_SOURCE_MASK;
}

void mesy_setTsDivisor_db(s_mesy * m) { mesy_setTsDivisor(m, m->ctraTsDivisor); }

void mesy_setTsDivisor(s_mesy * m, uint16_t div)
{
	SET16(m->md->vmeBase, MESY_CTRA_TS_DIVISOR, div & MESY_CTRA_TS_DIVISOR_MASK);
}

uint16_t mesy_getTsDivisor(s_mesy * m)
{
	return GET16(m->md->vmeBase, MESY_CTRA_TS_DIVISOR) & MESY_CTRA_TS_DIVISOR_MASK;
}

void mesy_moduleInfo(s_mesy * m)
{
	int firmware, mainRev, subRev;
	firmware = GET16(m->md->vmeBase, MESY_FIRMWARE_REV);
	mainRev = (firmware >> 8) & 0xff;
	subRev = firmware & 0xff;
	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, firmware %02x.%02x (%dk memory)",
									m->mpref,
									m->moduleName,
									m->md->physAddrVME,
									m->md->vmeBase,
									mainRev, subRev, (mainRev >= 2) ? 8 : 1);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
}

void mesy_setPrefix(s_mesy * m, char * prefix)
{
	strcpy(m->prefix, prefix);
	strcpy(m->mpref, "");
}

bool_t mesy_dumpRaw(s_mesy * m, char * file)
{
	int i;
	FILE * f;

	if (!m->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRaw] %s: Error writing file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRaw] %s: Dumping raw data to file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	for (i = 0x6000; i < 0x60B0; i += 2) {
		fprintf(f, "%#lx %#x\n", i, GET16(m->md->vmeBase, i));
	}
	fclose(f);
}

int mesy_readout(s_mesy * m, uint32_t * pointer)
{
	static int addrOffset = 0;

	uint32_t * dataStart, *p;
	uint32_t data;
	unsigned int i, j;
	int bmaError;
	int bmaCount;
	int tryIt;
	int numData, nd;

	uint32_t ptrloc;

	int mode;
	int sts;

	dataStart = pointer;
 
 	numData = (int) mesy_getFifoLength(m);
	
	if (numData == 0) return(0);

	m->nofReads++;
	
	if (m->blockXfer) {
		ptrloc = getPhysAddr((char *) pointer, numData * sizeof(uint32_t));
		if (ptrloc == NULL) {
			m->nofReadErrors++;
			if ((m->reportReadErrors == 0) || (m->nofReadErrors <= m->reportReadErrors)) {
				sprintf(msg, "[%sreadout] %s: Can't relocate mapped pointer %#lx to phys addr - BLT turned off", m->mpref, m->moduleName, pointer);
				f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
			m->blockXfer = FALSE;
		}
	}
		
	if (m->blockXfer) {
#ifndef CPU_TYPE_RIO2
		bmaCount = bma_read_count(m->md->bltBase + MESY_DATA, ptrloc, numData, m->md->bltModeId);
		if (bmaCount < 0) {
			m->nofReadErrors++;
			sprintf(msg, "[%sreadout] %s: Error \"%s\" (%d) while reading event data (numData=%d)", m->mpref, m->moduleName, bmaErrlist[bmaError], bmaError, numData);
			f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}		
		pointer += bmaCount;
#else
		bmaError = bma_read(m->md->bltBase + MESY_DATA, ptrloc, numData, m->md->bltModeId);
		if (bmaError != 0) {
			m->nofReadErrors++;
			if ((m->reportReadErrors == 0) || (m->nofReadErrors <= m->reportReadErrors)) {
				if (bmaError < 0) {
					sprintf(msg, "[%sreadout] %s: Error %d while reading event data (numData=%d)", m->mpref, m->moduleName, bmaError, numData);
				} else {
					sprintf(msg, "[%sreadout] %s: Error \"%s\" (%d) while reading event data (numData=%d)", m->mpref, m->moduleName, bmaErrlist[bmaError], bmaError, numData);
				}
				f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
			return(0);
		}			
		pointer += numData;
#endif
	} else if ((m->multiEvent & MESY_MULTI_EVENT_BERR) == 0) {
		busError = FALSE;
		nd = 0;
		signal(SIGBUS, mesy_catchBerr);
		while (1) {
			nd++;
			data = GET32(m->md->vmeBase, MESY_DATA);
			if (busError) {
				signal(SIGBUS, SIG_DFL);
				busError = FALSE;
				break;
			}
			if (nd == 1) {
				if ((data & 0xF0000000) != 0x40000000) {
					m->nofReadErrors++;
					if ((m->reportReadErrors == 0) || (m->nofReadErrors <= m->reportReadErrors)) {
						sprintf(msg, "[%sreadout] %s: Wrong header at start of data - %#x", m->mpref, m->moduleName, data);
						f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
					}
				}
			}	
			*pointer++ = data;
		}
	} else {
		nd = 0;
		while (1) {
			data = GET32(m->md->vmeBase, MESY_DATA);
			nd++;
			if (nd == 1) {
				if ((data & 0xF0000000) != 0x40000000) {
					m->nofReadErrors++;
					if ((m->reportReadErrors == 0) || (m->nofReadErrors <= m->reportReadErrors)) {
						sprintf(msg, "[%sreadout] %s: Wrong header at start of data - %#x", m->mpref, m->moduleName, data);
						f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
					}
				}
			}	
			if (data == 0x80000000) break;
			*pointer++ = data;
		}
	}

	if (m->repairRawData) pointer = mesy_repairRawData(m, pointer, dataStart);
	
	mesy_resetReadout(m);

	return (pointer - dataStart);
}

int mesy_readTimeB(s_mesy * m, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(m->md->vmeBase, MESY_CTRB_TIME_0);
	*p16++ = GET16(m->md->vmeBase, MESY_CTRB_TIME_1);
	*p16++ = GET16(m->md->vmeBase, MESY_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t mesy_dataReady(s_mesy * m)
{
	return TSTB16(m->md->vmeBase, MESY_DATA_READY, 0) ;
}

void mesy_resetReadout(s_mesy * m)
{
	SET16(m->md->vmeBase, MESY_READOUT_RESET, 0x1);
}

void mesy_resetTimestamp(s_mesy * m)
{
	SET16(m->md->vmeBase, MESY_CTRA_RESET_A_OR_B, 0x3);
}

bool_t mesy_testBusError(s_mesy * m)
{
	return TSTB16(m->md->vmeBase, MESY_MULTI_EVENT, MESY_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void mesy_enableBusError(s_mesy * m)
{
	CLRB16(m->md->vmeBase, MESY_MULTI_EVENT, MESY_MULTI_EVENT_BERR);
}

void mesy_disableBusError(s_mesy * m)
{
	SETB16(m->md->vmeBase, MESY_MULTI_EVENT, MESY_MULTI_EVENT_BERR);
}

void mesy_startAcq(s_mesy * m, bool_t v)
{
	if (mesy_mcstIsEnabled(m)) {
		if (m->mcstMaster) {
			SET16(m->mcstAddr, MESY_START_ACQUISITION, 0x0);
			mesy_resetFifo_mcst(m);
			mesy_resetReadout_mcst(m);
			mesy_resetTimestamp_mcst(m);
			SET16(m->mcstAddr, MESY_START_ACQUISITION, 0x1);
			if (v) {
				sprintf(msg, "[%sstartAcq] %s: MCST start - signature %#x, addr %#lx\n", m->mpref, m->moduleName, m->mcstSignature, m->mcstAddr);
				f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
		}
	} else {	
		SET16(m->md->vmeBase, MESY_START_ACQUISITION, 0x0);
		mesy_resetFifo(m);
		mesy_resetReadout(m);
		mesy_resetTimestamp(m);
		SET16(m->md->vmeBase, MESY_START_ACQUISITION, 0x1);
		printf("@@@ start %s %lx\n", m->moduleName, m->md->vmeBase);
	}
}

void mesy_stopAcq(s_mesy * m, bool_t v)
{
	if (mesy_mcstIsEnabled(m)) {
		if (m->mcstMaster) {
			SET16(m->mcstAddr, MESY_START_ACQUISITION, 0x0);
			mesy_resetFifo_mcst(m);
			mesy_resetReadout_mcst(m);
			mesy_resetTimestamp_mcst(m);
			if (v) {
				sprintf(msg, "[%sstopAcq] %s: MCST stop - signature %#x, addr %#lx\n", m->mpref, m->moduleName, m->mcstSignature, m->mcstAddr);
				f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
		}
	} else {	
		SET16(m->md->vmeBase, MESY_START_ACQUISITION, 0x0);
		mesy_resetFifo(m);
		mesy_resetReadout(m);
		mesy_resetTimestamp(m);
	}
}

void mesy_resetFifo(s_mesy * m)
{
	SET16(m->md->vmeBase, MESY_FIFO_RESET, 0x1);
}

void mesy_initMCST(s_mesy * m)
{	
	if (mesy_mcstIsEnabled(m)) {
		if (mesy_isMcstMaster(m)) {
			if (m->mcstAddr == 0) m->mcstAddr = mapAdditionalVME(m->md, (m->mcstSignature & 0xFF) << 24, 0);
			sprintf(msg, "[%smsctInit] %s: MCST master - signature %#x, addr %#lx\n", m->mpref, m->moduleName, m->mcstSignature, m->mcstAddr);
			f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		} else {
			sprintf(msg, "[%smsctInit] %s: MCST slave - signature %#x\n", m->mpref, m->moduleName, m->mcstSignature);
			f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}	
	} else {
		sprintf(msg, "[%smsctInit] %s: No MCST - running in individual mode\n", m->mpref, m->moduleName);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
}

void mesy_setMcstCblt_db(s_mesy * m) {
	mesy_setMcstSignature(m, m->mcstSignature);
	mesy_setCbltSignature(m, m->cbltSignature);
	if (m->firstInCbltChain) mesy_setFirstInCbltChain(m);
	else if (m->lastInCbltChain) mesy_setLastInCbltChain(m);
	else mesy_setMiddleOfCbltChain(m);
}

void mesy_setMcstSignature(s_mesy * m, unsigned long Signature) {
	SET16(m->md->vmeBase, MESY_MCST_ADDRESS, Signature);
	if (Signature != 0) mesy_setMcstEnable(m); else mesy_setMcstDisable(m);
}

uint16_t mesy_getMcstSignature(s_mesy * m) {
	uint16_t addr8;
	addr8 = GET16(m->md->vmeBase, MESY_MCST_ADDRESS);
	return addr8;
}

bool_t mesy_isMcstMaster(s_mesy * m) { return m->mcstMaster; }

void mesy_setMcstEnable(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_MCST_ENA);
}

void mesy_setMcstDisable(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_MCST_DIS);
}

bool_t mesy_mcstIsEnabled(s_mesy * m) {
	uint16_t ctrl;
	ctrl = GET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL);
	return ((ctrl & MESY_MCST_DIS) != 0);
}

void mesy_setCbltSignature(s_mesy * m, unsigned long Signature) {
	SET16(m->md->vmeBase, MESY_CBLT_ADDRESS, Signature);
	if (Signature != 0) mesy_setCbltEnable(m); else mesy_setCbltDisable(m);
}

uint16_t mesy_getCbltSignature(s_mesy * m) {
	uint16_t addr8;
	addr8 = GET16(m->md->vmeBase, MESY_CBLT_ADDRESS);
	return addr8;
}

void mesy_setCbltEnable(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_ENA);
}

void mesy_setCbltDisable(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_DIS);
}

bool_t mesy_cbltIsEnabled(s_mesy * m) {
	uint16_t ctrl;
	ctrl = GET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL);
	return ((ctrl & MESY_CBLT_DIS) != 0);
}

void mesy_setFirstInCbltChain(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_FIRST_ENA);
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_LAST_DIS);
}

bool_t mesy_isFirstInCbltChain(s_mesy * m) {
	uint16_t ctrl;
	ctrl = GET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL);
	return ((ctrl & MESY_CBLT_FIRST_DIS) != 0);
}

void mesy_setLastInCbltChain(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_LAST_ENA);
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_FIRST_DIS);
}

bool_t mesy_isLastInCbltChain(s_mesy * m) {
	uint16_t ctrl;
	ctrl = GET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL);
	return ((ctrl & MESY_CBLT_LAST_DIS) != 0);
}

void mesy_setMiddleOfCbltChain(s_mesy * m) {
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_FIRST_DIS);
	SET16(m->md->vmeBase, MESY_CBLT_MCST_CONTROL, MESY_CBLT_LAST_DIS);
}

bool_t mesy_isMiddleOfCbltChain(s_mesy * m) {
	bool_t first, last;
	first = mesy_isFirstInCbltChain(m);
	last = mesy_isLastInCbltChain(m);
	return (!first && !last);
}

void mesy_resetFifo_mcst(s_mesy * m)
{
	SET16(m->mcstAddr, MESY_FIFO_RESET, 0x1);
}

void mesy_resetReadout_mcst(s_mesy * m)
{
	SET16(m->mcstAddr, MESY_READOUT_RESET, 0x1);
}

void mesy_resetTimestamp_mcst(s_mesy * m)
{
	SET16(m->mcstAddr, MESY_CTRA_RESET_A_OR_B, 0x3);
}

void mesy_reportReadErrors(s_mesy * m)
{
	sprintf(msg, "[%sreadout] %s: read_requests=%d read_errors=%d", m->mpref, m->moduleName, m->nofReads, m->nofReadErrors);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
}

uint32_t * mesy_repairRawData(s_mesy * m, uint32_t * pointer, uint32_t * dataStart)
{
	return pointer;
}
