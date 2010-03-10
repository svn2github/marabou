#ifndef __VMESis3302Panel_h__
#define __VMESis3302Panel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel.h
// Purpose:        Class defs for VME modules
// Class:          VMESis3302Panel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302Panel.h,v 1.6 2010-03-10 12:08:11 Rudolf.Lutter Exp $
// Date:           $Date: 2010-03-10 12:08:11 $
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTextView.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

#include "M2L_CommonDefs.h"
#include "VMEControlData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        Panel to control a Sis3302 adc
// Constructors:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMESis3302Panel : public TGCompositeFrame {

	public:
		enum EVMESis3302CmdId 		{
										kVMESis3302SelectModule,
										kVMESis3302SelectChannel,
										kVMESis3302ClockSource,
										kVMESis3302LemoInMode,
										kVMESis3302LemoOutMode,
										kVMESis3302LemoInEnableMask,
										kVMESis3302DacOffset,
										kVMESis3302TrigMode,
										kVMESis3302GateMode,
										kVMESis3302NNTrigMode,
										kVMESis3302NNGateMode,
										kVMESis3302TrigPeaking,
										kVMESis3302TrigGap,
										kVMESis3302TrigSumG,
										kVMESis3302TrigIntGate,
										kVMESis3302TrigIntDelay,
										kVMESis3302TrigPol,
										kVMESis3302TrigThresh,
										kVMESis3302TrigCounts,
										kVMESis3302TrigOut,
										kVMESis3302TrigCond,
										kVMESis3302TrigDelay,
										kVMESis3302TrigGate,
										kVMESis3302TrigDecimation,
										kVMESis3302EnergyPeaking,
										kVMESis3302EnergyDecimation,
										kVMESis3302EnergyGap,
										kVMESis3302EnergyGate,
										kVMESis3302EnergyTauFactor,
										kVMESis3302EnergyDecayTime,
										kVMESis3302RawDataStart,
										kVMESis3302RawDataLength,
										kVMESis3302EnergyDataMode,
										kVMESis3302EnergyDataStart1,
										kVMESis3302EnergyDataStart2,
										kVMESis3302EnergyDataStart3,
										kVMESis3302EnergyDataLength,
										kVMESis3302ActionReset,
										kVMESis3302ActionRun,
										kVMESis3302ActionSaveRestore,
										kVMESis3302ActionCopySettings,
									};

		enum EVMESis3302Clock		{
										kVMEClockSource100MHzA	=	0,
										kVMEClockSource50MHz	=	1,
										kVMEClockSource25MHz	=	2,
										kVMEClockSource10MHz	=	3,
										kVMEClockSource1MHz		=	4,
										kVMEClockSource100kHz	=	5,
										kVMEClockSourceExt		=	6,
										kVMEClockSource100MHzB	=	7
									};


		enum EVMESis3302TrigGateMode	{
										kVMETrigGateModeDis 	=	0,
										kVMETrigGateModeIntern	=	BIT(0),
										kVMETrigGateModeExtern	=	BIT(1),
										kVMETrigGateModeBoth	=	kVMETrigGateModeIntern | kVMETrigGateModeExtern,
									};

		enum EVMESis3302NextNeighborMode	{
										kVMENextNeighborModeDis 	=	0,
										kVMENextNeighborModeR		=	BIT(0),
										kVMENextNeighborModeL		=	BIT(1),
										kVMENextNeighborModeBoth	=	kVMENextNeighborModeR | kVMENextNeighborModeL,
									};

		enum EVMESis3302TrigPol		{
										kVMETrigPolPos		=	BIT(0),
										kVMETrigPolNeg		=	BIT(1),
									};

		enum EVMESis3302TrigCond	{
										kVMETrigCondEnaGT	=	BIT(0),
										kVMETrigCondDis		=	BIT(1),
										kVMETrigCondEna		=	0,
									};

		enum EVMESis3302Decim	{
										kVMEDecimDis		=	0,
										kVMEDecim2,
										kVMEDecim4,
										kVMEDecim8,
									};

		enum EVMESis3302Sample	{
										kVMESampleFull		=	0,
										kVMESampleMinMax,
										kVMESampleProg,
									};

		enum						{	kVMENofSis3302Chans =	8	};

	public:
		VMESis3302Panel(TGCompositeFrame * TabFrame);
		virtual ~VMESis3302Panel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		void UpdateGUI();

		inline Int_t GetNofModules() { return(fLofModules.GetEntries()); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelChanged(Int_t FrameId, Int_t Selection);
		void ClockSourceChanged(Int_t FrameId, Int_t Selection);
		void LemoInModeChanged(Int_t FrameId, Int_t Selection);
		void LemoOutModeChanged(Int_t FrameId, Int_t Selection);
		void LemoInEnableMaskChanged(Int_t FrameId, Int_t Selection);
		void DacOffsetChanged(Int_t FrameId, Int_t Selection);
		void TrigModeChanged(Int_t FrameId, Int_t Selection);
		void GateModeChanged(Int_t FrameId, Int_t Selection);
		void NextNeighborTrigModeChanged(Int_t FrameId, Int_t Selection);
		void NextNeighborGateModeChanged(Int_t FrameId, Int_t Selection);
		void TrigPeakingChanged(Int_t FrameId, Int_t Selection);
		void TrigGapChanged(Int_t FrameId, Int_t Selection);
		void TrigInternalGateChanged(Int_t FrameId, Int_t Selection);
		void TrigInternalDelayChanged(Int_t FrameId, Int_t Selection);
		void TrigDecimationChanged(Int_t FrameId, Int_t Selection);
		void TrigPolarChanged(Int_t FrameId, Int_t Selection);
		void TrigThreshChanged(Int_t FrameId, Int_t Selection);
		void TrigDelayChanged(Int_t FrameId, Int_t Selection);
		void TrigOutChanged(Int_t FrameId, Int_t Selection);
		void TrigCondChanged(Int_t FrameId, Int_t Selection);
		void EnergyPeakingChanged(Int_t FrameId, Int_t Selection);
		void EnergyDecimationChanged(Int_t FrameId, Int_t Selection);
		void EnergyGapChanged(Int_t FrameId, Int_t Selection);
		void TauFactorChanged(Int_t FrameId, Int_t Selection);
		void RawDataStartChanged(Int_t FrameId, Int_t Selection);
		void RawDataLengthChanged(Int_t FrameId, Int_t Selection);
		void EnergyDataModeChanged(Int_t FrameId, Int_t Selection);
		void EnergyDataStart1Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataStart2Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataStart3Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataLengthChanged(Int_t FrameId, Int_t Selection);
		void ResetModule();

		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t SetupModuleList();
		void UpdateAdcCounts();
		void UpdateGates();
		void UpdateDecayTime();
		void EnergyDataStartOrLengthChanged(Int_t IdxNo, TGMrbLabelEntry * Entry, Int_t EntryNo);

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbLabelCombo * fSelectChannel;  			//		channel
		TGMrbLabelEntry * fModuleInfo;				// module id
		TGGroupFrame * fSettingsFrame; 				// settings
		TGGroupFrame * fClockLemo; 					//		clock, lemo in & out
		TGMrbLabelCombo * fClockSource;  			//			clock
		TGMrbLabelCombo * fLemoInMode; 	  			//			lemo in
		TGMrbLabelCombo * fLemoOutMode; 	  		//			lemo out
		TGMrbLabelEntry * fLemoInEnableMask; 	  	//			lemo in enable

		TGGroupFrame * fDacSettings; 		 		//		dac
		TGMrbLabelEntry * fDacOffset;  				//			offset

		TGGroupFrame * fTriggerSettings; 				//		trigger filter settings
		TGMrbLabelCombo * fTrigMode;  				//			trigger mode
		TGMrbLabelCombo * fGateMode;  				//			gate mode
		TGMrbLabelCombo * fNextNeighborTrigMode;  	//			next neighbor trigger mode
		TGMrbLabelCombo * fNextNeighborGateMode;  	//			... gate mode
		TGMrbLabelEntry * fTrigPeaking;  			//			peaking
		TGMrbLabelEntry * fTrigGap; 	  			//			gap
		TGMrbLabelEntry * fTrigInternalGate; 	  	//			internal gate
		TGMrbLabelEntry * fTrigInternalDelay; 	  	//			... delay
		TGMrbLabelCombo * fTrigDecimation; 	  		//			decimation
		TGMrbLabelEntry * fTrigThresh; 	  			//			threshold
		TGMrbLabelCombo * fTrigPol; 	 			//			polarity
		TGMrbLabelEntry * fTrigOut; 	  			//			output length
		TGMrbLabelCombo * fTrigCond; 	 			//			condition
		TGMrbLabelEntry * fTrigSumG; 	  			//			[derived] sumG
		TGMrbLabelEntry * fTrigCounts; 	  			//			[derived] adc counts

		TGGroupFrame * fEnergySettings; 			//		energy filter settings
		TGMrbLabelEntry * fEnergyPeaking;  			//			peaking
		TGMrbLabelCombo * fEnergyDecimation;  		//			decimation
		TGMrbLabelEntry * fEnergyGap; 	  			//			gap
		TGMrbLabelEntry * fPreTrigDelay; 	  		//			pretrig delay
		TGMrbLabelEntry * fEnergyTauFactor; 		//			tau
		TGMrbLabelEntry * fEnergyDecayTime; 		//			[derived] decay time
		TGMrbLabelEntry * fEnergyGateLength; 		//			[derived] energy gate
		TGMrbLabelEntry * fTrigGateLength; 		  	//			[derived] trigger gate

		TGGroupFrame * fRawDataSampling; 			//		raw data sampling
		TGMrbLabelEntry * fRawDataStart;			//			start index
		TGMrbLabelEntry * fRawDataLength;			//			length

		TGGroupFrame * fEnergyDataSampling; 		//		energy data sampling
		TGMrbLabelCombo * fEnergyDataMode;  		//			mode
		TGMrbLabelEntry * fEnergyDataStart1;		//			start index 1
		TGMrbLabelEntry * fEnergyDataStart2;		//			start index 2
		TGMrbLabelEntry * fEnergyDataStart3;		//			start index 3
		TGMrbLabelEntry * fEnergyDataLength;		//			length

		TGMrbTextButtonGroup * fActionButtons;	 	// actions

		TMrbLofNamedX fActions;

		TMrbLofNamedX fLofModules;

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302Panel, 0) 	// [VMEControl] Panel to control a Sis3302 adc
};

#endif
