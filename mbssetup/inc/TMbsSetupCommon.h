#ifndef __TMbsSetupCommon_h__
#define __TMbsSetupCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsSetupCommon.h
// Purpose:        Define a MBS setup: Common defs
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

enum						{	kNofTriggers			=	16	};			// number of triggers
enum						{	kNofCrates				=	16	};			// ... crates
enum						{	kNofRdoProcs			=	16	};			// ... readout procs

enum						{	kMinFCT 				=	20	};			// minimum FCT value

enum EMbsSetupMode			{ 	kModeSingleProc			=	1,				// setup modes
								kModeMultiProc
							};

enum EMbsProcType			{ 	kProcUndefined			=	0,				// processor types
								kProcCVC				=	1,
								kProcE6 				=	2,
								kProcE7 				=	3,
								kProcPPC				=	8
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
								kControllerCC32 		=   11
							};

enum EMbsTriggerModuleType	{ 	kTriggerModuleUndefined =	0,			// trigger modules
								kTriggerModuleCAMAC 	=	1,
								kTriggerModuleVME		=	2
							};

enum EMbsTriggerMode		{ 	kTriggerModeInterrupt	=	0,			// trigger modes
								kTriggerModePolling 	=	3
							};

enum EMbsSetupTags			{	kSetHostName				=	1,
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
								kSetVsbAddr
							};

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
								{0, 				NULL			}
							};

const SMrbNamedXShort kMbsLofControllers[] =		// list of legal crate controllers
							{
								{kControllerCVC,	"CVC"			},
								{kControllerCBV,	"CBV"			},
								{kControllerPPC,	"PPC"			},
								{kControllerCC32,	"CC32"			},
								{0, 				NULL			}
							};

const SMrbNamedXShort kMbsLofTriggerModules[] = 	// list of legal trigger modules
							{
								{kTriggerModuleCAMAC,	"CAMAC" 	},
								{kTriggerModuleVME, 	"VME"		},
								{0, 					NULL		}
							};

const SMrbNamedXShort kMbsLofTriggerModes[] =		// list of legal trigger modes
							{
								{kTriggerModeInterrupt, "INTERRUPT" },
								{kTriggerModePolling,	"POLLING"	},
								{0, 					NULL		}
							};

const SMrbNamedXShort kMbsSetupTags[] =			// list of setup tag words
							{
								{kSetHostName,			"SET_HOST_NAME" 			},
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
								{0, 					NULL						}
							};

#endif
