#ifndef __DGFMcaDisplayPanel_h__
#define __DGFMcaDisplayPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFMcaDisplayPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFMcaDisplayPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TRootEmbeddedCanvas.h"
#include "TTimer.h"
#include "TCanvas.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGFileDialog.h"
#include "TGMenu.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbDGF.h"
#include "TMrbDGFEventBuffer.h"

#include "HistPresent.h"
#include "FitHist.h"

#include "TNGMrbTextButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbPictureButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFileEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TNGMrbCheckButton.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbProgressBar.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel
// Purpose:        Main frame to accumulate & display histograms
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFMcaDisplayPanel : public TGCompositeFrame {

	public:

				
		// cmd ids to dispatch over X events in this panel
		enum EDGFMcaCmdId		{
									kDGFMcaDisplayRunTime, 			//		runtime
									kDGFMcaDisplayTimeScale,		//				time scale
									kDGFMcaDisplaySelectDisplay, 	//		module/channel
									kDGFMcaDisplayRefreshDisplay, 	//		refresh
									kDGFMcaDisplayAcquire,			//		actions start accu
									kDGFMcaDisplayStop,				//				stop accu
									kDGFMcaDisplayHisto, 			//				display histogram
									kDGFMcaDisplayHistoClear, 		//				... & clear
									kDGFMcaDisplaySelectAll,		//  	select	all
									kDGFMcaDisplaySelectNone,		//				none
									kDGFMcaDisplaySelectColumn,		//				column
									kDGFMcaDisplaySelectChannel,	//		select	channel
									kDGFMcaDisplaySelectTimeScale	//				time scale
								};

		enum EDGFMcaTimeScaleId {
									kDGFMcaTimeScaleSecs =	BIT(0),		//	seconds
									kDGFMcaTimeScaleMins =	BIT(1),		//	minutes
									kDGFMcaTimeScaleHours = BIT(2),		//	hours
									kDGFMcaTimeScaleInfin = BIT(3)		//	infin
								};

		enum EDGFMcaRunState	 {
									kDGFMcaRunStopped = 0,
									kDGFMcaRunPausing,
									kDGFMcaIsRunning
								};

		enum					{	kDGFAccuTimerID,
									kDGFRefreshTimerID
								};

	public:
		DGFMcaDisplayPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFMcaDisplayPanel() { fHeap.Delete(); };

		virtual Bool_t HandleTimer(TTimer * Timer);

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void SelectDisplay(Int_t FrameId, Int_t Selection);
		void EntryChanged(Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);
		void RadioButtonPressed(Int_t FrameId, Int_t Selection);

		Bool_t McaPause();
		Bool_t McaResume();

	protected:
		Bool_t ResetValues();													// clear entry fields
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry
		Bool_t AcquireHistos(); 												// start accu
		Bool_t StoreHistos();
		Bool_t DisplayHisto(Bool_t ClearMCA = kFALSE);

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TNGMrbCheckButtonGroup * fSelectChannel;		//		channel

		TGGroupFrame * fAccuFrame;	 				//	accu
		TNGMrbLabelEntry * fRunTimeEntry; 			//		runtime
		TNGMrbRadioButtonList * fTimeScale; 			//		time scale

		TGGroupFrame * fDisplayFrame; 				// display
		TNGMrbLabelCombo * fDisplayModule; 			//		module
		TNGMrbRadioButtonList * fDisplayChannel;		//		channel
		TNGMrbLabelEntry * fRefreshTimeEntry; 		//		refresh

		TNGMrbTextButtonGroup * fButtonFrame;

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TMrbLofNamedX fMcaTimeScaleButtons;
		TMrbLofNamedX fMcaActions;

		TMrbLofNamedX fLofModuleKeys;				//! ... key list
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TNGMrbFocusList fFocusList;

		TGMrbProgressBar * fProgressBar;			// progress bar to show accu state
		TTimer * fAccuTimer;						// accu timer
		Bool_t fStopAccu;							// kTRUE if accu has to be stopped
		EDGFMcaRunState fRunState;					// run state
		Int_t fSecsToWait;							// seconds to wait
		Int_t fStopWatch;							// stopwatch

		TTimer * fRefreshTimer;						// refresh timer

		TGGroupFrame * fCanvasFrame; 				// display
		TRootEmbeddedCanvas * fCanvas;				// canvas to display histogram

		DGFModule * fModuleToBeDisplayed;			// module selected for display

		FitHist * fFitHist; 						// canvas

	ClassDef(DGFMcaDisplayPanel, 0) 	// [DGFControl] Accumulate/display MCA spectra
};

#endif
