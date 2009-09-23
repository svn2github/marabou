//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit Channel CSRA
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditChanCSRAPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFEditChanCSRAPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "TMrbDGFData.h"
#include "DGFControlData.h"
#include "DGFEditChanCSRAPanel.h"

const SMrbNamedX kDGFEditChanCSRAButtons[] =
		{
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonApply, 	"Apply",	"Apply settings to channel CSR" },
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonReset, 	"Reset",	"Clear channel CSR" 			},
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonClose, 	"Close",	"Close window"					},
			{0, 													NULL,		NULL							}
		};

extern DGFControlData * gDGFControlData;
extern TMrbDGFData * gMrbDGFData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFEditChanCSRAPanel)

DGFEditChanCSRAPanel::DGFEditChanCSRAPanel(const TGWindow * Window, TNGMrbLabelEntry * Entry,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel
// Purpose:        Edit module CSRA bits
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 TGWindow * MainFrame -- main frame
//                 TGTextEntry * Entry  -- entry to be edited
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
//                 UInt_t Options       -- options
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
	stdProfile = gMrbLofProfiles->GetDefault();
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////

	DGFModule * module;
	Int_t chn;

	TMrbString title;


//	TGMrbLayout * frameGC;
//	TGMrbLayout * groupGC;
//	TGMrbLayout * buttonGC;

/*	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);
	
	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);*/
	
	this->ChangeBackground(gDGFControlData->fColorGreen);

	if (gMrbDGFData == NULL) gMrbDGFData = new TMrbDGFData();

//	Initialize button list
	fLofButtons.SetName("Buttons");
	fLofButtons.AddNamedX(kDGFEditChanCSRAButtons);

	fEntry = Entry; 			// entry to be editied

	module = gDGFControlData->GetSelectedModule();
	chn = gDGFControlData->GetSelectedChannel();

	title = "Channel CSRA for  ";
	title += module->GetName();
	title += " in C";
	title += module->GetCrate();
	title += ".N";
	title += module->GetStation();
	title += ", Chn ";
	title += chn;

// CSRA bits
	TGLayoutHints * csraFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
//	gDGFControlData->SetLH(groupGC, frameGC, csraFrameLayout);
	HEAP(csraFrameLayout);
	TGLayoutHints * csraButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
//	buttonGC->SetLH(csraButtonLayout);
	HEAP(csraButtonLayout);

	fCSRAFrame = new TNGMrbCheckButtonGroup(this, title.Data(), &gMrbDGFData->fLofChanCSRABits, -1,stdProfile,0, 1);
	HEAP(fCSRAFrame);
	this->AddFrame(fCSRAFrame, csraFrameLayout);

// get actual entry data
//	intStr = fEntry->GetText();
//	intStr.ToInteger(intVal);
	fCSRAFrame->SetState(fEntry->GetHexNumber());

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
//	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonList(this, NULL, &fLofButtons, -1,stdProfile,0, 1, 
												DGFEditChanCSRAPanel::kButtonWidth,
												DGFEditChanCSRAPanel::kButtonHeight);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	SetWindowName("DGFControl: Edit Channel CSRA");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

void DGFEditChanCSRAPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t btnState;
	TMrbString intStr;
	switch (Selection) {
		case kDGFEditChanCSRAButtonApply:
			btnState = fCSRAFrame->GetActive();
//			intStr.FromInteger(btnState, 0, 16, kTRUE);
//			fEntry->SetText(intStr);
			fEntry->SetHexNumber(btnState);
			this->CloseWindow();
			break;
		case kDGFEditChanCSRAButtonReset:
			fCSRAFrame->SetState((UInt_t) -1, kButtonUp);
			break;
		case kDGFEditChanCSRAButtonClose:
			this->CloseWindow();
			break;
	}
}
