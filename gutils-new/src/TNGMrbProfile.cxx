//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbProfile.cxx
// Purpose:        MARaBOU utilities: Graphics profiles
// Description:    Implements utilities to specify GC
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbProfile.cxx,v 1.6 2009-09-23 10:42:52 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "THashList.h"
#include "TVirtualX.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGClient.h"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TNGMrbProfile.h"
#include "TMrbLogger.h"
#include "TMrbString.h"

#include "SetColor.h"

ClassImp(TNGMrbGContext)
ClassImp(TNGMrbProfile)
ClassImp(TNGMrbLofProfiles)

TNGMrbLofProfiles * gMrbLofProfiles = NULL;	// system list of graph envs

static TString frameOptString = Form("SunkenFrame=%d:RaisedFrame=%d:DoubleBorder=%d", kSunkenFrame, kRaisedFrame, kDoubleBorder);

extern TMrbLogger * gMrbLog;			// access to message lgging

const SMrbNamedXShort kGMrbGCTypes[] =
							{
								{TNGMrbGContext::kGMrbGCFrame,			"Frame" 		},
								{TNGMrbGContext::kGMrbGCLabel,			"Label" 		},
								{TNGMrbGContext::kGMrbGCButton,			"Button"		},
								{TNGMrbGContext::kGMrbGCCombo,			"Combo" 		},
								{TNGMrbGContext::kGMrbGCEntry,			"Entry"			},
								{TNGMrbGContext::kGMrbGCGroupFrame,		"GroupFrame"	},
								{TNGMrbGContext::kGMrbGCTextEntry,		"TextEntry"		},
								{TNGMrbGContext::kGMrbGCLBEntry,		"ListBoxEntry"	},
								{TNGMrbGContext::kGMrbGCTextButton,		"TextButton"	},
								{TNGMrbGContext::kGMrbGCRadioButton, 	"RadioButton"	},
								{TNGMrbGContext::kGMrbGCCheckButton, 	"CheckButton"	},
								{TNGMrbGContext::kGMrbGCPictureButton,	"PictureButton"	},
								{0, 				NULL		}
							};

TNGMrbGContext::TNGMrbGContext(const Char_t * Font, const Char_t * Foreground, const Char_t * Background, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext
// Purpose:        Graphics context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Char_t * Foreground   -- foreground color
//                 Char_t * Background   -- background color
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	FontStruct_t font;
	TGFont * tgfont;
	TString fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data()) ) == 0 || (tgfont=gClient->GetFont(fontStr.Data()))==0) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl
						<< "Falling back to \"NormalFont\"" << endl;
		gMrbLog->Flush("TNGMrbGContext");
		font = gClient->GetFontByName(gEnv->GetValue("Gui.NormalFont","-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
		tgfont=gClient->GetFont(gEnv->GetValue("Gui.NormalFont","-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	Pixel_t fg;
	if (!gClient->GetColorByName(Foreground, fg)) {
		gMrbLog->Wrn()	<< "No such color - " << Foreground << " (foreground)" << endl
						<< "Falling back to \"black\"" << endl;
		gMrbLog->Flush("TNGMrbGContext");
		gClient->GetColorByName("black", fg);
	}

	Pixel_t bg;
	if (!gClient->GetColorByName(Background, bg)) {
		gMrbLog->Wrn()	<< "No such color - " << Background << " (background)" << endl
						<< "Falling back to \"white\"" << endl;
		gMrbLog->Flush("TNGMrbGContext");
		gClient->GetColorByName("white", bg);
	}

	fFontName = fontStr;
	fFont = font;
	fTGFont= tgfont;
	fForegroundName = Foreground;
	fForeground = fg;
	fBackgroundName = Background;
	fBackground = bg;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fTGFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

TNGMrbGContext::TNGMrbGContext(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext
// Purpose:        Graphics context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Pixel_t Foreground    -- foreground color
//                 Pixel_t Background    -- background color
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	TString fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");

	FontStruct_t font;
	TGFont * tgfont;
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data())) == 0 || (tgfont=gClient->GetFont(fontStr.Data()))==0) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl
						<< "Falling back to \"NormalFont\"" << endl;
		gMrbLog->Flush("TNGMrbGContext");
		font = gClient->GetFontByName(gEnv->GetValue("Gui.NormalFont",
													"-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
		tgfont=gClient->GetFont(gEnv->GetValue("Gui.NormalFont","-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	fFontName = fontStr;
	fFont = font;
	fTGFont= tgfont;
	fForegroundName = Form("#%0x", Foreground);
	fForeground = Foreground;
	fBackgroundName = Form("#%0x", Background);
	fBackground = Background;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fTGFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

TNGMrbGContext::TNGMrbGContext(TNGMrbGContext::EGMrbGCType Type, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext
// Purpose:        Graphics context within MARaBOU
// Arguments:      EGMrbGCType Type   -- grpahic object type
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fFontName = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	FontStruct_t font = gClient->GetFontByName(fFontName.Data());
	TGFont * tgfont=gClient->GetFont(fFontName.Data());
	
	fFont = font;
	fTGFont= tgfont;

	Pixel_t fg;
	fForegroundName = "black";
	gClient->GetColorByName(fForegroundName.Data(), fg);
	fForeground = fg;

	fBackgroundName = (Type >= TNGMrbGContext::kGMrbGCTextEntry) ? "white" : "gray";
	Pixel_t bg;
	gClient->GetColorByName(fBackgroundName.Data(), bg);
	fBackground = bg;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fTGFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

Bool_t TNGMrbGContext::SetFont(const Char_t * Font) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext::SetFont
// Purpose:        Change font in a GC
// Arguments:      Char_t * Font      -- font name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a font.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;
	FontStruct_t font;
	TGFont * tgfont;

	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	
	if ((fontStr.Length() == 0) || (font = gClient->GetFontByName(fontStr.Data())) == 0 || (tgfont=gClient->GetFont(fontStr.Data()))==0) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl
						<< "Falling back to \"NormalFont\"" << endl;
		gMrbLog->Flush("TNGMrbGContext", "SetFont");
		return(kFALSE);
	}
	fFontName = fontStr; 
	fFont = font;
	fTGFont=tgfont;
	fGC->SetFont(fTGFont->GetFontHandle());
	return(kTRUE);
}

Bool_t TNGMrbGContext::SetFG(const Char_t * Foreground) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext::SetFG
// Purpose:        Change foreground
// Arguments:      Char_t * Foreground  -- foreground color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a foreground.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Foreground, color)) {
		gMrbLog->Wrn()	<< "No such color - " << Foreground << " (foreground)" << endl
						<< "Falling back to \"black\"" << endl;
		gMrbLog->Flush("TNGMrbGContext", "SetFG");
		gClient->GetColorByName("black", color);
	}

	fForegroundName = Foreground; 
	fForeground = color;
	fGC->SetForeground(color);
	return(kTRUE);
}

Bool_t TNGMrbGContext::SetBG(const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext::SetBG
// Purpose:        Change background
// Arguments:      Char_t * Background  -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a background.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Background, color)) {
		gMrbLog->Wrn()	<< "No such color - " << Background << " (background)" << endl
						<< "Falling back to \"white\"" << endl;
		gMrbLog->Flush("TNGMrbGContext", "SetBG");
		gClient->GetColorByName("white", color);
	}

	fBackgroundName = Background; 
	fBackground = color;
	fGC->SetBackground(color);
	return(kTRUE);
}

void TNGMrbGContext::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbGContext::Print
// Purpose:        Print graphic context
// Arguments:      ostream & Out        -- output stream
// Results:        --
// Exceptions:
// Description:    Output profile data to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << endl << "Graph context:" << endl;
	Out << Form("Font         : ")
		<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
		<< this->Font() << resetiosflags(ios::uppercase) << setbase(10)
		<< " = " << this->GetFontName() << endl;
	Out << Form("Foreground   : ")
		<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
		<< this->FG() << resetiosflags(ios::uppercase) << setbase(10)
		<< " = " << this->GetForegroundName() << endl;
	Out << Form("Background   : ")
		<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
		<< this->BG() << resetiosflags(ios::uppercase) << setbase(10)
		<< " = " << this->GetBackgroundName() << endl;
	TMrbString opt;
	opt.Decode(this->GetOptions(), frameOptString.Data());
	Out << Form("Options      : ")
		<< setbase(16) << setiosflags(ios::showbase) << this->GetOptions() << resetiosflags(ios::showbase) << setbase(10);
	if (!opt.IsNull()) Out << " = " << opt;
	Out << endl;
}
 
TNGMrbProfile::TNGMrbProfile(const Char_t * Name, const Char_t * Title, TEnv * Env) : TNamed(Name, Title) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile
// Purpose:        Define graphics profile
// Arguments:      Char_t * Name   -- name
//                 Char_t * Title  -- title
//                 TEnv * Env      -- env definitions
// Description:    Defines a graphics profile:
//                 a list of TNGMrbGContext objects together with some options
//                 and other global params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fLofGCs.Delete();
	fLofGCs.SetPatternMode();

	TMrbLofNamedX wtypes;
	wtypes.AddNamedX(kGMrbGCTypes);

	TMrbNamedX * bnx = wtypes.FindByIndex(TNGMrbGContext::kGMrbGCButton);
	TNGMrbGContext * bgc = this->AddGC(bnx, Env);

	TMrbNamedX * enx = wtypes.FindByIndex(TNGMrbGContext::kGMrbGCEntry);
	TNGMrbGContext * egc = this->AddGC(enx, Env);

	TIterator * iter = wtypes.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		TNGMrbGContext * dfltGC = NULL;
		if (nx->GetIndex() & TNGMrbGContext::kGMrbGCButton)		dfltGC = bgc;
		else if (nx->GetIndex() & TNGMrbGContext::kGMrbGCEntry)	dfltGC = egc;
		this->AddGC(nx, Env, dfltGC);
	}
}
		
TNGMrbGContext * TNGMrbProfile::AddGC(TMrbNamedX * GCSpec, TEnv * Env, TNGMrbGContext * DefaultGC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::AddGC
// Purpose:        Add a graphics context to list
// Arguments:      TMrbNamedX * GCSpec         -- graphics context specs
//                 TEnv * Env                  -- ROOT environment
//                 TNGMrbGContext * DefaultGC  -- default context
// Results:        TNGMrbGContext * GC         -- context
// Exceptions:
// Description:    Fills table 'fLofGCs'
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGCs.FindByIndex(GCSpec->GetIndex());
	if (nx) return((TNGMrbGContext *) nx->GetAssignedObject());

	if (Env == NULL) Env = gEnv;

	TNGMrbGContext * gc = new TNGMrbGContext((TNGMrbGContext::EGMrbGCType) GCSpec->GetIndex());

	TString res = this->GetName();
	res.ToLower();
	this->SetName(res.Data());
	res(0,1).ToUpper();
	TString rName = res + ".";
	rName = rName + GCSpec->GetName();

	TString dflt = DefaultGC ? DefaultGC->GetFontName() : "";
	TString r = Env->GetValue(rName + ".Font", dflt.Data());
	if (!r.IsNull()) gc->SetFont(r.Data());

	dflt = DefaultGC ? DefaultGC->GetForegroundName() : "";
	r = Env->GetValue(rName + ".Foreground", dflt.Data());
	if (!r.IsNull()) gc->SetFG(r.Data());

	dflt = DefaultGC ? DefaultGC->GetBackgroundName() : "";
	r = Env->GetValue(rName + ".Background", dflt.Data());
	if (!r.IsNull()) gc->SetBG(r.Data());

	TMrbString opt = Env->GetValue(rName + ".Options", "");
	UInt_t optionBits;
	opt.Encode(optionBits, frameOptString.Data());
	if (optionBits == 0) optionBits = DefaultGC ? DefaultGC->GetOptions() : 0;
	gc->SetOptions(optionBits);

	fLofGCs.AddNamedX(GCSpec->GetIndex(), GCSpec->GetName(), "", gc);

	return(gc);
}

Bool_t TNGMrbProfile::SetGC(TNGMrbGContext::EGMrbGCType Type, TNGMrbGContext * GC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type        -- graphics context type
//                 TNGMrbGContext * GC      -- context
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByIndex(Type);
	if (nx == NULL) {
		gMrbLog->Err() << "No such graph context t type - " << Type << endl;
		gMrbLog->Flush(this->ClassName(), "SetGC");
		return(kFALSE);
	}
	nx->AssignObject(GC);
	return(kTRUE);
}

Bool_t TNGMrbProfile::SetGC(TNGMrbGContext::EGMrbGCType Type, const Char_t * Font, const Char_t * Foreground, const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type              -- gc type
//                 Char_t * Font                 -- font name
//                 Char_t * Foreground           -- foreground color
//                 Char_t * Background           -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbGContext * gc = new TNGMrbGContext(Font, Foreground, Background);
	if (this->SetGC(Type, gc)) return(kTRUE);
	delete gc;
	return(kFALSE);
}
	
Bool_t TNGMrbProfile::SetGC(TNGMrbGContext::EGMrbGCType Type, const Char_t * Font, Pixel_t Foreground, Pixel_t Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type              -- gc type
//                 Char_t * Font                 -- font name
//                 Pixel_t Foreground            -- foreground color
//                 Pixel_t Background            -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbGContext * gc = new TNGMrbGContext(Font, Foreground, Background);
	if (this->SetGC(Type, gc)) return(kTRUE);
	delete gc;
	return(kFALSE);
}

TNGMrbGContext * TNGMrbProfile::GetGC(TNGMrbGContext::EGMrbGCType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::GetGC
// Purpose:        Get graphics context
// Arguments:      EGMrbGCType Type              -- gc type
// Results:        TNGMrbGContext * GC            -- graphics context
// Exceptions:
// Description:    Table lookup in 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByIndex(Type);
	if (nx == NULL) {
		TMrbLofNamedX wtypes;
		wtypes.AddNamedX(kGMrbGCTypes);
		nx = wtypes.FindByIndex(Type);
		if (nx) {
			gMrbLog->Err() << "No such graph cont type - " << nx->GetName() << "(0x" << setbase(16) << nx->GetIndex() << ")" << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "GetGC");
		} else {
			gMrbLog->Err() << "No such graph cont type - " << "0x" << setbase(16) << Type << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "GetGC");
		}
		return(NULL);
	}
	return((TNGMrbGContext *) nx->GetAssignedObject());
}

void TNGMrbProfile::SetOptions(TNGMrbGContext::EGMrbGCType Type, UInt_t Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetOptions
// Purpose:        Set option bits
// Arguments:      EGMrbGCType Type              -- gc type
//                 UInt_t Options                -- option bits
// Results:        --
// Exceptions:
// Description:    Defines option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbGContext * gc = this->GetGC(Type);
	if (gc != NULL) gc->SetOptions(Options);
}

UInt_t  TNGMrbProfile::GetOptions(TNGMrbGContext::EGMrbGCType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::GetOptions
// Purpose:        Get option bits
// Arguments:      EGMrbGCType Type              -- gc type
// Results:        UInt_t Options                -- option bits
// Exceptions:
// Description:    Returns option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbGContext * gc = this->GetGC(Type);
	if (gc == NULL) return(0);
	else			return(gc->GetOptions());
}

UInt_t TNGMrbProfile::GetFrameOptions() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::GetFrameOptions
// Purpose:        Get frame options
// Arguments:      --
// Results:        UInt_t Options                -- option bits
// Exceptions:
// Description:    Returns option bits for a framet.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * gc = fLofGCs.FindByIndex(TNGMrbGContext::kGMrbGCFrame);
	return(gc ? ((TNGMrbGContext *) gc->GetAssignedObject())->GetOptions() : 0);
}

void TNGMrbProfile::SetOptions(const Char_t * Type, UInt_t Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetOptions
// Purpose:        Set option bits
// Arguments:      Char_t * Type                 -- gc type
//                 UInt_t Options                -- option bits
// Results:        --
// Exceptions:
// Description:    Defines option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByName(Type);
	if (nx) ((TNGMrbGContext *) nx->GetAssignedObject())->SetOptions(Options);
}

UInt_t  TNGMrbProfile::GetOptions(const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::SetOptions
// Purpose:        Get option bits
// Arguments:      Char_t * Type                 -- gc type
// Results:        UInt_t Options                -- option bits
// Exceptions:
// Description:    Returns option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByName(Type);
	if (nx == NULL) return(0);
	else			return(((TNGMrbGContext *) nx->GetAssignedObject())->GetOptions());
}

void TNGMrbProfile::Print(ostream & Out, const Char_t * Type) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile::Print
// Purpose:        Print profile data
// Arguments:      ostream & Out        -- output stream
//                 Char_t * Type        -- graph context type
// Results:        --
// Exceptions:
// Description:    Output profile data to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << endl << "Graph profile \"" << this->GetName() << "\"";
	TString title = this->GetTitle();
	if (!title.IsNull()) Out << " (" << title << ")";
	Out << ":" << endl;
	TIterator * iter = fLofGCs.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		Out << Form("%-15s", nx->GetName());
		TNGMrbGContext * gc = (TNGMrbGContext *) nx->GetAssignedObject();
		if (gc == NULL) {
			Out << "(empty)" << endl;
		} else {
			Out << Form("%15s", "[Font]: ") << gc->GetFontName() << endl;
			Out << Form("%15s%15s", "", "[Foreground]: ")
				<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
				<< gc->FG() << resetiosflags(ios::uppercase) << setbase(10)
				<< " = " << gc->GetForegroundName() << endl;
			Out << Form("%15s%15s", "", "[Background]: ")
				<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
				<< gc->BG() << resetiosflags(ios::uppercase) << setbase(10)
				<< " = " << gc->GetBackgroundName() << endl;
			TMrbString opt;
			opt.Decode(gc->GetOptions(), frameOptString.Data());
			Out << Form("%15s%15s", "", "[Options]: ")
				<< setbase(16) << setiosflags(ios::showbase) << gc->GetOptions() << resetiosflags(ios::showbase) << setbase(10);
			if (!opt.IsNull()) Out << " = " << opt;
			Out << endl;
		}
	}
}
 
TNGMrbLofProfiles::TNGMrbLofProfiles(const Char_t * Name, const Char_t * Title) : TMrbLofNamedX(Name, Title) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles
// Purpose:        A list of graphics profiles
// Arguments:      Char_t * Name   -- name
//                 Char_t * Title  -- title
// Description:    Defines a list of graphics profiles
//                 Tries to get profile specs from file given by resource <Name>.RcFile
//                 If this fails tries file .gprofrc
//                 If this fails looks at defs in .rootrc
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	TString name = this->GetName();
	TString title = this->GetTitle();
	if (title.IsNull() && name.CompareTo("LofGraphProfiles") == 0) this->SetTitle("MARaBOU's list of graphics profiles");

	TString res = this->GetName();
	res(0,1).ToUpper();
	TString envPath = res;
	envPath += ".RcFile";
	fRcFile = gEnv->GetValue(envPath.Data(), "");
	if (fRcFile.IsNull()) {
		envPath = this->ClassName();
		envPath += ".RcFile";
		fRcFile = gEnv->GetValue(envPath.Data(), "");
	}
	if (fRcFile.IsNull()) {
		TEnv * e = new TEnv(".gprofrc");
		if (e->GetTable()->Last()) {
			fEnv = e;
		} else {
			fEnv = gEnv;
			delete e;
		}
	} else {
		fEnv = new TEnv(fRcFile.Data());
	}
	TString mstr = fEnv->GetValue(res + ".Members", "");
	if (mstr.IsNull()) {
		gMrbLog->Wrn() << "No graphics profiles defined (resource \"" << res << ".Members\" missing)" << endl;
		gMrbLog->Flush("TNGMrbLofProfiles");
	} else {
		Int_t from = 0;
		Int_t n = 0;
		TString member;
		while (mstr.Tokenize(member, from, ":")) {
			member.ToLower();
			res = member;
			res(0,1).ToUpper();
			title = fEnv->GetValue(res + ".Title", "");
			TNGMrbProfile * gc = new TNGMrbProfile(member.Data(), title.Data(), fEnv);
			this->AddNamedX(this->GetEntries(), member.Data(), title.Data(), gc);
			n++;
		}
		if (n == 0) {
			res = this->GetName();
			res(0,1).ToUpper();
			gMrbLog->Wrn() << "No graphics profiles defined (resource \"" << res << ".Members\" is empty)" << endl;
			gMrbLog->Flush("TNGMrbLofProfiles");
		}
	}

	if (this->FindProfile("standard", kFALSE) == NULL) this->AddProfile("standard", "Standard graphics profile");
	res = this->GetName();
	res(0,1).ToUpper();
	TString defGC = fEnv->GetValue(res + ".Default", "");
	this->SetDefault(defGC.IsNull() ? "standard" : defGC.Data());
}

TNGMrbProfile * TNGMrbLofProfiles::AddProfile(const Char_t * Name, const Char_t * Title) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles::AddProfile
// Purpose:        Add a graphics profile to list
// Arguments:      Char_t * Name    -- name
//                 Char_t * Title   -- title
// Results:        --
// Exceptions:
// Description:    Adds a new graphics profile.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbProfile * gc;

	TString name = Name;
	name.ToLower();
	if ((gc = this->FindProfile(name.Data())) != NULL) {
		gMrbLog->Wrn() << "Graphics profile already defined - " << name << " (ignored)" << endl;
		gMrbLog->Flush(this->ClassName(), "AddProfile");
		return(gc);
	}
	gc = new TNGMrbProfile(name.Data(), Title);
	this->AddNamedX(this->GetEntries(), name.Data(), Title, gc);
	return(gc);
}

TNGMrbProfile * TNGMrbLofProfiles::FindProfile(const Char_t * Name, Bool_t FallBack) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles::FindProfile
// Purpose:        Find a given graph profile
// Arguments:      Char_t * Name          -- name
// Results:        TNGMrbProfile * Profile -- profile
// Exceptions:
// Description:    Searches for a given profile
//                 FallBack = kTRUE: Returns "default" profile if search fails
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString name = Name;
	name.ToLower();
	TMrbNamedX * nx = this->FindByName(name.Data());
	if (nx) return((TNGMrbProfile *) nx->GetAssignedObject());
	if (FallBack) return(fDefault);
	return(NULL);
}

TNGMrbProfile * TNGMrbLofProfiles::SetDefault(const Char_t * Name) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles::SetDefault
// Purpose:        Define a default graph profile
// Arguments:      Char_t * Name          -- name
// Results:        TNGMrbProfile * Profile -- default profile
// Exceptions:
// Description:    Defines a graph profile to be default.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TNGMrbProfile * gc = this->FindProfile(Name);
	if (gc == NULL) {
		gMrbLog->Err() << "Graphics profile not found - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "SetDefault");
		return(NULL);
	}
	fDefault = gc;
	return(gc);
}

void TNGMrbLofProfiles::Print(ostream & Out, const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles::Print
// Purpose:        Print a list of graph envs
// Arguments:      ostream & Out        -- output stream
//                 Char_t * Name        -- env name
// Results:        --
// Exceptions:
// Description:    Output list of profiles to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString name;
	TNGMrbProfile * gc;

	TString defName = fDefault->GetName();

	Out << endl << "=========================================================================================" << endl;
	Out << "List of graphics profiles: " << this->GetName();
	TString title = this->GetTitle();
	if (!title.IsNull()) Out << " (" << title << ")";
	Out << endl << "-----------------------------------------------------------------------------------------" << endl;
	if (Name != NULL) {
		name = Name;
		name.ToLower();
	}
	TIterator * iter = this->MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		if (Name == NULL || name.CompareTo(nx->GetName()) == 0) {
			gc = (TNGMrbProfile *) nx->GetAssignedObject();
			gc->Print(Out);
		}
	}

	Out << endl << "[Default profile is \"" << fDefault->GetName() << "\"]" << endl;
	if (!fRcFile.IsNull()) Out << "[Settings as read from file \"" << fRcFile << "\"]" << endl;
}
