//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302Panel.cxx,v 1.1 2008-09-23 10:47:20 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFInstrumentPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TObjString.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbC2Lynx.h"

#include "TObjString.h"
#include "TApplication.h"

#include "VMEControlData.h"
#include "VMESis3302Panel.h"

#include "SetColor.h"

extern TMrbC2Lynx * gMrbC2Lynx;
extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMESis3302Panel)

VMESis3302Panel::VMESis3302Panel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        Connect to LynxOs server
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
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

	frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorWhite);	HEAP(entryGC);


	fTabFrame = new TGTab(this, gVMEControlData->GetFrameWidth(), gVMEControlData->GetFrameHeight());
	HEAP(fTabFrame);
	TGLayoutHints * tabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
	HEAP(tabLayout);
	this->AddFrame(fTabFrame, tabLayout);
	fTabFrame->Connect("Selected(Int_t)", this->ClassName(), this, "TabChanged(Int_t)");

	fSettingsPanel = NULL;
	fSettingsTab = fTabFrame->AddTab("Settings");

	fTabFrame->SetTab(kVMETabSettings);
	this->TabChanged(kVMETabSettings);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void VMESis3302Panel::TabChanged(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TabChanged
// Purpose:        Slot method
// Arguments:      Int_t Selection   -- selection index
// Results:        --
// Exceptions:     
// Description:    Called upon 'tab select' events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = kVMETabSettings; i < kVMELastTab; i++) {
		fTabFrame->GetTabTab(i)->ChangeBackground(gVMEControlData->fColorGray);
	}
	fTabFrame->GetTabTab(Selection)->ChangeBackground(gVMEControlData->fColorGold);

	switch (Selection) {

		case kVMETabSettings:
			if (fSettingsPanel == NULL) {
				this->SetupModuleList();
				fSettingsPanel = new VMESis3302SettingsPanel(fSettingsTab, &fLofModules);
			}
			fSettingsPanel->UpdateGUI();
			break;
	}
}

Bool_t VMESis3302Panel::SetupModuleList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::SetupModuleList
// Purpose:        Fill list of modules
// Arguments:      -- 
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbC2Lynx) {
		if (gVMEControlData->SetupModuleList(fLofModules, "TMrbSis_3302")) return(kTRUE);
		if (this->GetNofModules() == 0) {
			gVMEControlData->MsgBox(this, "SetupModuleList", "No modules", "No SIS 3302 modules found");
			return(kFALSE);
		}
	} else {
		gVMEControlData->MsgBox(this, "SetupModuleList", "Not connected", "No connection to LynxOs server");
		return(kFALSE);
	}
	return(kFALSE);
}
