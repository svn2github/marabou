//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditRunTaskPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit RUNTASK parameter
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditRunTaskPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFEditRunTaskPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "TMrbDGFData.h"
#include "DGFControlData.h"
#include "DGFEditRunTaskPanel.h"

const SMrbNamedX kDGFEditRunTaskButtons[] =
		{
			{DGFEditRunTaskPanel::kDGFEditRunTaskButtonApply, 	"Apply",	"Apply settings to RUNTASK param"	},
			{DGFEditRunTaskPanel::kDGFEditRunTaskButtonReset, 	"Reset",	"Reset RUNTASK to default"			},
			{DGFEditRunTaskPanel::kDGFEditRunTaskButtonClose, 	"Close",	"Close window"						},
			{0, 													NULL,		NULL							}
		};


extern DGFControlData * gDGFControlData;
extern TMrbDGFData * gMrbDGFData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFEditRunTaskPanel)

DGFEditRunTaskPanel::DGFEditRunTaskPanel(const TGWindow * Window, TNGMrbLabelEntry * Entry,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditRunTaskPanel
// Purpose:        Edit RUNTASK parameter
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
//	TMrbString intStr;
//	Int_t intVal;

/*	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * buttonGC;

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
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
	fLofButtons.AddNamedX(kDGFEditRunTaskButtons);

	fEntry = Entry; 			// entry to be editied

	module = gDGFControlData->GetSelectedModule();
	chn = gDGFControlData->GetSelectedChannel();

	title = "RUNTASK for  ";
	title += module->GetName();
	title += " in C";
	title += module->GetCrate();
	title += ".N";
	title += module->GetStation();

// CSR bits
	TGLayoutHints * csrFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
//	gDGFControlData->SetLH(groupGC, frameGC, csrFrameLayout);
	HEAP(csrFrameLayout);
	TGLayoutHints * csrButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
//	buttonGC->SetLH(csrButtonLayout);
	HEAP(csrButtonLayout);

	fRunTaskFrame = new TNGMrbRadioButtonGroup(this, title.Data(), &gMrbDGFData->fLofRunTasks, -1,stdProfile,0, 1);
	HEAP(fRunTaskFrame);
	this->AddFrame(fRunTaskFrame,csrFrameLayout);

// get actual entry data
//	intStr = fEntry->GetText();
//	intStr.ToInteger(intVal);
	fRunTaskFrame->SetState(fEntry->GetHexNumber());

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
//	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonList(this, NULL, &fLofButtons, -1,stdProfile,0, 1, 
												DGFEditRunTaskPanel::kButtonWidth,
												DGFEditRunTaskPanel::kButtonHeight);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	SetWindowName("DGFControl: Edit RUNTASK param");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

void DGFEditRunTaskPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditRunTaskPanel::PerformAction
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
		case kDGFEditRunTaskButtonApply:
			btnState = fRunTaskFrame->GetActive();
//			intStr.FromInteger(btnState, 0, 16, kTRUE);
//			fEntry->SetText(intStr);
			fEntry->SetHexNumber(btnState);
			this->CloseWindow();
			break;
		case kDGFEditRunTaskButtonReset:
			fRunTaskFrame->SetState((UInt_t) -1, kButtonUp);
			break;
		case kDGFEditRunTaskButtonClose:
			this->CloseWindow();
			break;
	}
}
