//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLayout.cxx
// Purpose:        MARaBOU utilities: Graphic context
// Description:    Implements utilities to specify GC
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TVirtualX.h"
#include "TGWindow.h"
#include "TGClient.h"
#include "TGMrbLayout.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TGMrbLayout)

extern TMrbLogger * gMrbLog;			// access to message lgging

TGMrbLayout::TGMrbLayout(const Char_t * Font, const Char_t * Foreground, const Char_t * Background, TGLayoutHints * Hints) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        Graphic context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Char_t * Foreground   -- foreground color
//                 Char_t * Background   -- background color
// Description:    A graphic context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;

	FontStruct_t font;
	ULong_t fg;
	ULong_t bg;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout");
		font = gClient->GetFontByName(gEnv->GetValue("Gui.NormalFont",
													"-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	if (!gClient->GetColorByName(Foreground, fg)) {
		gMrbLog->Err() << "No such color - " << Foreground << " (foreground)" << endl;
		gMrbLog->Flush("TGMrbLayout");
		gClient->GetColorByName("black", fg);
	}

	if (!gClient->GetColorByName(Background, bg)) {
		gMrbLog->Err() << "No such color - " << Background << " (background)" << endl;
		gMrbLog->Flush("TGMrbLayout");
		gClient->GetColorByName("white", bg);
	}

	fGC = this->CreateGC(font, fg, bg);
	fFont = font;
	fForeground = fg;
	fBackground = bg;
	fLayoutHints = Hints;
}

TGMrbLayout::TGMrbLayout(const Char_t * Font, ULong_t Foreground, ULong_t Background, TGLayoutHints * Hints) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        Graphic context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 ULong_t Foreground    -- foreground color
//                 ULOng_t Background    -- background color
// Description:    A graphic context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;

	FontStruct_t font;

	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout");
		font = gClient->GetFontByName(gEnv->GetValue("Gui.NormalFont",
													"-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	fGC = this->CreateGC(font, Foreground, Background);
	fFont = font;
	fForeground = Foreground;
	fBackground = Background;
	fLayoutHints = Hints;
}

Bool_t TGMrbLayout::SetFont(const Char_t * Font) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetFont
// Purpose:        Change font in a GC
// Arguments:      Char_t * Font      -- font name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a font.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;
	FontStruct_t font;

	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout", "SetFont");
		return(kFALSE);
	}
	fFont = font; 
	fGC = this->CreateGC(font, fForeground, fBackground);
	return(kTRUE);
}

Bool_t TGMrbLayout::SetFG(const Char_t * Foreground) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetFG
// Purpose:        Change foreground
// Arguments:      Char_t * Foreground  -- foreground color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a foreground.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ULong_t color;

	if (!gClient->GetColorByName(Foreground, color)) {
		gMrbLog->Err() << "No such color - " << Foreground << " (foreground)" << endl;
		gMrbLog->Flush("TGMrbLayout", "SetFG");
		gClient->GetColorByName("black", color);
	}

	fForeground = color; 
	fGC = this->CreateGC(fFont, fForeground, fBackground);
	return(kTRUE);
}

Bool_t TGMrbLayout::SetBG(const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetBG
// Purpose:        Change background
// Arguments:      Char_t * Background  -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a background.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ULong_t color;

	if (!gClient->GetColorByName(Background, color)) {
		gMrbLog->Err() << "No such color - " << Background << " (background)" << endl;
		gMrbLog->Flush("TGMrbLayout", "SetBG");
		gClient->GetColorByName("white", color);
	}

	fBackground = color; 
	fGC = this->CreateGC(fFont, fForeground, fBackground);
	return(kTRUE);
}

GContext_t TGMrbLayout::CreateGC(FontStruct_t Font, ULong_t Foreground, ULong_t Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::CreateGC
// Purpose:        Create a new GC
// Arguments:      FontStruct_t Font     -- font
//                 ULong_t Foreground    -- foreground
//                 ULong_t Background    -- background
// Results:        GContext_t GC         -- graphic context
// Exceptions:
// Description:    Creates a new GC.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	GCValues_t gval;

	gval.fMask = kGCForeground | kGCBackground | kGCFont | kGCFillStyle | kGCGraphicsExposures;
	gval.fFillStyle = kFillSolid;
	gval.fGraphicsExposures = kFALSE;
	gval.fForeground = Foreground;
	gval.fBackground = Background;
	gval.fFont = gVirtualX->GetFontHandle(Font);
	return(gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval));
}
