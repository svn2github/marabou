//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFCptmPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Instrument Panel
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFCptmPanel.cxx,v 1.13 2008-10-14 10:22:29 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFCptmPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "TMrbCPTM.h"

#include "DGFControlData.h"
#include "DGFCptmPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFCptmButtons[] =
			{
				{DGFCptmPanel::kDGFCptmButtonDownloadCode,		"Download", 	"Download code from .rbf file"	},
				{DGFCptmPanel::kDGFCptmButtonReset,				"Reset", 		"Reset module"					},
				{DGFCptmPanel::kDGFCptmButtonSave,				"Save", 		"Save settings to file"			},
				{DGFCptmPanel::kDGFCptmButtonRestore,			"Restore", 		"Restore settings from file"	},
				{DGFCptmPanel::kDGFCptmButtonSynchEnable,		"Synch", 		"Enable synch"					},
				{DGFCptmPanel::kDGFCptmButtonSynchEnableReset,	"Synch+Reset", 	"Enable synch and reset clock"	},
				{DGFCptmPanel::kDGFCptmButtonShowEvent,			"Show Event", 	"Show next event"				},
				{DGFCptmPanel::kDGFCptmButtonShowBuffer,		"Show Buffer", 	"Show buffer contents"			},
				{DGFCptmPanel::kDGFCptmButtonResetAddr, 		"Reset Pointers", 	"Reset read & write pointers"			},
				{DGFCptmPanel::kDGFCptmButtonUpdateAddr,		"Update", 		"Update addr pointers"			},
				{0, 											NULL,		NULL								}
			};

const SMrbNamedX kDGFCptmMaskRegisterBits[] =
			{
				{DGFCptmPanel::kDGFCptmMaskE2,		"E2", 	"E2"		},
				{DGFCptmPanel::kDGFCptmMaskT2,		"T2", 	"T2"		},
				{DGFCptmPanel::kDGFCptmMaskT1,		"T1", 	"T1"		},
				{DGFCptmPanel::kDGFCptmMaskQ4,		"Q4", 	"Aux Q4"	},
				{DGFCptmPanel::kDGFCptmMaskQ3,		"Q3", 	"Aux Q3"	},
				{DGFCptmPanel::kDGFCptmMaskQ2,		"Q2", 	"Aux Q2"	},
				{DGFCptmPanel::kDGFCptmMaskQ1,		"Q1", 	"Aux Q1"	},
				{DGFCptmPanel::kDGFCptmMaskGe,		"Ge", 	"Ge Mult"	},
				{0, 								NULL,	NULL		}
			};

static Char_t * kDGFFileTypesSettings[]	=	{
												"All saves",			"*",
												NULL,					NULL
											};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

ClassImp(DGFCptmPanel)

DGFCptmPanel::DGFCptmPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel
// Purpose:        DGF Viewer: Instrument Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Instrument Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);

//	Initialize lists
	fCptmActions.SetName("cptm module actions");
	fCptmActions.AddNamedX(kDGFCptmButtons);

	fLofCptmModules.SetName("list of cptm modules");
	fLofCptmModules.Delete();

	this->ChangeBackground(gDGFControlData->fColorGreen);

	TGLayoutHints * cptmFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, cptmFrameLayout);
	HEAP(cptmFrameLayout);

//	module selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "CPTM Selection", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Int_t n = this->GetLofCptmModules();
	if (n > 0) {
		fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
											&fLofCptmModules,
											DGFCptmPanel::kDGFCptmSelectModule, 2,
											kTabWidth, kLEHeight,
											kComboWidth,
											frameGC, labelGC, comboGC, buttonGC, kTRUE);
		HEAP(fSelectModule);
		fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
		fCptmIndex = 0;
		fSelectModule->GetComboBox()->Select(((TMrbNamedX *) fLofCptmModules[fCptmIndex])->GetIndex());
		fSelectModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");

		TGLayoutHints * layout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
		frameGC->SetLH(layout);
		HEAP(layout);
		fH1Frame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
		HEAP(fH1Frame);
		this->AddFrame(fH1Frame, frameGC->LH());

		gDGFControlData->SetLH(groupGC, frameGC, layout);
		fGeFrame = new TGGroupFrame(fH1Frame, "DGG (Ge)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
		HEAP(fGeFrame);
		fH1Frame->AddFrame(fGeFrame, frameGC->LH());

		fGeDelayEntry = new TGMrbLabelEntry(fGeFrame, "Delay [us]",
												200, kDGFCptmGeDelayEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fGeDelayEntry);
		fGeFrame->AddFrame(fGeDelayEntry, frameGC->LH());
		fGeDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fGeDelayEntry->SetText("0");
		fGeDelayEntry->SetRange(0., 255 * 0.025);
		fGeDelayEntry->SetIncrement(0.1);
		fGeDelayEntry->AddToFocusList(&fFocusList);
		fGeDelayEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fGeWidthEntry = new TGMrbLabelEntry(fGeFrame, "Width [us]",
												200, kDGFCptmGeWidthEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fGeWidthEntry);
		fGeFrame->AddFrame(fGeWidthEntry, frameGC->LH());
		fGeWidthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fGeWidthEntry->SetText("0");
		fGeWidthEntry->SetRange(0., 255 * 0.025);
		fGeWidthEntry->SetIncrement(0.1);
		fGeWidthEntry->AddToFocusList(&fFocusList);
		fGeWidthEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fAuxFrame = new TGGroupFrame(fH1Frame, "DGG (Aux)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
		HEAP(fGeFrame);
		fH1Frame->AddFrame(fAuxFrame, frameGC->LH());

		fAuxDelayEntry = new TGMrbLabelEntry(fAuxFrame, "Delay [us]",
												200, kDGFCptmAuxDelayEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fAuxDelayEntry);
		fAuxFrame->AddFrame(fAuxDelayEntry, frameGC->LH());
		fAuxDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fAuxDelayEntry->SetText("0");
		fAuxDelayEntry->SetRange(0., 255 * 0.025);
		fAuxDelayEntry->SetIncrement(0.1);
		fAuxDelayEntry->AddToFocusList(&fFocusList);
		fAuxDelayEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fAuxWidthEntry = new TGMrbLabelEntry(fAuxFrame, "Width [us]",
												200, kDGFCptmAuxWidthEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fAuxWidthEntry);
		fAuxFrame->AddFrame(fAuxWidthEntry, frameGC->LH());
		fAuxWidthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fAuxWidthEntry->SetText("0");
		fAuxWidthEntry->SetRange(0., 255 * 0.025);
		fAuxWidthEntry->SetIncrement(0.1);
		fAuxWidthEntry->AddToFocusList(&fFocusList);
		fAuxWidthEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fTimeWdwEntry = new TGMrbLabelEntry(fAuxFrame, "Time wdw [us]",
												200, kDGFCptmTimeWdwEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fTimeWdwEntry);
		fAuxFrame->AddFrame(fTimeWdwEntry, frameGC->LH());
		fTimeWdwEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fTimeWdwEntry->SetText("0");
		fTimeWdwEntry->SetRange(0., 255 * 0.025);
		fTimeWdwEntry->SetIncrement(0.1);
		fTimeWdwEntry->AddToFocusList(&fFocusList);
		fTimeWdwEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fMultFrame = new TGGroupFrame(fH1Frame, "Multiplicity", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
		HEAP(fMultFrame);
		fH1Frame->AddFrame(fMultFrame, frameGC->LH());

		fMultValueEntry = new TGMrbLabelEntry(fMultFrame, "Multiplicity",
												200, kDGFCptmMultValueEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fMultValueEntry);
		fMultFrame->AddFrame(fMultValueEntry, frameGC->LH());
		fMultValueEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
		fMultValueEntry->SetText("0");
		fMultValueEntry->SetRange(0, 17);
		fMultValueEntry->SetIncrement(1);
		fMultValueEntry->AddToFocusList(&fFocusList);
		fMultValueEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fMultDacEntry = new TGMrbLabelEntry(fMultFrame, "DAC [mV]",
												200, kDGFCptmMultDacEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
		HEAP(fMultValueEntry);
		fMultFrame->AddFrame(fMultDacEntry, frameGC->LH());
		fMultDacEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
		fMultDacEntry->SetText("0");
		fMultDacEntry->SetRange(0, TMrbCPTM::kMrbCptmMultOffset + TMrbCPTM::kMrbCptmMaxMult * TMrbCPTM::kMrbCptmMilliVoltsPerMult);
		fMultDacEntry->SetIncrement(200);
		fMultDacEntry->AddToFocusList(&fFocusList);
		fMultDacEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fH2Frame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
		HEAP(fH2Frame);
		this->AddFrame(fH2Frame, frameGC->LH());

		TMrbLofNamedX maskRegBits;
		maskRegBits.SetName("Mask Register");
		maskRegBits.AddNamedX(kDGFCptmMaskRegisterBits);
		maskRegBits.SetPatternMode();

		fCptmMaskReg = new TGMrbCheckButtonGroup(fH2Frame, "Mask Register",	&maskRegBits, kDGFCptmMaskRegisterBtns, 1,
																groupGC, buttonGC,
																NULL,
																kHorizontalFrame);
		HEAP(fCptmMaskReg);
		fH2Frame->AddFrame(fCptmMaskReg, groupGC->LH());
		fCptmMaskReg->SetState(0xffffffff, kButtonUp);
	
		fAddrFrame = new TGGroupFrame(fH2Frame, "Addr Pointers", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
		HEAP(fAddrFrame);
		fH2Frame->AddFrame(fAddrFrame, frameGC->LH());

		fAddrReadEntry = new TGMrbLabelEntry(fAddrFrame, "Read",
												200, kDGFCptmAddrReadEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC);
		HEAP(fAddrReadEntry);
		fAddrFrame->AddFrame(fAddrReadEntry, frameGC->LH());
		fAddrReadEntry->GetEntry()->SetState(kFALSE);

		fAddrWriteEntry = new TGMrbLabelEntry(fAddrFrame, "Write",
												200, kDGFCptmAddrWriteEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC);
		HEAP(fAddrWriteEntry);
		fAddrFrame->AddFrame(fAddrWriteEntry, frameGC->LH());
		fAddrWriteEntry->GetEntry()->SetState(kFALSE);

//	buttons
		fCptmButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fCptmActions, -1, 1, groupGC, buttonGC);
		HEAP(fCptmButtonFrame);
		this->AddFrame(fCptmButtonFrame, buttonGC->LH());
		((TGMrbButtonFrame *) fCptmButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

		this->InitializeValues(fCptmIndex);

	}

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFCptmPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFCptmButtonDownloadCode:
			this->DownloadCode(fCptmIndex);
			break;
		case kDGFCptmButtonReset:
			this->Reset(fCptmIndex);
			break;
		case kDGFCptmButtonSave:
			this->SaveSettings(fCptmIndex);
			break;
		case kDGFCptmButtonRestore:
			this->RestoreSettings(fCptmIndex);
			break;
		case kDGFCptmButtonSynchEnable:
			this->EnableSynch(fCptmIndex, kFALSE);
			break;
		case kDGFCptmButtonSynchEnableReset:
			this->EnableSynch(fCptmIndex, kTRUE);
			break;
		case kDGFCptmButtonShowBuffer:
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			this->ShowBuffer(fCptmIndex, "cptm.dat");
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			break;
		case kDGFCptmButtonShowEvent:
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			this->ShowNextEvent(fCptmIndex);
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			break;
		case kDGFCptmButtonResetAddr:
			this->ResetAddrPointers(fCptmIndex);
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			break;
		case kDGFCptmButtonUpdateAddr:
			this->UpdateValue(kDGFCptmAddrReadEntry, fCptmIndex);
			this->UpdateValue(kDGFCptmAddrWriteEntry, fCptmIndex);
			break;
	}
}

void DGFCptmPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::SelectModule
// Purpose:        Slot method: select module
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fCptmIndex = Selection - 1;
	this->InitializeValues(fCptmIndex);
}

void DGFCptmPanel::EntryChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::EntryChanged
// Purpose:        Slot method: update after entry changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->UpdateValue(Selection, fCptmIndex);
}

Int_t DGFCptmPanel::GetLofCptmModules() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::GetLofCptmModules
// Purpose:        Determine cptm modules available
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    Reads file .DGFControl.rc and searches for CPTM modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofCptmModules.Delete();
	Int_t nofCptmModules = 0;

	Int_t nofSevts = gDGFControlData->Dgfrc()->Get(".NofSubevents", 0);
	for (Int_t i = 0; i < nofSevts; i++) {
		TString sevtName = gDGFControlData->Dgfrc()->Get(".Subevent", Form("%d", i), "Name", "");
		if (sevtName.Length() > 0) {
			TString sevtNuna = Form("%d:%s", i, sevtName.Data());
			TString sevtClass = gDGFControlData->Dgfrc()->Get(".Subevent", sevtNuna.Data(), "ClassName", "");
			if (sevtClass.Index("TMrbSubevent_CPTM", 0) == 0) {
				Int_t nofModules = gDGFControlData->Dgfrc()->Get(".Subevent", sevtNuna.Data(), "NofModules", 0);
				if (nofModules > 0) {
					TString sevtLofModules = gDGFControlData->Dgfrc()->Get(".Subevent", sevtNuna.Data(), "LofModules", "");
					TString cptmName;
					Int_t from = 0;
					while (sevtLofModules.Tokenize(cptmName, from, ":")) {
						TString cptmNuna = Form("%d:%s", i, cptmName.Data());
						Int_t crate = gDGFControlData->Dgfrc()->Get(".Module", cptmNuna.Data(), "Crate", 0);
						Int_t station = gDGFControlData->Dgfrc()->Get(".Module", cptmNuna.Data(), "Station", 0);
						TMrbCPTM * cptm = new TMrbCPTM(cptmName.Data(), gDGFControlData->fCAMACHost.Data(), crate, station);
						if (!cptm->IsZombie())	{
							nofCptmModules++;
							cptmName += " (C";
							cptmName += crate;
							cptmName += ".N";
							cptmName += station;
							cptmName += ")";
							cptm->SetTitle(cptmName.Data());
							fLofCptmModules.AddNamedX(nofCptmModules, cptmName, "", cptm);
						}
					}
				}
			}
		}
	}
	return(nofCptmModules);
}

void DGFCptmPanel::InitializeValues(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::InitializeValues
// Purpose:        Initialize values
// Arguments:      Int_t ModuleIndex -- module index
// Results:        
// Exceptions:     
// Description:    Reads values from hardware.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (ModuleIndex < 0) {
		for (Int_t i = 0; i < fLofCptmModules.GetEntriesFast(); i++) this->InitializeValues(i);
		return;
	}

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	fGeDelayEntry->SetText((Double_t) cptm->GetGeDelay() * .025);

	fGeWidthEntry->SetText((Double_t) cptm->GetGeWidth() * .025);

	fAuxDelayEntry->SetText((Double_t) cptm->GetAuxDelay() * .025);

	fAuxWidthEntry->SetText((Double_t) cptm->GetAuxWidth() * .025);

	fTimeWdwEntry->SetText((Double_t) cptm->GetTimeWindowAux() * .025);

	fMultValueEntry->SetText(cptm->GetMultiplicity());
	fMultDacEntry->SetText(cptm->GetDac(1));

	fCptmMaskReg->SetState(0xFF, kButtonUp);
	fCptmMaskReg->SetState(cptm->GetMask(), kButtonDown);

	fAddrReadEntry->GetEntry()->SetState(kTRUE);
	fAddrReadEntry->SetText(cptm->GetReadAddr());
	fAddrReadEntry->GetEntry()->SetState(kFALSE);

	fAddrWriteEntry->GetEntry()->SetState(kTRUE);
	fAddrWriteEntry->SetText(cptm->GetWriteAddr());
	fAddrWriteEntry->GetEntry()->SetState(kFALSE);
}

void DGFCptmPanel::UpdateValue(Int_t EntryId, Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::UpdateValue
// Purpose:        Update values
// Arguments:      Int_t EntryId     -- entry id
//                 Int_t ModuleIndex -- module index
// Results:        
// Exceptions:     
// Description:    Updates given value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;
	Double_t dblVal;

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	switch (EntryId) {
		case kDGFCptmGeDelayEntry:
			dblVal = fGeDelayEntry->GetText2Double();
			cptm->SetGeDelay((Int_t) ((dblVal + .024) / .025));
			fGeDelayEntry->SetText((Double_t) cptm->GetGeDelay() * .025);
			break;

		case kDGFCptmGeWidthEntry:
			dblVal = fGeWidthEntry->GetText2Double();
			cptm->SetGeWidth((Int_t) ((dblVal + .024) / .025));
			fGeWidthEntry->SetText((Double_t) cptm->GetGeWidth() * .025);
			break;

		case kDGFCptmAuxDelayEntry:
			dblVal = fAuxDelayEntry->GetText2Double();
			cptm->SetAuxDelay((Int_t) ((dblVal + .024) / .025));
			fAuxDelayEntry->SetText((Double_t) cptm->GetAuxDelay() * .025);
			break;

		case kDGFCptmAuxWidthEntry:
			dblVal = fAuxWidthEntry->GetText2Double();
			cptm->SetAuxWidth((Int_t) ((dblVal + .024) / .025));
			fAuxWidthEntry->SetText((Double_t) cptm->GetAuxWidth() * .025);
			break;

		case kDGFCptmTimeWdwEntry:
			dblVal = fTimeWdwEntry->GetText2Double();
			cptm->SetTimeWindowAux((Int_t) ((dblVal + .024) / .025));
			fTimeWdwEntry->SetText((Double_t) cptm->GetTimeWindowAux() * .025);
			break;

		case kDGFCptmMultValueEntry:
			intVal = fMultValueEntry->GetText2Int();
			cptm->SetMultiplicity(intVal);
			fMultDacEntry->SetText(cptm->GetDac(1));
			break;

		case kDGFCptmMultDacEntry:
			intVal = fMultDacEntry->GetText2Int();
			cptm->SetDac(1, intVal);
			fMultValueEntry->SetText(cptm->GetMultiplicity());
			break;

		case kDGFCptmMaskRegisterBtns:
			intVal = fCptmMaskReg->GetActive();
			cptm->SetMask(intVal & 0xFF);
			break;

		case kDGFCptmAddrReadEntry:
			fAddrReadEntry->GetEntry()->SetState(kTRUE);
			fAddrReadEntry->SetText(cptm->GetReadAddr());
			fAddrReadEntry->GetEntry()->SetState(kFALSE);
			break;

		case kDGFCptmAddrWriteEntry:
			fAddrWriteEntry->GetEntry()->SetState(kTRUE);
			fAddrWriteEntry->SetText(cptm->GetWriteAddr());
			fAddrWriteEntry->GetEntry()->SetState(kFALSE);
			break;

	}
}

void DGFCptmPanel::Reset(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::UpdateValue
// Purpose:        Update values
// Arguments:      Int_t ModuleIndex -- module index
// Results:        --
// Exceptions:     
// Description:    Performs reset.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
	cptm->Reset();
	gSystem->Sleep(200);
	this->InitializeValues(ModuleIndex);
}

Bool_t DGFCptmPanel::SaveSettings(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::SaveSettings
// Purpose:        Save settings to file
// Arguments:      Int_t ModuleIndex -- module index
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Saves cptm settings.
//                 Saves all modules if index == -1
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGFileInfo fileInfoSave;
	TMrbSystem uxSys;
	TString errMsg;

	Bool_t verbose = gDGFControlData->IsVerbose();

	fileInfoSave.fFileTypes = (const Char_t **) kDGFFileTypesSettings;
	if (!gDGFControlData->CheckAccess(gDGFControlData->fCptmSettingsPath.Data(),
						(Int_t) DGFControlData::kDGFAccessDirectory | DGFControlData::kDGFAccessWrite, errMsg)) {
		fileInfoSave.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoSave.fIniDir = StrDup(gDGFControlData->fCptmSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fileInfoSave);
	if (fileInfoSave.fFilename == NULL || *fileInfoSave.fFilename == '\0') return(kFALSE);
	TString saveDir = fileInfoSave.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	uxSys.GetBaseName(baseName1, saveDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, saveDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) saveDir = dirName;	// double click: strip off last part
	
	if (!uxSys.Exists(saveDir.Data())) {
		TString cmd = "mkdir -p ";
		cmd += saveDir;
		gSystem->Exec(cmd);
		if (verbose) {
			gMrbLog->Out() << "Creating directory - " << saveDir << endl;
			gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
		}
	}
	
	if (!uxSys.IsDirectory(saveDir.Data())) {
		errMsg = "File \"";
		errMsg += saveDir.Data();
		errMsg += "\" is NOT a directory";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", errMsg.Data(), kMBIconStop);
		return(kFALSE);
	}

	Int_t nerr = 0;
	Int_t nofModules = 0;
	for (Int_t modNo = 0; modNo < fLofCptmModules.GetEntriesFast(); modNo++) {
		if (ModuleIndex >= 0 && ModuleIndex != modNo) continue;
		TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[modNo];
		TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
		TString saveFile = saveDir;
		saveFile += "/";
		saveFile += cptm->GetName();
		saveFile += ".par";
		if (!cptm->SaveSettings(saveFile.Data())) nerr++;
		nofModules++;
	}

	if (nerr > 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Saving CPTM settings failed", kMBIconStop);
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " CPTM module(s) saved to directory " << saveDir << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
	}

	return(kTRUE);
}
		
Bool_t DGFCptmPanel::RestoreSettings(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::RestoreSettings
// Purpose:        Restore settings from file
// Arguments:      Int_t ModuleIndex -- module index
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores cptm settings.
//                 Restores all modules if index == -1
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGFileInfo fileInfoRestore;

	TMrbSystem uxSys;
	TString errMsg;

	fileInfoRestore.fFileTypes = (const Char_t **) kDGFFileTypesSettings;
	if (!gDGFControlData->CheckAccess(gDGFControlData->fCptmSettingsPath.Data(),
						(Int_t) DGFControlData::kDGFAccessDirectory | DGFControlData::kDGFAccessRead, errMsg)) {
		fileInfoRestore.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoRestore.fIniDir = StrDup(gDGFControlData->fCptmSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fileInfoRestore);
	if (fileInfoRestore.fFilename == NULL || *fileInfoRestore.fFilename == '\0') return(kFALSE);
	TString loadDir = fileInfoRestore.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	uxSys.GetBaseName(baseName1, loadDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, loadDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) loadDir = dirName;	// double click: strip off last part
	
	if (!uxSys.IsDirectory(loadDir.Data())) {
		errMsg = "No such directory - ";
		errMsg += loadDir;
		gMrbLog->Err() << errMsg << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreSettings");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", errMsg.Data(), kMBIconStop);
		return(kFALSE);
	}
	
	Int_t nerr = 0;
	Int_t nofModules = 0;
	for (Int_t modNo = 0; modNo < fLofCptmModules.GetEntriesFast(); modNo++) {
		if (ModuleIndex >= 0 && ModuleIndex != modNo) continue;
		TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[modNo];
		TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
		TString loadFile = loadDir;
		loadFile += "/";
		loadFile += cptm->GetName();
		loadFile += ".par";
		if (!cptm->RestoreSettings(loadFile.Data())) nerr++;
		nofModules++;
	}

	if (nerr > 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Restoring CPTM settings failed", kMBIconStop);
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " CPTM module(s) restored from directory " << loadDir << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreSettings", setblue);
	}

	gSystem->Sleep(200);
	this->InitializeValues(ModuleIndex);
	return(kTRUE);
}

void DGFCptmPanel::EnableSynch(Int_t ModuleIndex, Bool_t ResetClock) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::EnableSynch
// Purpose:        Enable synch (and reset clock)
// Arguments:      Int_t ModuleIndex -- module index
//                 Bool_t ResetClock -- reset clock if kTRUE
// Results:        --
// Exceptions:     
// Description:    Enables busy-synch loop
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t nofModules = 0;
	for (Int_t modNo = 0; modNo < fLofCptmModules.GetEntriesFast(); modNo++) {
		if (ModuleIndex >= 0 && ModuleIndex != modNo) continue;
		TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[modNo];
		TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
		cptm->EnableSynch(ResetClock);
		nofModules++;
	}
	gMrbLog->Out()	<< nofModules << " CPTM module(s): " << "Busy-synch loop enabled" << endl;
	gMrbLog->Flush(this->ClassName(), "EnableSynch", setblue);
}

Bool_t DGFCptmPanel::DownloadCode(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::DownloadCode
// Purpose:        Download altera code
// Arguments:      Int_t ModuleIndex -- module index
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Loads code from file to altera chip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t nofModules = 0;
	Int_t nerr = 0;
	TString codeFile = gDGFControlData->fCptmCodeFile;
	for (Int_t modNo = 0; modNo < fLofCptmModules.GetEntriesFast(); modNo++) {
		if (ModuleIndex >= 0 && ModuleIndex != modNo) continue;
		TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[modNo];
		TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
		cout << "[Downloading code to " << nx->GetName() << " - wait]" << endl;
		if (!cptm->DownloadAlteraCode(codeFile)) nerr++;
		nofModules++;
	}
	if (nofModules > 0) cout << "[done]" << endl;

	if (nerr > 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Downloading Altera code failed", kMBIconStop);
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " CPTM module(s): Downloaded code from file " << codeFile << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadCode", setblue);
	}

	gSystem->Sleep(200);
	this->InitializeValues(ModuleIndex);
	return(kTRUE);
}

void DGFCptmPanel::ShowNextEvent(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::ShowNextEvent
// Purpose:        Show next 10 words
// Arguments:      Int_t ModuleIndex -- module index
// Results:        --
// Exceptions:     
// Description:    Reads next event (10 words) from memory
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	Int_t rAddr = cptm->GetReadAddr();

	cout << "CPTM module " << cptm->GetTitle() << endl;
	cout << "Read addr         : " << rAddr << endl;
	for (Int_t i = 0; i < 10; i++, rAddr++) {
		Int_t data = cptm->ReadNext();
		cout << Form("%6d: %8d %0x\n", rAddr, data, data);
	}
}

void DGFCptmPanel::ShowBuffer(Int_t ModuleIndex, const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::ShowBuffer
// Purpose:        Show buffer data
// Arguments:      Int_t ModuleIndex -- module index
//                 Char_t * FileName -- file where to write the output
// Results:        --
// Exceptions:     
// Description:    Reads ram subsequently
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	cptm->ResetRead();
	Int_t rAddr = cptm->GetReadAddr();
	Int_t wAddr = cptm->GetWriteAddr();

	if (FileName != NULL && *FileName != '\0') {
		TString ofile = FileName;
		ofstream f(ofile.Data(), ios::out);
		f << "CPTM module " << cptm->GetTitle() << endl;
		f << "Read addr         : " << rAddr << endl;
		f << "Write addr        : " << wAddr << endl;
		for (Int_t i = 0; i < wAddr; i++, rAddr++) {
			Int_t data = cptm->ReadNext();
			f << Form("%6d: %8d %0x\n", rAddr, data, data);
		}
		f.close();
		gMrbLog->Out()	<< "[" << cptm->GetName() << "]: SRAM written to file " << ofile
						<< " (" << wAddr << " words)" << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadCode", setblue);
	} else {
		cout << "CPTM module " << cptm->GetTitle() << endl;
		cout << "Read addr         : " << rAddr << endl;
		cout << "Write addr        : " << wAddr << endl;
		for (Int_t i = 0; i < wAddr; i++, rAddr++) {
			Int_t data = cptm->ReadNext();
			cout << Form("%6d: %8d %0x\n", rAddr, data, data);
		}
	}
}

void DGFCptmPanel::ResetAddrPointers(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::ResetAddrPointers
// Purpose:        Reset read & write pointers
// Arguments:      Int_t ModuleIndex -- module index
// Results:        --
// Exceptions:     
// Description:    Resets pointers & clears memory
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();
	cptm->ResetRead();
	cptm->ResetWrite();
	cptm->ResetMemory();
}

void DGFCptmPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:     
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry = NULL;

	switch (EntryId) {
		case kDGFCptmGeDelayEntry:
			entry = fGeDelayEntry->GetEntry();
			break;
		case kDGFCptmGeWidthEntry:
			entry = fGeWidthEntry->GetEntry();
			break;
		case kDGFCptmAuxDelayEntry:
			entry = fAuxDelayEntry->GetEntry();
			break;
		case kDGFCptmAuxWidthEntry:
			entry = fAuxWidthEntry->GetEntry();
			break;
		case kDGFCptmTimeWdwEntry:
			entry = fTimeWdwEntry->GetEntry();
			break;
		case kDGFCptmMultValueEntry:
			entry = fMultValueEntry->GetEntry();
			break;
		case kDGFCptmMultDacEntry:
			entry = fMultDacEntry->GetEntry();
			break;
	}
	if (entry) fFocusList.FocusForward(entry);
	return;
}
