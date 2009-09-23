//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFOffsetsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFOffsetsPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFOffsetsPanel.gif>
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

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFOffsetsPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>
#include <math.h>

const SMrbNamedX kDGFOffsetsActions[] =
			{
				{DGFOffsetsPanel::kDGFOffsetsStart,		"Ramp DAC",	"Start DAC ramp and calc offsets (control task 3)"	},
				{DGFOffsetsPanel::kDGFOffsetsAbort,		"Abort ramp",	"Abort DAC ramping"	},
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

extern TMrbLogger * gMrbLog;


extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

static TMrbLofDGFs lofDgfs;

ClassImp(DGFOffsetsPanel)

DGFOffsetsPanel::DGFOffsetsPanel(TGCompositeFrame * TabFrame) :
						TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel
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

/*	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;*/

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

/*	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
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
								gDGFControlData->fColorWhite);	HEAP(entryGC);*/
	
	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFOffsetsSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFOffsetsSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFOffsetsSelectNone, "cbutton_none.xpm");
	
	
//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFOffsetsActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules, dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile, 1,0, 
							kTabWidth, kLEHeight,0,lofSpecialButtons, kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled, kFALSE);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown, kFALSE);
	}
	
	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, dgfFrameLayout);
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1,stdProfile, 1,0, 
							kTabWidth, kLEHeight);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], dgfFrameLayout);
		((TNGMrbButtonFrame *) fGroupSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
	}
	fAllSelect = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1,stdProfile, 1,0, 
							kTabWidth, kLEHeight);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	dgfFrameLayout->GetPadLeft(),
																			dgfFrameLayout->GetPadRight(),
																			dgfFrameLayout->GetPadTop(),
																			dgfFrameLayout->GetPadBottom()));
	((TNGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
			
// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
// 	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);

	fHFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fHFrame);
	this->AddFrame(fHFrame, dgfFrameLayout);

	fActionFrame = new TGGroupFrame(fHFrame, "Actions", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fActionFrame);
	fHFrame->AddFrame(fActionFrame, dgfFrameLayout);
	
	fActionButtons = new TNGMrbTextButtonList(fActionFrame, NULL, &fActions, -1,stdProfile, 1,0,
							kTabWidth, kLEHeight);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons, dgfFrameLayout);
	fActionButtons->JustifyButton(kTextCenterX);
	((TNGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	TGLayoutHints * oLayout = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 2, 1, 5, 1);
// 	entryGC->SetLH(oLayout);
	HEAP(oLayout);

	fOffsetFrame = new TGGroupFrame(fHFrame, "Offset", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fOffsetFrame);
	fHFrame->AddFrame(fOffsetFrame, dgfFrameLayout);
	
	fOffsetValue = new TNGMrbLabelEntry(fOffsetFrame, "Offset Value",	 1,stdProfile,
																kLEWidth,
																kLEHeight,
																150,TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fOffsetValue);
	fOffsetFrame->AddFrame(fOffsetValue,dgfFrameLayout);
// 	fOffsetValue->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fOffsetValue->SetFormat(TGNumberEntry::kNESInteger);
	fOffsetValue->SetIntNumber(400);
// 	fOffsetValue->SetRange(0,65504);
	fOffsetValue->SetLimits(TGNumberEntry::kNELLimitMinMax,0,65504);

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

void DGFOffsetsPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFOffsetsSelectColumn) {
		switch (Selection) {
			case kDGFOffsetsSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFOffsetsSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;							
		}
	} else {
		Selection -= kDGFOffsetsSelectColumn;
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

void DGFOffsetsPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFOffsetsStart:
			this->StartRamp();
			break;
		case kDGFOffsetsAbort:
			lofDgfs.Abort();
			break;
	}
}

Bool_t DGFOffsetsPanel::StartRamp() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel::StartRamp
// Purpose:        Start DAC ramp
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI rampBuffer;
	TFile * rampFile = NULL;
	DGFModule * dgfModule;
	Int_t modNo, cl;
	TMrbDGF * dgf;
	TH1F * h;
	Bool_t selectFlag;
	Int_t nofWords;
	TString hTitle;
	Int_t nofRamps;
	ofstream hl;
	TMrbString intStr;
	Int_t nofModules;
					
	Bool_t offlineMode = gDGFControlData->IsOffline();

	rampBuffer.Set(8192);
	
	Int_t offset, dacValue;
		
	offset = fOffsetValue->GetIntNumber();
	
	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	nofRamps = 0;
	lofDgfs.Clear();
	selectFlag = kFALSE;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				dgf->GetDacRamp_Init();
				dgf->GetDacRamp_Start();
				lofDgfs.AddModule(dgf, kTRUE);
				selectFlag = kTRUE;
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				

	if (!selectFlag) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	if (!offlineMode) {
		lofDgfs.ResetAbort();
		lofDgfs.WaitActive(10);
	}

	dgf = (TMrbDGF *) lofDgfs.First();
	selectFlag = kFALSE;
	while (dgf) {
		if (!offlineMode) {
			rampBuffer.Reset();
			nofWords = dgf->GetDacRamp_Stop(rampBuffer);
			if (nofWords > 0) {
				if (!selectFlag) {
					rampFile = new TFile("dacRamp.root", "RECREATE");
					hl.open("dacRamp.histlist", ios::out);
				}
				selectFlag = kTRUE;
				hTitle = "DAC ramps for module ";
				hTitle += dgf->GetName();
				h = new TH1F(dgf->GetName(), hTitle.Data(), 8192, 0., 8192.);
				for (Int_t i = 0; i < nofWords; i++) h->Fill((Axis_t) i, rampBuffer[i]);
				h->Write();
				hl << dgf->GetName() << endl;
				Int_t start = 0;
				for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
					Int_t ok = this->CalibrateDac(dgf->GetName(), chn, &rampBuffer[start], 2048, offset, dacValue);
					if (ok >= 0) {
						gMrbLog->Out()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
										<< " (chn " << chn << "): dac(" << offset << ") = " << dacValue
										<< endl;
						gMrbLog->Flush(this->ClassName(), "StartRamp", setblue);
						dgf->SetParValue(chn, "TRACKDAC", dacValue, kTRUE);
					} else {
						gMrbLog->Err()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
										<< ": Can't calibrate DAC for channel " << chn
										<< endl;
						gMrbLog->Flush(this->ClassName(), "StartRamp");
					}
					start += 2048;
				}
				nofRamps++;
				gMrbLog->Out()	<< "[" << dgf->GetName() << "] "
								<< nofWords << " ramp data written" << endl;
				gMrbLog->Flush(this->ClassName(), "StartRamp", setblue);
				
			}		
		}
		dgf = (TMrbDGF *) lofDgfs.After(dgf);
	}

	if (nofRamps > 0) {
		rampFile->Close();
		hl.close();
	}
	if (offlineMode || (nofRamps > 0)) {
		gMrbLog->Out()	<< nofRamps << " DAC ramps written to file \"dacRamp.root\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartRamp", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Couldn't get any traces" << endl;
		gMrbLog->Flush(this->ClassName(), "StartRamp");
		return(kTRUE);
	}
}

Int_t DGFOffsetsPanel::CalibrateDac(const Char_t * DgfName, Int_t Channel, Int_t Trace[], Int_t TraceLength, Int_t TraceOffset, Int_t & DacValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel::StartRamp
// Purpose:        Start DAC ramp
// Arguments:      Int_t Trace[]        -- trace data containing dac ramp
//                 Int_t TraceLength    -- length of trace
//                 Int_t TraceOffset    -- wanted offset for trace
// Results:        Int_t & dacValue     -- pointer to dac_value that produces the wanted offset 
// Exceptions:     
//                 -1 - big problemCalibrateDac
//                 0 - everything ok
//                 1 - not perfect, but ok
// Description:
//                 Algorithm:
//                 The dac-ramp trace can be divided in 3 parts.
//                      1) arbitrary but constant values
//                      2) actual ramp
//                      3) arbitrary but constant values
//                 like:
//
//                        (1)     (2)    (3)
//                                      ------------
//                                     /
//                        --------    /
//                                   /
//                                  /
//                                 /
//                                /
//
//                 - to find the start and end we search for different neigboring values (from 
//                   the beginning and from the end)
//                 - do linear fit of section (2) and determine right dac values from fit parameters
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

  Int_t retval;   /* return value */

  Int_t i;
  Int_t r_start=-1;  /* start of dac Ramp */
  Int_t r_end=-1;    /* one step after the end of dac Ramp */
  Int_t n=0;  /* length of ramp trace */

  Double_t offset;
  Double_t slope;
  Double_t sumx, sumy, sumx2, sumxy, sumw;
  Double_t diff;
  Double_t dacCalFactor;

  Double_t chi2;  /* standard deviation */

  Double_t * w = new Double_t[TraceLength];  /* weights */

  for (i = 0; i < TraceLength; i++) w[i]=1;

  /* find start of ramp */
//  for (i = 0; i < TraceLength - 1; i++) {
  for (i = 250; i < TraceLength - 250; i++) {
    if(Trace[i] != Trace[i + 1]) {
      if (abs((Trace[i] + Trace[i+2])/2 - Trace[i+1]) > 100) continue;
      if (Trace[i] == 0 && Trace[i+2] == 0) continue;
	  r_start = i + 1;
      break;
    }
  }
  
  if (r_start == -1) {
	gMrbLog->Err()	<< "r_start == -1" << endl;
	gMrbLog->Flush(this->ClassName(), "CalibrateDac");
	return(-1);
  }

  /* find end of ramp */
//  for (i = TraceLength - 1; i > 0; i--) {
  for (i = TraceLength - 250; i > 250; i--) {
    if(Trace[i] != Trace[i - 1]) {
      if (abs((Trace[i] + Trace[i-2])/2 - Trace[i-1]) > 100) continue;
      if (Trace[i] == 0 && Trace[i-2] == 0) continue;
      r_end = i;
      break;
    }
  }

  if (r_end == -1) {
	gMrbLog->Err()	<< "r_end == -1" << endl;
	gMrbLog->Flush(this->ClassName(), "CalibrateDac");
	return(-1);
  }

  /* increase r_start and decrease r_end samples to avoid problems with 'strange' values */
  r_start += 20;
  r_end -= 20;
static Bool_t once = kFALSE;
static ofstream f;
if (!once) {
	f.open("xxx.dat", ios::out);
	once = kTRUE;
}
f << DgfName << " chn=" << Channel << " start=" << r_start << " end=" << r_end << endl;

  for (Int_t j = 0; j < 3; j++) {    /* make some loops; after each loop points with a large diviations are excluded */
    
    /* calculate slope and offset using linear regression */
    /* x-value = offset    (trace[i])
       y-value = dac_value (i)
    */
    
    n = r_end - r_start;
    if (n < 3) {
		gMrbLog->Err()	<< "ramp has only " << n << " points" << endl;
		gMrbLog->Flush(this->ClassName(), "CalibrateDac");
		retval = -1;
		delete[] w;
		return retval;
    }
    
    sumx = 0;
    for (i = r_start; i < r_end; i++) {
      sumx += w[i] * Trace[i];
    }
    sumx2 = 0;
    for (i = r_start;i < r_end; i++) {
      sumx2 += w[i] * Trace[i] * Trace[i];
    }
    sumy = 0;
    for (i = r_start; i < r_end; i++) {
      sumy += w[i] * i;
    }
    sumxy = 0;
    for (i = r_start; i < r_end; i++) {
      sumxy += w[i] * Trace[i] * i;
    }
    sumw=0;
    for (i = r_start; i < r_end; i++) {
      sumw += w[i];
    }
    
    slope  = ( sumw * sumxy - sumx * sumy )   / (sumw * sumx2 - sumx * sumx);
    offset = (sumx2 * sumy - sumx * sumxy) / (sumw * sumx2 - sumx * sumx);
    
    /* determine standard deviation */
    
    chi2 = 0;
    for (i = r_start; i < r_end; i++) {
      diff = (i - (slope * Trace[i] + offset));
      chi2 += w[i] * diff * diff;
    }
    if (chi2 < 0) {
		gMrbLog->Err()	<< "chi2^2 < 0;  chi2^2= " << chi2 << endl;
		gMrbLog->Flush(this->ClassName(), "CalibrateDac");
		retval = -1;
		delete[] w;
		return retval;
    }
    chi2 = sqrt(chi2) / (n - 2);
    
    /* set weight to 0 if diff from curve too large */
    for (i = r_start; i < r_end; i++) {
      diff = (i - (slope * Trace[i] + offset));
      w[i] = 1;
      if (diff * diff > 100 * chi2 * chi2) w[i]=0;
    }
  } /* loop over w-adjustment */

  dacCalFactor = 65504. / 2047.; /* see xia manual */
  DacValue = (Int_t) (dacCalFactor * (slope * TraceOffset + offset));
  retval=0;
  delete[] w;
  return retval;
}
