//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    An entry in the list of modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TmrbModuleListEntry.cxx,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbModuleListEntry.h"

ClassImp(TMrbModuleListEntry)

void TMrbModuleListEntry::ResetChannelHits(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry::ResetChannelHits
// Purpose:        Reset module hits
// Arguments:      Int_t Channel           -- channel number
// Results:        --
// Exceptions:     
// Description:    Resets hit counter for given channel.
//                 Resets all channels if channel == -1.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Channel == -1) {
		fNofChannelHits.Reset(0);
	} else {
		fNofChannelHits[Channel] = 0;
	}
}

void TMrbModuleListEntry::IncrChannelHits(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry::IncrHits
// Purpose:        Increment module hits
// Arguments:      Int_t Channel           -- channel number
// Results:        --
// Exceptions:     
// Description:    Increments hit counter for given channel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fNofChannelHits[Channel]++;
}

Int_t TMrbModuleListEntry::GetChannelHits(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry::GetHits
// Purpose:        Return module hits
// Arguments:      Int_t Channel           -- channel number
// Results:        Int_t HitCount          -- hit counter
// Exceptions:     
// Description:    Returns hit counter for given channel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	return(fNofChannelHits[Channel]);
}

