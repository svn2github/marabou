#ifndef __DGFMcaDisplayPanel_h__
#define __DGFMcaDisplayPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFMcaDisplayPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
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

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel
// Purpose:        Transient frame to accumulate & display histograms
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFMcaDisplayPanel : public TGTransientFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFTauCmdId 	{
									kDGFMcaSelectModule, 				//		module
									kDGFMcaRunTime, 					//		runtime
									kDGFMcaTimeScale,					//				time scale
									kDGFMcaButtonAcquire,				//		actions start accu
									kDGFMcaButtonSaveHistos,			//				save current histograms
									kDGFMcaButtonReset, 				//				reset
									kDGFMcaButtonClose					//				close
								};

		enum EDGFMcaTimeScaleId {
									kDGFMcaTimeScaleSecs = kDGFChannel3 << 1,		//	seconds
									kDGFMcaTimeScaleMins = kDGFChannel3 << 2,		//	minutes
									kDGFMcaTimeScaleHours = kDGFChannel3 << 3,		//	hours
								};

		// geometry settings
		enum					{	kFrameWidth 			= 400					};
		enum					{	kFrameHeight 			= 330					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 150					};
		enum					{	kFileEntryWidth			= 210					};
		enum					{	kLEHeight				= 22					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFMcaDisplayPanel(const TGWindow * Parent, const TGWindow * Main, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFMcaDisplayPanel() { fHeap.Delete(); };

		DGFMcaDisplayPanel(const DGFMcaDisplayPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t ResetValues();													// clear entry fields
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry
		Bool_t AcquireHistos(); 												// start accu
		Bool_t SaveHistos(const Char_t * FileName, Int_t ModuleId = -1, UInt_t ChannelPattern = 0xf);	// save histograms

		void SetRunning(Bool_t RunFlag);										// run flip-flop
		
	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				//	select
		TGMrbLabelCombo * fSelectModule; 			//		module
		TGMrbCheckButtonList * fSelectChannel;		//		channel

		TGGroupFrame * fAccuFrame;	 				//	accu
		TGMrbLabelEntry * fRunTimeEntry; 			//		runtime
		TGMrbRadioButtonList * fTimeScale; 			//		time scale

		TGMrbTextButtonGroup * fButtonFrame;

		TMrbLofNamedX fLofModuleKeys;				//! ... key list
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TGFileInfo fMcaFileInfo;					//!

		Bool_t fIsRunning;							// kTRUE if trace acquisition running
		
		TMrbLofNamedX fMcaTimeScaleButtons;
		TMrbLofNamedX fMcaActions;

		TGMrbFocusList fFocusList;
		TGMrbLofKeyBindings fKeyBindings; 		// key bindings

	ClassDef(DGFMcaDisplayPanel, 0) 	// [DGFControl] Accumulate/display MCA spectra
};

#endif
