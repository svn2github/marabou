#ifndef __M2L_CommonDefs_h__
#define __M2L_CommonDefs_h__

/*////////////////////////////////////////////////////////////////////////
//                                                                      //
// Common definitions (#define, enums)                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//                                                                      //
// !!!! This header file will be used for C++ *AND* C as well           //
// !!!! Please use C style comments only                                //
////////////////////////////////////////////////////////////////////////*/

	enum ESis3302	{			/* DAC register */
						kSis3302DacCmdLoadShiftReg		= 0x1,
						kSis3302DacCmdLoad 			= 0x2,
						kSis3302DacBusy				= 0x8000,
						/* KEY commands */
						kSis3302KeyReset			= 1,
						kSis3302KeyResetSampling,
						kSis3302KeyDisarmSampling,
						kSis3302KeyTrigger,
						kSis3302KeyClearTimestamp,
						kSis3302KeyArmBank1Sampling,
						kSis3302KeyArmBank2Sampling,
						kSis3302KeyResetDDR2Logic,
						kSis3302KeyMcaScanLNEPulse,
						kSis3302KeyMcaScanArm,
						kSis3302KeyMcaScanStart,
						kSis3302KeyMcaScanDisable,
						kSis3302KeyMcaMultiscanStartResetPulse,
						kSis3302KeyMcaMultiscanArmScanArm,
						kSis3302KeyMcaMultiscanArmScanEnable,
						kSis3302KeyMcaMultiscanDisable,
						/* header */
						kSis3302HeaderMask	 		= 0x7FFF,
						/* trigger mode */
						kSis3302GateOff				= 0x0,
						kSis3302GateIntern			= BIT(0),
						kSis3302GateExtern			= BIT(1),
						kSis3302GateBoth 			= kSis3302GateIntern | kSis3302GateExtern,
						kSis3302TriggerOff			= 0x0,
						kSis3302TriggerIntern		= BIT(0),
						kSis3302TriggerExtern		= BIT(1),
						kSis3302TriggerBoth 		= kSis3302TriggerIntern | kSis3302TriggerExtern,
						/* polarity */
						kSis3302PolarityPositive	= 0x0,
						kSis3302PolarityNegative	= 0x1,
						/* end addr thresh */
						kSis3302EndAddrThreshMax	= 0xFFFFF4,
						/* trigger internal delay and gate length */
						kSis3302TrigIntDelayMin		= 0,
						kSis3302TrigIntDelayMax		= ((1 << 5) - 1),
						kSis3302TrigIntGateMin		= 0,
						kSis3302TrigIntGateMax		= ((1 << 6) - 1),
						/* trigger peaking and gap */
						kSis3302TrigPeakMin 		= 1,
						kSis3302TrigPeakMax 		= ((1 << 10) - 1),
						kSis3302TrigSumGMin 		= 1,
						kSis3302TrigSumGMax 		= ((1 << 10) - 1),
						kSis3302TrigGapMin 			= 0,
						kSis3302TrigGapMax 			= (kSis3302TrigSumGMax - 1),
						/* trigger threshold */
						kSis3302TrigThreshMin		= 0,
						kSis3302TrigThreshMax		= ((1 << 16) - 1),
						/* trigger pulse out */
						kSis3302TrigPulseMin		= 0,
						kSis3302TrigPulseMax		= ((1 << 8) - 1),
						/* pre trigger delay and trigger gate length */
						kSis3302PreTrigDelayMin 	= 0,
						kSis3302PreTrigDelayMax 	= ((1 << 10) - 1),
						kSis3302TrigGateLengthMin 	= 1,
						kSis3302TrigGateLengthMax 	= (1 << 16),
						kSis3302RawDataStartIndexMin 	= 0,
						kSis3302RawDataStartIndexMax 	= ((1 << 16) - 2),
						kSis3302RawDataSampleLengthMin 	= 0,
						kSis3302RawDataSampleLengthMax 	= ((1 << 16) - 4),
						/* energy peaking and gap */
						kSis3302EnergyPeakMin 		= 1,
						kSis3302EnergyPeakMax 		= ((1 << 10) - 1),
						kSis3302EnergyGapMin 		= 0,
						kSis3302EnergyGapMax 		= ((1 << 8) - 1),
						kSis3302EnergySampleLengthMin 	= 0,
						kSis3302EnergySampleLengthMax 	= 510,
						kSis3302EnergyTauMin 		= 0,
						kSis3302EnergyTauMax 		= ((1 << 6) - 1),
					};

	enum			{	kSis3302NofChans		= 8 };
	enum			{	kSis3302NofGroups		= kSis3302NofChans/2 };
	enum			{	kSis3302AllChans 		= -1 };
	enum			{	kSis3302ChanPattern 		= 0xFF };
	enum			{	kSis3302MaxEvents 		= -1 };
	enum			{	kSis3302EventPreHeader 		= 5 };
	enum			{	kSis3302EventHeader 		= 2 };
	enum			{	kSis3302EventMinMax 		= 2 };
	enum			{	kSis3302EventTrailer 		= 2 };

#ifdef	CPU_TYPE_RIO2
	enum			{	kSis3302SegSize		 		= 0x4000000 };
#endif
#ifdef	CPU_TYPE_RIO3
	enum			{	kSis3302SegSize		 		= 0x8000000 };
#endif

	enum			{	kSis3302StatusCollectingTraces	=	0x1 };
	enum			{	kSis3302MaxBufSize		=	0x20000	};

	enum EVulomTB	{	kVulomTBNofScalers			= 3,
						kVulomTBNofChannels 		= 16,
						kVulomTBAllScalers	 		= -1,
						kVulomTBAllChannels 		= -1
					};

#endif
