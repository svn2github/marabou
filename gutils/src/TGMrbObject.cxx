//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbObject.cxx
// Purpose:        MARaBOU graphic utilities: common methods
// Description:    Implements class methods common to graphic objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbObject.cxx,v 1.2 2004-09-28 13:47:33 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TGFrame.h"
#include "TGMrbObject.h"

ClassImp(TGMrbObject)

TGMrbLayout * TGMrbObject::SetupGC(TGMrbLayout * Layout, UInt_t FrameOptions) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbObject::SetupGC
// Purpose:        Initialize graphic context & layout
// Arguments:      TGMrbLayout * Layout    -- layout & graphic context
//                 UInt_t FrameOptions     -- frame orientation
// Results:        TGMrbLayout * Layout    -- (modified) layout
// Exceptions:     
// Description:    Initializes layout specs
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	TGLayoutHints * hints;

	if (Layout == NULL) {
		font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
		Layout = new TGMrbLayout(font, "black", "white");
		fHeap.AddFirst((TObject *) Layout);
	}
	if (Layout->LH() == NULL) {
		if (FrameOptions & kHorizontalFrame)	hints = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 2, 2, 2);
		else									hints = new TGLayoutHints(kLHintsCenterY | kLHintsExpandY, 2, 2, 2, 2);
		fHeap.AddFirst((TObject *) hints);
		Layout->SetLH(hints);
	}
	return(Layout);
}

