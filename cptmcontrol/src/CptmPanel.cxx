//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            CptmPanel
// Purpose:        A GUI to control a C_PTM module
// Description:    Control Panel
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: CptmPanel.cxx,v 1.5 2005-09-07 06:55:24 marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/CptmPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TApplication.h"
#include "TObjString.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "TMrbCPTM.h"

#include "CptmControlData.h"
#include "CptmPanel.h"

#include "SetColor.h"

const SMrbNamedX kCptmButtons[] =
			{
				{CptmPanel::kCptmButtonDownloadCode,		"Download", 	"Download code from .rbf file"	},
				{CptmPanel::kCptmButtonReset,				"Reset", 		"Reset module"					},
				{CptmPanel::kCptmButtonSave,				"Save", 		"Save settings to file"			},
				{CptmPanel::kCptmButtonRestore,			"Restore", 		"Restore settings from file"	},
				{CptmPanel::kCptmButtonSynchEnable,		"Synch", 		"Enable synch"					},
				{CptmPanel::kCptmButtonSynchEnableReset,	"Synch+Reset", 	"Enable synch and reset clock"	},
				{CptmPanel::kCptmButtonShowBuffer,		"Show", 		"Show buffer contents"			},
				{0, 											NULL,		NULL								}
			};

const SMrbNamedX kCptmMaskRegisterBits[] =
			{
				{CptmPanel::kCptmMaskE2,		"E2", 	"E2"		},
				{CptmPanel::kCptmMaskT2,		"T2", 	"T2"		},
				{CptmPanel::kCptmMaskT1,		"T1", 	"T1"		},
				{CptmPanel::kCptmMaskQ4,		"Q4", 	"Aux Q4"	},
				{CptmPanel::kCptmMaskQ3,		"Q3", 	"Aux Q3"	},
				{CptmPanel::kCptmMaskQ2,		"Q2", 	"Aux Q2"	},
				{CptmPanel::kCptmMaskQ1,		"Q1", 	"Aux Q1"	},
				{CptmPanel::kCptmMaskGe,		"Ge", 	"Ge Mult"	},
				{0, 								NULL,	NULL		}
			};

static Char_t * kCptmFileTypesSettings[]	=	{
												"All saves",			"*",
												NULL,					NULL
											};

extern CptmControlData * gCptmControlData;
extern TMrbLogger * gMrbLog;

ClassImp(CptmPanel)

CptmPanel::CptmPanel(const TGWindow * Window, UInt_t Width, UInt_t Height) : TGMainFrame(Window, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel
// Purpose:        CPTM Viewer: Control Panel
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
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

	frameGC = new TGMrbLayout(	gCptmControlData->NormalFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorGold);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gCptmControlData->SlantedFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorGold);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gCptmControlData->NormalFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorGold);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gCptmControlData->NormalFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorGold);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gCptmControlData->NormalFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gCptmControlData->NormalFont(), 
								gCptmControlData->fColorBlack,
								gCptmControlData->fColorWhite);	HEAP(entryGC);

//	menu bar
	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);
	fMenuFile->AddEntry("Exit ... Ctrl-q", kCptmFileExit);
	fMenuFile->Associate(this);
	fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame | kSunkenFrame);
	HEAP(fMenuBar);
	TGLayoutHints * menuBarFileLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarFileLayout);
	fMenuBar->AddPopup("&File", fMenuFile, menuBarFileLayout);
	TGLayoutHints * menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 3);
	HEAP(menuBarLayout);
	this->AddFrame(fMenuBar, menuBarLayout);
	fMenuBar->ChangeBackground(gCptmControlData->fColorDarkBlue);

//	Initialize lists
	fCptmActions.SetName("cptm module actions");
	fCptmActions.AddNamedX(kCptmButtons);

	fLofCptmModules.SetName("list of cptm modules");
	fLofCptmModules.Delete();

	this->ChangeBackground(gCptmControlData->fColorGold);

	TGLayoutHints * cptmFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	gCptmControlData->SetLH(groupGC, frameGC, cptmFrameLayout);
	HEAP(cptmFrameLayout);

//	module selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gCptmControlData->SetLH(groupGC, frameGC, selectLayout);
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

	Int_t n  = this->GetLofCptmModules();
	if (n <= 0) {
		gMrbLog->Err() << "No CPTM module found. Sorry" << endl;
		gMrbLog->Flush(this->ClassName());
		gSystem->Exit(1);
	}

	fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
											&fLofCptmModules,
											CptmPanel::kCptmSelectModule, 2,
											kTabWidth, kLEHeight,
											kEntryWidth,
											frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fCptmIndex = 0;
	fSelectModule->GetComboBox()->Select(fCptmIndex);
	fSelectModule->Associate(this);

	TGLayoutHints * layout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	frameGC->SetLH(layout);
	HEAP(layout);
	fH1Frame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fH1Frame);
	this->AddFrame(fH1Frame, frameGC->LH());

	gCptmControlData->SetLH(groupGC, frameGC, layout);
	fGeFrame = new TGGroupFrame(fH1Frame, "DGG (Ge)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fGeFrame);
	fH1Frame->AddFrame(fGeFrame, frameGC->LH());

	fGeDelayEntry = new TGMrbLabelEntry(fGeFrame, "Delay [us]",
												200, kCptmGeDelayEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fGeDelayEntry);
	fGeFrame->AddFrame(fGeDelayEntry, frameGC->LH());
	fGeDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fGeDelayEntry->SetText(0);
	fGeDelayEntry->SetRange(0., 255 * 0.025);
	fGeDelayEntry->SetIncrement(0.1);
	fGeDelayEntry->AddToFocusList(&fFocusList);
	fGeDelayEntry->ShowToolTip(kTRUE, kTRUE);
	fGeDelayEntry->Associate(this);

	fGeWidthEntry = new TGMrbLabelEntry(fGeFrame, "Width [us]",
												200, kCptmGeWidthEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fGeWidthEntry);
	fGeFrame->AddFrame(fGeWidthEntry, frameGC->LH());
	fGeWidthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fGeWidthEntry->SetText(0);
	fGeWidthEntry->SetRange(0., 255 * 0.025);
	fGeWidthEntry->SetIncrement(0.1);
	fGeWidthEntry->AddToFocusList(&fFocusList);
	fGeWidthEntry->ShowToolTip(kTRUE, kTRUE);
	fGeWidthEntry->Associate(this);

	fAuxFrame = new TGGroupFrame(fH1Frame, "DGG (Aux)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fGeFrame);
	fH1Frame->AddFrame(fAuxFrame, frameGC->LH());

	fAuxDelayEntry = new TGMrbLabelEntry(fAuxFrame, "Delay [us]",
												200, kCptmAuxDelayEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fAuxDelayEntry);
	fAuxFrame->AddFrame(fAuxDelayEntry, frameGC->LH());
	fAuxDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fAuxDelayEntry->SetText(0);
	fAuxDelayEntry->SetRange(0., 255 * 0.025);
	fAuxDelayEntry->SetIncrement(0.1);
	fAuxDelayEntry->AddToFocusList(&fFocusList);
	fAuxDelayEntry->ShowToolTip(kTRUE, kTRUE);
	fAuxDelayEntry->Associate(this);

	fAuxWidthEntry = new TGMrbLabelEntry(fAuxFrame, "Width [us]",
												200, kCptmAuxWidthEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fAuxWidthEntry);
	fAuxFrame->AddFrame(fAuxWidthEntry, frameGC->LH());
	fAuxWidthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fAuxWidthEntry->SetText(0);
	fAuxWidthEntry->SetRange(0., 255 * 0.025);
	fAuxWidthEntry->SetIncrement(0.1);
	fAuxWidthEntry->AddToFocusList(&fFocusList);
	fAuxWidthEntry->ShowToolTip(kTRUE, kTRUE);
	fAuxWidthEntry->Associate(this);

	fTimeWdwEntry = new TGMrbLabelEntry(fAuxFrame, "Time wdw [us]",
												200, kCptmTimeWdwEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTimeWdwEntry);
	fAuxFrame->AddFrame(fTimeWdwEntry, frameGC->LH());
	fTimeWdwEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTimeWdwEntry->SetText(0);
	fTimeWdwEntry->SetRange(0., 255 * 0.025);
	fTimeWdwEntry->SetIncrement(0.1);
	fTimeWdwEntry->AddToFocusList(&fFocusList);
	fTimeWdwEntry->ShowToolTip(kTRUE, kTRUE);
	fTimeWdwEntry->Associate(this);

	fMultFrame = new TGGroupFrame(fH1Frame, "Multiplicity", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fMultFrame);
	fH1Frame->AddFrame(fMultFrame, frameGC->LH());

	fMultValueEntry = new TGMrbLabelEntry(fMultFrame, "Multiplicity",
												200, kCptmMultValueEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fMultValueEntry);
	fMultFrame->AddFrame(fMultValueEntry, frameGC->LH());
	fMultValueEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMultValueEntry->SetText(0);
	fMultValueEntry->SetRange(0, (4095 + 34) / 35);
	fMultValueEntry->SetIncrement(1);
	fMultValueEntry->AddToFocusList(&fFocusList);
	fMultValueEntry->ShowToolTip(kTRUE, kTRUE);
	fMultValueEntry->Associate(this);

	fMultDacEntry = new TGMrbLabelEntry(fMultFrame, "DAC [mV]",
												200, kCptmMultDacEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fMultValueEntry);
	fMultFrame->AddFrame(fMultDacEntry, frameGC->LH());
	fMultDacEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMultDacEntry->SetText(0);
	fMultDacEntry->SetRange(0, 4095);
	fMultDacEntry->SetIncrement(35);
	fMultDacEntry->AddToFocusList(&fFocusList);
	fMultDacEntry->ShowToolTip(kTRUE, kTRUE);
	fMultDacEntry->Associate(this);

	fH2Frame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fH2Frame);
	this->AddFrame(fH2Frame, frameGC->LH());

	TMrbLofNamedX maskRegBits;
	maskRegBits.SetName("Mask Register");
	maskRegBits.AddNamedX(kCptmMaskRegisterBits);
	maskRegBits.SetPatternMode();

	fCptmMaskReg = new TGMrbCheckButtonGroup(fH2Frame, "Mask Register",	&maskRegBits, kCptmMaskRegisterBtns, 1,
																groupGC, buttonGC,
																NULL,
																kHorizontalFrame);
	HEAP(fCptmMaskReg);
	fH2Frame->AddFrame(fCptmMaskReg, groupGC->LH());
	fCptmMaskReg->SetState(0xffffffff, kButtonUp);
	fCptmMaskReg->Associate(this);
	
	fAddrFrame = new TGGroupFrame(fH2Frame, "Addr Pointers", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fAddrFrame);
	fH2Frame->AddFrame(fAddrFrame, frameGC->LH());

	fAddrReadEntry = new TGMrbLabelEntry(fAddrFrame, "Read",
												200, kCptmAddrReadEntry,
												kLEWidth,
												kLEHeight,
												kEntryWidth,
												frameGC, labelGC, entryGC);
	HEAP(fAddrReadEntry);
	fAddrFrame->AddFrame(fAddrReadEntry, frameGC->LH());
	fAddrReadEntry->GetEntry()->SetState(kFALSE);

	fAddrWriteEntry = new TGMrbLabelEntry(fAddrFrame, "Write",
												200, kCptmAddrWriteEntry,
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
	fCptmButtonFrame->Associate(this);

	this->InitializeValues(fCptmIndex);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, Height);
	MapWindow();
	SetWindowName("CptmControl - keep control over C_PTM modules");
}

void CptmPanel::CloseWindow() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::CloseWindow
// Purpose:        Close window and terminate application
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Got close message for this MainFrame.
//                 Calls parent's CloseWindow()
//                 (which destroys the window) and terminates the application.
//                 The close message is generated by the window manager when
//                 its close window menu item is selected.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   TGMainFrame::CloseWindow();
   gApplication->Terminate(0);
}

Bool_t CptmPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::ProcessMessage
// Purpose:        Message handler for the instrument panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to CptmPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_MENU:
					switch (Param1) {
						case kCptmFileExit:
							this->CloseWindow();
							break;
					}
					break;
				case kCM_BUTTON:
					switch (Param1) {
						case kCptmButtonDownloadCode:
							this->DownloadCode(fCptmIndex);
							break;
						case kCptmButtonReset:
							this->Reset(fCptmIndex);
							break;
						case kCptmButtonSave:
							this->SaveSettings(fCptmIndex);
							break;
						case kCptmButtonRestore:
							this->RestoreSettings(fCptmIndex);
							break;
						case kCptmButtonSynchEnable:
							this->EnableSynch(fCptmIndex, kFALSE);
							break;
						case kCptmButtonSynchEnableReset:
							this->EnableSynch(fCptmIndex, kTRUE);
							break;
						case kCptmButtonShowBuffer:
							this->ShowBuffer(fCptmIndex);
							break;
					}
					break;
				case kCM_CHECKBUTTON:
					this->UpdateValue(Param1, fCptmIndex);
					break;
				case kCM_RADIOBUTTON:
					this->UpdateValue(Param1, fCptmIndex);
					break;
				case kCM_COMBOBOX:
					fCptmIndex = Param2;
					this->InitializeValues(fCptmIndex);
					break;
			}
			break;

		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_ENTER:
					this->UpdateValue(Param1, fCptmIndex);
					break;
				case kTE_TAB:
					this->UpdateValue(Param1, fCptmIndex);
					this->MoveFocus(Param1);
					break;
			}
			break;
			
	}
	return(kTRUE);
}

Int_t CptmPanel::GetLofCptmModules() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::GetLofCptmModules
// Purpose:        Determine cptm modules available
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    Reads file .DGFControl.rc and searches for CPTM modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TObjArray lofModules;
	Int_t crate, station;

	fLofCptmModules.Delete();
	TMrbString cptmRes;

	cptmRes = gEnv->GetValue("DGFControl.LofCptmModules", "");
	lofModules.Delete();
	Int_t n = cptmRes.Split(lofModules);
	Int_t nofCptmModules = 0;
	for (Int_t modNo = 0; modNo < n; modNo++) {
		TString cptmName = ((TObjString *) lofModules.At(modNo))->GetString();
		gCptmControlData->GetResource(crate, "DGFControl.Module", modNo + 1, cptmName.Data(), "Crate");
		gCptmControlData->GetResource(station, "DGFControl.Module", modNo + 1, cptmName.Data(), "Station");
		TMrbCPTM * cptm = new TMrbCPTM(cptmName.Data(), gCptmControlData->fCAMACHost.Data(), crate, station);
		if (!cptm->IsZombie())	{
			nofCptmModules++;
			cptmName += " (C";
			cptmName += crate;
			cptmName += ".N";
			cptmName += station;
			cptmName += ")";
			cptm->SetTitle(cptmName.Data());
			fLofCptmModules.AddNamedX(modNo, cptmName, "", cptm);
		}
	}
	return(nofCptmModules);
}

void CptmPanel::InitializeValues(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::InitializeValues
// Purpose:        Initialize values
// Arguments:      Int_t ModuleIndex -- module index
// Results:        
// Exceptions:     
// Description:    Reads values from hardware.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString dblStr, intStr;

	if (ModuleIndex < 0) {
		for (Int_t i = 0; i < fLofCptmModules.GetEntriesFast(); i++) this->InitializeValues(i);
		return;
	}

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	Int_t intVal = cptm->GetGeDelay();
	Double_t dblVal = (Double_t) intVal * .025;
	fGeDelayEntry->SetText(dblVal);

	intVal = cptm->GetGeWidth();
	dblVal = (Double_t) intVal * .025;
	fGeWidthEntry->SetText(dblVal);

	intVal = cptm->GetAuxDelay();
	dblVal = (Double_t) intVal * .025;
	fAuxDelayEntry->SetText(dblVal);

	intVal = cptm->GetAuxWidth();
	dblVal = (Double_t) intVal * .025;
	fAuxWidthEntry->SetText(dblVal);

	intVal = cptm->GetTimeWindowAux();
	dblVal = (Double_t) intVal * .025;
	fTimeWdwEntry->SetText(dblVal);

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

void CptmPanel::UpdateValue(Int_t EntryId, Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::UpdateValue
// Purpose:        Update values
// Arguments:      Int_t EntryId     -- entry id
//                 Int_t ModuleIndex -- module index
// Results:        
// Exceptions:     
// Description:    Updates given value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString dblStr, intStr;
	Int_t intVal;
	Double_t dblVal;

	TMrbNamedX * nx = (TMrbNamedX *) fLofCptmModules[ModuleIndex];
	TMrbCPTM * cptm = (TMrbCPTM *) nx->GetAssignedObject();

	switch (EntryId) {
		case kCptmGeDelayEntry:
			dblVal = fGeDelayEntry->GetText2Double();
			cptm->SetGeDelay((Int_t) ((dblVal + .024) / .025));
			dblVal = (Double_t) (cptm->GetGeDelay() * .025);
			fGeDelayEntry->SetText(dblVal);
			break;

		case kCptmGeWidthEntry:
			dblVal = fGeWidthEntry->GetText2Double();
			cptm->SetGeWidth((Int_t) ((dblVal + .024) / .025));
			dblVal = (Double_t) (cptm->GetGeWidth() * .025);
			fGeWidthEntry->SetText(dblVal);
			break;

		case kCptmAuxDelayEntry:
			dblVal = fAuxDelayEntry->GetText2Double();
			cptm->SetAuxDelay((Int_t) ((dblVal + .024) / .025));
			dblVal = (Double_t) (cptm->GetAuxDelay() * .025);
			fAuxDelayEntry->SetText(dblVal);
			break;

		case kCptmAuxWidthEntry:
			dblVal = fAuxWidthEntry->GetText2Double();
			cptm->SetAuxWidth((Int_t) ((dblVal + .024) / .025));
			dblVal = (Double_t) (cptm->GetAuxWidth() * .025);
			fAuxWidthEntry->SetText(dblVal);
			break;

		case kCptmTimeWdwEntry:
			dblVal = fTimeWdwEntry->GetText();
			cptm->SetTimeWindowAux((Int_t) ((dblVal + .024) / .025));
			dblVal = (Double_t) (cptm->GetTimeWindowAux() * .025);
			fTimeWdwEntry->SetText(dblVal);
			break;

		case kCptmMultValueEntry:
			intVal = fMultValueEntry->GetText2Int();
			cptm->SetMultiplicity(intVal);
			fMultDacEntry->SetText(cptm->GetDac(1));
			break;

		case kCptmMultDacEntry:
			intVal = fMultDacEntry->GetText2Int();
			cptm->SetDac(1, intVal);
			fMultValueEntry->SetText(cptm->GetMultiplicity());
			break;

		case kCptmMaskRegisterBtns:
			intVal = fCptmMaskReg->GetActive();
			cptm->SetMask(intVal & 0xFF);
			break;
	}
}

void CptmPanel::Reset(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::UpdateValue
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

Bool_t CptmPanel::SaveSettings(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::SaveSettings
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

	Bool_t verbose = gCptmControlData->IsVerbose();

	fileInfoSave.fFileTypes = (const Char_t **) kCptmFileTypesSettings;
	if (!gCptmControlData->CheckAccess(gCptmControlData->fCptmSettingsPath.Data(),
						(Int_t) CptmControlData::kDGFAccessDirectory | CptmControlData::kDGFAccessWrite, errMsg)) {
		fileInfoSave.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoSave.fIniDir = StrDup(gCptmControlData->fCptmSettingsPath);
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
		
Bool_t CptmPanel::RestoreSettings(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::RestoreSettings
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

	fileInfoRestore.fFileTypes = (const Char_t **) kCptmFileTypesSettings;
	if (!gCptmControlData->CheckAccess(gCptmControlData->fCptmSettingsPath.Data(),
						(Int_t) CptmControlData::kDGFAccessDirectory | CptmControlData::kDGFAccessRead, errMsg)) {
		fileInfoRestore.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoRestore.fIniDir = StrDup(gCptmControlData->fCptmSettingsPath);
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

void CptmPanel::EnableSynch(Int_t ModuleIndex, Bool_t ResetClock) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::EnableSynch
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

Bool_t CptmPanel::DownloadCode(Int_t ModuleIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::DownloadCode
// Purpose:        Download altera code
// Arguments:      Int_t ModuleIndex -- module index
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Loads code from file to altera chip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t nofModules = 0;
	Int_t nerr = 0;
	TString codeFile = gCptmControlData->fCptmCodeFile;
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

void CptmPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:     
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry = NULL;

	switch (EntryId) {
		case kCptmGeDelayEntry:
			entry = fGeDelayEntry->GetEntry();
			break;
		case kCptmGeWidthEntry:
			entry = fGeWidthEntry->GetEntry();
			break;
		case kCptmAuxDelayEntry:
			entry = fAuxDelayEntry->GetEntry();
			break;
		case kCptmAuxWidthEntry:
			entry = fAuxWidthEntry->GetEntry();
			break;
		case kCptmTimeWdwEntry:
			entry = fTimeWdwEntry->GetEntry();
			break;
		case kCptmMultValueEntry:
			entry = fMultValueEntry->GetEntry();
			break;
		case kCptmMultDacEntry:
			entry = fMultDacEntry->GetEntry();
			break;
	}
	if (entry) fFocusList.FocusForward(entry);
	return;
}
