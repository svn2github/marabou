
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

#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#include "sis_3302_bus_trap.h"

enum	{ kMaxTry = 100 };

static FILE * dump = NULL;
static dcnt = 0;

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
	Int_t i, ilast;
	UInt_t d;

	uint32_t ptrloc;

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

#ifdef DUMP_IT
	if (dump == NULL) {
		dcnt++;
		sprintf(msg, "xxx-%d.dat", dcnt);
		dump = fopen(msg, "w");
	}
	fprintf(dump, "-----------------------------------------------------------\n");
#endif
	for (grp = 0; grp < kSis3302NofGroups; grp++) {
		rdl = Module->tracingMode ? Module->rawDataSampleLength[grp] / 2 : 0;
		edl = Module->tracingMode ? Module->energySampleLength[grp] : 0;
		*pointer++ = (rdl << 16) | edl;
#ifdef DUMP_IT
		fprintf(dump, "grp=%d rdl=%#lx edl=%#lx\n", grp, rdl, edl);
#endif
	}
#ifdef DUMP_IT
	fprintf(dump, "-----------------------------------------------------------\n");
#endif

	totalSize = 0;
	channelPattern = Module->activeChannels;
	sampling = Module->currentSampling;
	for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
		if (channelPattern & 1) {
			nofEvents[chn] = 0;
			tryIt = 0;
			bankOk = kFALSE;
			do {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);	/* get last sampling address (16-bit words) */
				if (nxs == 0xaffec0c0) break;
				bankFlag = nxs & 0x01000000;				/* check bit 24: bank flag */
				bankShouldBe = (sampling == kSis3302KeyArmBank1Sampling) ? 0x01000000 : 0;
				bankOk = (bankFlag == bankShouldBe);
				if (bankOk) {
					nxs &= Module->reducedAddressSpace ? 0x3FFFC : 0x3FFFFC;	/* mask out relevant address bits */
					if (nxs == 0) break;
					nxs >>= 1;					/* now 32-bit words */
					grp = chn / 2;
					rdl = Module->rawDataSampleLength[grp];
					edl = Module->energySampleLength[grp];
					wc = kSis3302EventHeader + kSis3302EventMinMax + kSis3302EventTrailer + edl + rdl/2;	/* wc (32-bit words) */
					nofEvents[chn] = nxs / wc;			/* calc number of events */
					totalSize += nofEvents[chn] * (wc + 1) * sizeof(Int_t);	/* total size (bytes) */
					break;
				}
				usleep(1);
			} while (++tryIt < kMaxTry);
			if (!bankOk) {
				sprintf(msg, "[readout] [%s]: Bank switching failed for chn %d - bank bit is %#lx but should be %#lx", Module->moduleName, chn, bankFlag, bankShouldBe);
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

	if (!Module->blockXfer || dataTruncated) {
		channelPattern = Module->activeChannels;
		for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
			if (channelPattern & 1) {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);
				nxs &= Module->reducedAddressSpace ? 0x3FFFC : 0x3FFFFC;
				if (nxs == 0) continue;
				nxs >>= 1;
				startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;
				mappedAddr = (volatile Int_t *) sis3302_mapAddress(Module, ca(Module, startAddr));
				if (mappedAddr == NULL) continue;
				evtNo = nofEvents[chn];
				CLEAR_BUS_TRAP_FLAG;
				ilast = 0;
				for (i = 0; i < nxs; i++) {
					d = *mappedAddr++;
					if (i == 0) d = (d & 0xFFFF0000) | chn;
					*pointer++ = d;
#ifdef DUMP_IT
					fprintf(dump, "%d %#lx\n", i, d);
#endif
					if (d == 0xdeadbeef) {
						ilast = i;
						evtNo--;
						if (evtNo == 0) break;
					}
				}
				CHECK_BUS_TRAP(Module, ca(Module, startAddr), "readout");
			}
		}
	} else {
		channelPattern = Module->activeChannels;
		for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
			if (channelPattern & 1) {
				nxs = sis3302_readPrevBankSampleAddr(Module, chn);
				nxs &= Module->reducedAddressSpace ? 0x3FFFC : 0x3FFFFC;
				if (nxs == 0) continue;
				nxs >>= 1;
				startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;

				if ((((UInt_t) pointer) % 8) != 0) *pointer++ = 0x0d640d64;	/* align to 64 bit */
				ptrloc = getPhysAddr((char *) pointer, nxs * sizeof(uint32_t));
				if (ptrloc == NULL) continue;
				CLEAR_BUS_TRAP_FLAG;
				bmaError = bma_read(Module->md->bltBase + startAddr, ptrloc | 0x0, nxs, Module->md->bltModeId);
				if (bmaError != 0) {
					sprintf(msg, "[readout] [%s]: %s (%d) while reading event data (chn=%d, wc=%d)", Module->moduleName, bmaErrlist[bmaError], bmaError, chn, nxs);
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					continue;
				}
				CHECK_BUS_TRAP(Module, Module->md->bltBase + startAddr, "readout");
				pointer += nxs;
			}
		}
	}

	wc = (Int_t) (pointer - pointerBegin);
	*pointerBegin = (wc << 8) | Module->serial;		/* update 1st word: wc[31:8], serial[7:0] */
#ifdef DUMP_IT
	fprintf(dump, "wc=%d serial=%#lx\n", wc, (wc << 16) | Module->serial);
	fclose(dump);
	dump = NULL;
	printf("@@@ %d\n", dcnt);
#endif
	return (wc);
}

