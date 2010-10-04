#ifndef __SrvUtils_h__
#define __SrvUtils_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvUtils.h
//! \brief			MARaBOU to Lynx: Utilities
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $     
//! $Date: 2010-10-04 10:43:26 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrNamedX.h"
#include "LwrUtils.h"
#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

//! Initialize message
void initMessage(M2L_MsgHdr * Hdr, Int_t Length, UInt_t What);
//! allocate message buffers
M2L_MsgHdr * allocMessage(Int_t Length, Int_t Wc, UInt_t What);
//! swap message header
void swapHdr(M2L_MsgHdr * Hdr);
//! swap message data
void swapData(Int_t * Data, Int_t Wc = 1);
//! trap function to catch bus errors
void catchSignal(Int_t Signal);

#endif
