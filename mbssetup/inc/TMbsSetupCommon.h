#ifndef __TMbsSetupCommon_h__
#define __TMbsSetupCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsSetupCommon.h
// Purpose:        Define a MBS setup: Common defs
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsSetupCommon.h,v 1.8 2005-08-02 08:54:04 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

enum						{	kNofTriggers			=	16	};			// number of triggers
enum						{	kNofCrates				=	16	};			// ... crates
enum						{	kNofRdoProcs			=	16	};			// ... readout procs
enum						{	kNofPPCs				=	8	};			// ... ppcs

enum						{	kMinFCT 				=	20	};			// minimum FCT value

enum EMbsSetupMode			{ 	kModeSingleProc			=	1,				// setup modes
								kModeMultiProc
							};

enum EMbsProcType			{ 	kProcUndefined			=	0,				// processor types
								kProcCVC				=	1,
								kProcE6 				=	2,
								kProcE7 				=	3,
								kProcPPC				=	8,
								kProcRIO2				=	8,
								kProcRIO3				=	10
							};

enum EMbsControllerType		{ 	kControllerUndefined	=	0,				// crate controllers
								kControllerCVC			=	kProcCVC,
								kControllerE6			=	kProcE6,
								kControllerE7			=	kProcE7,
								kControllerAEB			=	4,
								kControllerCBV			=	5,
								kControllerCVI			=	6,
								kControllerCAV			=	7,
								kControllerPPC			=	kProcPPC,
								kControllerRIO2			=	kProcRIO2,
								kControllerRIO3			=	kProcRIO3,
								kControllerCC32 		=   11
							};

enum EMbsTriggerModuleType	{ 	kTriggerModuleUndefined =	0,			// trigger modules
								kTriggerModuleCAMAC 	=	1,
								kTriggerModuleVME		=	2
							};

enum EMbsTriggerMode		{ 	kTriggerModeLocalInterrupt		=	0,			// trigger modes
 								kTriggerModeLocalIntNonStdBase	=	1,
 								kTriggerModeVsbInterrupt		=	2,
								kTriggerModePolling 	 		=	3
							};

enum EMbsSetupTags			{	kSetHostName				=	1,
								kSetDate,
								kSetAuthor,
								kSetRdoNames,
								kSetHostFlag,
								kSetSetupPath,
								kSetPipeAddr,
								kSetBuffers,
								kLoadMLSetup,
								kLoadSPSetup,
								kStartReadout,
								kStartEvtBuilder,
								kSetDispRdo1,
								kStopReadout,
								kReloadReadout,
								kStopEvtBuilder,
								kSetDispEvb,
								kSetFlushTime,
								kSetMasterType,
								kSetRemMemoryBase,
								kSetRemMemoryOffset,
								kSetRemMemoryLength,
								kSetRemCamacBase,
								kSetRemCamacOffset,
								kSetRemCamacLength,
								kSetRemEsoneBase,
								kSetSevtSize,
								kSetRdoFlag,
								kSetControllerID,
								kSetTriggerModule,
								kSetTrigStation,
								kSetTrigFastClear,
								kSetTrigConvTime,
								kLoadRdoSetup,
								kSetIrqMode,
								kStartRdoTask,
								kTestHostName,
								kPrintBanner,
								kSetVsbAddr,
								kMbsLogin
							};

enum						{	kMbsV43 				= 43			};
enum						{	kRemMemoryBaseCBV		= 0xd0380000	};
enum						{	kRemMemoryBaseCC32RIO2	= 0xee550000	};
enum						{	kRemMemoryBaseCC32RIO3	= 0x7d550000	};
enum						{	kRemMemoryLengthCBV 	= 0x00200000	};
enum						{	kRemMemoryLengthCC32	= 0x00008000	};
enum						{	kRemMemoryLengthCC32v4x	= 0x00010000	};
enum						{	kTriggerModuleBaseRIO2	= 0xe2000000	};
enum						{	kTriggerModuleBaseRIO3	= 0x42000000	};

const SMrbNamedXShort kMbsLofSetupModes[] =		// list of legal setup modes
							{
								{kModeSingleProc,	"SingleProc"	},
								{kModeMultiProc,	"MultiProc" 	},
								{0, 				NULL			}
							};

const SMrbNamedXShort kMbsLofProcs[] =				// list of legal processors
							{
								{kProcCVC,			"CVC"			},
								{kProcPPC,			"PPC"			},
								{kProcRIO2,			"RIO2"			},
								{kProcRIO3,			"RIO3"			},
								{0, 				NULL			}
							};

const SMrbNamedXShort kMbsLofControllers[] =		// list of legal crate controllers
							{
								{kControllerCVC,	"CVC"			},
								{kControllerCBV,	"CBV"			},
								{kControllerPPC,	"PPC"			},
								{kControllerRIO2,	"RIO2"			},
								{kControllerRIO3,	"RIO3"			},
								{kControllerCC32,	"CC32"			},
								{0, 				NULL			}
							};

const SMrbNamedXShort kMbsLofTriggerModules[] = 	// list of legal trigger modules
							{
								{kTriggerModuleCAMAC,	"CAMAC" 	},
								{kTriggerModuleVME, 	"VME"		},
								{0, 					NULL		}
							};

const SMrbNamedX kMbsLofTriggerModes[] =		// list of legal trigger modes
							{
								{kTriggerModeLocalInterrupt,		"LOCAL-INT",		"Local Interrupt"							},
								{kTriggerModeLocalIntNonStdBase,	"LOCAL-INT-NONSTD",	"Local Interrupt, non-standard BaseAddr"	},
								{kTriggerModeVsbInterrupt,			"VSB-INTERRUPT",	"VSB Interrupt (not implemented)"			},
								{kTriggerModePolling,				"POLLING",			"Polling Mode (no interrupt)"				},
								{0, 								NULL		}
							};

const SMrbNamedXShort kMbsSetupTags[] =			// list of setup tag words
							{
								{kSetHostName,			"SET_HOST_NAME" 			},
								{kSetAuthor,			"AUTHOR" 				},
								{kSetDate,			"DATE" 					},
								{kSetRdoNames,			"SET_RDO_NAMES" 			},
								{kSetHostFlag,			"SET_HOST_FLAG" 			},
								{kSetSetupPath, 		"SET_SETUP_PATH"			},
								{kSetPipeAddr,			"SET_PIPE_ADDR" 			},
								{kSetBuffers,			"SET_BUFFERS"				},
								{kLoadMLSetup,			"LOAD_ML_SETUP" 			},
								{kLoadSPSetup,			"LOAD_SP_SETUP" 			},
								{kStartReadout, 		"START_READOUT" 			},
								{kStartEvtBuilder,		"START_EVTBUILDER"			},
								{kSetDispRdo1,			"SET_DISP_RDO1" 			},
								{kStopReadout,			"STOP_READOUT"				},
								{kReloadReadout,		"RELOAD_READOUT"			},
								{kStopEvtBuilder,		"STOP_EVTBUILDER"			},
								{kSetDispEvb,			"SET_DISP_EVB"				},
								{kSetFlushTime, 		"SET_FLUSHTIME" 			},
								{kSetMasterType,		"SET_MASTER_TYPE"			},
								{kSetRemMemoryBase,		"SET_REM_MEMORY_BASE"		},
								{kSetRemMemoryOffset,	"SET_REM_MEMORY_OFFSET"		},
								{kSetRemMemoryLength,	"SET_REM_MEMORY_LENGTH"		},
								{kSetRemCamacBase,		"SET_REM_CAMAC_BASE"		},
								{kSetRemCamacOffset,	"SET_REM_CAMAC_OFFSET"		},
								{kSetRemCamacLength,	"SET_REM_CAMAC_LENGTH"		},
								{kSetRemEsoneBase,		"SET_REM_ESONE_BASE"		},
								{kSetSevtSize,			"SET_SEVT_BUFSIZE"			},
								{kSetRdoFlag,			"SET_RDO_FLAG"				},
								{kSetControllerID,		"SET_CONTROLLER_ID" 		},
								{kSetTriggerModule, 	"SET_TRIGGER_MODULE"		},
								{kSetTrigStation,		"SET_TRIG_STATION"			},
								{kSetTrigFastClear, 	"SET_TRIG_FASTCLEAR"		},
								{kSetTrigConvTime,		"SET_TRIG_CONVTIME"			},
								{kLoadRdoSetup, 		"LOAD_RDO_SETUP"			},
								{kSetIrqMode,			"SET_IRQ_MODE"				},
								{kStartRdoTask, 		"START_RDO_TASK"			},
								{kTestHostName, 		"TEST_HOSTNAME" 			},
								{kPrintBanner,			"PRINT_BANNER"				},
								{kSetVsbAddr,			"SET_VSB_ADDR"				},
								{kMbsLogin, 			"MBS_LOGIN" 				},
								{0, 					NULL						}
							};

#endif
