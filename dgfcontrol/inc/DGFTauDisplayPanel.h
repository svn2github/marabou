#ifndef __DGFTauDisplayPanel_h__
#define __DGFTauDisplayPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFTauDisplayPanel
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
// Name:           DGFTauDisplayPanel
// Purpose:        Main frame to calculate best tau value
// Constructors:   
// Description:    A dialog window to enter module params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFTauDisplayPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFTauCmdId 	{
									kDGFTauSelectModule, 				//		module
									kDGFTauTraceLength,					//		trace 	length
									kDGFTauTraceNofTraces,				//				number of traces
									kDGFTauFitFrom, 					//		fit 	from
									kDGFTauFitTo,						//				to
									kDGFTauFitParamA0,					//			 	param A0
									kDGFTauFitParamA1,					//				... A1
									kDGFTauFitParamA2,					//				... A2
									kDGFTauFitError,					//				const error
									kDGFTauFitChiSquare,				//				chi square
									kDGFTauDispTraceNo,					//		display trace#
									kDGFTauDispCurTau,					//				current tau
									kDGFTauDispBestTau,					//				best tau
									kDGFTauDispFile,					//				trace file
									kDGFTauButtonAcquire,				//		actions start tracing
									kDGFTauButtonTauOK, 				//				tau ok
									kDGFTauButtonBestTau,				//				take best tau
									kDGFTauButtonRemoveTraces,			//				remove trace files
									kDGFTauButtonSaveTrace,				//				save current trace data
									kDGFTauButtonReset, 				//				reset
									kDGFTauButtonClose					//				close
								};

		enum EDGFFitButtonsId 	{
									kDGFTauFitTraceYes = kDGFChannel3 << 1,			//		fit 	yes
									kDGFTauFitTraceNo = kDGFChannel3 << 2			//				no
								};

		enum EDGFStatBoxButtonsId
								{
									kDGFTauDispStatBoxYes = kDGFChannel3 << 3,		//		stat box	yes
									kDGFTauDispStatBoxNo = kDGFChannel3 << 4		//					no
								};

		// geometry settings
		enum					{	kFrameWidth 			= 400					};
		enum					{	kFrameHeight 			= 820					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 150					};
		enum					{	kFileEntryWidth			= 210					};
		enum					{	kLEHeight				= 22					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFTauDisplayPanel(const TGWindow * Parent, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFTauDisplayPanel() { fHeap.Delete(); };

		DGFTauDisplayPanel(const DGFTauDisplayPanel & f) : TGMainFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t ResetValues();													// clear entry fields
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry
		Bool_t AcquireTraces(); 												// start tracing
		Bool_t RemoveTrace(Int_t ModuleId = -1, Int_t ChannelId = -1);			// remove file(s) containing trace data
		Bool_t SaveTrace(const Char_t * FileName, Int_t ModuleId = -1, Int_t ChannelId = -1);	// save trace to files

		TMrbDGFEventBuffer * FindBuffer(Int_t TraceNo); // find buffer by trace/event number
		
		void SetRunning(Bool_t RunFlag);										// run flip-flop
		
		const Char_t * TraceFileName(const Char_t * Text = "", Int_t ModuleId = -1, Int_t ChannelId = -1);
		const Char_t * HistogramName(Int_t TraceNo = 1, Int_t ModuleId = -1, Int_t ChannelId = -1);
		const Char_t * BufferName(Int_t Index = 1, Int_t ModuleId = -1, Int_t ChannelId = -1);

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule; 			//		module
		TGMrbRadioButtonList * fSelectChannel;		//		channel

		TGGroupFrame * fTraceFrame; 				//	trace
		TGMrbLabelEntry * fTraceLengthEntry; 		//		length
		TGMrbLabelEntry * fNofTracesEntry; 			//		number of traces

		TGGroupFrame * fFitFrame;					//	fit
		TGMrbRadioButtonList * fFitTraceYesNo; 		//		yes/no
		TGMrbLabelEntry * fFitFromEntry; 			//		from
		TGMrbLabelEntry * fFitToEntry;	 			//		to
		TGMrbLabelEntry * fFitA0Entry;	 			//		A0
		TGMrbLabelEntry * fFitA1Entry;  			//		A1
		TGMrbLabelEntry * fFitA2Entry;  			//		A2
		TGMrbLabelEntry * fFitErrorEntry; 			//		error
		TGMrbLabelEntry * fFitChiSquareEntry; 		//		chi square

		TGGroupFrame * fDisplayFrame;				//	display
		TGMrbLabelEntry * fDispTraceNoEntry; 		//		trace number
		TGMrbRadioButtonList * fDispStatBox; 		//		stat box yes/no
		TGMrbLabelEntry * fDispCurTauEntry;  		//		current tau
		TGMrbLabelEntry * fDispBestTauEntry; 		//		best tau
		TGMrbFileEntry * fDispFileEntry;	 		//		trace file

		FitHist * fFitTrace; 						// ... canvas
		FitHist * fFitTau; 							// ... canvas

		TGMrbTextButtonGroup * fButtonFrame;

		TMrbLofNamedX fLofModuleKeys;				//! ... key list
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TGFileInfo fTraceFileInfo;					//!

		TFile * fTraceFile; 						// current trace file
		TMrbString fTraceFileName;					// file name, internal storage
		TH1F * fHistogram; 							// current histogram
		TMrbString fHistoName;						// histogram name & title, internal storage
		TMrbString fHistoTitle;
		TMrbString fEbuName;						// name of event buffer
		TObjArray fLofBuffers;						//! list of event buffers
				
		TH1F * fTauDistr;							//! tau distribution
		TF1 * fGaussian;							//! gaussian fit on tau distr
		
		TMrbDGFEventBuffer * fBuffer;				// event buffer
		
		Bool_t fIsRunning;							// kTRUE if trace acquisition running
		
		TMrbLofNamedX fFitTraceButtons;
		TMrbLofNamedX fDispStatBoxButtons;
		TMrbLofNamedX fTauActions;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFTauDisplayPanel, 0) 	// [DGFControl] Collect/display tau traces
};

#endif
