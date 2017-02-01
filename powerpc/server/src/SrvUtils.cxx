//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvUtils.cxx
//! \brief			MARaBOU to Lynx: Utilities
//! \details		Static utility functions
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $       
//! $Date: 2010-12-27 09:02:14 $
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "LwrTypes.h"
#include "SrvUtils.h"

Bool_t gSignalTrap = kFALSE;				//!< bus trap flag: kTRUE after SIGBUS

static Char_t * gMsgBuffer = NULL; 			//!< common data storage for messages
static Int_t gBytesAllocated = 0;			//!< size of message storage

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Clears message space and sets (swapped) length & type
//! \param[out] 	Hdr 	-- ptr to message (header)
//! \param[in]		Length	-- message length
//! \param[in]		What	-- message type
//////////////////////////////////////////////////////////////////////////////

void initMessage(M2L_MsgHdr * Hdr, Int_t Length, UInt_t What) {

	memset((Char_t *) Hdr, 0, Length);
	Hdr->fLength = swapIt(Length);
	Hdr->fWhat = swapIt(What);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Allocates space for a data message and performs init.
//! \param[in]		Length		-- length of message (= sizeof(...))
//! \param[in]		Wc			-- length of data section (words)
//! \param[in]		What		-- message type
//! \retval 		Hdr 		-- ptr to message (hdr)
//////////////////////////////////////////////////////////////////////////////

M2L_MsgHdr * allocMessage(Int_t Length, Int_t Wc, UInt_t What) {

	Int_t lengthNeeded = Length + (Wc - 1) * sizeof(Int_t);
	if (gMsgBuffer) {
		if (gBytesAllocated < lengthNeeded) {
			free(gMsgBuffer);
			gMsgBuffer = NULL;
			gBytesAllocated = 0;
		}
	}
	if (gMsgBuffer == NULL) {
		gMsgBuffer = (Char_t *) calloc(1, lengthNeeded);
		gBytesAllocated = lengthNeeded;
	}
	memset(gMsgBuffer, 0, gBytesAllocated);
	initMessage((M2L_MsgHdr *) gMsgBuffer, lengthNeeded, What);
	return((M2L_MsgHdr *) gMsgBuffer);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Swaps length and type fields
//! \param[in,out]	Hdr 	-- ptr to message (header)
//////////////////////////////////////////////////////////////////////////////

void swapHdr(M2L_MsgHdr * Hdr) {

	Hdr->fLength = swapIt(Hdr->fLength);
	Hdr->fWhat = swapIt(Hdr->fWhat);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Swaps data in place
//! \param[in,out]	Data	-- ptr to data
//! \param[in]		Wc		-- word count
//////////////////////////////////////////////////////////////////////////////

void swapData(Int_t * Data, Int_t Wc = 1) {

	for (;Wc--; Data++) *Data = swapIt(*Data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets bus trap flag
//! \param[in]		Signal    -- signal code
//////////////////////////////////////////////////////////////////////////////

void catchSignal(Int_t Signal) {

	gSignalTrap = kTRUE;
}
