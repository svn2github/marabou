#ifndef __GamsCommonIndices_h__
#define __GamsCommonIndices_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           GamsCommonIndices.h
// Purpose:        Index declarations common to analysis and readout
// Description:    Defines a set of indices which should be used in analysis
//                 as well as readout code to guarantee coherent addresses
//
// Author:         gams
// Revision:         
// Date:           Mon Aug 13 12:29:13 2007
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

enum EMrbSevt_10_12	{	kMrbSevt_10_12_B_Header 		=	(0x1 << 15),
						kMrbSevt_10_12_M_ModuleNumber	=	0xFF,
						kMrbSevt_10_12_M_ModuleId		=	0x7F,
						kMrbSevt_10_12_SH_ModuleId		=	8
					};

enum EMrbTriggerId	{
						kMrbTriggerDet = 1,		// event det, trigger 1
						kMrbTriggerStartAcquisition = 14,	// start acquisition, trigger 14
						kMrbTriggerStopAcquisition = 15 	// stop acquisition, trigger 15
					};

enum EMrbSevtSerial {
 						kMrbSevtDet = 1,				// subevent det, unique serial number
						kMrbSevtDeadTime = 998, 			// dead time, serial=998
						kMrbSevtTimeStamp = 999, 			// time stamp, serial=999
						kMrbSevtDummy = 888 	 			// dummy subevent created by MBS internally, serial=888
					};

enum EMrbSevtBits	 {
 						kMrbSevtBitDet = 0x1,		// subevent det
					};


enum EMrbModuleID {
						kMrbModIdSil_4418v = 0x1001,			// type Sil_4418v (Silena 4418/V ADC 8 x 4096)
						kMrbModIdUct = 0x10003,			// type Uct (UCT Universal Timer/Counter N.Franz TUM)
					};

enum EMrbModuleSerial {
						kMrbModuleAdc1 = 1,			// module adc1 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleAdc2 = 2,			// module adc2 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleAdc3 = 3,			// module adc3 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleAdc4 = 4,			// module adc4 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleSca1 = 5,			// module sca1 (UCT Universal Timer/Counter N.Franz TUM)
						kMrbModuleSca2 = 6,			// module sca2 (UCT Universal Timer/Counter N.Franz TUM)
						kMrbModuleSca3 = 7,			// module sca3 (UCT Universal Timer/Counter N.Franz TUM)
					};


#endif
