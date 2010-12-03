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

int sis_3302_readout(struct s_sis_3302 * Module, UInt_t * Pointer)
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
	  Int_t nofEvents;
	  Int_t startAddr;
	  volatile Int_t * mappedAddr;
	  Int_t i;
	  UInt_t d;

	  pointerBegin = pointer;

	  channelPattern = Module->activeChannels;
	  for (chn = 0; chn < kSis3302NofChans; chn++, channelPattern >>= 1) {
		  if (channelPattern & 1) {
			  nxs = sis3302_readPrevBankSampleAddr(Module, chn);
			  if (nxs == 0xaffec0c0) continue;
			  nxs &= 0x3FFFFC;
			  nxs >>= 1;
			  grp = chn / 2;
			  rdl = Module->rawDataSampleLength[grp] / 2;
			  edl = Module->energySampleLength[grp];
			  wc = kSis3302EventHeader + kSis3302EventMinMax + kSis3302EventTrailer + edl + rdl;
			  nofEvents = nxs / wc;
			  startAddr = SIS3302_ADC1_OFFSET + chn * SIS3302_NEXT_ADC_OFFSET;
			  mappedAddr = (Int_t *) sis3302_mapAddress(Module, startAddr);
			  if (mappedAddr == NULL) continue;
			  for (evtNo = 0; evtNo <= nofEvents; evtNo++) {
				  for (i = 0; i < kSis3302EventHeader; i++) *pointer++ = *mappedAddr++;		/* event header: 32bit words */
				  for (i = 0; i < rdl; i++) {												/* raw data: fetch 2 samples packed in 32bit, store each in a single 32bit word */
					  d = *mappedAddr++;
					  *pointer++ = (d >> 16) & 0xFFFF;
					  *pointer++ = d & 0xFFFF;
				  }
				  for (i = 0; i < edl; i++) *pointer++ = *mappedAddr++;						/* event data: 32bit words */
				  for (i = 0; i < kSis3302EventMinMax + kSis3302EventTrailer; i++) *pointer++ = *mappedAddr++;	/* max energy, min energy, status, trailer */
			  }
		  }
	  }
	  return ((Int_t) (pointer - pointerBegin));
}

