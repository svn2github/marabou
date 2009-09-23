#ifndef __DGFTauFitPanel_h__
#define __DGFTauFitPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFTauFitPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFTauFitPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TRootEmbeddedCanvas.h"
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
#include "TNGMrbCheckButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbPictureButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFileEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel
// Purpose:        Main frame to accumulate & display histograms
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFTauFitPanel : public TGCompositeFrame {

	public:

				
		// cmd ids to dispatch over X events in this panel
		enum EDGFTauFitCmdId 	{
										kDGFTauFitRangeStart, 		//		tau range
										kDGFTauFitRangeStep,
										kDGFTauFitRangeStop,
										kDGFTauFitRunTime, 			//		runtime
										kDGFTauFitTimeScale,		//				time scale
										kDGFTauFitStartFit,			//		actions start accu
										kDGFTauFitStop,			//				stop accu
										kDGFTauFitReset, 			//				reset
										kDGFTauFitSelectAll,		// select	all
										kDGFTauFitSelectNone,		//				none
										kDGFTauFitSelectColumn		//				column
									};

		enum EDGFTauFitTimeScaleId {
									kDGFTauFitTimeScaleSecs = kDGFChannel3 << 1,		//	seconds
									kDGFTauFitTimeScaleMins = kDGFChannel3 << 2,		//	minutes
									kDGFTauFitTimeScaleHours = kDGFChannel3 << 3,		//	hours
								};

	public:
		DGFTauFitPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFTauFitPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot methods
		void EntryChanged(Int_t Selection);
		void SelectModule(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t ResetValues();													// clear entry fields
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry
		Bool_t TauFit();		 												// start tau fit

		void SetRunning(Bool_t RunFlag);										// run flip-flop
		
	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TNGMrbCheckButtonGroup * fSelectChannel;		//		channel

		TGGroupFrame * fTauFrame;	 				//	tau
		TNGMrbLabelEntry * fTauStartEntry; 			//		start
		TNGMrbLabelEntry * fTauStepEntry; 			//		step
		TNGMrbLabelEntry * fTauStopEntry; 			//		stop

		TGGroupFrame * fAccuFrame;	 				//	accu
		TNGMrbLabelEntry * fRunTimeEntry; 			//		runtime
		TNGMrbRadioButtonList * fTimeScale; 			//		time scale

		TNGMrbTextButtonGroup * fButtonFrame;

		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		Bool_t fIsRunning;							// kTRUE if trace acquisition running
		
		TMrbLofNamedX fTauFitTimeScaleButtons;
		TMrbLofNamedX fTauFitActions;

		TNGMrbFocusList fFocusList;

	ClassDef(DGFTauFitPanel, 0) 	// [DGFControl] Tau fit
};

#endif
