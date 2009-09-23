#ifndef __DGFInstrumentPanel_h__
#define __DGFInstrumentPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFInstrumentPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFInstrumentPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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

#include "TNGMrbTextButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"

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
		void EntryChanged(Int_t FrameId);
		void SelectModule(Int_t Selection);
		void RadioButtonPressed(Int_t FrameId, Int_t Selection);
		void EditBits(Int_t ButtonId);
		void MoveFocus(Int_t EntryId);
		inline void EditBits(Int_t ButtonId, Int_t Button){ EditBits(ButtonId);};

	protected:
		Bool_t WriteDSP(DGFModule * Module, Int_t ChannelId);					// update DSP
		Bool_t ReadDSP(DGFModule * Module, Int_t ChannelId);					// read DSP
		Bool_t ShowModuleSettings();											// show settings
		Bool_t UpdateValue(Int_t EntryId, Int_t ModuleId, Int_t ChannelId);		// update entry value

											// move focus to next entry
		Bool_t SetGFLT(Bool_t OnFlag = kTRUE);									// set/clear gflt
		Bool_t UpdateCFD(TMrbDGF * Module, Int_t Channel);						// update cfd
		Bool_t InitializeCFD(Int_t OnOff, Int_t Fraction);						// init cfd
		Bool_t UpdateUPSA(TMrbDGF * Module, Int_t Channel);						// update user psa
		Bool_t InitializeUPSA();												// init upsa

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TNGMrbLabelCombo * fSelectModule; 			//		module
		TNGMrbRadioButtonList * fSelectChannel;		//		channel

		TGHorizontalFrame * fInstrFrame;			// horiz frame containing 2 rows
		TGVerticalFrame * fLeftFrame; 				//  	left row
		TGGroupFrame * fFilterFrame;				//			filter
		TGGroupFrame * fEnergyFilterFrame;			//				energy
		TNGMrbLabelEntry * fEnergyPeakTimeEntry; 	//					peak time
		TNGMrbLabelEntry * fEnergyGapTimeEntry;  	//					gap time
		TNGMrbLabelEntry * fEnergyAveragingEntry;	//					average
		TNGMrbLabelEntry * fEnergyTauEntry;			//					tau value
		TGGroupFrame * fTriggerFilterFrame;			//				trigger
		TNGMrbLabelEntry * fTriggerPeakTimeEntry; 	//					peak time
		TNGMrbLabelEntry * fTriggerGapTimeEntry;  	//					gap time
		TNGMrbLabelEntry * fTriggerThresholdEntry;	//					threshold
		TGGroupFrame * fTraceFrame;					//			trace
		TGVerticalFrame * fTraceLeftFrame;
		TGVerticalFrame * fTraceRightFrame;
		TGGroupFrame * fTraceLengthDelayFrame;		//				length & delay
		TNGMrbLabelEntry * fTraceLengthEntry;	  	//					length
		TNGMrbLabelEntry * fTraceDelayEntry; 		//					delay
		TGGroupFrame * fTraceXPSAFrame;				//				xia-psa
		TNGMrbLabelEntry * fTraceXPSAOffsetEntry;	//					offset
		TNGMrbLabelEntry * fTraceXPSALengthEntry; 	//					length
		TGGroupFrame * fTraceUPSAFrame;				//				user-psa
		TNGMrbRadioButtonList * fTraceUPSAOnOffButton;	//				on/off
		TNGMrbLabelEntry * fTraceUPSABaselineEntry; 	//					baseline
		TNGMrbLabelEntry * fTraceUPSAEnergyCutoffEntry; 	//				energy cutoff
		TNGMrbLabelEntry * fTraceUPSATriggerThreshEntry; 	//			trigger threshold
		TNGMrbLabelEntry * fTraceUPSAT90ThreshEntry; 	//					t90 threshold
		TNGMrbLabelEntry * fTraceUPSALengthEntry; 	//					length
		TNGMrbLabelEntry * fTraceUPSAOffsetEntry;	//					offset
		TNGMrbLabelEntry * fTraceUPSATFAEnergyCutoffEntry;	//			TFA energy cutoff
		TGGroupFrame * fStatRegFrame;				//			status regs
		TGGroupFrame * fStatRegEntryFrame;			//
		TNGMrbLabelEntry * fStatRegModICSREntry; 	//				switchbus
		TMrbNamedX * fModICSREditButton;			//
		TNGMrbLabelEntry * fStatRegChanCSRAEntry; 	//				channel csra
		TMrbNamedX * fChanCSRAEditButton;			//
		TNGMrbLabelEntry * fStatCoincPatternEntry; 	//				coinc pattern
		TMrbNamedX * fCoincPatternEditButton;		//
		TNGMrbLabelEntry * fStatRegUserPsaCSREntry; 	//				user psa
		TMrbNamedX * fUserPsaCSREditButton;			//
		TNGMrbLabelEntry * fStatRunTaskEntry; 		//				runtask
		TMrbNamedX * fRunTaskEditButton;			//

		TGVerticalFrame * fRightFrame; 				//		right row
		TGGroupFrame * fDACFrame;					//			DAC
		TGGroupFrame * fDACGainFrame;				//				gain
		TNGMrbLabelEntry * fDACGainEntry; 			//					DAC
		TNGMrbLabelEntry * fDACVVEntry;			  	//					V/V
		TGGroupFrame * fDACOffsetFrame; 			//				offset
		TNGMrbLabelEntry * fDACOffsetEntry;		 	//					DAC
		TNGMrbLabelEntry * fDACVoltEntry;		  	//					Volt
		TGGroupFrame * fCFDFrame;					//			CFD
		TGGroupFrame * fCFDDataFrame;				//				data
		TNGMrbLabelEntry * fCFDRegEntry; 		  	//
		TNGMrbLabelEntry * fCFDDelayBeforeLEEntry;	//
		TNGMrbLabelEntry * fCFDDelayBipolarEntry;	//
		TNGMrbLabelEntry * fCFDWalkEntry;		  	//
		TNGMrbLabelEntry * fCFDFractionEntry;	  	//
		TNGMrbRadioButtonList * fCFDOnOffButton;
		TNGMrbRadioButtonList * fCFDFractionButton;
		TGGroupFrame * fMCAFrame;					//			MCA
		TGGroupFrame * fMCAEnergyFrame;				//				energy
		TNGMrbLabelEntry * fMCAEnergyEntry; 			//					energy
		TNGMrbLabelEntry * fMCAEnergyBinsEntry;		//					combine bins
		TGGroupFrame * fMCABaselineFrame;			//				baseline
		TNGMrbLabelEntry * fMCABaselineDCEntry;		//					DC
		TNGMrbLabelEntry * fMCABaselineBinsEntry;	//					combine bins

		TNGMrbTextButtonGroup * fModuleButtonFrame;	// buttons

		TGLabel * fDummyLabel;

		TMrbLofNamedX fLofModuleKeys;				//! ... key list
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TMrbLofNamedX fInstrChnActions;				// list of actions
		TMrbLofNamedX fInstrModuleActions;
		TMrbLofNamedX fInstrUPSAOnOff;
		TMrbLofNamedX fInstrCFDOnOff;
		TMrbLofNamedX fInstrCFDFraction;

		TNGMrbFocusList fFocusList;

	ClassDef(DGFInstrumentPanel, 0) 	// [DGFControl] Edit module/channel settings
};

#endif
