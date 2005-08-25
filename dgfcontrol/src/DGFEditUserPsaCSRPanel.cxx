//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditUserPsaCSRPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit UserPSA control bits
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditUserPsaCSRPanel.cxx,v 1.1 2005-08-25 14:35:34 Rudolf.Lutter Exp $       
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

ClassImp(DGFEditUserPsaCSRPanel)

DGFEditUserPsaCSRPanel::DGFEditUserPsaCSRPanel(const TGWindow * Window, TGTextEntry * Entry,
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

	DGFModule * module;
	Int_t chn;

	TMrbString title;
	TMrbString intStr;
	Int_t intVal;

	TGMrbLayout * frameGC;
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
								gDGFControlData->fColorGray);	HEAP(buttonGC);
	
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
	gDGFControlData->SetLH(groupGC, frameGC, csrFrameLayout);
	HEAP(csrFrameLayout);
	TGLayoutHints * csrButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
	buttonGC->SetLH(csrButtonLayout);
	HEAP(csrButtonLayout);

	fCSRFrame = new TGMrbCheckButtonGroup(this, title.Data(), &gMrbDGFData->fLofUserPsaCSRBits, -1, 1, groupGC, buttonGC, NULL, kVerticalFrame);
	HEAP(fCSRFrame);
	this->AddFrame(fCSRFrame, groupGC->LH());

// get actual entry data
	intStr = fEntry->GetText();
	intStr.ToInteger(intVal);
	fCSRFrame->SetState(intVal);

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonList(this, NULL, &fLofButtons, -1, 1, 
												DGFEditUserPsaCSRPanel::kButtonWidth,
												DGFEditUserPsaCSRPanel::kButtonHeight,
												frameGC, NULL, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	fButtonFrame->Associate(this);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	SetWindowName("DGFControl: Edit UserPSA CSR");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

Bool_t DGFEditUserPsaCSRPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditUserPsaCSRPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFEditUserPsaCSRPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t btnState;
	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case kDGFEditUserPsaCSRButtonApply:
							btnState = fCSRFrame->GetActive();
							intStr.FromInteger(btnState, 0, '0', 16);
							fEntry->SetText(intStr);
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
			break;
			
		case kC_KEY:
			switch (Param1) {
				case TGMrbLofKeyBindings::kGMrbKeyActionClose:
					this->CloseWindow();
					break;
			}
			break;
	}
	return(kTRUE);
}
