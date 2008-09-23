#ifndef __VMESis3302SettingsPanel_h__
#define __VMESis3302SettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SettingsPanel.h
// Purpose:        Class defs for VME modules
// Class:          VMESis3302SettingsPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302SettingsPanel.h,v 1.1 2008-09-23 10:47:20 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-09-23 10:47:20 $
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

#include "Signal.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SettingsPanel
// Purpose:        Panel to control a Sis3302 adc
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMESis3302SettingsPanel : public TGCompositeFrame {

	public:
		enum EVMESis3302CmdId 		{
										kVMESis3302SelectModule,
										kVMESis3302SelectChannel,
										kVMESis3302ClockSource,
										kVMESis3302CommonTrigMode,
										kVMESis3302ExtLemo,
										kVMESis3302DacOffset,
										kVMESis3302TrigMode,
										kVMESis3302TrigPeaking,
										kVMESis3302TrigPol,
										kVMESis3302TrigGap,
										kVMESis3302TrigSumG,
										kVMESis3302TrigThresh,
										kVMESis3302TrigCounts,
										kVMESis3302TrigOut,
										kVMESis3302TrigCond,
										kVMESis3302TrigDelay,
										kVMESis3302TrigGate,
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
									};

		enum EVMESis3302Clock		{
										kVMEClockSource100	=	0,
										kVMEClockSource50	=	1,
										kVMEClockSource25	=	2,
										kVMEClockSource10	=	3,
										kVMEClockSource1	=	4,
										kVMEClockSourceRes	=	5,
										kVMEClockSourceExt	=	6,
										kVMEClockSourceP2	=	7
									};


		enum EVMESis3302TrigMode	{
										kVMETrigModeDis 	=	0,
										kVMETrigModeIntern	=	BIT(0),
										kVMETrigModeExtern	=	BIT(1),
										kVMETrigModeBoth	=	kVMETrigModeIntern | kVMETrigModeExtern,
										kVMETrigModeLemo2	=	BIT(2),
										kVMETrigModeLemo3	=	BIT(3),
									};

		enum EVMESis3302TrigPol		{
										kVMETrigPolPos		=	BIT(0),
										kVMETrigPolNeg		=	BIT(1),
									};

		enum EVMESis3302TrigCond	{
										kVMETrigCondDis		=	BIT(0),
										kVMETrigCondGTEna	=	BIT(1),
										kVMETrigCondGTDis	=	BIT(2),
										kVMETrigCondEna		=	BIT(3),
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

		enum						{	kVMENofChannels 	=	8	};

	public:
		VMESis3302SettingsPanel(TGCompositeFrame * TabFrame, TMrbLofNamedX * LofModules);
		virtual ~VMESis3302SettingsPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		void UpdateGUI();

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelChanged(Int_t FrameId, Int_t Selection);
		void ClockSourceChanged(Int_t FrameId, Int_t Selection);
		void CommonTrigModeChanged(Int_t FrameId, Int_t Selection);
		void DacOffsetChanged(Int_t FrameId, Int_t Selection);
		void TrigModeChanged(Int_t FrameId, Int_t Selection);
		void TrigPeakingChanged(Int_t FrameId, Int_t Selection);
		void TrigGapChanged(Int_t FrameId, Int_t Selection);
		void TrigPolarChanged(Int_t FrameId, Int_t Selection);
		void TrigThreshChanged(Int_t FrameId, Int_t Selection);
		void TrigOutChanged(Int_t FrameId, Int_t Selection);
		void TrigCondChanged(Int_t FrameId, Int_t Selection);
		void TrigDelayChanged(Int_t FrameId, Int_t Selection);
		void EnergyPeakingChanged(Int_t FrameId, Int_t Selection);
		void DecimationChanged(Int_t FrameId, Int_t Selection);
		void EnergyGapChanged(Int_t FrameId, Int_t Selection);
		void TauFactorChanged(Int_t FrameId, Int_t Selection);
		void RawDataStartChanged(Int_t FrameId, Int_t Selection);
		void RawDataLengthChanged(Int_t FrameId, Int_t Selection);
		void EnergyDataModeChanged(Int_t FrameId, Int_t Selection);
		void EnergyDataStart1Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataStart2Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataStart3Changed(Int_t FrameId, Int_t Selection);
		void EnergyDataLengthChanged(Int_t FrameId, Int_t Selection);

	protected:
		void UpdateAdcCounts();
		void UpdateGates();
		void UpdateDecayTime();
		void EnergyDataStartChanged(Int_t IdxNo, TGMrbLabelEntry * Entry, Int_t EntryNo);

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbLabelCombo * fSelectChannel;  			//		channel
		TGGroupFrame * fSettingsFrame; 				// settings
		TGGroupFrame * fClockAndTrig; 				//		clock and trigger
		TGMrbLabelCombo * fClockSource;  			//			clock
		TGMrbCheckButtonList * fCommonTrigMode; 	//			common trigger mode

		TGGroupFrame * fDacSettings; 		 		//		dac
		TGMrbLabelEntry * fDacOffset;  				//			offset

		TGGroupFrame * fTriggerFilter; 				//		trigger filter
		TGMrbLabelCombo * fTrigMode;  				//			mode
		TGMrbLabelEntry * fTrigPeaking;  			//			peaking
		TGMrbLabelEntry * fTrigGap; 	  			//			gap
		TGMrbLabelEntry * fTrigThresh; 	  			//			threshold
		TGMrbLabelCombo * fTrigPol; 	 			//			polarity
		TGMrbLabelEntry * fTrigOut; 	  			//			output length
		TGMrbLabelCombo * fTrigCond; 	 			//			condition
		TGMrbLabelEntry * fTrigSumG; 	  			//			[derived] sumG
		TGMrbLabelEntry * fTrigCounts; 	  			//			[derived] adc counts

		TGGroupFrame * fEnergyFilter; 				//		energy filter
		TGMrbLabelEntry * fEnergyPeaking;  			//			peaking
		TGMrbLabelCombo * fEnergyDecimation;  		//			decimation
		TGMrbLabelEntry * fEnergyGap; 	  			//			gap
		TGMrbLabelEntry * fPreTrigDelay; 	  		//			pretrig delay
		TGMrbLabelEntry * fEnergyTauFactor; 		//			tau
		TGMrbLabelEntry * fEnergyDecayTime; 		//			[derived] decay time
		TGMrbLabelEntry * fEnergyGate; 		  		//			[derived] energy gate
		TGMrbLabelEntry * fTrigGate; 		  		//			[derived] trigger gate

		TGGroupFrame * fRawDataSampling; 			//		raw data sampling
		TGMrbLabelEntry * fRawDataStart;			//			start index
		TGMrbLabelEntry * fRawDataLength;			//			length

		TGGroupFrame * fEnergyDataSampling; 		//		energy data sampling
		TGMrbLabelCombo * fEnergyDataMode;  		//			mode
		TGMrbLabelEntry * fEnergyDataStart1;		//			start index 1
		TGMrbLabelEntry * fEnergyDataStart2;		//			start index 2
		TGMrbLabelEntry * fEnergyDataStart3;		//			start index 3
		TGMrbLabelEntry * fEnergyDataLength;		//			length

		TMrbLofNamedX * fLofModules;

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302SettingsPanel, 0) 	// [VMEControl] Panel to control a Sis3302 adc
};

#endif
