//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFEditCoincPatternPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Edit Coincidence Pattern
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFEditCoincPatternPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFEditCoincPatternPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "DGFControlData.h"
#include "DGFEditCoincPatternPanel.h"

const SMrbNamedX kDGFEditCoincPatternButtons[] =
		{
			{DGFEditCoincPatternPanel::kDGFEditCoincPatternButtonApply, 	"Apply",	"Apply selected patterns"	},
			{DGFEditCoincPatternPanel::kDGFEditCoincPatternButtonSetAll,	"Set ALL",	"Enable ALL coinc patterns"	},
			{DGFEditCoincPatternPanel::kDGFEditCoincPatternButtonReset, 	"Reset",	"Clear patterns"			},
			{DGFEditCoincPatternPanel::kDGFEditCoincPatternButtonClose, 	"Close",	"Close window"				},
			{0, 															NULL,		NULL						}
		};

// coinc patterns
const SMrbNamedXShort kDGFCoincPatternsLeft[] =
							{
								{BIT(0),		"0 0 0 0"		},
								{BIT(1),		"0 0 0 1"		},
								{BIT(2),		"0 0 1 0"		},
								{BIT(3),		"0 0 1 1"		},
								{BIT(4),		"0 1 0 0"		},
								{BIT(5),		"0 1 0 1"		},
								{BIT(6),		"0 1 1 0"		},
								{BIT(7),		"0 1 1 1"		},
								{0, 			NULL			}
							};
const SMrbNamedXShort kDGFCoincPatternsRight[] =
							{
								{BIT(8),		"1 0 0 0"		},
								{BIT(9),		"1 0 0 1"		},
								{BIT(10),		"1 0 1 0"		},
								{BIT(11),		"1 0 1 1"		},
								{BIT(12),		"1 1 0 0"		},
								{BIT(13),		"1 1 0 1"		},
								{BIT(14),		"1 1 1 0"		},
								{BIT(15),		"1 1 1 1"		},
								{0, 			NULL			}
							};

extern DGFControlData * gDGFControlData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFEditCoincPatternPanel)

DGFEditCoincPatternPanel::DGFEditCoincPatternPanel(const TGWindow * Window, TNGMrbLabelEntry * Entry,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditCoincPatternPanel
// Purpose:        Edit coinc patterns
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

	TMrbString title;
	TMrbString intStr;

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

//	Initialize button list
	fLofCoincPatternsLeft.SetName("Coinc Patterns 0x0 - 0x7");
	fLofCoincPatternsLeft.AddNamedX(kDGFCoincPatternsLeft);
	fLofCoincPatternsRight.SetName("Coinc Patterns 0x8 - 0xF");
	fLofCoincPatternsRight.AddNamedX(kDGFCoincPatternsRight);
	fLofButtons.SetName("Buttons");
	fLofButtons.AddNamedX(kDGFEditCoincPatternButtons);

	fEntry = Entry; 			// entry to be editied

	module = gDGFControlData->GetSelectedModule();

	title = "Coinc patterns for  ";
	title += module->GetName();
	title += " in C";
	title += module->GetCrate();
	title += ".N";
	title += module->GetStation();

// pattern frame
	TGLayoutHints * patFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
//	gDGFControlData->SetLH(groupGC, frameGC, patFrameLayout);
	HEAP(patFrameLayout);
	TGLayoutHints * patButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 5);
//	buttonGC->SetLH(patButtonLayout);
	HEAP(patButtonLayout);

	fPatternFrame = new TGGroupFrame(this, title.Data(), kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fPatternFrame);
	this->AddFrame(fPatternFrame, patFrameLayout);

	fLeftFrame = new TNGMrbCheckButtonGroup(this, "-",	&fLofCoincPatternsLeft, -1,stdProfile,0, 1);
	HEAP(fLeftFrame);
	fPatternFrame->AddFrame(fLeftFrame, patFrameLayout);

	fRightFrame = new TNGMrbCheckButtonGroup(this, "-",	&fLofCoincPatternsRight, -1,stdProfile,0, 1);
	HEAP(fRightFrame);
	fPatternFrame->AddFrame(fRightFrame, patFrameLayout);

	TMrbDGF * dgf = module->GetAddr();
	UInt_t coincPatt = gDGFControlData->IsOffline() ? 0 : dgf->GetCoincPattern();
	fLeftFrame->SetState(coincPatt);	// initial state
	fRightFrame->SetState(coincPatt);

	// buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
//	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonList(this, NULL, &fLofButtons, -1,stdProfile,0, 1, 
												DGFEditCoincPatternPanel::kButtonWidth,
												DGFEditCoincPatternPanel::kButtonHeight);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	SetWindowName("DGFControl: Edit Coinc Patterns");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

void DGFEditCoincPatternPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditCoincPatternPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFEditCoincPatternButtonApply:
			if (!gDGFControlData->IsOffline()) {
				TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
				UInt_t patMask = fLeftFrame->GetActive();
				patMask |= fRightFrame->GetActive();
				dgf->SetCoincPattern(patMask);
				UInt_t pattern = dgf->GetCoincPattern();
//				TMrbString intStr;
//				intStr.FromInteger((Int_t) pattern, 0, 16, kTRUE);
//				fEntry->SetText(intStr.Data());
				fEntry->SetHexNumber(pattern);
			} else {
				fEntry->SetHexNumber(0x0000);
			}
			this->CloseWindow();
			break;
		case kDGFEditCoincPatternButtonSetAll:
			fLeftFrame->SetState(0xffffffff, kButtonDown);
			fRightFrame->SetState(0xffffffff, kButtonDown);
			break;
		case kDGFEditCoincPatternButtonReset:
			fLeftFrame->SetState(0xffffffff, kButtonUp);
			fRightFrame->SetState(0xffffffff, kButtonUp);
			break;
		case kDGFEditCoincPatternButtonClose:
			this->CloseWindow();
	}
}
