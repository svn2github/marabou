//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFParamsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
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

using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLofDGFs.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFParamsPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kDGFParamsActions[] =
			{
				{DGFParamsPanel::kDGFParamsApply,		"Read",			"Read selected param"			},
				{DGFParamsPanel::kDGFParamsApply,		"Apply",		"Apply param settings"			},
				{DGFParamsPanel::kDGFParamsClose,		"Close",		"Close window"					},
				{0, 									NULL,			NULL							}
			};

extern DGFControlData * gDGFControlData;

ClassImp(DGFParamsPanel)

DGFParamsPanel::DGFParamsPanel(const TGWindow * Window, const TGWindow * MainFrame, UInt_t Width, UInt_t Height, UInt_t Options)
														: TGTransientFrame(Window, MainFrame, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 TGWindow * MainFrame -- main frame
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
//                 UInt_t Options       -- options
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
	TGMrbLayout * comboGC;

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
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

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(comboGC);

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
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), 1, 
							DGFParamsPanel::kFrameWidth, DGFParamsPanel::kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}
	
	fGroupFrame = new TGHorizontalFrame(fModules, DGFParamsPanel::kFrameWidth, DGFParamsPanel::kFrameHeight,
													kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], 1, 
							DGFParamsPanel::kFrameWidth, DGFParamsPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, 1, 
							DGFParamsPanel::kFrameWidth, DGFParamsPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);
			
// param selection
	fSelectFrame = new TGGroupFrame(this, "Select", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	DGFModule * dgfModule = gDGFControlData->FirstModule();
	TMrbLofNamedX * pNameAddr = dgfModule->GetAddr()->Data()->GetLofParamNames();
	TObjArray pNames;
	TMrbNamedX * px = (TMrbNamedX *) pNameAddr->First();
	while (px) {
		pNames.Add(new TObjString(px->GetName()));
		px = (TMrbNamedX *) pNameAddr->After(px);
	}
	pNames.Sort();
	TMrbLofNamedX lofParams;
	TObjString * ps = (TObjString *) pNames.First();
	while (ps) {
		lofParams.AddNamedX(pNameAddr->FindByName(ps->GetString().Data())->GetIndex(), ps->GetString().Data());
		ps = (TObjString *) pNames.After(ps);
	}	

	fSelectParam = new TGMrbLabelCombo(fSelectFrame,  "Param", &lofParams,
													DGFParamsPanel::kDGFParamsSelectParam, 2,
													DGFParamsPanel::kFrameWidth, DGFParamsPanel::kLEHeight,
													DGFParamsPanel::kEntryWidth,
													frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectParam);
	fSelectFrame->AddFrame(fSelectParam, frameGC->LH());
	fSelectParam->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectParam->Associate(this); 	// get informed if param selection changes
	fActiveParam = pNameAddr->FindByName("MODNUM")->GetIndex();
		
// values
	fValueFrame = new TGGroupFrame(this, "Values", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fValueFrame);
	this->AddFrame(fValueFrame, groupGC->LH());
	
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fClusterVals[cl] = new TGHorizontalFrame(fValueFrame, DGFParamsPanel::kFrameWidth, DGFParamsPanel::kFrameHeight,
													kChildFrame, frameGC->BG());
		HEAP(fClusterVals[cl]);
		fValueFrame->AddFrame(fClusterVals[cl], groupGC->LH());
		for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
			dgfModule = gDGFControlData->GetModule(cl, modNo);
			fParVal[modNo] = new TGMrbLabelEntry(fClusterVals[cl], dgfModule->GetName(),
																200, -1,
																DGFParamsPanel::kLEWidth,
																DGFParamsPanel::kLEHeight,
																DGFParamsPanel::kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
			HEAP(fParVal[modNo]);
			fClusterVals[cl]->AddFrame(fParVal[modNo], frameGC->LH());
			fParVal[modNo]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
			fParVal[modNo]->GetEntry()->SetText("0");
			fParVal[modNo]->SetRange(0, 100000);
			fParVal[modNo]->SetIncrement(1);
		}
	}	
	
// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);
	fActionFrame = new TGMrbTextButtonGroup(this, "Actions", &fActions, 1, groupGC, labelGC);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());
	fActionFrame->JustifyButton(kTextCenterX);
	fActionFrame->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorGreen);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(MainFrame->GetId(), this->GetParent()->GetId(),
								(((TGFrame *) MainFrame)->GetWidth() + 10), 0,
								ax, ay, wdum);
	Move(ax, ay);

	SetWindowName("DGFControl: ParamsPanel");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
//	gClient->WaitFor(this);
}

Bool_t DGFParamsPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFParamsPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFParamsSelectColumn) {
						switch (Param1) {
							case kDGFParamsRead:
								this->ReadParams();
								break;
							case kDGFParamsApply:
								this->ApplyParams();
								break;
							case kDGFParamsClose:
								this->CloseWindow();
								break;
							case kDGFParamsSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFParamsSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFParamsSelectColumn;
						Bool_t select = ((Param1 & 1) == 0);
						UInt_t bit = 0x1 << (Param1 >> 1);
						for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
							if (gDGFControlData->GetPatInUse(cl) & bit) {
								if (select) fCluster[cl]->SetState(bit, kButtonDown);
								else		fCluster[cl]->SetState(bit, kButtonUp);
							}						}
					}
					break;
					
				case kCM_COMBOBOX:
					fActiveParam = Param2;
					this->ReadParams();
					break;
				default:	break;
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
		
	TMrbLofNamedX * pNameAddr = gDGFControlData->GetSelectedModule()->GetAddr()->Data()->GetLofParamNames();
	TMrbNamedX * px = pNameAddr->FindByIndex(fActiveParam);
	selectOk = kFALSE;
	if (px) {
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
				Int_t n = modNo + cl * kNofModulesPerCluster;
				UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
				if ((fCluster[cl]->GetActive() & bits) == bits) {
					selectOk = kTRUE;
					TMrbDGF * dgf = gDGFControlData->GetModule(modNo)->GetAddr();
					Int_t parVal = dgf->GetParValue(px->GetName(), kTRUE);
					TString pName = px->GetName();
					intStr = "";
					if (pName.Index("CSRA", 0) >= 0 || pName.Index("PATTERN", 0) >= 0) {
						intStr = "0x";
						intStr.AppendInteger(parVal, 4, '0', 16);
					} else {
						intStr = parVal;
					}
					fParVal[n]->GetEntry()->SetText(intStr.Data());
				} else {
					fParVal[n]->GetEntry()->SetText(" ");
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

Bool_t DGFParamsPanel::ApplyParams() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel::ApplyParams
// Purpose:        Write params to DGF modules
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;
	Int_t parVal;
	Int_t idx;
	Bool_t selectOk;
				
	TMrbLofNamedX * pNameAddr = gDGFControlData->GetSelectedModule()->GetAddr()->Data()->GetLofParamNames();
	TMrbNamedX * px = pNameAddr->FindByIndex(fActiveParam);
	selectOk = kFALSE;
	if (px) {
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
				UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
				if ((fCluster[cl]->GetActive() & bits) == bits) {
					selectOk = kTRUE;
					TMrbDGF * dgf = gDGFControlData->GetModule(cl, modNo)->GetAddr();
					intStr = fParVal[modNo + cl * kNofModulesPerCluster]->GetEntry()->GetText();
					TString pName = px->GetName();
					intStr = intStr.Strip();
					if (intStr.Length() > 0) {
						idx = intStr.Index("0x", 0);
						if (idx >= 0) {
							intStr = intStr(idx + 2, intStr.Length());
							intStr.ToInteger(parVal, 16);
						} else {
							intStr.ToInteger(parVal);
						}
						dgf->SetParValue(px->GetName(), parVal, kTRUE);
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
