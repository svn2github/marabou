//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFCopyModuleSettingsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Copy Module Settings
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
<img src=dgfcontrol/DGFCopyModuleSettingsPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "DGFCopyModuleSettingsPanel.h"

const SMrbNamedX kDGFCopyModuleSettingsButtons[] =
		{
			{DGFCopyModuleSettingsPanel::kDGFCopyModuleSettingsButtonCopy,		"Copy to DSP",		"Copy selected items to DSP"	},
			{DGFCopyModuleSettingsPanel::kDGFCopyModuleSettingsButtonClose, 	"Close",			"Close window"			},
			{0, 																NULL,				NULL					}
		};

// copy bits
const SMrbNamedXShort kDGFCopyModuleSettingsBits[] =
							{
								{DGFCopyModuleSettingsPanel::kDGFCopyBitGain,			"Gain"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitOffset, 		"Offset"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitSum,			"Sum"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitFilter, 		"Filter"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitTrace,			"Trace" 		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitPileup, 		"Pileup"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitFIFO,			"FIFO"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitCSR,			"CSR"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitCoinc,			"Coinc" 		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitMCA,			"MCA"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitTau,			"Tau"			},
								{0, 													NULL			}
							};

extern DGFControlData * gDGFControlData;

ClassImp(DGFCopyModuleSettingsPanel)

DGFCopyModuleSettingsPanel::DGFCopyModuleSettingsPanel(const TGWindow * Window, const TGWindow * MainFrame,
												DGFControlInfo * InfoFrom, DGFControlInfo * InfoTo,
												DGFControlData * Data,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGTransientFrame(Window, MainFrame, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel
// Purpose:        Copy module settings
// Arguments:      TGWindow Window                -- connection to ROOT graphics
//                 TGWindow * MainFrame           -- main frame
//                 DGFControlInfo * InfoFrom      -- where to pass resulting bit masks
//                 DGFControlInfo * InfoTo        -- ...
//                 DGFControlData * Data          -- dgf database
//                 UInt_t Width                   -- window width in pixels
//                 UInt_t Height                  -- window height in pixels
//                 UInt_t Options                 -- options
// Results:        DGFControlInfo * Info
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * rbuttonGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * comboGC;

	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(comboGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	rbuttonGC = new TGMrbLayout(gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(rbuttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(labelGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);

	this->ChangeBackground(gDGFControlData->fColorGreen);


//	Initialize lists
	fLofCopyBits.SetName("Copy Bits");				// list of copy bits
	fLofCopyBits.AddNamedX(kDGFCopyModuleSettingsBits);
	fLofButtons.SetName("Action Buttons");			// list of actions
	fLofButtons.AddNamedX(kDGFCopyModuleSettingsButtons);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fInfoFrom = InfoFrom;
	fInfoTo = InfoTo;

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFCopyModuleSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFCopyModuleSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFCopyModuleSelectNone, "cbutton_none.xpm");
		
	TGLayoutHints * hFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(hFrameLayout);
	TGLayoutHints * vFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(vFrameLayout);
	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	buttonGC->SetLH(buttonLayout);
	HEAP(buttonLayout);
	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(labelLayout);

//	module / channel selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "Copy from", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, frameGC->LH());

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Bool_t clearList = kTRUE;
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->CopyKeyList(&fLofSrcModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}
	gDGFControlData->SetSelectedModule(fLofSrcModuleKeys.First()->GetName());		// dgf to start with
	gDGFControlData->SetSelectedChannel(0); 									// channel 0
	fModuleFrom = gDGFControlData->GetSelectedModuleIndex();
	
	fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
										&fLofSrcModuleKeys,
										DGFCopyModuleSettingsPanel::kDGFCopySelectModule, 2,
										DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kLEHeight,
										DGFCopyModuleSettingsPanel::kEntryWidth,
										frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectModule->Associate(this);
	
	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 40, 1, 10, 1);
	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 5, 1);
	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	rbuttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fSelectChannel = new TGMrbRadioButtonList(fSelectFrame,  "Channel", &fLofChannels, 1, 
										DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kLEHeight,
										frameGC, labelGC, rbuttonGC);
	HEAP(fSelectChannel);
	fSelectChannel->SetState(kDGFChannel0, kButtonDown);
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());

	gDGFControlData->SetLH(groupGC, frameGC, hFrameLayout);
	fVFrame = new TGVerticalFrame(this, 		DGFCopyModuleSettingsPanel::kFrameWidth,
												DGFCopyModuleSettingsPanel::kFrameHeight,
												kChildFrame, frameGC->BG());
	HEAP(fVFrame);
	this->AddFrame(fVFrame, frameGC->LH());
	
	gDGFControlData->SetLH(groupGC, frameGC, vFrameLayout);
	fCopyBits = new TGMrbCheckButtonGroup(fVFrame, "Settings",	&fLofCopyBits, 1,
																groupGC, buttonGC,
																lofSpecialButtons,
																kVerticalFrame);
	HEAP(fCopyBits);
	fVFrame->AddFrame(fCopyBits, groupGC->LH());
	fCopyBits->SetState(0xffffffff, kButtonDown);
	
	fChannels = new TGMrbCheckButtonGroup(fVFrame, "to Channels",	&fLofChannels, 1,
																groupGC, buttonGC,
																lofSpecialButtons,
																kHorizontalFrame);
	HEAP(fChannels);
	fVFrame->AddFrame(fChannels, groupGC->LH());

	fModules = new TGGroupFrame(fVFrame, "to Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	fVFrame->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
								gDGFControlData->CopyKeyList(&fLofDestModuleKeys[cl], cl, 1, kTRUE), 1, 
								DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kLEHeight,
								frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kFrameHeight,
													kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], 1, 
							DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, 1, 
							DGFCopyModuleSettingsPanel::kFrameWidth, DGFCopyModuleSettingsPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, frameGC->LH()->GetPadLeft(),
																		frameGC->LH()->GetPadRight(),
																		frameGC->LH()->GetPadTop(),
																		frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);

// buttons
	gDGFControlData->SetLH(groupGC, frameGC, vFrameLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Action", &fLofButtons, 1, groupGC, buttonGC, kHorizontalFrame);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, groupGC->LH());
	fButtonFrame->Associate(this);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(MainFrame->GetId(), this->GetParent()->GetId(),
								(((TGFrame *) MainFrame)->GetWidth() + 10), 0,
								ax, ay, wdum);
	Move(ax, ay);

	SetWindowName("DGFControl: Copy Module Settings");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
	gClient->WaitFor(this);
}

Bool_t DGFCopyModuleSettingsPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCoincModuleSettingsPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFCoincModuleSettingsPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFCopyModuleSelectColumn) {
						switch (Param1) {
							case kDGFCopyModuleSettingsButtonCopy:
								fInfoFrom->fModules[0] = fModuleFrom;
								fInfoFrom->fChannels = fSelectChannel->GetActive();
								fInfoFrom->fExecute = kTRUE;
								fInfoFrom->fAccessDSP = kFALSE;
								fInfoTo->fActionBits = fCopyBits->GetActive();
								fInfoTo->fChannels = fChannels->GetActive();
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) fInfoTo->fModules[cl] = fCluster[cl]->GetActive();
								fInfoTo->fExecute = kTRUE;
								fInfoTo->fAccessDSP = kTRUE;
								this->CloseWindow();
								break;
							case kDGFCopyModuleSettingsButtonClose:
								fInfoTo->fExecute = kFALSE;
								this->CloseWindow();
								break;
							case kDGFCopyModuleSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFCopyModuleSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
						}
					} else {
						Param1 -= kDGFCopyModuleSelectColumn;
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
						
				case kCM_COMBOBOX:
					fModuleFrom = Param2;
					break;
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
