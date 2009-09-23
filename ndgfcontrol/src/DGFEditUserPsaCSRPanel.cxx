//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditUserPsaCSRPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit UserPSA control bits
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditUserPsaCSRPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFEditUserPsaCSRPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "TMrbDGFData.h"
#include "DGFControlData.h"
#include "DGFEditUserPsaCSRPanel.h"

const SMrbNamedX kDGFEditUserPsaCSRButtons[] =
		{
			{DGFEditUserPsaCSRPanel::kDGFEditUserPsaCSRButtonApply, 	"Apply",	"Apply settings to UserPSA CSR" },
			{DGFEditUserPsaCSRPanel::kDGFEditUserPsaCSRButtonReset, 	"Reset",	"Clear UserPSA CSR" 			},
			{DGFEditUserPsaCSRPanel::kDGFEditUserPsaCSRButtonClose, 	"Close",	"Close window"					},
			{0, 													NULL,		NULL							}
		};

extern DGFControlData * gDGFControlData;
extern TMrbDGFData * gMrbDGFData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFEditUserPsaCSRPanel)

DGFEditUserPsaCSRPanel::DGFEditUserPsaCSRPanel(const TGWindow * Window, TNGMrbLabelEntry * Entry,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditUserPsaCSRPanel
// Purpose:        Edit UserPSA control bits
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
	fLofButtons.AddNamedX(kDGFEditUserPsaCSRButtons);

	fEntry = Entry; 			// entry to be editied

	module = gDGFControlData->GetSelectedModule();
	chn = gDGFControlData->GetSelectedChannel();

	title = "UserPSA CSR for  ";
	title += module->GetName();
	title += " in C";
	title += module->GetCrate();
	title += ".N";
	title += module->GetStation();
	title += ", Chn ";
	title += chn;

// CSR bits
	TGLayoutHints * csrFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
//	gDGFControlData->SetLH(groupGC, frameGC, csrFrameLayout);
	HEAP(csrFrameLayout);
	TGLayoutHints * csrButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
//	buttonGC->SetLH(csrButtonLayout);
	HEAP(csrButtonLayout);

	fCSRFrame = new TNGMrbCheckButtonGroup(this, title.Data(), &gMrbDGFData->fLofUserPsaCSRBits, -1,stdProfile,0, 1);
	HEAP(fCSRFrame);
	this->AddFrame(fCSRFrame, csrFrameLayout);

// get actual entry data
//	intStr = fEntry->GetText();
//	intStr.ToInteger(intVal);
	fCSRFrame->SetState(fEntry->GetHexNumber());

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
//	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonList(this, NULL, &fLofButtons, -1,stdProfile,0, 1, 
												DGFEditUserPsaCSRPanel::kButtonWidth,
												DGFEditUserPsaCSRPanel::kButtonHeight);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	SetWindowName("DGFControl: Edit UserPSA CSR");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

void DGFEditUserPsaCSRPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditUserPsaCSRPanel::PerformAction
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
		case kDGFEditUserPsaCSRButtonApply:
			btnState = fCSRFrame->GetActive();
//			intStr.FromInteger(btnState, 0, 16, kTRUE);
//			fEntry->SetText(intStr);
			fEntry->SetHexNumber(btnState);
			this->CloseWindow();
			break;
		case kDGFEditUserPsaCSRButtonReset:
			fCSRFrame->SetState((UInt_t) -1, kButtonUp);
			break;
		case kDGFEditUserPsaCSRButtonClose:
			this->CloseWindow();
			break;
	}
}
