#ifndef __TMbsSetupCommon_h__
#define __TMbsSetupCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsSetupCommon.h
// Purpose:        Define a MBS setup: Common defs
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsSetupCommon.h,v 1.12 2006-10-13 11:31:42 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

enum						{	kNofTriggers			=	16	};			// number of triggers
enum						{	kNofCrates				=	16	};			// ... crates
enum						{	kNofRdoProcs			=	16	};			// ... readout procs
enum						{	kNofPPCs				=	8	};			// ... ppcs

enum						{	kMinFCT 				=	20	};			// minimum FCT value

enum						{	kUndefVal				=	0xFFFFFFFF	};

enum EMbsSetupMode			{ 	kModeSingleProc			=	1,				// setup modes
								kModeMultiProc,
								kModeMultiBranch
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
								kSetRdPipeBaseAddr,
								kSetSbsSetupPath,
								kSetRemMemoryBase,
								kSetRemMemoryOffset,
								kSetRemMemoryLength,
								kSetRemCamacBase,
								kSetRemCamacOffset,
								kSetRemCamacLength,
								kSetRemEsoneBase,
								kSetLocEsoneBase,
								kSetLocMemoryBase,
								kSetLocMemoryLength,
								kSetLocPipeBase,
								kSetLocPipeOffset,
								kSetLocPipeSegLength,
								kSetLocPipeLength,
								kSetSevtSize,
								kSetRdoFlag,
								kSetControllerID,
								kSetTriggerModule,
								kSetTrigModStation,
								kSetTrigModFastClear,
								kSetTrigModConvTime,
								kSetTrigModBase,
								kLoadRdoSetup,
								kSetIrqMode,
								kStartRdoTask,
								kTestHostName,
								kPrintBanner,
								kSetVsbAddr,
								kMbsLogin
							};

const SMrbNamedXShort kMbsLofSetupModes[] =		// list of legal setup modes
							{
								{kModeSingleProc,	"SingleProc"	},
								{kModeMultiProc,	"MultiProc" 	},
								{kModeMultiBranch,	"MultiBranch" 	},
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
								{kSetHostName,				"SET_HOST_NAME" 			},
								{kSetAuthor,				"AUTHOR" 					},
								{kSetDate,					"DATE" 						},
								{kSetRdoNames,				"SET_RDO_NAMES" 			},
								{kSetHostFlag,				"SET_HOST_FLAG" 			},
								{kSetBuffers,				"SET_BUFFERS"				},
								{kLoadMLSetup,				"LOAD_ML_SETUP" 			},
								{kLoadSPSetup,				"LOAD_SP_SETUP" 			},
								{kStartReadout, 			"START_READOUT" 			},
								{kStartEvtBuilder,			"START_EVTBUILDER"			},
								{kSetDispRdo1,				"SET_DISP_RDO1" 			},
								{kStopReadout,				"STOP_READOUT"				},
								{kReloadReadout,			"RELOAD_READOUT"			},
								{kStopEvtBuilder,			"STOP_EVTBUILDER"			},
								{kSetDispEvb,				"SET_DISP_EVB"				},
								{kSetFlushTime, 			"SET_FLUSHTIME" 			},
								{kSetMasterType,			"SET_MASTER_TYPE"			},
								{kSetRdPipeBaseAddr,		"SET_RD_PIPE_BASE_ADDR" 	},
								{kSetSbsSetupPath,			"SET_SBS_SETUP_PATH"	 	},
								{kSetRemMemoryBase,			"SET_REM_MEMORY_BASE"		},
								{kSetRemMemoryOffset,		"SET_REM_MEMORY_OFFSET"		},
								{kSetRemMemoryLength,		"SET_REM_MEMORY_LENGTH"		},
								{kSetRemCamacBase,			"SET_REM_CAMAC_BASE"		},
								{kSetRemCamacOffset,		"SET_REM_CAMAC_OFFSET"		},
								{kSetRemCamacLength,		"SET_REM_CAMAC_LENGTH"		},
								{kSetRemEsoneBase,			"SET_REM_ESONE_BASE"		},
								{kSetLocEsoneBase,			"SET_LOC_ESONE_BASE"		},
								{kSetLocMemoryBase,			"SET_LOC_MEMORY_BASE"		},
								{kSetLocMemoryLength,		"SET_LOC_MEMORY_LENGTH"		},
								{kSetLocPipeBase,			"SET_LOC_PIPE_BASE" 		},
								{kSetLocPipeOffset, 		"SET_LOC_PIPE_OFFSET"		},
								{kSetLocPipeSegLength,		"SET_LOC_PIPE_SEG_LENGTH"	},
								{kSetLocPipeLength, 		"SET_LOC_PIPE_LENGTH"		},
								{kSetSevtSize,				"SET_SEVT_BUFSIZE"			},
								{kSetRdoFlag,				"SET_RDO_FLAG"				},
								{kSetControllerID,			"SET_CONTROLLER_ID" 		},
								{kSetTriggerModule, 		"SET_TRIGGER_MODULE"		},
								{kSetTrigModStation,		"SET_TRIG_MOD_STATION"		},
								{kSetTrigModFastClear, 		"SET_TRIG_MOD_FASTCLEAR"	},
								{kSetTrigModConvTime,		"SET_TRIG_MOD_CONVTIME"		},
								{kSetTrigModBase,			"SET_TRIG_MOD_BASE"			},
								{kLoadRdoSetup, 			"LOAD_RDO_SETUP"			},
								{kSetIrqMode,				"SET_IRQ_MODE"				},
								{kStartRdoTask, 			"START_RDO_TASK"			},
								{kTestHostName, 			"TEST_HOSTNAME" 			},
								{kPrintBanner,				"PRINT_BANNER"				},
								{kSetVsbAddr,				"SET_VSB_ADDR"				},
								{kMbsLogin, 				"MBS_LOGIN" 				},
								{0, 						NULL						}
							};

#endif
