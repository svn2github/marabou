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

	  Char_t msg[256];
	  Char_t * mp;

	  Int_t nofEvents[kSis3302NofChans];
	  Int_t totalSize;

	  memset(nofEvents, 0, kSis3302NofChans * sizeof(Int_t));

	  pointerBegin = Pointer;		/* save pointer to beginning */
	  pointer = Pointer;			/* where to start */
	  *pointer++ = 0;			/* lh: wc=0, rh=serial, will be updated later */

	totalSize = 0;
	channelPattern = Module->activeChannels;
	for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
		if (channelPattern & 1) {
			  nxs = sis3302_readPrevBankSampleAddr(Module, chn);
			  if (nxs == 0xaffec0c0) continue;
			  nxs &= 0x3FFFFC;
			  if (nxs == 0) continue;
			  nxs >>= 1;
			  grp = chn / 2;
			  rdl = Module->rawDataSampleLength[grp] / 2;
			  edl = Module->energySampleLength[grp];
			  wc = kSis3302EventHeader + kSis3302EventMinMax + kSis3302EventTrailer + edl + rdl;
			  nofEvents[chn] = nxs / wc;
			  totalSize += nofEvents[chn] * (wc + 1) * sizeof(Int_t);
		}
	}

	if (totalSize > Module->bufferSize) {
		Float_t x = ((Float_t) Module->bufferSize) / totalSize;
		if (Module->verbose) {
			f_ut_send_msg("m_read_meb", "--------------------------------------------------------------------------------------------", ERR__MSG_INFO, MASK__PRTT);
			sprintf(msg, "[readout] module %s:", Module->moduleName);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			sprintf(msg, "[readout] total=%d, buffer=%d, fac=%6.2f ... ", totalSize, Module->bufferSize, x);
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
			sprintf(msg, "[readout] total=%d, buffer=%d,            ... ", totalSize, Module->bufferSize);
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
	
	channelPattern = Module->activeChannels;
	for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
		if (channelPattern & 1) {
			  startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;
			  mappedAddr = (volatile Int_t *) sis3302_mapAddress(Module, ca(Module, startAddr));
			  if (mappedAddr == NULL) continue;
			  for (evtNo = 0; evtNo < nofEvents[chn]; evtNo++) {
				  for (i = 0; i < kSis3302EventHeader; i++) {
					d = *mappedAddr++;		/* event header: 32bit words */
					if (i == 0) d = (d & 0xFFFF0000) | chn;
					*pointer++ = d;
				  }
				  *pointer++ = (rdl << 16) | edl;	/* extra word: lh=raw data length, rh=energy data length */
				  for (i = 0; i < rdl; i++) {		/* raw data: fetch 2 samples packed in 32bit, store each in a single 32bit word */
					  d = *mappedAddr++;
					  *pointer++ = (d >> 16) & 0xFFFF;
					  *pointer++ = d & 0xFFFF;
				  }
				  for (i = 0; i < edl; i++) *pointer++ = *mappedAddr++;			/* event data: 32bit words */
				  for (i = 0; i < kSis3302EventMinMax; i++) {
					d = *mappedAddr++;	/* max energy, min energy */
					*pointer++ = d;
				  }
				  for (i = 0; i < kSis3302EventTrailer; i++) {
					d = *mappedAddr++;	/* status, trailer */
					*pointer++ = d;
				  }
			  }
		  }
	  }

	  wc = (Int_t) (pointer - pointerBegin);
	  *pointerBegin = (wc << 16) | Module->serial;		/* update 1st word: lh: wc=0, rh=serial */
	  return (wc);
}

