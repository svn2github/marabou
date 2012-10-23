/*____________________________________________________________________[C CODE]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3302_readout.c
// Purpose:        Readout code for module SIS3302
// Description:    Supplies readout functions for SIS3302 digitizing ADC
// Author:         R. Lutter (adopted from code written by L. Maier)
// Revision:
// Date:
// Keywords:
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ces/vmelib.h>
#include <ces/bmalib.h>

#include "gd_readout.h"

#include "LwrTypes.h"
#include "M2L_CommonDefs.h"

#include "sis_3302_protos.h"
#include "sis_3302_layout.h"
#include "sis_3302_database.h"


#include "err_mask_def.h"
#include "errnum_def.h"

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read data
//! \param[in]		Module			-- module structure
//! \param[in]		Pointer		 	-- pointer to MBS output structure
//! \return 		Length			-- length of data xferred (32 bit words))
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readout(struct s_sis_3302 * Module, UInt_t * Pointer)
{
	UInt_t * pointer;
	UInt_t * pointerBegin;
	UInt_t channelPattern;
	Int_t chn;
	Int_t grp;
	Int_t nxs;
	Int_t wc;
	Int_t rdl;
	Int_t edl;
	Int_t evtNo;
	Int_t startAddr;
	volatile Int_t * mappedAddr;
	Int_t i;
	UInt_t d;

	Int_t bankFlag, bankShouldBe;
	Int_t sampling;
	Int_t tryIt;
	Bool_t bankOk;

	Char_t msg[256];
	Char_t * mp;

	Int_t nofEvents[kSis3302NofChans];
	Int_t totalSize;
	Bool_t dataTruncated;
	Int_t bmaError;

	memset(nofEvents, 0, kSis3302NofChans * sizeof(Int_t));
	
	pointerBegin = Pointer;		/* save pointer to beginning */
	pointer = Pointer;			/* where to start */
	*pointer++ = 0;			/* lh: wc=0, rh=serial, will be updated later */

	for (grp = 0; grp < kSis3302NofGroups; grp++) {
		rdl = Module->tracingMode ? Module->rawDataSampleLength[grp] / 2 : 0;
		edl = Module->tracingMode ? Module->energySampleLength[grp] : 0;
		*pointer++ = (rdl << 16) | edl;
	}

	totalSize = 0;
	channelPattern = Module->activeChannels;
	sampling = Module->currentSampling;
	for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
		if (channelPattern & 1) {
			nofEvents[chn] = 0;
			tryIt = 0;
			bankOk = kFALSE;
			do {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);
				if (nxs == 0xaffec0c0) break;
				bankFlag = nxs & 0x01000000;	/* check bit 24 */
				bankShouldBe = (sampling == kSis3302KeyArmBank1Sampling) ? 0x01000000 : 0;
				bankOk = (bankFlag == bankShouldBe);
				if (bankOk) {
					nxs &= 0x3FFFFC;
					if (nxs == 0) break;
					nxs >>= 1;
					grp = chn / 2;
					rdl = Module->rawDataSampleLength[grp];
					edl = Module->energySampleLength[grp];
					wc = kSis3302EventHeader + kSis3302EventMinMax + kSis3302EventTrailer + edl + rdl/2;
					nofEvents[chn] = nxs / wc;
					totalSize += nofEvents[chn] * (wc + 1) * sizeof(Int_t);
					break;
				}
			} while (++tryIt < 20);
			if (!bankOk) {
				sprintf(msg, "[readout] [%s]: Bank switching failed for chn %d - bank bit is %d but should be %d", Module->moduleName, chn, bankFlag, bankShouldBe);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			}
		}
	}

	dataTruncated = (totalSize > Module->bufferSize);
	if (dataTruncated) {	/* if data exceed buffer space in the ppc */
		Float_t x = ((Float_t) Module->bufferSize) / totalSize;
		if (Module->verbose) {
			f_ut_send_msg("m_read_meb", "--------------------------------------------------------------------------------------------", ERR__MSG_INFO, MASK__PRTT);
			sprintf(msg, "[readout] [%s]: total=%d, buffer=%d, fac=%6.2f ... ", Module->moduleName, totalSize, Module->bufferSize, x);
			mp = &msg[strlen(msg)];
			for (chn = 0; chn < kSis3302NofChans; chn++) {
				if (nofEvents[chn] > 0) {
					sprintf(mp, "%d:%d ", chn, nofEvents[chn]);
					mp = &msg[strlen(msg)];
				}
			}
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		}
		totalSize = 0;
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			nofEvents[chn] = (Int_t) (nofEvents[chn] * x);
			totalSize += nofEvents[chn] * (wc + 1) * sizeof(Int_t);
		}
		if (Module->verbose) {
			sprintf(msg, "[readout] [%s]: total=%d, buffer=%d,            ... ", Module->moduleName, totalSize, Module->bufferSize);
			mp = &msg[strlen(msg)];
			for (chn = 0; chn < kSis3302NofChans; chn++) {
				if (nofEvents[chn] > 0) {
					sprintf(mp, "%d:%d ", chn, nofEvents[chn]);
					mp = &msg[strlen(msg)];
				}
			}
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			f_ut_send_msg("m_read_meb", "--------------------------------------------------------------------------------------------", ERR__MSG_INFO, MASK__PRTT);
		}
	}

	if (Module->blockXfer == SIS3302_BLT_OFF || dataTruncated) {
		channelPattern = Module->activeChannels;
		for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
			if (channelPattern & 1) {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);
				nxs &= 0x3FFFFC;
				if (nxs == 0) continue;
				nxs >>= 1;
				startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;
				mappedAddr = (volatile Int_t *) sis3302_mapAddress(Module, ca(Module, startAddr));
				if (mappedAddr == NULL) continue;
				evtNo = nofEvents[chn];
				for (i = 0; i < nxs; i++) {
					d = *mappedAddr++;
					if (i == 0) d = (d & 0xFFFF0000) | chn;
					*pointer++ = d;
					if (d == 0xdeadbeef) {
						evtNo--;
						if (evtNo == 0) break;
					}
				}
			}
		}
	} else if (Module->blockXfer == SIS3302_BLT_NORMAL) {
		channelPattern = Module->activeChannels;
		for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
			if (channelPattern & 1) {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);
				nxs &= 0x3FFFFC;
				if (nxs == 0) continue;
				nxs >>= 1;
				startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;

				if ((((UInt_t) pointer) % 8) != 0) *pointer++ = 0x0d640d64;	/* align to 64 bit */
#ifdef CPU_TYPE_RIO2
            			if ((bmaError = vmtopm(getpid(), &Module->bltChain, (char*) pointer, nxs * sizeof(UInt_t))) == -1)
					sprintf(msg, "[readout] [%s]: vmtopm call failed (chn=%d, wc=%d)", Module->moduleName, chn, nxs);
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					continue;
				}

				bmaError = bma_read(Module->vmeAddr + startAddr, Module->bltChain.address | 0x0, nxs, BMA_DEFAULT_MODE);
				if (bmaError != 0) {
					sprintf(msg, "[readout] [%s]: %s (%d) while reading event data (chn=%d, wc=%d)", Module->moduleName, sys_errlist[errno], errno, chn, nxs);
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					continue;
				}
#endif

#ifdef CPU_TYPE_RIO3
				bmaError = bma_read(Module->vmeAddr + startAddr, (UInt_t) ((UInt_t) pointer & 0x0FFFFFFF) | 0x0, nxs , BMA_DEFAULT_MODE);

#endif
				pointer += nxs;
			}
		}
	}

	wc = (Int_t) (pointer - pointerBegin);
	*pointerBegin = (wc << 16) | Module->serial;		/* update 1st word: lh: wc=0, rh=serial */
	return (wc);
}

