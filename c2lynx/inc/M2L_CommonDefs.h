#ifndef __M2L_CommonDefs_h__
#define __M2L_CommonDefs_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Common definitions (#define, enums)                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//////////////////////////////////////////////////////////////////////////

	enum ESis3302	{	kSis3302NofAdcs				= 8,
						kSis3302AllAdcs 			= -1,
						// DAC register
						kSis3302DacCmdLoadShiftReg	= 0x1,
						kSis3302DacCmdLoad 			= 0x2,
						kSis3302DacBusy				= 0x8000,
						// KEY commands
						kSis3302KeyReset			= 1,
						kSis3302KeyResetSample,
						kSis3302KeyDisarmSample,
						kSis3302KeyTrigger,
						kSis3302KeyClearTimestamp,
						kSis3302KeyArmBank1Sampling,
						kSis3302KeyArmBank2Sampling,
						// header
						kSis3302HeaderMask	 		= 0x7FFF,
						// trigger mode
						kSis3302TriggerOff			= 0x0,
						kSis3302TriggerIntern		= 0x1,
						kSis3302TriggerExtern		= 0x2,	
						kSis3302TriggerBoth 		= 0x3,
						// polarity
						kSis3302PolarityPositive	= 0x0,
						kSis3302PolarityNegative	= 0x1,
						// end addr thresh
						kSis3302EndAddrThreshMax	= 0xFFFFF4,
						// pre trigger delay and trigger gate length
						kSis3302PreTrigDelayMin 	= 0,
						kSis3302PreTrigDelayMax 	= 1023,
						kSis3302TrigGateLengthMin 	= 1,
						kSis3302TrigGateLengthMax 	= 1024,
					};

#endif
