//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFParamsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFParamsPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFParamsPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLofDGFs.h"

#include "TGMsgBox.h"

#include "TMrbLogger.h"

#include "DGFControlData.h"
#include "DGFParamsPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kDGFParamsActions[] =
			{
				{DGFParamsPanel::kDGFParamsRead,		"Read",			"Read selected param"			},
				{DGFParamsPanel::kDGFParamsApply,		"Apply",		"Apply param settings"			},
				{DGFParamsPanel::kDGFParamsApplyMarked,	"Apply Marked",	"Apply marked value to all"		},
				{0, 									NULL,			NULL							}
			};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

//////////////////////////////////////fuer Profiles///////////////////////////////////////////
extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;
///////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(DGFParamsPanel)

DGFParamsPanel::DGFParamsPanel(TGCompositeFrame * TabFrame) :
					TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Untrig Trace Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	stdProfile = gMrbLofProfiles->FindProfile("green", kTRUE);
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////
/*
	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;
*/

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();
	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);
/*
	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);
	
	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(labelGC);
	
	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);
*/
	
	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFParamsSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFParamsSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFParamsSelectNone, "cbutton_none.xpm");
			
//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFParamsActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules

	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules, dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile,1, 0, 
							kTabWidth, kLEHeight,0, lofSpecialButtons,kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}
	
	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, dgfFrameLayout);
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1,stdProfile,1, 0, 
							kTabWidth, kLEHeight);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], dgfFrameLayout);
		((TNGMrbButtonFrame *) fGroupSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
	}
	fAllSelect = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1,stdProfile,1, 0, 
							kTabWidth, kLEHeight);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	 dgfFrameLayout->GetPadLeft(),
																			 dgfFrameLayout->GetPadRight(),
																			 dgfFrameLayout->GetPadTop(),
																			 dgfFrameLayout->GetPadBottom()));
	((TNGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
			
// param selection
	fSelectFrame = new TGGroupFrame(this, "Select", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, dgfFrameLayout);

	fDGFData = new TMrbDGFData();
	fDGFData->ReadNameTable();
	TMrbLofNamedX * pNameAddr = fDGFData->GetLofParamNames();
	TObjArray pNames;
	TMrbNamedX * px = (TMrbNamedX *) pNameAddr->First();
	while (px) {
		pNames.Add(new TObjString(px->GetName()));
		px = (TMrbNamedX *) pNameAddr->After(px);
	}
	pNames.Sort();
	TObjString * ps = (TObjString *) pNames.First();
	while (ps) {
		fLofParams.AddNamedX(pNameAddr->FindByName(ps->GetString().Data())->GetIndex(), ps->GetString().Data());
		ps = (TObjString *) pNames.After(ps);
	}	

	fSelectParam = new TNGMrbLabelCombo(fSelectFrame,  "Param", &fLofParams, stdProfile,
													kDGFParamsSelectParam, 2,
													kTabWidth, kLEHeight,
													kComboWidth, TNGMrbLabelCombo::kGMrbComboHasUpDownButtons|TNGMrbLabelCombo::kGMrbComboHasBeginEndButtons);
	HEAP(fSelectParam);
	fSelectFrame->AddFrame(fSelectParam, dgfFrameLayout);
//	fSelectParam->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
// 	fSelectParam->GetComboBox()->GetListBox()->SetTopEntry(gDGFControlData->GetSelectedModuleIndex());
	fSelectParam->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "SelectParam(Int_t, Int_t)");
	fActiveParam = pNameAddr->FindByName("MODNUM")->GetIndex();
//  	Select(fActiveParam) erfolgt weiter unten nach initialisierung von fParVal...


// 	fSelectParam->SetEntryBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCLBEntry)->BG());
		
	Char_t c = 'A';
	Int_t bit = 1;
	for (Int_t i = 0; i < ((Int_t) 'Z' - (Int_t) 'A' + 1); i++) {
		TString * x = new TString(c);
		fLofInitials.AddNamedX(bit, x->Data());
		c++;
		bit <<= 1;
	}
	fAlpha = new TNGMrbRadioButtonList(fSelectFrame, NULL, &fLofInitials,
													0,stdProfile,0, 13, 
													kTabWidth, kLEHeight);
	HEAP(fAlpha);
	fAlpha->SetState(1);
	((TNGMrbButtonFrame *) fAlpha)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fSelectFrame->AddFrame(fAlpha, dgfFrameLayout);

// values

	fValueFrame = new TGGroupFrame(this, "Values", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fValueFrame);
	this->AddFrame(fValueFrame, dgfFrameLayout);
	
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fClusterVals[cl] = new TGHorizontalFrame(fValueFrame, kTabWidth, kTabHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
		HEAP(fClusterVals[cl]);
		fValueFrame->AddFrame(fClusterVals[cl], dgfFrameLayout);
		for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
			DGFModule * dgfModule = gDGFControlData->GetModule(cl, modNo);
			Int_t n = modNo + cl * kNofModulesPerCluster;
			TMrbString dgfName;
//			TGMrbLayout * bgc;
			if (dgfModule == NULL) {
				dgfName = "clu";
				dgfName += cl + 1;
				dgfName += "-void";
				dgfName += modNo + 1;
//				bgc = NULL;
			} else {
				dgfName = dgfModule->GetName();
//				bgc = buttonGC;
			}
			fParVal[n] = new TNGMrbLabelEntry(fClusterVals[cl], dgfName,
																n,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons);
			HEAP(fParVal[n]);
			fClusterVals[cl]->AddFrame(fParVal[n], dgfFrameLayout);
			if (dgfModule == NULL) {
				fParVal[n]->GetNumberEntry()->SetState(kFALSE);
			} else {
//				fParVal[n]->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
				fParVal[n]->SetFormat(TGNumberEntry::kNESInteger);
				fParVal[n]->SetNumber(0);
//				fParVal[n]->SetRange(0, 100000);
				fParVal[n]->SetLimits(TGNumberEntry::kNELLimitMinMax,0,100000);
				fParVal[n]->SetIncrement(1);
			}
		}
	}
	
	fSelectParam->GetComboBox()->Select(fActiveParam);
	fSelectParam->GetComboBox()->GetListBox()->SetTopEntry(fActiveParam);
	

// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
//	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);
	fActionFrame = new TNGMrbTextButtonGroup(this, "Actions", &fActions, -1,stdProfile,1, 0);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, aFrameLayout);
	fActionFrame->JustifyButton(kTextCenterX);
	((TNGMrbButtonFrame *) fActionFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gDGFControlData->fColorGreen);


	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFParamsPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFParamsSelectColumn) {
		switch (Selection) {
			case kDGFParamsSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFParamsSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;							
		}
	} else {
		Selection -= kDGFParamsSelectColumn;
		Bool_t select = ((Selection & 1) == 0);
		UInt_t bit = 0x1 << (Selection >> 1);
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			if (gDGFControlData->GetPatInUse(cl) & bit) {
				UInt_t act = fCluster[cl]->GetActive();
				UInt_t down = select ? (act | bit) : (act & ~bit);
				fCluster[cl]->SetState( 0xFFFF, kButtonUp);
				fCluster[cl]->SetState(down & 0xFFFF, kButtonDown);
			}
		}
	}
}
					
void DGFParamsPanel::SelectParam(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::SelectParam
// Purpose:        Slot method: select param
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fActiveParam = Selection;
	this->ReadParams();
}

void DGFParamsPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFParamsRead:
			this->ReadParams();
			break;
		case kDGFParamsApply:
			this->ApplyParams(kFALSE);
			break;
		case kDGFParamsApplyMarked:
			this->ApplyParams(kTRUE);
			break;
	}
}

void DGFParamsPanel::RadioButtonPressed(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::RadioButtonPressed
// Purpose:        Signal catcher for radio buttons
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Will be called on radio button events.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofInitials.FindByIndex(Selection);
	if (nx) {
		TString x = nx->GetName();
		Char_t c = x(0);
		nx = (TMrbNamedX *) fLofParams.First();
		Bool_t found = kFALSE;
		while (nx) {
			x = nx->GetName();
			if (x(0) == c) {
				fSelectParam->GetComboBox()->Select(nx->GetIndex());
				fSelectParam->GetComboBox()->GetListBox()->SetTopEntry(nx->GetIndex());
				found = kTRUE;
				break;
			}
			nx = (TMrbNamedX *) fLofParams.After(nx);
		}
		if (!found) {
			gMrbLog->Err() << "No param found - " << c << "..." << endl;
			gMrbLog->Flush(this->ClassName(), "RadioButtonPressed");
		}
	}
}

Bool_t DGFParamsPanel::ReadParams() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::ReadParams
// Purpose:        Read module params
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;
	Bool_t selectOk;

	if (gDGFControlData->IsOffline()) return(kTRUE);

	TMrbLofNamedX * pNameAddr = fDGFData->GetLofParamNames();
	TMrbNamedX * px = pNameAddr->FindByIndex(fActiveParam);
	selectOk = kFALSE;
	if (px) {
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
				DGFModule * dgfModule = gDGFControlData->GetModule(cl, modNo);
				Int_t n = modNo + cl * kNofModulesPerCluster;
				if (dgfModule && dgfModule->IsActive()) {
					UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
					if ((fCluster[cl]->GetActive() & bits) == bits) {
						selectOk = kTRUE;
						TMrbDGF * dgf = dgfModule->GetAddr();
						Int_t parVal = dgf->GetParValue(px->GetName(), kTRUE);
						TString pName = px->GetName();
						intStr = "";

						if (pName.Index("CSRA", 0) >= 0 || pName.Index("PATTERN", 0) >= 0) {
							  fParVal[n]->SetHexNumber(parVal);
							//intStr.FromInteger(parVal, 4, 16, kTRUE);
						} else {
							//intStr = parVal;
//  							  cout<<"@@@ n: "<<n<<endl;
//  							  cout<<"@@@ fParVal[n]: "<<fParVal[n]<<endl;
// 							  cout<<"@@@ parVal: "<<parVal<<endl;
							  fParVal[n]->SetNumber(parVal);
						}
						//fParVal[n]->SetText(intStr.Data());
					} else {
						//fParVal[n]->SetText(" ");
						//if (pName.Index("CSRA", 0) >= 0 ||pName.Index("PATTERN", 0) >= 0)   fParVal[n]->SetHexNumber(0);
						//else fParVal[n]->SetNumber(0);
						fParVal[n]->SetNumber(0);
					}
				}
			}
		}
	}
	if (!selectOk) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
	return(kTRUE);
}	

Bool_t DGFParamsPanel::ApplyParams(Bool_t Marked) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::ApplyParams
// Purpose:        Write params to DGF modules
// Arguments:      Bool_t Marked   -- kTRUE, if marked value should be taken
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;
	Int_t parVal;
	Int_t idx;
	Bool_t selectOk;
				
	if (gDGFControlData->IsOffline()) return(kTRUE);

	TMrbLofNamedX * pNameAddr = fDGFData->GetLofParamNames();
	TMrbNamedX * px = pNameAddr->FindByIndex(fActiveParam);
	selectOk = kFALSE;
	Bool_t foundMarked = kFALSE;
	Int_t markedVal;
	if (px) {
// 	  Wenn ich das richtig sehe, kann man keine mehr auswaehlen
		if (Marked) {
			for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
				for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
					DGFModule * dgfModule = gDGFControlData->GetModule(cl, modNo);
					Int_t n = modNo + cl * kNofModulesPerCluster;
					if (dgfModule && dgfModule->IsActive()) {
						UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
						if ((fCluster[cl]->GetActive() & bits) == bits) {
							TGTextEntry * e = fParVal[n]->GetEntry();
							if (e->HasMarkedText()) {
								if (foundMarked) {
									gMrbLog->Err()	<< "More than one entry MARKED" << endl;
									gMrbLog->Flush(this->ClassName(), "ApplyParams");
									new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "More than one entry MARKED", kMBIconStop);
									return(kFALSE);
								}
								foundMarked = kTRUE;
								intStr = e->GetText();
								intStr = intStr.Strip();
								if (intStr.Length() > 0) {
									idx = intStr.Index("0x", 0);
									if (idx >= 0) {
										intStr = intStr(idx + 2, intStr.Length());
										intStr.ToInteger(markedVal, 16);
									} else {
										intStr.ToInteger(markedVal);
									}
								}
							}
						}
					}
				}
			}
			if (!foundMarked) {
				gMrbLog->Err()	<< "No entry MARKED" << endl;
				gMrbLog->Flush(this->ClassName(), "ApplyParams");
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No entry MARKED", kMBIconStop);
				return(kFALSE);
			}
		}
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
				DGFModule * dgfModule = gDGFControlData->GetModule(cl, modNo);
				Int_t n = modNo + cl * kNofModulesPerCluster;
				if (dgfModule && dgfModule->IsActive()) {
					UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
					if ((fCluster[cl]->GetActive() & bits) == bits) {
						selectOk = kTRUE;
						gDGFControlData->AddToUpdateList(dgfModule);
						TMrbDGF * dgf = dgfModule->GetAddr();
						if (Marked && foundMarked) {
							dgf->SetParValue(px->GetName(), markedVal, kTRUE);
						} else {
// 							intStr = fParVal[n]->GetText();
// 							TString pName = px->GetName();
// 							intStr = intStr.Strip();
							if (intStr.Length() > 0) {
								/*idx = intStr.Index("0x", 0);
								if (idx >= 0) {				
									intStr = intStr(idx + 2, intStr.Length());
									intStr.ToInteger(parVal, 16);
								} else {
									intStr.ToInteger(parVal);
								}*/
// 								dgf->SetParValue(px->GetName(), parVal, kTRUE);
							}
							parVal=fParVal[n]->GetNumber();
							dgf->SetParValue(px->GetName(), parVal, kTRUE);
						}
					}
				}
			}
		}
	}
	if (!selectOk) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
	gDGFControlData->UpdateParamsAndFPGAs();
	this->ReadParams();
	return(kTRUE);
}	
