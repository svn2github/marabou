#ifndef __TMbsSetupCommon_h__
#define __TMbsSetupCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsSetupCommon.h
// Purpose:        Define a MBS setup: Common defs
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsSetupCommon.h,v 1.14 2006-10-31 16:13:53 Rudolf.Lutter Exp $       
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

enum EMbsSetupTags			{
								kSetAuthor	=	1,
								kSetDate,
								kLoadRdoSetup,
								kMbsLogin,
								kPrintBanner,
								kReloadReadout,
								kSetBuffers,
								kSetControllerID,
								kSetDispEvb,
								kSetDispRdo,
								kSetFlushTime,
								kSetHostFlag,
								kSetHostName,
								kSetIrqMode,
								kSetLocEsoneBase,
								kSetLocMemoryBase,
								kSetLocMemoryLength,
								kSetLocPipeBase,
								kSetLocPipeLength,
								kSetLocPipeOffset,
								kSetLocPipeSegLength,
								kSetMasterType,
								kSetRdoFlag,
								kSetRdoNames,
								kSetRdPipeBaseAddr,
								kSetRemCamacBase,
								kSetRemCamacLength,
								kSetRemCamacOffset,
								kSetRemEsoneBase,
								kSetRemMemoryBase,
								kSetRemMemoryLength,
								kSetRemMemoryOffset,
								kSetSbsSetupPath,
								kSetSevtSize,
								kSetTriggerModule,
								kSetTrigModConvTime,
								kSetTrigModFastClear,
								kSetTrigModStation,
								kSetVsbAddr,
								kStartEvtBuilder,
								kStartRdoTask,
								kStartReadout,
								kStopEvtBuilder,
								kStopReadout,
								kTestHostName,
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
								{kSetAuthor,				"AUTHOR" 					},
								{kSetDate,					"DATE" 						},
								{kLoadRdoSetup, 			"LOAD_RDO_SETUP"			},
								{kMbsLogin, 				"MBS_LOGIN" 				},
								{kPrintBanner,				"PRINT_BANNER"				},
								{kReloadReadout,			"RELOAD_READOUT"			},
								{kSetBuffers,				"SET_BUFFERS"				},
								{kSetControllerID,			"SET_CONTROLLER_ID" 		},
								{kSetDispEvb,				"SET_DISP_EVB"				},
								{kSetDispRdo,				"SET_DISP_RDO"				},
								{kSetFlushTime, 			"SET_FLUSHTIME" 			},
								{kSetHostFlag,				"SET_HOST_FLAG" 			},
								{kSetHostName,				"SET_HOST_NAME" 			},
								{kSetIrqMode,				"SET_IRQ_MODE"				},
								{kSetLocEsoneBase,			"SET_LOC_ESONE_BASE"		},
								{kSetLocMemoryBase,			"SET_LOC_MEMORY_BASE"		},
								{kSetLocMemoryLength,		"SET_LOC_MEMORY_LENGTH"		},
								{kSetLocPipeBase,			"SET_LOC_PIPE_BASE"			},
								{kSetLocPipeLength, 		"SET_LOC_PIPE_LENGTH"		},
								{kSetLocPipeOffset, 		"SET_LOC_PIPE_OFFSET"		},
								{kSetLocPipeSegLength,		"SET_LOC_PIPE_SEG_LENGTH"	},
								{kSetMasterType,			"SET_MASTER_TYPE"			},
								{kSetRdoFlag,				"SET_RDO_FLAG"  			},
								{kSetRdoNames,				"SET_RDO_NAMES"				},
								{kSetRdPipeBaseAddr,		"SET_RD_PIPE_BASE_ADDR" 	},
								{kSetRemCamacBase,			"SET_REM_CAMAC_BASE"		},
								{kSetRemCamacLength,		"SET_REM_CAMAC_LENGTH"		},
								{kSetRemCamacOffset,		"SET_REM_CAMAC_OFFSET"		},
								{kSetRemEsoneBase,			"SET_REM_ESONE_BASE"		},
								{kSetRemMemoryBase,			"SET_REM_MEMORY_BASE"		},
								{kSetRemMemoryLength,		"SET_REM_MEMORY_LENGTH"		},
								{kSetRemMemoryOffset,		"SET_REM_MEMORY_OFFSET"		},
								{kSetSbsSetupPath,			"SET_SBS_SETUP_PATH"		},
								{kSetSevtSize,				"SET_SEVT_BUFSIZE"  		},
								{kSetTriggerModule, 		"SET_TRIGGER_MODULE"		},
								{kSetTrigModConvTime,		"SET_TRIG_MOD_CONVTIME"		},
								{kSetTrigModFastClear, 		"SET_TRIG_MOD_FASTCLEAR"	},
								{kSetTrigModStation,		"SET_TRIG_MOD_STATION"  	},
								{kSetVsbAddr,				"SET_VSB_ADDR"				},
								{kStartEvtBuilder,			"START_EVTBUILDER"  		},
								{kStartRdoTask, 			"START_RDO_TASK"			},
								{kStartReadout, 			"START_READOUT"				},
								{kStopEvtBuilder,			"STOP_EVTBUILDER"			},
								{kStopReadout,				"STOP_READOUT"				},
								{kTestHostName, 			"TEST_HOSTNAME" 			},
								{0, 						NULL						}
							};

#endif
