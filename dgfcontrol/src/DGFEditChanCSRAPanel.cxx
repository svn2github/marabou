//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditChanCSRAPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit Channel CSRA
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditChanCSRAPanel.cxx,v 1.4 2004-09-28 13:47:32 rudi Exp $       
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

#include "DGFControlData.h"
#include "DGFEditChanCSRAPanel.h"

const SMrbNamedX kDGFEditChanCSRAButtons[] =
		{
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonApply, 	"Apply",	"Apply settings to channel CSR" },
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonReset, 	"Reset",	"Clear channel CSR" 			},
			{DGFEditChanCSRAPanel::kDGFEditChanCSRAButtonClose, 	"Close",	"Close window"					},
			{0, 													NULL,		NULL							}
		};

// bit definitions in parameter CHANCSRA: control/status reg A for channel X
const SMrbNamedXShort kDGFChanCSRABits[] =
							{
								{TMrbDGFData::kGroupTriggerOnly,	"Respond to group triggers only"		},
								{TMrbDGFData::kIndivLiveTime,		"Measure individual live time"			},
								{TMrbDGFData::kGoodChannel, 		"Good channel"							},
								{TMrbDGFData::kReadAlways,			"Read always"							},
								{TMrbDGFData::kEnableTrigger,		"Enable trigger"						},
								{TMrbDGFData::kTriggerPositive, 	"Trigger positive"						},
								{TMrbDGFData::kGFLTValidate,		"Validate events using GFLT"			},
								{TMrbDGFData::kHistoEnergies,		"Histogram energies"					},
								{TMrbDGFData::kHistoBaselines,		"Histogram baselines"					},
								{TMrbDGFData::kCorrBallDeficit, 	"Correct for ballistic deficit" 		},
								{TMrbDGFData::kComputeCFT,			"Compute CFT"							},
								{TMrbDGFData::kEnaMultiplicity, 	"Enable contribution to multiplicity"	},
								{TMrbDGFData::kBipolarSignals,		"Bipolar signals"						},
								{0, 								NULL									}
							};

extern DGFControlData * gDGFControlData;

ClassImp(DGFEditChanCSRAPanel)

DGFEditChanCSRAPanel::DGFEditChanCSRAPanel(const TGWindow * Window, TGTextEntry * Entry,
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

//	Initialize button list
	fLofCSRABits.SetName("Channel CSRA Bits");
	fLofCSRABits.AddNamedX(kDGFChanCSRABits);
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
	gDGFControlData->SetLH(groupGC, frameGC, csraFrameLayout);
	HEAP(csraFrameLayout);
	TGLayoutHints * csraButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
	buttonGC->SetLH(csraButtonLayout);
	HEAP(csraButtonLayout);

	fCSRAFrame = new TGMrbCheckButtonGroup(this, title.Data(), &fLofCSRABits, 1, groupGC, buttonGC, NULL, kVerticalFrame);
	HEAP(fCSRAFrame);
	this->AddFrame(fCSRAFrame, groupGC->LH());

// get actual entry data
	intStr = fEntry->GetText();
	intStr.ToInteger(intVal);
	fCSRAFrame->SetState(intVal);

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonList(this, NULL, &fLofButtons, 1, 
												DGFEditChanCSRAPanel::kButtonWidth,
												DGFEditChanCSRAPanel::kButtonHeight,
												frameGC, NULL, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	fButtonFrame->Associate(this);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	SetWindowName("DGFControl: Edit Channel CSRA");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

Bool_t DGFEditChanCSRAPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFEditChanCSRAPanel.
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
						case kDGFEditChanCSRAButtonApply:
							btnState = fCSRAFrame->GetActive();
							intStr.FromInteger(btnState, 0, '0', 16);
							fEntry->SetText(intStr);
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
