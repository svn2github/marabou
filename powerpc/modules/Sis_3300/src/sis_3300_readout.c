/*____________________________________________________________________[C CODE]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3300_readout.c
// Purpose:        Readout code for module SIS3300
// Description:    Supplies readout functions for SIS3300 digitizing ADC
// Author:         R. Lutter (adopted from code written by L. Maier)
// Revision:       
// Date:           
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ces/bmalib.h>

#include "gd_readout.h"

#include "sis_3300.h"
#include "sis_3300_protos.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

int sis_3300_readout(s_sis_3300 * s, uint8_t bank, uint32_t * pointer)
{
	static const uint32_t memoryOffset[4] = {0x000000, 1 * 0x080000, 2 * 0x080000, 3 * 0x080000};
	
	static const uint32_t timeStampOffset = 4 * 0x080000;
	static const uint32_t triggerEventOffset = 4 * 0x080000 + 0x1000;
	
	uint32_t address;
	uint32_t numEvts;
	uint32_t *tmp;
	uint32_t evt;
	uint8_t chgrp;
	uint32_t pageSize;
	int length;
	uint32_t memSize;
	uint32_t * pointerBegin;

	sis_3300_disarmBank(s, bank);
	
	pageSize = sis_3300_getPageSizeVal(s);
	
	numEvts = sis_3300_getEventCounter(s, bank);

	if (numEvts == 0) {
		sis_3300_clearBankFull(s, bank);
		return 0;
	}

	if (s->acqMode == SIS_3300_GATE_CHAINING) {
		memSize = sis_3300_getAddressCounter(s, bank);		
	} else {
		/* Single / Multi Event Mode (fixed size of events)*/
		if ((s->maxNumEvts != 0) && (numEvts > s->maxNumEvts)) {
			numEvts = s->maxNumEvts;
		}	
		memSize = numEvts * pageSize;
	}

	/* header: length in 32 bit words, subEvtId,				*/
	/*				 acqMode, eventCounter,									 */
	/*				 pageSize (as number), status register,	 */
	/*				 acquisition status register							*/

	pointerBegin = pointer;

	*pointer++ = (uint32_t)s->acqMode;
	*pointer++ = numEvts;
	*pointer++ = (uint32_t)pageSize;
	*pointer++ = GET32(s->baseAddr, SIS_3300_STATUS);
	*pointer++ = GET32(s->baseAddr, SIS_3300_ACQ_STATUS);
	
	if (s->blockTransOn == 1) {
		/* prepare Chain of BlockTransfers */
		if (bmaResetChain(s->bma) < 0) {
			sprintf(msg, "%s: resetting block xfer chain failed", s->moduleName);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
		
		/* Read Event Timestamp Directory */
		address = (bank == 0) ? SIS_3300_BANK1_TIME_DIR : SIS_3300_BANK2_TIME_DIR;
		if (bmaReadChain(s->bma, timeStampOffset, s->vmeAddr + address, numEvts, 0x0b) < 0) {
			sprintf(msg, "%s: block xfer failed while reading event timestamp dir on bank %d", s->moduleName, bank);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
		
		/* Read Trigger Event directory	 */
		address = (bank == 0) ? SIS_3300_EVENT_DIRECTORY_BANK1_ALL_ADC : SIS_3300_EVENT_DIRECTORY_BANK2_ALL_ADC;
		if (bmaReadChain(s->bma, triggerEventOffset, s->vmeAddr + address, numEvts, 0x0b) < 0) {
			sprintf(msg, "%s: block xfer failed while reading trigger event dir on bank %d", s->moduleName, bank);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
			
		/* Read Data Memory Bank						*/
		for (chgrp = 0; chgrp < 4; chgrp++) { 
			if (s->channelGrpOn[chgrp] == 0) continue;
			address = (bank == 0)
				? SIS_3300_BANK1_MEMORY + chgrp * 0x00080000
				: SIS_3300_BANK2_MEMORY + chgrp * 0x00080000;
			if (bmaReadChain(s->bma, memoryOffset[chgrp], s->vmeAddr + address, memSize, 0x0b) < 0) {
				sprintf(msg, "%s: block xfer failed while reading memory bank %d", s->moduleName, bank);
				f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
				return(0);
			}
		}
		
		/* submit chain */
		if (bmaStartChain(s->bma) < 0) {
			sprintf(msg, "%s: starting block xfer chain failed on bank %d", s->moduleName, bank);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}			
		if (bmaWaitChain(s->bma) < 0) {
			sprintf(msg, "%s: waiting for block xfer chain failed on bank %d", s->moduleName, bank);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
	}
	
	/* write buffer to pointer */
	
	/* Read Event timestamp directory */
	/* format: length, data					 */
	tmp = pointer++;	
	if (s->blockTransOn == 1) {
		/* copy to pointer */
		memcpy(pointer, s->bltBuffer + timeStampOffset, sizeof(uint32_t) * numEvts);
		pointer += numEvts; 
	} else {
		address = (bank == 0) ? SIS_3300_BANK1_TIME_DIR : SIS_3300_BANK2_TIME_DIR;
		for (evt = 0; evt < numEvts; evt++) {
			*pointer++ = GET32(s->baseAddr, address + evt * 4);
		}
	}
	*tmp = pointer - tmp;
	
	/* Read Trigger Event directory	 */
	/* format: length, data					 */
	tmp = pointer++;	
	if (s->blockTransOn == 1) {
		/* copy to pointer */
		memcpy(pointer, s->bltBuffer + triggerEventOffset, sizeof(uint32_t) * numEvts);
		pointer += numEvts; 
	} else {
		address = (bank == 0)
			? SIS_3300_EVENT_DIRECTORY_BANK1_ALL_ADC
			: SIS_3300_EVENT_DIRECTORY_BANK2_ALL_ADC;
		
		for (evt = 0; evt < numEvts; evt++) {
			*pointer++ = GET32(s->baseAddr, address + evt * 4);
		}
	}
	*tmp = pointer - tmp;

	/* Read Data																											 */
	/* format: 1..4 x (length, channelGroupNumber, triggermode, data)	*/
	for (chgrp = 0; chgrp < 4; chgrp++) { 
		if (s->channelGrpOn[chgrp] == 0) continue;
		
		tmp = pointer++;
		*pointer++ = (uint32_t)chgrp;
		*pointer++ = (uint32_t)s->triggerOn[chgrp];
		if (s->blockTransOn == 1) {
			/* copy to pointer */
			memcpy(pointer, s->bltBuffer + memoryOffset[chgrp], sizeof(uint32_t) * memSize);
			pointer += memSize;
		} else {
			address = (bank == 0)
				? SIS_3300_BANK1_MEMORY + chgrp * 0x00080000
				: SIS_3300_BANK2_MEMORY + chgrp * 0x00080000;
			for (evt = 0; evt < memSize; evt++) {
				*pointer++ = GET32(s->baseAddr, address + evt * 4);
			}
		}
		*tmp = pointer - tmp;
	}
	
	length = pointer - pointerBegin;		/* length of data */

	sis_3300_clearBankFull(s, bank);
	
	return (length);
}

