#ifndef __DGFInstrumentPanel_h__
#define __DGFInstrumentPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFInstrumentPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFModule.h"
#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel
// Purpose:        Transient frame to define module params
// Constructors:   
// Methods:        CloseWindow      -- close main frame
//                 ProcessMessage   -- process widget events
// Description:    A dialog window to enter module params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFInstrumentPanel : public TGTransientFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFInstrCmdId 	{
									kDGFInstrSelectModule, 				//		module
									kDGFInstrEnergyPeakTimeEntry, 		//		energy peak time
									kDGFInstrEnergyGapTimeEntry,	  	//		energy gap time
									kDGFInstrEnergyAveragingEntry,		//		average
									kDGFInstrEnergyTauEntry,			//		tau value
									kDGFInstrTriggerPeakTimeEntry, 		//		trigger peak time
									kDGFInstrTriggerGapTimeEntry,   	//		trigger gap time
									kDGFInstrTriggerThresholdEntry,		//		threshold
									kDGFInstrTraceLengthEntry,	  		//		length
									kDGFInstrTraceDelayEntry, 			//		delay
									kDGFInstrTracePSAOffsetEntry, 		//		psa offset
									kDGFInstrTracePSALengthEntry,		//		psa length
									kDGFInstrStatRegModICSREntry, 		//		module switchbus
									kDGFInstrStatRegModICSREditButton,	//		edit
									kDGFInstrStatRegChanCSRAEntry, 		//		channel csra
									kDGFInstrStatRegChanCSRAEditButton,	//		edit
									kDGFInstrStatCoincPatternEntry, 	//		pattern
									kDGFInstrStatCoincPatternEditButton,	//	edit
									kDGFInstrDACGainEntry, 				//		DAC
									kDGFInstrDACVVEntry,			  	//		V/V
									kDGFInstrDACOffsetEntry,		 	//		DAC
									kDGFInstrDACVoltEntry,		  		//		Volt
									kDGFInstrCFDFractionEntry,	  		//		fraction
									kDGFInstrMCAEnergyEntry, 			//		energy
									kDGFInstrMCAEnergyBinsEntry,		//		combine bins
									kDGFInstrMCABaselineDCEntry,		//		DC
									kDGFInstrMCABaselineBinsEntry,		//		combine bins
									kDGFInstrButtonApplyChanges,		//		apply changes & calc corrections
									kDGFInstrButtonCopy,				//		copy
									kDGFInstrButtonShow,				//		show params
									kDGFInstrButtonClose				//		close
								};

		// geometry settings
		enum					{	kFrameWidth 			= 1000					};
		enum					{	kFrameHeight 			= 670					};

		enum					{	kVFrameWidth 			= kFrameWidth / 2		};
		enum					{	kVFrameHeight 			= 300					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kLEHeight				= 22					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFInstrumentPanel(const TGWindow * Parent, const TGWindow * Main, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFInstrumentPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		DGFInstrumentPanel(const DGFInstrumentPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t InitializeValues(Bool_t ReadFromDSP = kFALSE);					// initialize entry fields with dgf values
		Bool_t WriteDSP(DGFModule * Module, Int_t ChannelId);					// update DSP
		Bool_t ReadDSP(DGFModule * Module, Int_t ChannelId);					// read DSP
		Bool_t CopyModuleSettings();											// copy settings
		Bool_t ShowModuleSettings();											// show settings
		Bool_t UpdateValue(Int_t EntryId, Int_t ModuleId, Int_t ChannelId);		// update entry value
		void MoveFocus(Int_t EntryId);											// move focus to next entry

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule; 			//		module
		TGMrbRadioButtonList * fSelectChannel;		//		channel

		TGHorizontalFrame * fInstrFrame;			// horiz frame containing 2 rows
		TGVerticalFrame * fLeftFrame; 				//  	left row
		TGGroupFrame * fFilterFrame;				//			filter
		TGGroupFrame * fEnergyFilterFrame;			//				energy
		TGMrbLabelEntry * fEnergyPeakTimeEntry; 	//					peak time
		TGMrbLabelEntry * fEnergyGapTimeEntry;  	//					gap time
		TGMrbLabelEntry * fEnergyAveragingEntry;	//					average
		TGMrbLabelEntry * fEnergyTauEntry;			//					tau value
		TGGroupFrame * fTriggerFilterFrame;			//				trigger
		TGMrbLabelEntry * fTriggerPeakTimeEntry; 	//					peak time
		TGMrbLabelEntry * fTriggerGapTimeEntry;  	//					gap time
		TGMrbLabelEntry * fTriggerThresholdEntry;	//					threshold
		TGGroupFrame * fTraceFrame;					//			trace
		TGGroupFrame * fTraceLengthDelayFrame;		//				length & delay
		TGMrbLabelEntry * fTraceLengthEntry;	  	//					length
		TGMrbLabelEntry * fTraceDelayEntry; 		//					delay
		TGGroupFrame * fTracePSAFrame;				//				psa
		TGMrbLabelEntry * fTracePSAOffsetEntry;	  	//					offset
		TGMrbLabelEntry * fTracePSALengthEntry; 	//					length
		TGGroupFrame * fStatRegFrame;				//			status regs
		TGGroupFrame * fStatRegEntryFrame;			//
		TGMrbLabelEntry * fStatRegModICSREntry; 	//				switchbus
		TMrbNamedX * fModICSREditButton;			//
		TGMrbLabelEntry * fStatRegChanCSRAEntry; 	//				channel csra
		TMrbNamedX * fChanCSRAEditButton;			//
		TGMrbLabelEntry * fStatCoincPatternEntry; 	//				coinc pattern
		TMrbNamedX * fCoincPatternEditButton;		//

		TGVerticalFrame * fRightFrame; 				//		right row
		TGGroupFrame * fDACFrame;					//			DAC
		TGGroupFrame * fDACGainFrame;				//				gain
		TGMrbLabelEntry * fDACGainEntry; 			//					DAC
		TGMrbLabelEntry * fDACVVEntry;			  	//					V/V
		TGGroupFrame * fDACOffsetFrame; 			//				offset
		TGMrbLabelEntry * fDACOffsetEntry;		 	//					DAC
		TGMrbLabelEntry * fDACVoltEntry;		  	//					Volt
		TGGroupFrame * fCFDFrame;					//			CFD
		TGGroupFrame * fCFDFractionFrame;			//				fraction
		TGMrbLabelEntry * fCFDFractionEntry;	  	//
		TGGroupFrame * fMCAFrame;					//			MCA
		TGGroupFrame * fMCAEnergyFrame;				//				energy
		TGMrbLabelEntry * fMCAEnergyEntry; 			//					energy
		TGMrbLabelEntry * fMCAEnergyBinsEntry;		//					combine bins
		TGGroupFrame * fMCABaselineFrame;			//				baseline
		TGMrbLabelEntry * fMCABaselineDCEntry;		//					DC
		TGMrbLabelEntry * fMCABaselineBinsEntry;	//					combine bins

		TGMrbTextButtonGroup * fModuleButtonFrame;	// buttons

		TGLabel * fDummyLabel;

		TMrbLofNamedX fLofModuleKeys;				//! ... key list
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TMrbLofNamedX fInstrChnActions;				// list of actions
		TMrbLofNamedX fInstrModuleActions;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFInstrumentPanel, 0) 	// [DGFControl] Edit module/channel settings
};

#endif
