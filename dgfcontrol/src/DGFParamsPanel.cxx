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

ClassImp(DGFParamsPanel)

DGFParamsPanel::DGFParamsPanel(TGCompositeFrame * TabFrame)
														: TGCompositeFrame(TabFrame, kTabWidth, kTabHeight, kVerticalFrame) {
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
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, 1, 
							kTabWidth, kLEHeight,
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

	fSelectParam = new TGMrbLabelCombo(fSelectFrame,  "Param", &fLofParams,
													kDGFParamsSelectParam, 2,
													kTabWidth, kLEHeight,
													kEntryWidth,
													frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectParam);
	fSelectFrame->AddFrame(fSelectParam, frameGC->LH());
	fSelectParam->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectParam->Associate(this); 	// get informed if param selection changes
	fActiveParam = pNameAddr->FindByName("MODNUM")->GetIndex();
		
	Char_t c = 'A';
	Int_t bit = 1;
	for (Int_t i = 0; i < ((Int_t) 'Z' - (Int_t) 'A' + 1); i++) {
		TString * x = new TString(c);
		fLofInitials.AddNamedX(bit, x->Data());
		c++;
		bit <<= 1;
	}
	fAlpha = new TGMrbRadioButtonList(fSelectFrame, NULL, &fLofInitials, 13, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, comboGC);
	HEAP(fAlpha);
	fAlpha->SetState(1);
	fAlpha->Associate(this);	// get informed if channel number changes
	fSelectFrame->AddFrame(fAlpha, frameGC->LH());

// values
	fValueFrame = new TGGroupFrame(this, "Values", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fValueFrame);
	this->AddFrame(fValueFrame, groupGC->LH());
	
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fClusterVals[cl] = new TGHorizontalFrame(fValueFrame, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
		HEAP(fClusterVals[cl]);
		fValueFrame->AddFrame(fClusterVals[cl], groupGC->LH());
		for (Int_t modNo = 0; modNo < kNofModulesPerCluster; modNo++) {
			DGFModule * dgfModule = gDGFControlData->GetModule(cl, modNo);
			Int_t n = modNo + cl * kNofModulesPerCluster;
			TMrbString dgfName;
			TGMrbLayout * bgc;
			if (dgfModule == NULL) {
				dgfName = "clu";
				dgfName += cl + 1;
				dgfName += "-void";
				dgfName += modNo + 1;
				bgc = NULL;
			} else {
				dgfName = dgfModule->GetName();
				bgc = buttonGC;
			}
			fParVal[n] = new TGMrbLabelEntry(fClusterVals[cl], dgfName,
																200, n,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, bgc);
			HEAP(fParVal[n]);
			fClusterVals[cl]->AddFrame(fParVal[n], frameGC->LH());
			if (dgfModule == NULL) {
				fParVal[n]->GetEntry()->SetEnabled(kFALSE);
			} else {
				fParVal[n]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
				fParVal[n]->GetEntry()->SetText("0");
				fParVal[n]->SetRange(0, 100000);
				fParVal[n]->SetIncrement(1);
				fParVal[n]->Associate(this);
			}
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

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(kTabWidth, kTabHeight);
	MapWindow();
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
								this->ApplyParams(kFALSE);
								break;
							case kDGFParamsApplyMarked:
								this->ApplyParams(kTRUE);
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
					
				case kCM_RADIOBUTTON:
					{
						fAlpha->SetState(Param1);
						TMrbNamedX * nx = fLofInitials.FindByIndex(Param1);
						TString x = nx->GetName();
						Char_t c = x(0);
						nx = (TMrbNamedX *) fLofParams.First();
						while (nx) {
							x = nx->GetName();
							if (x(0) == c) {
								fSelectParam->GetComboBox()->Select(nx->GetIndex());
								break;
							}
							nx = (TMrbNamedX *) fLofParams.After(nx);
						}
					}
					break;

				case kCM_COMBOBOX:
					fActiveParam = Param2;
					this->ReadParams();
					break;
				default:	break;
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
						TMrbDGF * dgf = dgfModule->GetAddr();
						if (Marked && foundMarked) {
							dgf->SetParValue(px->GetName(), markedVal, kTRUE);
						} else {
							intStr = fParVal[n]->GetEntry()->GetText();
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
		}
	}
	if (!selectOk) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
	this->ReadParams();
	return(kTRUE);
}	
