//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFOffsetsPanel
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
<img src=dgfcontrol/DGFOffsetsPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

using namespace std;

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

const SMrbNamedX kDGFOffsetsActions[] =
			{
				{DGFOffsetsPanel::kDGFOffsetsStart,		"Ramp DAC",	"Start DAC ramp and calc offsets (control task 3)"	},
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

extern TMrbLogger * gMrbLog;

ClassImp(DGFOffsetsPanel)

DGFOffsetsPanel::DGFOffsetsPanel(TGCompositeFrame * TabFrame)
														: TGCompositeFrame(TabFrame, kTabWidth, kTabHeight, kVerticalFrame) {
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
	
	fActionButtons = new TGMrbTextButtonList(fActionFrame, NULL, &fActions, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	fActionButtons->Associate(this);

	TGLayoutHints * oLayout = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 2, 1, 5, 1);
	entryGC->SetLH(oLayout);
	HEAP(oLayout);

	fOffsetFrame = new TGGroupFrame(fHFrame, "Offset", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fOffsetFrame);
	fHFrame->AddFrame(fOffsetFrame, groupGC->LH());
	
	fOffsetValue = new TGMrbLabelEntry(fOffsetFrame, NULL,	200, 1,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fOffsetValue);
	fOffsetFrame->AddFrame(fOffsetValue, frameGC->LH());
	fOffsetValue->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fOffsetValue->GetEntry()->SetText("0");
	fOffsetValue->SetRange(0,65504);

	this->ChangeBackground(gDGFControlData->fColorGreen);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(kTabWidth, kTabHeight);
	MapWindow();
}

Bool_t DGFOffsetsPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFOffsetsPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFOffsetsSelectColumn) {
						switch (Param1) {
							case kDGFOffsetsStart:
								this->StartRamp();
								break;
							case kDGFOffsetsSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFOffsetsSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFOffsetsSelectColumn;
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
		
	TGTextEntry * entry = fOffsetValue->GetEntry();
	intStr = entry->GetText();
	intStr.ToInteger(offset);
	
	selectFlag = kFALSE;
	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	nofRamps = 0;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				rampBuffer.Reset();
				nofWords = dgf->GetDacRamp(rampBuffer);
				if (nofWords > 0) {
					if (!selectFlag) {
						rampFile = new TFile("dacRamp.root", "RECREATE");
						hl.open("dacRamp.histlist", ios::out);
					}
					selectFlag = kTRUE;
					hTitle = "DAC ramps for module ";
					hTitle += dgfModule->GetName();
					h = new TH1F(dgfModule->GetName(), hTitle.Data(), 8192, 0., 8192.);
					for (Int_t i = 0; i < nofWords; i++) h->Fill((Axis_t) i, rampBuffer[i]);
					h->Write();
					hl << dgfModule->GetName() << endl;
					Int_t start = 0;
					for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
						Int_t ok = this->CalibrateDac(&rampBuffer[start], 2048, offset, dacValue);
						if (ok >= 0) {
							gMrbLog->Out()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
											<< " (chn " << chn << "): dac(" << offset << ") = " << dacValue
											<< endl;
							gMrbLog->Flush(this->ClassName(), "CalibrateDac", setblue);
							dgf->SetParValue(chn, "TRACKDAC", dacValue, kTRUE);
						} else {
							gMrbLog->Err()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
											<< ": Can't calibrate DAC for channel " << chn
											<< endl;
							gMrbLog->Flush(this->ClassName(), "CalibrateDac");
						}
						start += 2048;
					}
					nofRamps++;
					gMrbLog->Out()	<< "StartRamp(): [" << dgfModule->GetName() << "] "
									<< nofWords << " ramp data written" << endl;
					gMrbLog->Flush(this->ClassName(), "CalibrateDac", setblue);
				}
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				
	if (selectFlag) {
		rampFile->Close();
		hl.close();
	}
	if (offlineMode || selectFlag) {
		gMrbLog->Out()	<< "StartRamp(): " << nofRamps
						<< " DAC ramps written to file \"dacRamp.root\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CalibrateDac", setblue);
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Int_t DGFOffsetsPanel::CalibrateDac(Int_t Trace[], Int_t TraceLength, Int_t TraceOffset, Int_t & DacValue) {
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
  for (i = 10; i < TraceLength - 21; i++) {
    if(Trace[i] != Trace[i + 1]) {
      r_start = i + 1;
      break;
    }
  }
  
  if (r_start == -1) {
	gMrbLog->Err()	<< "r_start == -1" << endl;
	gMrbLog->Flush(this->ClassName(), "CalibrateDac");
  }

  /* find end of ramp */
//  for (i = TraceLength - 1; i > 0; i--) {
  for (i = TraceLength - 21; i > 10; i--) {
    if(Trace[i] != Trace[i - 1]) {
      r_end = i;
      break;
    }
  }

  if (r_end == -1) {
	gMrbLog->Err()	<< "r_end == -1" << endl;
	gMrbLog->Flush(this->ClassName(), "CalibrateDac");
  }

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
