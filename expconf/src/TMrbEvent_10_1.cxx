//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbEvent_10_1.cxx
// Purpose:        MARaBOU configuration: event definition
// Description:    Implements class methods to define events/triggers
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TMrbConfig.h"
#include "TMrbEvent.h"
#include "TMrbEvent_10_1.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbEvent_10_1)

TMrbEvent_10_1::TMrbEvent_10_1(Int_t Trigger, const Char_t * EvtName, const Char_t * EvtTitle) : TMrbEvent(Trigger, EvtName, EvtTitle) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent_10_1
// Purpose:        Create a MBS event of standard type [10,1] 
// Arguments:      Int_t Trigger           -- associated trigger number
//                 Char_t * EvtName        -- event name
//                 Char_t * EvtTitle       -- ... and title
// Results:        --
// Exceptions:
// Description:    Class constructor. Currently a synonym to TMrbEvent.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fEventType = 10;
	fEventSubtype = 1;
	fPrivateHistograms = kFALSE;
}
