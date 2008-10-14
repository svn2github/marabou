#ifndef __DGFInstrumentPanel_h__
#define __DGFInstrumentPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFInstrumentPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFInstrumentPanel.h,v 1.20 2008-10-14 10:22:29 Marabou Exp $       
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
// Purpose:        Main frame to define module params
// Constructors:   
// Description:    A dialog window to enter module params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFInstrumentPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMEServerCmdId 	{
									kDGFInstrSelectModule = 100, 		//		module
									kDGFInstrSelectChannel, 			//		channel
									kDGFInstrEnergyPeakTimeEntry, 		//		energy peak time
									kDGFInstrEnergyGapTimeEntry,	  	//		energy gap time
									kDGFInstrEnergyAveragingEntry,		//		average
									kDGFInstrEnergyTauEntry,			//		tau value
									kDGFInstrTriggerPeakTimeEntry, 		//		trigger peak time
									kDGFInstrTriggerGapTimeEntry,   	//		trigger gap time
									kDGFInstrTriggerThresholdEntry,		//		threshold
									kDGFInstrTraceLengthEntry,	  		//		length
									kDGFInstrTraceDelayEntry, 			//		delay
									kDGFInstrTraceXPSAOffsetEntry, 		//		xia-psa:	offset
									kDGFInstrTraceXPSALengthEntry,		//					length
									kDGFInstrTraceUPSAOnOffButton, 		//		user-psa:	on/off
									kDGFInstrTraceUPSABaselineEntry, 	//					baseline
									kDGFInstrTraceUPSAEnergyCutoffEntry,	//				energy cutoff
									kDGFInstrTraceUPSATriggerThreshEntry,	//				trigger threshold
									kDGFInstrTraceUPSAT90ThreshEntry,	//					T90 threshold
									kDGFInstrTraceUPSALengthEntry,		//					length
									kDGFInstrTraceUPSAOffsetEntry,		//					offset
									kDGFInstrTraceUPSATFAEnergyCutoffEntry, 	//			TFA energy cutoff
									kDGFInstrStatRegModICSREntry, 		//		module switchbus
									kDGFInstrStatRegModICSREditButton,	//		edit
									kDGFInstrStatRegChanCSRAEntry, 		//		channel csra
									kDGFInstrStatRegChanCSRAEditButton,	//		edit
									kDGFInstrStatRegUserPsaCSREntry, 		//		user psa
									kDGFInstrStatRegUserPsaCSREditButton,	//		edit
									kDGFInstrStatCoincPatternEntry, 		//		coinc pattern
									kDGFInstrStatCoincPatternEditButton,	//		edit
									kDGFInstrStatRunTaskEntry,  			//		runtask
									kDGFInstrStatRunTaskEditButton, 		//		edit
									kDGFInstrDACGainEntry, 				//		DAC
									kDGFInstrDACVVEntry,			  	//		V/V
									kDGFInstrDACOffsetEntry,		 	//		DAC
									kDGFInstrDACVoltEntry,		  		//		Volt
									kDGFInstrCFDRegEntry,		  		//		CFD register
									kDGFInstrCFDOnOffButton,			//		CFD on/off
									kDGFInstrCFDDelayBeforeLEEntry, 	//		CFD delay before LE
									kDGFInstrCFDDelayBipolarEntry,	 	//		CFD delay bipolar
									kDGFInstrCFDWalkEntry,  		 	//		CFD walk
									kDGFInstrCFDFractionButton, 		//		CFD walk
									kDGFInstrMCAEnergyEntry, 			//		energy
									kDGFInstrMCAEnergyBinsEntry,		//		combine bins
									kDGFInstrMCABaselineDCEntry,		//		DC
									kDGFInstrMCABaselineBinsEntry,		//		combine bins
									kDGFInstrButtonUpdateFPGAs,			//		update fpgas
									kDGFInstrButtonShowParams,			//		show params
								};

		enum EDGFInstrCDFOnOff 	{
										kDGFInstrCFDOn	=	BIT(0),
										kDGFInstrCFDOff =	BIT(1)
								};

		enum EDGFInstrCDFFraction 	{
										kDGFInstrCFDFract00	=	BIT(0),
										kDGFInstrCFDFract01	=	BIT(1),
										kDGFInstrCFDFract10	=	BIT(2),
									};

		enum EDGFInstrUPSAOnOff 	{
										kDGFInstrUPSAOn 	=	BIT(0),
										kDGFInstrUPSAOff	=	BIT(1)
								};

	public:
		DGFInstrumentPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFInstrumentPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		Bool_t InitializeValues(Bool_t ReadFromDSP = kFALSE);					// initialize entry fields with dgf values

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot methods
		void EntryChanged(Int_t FrameId, Int_t Selection);
		void SelectModule(Int_t FrameId, Int_t Selection);
		void RadioButtonPressed(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t WriteDSP(DGFModule * Module, Int_t ChannelId);					// update DSP
		Bool_t ReadDSP(DGFModule * Module, Int_t ChannelId);					// read DSP
		Bool_t ShowModuleSettings();											// show settings
		Bool_t UpdateValue(Int_t EntryId, Int_t ModuleId, Int_t ChannelId);		// update entry value
		void MoveFocus(Int_t EntryId);											// move focus to next entry
		Bool_t SetGFLT(Bool_t OnFlag = kTRUE);									// set/clear gflt
		Bool_t UpdateCFD(TMrbDGF * Module, Int_t Channel);						// update cfd
		Bool_t InitializeCFD(Int_t OnOff, Int_t Fraction);						// init cfd
		Bool_t UpdateUPSA(TMrbDGF * Module, Int_t Channel);						// update user psa
		Bool_t InitializeUPSA();												// init upsa

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
		TGVerticalFrame * fTraceLeftFrame;
		TGVerticalFrame * fTraceRightFrame;
		TGGroupFrame * fTraceLengthDelayFrame;		//				length & delay
		TGMrbLabelEntry * fTraceLengthEntry;	  	//					length
		TGMrbLabelEntry * fTraceDelayEntry; 		//					delay
		TGGroupFrame * fTraceXPSAFrame;				//				xia-psa
		TGMrbLabelEntry * fTraceXPSAOffsetEntry;	//					offset
		TGMrbLabelEntry * fTraceXPSALengthEntry; 	//					length
		TGGroupFrame * fTraceUPSAFrame;				//				user-psa
		TGMrbRadioButtonList * fTraceUPSAOnOffButton;	//				on/off
		TGMrbLabelEntry * fTraceUPSABaselineEntry; 	//					baseline
		TGMrbLabelEntry * fTraceUPSAEnergyCutoffEntry; 	//				energy cutoff
		TGMrbLabelEntry * fTraceUPSATriggerThreshEntry; 	//			trigger threshold
		TGMrbLabelEntry * fTraceUPSAT90ThreshEntry; 	//					t90 threshold
		TGMrbLabelEntry * fTraceUPSALengthEntry; 	//					length
		TGMrbLabelEntry * fTraceUPSAOffsetEntry;	//					offset
		TGMrbLabelEntry * fTraceUPSATFAEnergyCutoffEntry;	//			TFA energy cutoff
		TGGroupFrame * fStatRegFrame;				//			status regs
		TGGroupFrame * fStatRegEntryFrame;			//
		TGMrbLabelEntry * fStatRegModICSREntry; 	//				switchbus
		TMrbNamedX * fModICSREditButton;			//
		TGMrbLabelEntry * fStatRegChanCSRAEntry; 	//				channel csra
		TMrbNamedX * fChanCSRAEditButton;			//
		TGMrbLabelEntry * fStatCoincPatternEntry; 	//				coinc pattern
		TMrbNamedX * fCoincPatternEditButton;		//
		TGMrbLabelEntry * fStatRegUserPsaCSREntry; 	//				user psa
		TMrbNamedX * fUserPsaCSREditButton;			//
		TGMrbLabelEntry * fStatRunTaskEntry; 		//				runtask
		TMrbNamedX * fRunTaskEditButton;			//

		TGVerticalFrame * fRightFrame; 				//		right row
		TGGroupFrame * fDACFrame;					//			DAC
		TGGroupFrame * fDACGainFrame;				//				gain
		TGMrbLabelEntry * fDACGainEntry; 			//					DAC
		TGMrbLabelEntry * fDACVVEntry;			  	//					V/V
		TGGroupFrame * fDACOffsetFrame; 			//				offset
		TGMrbLabelEntry * fDACOffsetEntry;		 	//					DAC
		TGMrbLabelEntry * fDACVoltEntry;		  	//					Volt
		TGGroupFrame * fCFDFrame;					//			CFD
		TGGroupFrame * fCFDDataFrame;				//				data
		TGMrbLabelEntry * fCFDRegEntry; 		  	//
		TGMrbLabelEntry * fCFDDelayBeforeLEEntry;	//
		TGMrbLabelEntry * fCFDDelayBipolarEntry;	//
		TGMrbLabelEntry * fCFDWalkEntry;		  	//
		TGMrbLabelEntry * fCFDFractionEntry;	  	//
		TGMrbRadioButtonList * fCFDOnOffButton;
		TGMrbRadioButtonList * fCFDFractionButton;
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
		TMrbLofNamedX fInstrUPSAOnOff;
		TMrbLofNamedX fInstrCFDOnOff;
		TMrbLofNamedX fInstrCFDFraction;

		TGMrbFocusList fFocusList;

	ClassDef(DGFInstrumentPanel, 0) 	// [DGFControl] Edit module/channel settings
};

#endif
