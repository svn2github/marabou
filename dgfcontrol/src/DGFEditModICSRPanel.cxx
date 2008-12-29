//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditModICSRPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit Module ICSR
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditModICSRPanel.cxx,v 1.10 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFEditModICSRPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TEnv.h"

#include "DGFControlData.h"
#include "DGFEditModICSRPanel.h"

const SMrbNamedX kDGFEditModICSRButtons[] =
		{
			{DGFEditModICSRPanel::kDGFEditModICSRButtonApply,	"Apply",	"Apply settings to module ICSR"	},
			{DGFEditModICSRPanel::kDGFEditModICSRButtonReset,	"Reset",	"Clear module ICSR" 			},
			{DGFEditModICSRPanel::kDGFEditModICSRButtonClose,	"Close",	"Close window"					},
			{0, 												NULL,		NULL							}
		};

const SMrbNamedX kDGFModDSPTriggerBits[] =
		{
			{TMrbDGFData::kSwitchBusNormal, "Normal",		"DSP trigger settings for a module within a group"		},
			{TMrbDGFData::kTerminateDSP,	"Terminate",	"Set DSP trigger termination for last module in a group"},
			{0, 							NULL,			NULL													}
		};

const SMrbNamedX kDGFModFastTriggerBits[] =
		{
			{TMrbDGFData::kSwitchBusNormal, "Normal",		"Fast trigger settings for a module within a group"		},
			{TMrbDGFData::kTerminateFast,	"Terminate",	"Set fast trigger termination for last module in a group"},
			{0, 							NULL,			NULL													}
		};

extern DGFControlData * gDGFControlData;

ClassImp(DGFEditModICSRPanel)

DGFEditModICSRPanel::DGFEditModICSRPanel(const TGWindow * Window, TGTextEntry * Entry,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel
// Purpose:        Edit module ICSR bits
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

	fEntry = Entry; 			// entry to be editied

	module = gDGFControlData->GetSelectedModule();

//	Initialize button list
	fLofDSPTriggerBits.SetName("Module DSP Trigger Bits");
	fLofFastTriggerBits.SetName("Module Fast Trigger Bits");
	fLofDSPTriggerBits.AddNamedX(kDGFModDSPTriggerBits);
	fLofFastTriggerBits.AddNamedX(kDGFModFastTriggerBits);
	fLofButtons.SetName("Buttons");
	fLofButtons.AddNamedX(kDGFEditModICSRButtons);

// ICSR bits
	TGLayoutHints * csraFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
	gDGFControlData->SetLH(groupGC, frameGC, csraFrameLayout);
	HEAP(csraFrameLayout);
	TGLayoutHints * csraButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
	buttonGC->SetLH(csraButtonLayout);
	HEAP(csraButtonLayout);

	title = "SwitchBus Register for ";
	title += module->GetName();
	title += " in C";
	title += module->GetCrate();
	title += ".N";
	title += module->GetStation();
	fICSRFrame = new TGGroupFrame(this, title.Data(), kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG()); 
	HEAP(fICSRFrame);
	this->AddFrame(fICSRFrame, groupGC->LH());

	fDSPTriggerFrame = new TGMrbRadioButtonGroup(this, "DSP trigger", &fLofDSPTriggerBits,
																	-1, 1, groupGC, buttonGC, kVerticalFrame);
	HEAP(fDSPTriggerFrame);
	fICSRFrame->AddFrame(fDSPTriggerFrame, groupGC->LH());

	fFastTriggerFrame = new TGMrbRadioButtonGroup(this, "Fast trigger", &fLofFastTriggerBits,
																	-1, 1, groupGC, buttonGC, kVerticalFrame);
	HEAP(fFastTriggerFrame);
	fICSRFrame->AddFrame(fFastTriggerFrame, groupGC->LH());

// get actual entry data
	intStr = fEntry->GetText();
	intStr.ToInteger(intVal);
	fDSPTriggerFrame->SetState(intVal & TMrbDGFData::kConnectDSPTrigger);
	fFastTriggerFrame->SetState(intVal & TMrbDGFData::kConnectFastTrigger);

// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonList(this, NULL, &fLofButtons, -1, 1, 
												DGFEditModICSRPanel::kButtonWidth,
												DGFEditModICSRPanel::kButtonHeight,
												frameGC, NULL, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	((TGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	SetWindowName("DGFControl: Edit SwitchBus Register");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

void DGFEditModICSRPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t icsr, dspState, fastState;
	TMrbString intStr;

	switch (Selection) {
		case kDGFEditModICSRButtonApply:
			dspState = fDSPTriggerFrame->GetActive();
			fastState = fFastTriggerFrame->GetActive();
			icsr = dspState | fastState;
			icsr &= TMrbDGFData::kSwitchBus;
			intStr.FromInteger(icsr, 0, 16, kTRUE);
			fEntry->SetText(intStr);
			this->CloseWindow();
			break;
		case kDGFEditModICSRButtonReset:
			fDSPTriggerFrame->SetState((UInt_t) -1, kButtonUp);
			fFastTriggerFrame->SetState((UInt_t) -1, kButtonUp);
			break;
		case kDGFEditModICSRButtonClose:
			this->CloseWindow();
			break;
	}
}
