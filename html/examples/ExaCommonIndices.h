#ifndef __ExaCommonIndices_h__
#define __ExaCommonIndices_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           ExaCommonIndices.h
// Purpose:        Index declarations common to analysis and readout
// Description:    Defines a set of indices which should be used in analysis
//                 as well as readout code to guarantee coherent addresses
//
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

enum EMrbTriggerId	{
						kMrbTriggerReadout = 1,		// event readout, trigger 1
						kMrbTriggerStartAcquisition = 14,	// start acquisition, trigger 14
						kMrbTriggerStopAcquisition = 15 	// stop acquisition, trigger 15
					};

enum EMrbSevtSerial {
 						kMrbSevtData = 1,				// subevent data, unique serial number
						kMrbSevtDeadTime = 998, 			// dead time, serial=998
						kMrbSevtTimeStamp = 999, 			// time stamp, serial=999
						kMrbSevtDummy = 888 	 			// dummy subevent created by MBS internally, serial=888
					};

enum EMrbSevtBits	 {
 						kMrbSevtBitData = 0x1,		// subevent data
					};


enum EMrbModuleSerial {
						kMrbModuleAdc1 = 1,			// module adc1 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleAdc2 = 2,			// module adc2 (Silena 4418/V ADC 8 x 4096)
						kMrbModuleDtsca = 3,			// module dtsca (UCT Universal Timer/Counter N.Franz TUM)
					};
#endif
