#ifndef __TSnkDDACommon_h__
#define __TSnkDDACommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDACommon
// Purpose:        Common defs for the DDA0816
// Description:    Definitions common for all DDA0816 programs
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define kSnkMaxInterval				100000.

enum 					{	kSnkMaxAmplitude		=	2048 	};
enum					{	kSnkOffset0				=	2055	};
enum 					{	kSnkMaxRange			=	kSnkMaxAmplitude * 2 	};
enum 					{	kSnkMaxPoints			=	kSnkMaxAmplitude * 4 	};
enum 					{	kSnkDefaultPort			=	9010 	};
enum 					{	kSnkWaitForServer		=	15	 	};

// number of DDA modules
enum 					{	kSnkNofSubdevs 			=	4				 	};
enum 					{	kSnkNofChnPerSubdev 	=	4				 	};

enum ESnkDacChannels	{	kDacX					= 0,
							kDacY					= 1,
							kDacH					= 2,
							kDacB					= 3
						};

// Must be same as defined in dda0816_P.h !!!
enum ESnkPreScale		{	kSnkPSUndef				=	BIT(10),
							kSnkPS1					=	0,
							kSnkPS10				=	BIT(5),
							kSnkPS100				=	BIT(6),
							kSnkPS1k				=	BIT(6) | BIT(5),
							kSnkPS10k				=	BIT(7),
							kSnkPS100k				=	BIT(7) | BIT(5),
							kSnkPSExt				=	BIT(11)
						};


// Must be same as defined in dda0816_iomap.h !!!
enum ESnkClockSource 	{	kSnkCSUndef				=	-1,
							kSnkCSOFF		 		=	0x0,
							kSnkCSONB		 		=	0x1,
							kSnkCSEXT				=	0x2,
							kSnkCSSOFT 				=	0x3
						};

enum ESnkScanMode		{	kSnkSMUndef				=	BIT(0),
							kSnkSMSymm 				=	BIT(1),
							kSnkSMAsymm				=	BIT(2),
							kSnkSMLShape			=	BIT(3),
							kSnkSMXOnly				=	BIT(4),
							kSnkSMYOnly				=	BIT(5),
							kSnkSMXYConst			=	BIT(6),
							kSnkSMFromFile 			=	BIT(7)
						};

enum ESnkExecMode		{	kSnkEMNormal			=	BIT(0),
							kSnkEMVerbose			=	BIT(1),
							kSnkEMDebug				=	BIT(2),
							kSnkEMOffline 			=	BIT(3)
						};

enum ESnkScanProfile	{	kSnkSPUndef				=	BIT(0),
							kSnkSPTriangle 			=	BIT(1),
							kSnkSPSawToothLR		=	BIT(2),
							kSnkSPSawToothRL		=	BIT(3),
							kSnkSPConstant 			=	BIT(4),
						};

enum ESnkHysteresis 	{	kSnkHystLow				=	2300,
							kSnkHystHigh			=	3000
						};

enum ESnkBeamOnOff		{	kSnkBeamOff				=	2300,
							kSnkBeamOn				=	3000
						};

enum ESnkAction 		{	kSnkActionUndef 		=	-1,
							kSnkActionConnect		=	1,
							kSnkActionDisconnect,
							kSnkActionStartScan,
							kSnkActionStopScan,
							kSnkActionKillServer,
							kSnkActionSendData,
							kSnkActionReset,
							kSnkActionGetStatus
						};

enum ESnkMsgIdx 	 	{	kSnkMsgIdxType,
							kSnkMsgIdxHost,
							kSnkMsgIdxPort,
							kSnkMsgIdxPid,
							kSnkMsgIdxText,
							kSnkMsgIdxProgram,
							kSnkMsgIdxAction,
							kSnkMsgIdxExecMode,
							kSnkMsgIdxInterval,
							kSnkMsgIdxPath,
							kSnkMsgIdxScanFile
						};

//_________________________________________________________________________________________________________ prescale frequencies

const SMrbNamedX kSnkLofPreScales[] =
							{
								{kSnkPSUndef, 	"undef",	"Pixel frequency undefined"			},
								{kSnkPS1, 		"1Hz",		"Pixel frequency 1Hz"				},
								{kSnkPS10,		"10Hz",		"Pixel frequency 10Hz"				},
								{kSnkPS100,		"100Hz",	"Pixel frequency 100Hz" 			},
								{kSnkPS1k,		"1kHz",		"Pixel frequency 1kHz"				},
								{kSnkPS10k, 	"10kHz",	"Pixel frequency 10kHz" 			},
								{kSnkPS100k,	"100kHz",	"Pixel frequency 100kHz"			},
								{kSnkPSExt, 	"extern",	"EXTERNAL clock source"				},
								{0, 			NULL,		NULL								}
							};

//_________________________________________________________________________________________________________ clock sources

const SMrbNamedXShort kSnkLofClockSources[] =
							{
								{kSnkCSUndef, 	"undef" 			},
								{kSnkCSOFF,		"off"				},
								{kSnkCSONB,		"onboard"			},
								{kSnkCSEXT,		"external"			},
								{kSnkCSSOFT,	"software"			},
								{0, 			NULL				}
							};

//_________________________________________________________________________________________________________ wave forms

const SMrbNamedX kSnkLofScanProfiles[] =
							{
								{kSnkSPUndef,		"undef",			"Scan profile undefined" 				},
								{kSnkSPTriangle,	"triangular",		"Scan profile: triangular"				},
								{kSnkSPSawToothLR,	"sawtooth-left",	"Scan profile: sawtooth, ///" 			},
								{kSnkSPSawToothRL,	"sawtooth-right",	"Scan profile: sawtooth, \\\\\\"		},
								{kSnkSPConstant,	"constant",			"Scan profile: data remain constant"	},
								{0, 				NULL,				NULL									}
							};

//_________________________________________________________________________________________________________ scan modes

const SMrbNamedX kSnkLofScanModes[] =
							{
								{kSnkSMUndef,	 	"undef",			"Scan mode: undefined" 				},
								{kSnkSMSymm,		"symm", 			"Scan mode: X,Y symmectric"			},
								{kSnkSMAsymm, 		"asymm",			"Scan mode: X,Y individual"			},
								{kSnkSMLShape,  	"L shape",			"Scan mode: L-shaped scan" 			},
								{kSnkSMXOnly, 		"along X",			"Scan mode: X data vary, Y=const"	},
								{kSnkSMYOnly,   	"along Y",			"Scan mode: Y data vary, X=const"	},
								{kSnkSMXYConst,		"(X,Y)=c",			"Scan mode: X=const and Y=const"	},
								{kSnkSMFromFile,	"file", 			"Scan mode: X,Y taken from file"	},
								{0, 				NULL,				NULL								}
							};

//_________________________________________________________________________________________________________ exec modes

const SMrbNamedX kSnkLofExecModes[] =
							{
								{kSnkEMNormal,		"normal",		"OutputMode: normal"				},
								{kSnkEMVerbose, 	"verbose",		"OutputMode: verbose"				},
								{kSnkEMDebug,		"debug",		"OutputMode: debug"					},
								{kSnkEMOffline, 	"offline",		"OutputMode: no DDA access"			},
								{0, 				NULL,			NULL								}
							};
										
//_________________________________________________________________________________________________________ actions

const SMrbNamedX kSnkLofActions[] =
							{
								{kSnkActionUndef,	 	"undef",		"undefined"				 			},										
								{kSnkActionConnect, 	"connect",		"Connect to server" 				},										
								{kSnkActionDisconnect, 	"disconnect",	"Disconnect from server" 			},										
								{kSnkActionStartScan, 	"startScan",	"Start DDA scan" 					},										
								{kSnkActionStopScan, 	"stopScan",		"Stop DDA scan" 	 				},										
								{kSnkActionKillServer, 	"killServer",	"Kill DDA server" 	 				},										
								{kSnkActionSendData, 	"sendData",		"Send data to server" 			 	},										
								{kSnkActionReset, 		"reset"	,		"Reset" 							},										
								{kSnkActionGetStatus, 	"status",		"Get status from server" 			},										
								{0, 					NULL,			NULL								}
							};
#endif
