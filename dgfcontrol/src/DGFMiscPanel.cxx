//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFMiscPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFMiscPanel.cxx,v 1.7 2005-04-28 10:27:14 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFMiscPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TGMrbProgressBar.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFMiscPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kDGFMiscActions[] =
			{
				{DGFMiscPanel::kDGFMiscSetGFLT,		"Set GFLT",	"Set Global First Level Trigger"	},
				{DGFMiscPanel::kDGFMiscClearGFLT,	"Clear GFLT",	"Clear Global First Level Trigger"	},
				{DGFMiscPanel::kDGFMiscSetCoincWait,	"Set CoincWait",	"Set COINCWAIT according to formula"	},
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

extern TMrbLogger * gMrbLog;

static TMrbLofDGFs lofDgfs;

ClassImp(DGFMiscPanel)

DGFMiscPanel::DGFMiscPanel(TGCompositeFrame * TabFrame) :
						TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Untrig Trace Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);
	
	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(labelGC);
	
	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);
	
	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFMiscSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFMiscSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFMiscSelectNone, "cbutton_none.xpm");
	
	
//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFMiscActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}
	
	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);
			
// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);

	fHFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight,
													kChildFrame, frameGC->BG());
	HEAP(fHFrame);
	this->AddFrame(fHFrame, frameGC->LH());

	fActionFrame = new TGGroupFrame(fHFrame, "Actions", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fActionFrame);
	fHFrame->AddFrame(fActionFrame, groupGC->LH());
	
	fActionButtons = new TGMrbTextButtonList(fActionFrame, NULL, &fActions, -1, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	fActionButtons->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFMiscPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFMiscPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFMiscSelectColumn) {
						switch (Param1) {
							case kDGFMiscSetGFLT:
								this->SetGFLT(kTRUE);
								break;
							case kDGFMiscClearGFLT:
								this->SetGFLT(kFALSE);
								break;
							case kDGFMiscSetCoincWait:
								this->SetCoincWait();
								break;
							case kDGFMiscSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFMiscSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFMiscSelectColumn;
						Bool_t select = ((Param1 & 1) == 0);
						UInt_t bit = 0x1 << (Param1 >> 1);
						for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
							if (gDGFControlData->GetPatInUse(cl) & bit) {
								if (select) fCluster[cl]->SetState(bit, kButtonDown);
								else		fCluster[cl]->SetState(bit, kButtonUp);
							}
						}
					}
					break;
				default:	break;
			}
			break;

	}
	return(kTRUE);
}

Bool_t DGFMiscPanel::SetGFLT(Bool_t SetFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel::SetGFLT
// Purpose:        Set/Clear GFLT
// Arguments:      Bool_t SetFlag   -- kTRUE if GFLT is to be set
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	Int_t modNo, cl;
	TMrbDGF * dgf;
	Bool_t selectFlag;
	Int_t nofModules;
					
	Bool_t offlineMode = gDGFControlData->IsOffline();

	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	selectFlag = kFALSE;
	TString setOrClear = SetFlag ? "Setting GFLT bit ..." : "Clearing GFLT bit ...";
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, setOrClear, 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				if (SetFlag) {
					dgf->SetChanCSRA(0, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitOr, kTRUE);
					dgf->SetChanCSRA(1, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitOr, kTRUE);
					dgf->SetChanCSRA(2, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitOr, kTRUE);
					dgf->SetChanCSRA(3, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitOr, kTRUE);
				} else {
					dgf->SetChanCSRA(0, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitClear, kTRUE);
					dgf->SetChanCSRA(1, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitClear, kTRUE);
					dgf->SetChanCSRA(2, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitClear, kTRUE);
					dgf->SetChanCSRA(3, TMrbDGFData::kGFLTValidate, TMrbDGF::kBitClear, kTRUE);
				}
				selectFlag = kTRUE;
			}
		}
		pgb->Increment(1, dgfModule->GetName());
		gSystem->ProcessEvents();
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				
	delete pgb;

	if (!selectFlag) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	} else {
		setOrClear = SetFlag ? "SET" : "CLEARED";
		gMrbLog->Out()	<< "GFLT bit" << setOrClear << endl;
		gMrbLog->Flush(this->ClassName(), "SetGFLT", setblue);
	}

	return(kTRUE);
}

Bool_t DGFMiscPanel::SetCoincWait() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel::SetCoincWait
// Purpose:        Set COINCWAIT
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	Int_t modNo, cl;
	TMrbDGF * dgf;
	Bool_t selectFlag;
	Int_t nofModules;
					
	Bool_t offlineMode = gDGFControlData->IsOffline();

	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	selectFlag = kFALSE;
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Setting COINCWAIT ...", 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				dgf->SetCoincWait();
				selectFlag = kTRUE;
			}
		}
		pgb->Increment(1, dgfModule->GetName());
		gSystem->ProcessEvents();
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				
	delete pgb;

	if (!selectFlag) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	} else {
		gMrbLog->Out()	<< "COINCWAIT set properly" << endl;
		gMrbLog->Flush(this->ClassName(), "SetCoincWait", setblue);
	}

	return(kTRUE);
}
