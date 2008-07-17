//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:            expconf/src/TMrbConfig.cxx
// Purpose:        MARaBOU configuration: base class
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfig.cxx,v 1.160 2008-07-17 13:53:41 Rudolf.Lutter Exp $
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <pwd.h>
#include <sys/types.h>

#include "TROOT.h"
#include "TKey.h"
#include "TFile.h"
#include "TEnv.h"

#include "TMrbSystem.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbLofUserVars.h"
#include "TMrbString.h"
#include "TMrbConfig.h"
#include "TMrbEvent.h"
#include "TMrbEvent_10_1.h"
#include "TMrbSubevent.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbCamacModule.h"
#include "TMrbVMEModule.h"
#include "TMrbCamacScaler.h"
#include "TMrbNamedArray.h"
#include "TMrbMesytec_Mux16.h"

#include "TMbsSetup.h"

#include "SetColor.h"

//_________________________________________________________________________________________________________ data types

const SMrbNamedX kMrbLofDataTypes[] =
							{
								{TMrbConfig::kDataChar, 		"C",	"Char_t"		},
								{TMrbConfig::kDataUShort,		"S",	"Short_t"		},
								{TMrbConfig::kDataUInt, 		"I",	"Int_t"			},
								{TMrbConfig::kDataULong, 		"L",	"Long_t"		},
								{TMrbConfig::kDataFloat, 		"F",	"Float_t"		},
								{0, 							NULL,	NULL			}
							};

//_________________________________________________________________________________________________________ crate types

const SMrbNamedXShort kMrbLofCrateTypes[] =
							{
								{TMrbConfig::kCrateCamac,		"CAMAC" 		},
								{TMrbConfig::kCrateVME, 		"VME"			},
								{0, 							NULL			}
							};

//_________________________________________________________________________________________________________ controller types

const SMrbNamedXShort kMrbLofControllerTypes[] =
							{
								{TMrbConfig::kControllerCBV,	"CBV"	 		},
								{TMrbConfig::kControllerCC32, 	"CC32"			},
								{0, 							NULL			}
							};

//_________________________________________________________________________________________________________ module types

const SMrbNamedXShort kMrbLofModuleTypes[] =
							{
								{TMrbConfig::kModuleRaw,		"RAW"	 		},
								{TMrbConfig::kModuleCamac,		"CAMAC" 		},
								{TMrbConfig::kModuleVME, 		"VME"			},
								{TMrbConfig::kModuleListMode,	"ListMode"		},
								{TMrbConfig::kModuleSingleMode,	"SingleMode"	},
								{TMrbConfig::kModuleScaler,		"Scaler"		},
								{TMrbConfig::kModuleControl, 	"Control"		},
								{0, 							NULL			}
							};

//_________________________________________________________________________________________________________ histogram types

const SMrbNamedX kMrbLofHistoTypes[] =
							{
								{TMrbConfig::kHistoTH1C,		"TH1C", "1-dim histogram with one byte per channel"		},
								{TMrbConfig::kHistoTH1D,		"TH1D", "1-dim histogram with one double per channel"	},
								{TMrbConfig::kHistoTH1F,		"TH1F", "1-dim histogram with one float per channel"	},
								{TMrbConfig::kHistoTH1S,		"TH1S", "1-dim histogram with one short per channel"	},
								{TMrbConfig::kHistoTH2C,		"TH2C", "2-dim histogram with one byte per channel"		},
								{TMrbConfig::kHistoTH2D,		"TH2D", "2-dim histogram with one double per channel"	},
								{TMrbConfig::kHistoTH2F,		"TH2F", "2-dim histogram with one float per channel"	},
								{TMrbConfig::kHistoTH2S,		"TH2S", "2-dim histogram with one short per channel"	},
								{TMrbConfig::kHistoRate,		"Rate", "rate histogram"								},
								{0, 							NULL			}
							};

//_________________________________________________________________________________________________________ cnaf code names

const SMrbNamedXShort kMrbCNAFNames[] =
							{
								{TMrbCNAF::kCnafBranch, 		"BRANCH"		},
								{TMrbCNAF::kCnafCrate,			"CRATE" 		},
								{TMrbCNAF::kCnafStation,		"NSTATION"		},
								{TMrbCNAF::kCnafAddr,			"ADDRESS"		},
								{TMrbCNAF::kCnafFunction,		"FUNCTION"		},
								{TMrbCNAF::kCnafData,			"DATA"			},
								{0, 							NULL			}
							};

//_________________________________________________________________________________________________________ tag words in MakeReadoutCode()

const SMrbNamedXShort kMrbLofReadoutTags[] =
							{
								{TMrbConfig::kRdoPath,						"READOUT_PATH"				},
								{TMrbConfig::kRdoFile,						"READOUT_FILE"				},
								{TMrbConfig::kRdoInclude,					"READOUT_INCLUDE"			},
								{TMrbConfig::kRdoLibs,						"READOUT_LIBS"				},
								{TMrbConfig::kRdoPosix,						"READOUT_POSIX"				},
								{TMrbConfig::kRdoPosixLib,					"POSIX_LIB"					},
								{TMrbConfig::kRdoLynxPlatform,				"GSI_LYNX_PLATFORM"			},
								{TMrbConfig::kRdoAlign64,					"ALIGN_TO_64_BITS"			},
								{TMrbConfig::kRdoDebug,						"DEBUG_READOUT"				},
								{TMrbConfig::kRdoNameLC, 					"EXP_NAME_LC"				},
								{TMrbConfig::kRdoNameUC, 					"EXP_NAME_UC"				},
								{TMrbConfig::kRdoTitle,						"EXP_TITLE" 				},
								{TMrbConfig::kRdoSetup,						"EXP_SETUP" 				},
								{TMrbConfig::kRdoCreationDate,				"CREATION_DATE" 			},
								{TMrbConfig::kRdoAuthor, 					"AUTHOR"					},
								{TMrbConfig::kRdoUser,	 					"USER"						},
								{TMrbConfig::kRdoCamacController,			"CAMAC_CONTROLLER"			},
								{TMrbConfig::kRdoDefineTriggers, 			"DEFINE_TRIGGERS"			},
								{TMrbConfig::kRdoDefinePointers, 			"DEFINE_POINTERS"			},
								{TMrbConfig::kRdoDeviceTables,	 			"DEVICE_TABLES" 			},
								{TMrbConfig::kRdoInitPointers,				"INIT_POINTERS" 			},
								{TMrbConfig::kRdoInitEnvironment,			"INIT_ENVIRONMENT"			},
								{TMrbConfig::kRdoInitCommonCode,			"INIT_COMMON_CODE"			},
								{TMrbConfig::kRdoInitModules,				"INIT_MODULES"				},
								{TMrbConfig::kRdoOnStartAcquisition,		"ON_START_ACQUISITION"		},
								{TMrbConfig::kRdoOnStopAcquisition, 		"ON_STOP_ACQUISITION"		},
								{TMrbConfig::kRdoOnTriggerXX,				"ON_TRIGGER_XX" 			},
								{TMrbConfig::kRdoIgnoreTriggerXX,			"IGNORE_TRIGGER_XX" 		},
								{TMrbConfig::kRdoWriteSetup,				"WRITE_SETUP "				},
								{TMrbConfig::kRdoIncludesAndDefs,			"INCLUDES_AND_DEFS_READOUT" },
								{TMrbConfig::kRdoClearModule,				"CLEAR_MODULE"				},
								{TMrbConfig::kRdoUtilities,					"UTILITIES" 				},
								{TMrbConfig::kRdoDefinePrototypes,			"DEFINE_PROTOTYPES" 		},
								{TMrbConfig::kRdoDefineGlobals, 	 		"DEFINE_GLOBALS"		 	},
								{TMrbConfig::kRdoDefineGlobalsOnce, 		"DEFINE_GLOBALS_ONCE_ONLY"	},
								{TMrbConfig::kRdoDefineLocalVarsInit,		"DEFINE_LOCAL_VARS_INIT" 	},
								{TMrbConfig::kRdoDefineLocalVarsReadout,	"DEFINE_LOCAL_VARS_READOUT" },
								{TMrbConfig::kRdoDefinePrototypes,			"DEFINE_PROTOTYPES" 		},
								{TMrbConfig::kRdoUserDefinedDefines,		"USER_DEFINED_DEFINES" 	 	},
								{0, 										NULL						}
							};
							
//_________________________________________________________________________________________________________ special camac readout tags

const SMrbNamedXShort kMrbLofModuleTags[] =
							{
								{TMrbConfig::kModuleDefs,					"MODULE_DEFS"				},
								{TMrbConfig::kModuleInitCommonCode, 		"INIT_COMMON_CODE"			},
								{TMrbConfig::kModuleInitModule, 			"INIT_MODULE"				},
								{TMrbConfig::kModuleInitChannel,			"INIT_CHANNEL"				},
								{TMrbConfig::kModuleReadChannel,			"READ_CHANNEL"				},
								{TMrbConfig::kModuleIncrementChannel,		"INCREMENT_CHANNEL" 		},
								{TMrbConfig::kModuleSkipChannels,			"SKIP_CHANNELS" 			},
								{TMrbConfig::kModuleWriteSubaddr,			"WRITE_SUBADDR" 			},
								{TMrbConfig::kModuleStoreData,				"STORE_DATA"				},
								{TMrbConfig::kModuleClearModule,			"CLEAR_MODULE"				},
								{TMrbConfig::kModuleReadModule, 			"READ_MODULE"				},
								{TMrbConfig::kModuleSetupReadout,			"SETUP_READOUT" 			},
								{TMrbConfig::kModuleFinishReadout,			"FINISH_READOUT"			},
								{TMrbConfig::kModuleStartAcquisition,		"START_ACQUISITION" 		},
								{TMrbConfig::kModuleStopAcquisition,		"STOP_ACQUISITION"			},
								{TMrbConfig::kModuleStartAcquisitionGroup,	"START_ACQUISITION_GROUP" 	},
								{TMrbConfig::kModuleStopAcquisitionGroup,	"STOP_ACQUISITION_GROUP"	},
								{TMrbConfig::kModuleUtilities,				"UTILITIES" 				},
								{TMrbConfig::kModuleDefineGlobals, 			"DEFINE_GLOBALS"	 		},
								{TMrbConfig::kModuleDefineGlobalsOnce,  	"DEFINE_GLOBALS_ONCE_ONLY"	},
								{TMrbConfig::kModuleDefineLocalVarsInit,	"DEFINE_LOCAL_VARS_INIT" 	},
								{TMrbConfig::kModuleDefineLocalVarsReadout, "DEFINE_LOCAL_VARS_READOUT" },
								{TMrbConfig::kModuleDefinePrototypes,		"DEFINE_PROTOTYPES" 		},
								{TMrbConfig::kModuleDefineIncludePaths,		"DEFINE_INCLUDE_PATHS" 		},
								{TMrbConfig::kModuleDefineLibraries,		"DEFINE_LIBRARIES"	 		},
								{0, 										NULL						}
							};
							
//_________________________________________________________________________________________________________ tag words in MakeAnalyzeCode()

const SMrbNamedXShort kMrbLofAnalyzeTags[] =
							{
								{TMrbConfig::kAnaFile,						"ANALYZE_FILE"					},
								{TMrbConfig::kAnaNameLC,					"ANALYZE_NAME"					},
								{TMrbConfig::kAnaNameUC,					"ANALYZE_NAME_UC"				},
								{TMrbConfig::kAnaTitle, 					"ANALYZE_TITLE" 				},
								{TMrbConfig::kAnaCreationDate,				"CREATION_DATE" 				},
								{TMrbConfig::kAnaAuthor,					"AUTHOR"						},
								{TMrbConfig::kAnaUser,						"USER"	 						},
								{TMrbConfig::kAnaHtmlMakeSetup,				"HTML_MAKE_SETUP" 				},
								{TMrbConfig::kAnaIncludeXhitDefs,			"INCLUDE_XHIT_DEFS" 		 	},
								{TMrbConfig::kAnaIncludesAndDefs,			"INCLUDES_AND_DEFS_ANALYZE" 	},
								{TMrbConfig::kAnaPragmaLinkClasses, 		"PRAGMA_LINK_CLASSES"			},
								{TMrbConfig::kAnaClassImp,					"IMPLEMENT_CLASSES" 			},
								{TMrbConfig::kAnaReservedEvents,			"RESERVED_EVENTS"	 			},
								{TMrbConfig::kAnaMakeClassNames,			"MAKE_CLASS_NAMES"				},
								{TMrbConfig::kAnaInitializeLists,		 	"INITIALIZE_LISTS"	 			},
								{TMrbConfig::kAnaModuleTimeOffset,		 	"MODULE_TIME_OFFSET" 			},
								{TMrbConfig::kAnaAddUserEnv,			 	"ADD_USER_ENV"		 			},
								{TMrbConfig::kAnaFindVars,					"ANALYZE_FIND_VARS" 			},
								{TMrbConfig::kAnaEventClassDef, 			"EVT_CLASS_DEFINITION"			},
								{TMrbConfig::kAnaEventClassMethods, 		"EVT_CLASS_METHODS" 			},
								{TMrbConfig::kAnaEventUserClassMethods, 	"EVT_USER_CLASS_METHODS" 		},
								{TMrbConfig::kAnaEventClassInstance,		"EVT_CLASS_INSTANCE"			},
								{TMrbConfig::kAnaEventUserClassInstance,	"EVT_USER_CLASS_INSTANCE"		},
								{TMrbConfig::kAnaEventDefinePointers,		"EVT_DEFINE_POINTERS"			},
								{TMrbConfig::kAnaEventNameLC,				"EVT_NAME_LC"					},
								{TMrbConfig::kAnaEventNameUC,				"EVT_NAME_UC"					},
								{TMrbConfig::kAnaEventTitle,				"EVT_TITLE" 					},
								{TMrbConfig::kAnaEventAutoSave,				"EVT_AUTOSAVE" 					},
								{TMrbConfig::kAnaEventTrigger,				"EVT_TRIGGER"					},
								{TMrbConfig::kAnaEventIdEnum,				"EVT_ID_ENUM"					},
								{TMrbConfig::kAnaEventPrivateData,			"EVT_PRIVATE_DATA"				},
								{TMrbConfig::kAnaEventUserMethods,			"EVT_USER_METHODS"				},
								{TMrbConfig::kAnaEventUserData,				"EVT_USER_DATA"					},
								{TMrbConfig::kAnaEventMethodToProcessEvent, "EVT_METHOD_TO_PROCESS_EVENT"	},
								{TMrbConfig::kAnaEventDispatchOverTrigger,	"EVT_DISPATCH_OVER_TRIGGER" 	},
								{TMrbConfig::kAnaEventIgnoreTrigger,		"EVT_IGNORE_TRIGGER"			},
								{TMrbConfig::kAnaEventTriggerStartAcq,		"EVT_TRIGGER_START_ACQUISITION" },
								{TMrbConfig::kAnaEventTriggerStopAcq,		"EVT_TRIGGER_STOP_ACQUISITION" 	},
								{TMrbConfig::kAnaEventReplayEvent,			"EVT_REPLAY_EVENT"			 	},
								{TMrbConfig::kAnaEventSetFakeMode,			"EVT_SET_FAKE_MODE"				},
								{TMrbConfig::kAnaEventCreateTree,			"EVT_CREATE_TREE"				},
								{TMrbConfig::kAnaEventAddBranches,			"EVT_ADD_BRANCHES"				},
								{TMrbConfig::kAnaEventBookParams,			"EVT_BOOK_PARAMS"				},
								{TMrbConfig::kAnaEventBookHistograms,		"EVT_BOOK_HISTOGRAMS"			},
								{TMrbConfig::kAnaEventFillHistograms,		"EVT_FILL_HISTOGRAMS"			},
								{TMrbConfig::kAnaEventFillRateHistograms,	"EVT_FILL_RATE_HISTOGRAMS"		},
								{TMrbConfig::kAnaEventSetupSevtList,		"EVT_SETUP_SEVT_LIST"			},
								{TMrbConfig::kAnaEventAllocHitBuffer,		"EVT_ALLOC_HITBUFFER"			},
								{TMrbConfig::kAnaEventSetScaleDown, 		"EVT_SET_SCALEDOWN" 			},
								{TMrbConfig::kAnaEventInitializeTree,		"EVT_INITIALIZE_TREE"			},
								{TMrbConfig::kAnaEventSetReplayMode,		"EVT_SET_REPLAY_MODE"			},
								{TMrbConfig::kAnaEventSetWriteTree,			"EVT_SET_WRITE_TREE"			},
								{TMrbConfig::kAnaEventInitializeBranches,	"EVT_INITIALIZE_BRANCHES"		},
								{TMrbConfig::kAnaEventReplayTree,			"EVT_REPLAY_TREE"				},
								{TMrbConfig::kAnaEventFirstSubevent,		"EVT_FIRST_SEVT"				},
								{TMrbConfig::kAnaEventSetBranchStatus,		"EVT_SET_BRANCH_STATUS" 		},
								{TMrbConfig::kAnaEventAnalyze,				"EVT_ANALYZE"			 		},
								{TMrbConfig::kAnaEvtResetData,				"EVT_RESET_DATA"				},
								{TMrbConfig::kAnaEvtBaseClass,				"EVT_BASE_CLASS"				},
								{TMrbConfig::kAnaSevtNameLC,				"SEVT_NAME_LC"					},
								{TMrbConfig::kAnaSevtNameUC,				"SEVT_NAME_UC"					},
								{TMrbConfig::kAnaSevtTitle, 				"SEVT_TITLE"					},
								{TMrbConfig::kAnaSevtSetName,				"SEVT_SET_NAME" 				},
								{TMrbConfig::kAnaSevtSerial,				"SEVT_SERIAL"					},
								{TMrbConfig::kAnaSevtNofParams,				"SEVT_NOF_PARAMS"				},
								{TMrbConfig::kAnaSevtSerialEnum,			"SEVT_SERIAL_ENUM"				},
								{TMrbConfig::kAnaSevtBitsEnum,				"SEVT_BITS_ENUM" 				},
								{TMrbConfig::kAnaSevtIndicesEnum,	 		"SEVT_INDICES_ENUM" 			},
								{TMrbConfig::kAnaSevtClassDef,				"SEVT_CLASS_DEFINITION" 		},
								{TMrbConfig::kAnaSevtClassMethods,			"SEVT_CLASS_METHODS"			},
								{TMrbConfig::kAnaSevtClassInstance, 		"SEVT_CLASS_INSTANCE"			},
								{TMrbConfig::kAnaSevtGetAddr, 				"SEVT_GET_ADDR" 				},
								{TMrbConfig::kAnaSevtUserMethods,			"SEVT_USER_METHODS"				},
								{TMrbConfig::kAnaSevtUserData,				"SEVT_USER_DATA"				},
								{TMrbConfig::kAnaSevtFriends,				"SEVT_FRIENDS"					},
								{TMrbConfig::kAnaSevtCtor,					"SEVT_CTOR" 					},
								{TMrbConfig::kAnaSevtDispatchOverType,		"SEVT_DISPATCH_OVER_TYPE"	 	},
								{TMrbConfig::kAnaSevtDispatchOverSerial,	"SEVT_DISPATCH_OVER_SERIAL" 	},
								{TMrbConfig::kAnaSevtDefineAddr,			"SEVT_DEFINE_ADDR"				},
								{TMrbConfig::kAnaSevtPrivateData,			"SEVT_PRIVATE_DATA" 			},
								{TMrbConfig::kAnaSevtPrivateHistograms, 	"SEVT_PRIVATE_HISTOGRAMS"		},
								{TMrbConfig::kAnaSevtFillSubevent,			"SEVT_FILL_SUBEVENT"			},
								{TMrbConfig::kAnaSevtBookParams,			"SEVT_BOOK_PARAMS"				},
								{TMrbConfig::kAnaSevtBookHistograms,		"SEVT_BOOK_HISTOGRAMS"			},
								{TMrbConfig::kAnaSevtFillHistograms,		"SEVT_FILL_HISTOGRAMS"			},
								{TMrbConfig::kAnaSevtInitializeBranch,		"SEVT_INITIALIZE_BRANCH"		},
								{TMrbConfig::kAnaSevtResetData,				"SEVT_RESET_DATA"				},
								{TMrbConfig::kAnaSevtXhitClass,				"SEVT_XHIT_CLASS"				},
								{TMrbConfig::kAnaModuleIdEnum,				"MODULE_ID_ENUM"				},
								{TMrbConfig::kAnaModuleSerialEnum,			"MODULE_SERIAL_ENUM"			},
								{TMrbConfig::kAnaModuleSpecialEnum,			"MODULE_SPECIAL_ENUM"			},
								{TMrbConfig::kAnaHistoDefinePointers,		"HISTO_DEFINE_POINTERS" 		},
								{TMrbConfig::kAnaHistoInitializeArrays, 	"HISTO_INIT_ARRAYS" 			},
								{TMrbConfig::kAnaHistoBookUserDefined,	 	"HISTO_BOOK_USER_DEFINED"		},
								{TMrbConfig::kAnaHistoFillArrays,	 		"HISTO_FILL_ARRAYS" 			},
								{TMrbConfig::kAnaVarDefinePointers, 		"VAR_DEFINE_POINTERS"			},
								{TMrbConfig::kAnaVarClassInstance,			"VAR_CLASS_INSTANCE"			},
								{TMrbConfig::kAnaVarArrDefinePointers,		"VARARR_DEFINE_POINTERS"		},
								{TMrbConfig::kAnaVarArrClassInstance,		"VARARR_CLASS_INSTANCE" 		},
								{TMrbConfig::kAnaWdwDefinePointers, 		"WDW_DEFINE_POINTERS"			},
								{TMrbConfig::kAnaWdwClassInstance,			"WDW_CLASS_INSTANCE"			},
								{TMrbConfig::kAnaWdwAdjustPointers, 		"WDW_ADJUST_POINTERS"			},
								{TMrbConfig::kAnaUserInitializeBeforeHB,	"INCLUDE_USER_INITIALIZE_BEFORE_HB"	},
								{TMrbConfig::kAnaUserInitializeAfterHB, 	"INCLUDE_USER_INITIALIZE_AFTER_HB"	},
								{TMrbConfig::kAnaUserBookParams,			"INCLUDE_USER_BOOK_PARAMS"		},
								{TMrbConfig::kAnaUserBookHistograms,		"INCLUDE_USER_BOOK_HISTOGRAMS"	},
								{TMrbConfig::kAnaUserGlobals,				"INCLUDE_USER_GLOBALS"			},
								{TMrbConfig::kAnaUserUtilities,				"INCLUDE_USER_UTILITIES"		},
								{TMrbConfig::kAnaUserReloadParams,			"RELOAD_PARAMS" 				},
								{TMrbConfig::kAnaUserFinishRun, 			"FINISH_RUN"	 				},
								{TMrbConfig::kAnaUserMessages,				"HANDLE_MESSAGES"				},
								{TMrbConfig::kAnaUserDummyMethods,			"INCLUDE_DUMMY_METHODS" 		},
								{TMrbConfig::kAnaUsingNameSpace,			"USING_NAMESPACE"	 			},
								{TMrbConfig::kAnaUserDefinedGlobals,		"USER_DEFINED_GLOBALS"	 		},
								{TMrbConfig::kAnaUserDefinedEnums,			"USER_DEFINED_ENUMS" 	 		},
								{TMrbConfig::kAnaUserDefinedDefines,		"USER_DEFINED_DEFINES" 	 		},
								{TMrbConfig::kAnaMakeUserCxxFlags,			"MAKE_USER_CXXFLAGS" 			},
								{TMrbConfig::kAnaMakeUserHeaders,			"MAKE_USER_HEADERS" 			},
								{TMrbConfig::kAnaMakeUlibHeaders,			"MAKE_ULIB_HEADERS" 			},
								{TMrbConfig::kAnaMakeUserCode,				"MAKE_USER_CODE"	 			},
								{TMrbConfig::kAnaMakeUserLibs,				"MAKE_USER_LIBS"	 			},
								{TMrbConfig::kAnaMakeUserRules,				"MAKE_USER_RULES"	 			},
								{TMrbConfig::kAnaMakeUlibRules,				"MAKE_ULIB_RULES"	 			},
								{TMrbConfig::kAnaMakeAll,					"MAKE_ALL"	 					},
								{TMrbConfig::kAnaMakeClean,					"MAKE_CLEAN"	 				},
								{TMrbConfig::kAnaMakeLibNew,				"MAKE_LIBNEW"		 			},
								{TMrbConfig::kAnaIncludeEvtSevtModGlobals,	"INCLUDE_EVT_SEVT_MOD_GLOBALS"	},
								{TMrbConfig::kAnaInitializeEvtSevtMods,		"INITIALIZE_EVT_SEVT_MODS"	 	},
								{TMrbConfig::kAnaLoadUserLibs,				"LOAD_USER_LIBS"			 	},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ tag words in MakeConfigCode()
const SMrbNamedXShort kMrbLofConfigTags[] =
							{
								{TMrbConfig::kCfgFile,						"CONFIG_FILE"					},
								{TMrbConfig::kCfgNameLC,					"CONFIG_NAME_LC"				},
								{TMrbConfig::kCfgNameUC,					"CONFIG_NAME_UC"				},
								{TMrbConfig::kCfgTitle, 					"CONFIG_TITLE"					},
								{TMrbConfig::kCfgAuthor,					"AUTHOR"						},
								{TMrbConfig::kCfgCreationDate,				"CREATION_DATE" 				},
								{TMrbConfig::kCfgCreateConfig,				"CREATE_CONFIG" 				},
								{TMrbConfig::kCfgDefineEvents,				"DEFINE_EVENTS" 				},
								{TMrbConfig::kCfgDefineSubevents,			"DEFINE_SUBEVENTS"				},
								{TMrbConfig::kCfgDefineModules, 			"DEFINE_MODULES"				},
								{TMrbConfig::kCfgAssignParams,				"ASSIGN_PARAMS" 				},
								{TMrbConfig::kCfgConnectToEvent,			"CONNECT_TO_EVENT"				},
								{TMrbConfig::kCfgWriteTimeStamp,			"WRITE_TIME_STAMP"				},
								{TMrbConfig::kCfgDefineVariables,			"DEFINE_VARIABLES"				},
								{TMrbConfig::kCfgDefineWindows, 			"DEFINE_WINDOWS"				},
								{TMrbConfig::kCfgDefineScalers, 			"DEFINE_SCALERS"				},
								{TMrbConfig::kCfgMakeCode,					"MAKE_CODE" 					},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ tag words in MakeRcFile()

const SMrbNamedXShort kMrbLofRcFileTags[] =
							{
								{TMrbConfig::kRcExpData,					"EXP_DATA"						},
								{TMrbConfig::kRcEvtData,					"EVT_DATA"						},
								{TMrbConfig::kRcSevtData,					"SEVT_DATA"						},
								{TMrbConfig::kRcModuleData,					"MODULE_DATA"					},
								{TMrbConfig::kRcModuleSettings,				"MODULE_SETTINGS"					},
								{TMrbConfig::kRcUserGlobals,				"USER_GLOBALS"					},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ tag words in CreatePrototypeForUserMacro()

const SMrbNamedXShort kMrbLofUserMacroTags[] =
							{
								{TMrbConfig::kUmaFile,						"USER_MACRO_FILE"				},
								{TMrbConfig::kUmaNameLC,					"USER_MACRO_NAME"				},
								{TMrbConfig::kUmaNameUC,					"USER_MACRO_NAME_UC"			},
								{TMrbConfig::kUmaTitle, 					"USER_MACRO_TITLE" 				},
								{TMrbConfig::kUmaCreationDate,				"CREATION_DATE" 				},
								{TMrbConfig::kUmaAuthor,					"AUTHOR"						},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ tag words in CreateUserEvent()

const SMrbNamedXShort kMrbLofUserEventTags[] =
							{
								{TMrbConfig::kUevFile,						"USER_EVENT_FILE"				},
								{TMrbConfig::kUevNameLC,					"USER_EVENT_NAME_LC"			},
								{TMrbConfig::kUevNameUC,					"USER_EVENT_NAME_UC"			},
								{TMrbConfig::kUevConfigLC,					"USER_EVENT_CONFIG_LC"			},
								{TMrbConfig::kUevConfigUC,					"USER_EVENT_CONFIG_UC"			},
								{TMrbConfig::kUevTitle, 					"USER_EVENT_TITLE" 				},
								{TMrbConfig::kUevCreationDate,				"CREATION_DATE" 				},
								{TMrbConfig::kUevAuthor,					"AUTHOR"						},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ tag words in CreateXhit()

const SMrbNamedXShort kMrbLofXhitTags[] =
							{
								{TMrbConfig::kXhitFile,					"EXTENDED_HIT_FILE"				},
								{TMrbConfig::kXhitNameLC,					"EXTENDED_HIT_NAME_LC"			},
								{TMrbConfig::kXhitNameUC,					"EXTENDED_HIT_NAME_UC"			},
								{TMrbConfig::kXhitConfigLC,				"EXTENDED_HIT_CONFIG_LC"		},
								{TMrbConfig::kXhitConfigUC,				"EXTENDED_HIT_CONFIG_UC"		},
								{TMrbConfig::kXhitTitle, 					"EXTENDED_HIT_TITLE" 			},
								{TMrbConfig::kXhitCreationDate,			"CREATION_DATE" 				},
								{TMrbConfig::kXhitAuthor,					"AUTHOR"						},
								{TMrbConfig::kXhitDataLength, 				"EXTENDED_HIT_DATA_LENGTH" 		},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ camac modules ids

const SMrbNamedXShort kMrbLofModuleIDs[] =
							{
								{TMrbConfig::kModuleSilena4418V,			"Silena_4418V"					},
								{TMrbConfig::kModuleSilena4418T,			"Silena_4418T"					},
								{TMrbConfig::kModuleUct8904,				"Uct_8904"						},
								{TMrbConfig::kModuleSen16P2047, 			"Sen_16P_2047"					},
								{TMrbConfig::kModuleSilena7420Smdw, 		"Silena_7420"					},
								{TMrbConfig::kModuleLeCroy2228A,			"LeCroy_2228A"					},
								{TMrbConfig::kModuleLeCroy4434, 			"LeCroy_4434"					},
								{TMrbConfig::kModuleAdcd9701,				"Adcd_9701" 					},
								{TMrbConfig::kModuleSen2010,				"Sen_2010"						},
								{TMrbConfig::kModuleNE9041, 				"NE_9041"						},
								{TMrbConfig::kModuleCaenV260,				"Caen_V260" 					},
								{TMrbConfig::kModuleCaenV556,				"Caen_V556" 					},
								{TMrbConfig::kModuleCaenV775,				"Caen_V775" 					},
								{TMrbConfig::kModuleCaenV785,				"Caen_V785" 					},
								{TMrbConfig::kModuleCaenV820,				"Caen_V820" 					},
								{TMrbConfig::kModuleGanelec_Q1612F, 		"Ganelec_Q1612F"				},
								{TMrbConfig::kModuleOrtec_413A, 			"Ortec_413A"					},
								{TMrbConfig::kModuleXia_DGF_4C, 			"Xia_DGF_4C"					},
								{TMrbConfig::kModuleAcromag_IP341, 			"Acromag_IP341"					},
								{TMrbConfig::kModuleUserDefined,	 		"UserDefined/RAW"				},
								{TMrbConfig::kModuleLeCroy4432, 			"LeCroy_4432"					},
								{TMrbConfig::kModuleLeCroy4448,				"LeCroy_4448"					},
								{TMrbConfig::kModuleKinetics3655,			"Kinetics_3655"					},
								{TMrbConfig::kModuleMpiHD_IOReg,			"MpiHD_IOReg"					},
								{TMrbConfig::kModuleSis_3300,				"Sis_3300"						},
								{TMrbConfig::kModuleSis_3600,				"Sis_3600"						},
								{TMrbConfig::kModuleSis_3801,				"Sis_3801"						},
								{TMrbConfig::kModuleSis_3820,				"Sis_3820"						},
								{TMrbConfig::kModuleLeCroy_1176,			"LeCroy_1176"					},
								{TMrbConfig::kModuleLeCroy_2280,			"LeCroy_2280"					},
								{TMrbConfig::kModuleCologne_CPTM,			"Cologne_CPTM"					},
								{TMrbConfig::kModuleCaenV879,				"Caen_V879" 					},
								{TMrbConfig::kModuleCaenV965,				"Caen_V965" 					},
								{TMrbConfig::kModuleISN4481,				"ISN_4481" 						},
								{TMrbConfig::kModuleMesytecMux16,			"Mesytec_Mux16" 				},
								{TMrbConfig::kModuleSoftModule, 	 		"@SoftMod@" 					},
								{0, 										NULL							}
							};


//_________________________________________________________________________________________________________ options in MakeReadoutCode()

const SMrbNamedXShort kMrbReadoutOptions[] =
							{
								{TMrbConfig::kRdoOptOverwrite,				"OVERWRITE" 					},
								{TMrbConfig::kRdoOptVerbose,				"VERBOSE"	 					},
								{TMrbConfig::kRdoOptDefault,				"DEFAULT"						},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ options in MakeAnalyzeCode()

const SMrbNamedXShort kMrbAnalyzeOptions[] =
							{
								{TMrbConfig::kAnaOptSubevents,				"SUBEVENTS" 					},
								{~TMrbConfig::kAnaOptSubevents,				"NOSUBEVENTS"					},
								{TMrbConfig::kAnaOptParamsByName,			"BYNAME"						},
								{~TMrbConfig::kAnaOptParamsByName,			"BYINDEX"						},
								{TMrbConfig::kAnaOptHistograms, 			"HISTOGRAMS"					},
								{~TMrbConfig::kAnaOptHistograms, 			"NOHISTOGRAMS"					},
								{TMrbConfig::kAnaOptLeaves, 	 			"LEAVES"						},
								{~TMrbConfig::kAnaOptLeaves,	 			"BRANCHES"						},
								{TMrbConfig::kAnaOptOverwrite,				"OVERWRITE" 					},
								{TMrbConfig::kAnaOptReplayMode, 			"REPLAYMODE"					},
								{TMrbConfig::kAnaOptVerbose,	 			"VERBOSE"						},
								{TMrbConfig::kAnaOptDefault,				"DEFAULT"						},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ options in MakeConfigCode()

const SMrbNamedXShort kMrbConfigOptions[] =
							{
								{TMrbConfig::kCfgOptOverwrite,				"OVERWRITE" 					},
								{TMrbConfig::kCfgOptVerbose,				"VERBOSE"						},
								{TMrbConfig::kCfgOptDefault,				"DEFAULT"						},
								{0, 										NULL							}
							};

//_________________________________________________________________________________________________________ options in MakeRcFile()

const SMrbNamedXShort kMrbRcFileOptions[] =
							{
								{TMrbConfig::kRcOptOverwrite,				"OVERWRITE" 					},
								{TMrbConfig::kRcOptVerbose,					"VERBOSE"						},
								{TMrbConfig::kRcOptByName,					"BYNAME"						},
								{TMrbConfig::kRcOptDefault,					"DEFAULT"						},
								{0, 										NULL							}
							};


//_________________________________________________________________________________________________________ options for user-specific include files

const SMrbNamedXShort kMrbIncludeOptions[] =
							{
								{TMrbConfig::kIclOptUtilities,				"UTILITIES"				},
								{TMrbConfig::kIclOptUserMethod,				"USERMETHOD"			},
								{TMrbConfig::kIclOptInitialize,				"INITIALIZE"			},
								{TMrbConfig::kIclOptReloadParams,			"RELOADPARAMS"			},
								{TMrbConfig::kIclOptFinishRun,				"FINISHRUN" 			},
								{TMrbConfig::kIclOptBookHistograms,			"BOOKHISTOGRAMS"		},
								{TMrbConfig::kIclOptBookParams, 			"BOOKPARAMS"			},
								{TMrbConfig::kIclOptProcessEvent,			"PROCESSEVENT"			},
								{TMrbConfig::kIclOptProcessEvent,			"ANALYZE"				},
								{TMrbConfig::kIclOptHandleMessages,			"HANDLEMESSAGES"		},
								{0, 										NULL					}
							};

//_________________________________________________________________________________________________________ types of variables and windows
const SMrbNamedXShort kMrbVariableTypes[] =
							{
								{kVarI,						"INTEGER"						},
								{kVarF,						"FLOAT" 						},
								{kVarS,						"STRING"						},
								{kVarB,						"BOOLEAN"						},
								{0, 						NULL							}
							};

const SMrbNamedXShort kMrbWindowTypes[] =
							{
								{kWindowI,					"INTEGER"						},
								{kWindowF,					"FLOAT" 						},
								{kWindow2D,					"CUT" 							},
								{0, 						NULL							}
							};

//__________________________________________________________________________________________________________________________________________

extern TSystem * gSystem;

extern TMrbLofUserVars * gMrbLofUserVars; 		// a list of all user vars/windows
extern TMrbLogger * gMrbLog;					// message logger

TMrbConfig * gMrbConfig = NULL;

ClassImp(TMrbConfig)

TMrbConfig::TMrbConfig(const Char_t * CfgName, const Char_t * CfgTitle) : TNamed(CfgName, CfgTitle) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig
// Purpose:        Create a MBS configuration
// Arguments:      Char_t * CfgName   -- name of experiment
//                 Char_t * CfgTitle  -- (opt.) title
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resPath;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fMessageLogger = gMrbLog;
		
	if (gMrbConfig != NULL) {
		gMrbLog->Err() << "Config already defined - " << gMrbConfig->fName << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (this->NameNotLegal("config", CfgName)) {
		this->MakeZombie();
	} else {
		if (*CfgTitle == '\0') SetTitle("MARaBOU Config");

		fVerboseMode = gEnv->GetValue("TMrbConfig.VerboseMode", kFALSE);
		
		fNofEvents = 0;				// init list of events/triggers
		fLofEvents.Delete();

		fNofSubevents = 0;			// init list of subevents
		fLofSubevents.Delete();

		fNofModules = 0;			// init list of modules
		fLofModules.Delete();

		fLongParamNames = kFALSE;	// we use param names w/o subevent prefix

		fUseMapFile = kTRUE;		// use TMap normally

		fDeadTimeScaler = NULL; 	// no dead-time scaler
		fDeadTimeInterval = 0;

		fConfigChecked = kFALSE;	// consistency check has to be done
		fConfigOk = kFALSE;

		fCrateTable.Set(TMrbConfig::kNofCrates);				// init crate table
		fControllerTable.Set(TMrbConfig::kNofCrates);			// init controller table
		fControllerTable.Reset(kCrateUnused);

		fLofDataTypes.SetName("Data Types");					// ... data types
		fLofDataTypes.SetPatternMode();
		fLofDataTypes.AddNamedX(kMrbLofDataTypes);	

		fLofCrateTypes.SetName("Crate Types"); 					// ... crate types
		fLofCrateTypes.AddNamedX(kMrbLofCrateTypes);	

		fLofControllerTypes.SetName("Controller Types"); 		// ... controller types
		fLofControllerTypes.AddNamedX(kMrbLofControllerTypes);	

		fLofModuleTypes.SetName("Module Types");				// ... module types
		fLofModuleTypes.SetPatternMode();
		fLofModuleTypes.AddNamedX(kMrbLofModuleTypes);	

		fLofHistoTypes.SetName("Histogram Types");				// ... histogram types
		fLofHistoTypes.SetPatternMode();
		fLofHistoTypes.AddNamedX(kMrbLofHistoTypes);	

		fLofReadoutTags.SetName("Readout Tags");				// ... readout tags
		fLofReadoutTags.AddNamedX(kMrbLofReadoutTags);	

		fLofAnalyzeTags.SetName("Analyze Tags");				// ... analyze tags
		fLofAnalyzeTags.AddNamedX(kMrbLofAnalyzeTags);

		fLofConfigTags.SetName("Configuration Tags");			// ... config tags
		fLofConfigTags.AddNamedX(kMrbLofConfigTags);

		fLofRcFileTags.SetName("RcFile Tags");  				// ... rc file tags
		fLofRcFileTags.AddNamedX(kMrbLofRcFileTags);

		fLofUserMacroTags.SetName("UserMacro Tags");  			// ... user macro tags
		fLofUserMacroTags.AddNamedX(kMrbLofUserMacroTags);

		fLofUserEventTags.SetName("UserEvent Tags");  			// ... user event tags
		fLofUserEventTags.AddNamedX(kMrbLofUserEventTags);

		fLofXhitTags.SetName("ExtendedHIt Tags");  				// ... special hit tags
		fLofXhitTags.AddNamedX(kMrbLofXhitTags);

		fCNAFNames.SetName("CNAF Names"); 						// ... cnaf key words
		fCNAFNames.SetPatternMode();
		fCNAFNames.AddNamedX(kMrbCNAFNames);

		fLofModuleTags.SetName("Module Tags");					// ... tags
		fLofModuleTags.AddNamedX(kMrbLofModuleTags);

		fLofModuleIDs.SetName("Modules"); 						// ... modules available
		fLofModuleIDs.AddNamedX(kMrbLofModuleIDs);

		fLofGlobals.Delete();
		fLofGlobals.SetName("Global Vars");

		fLofDefines.Delete();
		fLofDefines.SetName("#define");

		fLofMbsBranches.Delete();
		fLofMbsBranches.SetName("MBS branches");

		this->GetAuthor();		 								// author's name
		this->GetMailAddr();		 							// author's name

		TDatime dt; 											// creation date
		fCreationDate = dt.AsString();

		fLofEvents.SetName("List of events");
		fLofSubevents.SetName("List of subevents");
		fLofModules.SetName("List of modules");
		fLofScalers.SetName("List of scalers");
		fLofMuxs.SetName("List of multiplexers");

		UpdateTriggerTable();									// initialize trigger table								

		fSevtSize = 0;

		fLofUserHistograms.Delete();							// init list of user-defined histograms
		fLofHistoArrays.Delete();								// init list of histogram arrays
		fLofHistoConditions.Delete();							// init list of histogram booking conds
		fLofOnceOnlyTags.Delete();								// init list of once-only code files		
		fLofXhits.Delete(); 									// init list of special hits		
		fLofRdoIncludes.Delete(); 								// init list of MBS includes		
		fLofRdoLibs.Delete(); 									// init list of MBS libs		

		fUserMacroToBeCalled = kFALSE;							// don't call user macro per default
		
		fLofUserIncludes.SetName("User code to be included");
		fLofUserIncludes.Delete();
		fLofUserIncludes.SetPatternMode();

		fLofUserLibs.SetName("User libs to be included");
		fLofUserLibs.Delete();
		fLofUserLibs.SetPatternMode();

		fLofUserClasses.SetName("User classes to be included");
		fLofUserClasses.Delete();
		fLofUserClasses.SetPatternMode();

		fMbsVVersion = "";
		fMbsVersion = "";
		fLynxVersion = "";

		fNofScalers = 0;
		fNofMuxs = 0;

		gMrbConfig = this; 		// holds addr of current config def
		gDirectory->Append(this);
	}
}

TMrbConfig::EMrbTriggerStatus TMrbConfig::GetTriggerStatus(Int_t Trigger) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetTriggerStatus
// Purpose:        Return status info for given trigger
// Arguments:      Int_t Trigger       -- trigger number
// Results:        EMrbTriggerStatus   -- trigger status
//                                        (OutOfRange, Reserved, Ignored ...)
// Exceptions:
// Description:    Performs a table lookup and returns the trigger status.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Trigger <= 0 || Trigger >= kNofTriggers) return(kTriggerOutOfRange);
	return((TMrbConfig::EMrbTriggerStatus) fTriggersToBeHandled[Trigger]);
}

void TMrbConfig::UpdateTriggerTable(Int_t Trigger) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::UpdateTriggerTable
// Purpose:        Update trigger table
// Arguments:      Int_t Trigger      -- trigger number
// Results:        
// Exceptions:
// Description:    Inserts a new trigger into trigger table, then updates
//                 table data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t trg;

	if (Trigger == 0) { 									// w/o argument: initialize trigger table
		fTriggersToBeHandled.Set(kNofTriggers); 			// size: triggers 0 .. 15
		fTriggersToBeHandled.Reset();						// reset to 0 (kMrbCfgTriggerUnused)
		fTriggersToBeHandled[0] = kTriggerOutOfRange; // trigger 0 illegal
		fTriggersToBeHandled[kTriggerStartAcq] = kTriggerReserved;	// triggers 14 & 15 reserved for MBS
		fTriggersToBeHandled[kTriggerStopAcq] = kTriggerReserved;
		fTriggerMask = 0;									// mask to hold single trigger bits
		fSingleBitTriggersOnly = kTRUE; 					// single-bit triggers only so far

	} else if ((Trigger > 0) || (Trigger < kNofTriggers)) {		// with argument "trigger"
															// status "pattern": multiple trigger defined previously
															// status "reserved": skip start and stop striggers
		if (fTriggersToBeHandled[Trigger] != kTriggerPattern && fTriggersToBeHandled[Trigger] != kTriggerReserved) {
			fTriggersToBeHandled[Trigger] = kTriggerAssigned; 	// mark this trigger in trigger table

			if (fSingleBitTriggersOnly) {								// check if single-bit triggers only
				fSingleBitTriggersOnly = (		Trigger == 1
											||	Trigger == 2
											||	Trigger == 4
											||	Trigger == 8);

				if (fSingleBitTriggersOnly) {							// single-bit triggers
					fTriggerMask |= Trigger;							// add bit to trigger mask
					for (trg = 0; trg < kNofTriggers; trg++) {
						if (((trg & fTriggerMask) == (UInt_t) trg)		// disable any trigger patterns not defined so far
						&&  (fTriggersToBeHandled[trg] == kTriggerUnused))
												fTriggersToBeHandled[trg] = kTriggerIgnored;
					}
				} else {
					fTriggerMask = 0;									// mask doesn't make sense from now on
					for (trg = 0; trg < kNofTriggers; trg++) {	// remove pattern assignments from table
						if (fTriggersToBeHandled[trg] == kTriggerPattern)
												fTriggersToBeHandled[trg] = kTriggerUnused;
					}
				}
			}
		}
	}
}

Bool_t TMrbConfig::HandleMultipleTriggers(Int_t T1, Int_t T2, Int_t T3, Int_t T4, Int_t T5) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::HandleMultipleTriggers
// Purpose:        Allow trigger patterns
// Arguments:      Int_t T'n         -- triggers patterns
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    In case of single-bit triggers: allow multiple triggers
//                 to occur. Resulting code is a combination of single triggers.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t trig[5];
	Int_t i;
	Int_t trg;
	UInt_t trigMask;
	TMrbConfig::EMrbTriggerStatus trigStatus;
	TMrbEvent * evt, * multiEvt;

	if (fSingleBitTriggersOnly) {
		if (T1 == 0) {						// w/o arguments:
			for (trg = 0; trg < kNofTriggers; trg++) {
				if ((trg & fTriggerMask) == (UInt_t) trg) {
					trigStatus = (TMrbConfig::EMrbTriggerStatus) fTriggersToBeHandled[trg];
					if (trigStatus == kTriggerUnused
					||  trigStatus == kTriggerIgnored) {
						fTriggersToBeHandled[trg] = kTriggerPattern;
					}
				}
			}
		} else {
			trig[0] = T1;					// pass arguments
			trig[1] = T2;
			trig[2] = T3;
			trig[3] = T4;
			trig[4] = T5;
			for (i = 0; i < 5; i++) {
				trg = trig[i];
				if (trg == 0) break;
				if ((trg & fTriggerMask) == (UInt_t) trg) {
					trigStatus = (TMrbConfig::EMrbTriggerStatus) fTriggersToBeHandled[trg];
					if (trigStatus == kTriggerUnused
					||  trigStatus == kTriggerIgnored) fTriggersToBeHandled[trg] = kTriggerPattern;
				} else {
					gMrbLog->Err() << "Not within trigger mask - " << trg << endl;
					gMrbLog->Flush(this->ClassName(), "HandleMultipleTriggers");
				}
			}
		}
		for (i = 0; i < kNofTriggers; i++) {
			trg = i;
			if (fTriggersToBeHandled[trg] == kTriggerPattern) {
				TString evtName = Form("pattern%d", trg);
				TString evtTitle = Form("multiple trigger ", trg);
				multiEvt = new TMrbEvent_10_1(trg, evtName.Data(), evtTitle.Data());
				trigMask = 1;
				while (trg && trigMask < kNofTriggers) {
					if (trg & trigMask) {
						evt = (TMrbEvent *) this->FindEvent(trigMask);
						evt->ShareSubevents(multiEvt);
						trg &= ~trigMask;
					}
					trigMask <<= 1;
				}
			}
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Legal for single-bit triggers only" << endl;
		gMrbLog->Flush(this->ClassName(), "HandleMultipleTriggers");
		return(kFALSE);
	}
}

TMrbEvent * TMrbConfig::FindEvent(Int_t Trigger) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindEvent
// Purpose:        Find an event by its trigger number
// Arguments:      Int_t Trigger       -- trigger number
// Results:        TMrbEvent * Event   -- event address
// Exceptions:
// Description:    Loops thru the list of events to find specified trigger.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) {
		if (evt->GetTrigger() == Trigger) return(evt);
	}
	return(NULL);
}

TMrbSubevent * TMrbConfig::FindSubevent(Int_t SevtSerial) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindSubevent
// Purpose:        Find a subevent by its serial number
// Arguments:      Int_t SevtSerial         -- unique subevent id
// Results:        TMrbSubevent * Subevent  -- subevent address
// Exceptions:
// Description:    Loops thru the list of subevents to find specified id.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;
	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if (sevt->GetSerial() == SevtSerial) return(sevt);
	}
	return(NULL);
}

TMrbSubevent * TMrbConfig::FindSubevent(TClass * Class, TMrbSubevent * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindSubevent
// Purpose:        Find a subevent by its class
// Arguments:      TClass * Class           -- class
//                 TMrbSubevent * After     -- subevent addr from last call
// Results:        TMrbSubevent * Subevent  -- subevent address
// Exceptions:
// Description:    Loops thru the list of subevents to find specified class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = (After == NULL);
	TMrbSubevent * sevt;
	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if (ok) {
			if (sevt->IsA() == Class) return(sevt);
		} else {
			ok = (sevt == After);
		}
	}
	return(NULL);
}

const Char_t * TMrbConfig::GetMbsVersion(Bool_t Vformat, Bool_t Verbose)  {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetMbsVersion
// Purpose:        Return MBS version
// Arguments:      Bool_t Vformat       -- return old format vNN if kTRUE
//                 Bool_t Verbose       -- output error message if not ok
// Results:        Char_t * MbsVersion  -- result
// Exceptions:
// Description:    Returns MBS version as vNN or N.M
//                 Version is read from rootrc environment: TMbsSetup.MBsVersion
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mbsVersion;
	if (fMbsVersion.IsNull()) {
		mbsVersion = gEnv->GetValue("TMbsSetup.MbsVersion", "");
		if (mbsVersion.IsNull()) {
			if (Verbose) {
				gMrbLog->Err() << "MBS version not defined - set TMbsSetup.MBsVersion in .rootrc properly" << endl;
				gMrbLog->Flush(this->ClassName(), "GetMbsVersion");
			}
			return("");
		}
		if (mbsVersion(0) == 'v') {
			fMbsVersion = Form("%c.%c", mbsVersion(1), mbsVersion(2));
			fMbsVVersion = mbsVersion;
		} else {
			TObjArray * v = mbsVersion.Tokenize(".");
			if (v->GetEntries() != 2) {
				if (Verbose) {
					gMrbLog->Err() << "Wrong format of MBS version - " << mbsVersion << endl;
					gMrbLog->Flush(this->ClassName(), "GetMbsVersion");
				}
				delete v;
				return("");
			}
			TString major = ((TObjString *) v->At(0))->String();
			if (!major.IsDigit()) {
				if (Verbose) {
					gMrbLog->Err() << "Wrong format of MBS version - " << mbsVersion << endl;
					gMrbLog->Flush(this->ClassName(), "GetMbsVersion");
				}
				delete v;
				return("");
			}
			TString minor = ((TObjString *) v->At(1))->String();
			if (!minor.IsDigit()) {
				if (Verbose) {
					gMrbLog->Err() << "Wrong format of MBS version - " << mbsVersion << endl;
					gMrbLog->Flush(this->ClassName(), "GetMbsVersion");
				}
				delete v;
				return("");
			}
			fMbsVVersion = Form("v%s%s", major.Data(), minor.Data());
			fMbsVersion = mbsVersion;
			delete v;
		}
	}

	if (Vformat) {
		return(fMbsVVersion.Data());
	} else {
		return(fMbsVersion.Data());
	}
}

const Char_t * TMrbConfig::GetLynxVersion(Bool_t Verbose)  {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetLynxVersion
// Purpose:        Return LynxOs version
// Arguments:      Bool_t Verbose        -- output error message if not ok
// Results:        Char_t * LynxVersion  -- result
// Exceptions:
// Description:    Returns LynxOs version as N.M
//                 Version is read from rootrc environment: TMbsSetup.LynxVersion
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString lynxVersion;
	if (fLynxVersion.IsNull()) {
		lynxVersion = gEnv->GetValue("TMbsSetup.LynxVersion", "");
		if (lynxVersion.IsNull()) {
			if (Verbose) {
				gMrbLog->Err() << "LynxOs version not defined - set TMbsSetup.LynxVersion in .rootrc properly" << endl;
				gMrbLog->Flush(this->ClassName(), "GetLynxVersion");
			}
			return("");
		}

		TObjArray * v = lynxVersion.Tokenize(".");
		if (v->GetEntries() != 2) {
			if (Verbose) {
				gMrbLog->Err() << "Wrong format of LynxOs version - " << lynxVersion << endl;
				gMrbLog->Flush(this->ClassName(), "GetLynxVersion");
			}
			delete v;
			return("");
		}
		TString major = ((TObjString *) v->At(0))->String();
		if (!major.IsDigit()) {
			if (Verbose) {
				gMrbLog->Err() << "Wrong format of LynxOs version - " << lynxVersion << endl;
				gMrbLog->Flush(this->ClassName(), "GetLynxVersion");
			}
			delete v;
			return("");
		}
		TString minor = ((TObjString *) v->At(1))->String();
		if (!minor.IsDigit()) {
			if (Verbose) {
				gMrbLog->Err() << "Wrong format of LynxOs version - " << lynxVersion << endl;
				gMrbLog->Flush(this->ClassName(), "GetLynxVersion");
			}
			delete v;
			return("");
		}
		fLynxVersion = lynxVersion;
		delete v;
	}

	return(fLynxVersion.Data());
}

const Char_t * TMrbConfig::GetProcType(Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetProcType
// Purpose:        Return processor type
// Arguments:      Bool_t Verbose        -- output error message if not ok
// Results:        Char_t * ProcType     -- result
// Exceptions:
// Description:    Returns ppc type: PPC (=RIO2) or RIO3
//                 Type is read from rootrc environment: TMbsSetup.ProcType
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString procType;
	if (fProcType.IsNull()) {
		procType = gEnv->GetValue("TMbsSetup.ProcType", "");
		if (procType.IsNull()) {
			if (Verbose) {
				gMrbLog->Err() << "Processor type not defined - set TMbsSetup.ProcType in .rootrc properly" << endl;
				gMrbLog->Flush(this->ClassName(), "GetProcType");
			}
			return("");
		} else {
			if (	procType.CompareTo("PPC") != 0
			&&		procType.CompareTo("RIO2") != 0
			&&		procType.CompareTo("RIO3") != 0) {
				if (Verbose) {
					gMrbLog->Err() << "Wrong processor type - " << procType << ", set TMbsSetup.ProcType in .rootrc properly" << endl;
					gMrbLog->Flush(this->ClassName(), "GetProcType");
				}
				return("");
			}
			fProcType = procType;
		}
	}
	return(fProcType.Data());
}

Bool_t TMrbConfig::CheckModuleAddress(TMrbModule * Module, Bool_t WrnOnly) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CheckModuleAddress
// Purpose:        Check if module address or position is legal
// Arguments:      TMrbModule * Module  -- module address
//                 Bool_t WrnOnly       -- give warning only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks module address or position:
//                    CAMAC:   check if B.C.N already accupied
//                    VME:     check if address within other module's segment
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacModule * camac;
	TMrbVMEModule * vme;
	TString pos;

	UInt_t mType;
	TString cnaf;
	UInt_t startAddr, sa;
	UInt_t endAddr, ea;

	mType = Module->GetType()->GetIndex() & (TMrbConfig::kModuleCamac | TMrbConfig::kModuleVME);
	if (mType == TMrbConfig::kModuleCamac) {							// camac module
		cnaf = ((TMrbCamacModule *) Module)->GetPosition(); 			// get camac B.C.N
		camac = (TMrbCamacModule *) this->FindModuleByType(TMrbConfig::kModuleCamac);
		while (camac) { 												// step thru list of camac modules
			if (camac != Module && camac->GetMbsBranchNo() == ((TMrbModule *) Module)->GetMbsBranchNo()) {
				if (cnaf.CompareTo(camac->GetPosition()) == 0) {			// same B.C.N: check if subdevice
					if	((Module->GetModuleID()->GetIndex() != camac->GetModuleID()->GetIndex())
					||	 (Module->GetSubDevice() == camac->GetSubDevice())) {	// id different or subdevice same
						gMrbLog->Wrn()	<< Module->GetName() << ": Camac slot " << cnaf
										<< " already occupied by module " << camac->GetName()
										<< " (" << camac->GetTitle() << ")" << endl;
						if (WrnOnly) gMrbLog->Wrn()	<< "WARNING! This will cause severe address conflicts in a single branch environment!" << endl;
						gMrbLog->Flush(this->ClassName(), "CheckModuleAddress");
						return(WrnOnly ? kTRUE : kFALSE);
					}
				}
			}
			camac = (TMrbCamacModule *) this->FindModuleByType(TMrbConfig::kModuleCamac, camac);
		}
	} else {															// vme module
		startAddr = ((TMrbVMEModule *) Module)->GetBaseAddr();			// get base addr
		endAddr = startAddr + ((TMrbVMEModule *) Module)->GetSegmentSize() - 1;	// + length of segment
		vme = (TMrbVMEModule *) this->FindModuleByType(TMrbConfig::kModuleVME);
		while (vme) { 											// step thru list of vmemodules
			if (vme != Module && vme->GetMbsBranchNo() == ((TMrbModule *) Module)->GetMbsBranchNo()) {
				sa = vme->GetBaseAddr();
				ea = sa + vme->GetSegmentSize() - 1;
				if ((sa >= startAddr && sa <= endAddr) || (ea >= startAddr && ea <= endAddr)) {
					gMrbLog->Wrn()	<< Module->GetName() << ": Address space ["
									<< setiosflags(ios::showbase) << setbase(16)
									<< startAddr << "," << endAddr
									<< "] overlapping with module " << vme->GetName() << " ["
									<< sa << "," << ea << "]"
									<< resetiosflags(ios::showbase) << setbase(10) << endl;
					if (WrnOnly) gMrbLog->Wrn()	<< "WARNING! This will cause severe address conflicts in a single branch environment!" << endl;
					gMrbLog->Flush(this->ClassName(), "CheckModuleAddress");
					return(WrnOnly ? kTRUE : kFALSE);
				}
			}
			vme = (TMrbVMEModule *) this->FindModuleByType(TMrbConfig::kModuleVME, vme);
		}
	}
	return(kTRUE);
}

TMrbModule * TMrbConfig::FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TMrbModule * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindModuleByID
// Purpose:        Find a module by its id
// Arguments:      EMrbModuleID ModuleID     -- module id
//                 TMrbModule * After        -- search to be started after this module
// Results:        TMrbModule * Module       -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 next module having specified id
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	Bool_t ok = (After == NULL);
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (ok) {
			if (module->GetModuleID()->GetIndex() == ModuleID) return(module);
		} else {
			ok = (module == After);
		}
	}
	return(NULL);
}

TMrbModule * TMrbConfig::FindModuleByType(UInt_t ModuleType, TMrbModule * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindModuleByType
// Purpose:        Find a module by its type
// Arguments:      UInt_t ModuleType        -- module type
//                 TMrbModule * After       -- search to be started after this module
// Results:        TMrbModule * Module      -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 next module having specified type
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = (After == NULL);
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (ok) {
			if ((module->GetType()->GetIndex() & ModuleType) == ModuleType) return(module);
		} else {
			ok = (module == After);
		}
	}
	return(NULL);
}

TMrbModule * TMrbConfig::FindModuleByCrate(Int_t Crate, TMrbModule * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindModuleByCrate
// Purpose:        Find a module by crate number
// Arguments:      Int_t Crate              -- crate number
//                 TMrbModule * After       -- search to be started after this module
// Results:        TMrbModule * Module      -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 next module in specified crate
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = (After == NULL);
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (ok) {
			if (module->GetCrate() == Crate) return(module);
		} else {
			ok = (module == After);
		}
	}
	return(NULL);
}

TMrbModule * TMrbConfig::FindModuleBySerial(Int_t ModuleSerial) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindModuleBySerial
// Purpose:        Find a module by its unique serial number
// Arguments:      Int_t ModuleSerial       -- serial number
// Results:        TMrbModule * Module      -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 specified serial number
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->GetSerial() == ModuleSerial) return(module);
	}
	return(NULL);
}

TMrbModule * TMrbConfig::FindScalerByCrate(Int_t Crate, TMrbModule * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindScalerByCrate
// Purpose:        Find a scaler by crate number
// Arguments:      Int_t Crate              -- crate number
//                 TMrbModule * After  -- search to be started after this module
// Results:        TMrbModule * Scaler -- scaler address
// Exceptions:
// Description:    Loops thru the list of scalers to find
//                 next scaler in specified crate.
//                 Returned TMrbModule object has to be casted as TMrbCamacScaler.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = (After == NULL);
	TMrbModule * scaler;
	TIterator * scaIter = fLofScalers.MakeIterator();
	while (scaler = (TMrbModule *) scaIter->Next()) {
		if (ok) {
			TMrbNamedX * type = scaler->GetType();
			if ((type->GetIndex() & TMrbConfig::kModuleCamac) && (((TMrbCamacScaler *)scaler)->GetCrate() == Crate)) return(scaler);
		} else {
			ok = (scaler == After);
		}
	}
	return(NULL);
}

Int_t TMrbConfig::FindCrate(Int_t After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindCrate
// Purpose:        Find next crate
// Arguments:      Int_t After              -- start with crate number ...
// Results:        Int_t Crate              -- crate number or -1
// Exceptions:
// Description:    Loops thru table of crates to find next crate which is active
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t crate;
	TMrbConfig::EMrbCrateType type;
	for (crate = After + 1; crate < TMrbConfig::kNofCrates; crate++) {
		type = this->GetCrateType(crate);
		if (type != TMrbConfig::kCrateUnused) return(crate);
	}
	return(-1);
}

Int_t TMrbConfig::GetNofCrates(EMrbCrateType CrateType) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetNofCrates
// Purpose:        Return number of crates in use
// Arguments:      EMrbCrateType CrateType -- crate type (camac, vme, any)
// Results:        Int_t NofCrates         -- number of crates in use
// Exceptions:
// Description:    Loops thru table of crates and returns number of active crates
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t crate;
	Int_t nofCrates;
	TMrbConfig::EMrbCrateType type;
	nofCrates = 0;
	for (crate = 0; crate < TMrbConfig::kNofCrates; crate++) {
		type = this->GetCrateType(crate);
		if ((type != TMrbConfig::kCrateUnused) && (CrateType & type)) nofCrates++;
	}
	return(nofCrates);
}

UInt_t TMrbConfig::GetCratePattern(EMrbCrateType CrateType) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetCratePattern
// Purpose:        Return number of crates as bit pattern
// Arguments:      EMrbCrateType CrateType     -- crate type (camac, vme, any)
// Results:        UInt_t CratePattern         -- bit pattern
// Exceptions:
// Description:    Loops thru table of crates and returns bit pattern of active crates
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t crate;
	UInt_t cratePattern;
	TMrbConfig::EMrbCrateType type;
	cratePattern = 0;
	for (crate = 0; crate < TMrbConfig::kNofCrates; crate++) {
		type = this->GetCrateType(crate);
		if ((type != TMrbConfig::kCrateUnused) && (CrateType & type)) cratePattern |= BIT(crate);
	}
	return(cratePattern);
}

Bool_t TMrbConfig::SetControllerType(const Char_t * Crate, const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::SetControllerType
// Purpose:        Define type of (CAMAC) controller to be used
// Arguments:      Char_t * Crate        -- crate number (Cxx)
//                 Char_t * Type         -- controller type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines which CAMAC controller to be used.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCNAF cnaf;
	Int_t crate;
		
	cnaf.SetBit(TMrbCNAF::kCnafCrate, TMrbCNAF::kCnafCrate);
	if (cnaf.Ascii2Int(Crate)) {					// decode crate
		crate = cnaf.Get(TMrbCNAF::kCnafCrate);
		return(this->SetControllerType(crate, Type));
	} else {
		gMrbLog->Err() << "Illegal crate number - " << Crate << endl;
		gMrbLog->Flush(this->ClassName(), "SetControllerType");
		return(kFALSE);
	}
}

Bool_t TMrbConfig::SetControllerType(Int_t Crate, const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::SetControllerType
// Purpose:        Define type of (CAMAC) controller to be used
// Arguments:      Int_t Crate                 -- crate number (Cxx)
//                 Char_t * Type               -- controller type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines which CAMAC controller to be used.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	EMrbCrateType cType;
	TMrbNamedX * cntrl;
	
	cType = gMrbConfig->GetCrateType(Crate);
	if (cType == TMrbConfig::kCrateUnused || cType == TMrbConfig::kCrateCamac) {
		if ((cntrl = fLofControllerTypes.FindByName(Type, TMrbLofNamedX::kFindUnique)) == NULL) {
			gMrbLog->Err() << "Illegal CAMAC controller - " << Type << endl;
			gMrbLog->Flush(this->ClassName());
			return(kFALSE);
		}
		this->SetCrateType(Crate, TMrbConfig::kCrateCamac);			// mark camac crate active
		this->SetControllerType(Crate, (EMrbControllerType) cntrl->GetIndex());
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Crate C" << Crate << " is not a CAMAC crate" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	}
}

TMrbModuleChannel * TMrbConfig::FindParam(const Char_t * ParamName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Findparam
// Purpose:        Find a parameter thru all subevents
// Arguments:      Char_t * ParamName        -- param name
// Results:        TMrbCamacScaler * Param   -- param/channel addr
// Exceptions:
// Description:    Search for a parameter name throughout the system.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;
	TMrbModuleChannel * param;

	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if ((param = sevt->FindParam(ParamName)) != NULL) return(param);
	}
	return(NULL);
}

Bool_t TMrbConfig::HistogramExists(const Char_t * HistoName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::HistogramExists
// Purpose:        Check if histogram exists
// Arguments:      Char_t * HistoName   -- histo name
// Results:        TMrbNamedX * Param   -- param/channel addr
// Exceptions:
// Description:    Search for a histogram name throughout the system.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fLofUserHistograms.FindObject(HistoName)) return(kTRUE);

	TMrbSubevent * sevt;
	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		TMrbModuleChannel * param;
		TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
		while (param = (TMrbModuleChannel *) paramIter->Next()) {
			TString pName = param->GetName();
			pName(0,1).ToUpper();
			pName.Prepend("h");
			if (pName.CompareTo(HistoName) == 0) return(kTRUE);
		}
	}
	return(kFALSE);
}

Bool_t TMrbConfig::MakeReadoutCode(const Char_t * CodeFile, Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeReadoutCode
// Purpose:        Generate user-defined code for MBS
// Arguments:      Char_t * CodeFile         -- name of code file to be generated
//                 Option_t * Options          -- options to be used
// Results:
// Exceptions:
// Description:    Opens a file for user-defined readout code. Calls write-functions for all
//                 members recursively.
//
//                 Template file should contain tags like %%TAG_NAME%%
//                 They will be expanded to real code by MakeReadoutCode and its descendants.
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register Int_t i;
	TMrbEvent * evt;
	TMrbSubevent * sevt;
	TMrbModule * module;
	TMrbVMEModule * vmeModule;
	TString cf, tf, tf1, tf2;
	TString expName, expTitle;
	TString moduleName, sevtName;
	TString line;
	TString cfile;
	EMrbControllerType contrType;
	TMrbNamedX * camacController;
	TMrbNamedX * readoutTag;
	TMrbConfig::EMrbReadoutTag tagIdx;
	Int_t crate;
	TMrbNamedX * cType;
	Int_t trg;
	UInt_t trigMask;
	Bool_t wtstmpFlag;

	TString templatePath;
	TString rdoTemplateFile;
	TMrbString iniTag;
	TString prefix;

	TString pos;
		
	ofstream rdoStrm;

	TMrbTemplate rdoTmpl;

	TObjArray filesToCreate;

	TMrbLofNamedX readoutOptions;
	
	fReadoutOptions = readoutOptions.CheckPatternShort(this->ClassName(), "MakeReadoutCode", Options, kMrbReadoutOptions);
	if (fReadoutOptions == TMrbLofNamedX::kIllIndexBit) return(kFALSE);

	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	Bool_t verboseMode = (this->IsVerbose() || (this->GetReadoutOptions() & kRdoOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	prefix = this->GetName();
	prefix(0,1).ToUpper();
	
	if (*CodeFile == '\0') {
		cfile = prefix;
		cfile += "Readout";
	} else {
		cfile = CodeFile;
	}

	packNames * pp;

	TString mbsVersion = this->GetMbsVersion(kFALSE, kTRUE);
	if (mbsVersion.IsNull()) return(kFALSE);
	TString lynxVersion = this->GetLynxVersion(kTRUE);
	if (lynxVersion.IsNull()) return(kFALSE);
	TString procType = this->GetProcType(kTRUE);
	if (procType.IsNull()) return(kFALSE);

	TString mbsv = Form("v%cx", mbsVersion(0));			// make file depends on major mbs version: v22 -> v2x, v42 & v43 -> v4x
	TString mkFile = Form("Readout_%s.mk.code", mbsv.Data());

	Int_t nofMbsBranches = fLofMbsBranches.GetEntriesFast();
	if (nofMbsBranches == 0) {
		pp = new packNames(cfile.Data(), "Readout.c.code", ".c", "C code (VME/CAMAC readout) for MBS");
		filesToCreate.Add(pp);

		pp = new packNames(cfile.Data(), "Readout.h.code", ".h", "C definitions for MBS");
		filesToCreate.Add(pp);

		TString cmt = Form("Makefile (LynxOs %s, MBS %s)", lynxVersion.Data(), mbsVersion.Data());
		pp = new packNames(cfile.Data(), mkFile.Data(), ".mk", cmt.Data());
		filesToCreate.Add(pp);
	} else {
		TIterator * bIter = fLofMbsBranches.MakeIterator();
		TMrbNamedX * branch;
		while (branch = (TMrbNamedX *) bIter->Next()) {
			Int_t bNo = branch->GetIndex();
			TString bName = branch->GetName();
			bName(0,1).ToUpper();

			TString bfile = Form("%s%s", cfile.Data(), bName.Data());

			TString cmt = Form("C code (VME/CAMAC readout) for MBS (%s)", bName.Data());
			pp = new packNames(bfile.Data(), "Readout.c.code", ".c", cmt.Data(), bNo);
			filesToCreate.Add(pp);

			cmt = Form("C definitions for MBS (%s)", bName.Data());
			pp = new packNames(bfile.Data(), "Readout.h.code", ".h", cmt.Data(), bNo);
			filesToCreate.Add(pp);

			cmt = Form("Makefile (LynxOs %s, MBS %s) (%s)", lynxVersion.Data(), mbsVersion.Data(), bName.Data());
			pp = new packNames(bfile.Data(), mkFile.Data(), ".mk", cmt.Data(), bNo);
			filesToCreate.Add(pp);
		}
	}

	TIterator * ppIter = filesToCreate.MakeIterator();
	while (pp  = (packNames *) ppIter->Next()) {
		cf = pp->GetF() + pp->GetX();
		if (((this->GetReadoutOptions() & kRdoOptOverwrite) == 0) && (gSystem->AccessPathName(cf) == 0)) {
			gMrbLog->Err() << "File exists - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
			continue;
		}
		rdoStrm.open(cf, ios::out);
		if (!rdoStrm.good()) {	
			gMrbLog->Err() << gSystem->GetError() << " - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
			continue;
		}

		tf = pp->GetT();
		tf.Prepend(prefix.Data());
		tf1 = tf;
		TString fileSpec;
		TMrbSystem ux;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			tf = pp->GetT();
			tf2 = tf;
			ux.Which(fileSpec, templatePath.Data(), tf.Data());
		}
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "Template file not found -" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
			gMrbLog->Err()	<< "            Searching on path " << templatePath << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            for file " << tf1 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            or       " << tf2 << endl;
			gMrbLog->Flush();
			rdoStrm.close();
			continue;
		}

		rdoTemplateFile = fileSpec;

		if (!rdoTmpl.Open(rdoTemplateFile, &fLofReadoutTags)) {
			if (verboseMode) {
				gMrbLog->Err()  << "Skipping template file - " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
			}
			continue;
		} else if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		}

		for (;;) {
			readoutTag = rdoTmpl.Next(line);
			if (rdoTmpl.IsEof()) break;
			if (rdoTmpl.IsError()) continue;
			if (rdoTmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) rdoStrm << line << endl;
			} else if (this->ExecUserMacro(&rdoStrm, this, readoutTag->GetName())) {
				continue;
			} else {
				switch (tagIdx = (TMrbConfig::EMrbReadoutTag) readoutTag->GetIndex()) {
					case TMrbConfig::kRdoPath:
						{
							TString pwd = gSystem->Getenv("PWD");
							if (pwd.Length() == 0) pwd = gSystem->WorkingDirectory();
							rdoStrm << rdoTmpl.Encode(line, pwd.Data()) << endl;
						}
						break;
					case TMrbConfig::kRdoFile:
						rdoStrm << rdoTmpl.Encode(line, pp->GetF()) << endl;
						break;
					case TMrbConfig::kRdoInclude:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefineIncludePaths);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
							TObjString * o;
							TString iclPath = "";
							TIterator * objIter = fLofRdoIncludes.MakeIterator();
							while (o = (TObjString *) objIter->Next()) {
								iclPath += o->String();
								iclPath += " ";
							}
							iclPath += "-I";
							TString ip = gEnv->GetValue("TMrbConfig.ReadoutIncludePath", "$(MARABOU)/powerpc/include");
							gSystem->ExpandPathName(ip);
							iclPath += ip;
							iclPath.ReplaceAll("/share/cernlib", "/usr/cern");
							rdoStrm << rdoTmpl.Encode(line, iclPath.Data()) << endl << endl;
						}
						break;
					case TMrbConfig::kRdoLibs:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefineLibraries);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
							TObjString * o;
							TString libString = "";
							TIterator * objIter = fLofRdoLibs.MakeIterator();
							while (o = (TObjString *) objIter->Next()) {
								libString += o->String();
								libString += " ";
							}
							TString rdoLibs = "$(MARABOU)/powerpc/lib/";
							rdoLibs += this->GetMbsVersion(kTRUE, kTRUE);
							rdoLibs += "/lib_utils.a";
							TString ip = gEnv->GetValue("TMrbConfig.ReadoutLibs", rdoLibs.Data());
							gSystem->ExpandPathName(ip);
							libString += ip;
							libString.ReplaceAll("/share/cernlib", "/usr/cern");
							rdoStrm << rdoTmpl.Encode(line, libString.Data()) << endl << endl;
						}
						break;
					case TMrbConfig::kRdoAlign64:
						{
							Bool_t align64 = gEnv->GetValue("TMrbConfig.AlignTo64Bits", kFALSE);
							if (align64) {
								rdoTmpl.InitializeCode();
								rdoTmpl.WriteCode(rdoStrm);
							}
						}
						break;
					case TMrbConfig::kRdoPosix:
						{
							TString posixFlags = gEnv->GetValue("TMrbConfig.ReadoutPosixFlags", "");
							if (posixFlags.Length() == 0) {
								if (fMbsVVersion.Contains("v2")) 		posixFlags = "-mposix4d9 -mthreads";
								else if (fMbsVVersion.Contains("v4"))	posixFlags = "-D_THREADS_POSIX4ad4 -mthreads";
							}
							rdoStrm << rdoTmpl.Encode(line, posixFlags.Data()) << endl;
						}
						break;
					case TMrbConfig::kRdoPosixLib:
						{
							TString posixLib = gEnv->GetValue("TMrbConfig.ReadoutPosixLib", "");
							if (posixLib.Length() == 0) {
								if (fMbsVVersion.Contains("v2")) 		posixLib = "";
								else if (fMbsVVersion.Contains("v4"))	posixLib = "-lposix-pre1c";
							}
							rdoStrm << rdoTmpl.Encode(line, posixLib.Data()) << endl;
						}
						break;
					case TMrbConfig::kRdoLynxPlatform:
						{
							if (fMbsVVersion.Contains("v2")) {
								rdoStrm << rdoTmpl.Encode(line, "GSI_LYNX_PROC_FAM") << endl;
							} else if (fMbsVVersion.Contains("v4")) {
								rdoStrm << rdoTmpl.Encode(line, "GSI_LYNX_PLATFORM") << endl;
							}
						}
						break;
					case TMrbConfig::kRdoDebug:
						if (gEnv->GetValue("TMrbConfig.DebugReadout", kFALSE)) {
							rdoStrm << rdoTmpl.Encode(line, "m_read_meb_debug") << endl;
						} else {
							rdoStrm << rdoTmpl.Encode(line, "m_read_meb_no_debug") << endl;
						}
						break;
					case TMrbConfig::kRdoCamacController:
						{
							UInt_t camacPattern = this->GetCratePattern(kCrateCamac);
							TString ctrlName = "NoCamac";
							if (camacPattern != 0) {
								contrType = this->GetControllerType(1);
								camacController = fLofControllerTypes.FindByIndex((UInt_t) contrType);
								ctrlName = camacController->GetName();
							}
							rdoStrm << rdoTmpl.Encode(line, ctrlName.Data()) << endl;
						}
						break;
					case TMrbConfig::kRdoNameLC:
						rdoStrm << rdoTmpl.Encode(line, GetName()) << endl;
						break;
					case TMrbConfig::kRdoNameUC:
						expName = this->GetName();
						expName(0,1).ToUpper();
						rdoStrm << rdoTmpl.Encode(line, expName) << endl;
						break;
					case TMrbConfig::kRdoTitle:
						expTitle = this->GetName();
						expTitle += ": ";
						expTitle += this->GetTitle();
						rdoStrm << rdoTmpl.Encode(line, expTitle) << endl;
						break;
					case TMrbConfig::kRdoSetup:
						this->Print(rdoStrm, "//	");
						break;
					case TMrbConfig::kRdoAuthor:
						rdoStrm << rdoTmpl.Encode(line, fAuthor) << endl;
						break;
					case TMrbConfig::kRdoUser:
						rdoStrm << rdoTmpl.Encode(line, fUser) << endl;
						break;
					case TMrbConfig::kRdoCreationDate:
						rdoStrm << rdoTmpl.Encode(line, fCreationDate) << endl;
						break;
					case TMrbConfig::kRdoDefinePointers:
						{
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (module->IsVME()) {
									if (module->GetNofSubDevices() <= 1) {
										rdoTmpl.InitializeCode("%V%");
									} else {
										rdoTmpl.InitializeCode("%VS%");
										rdoTmpl.Substitute("$subDevice", module->GetSubDevice());
									}
								} else {
									rdoTmpl.InitializeCode("%C%");
								}
								rdoTmpl.Substitute("$moduleName", module->GetName());
								if (module->IsCamac())
										rdoTmpl.Substitute("$modulePosition", ((TMrbCamacModule *) module)->GetPosition());
								else if (module->IsVME())
										rdoTmpl.Substitute("$modulePosition", ((TMrbVMEModule *) module)->GetPosition());
								else	rdoTmpl.Substitute("$modulePosition", "");
								rdoTmpl.Substitute("$moduleTitle", module->GetTitle());
								rdoTmpl.WriteCode(rdoStrm);
							}
							crate = this->FindCrate();
							while (crate >= 0) {
								if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
									Int_t cc = this->GetControllerType(crate);
									if (cc == TMrbConfig::kControllerCBV) rdoTmpl.InitializeCode("%CBV%");
									else if (cc == TMrbConfig::kControllerCC32) rdoTmpl.InitializeCode("%CC32%");
									rdoTmpl.Substitute("$crateNo", crate);
									rdoTmpl.WriteCode(rdoStrm);
								}
								crate = this->FindCrate(crate);
							}
						}
						break;
					case TMrbConfig::kRdoDeviceTables:
						{
							rdoTmpl.InitializeCode("%MTA%");
							rdoTmpl.WriteCode(rdoStrm);
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								rdoTmpl.InitializeCode("%MT%");
								rdoTmpl.Substitute("$moduleNameLC", module->GetName());
								moduleName = module->GetName();
								moduleName(0,1).ToUpper();
								rdoTmpl.Substitute("$moduleNameUC", moduleName.Data());
								Int_t lam = (module->IsCamac()) ?
											(Int_t) (0x1 << (((TMrbCamacModule *) module)->GetStation() - 1)) : 0;
								rdoTmpl.Substitute("$lam", lam, 16);
								rdoTmpl.Substitute("$crate", module->GetCrate());
								Int_t station = (module->IsCamac()) ? ((TMrbCamacModule *) module)->GetStation() : 0;
								rdoTmpl.Substitute("$station", station);
								rdoTmpl.Substitute("$nofParams", module->GetNofChannels());
								rdoTmpl.Substitute("$nofParUsed", module->GetNofChannelsUsed());
								rdoTmpl.Substitute("$paramPattern", (Int_t) module->GetPatternOfChannelsUsed(), 16);
								rdoTmpl.WriteCode(rdoStrm);
							}
							rdoTmpl.InitializeCode("%MTE%");
							rdoTmpl.WriteCode(rdoStrm);
							rdoTmpl.InitializeCode("%STA%");
							rdoTmpl.WriteCode(rdoStrm);
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								if (sevt->GetMbsBranchNo() != pp->GetB()) continue;
								Int_t nofModules = sevt->GetNofModules();
								rdoTmpl.InitializeCode((nofModules == 0) ? "%ST0%" : "%ST%");
								rdoTmpl.Substitute("$sevtNameLC", sevt->GetName());
								sevtName = sevt->GetName();
								sevtName(0,1).ToUpper();
								rdoTmpl.Substitute("$sevtNameUC", sevtName.Data());
								rdoTmpl.Substitute("$nofModules", nofModules);
								if (nofModules > 0) {
									module = (TMrbModule *) sevt->GetLofModules()->First();
									moduleName = module->GetName();
									moduleName(0,1).ToUpper();
									rdoTmpl.Substitute("$firstModule", moduleName.Data());
									Int_t lam = 0;
									TIterator * modIter = sevt->GetLofModules()->MakeIterator();
									while (module = (TMrbModule *) modIter->Next()) {
										lam |= (module->IsCamac()) ?
											(Int_t) (0x1 << (((TMrbCamacModule *) module)->GetStation() - 1)) : 0;
									}
									rdoTmpl.Substitute("$lam", lam, 16);
								}
								rdoTmpl.WriteCode(rdoStrm);
							}
							rdoTmpl.InitializeCode("%STE%");
							rdoTmpl.WriteCode(rdoStrm);
						}
						break;
					case TMrbConfig::kRdoInitPointers:
						{
							if ((vmeModule = (TMrbVMEModule *) this->FindModuleByType(TMrbConfig::kModuleVME)) != NULL) {
								rdoTmpl.InitializeCode("%BV%");
								rdoTmpl.WriteCode(rdoStrm);
							}
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() == pp->GetB()) module->MakeReadoutCode(rdoStrm, tagIdx, rdoTmpl, NULL);
							}
							crate = this->FindCrate();
							while (crate >= 0) {
								if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
									iniTag = "%";
									Int_t cc = this->GetControllerType(crate);
									if (cc == TMrbConfig::kControllerUnused) cc = TMrbConfig::kControllerCBV;
									TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
									iniTag += ccx->GetName();
									iniTag += "%";
									rdoTmpl.InitializeCode(iniTag.Data());
									rdoTmpl.Substitute("$crateNo", crate);
									rdoTmpl.WriteCode(rdoStrm);
								}
								crate = this->FindCrate(crate);
							}
						}
						break;
					case TMrbConfig::kRdoInitEnvironment:
						rdoTmpl.InitializeCode();
						expName = this->GetName();
						expName(0,1).ToUpper();
						expName.Prepend("/.");
						expName.Prepend(gSystem->WorkingDirectory());
						expName += "Config.rc";
						rdoTmpl.Substitute("$wDir", gSystem->WorkingDirectory());
						rdoTmpl.Substitute("$envFile", expName.Data());
						rdoTmpl.WriteCode(rdoStrm);
						break;
					case TMrbConfig::kRdoInitCommonCode:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleInitCommonCode);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoInitModules:
						rdoTmpl.InitializeCode("%B%");
						rdoTmpl.WriteCode(rdoStrm);
						crate = this->FindCrate();
						while (crate >= 0) {
							module = (TMrbModule *) this->FindModuleByCrate(crate);
							if (module) {
								rdoTmpl.InitializeCode("%CB%");
								rdoTmpl.Substitute("$crateNo", crate);
								cType = this->fLofCrateTypes.FindByIndex(this->GetCrateType(crate));
								rdoTmpl.Substitute("$crateType", cType->GetName());
								rdoTmpl.WriteCode(rdoStrm);
								if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
									iniTag = "%CB";
									Int_t cc = this->GetControllerType(crate);
									TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
									iniTag += ccx->GetName();
									iniTag += "%";
									rdoTmpl.InitializeCode(iniTag.Data());
									rdoTmpl.Substitute("$crateNo", crate);
									rdoTmpl.WriteCode(rdoStrm);
								}
							}
							while (module) {
								if (module->GetMbsBranchNo() == pp->GetB()) module->MakeReadoutCode(rdoStrm, kModuleInitModule);
								module = (TMrbModule *) this->FindModuleByCrate(crate, module);
							}
							if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
								iniTag = "%CE";
								Int_t cc = this->GetControllerType(crate);
								TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
								iniTag += ccx->GetName();
								iniTag += "%";
								rdoTmpl.InitializeCode(iniTag.Data());
								rdoTmpl.Substitute("$crateNo", crate);
								rdoTmpl.WriteCode(rdoStrm);
							}
							crate = this->FindCrate(crate);
							rdoTmpl.InitializeCode("%CE%");
							rdoTmpl.WriteCode(rdoStrm);
						}
						rdoTmpl.InitializeCode("%E%");
						rdoTmpl.WriteCode(rdoStrm);
						break;
					case TMrbConfig::kRdoOnTriggerXX:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (!evt->IsReservedEvent()) {
									if (evt->SelectMbsBranch(pp->GetB())) {
										evt->MakeReadoutCode(rdoStrm, tagIdx, rdoTmpl);
									} else {
										rdoTmpl.InitializeCode("%MB%");
										rdoTmpl.Substitute("$trigNo", evt->GetTrigger());
										rdoTmpl.Substitute("$branch", pp->GetB());
										rdoTmpl.WriteCode(rdoStrm);
									}
								}
							}
						}
						break;
					case TMrbConfig::kRdoIgnoreTriggerXX:
						for (i = 0; i < kNofTriggers; i++) {
							trg = i;
							if (((trg & fTriggerMask) == (UInt_t) trg)
							&&  (fTriggersToBeHandled[trg] == kTriggerIgnored)) {
								rdoTmpl.InitializeCode("%C%");
								rdoTmpl.Substitute("$trigNo", (Int_t) trg);
								rdoTmpl.WriteCode(rdoStrm);
								trigMask = 1;
								while (trg && trigMask < kNofTriggers) {
									if (trg & trigMask) {
										evt = (TMrbEvent *) this->FindEvent(trigMask);
										evt->MakeReadoutCode(rdoStrm, kRdoIgnoreTriggerXX, rdoTmpl);
										trg &= ~trigMask;
									}
									trigMask <<= 1;
								}
								rdoTmpl.InitializeCode("%E%");
								rdoTmpl.WriteCode(rdoStrm);
							}
						}
						break;
					case TMrbConfig::kRdoOnStartAcquisition:
						rdoTmpl.InitializeCode("%B%");
						rdoTmpl.WriteCode(rdoStrm);
						wtstmpFlag = kFALSE;
						crate = this->FindCrate();
						while (crate >= 0) {
							rdoTmpl.InitializeCode("%CB%");
							rdoTmpl.Substitute("$crateNo", crate);
							cType = this->fLofCrateTypes.FindByIndex(this->GetCrateType(crate));
							rdoTmpl.Substitute("$crateType", cType->GetName());
							rdoTmpl.WriteCode(rdoStrm);
							if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
								iniTag = "%CB";
								Int_t cc = this->GetControllerType(crate);
								TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
								iniTag += ccx->GetName();
								iniTag += "%";
								rdoTmpl.InitializeCode(iniTag.Data());
								rdoTmpl.Substitute("$crateNo", crate);
								rdoTmpl.WriteCode(rdoStrm);
							}
							if (!wtstmpFlag) {
								rdoTmpl.InitializeCode("%CT%");
								rdoTmpl.WriteCode(rdoStrm);
								wtstmpFlag = kTRUE;
							}
							module = (TMrbModule *) this->FindModuleByCrate(crate);
							while (module) {
								if (module->GetMbsBranchNo() == pp->GetB()) module->MakeReadoutCode(rdoStrm, kModuleStartAcquisition);
								module = (TMrbModule *) this->FindModuleByCrate(crate, module);
							}
							if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
								iniTag = "%CE";
								Int_t cc = this->GetControllerType(crate);
								TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
								iniTag += ccx->GetName();
								iniTag += "%";
								rdoTmpl.InitializeCode(iniTag.Data());
								rdoTmpl.Substitute("$crateNo", crate);
								rdoTmpl.WriteCode(rdoStrm);
							}
							evt = (TMrbEvent *) this->FindEvent(TMrbConfig::kTriggerStartAcq);
							if (evt) {
								TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
								while (sevt = (TMrbSubevent *) sevtIter->Next()) {
									if (sevt->GetMbsBranchNo() == pp->GetB() && sevt->GetCrate() == crate) sevt->MakeReadoutCode(rdoStrm, TMrbConfig::kRdoOnTriggerXX, rdoTmpl);
								}
							}
							rdoTmpl.InitializeCode("%CE%");
							rdoTmpl.WriteCode(rdoStrm);
							crate = this->FindCrate(crate);
						}   
						rdoTmpl.InitializeCode("%E%");
						rdoTmpl.WriteCode(rdoStrm);
						break;
					case TMrbConfig::kRdoOnStopAcquisition:
						rdoTmpl.InitializeCode("%B%");
						rdoTmpl.WriteCode(rdoStrm);
						wtstmpFlag = kFALSE;
						crate = this->FindCrate();
						while (crate >= 0) {
							rdoTmpl.InitializeCode("%CB%");
							rdoTmpl.Substitute("$crateNo", crate);
							cType = this->fLofCrateTypes.FindByIndex(this->GetCrateType(crate));
							rdoTmpl.Substitute("$crateType", cType->GetName());
							rdoTmpl.WriteCode(rdoStrm);
							if (this->GetCrateType(crate) == TMrbConfig::kCrateCamac) {
								iniTag = "%CB";
								Int_t cc = this->GetControllerType(crate);
								TMrbNamedX * ccx = fLofControllerTypes.FindByIndex(cc);
								iniTag += ccx->GetName();
								iniTag += "%";
								rdoTmpl.InitializeCode(iniTag.Data());
								rdoTmpl.Substitute("$crateNo", crate);
								rdoTmpl.WriteCode(rdoStrm);
							}
							if (!wtstmpFlag) {
								rdoTmpl.InitializeCode("%CT%");
								rdoTmpl.WriteCode(rdoStrm);
								wtstmpFlag = kTRUE;
							}
							module = (TMrbModule *) this->FindModuleByCrate(crate);
							while (module) {
								if (module->GetMbsBranchNo() == pp->GetB()) module->MakeReadoutCode(rdoStrm, kModuleStopAcquisition);
								module = (TMrbModule *) this->FindModuleByCrate(crate, module);
							}
							evt = (TMrbEvent *) this->FindEvent(TMrbConfig::kTriggerStopAcq);
							if (evt) {
								TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
								while (sevt = (TMrbSubevent *) sevtIter->Next()) {
									if (sevt->GetMbsBranchNo() == pp->GetB() && sevt->GetCrate() == crate) sevt->MakeReadoutCode(rdoStrm, TMrbConfig::kRdoOnTriggerXX, rdoTmpl);
								}
							}
							rdoTmpl.InitializeCode("%CE%");
							rdoTmpl.WriteCode(rdoStrm);
							crate = this->FindCrate(crate);
						}   
						rdoTmpl.InitializeCode("%E%");
						rdoTmpl.WriteCode(rdoStrm);
						break;
					case TMrbConfig::kRdoDefineGlobalsOnce:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefineGlobalsOnce);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoDefineGlobals:
						{
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() == pp->GetB()) module->MakeReadoutCode(rdoStrm, kModuleDefineGlobals);
							}   
						}
						break;
					case TMrbConfig::kRdoDefineLocalVarsInit:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefineLocalVarsInit);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoDefineLocalVarsReadout:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefineLocalVarsReadout);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoDefinePrototypes:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefinePrototypes);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoUtilities:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleUtilities);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoIncludesAndDefs:
						{
							TObjArray onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetMbsBranchNo() != pp->GetB()) continue;
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeReadoutCode(rdoStrm, kModuleDefs);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kRdoUserDefinedDefines:
						{
							TIterator * defIter = fLofDefines.MakeIterator();
							TMrbNamedX * nx;
							while (nx = (TMrbNamedX *) defIter->Next()) {
								TString dName = nx->GetName();
								TString dMode = nx->GetTitle();
								if (dMode.BeginsWith("Bool_t")) {
									if ((Bool_t) nx->GetIndex()) {
										rdoTmpl.InitializeCode("%DB%");
										rdoTmpl.Substitute("$dName", dName.Data());
										rdoTmpl.WriteCode(rdoStrm);
									}
								} else if (dMode.BeginsWith("Int_t")) {
									rdoTmpl.InitializeCode("%DV%");
									rdoTmpl.Substitute("$dName", dName.Data());
									rdoTmpl.Substitute("$dVal", nx->GetIndex());
									rdoTmpl.WriteCode(rdoStrm);
								}
							}
						}
						break;
				}
			}
		}	
		rdoStrm.close();
		gMrbLog->Out() << "[" << cf << ": " << pp->GetC() << "]" << endl;
		gMrbLog->Flush("", "", setblue);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::MakeAnalyzeCode(const Char_t * CodeFile, Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeAnalyzeCode
// Purpose:        Generate user-defined code to analyze MBS data
// Arguments:      Char_t * CodeFile         -- name of code file to be generated
//                 Option_t * Options        -- how to generate code: default "Subevents:byName:Histograms"
//                                              Subevents: event->subevent->params
//                                              byName: each param has its own name
//                                              byIndex: array of params, accessed by index
//                                              Histograms: generates histos for all params
//                                              Overwrite: overwrite existing code files
// Results:
// Exceptions:
// Description:    Opens a file for user-defined analyzing code. Calls write-functions for all
//                 members recursively.
//
//                 Template file should contain tags like %%TAG_NAME%%
//                 They will be expanded to real code by MakeAnalyzeCode and its descendants.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t found;
	Bool_t first;
	Bool_t header;
	TMrbEvent * evt;
	TMrbSubevent * sevt;
	TMrbModule * module;
	TMrbModuleChannel * param;
	TString line;
	TString cfile;
	TString clibf, cidxf;
	TString cf, tf, tf1, tf2;
	TString expName;
	TString evtNameUC;
	TString evtNameLC;
	TString sevtNameUC;
	TString sevtNameLC;
	TString moduleNameUC;
	TString moduleNameLC;
	TString paramNameUC;
	TString paramNameLC;
	TMrbNamedX * analyzeTag;
	TMrbConfig::EMrbAnalyzeTag tagIdx;
	Int_t nofParams;
	Int_t trg;
	Bool_t ignTrigger;

	TString templatePath;
	TString anaTemplateFile;
	TString prefix;
	TString rcFile;
		
	TMrbNamedX * icl;
	TString iclFile, iclPrefix, iclEvt;
	TString srcPath;

	ofstream anaStrm;

	TMrbTemplate anaTmpl;

	TObjArray filesToCreate;

	TMrbLofNamedX analyzeOptions;
	
	fAnalyzeOptions = analyzeOptions.CheckPatternShort(this->ClassName(), "MakeAnalyzeCode", Options, kMrbAnalyzeOptions);
	if (fAnalyzeOptions == TMrbLofNamedX::kIllIndexBit) return(kFALSE);

	if (fNofEvents == 0) {
		gMrbLog->Err() << "No events/triggers defined" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		return(kFALSE);
	}

	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0) ;

	srcPath = gEnv->GetValue("TMrbConfig.SrcPath", gSystem->WorkingDirectory());
	gSystem->ExpandPathName(srcPath);
	
	prefix = this->GetName();
	prefix(0,1).ToUpper();
	
	if (*CodeFile == '\0') {
		cfile = prefix;
		cfile += "Analyze";
		clibf = prefix;
		cidxf = prefix;
	} else {
		cfile = CodeFile;
		clibf = CodeFile;
		cidxf = CodeFile;
		clibf += "LoadLibs";
		cidxf += "CommonIndices";
	}

	packNames acxx(cfile.Data(), "Analyze.cxx.code", ".cxx", "C++ code to be used with ROOT");
	filesToCreate.Add((TObject *) &acxx);

	packNames ah(cfile.Data(), "Analyze.h.code", ".h", "C++ class definitions");
	filesToCreate.Add((TObject *) &ah);

	packNames aldf(cfile.Data(), "AnalyzeLinkDef.h.code", "LinkDef.h", "CINT directives");
	filesToCreate.Add((TObject *) &aldf);

	packNames amk(cfile.Data(), "Analyze.mk.code", ".mk", "Makefile (ROOT)");
	filesToCreate.Add((TObject *) &amk);

	packNames agl(cfile.Data(), "AnalyzeGlobals.h.code", "Globals.h", "User's global pointers (ROOT)");
	filesToCreate.Add((TObject *) &agl);

	packNames aul(clibf.Data(), "LoadLibs.C.code", "LoadLibs.C", "Macro to load libs for an interactive ROOT session");
	filesToCreate.Add((TObject *) &aul);

	packNames ci(cidxf.Data(), "CommonIndices.h.code", "CommonIndices.h", "Common indices for analysis AND readout");
	filesToCreate.Add((TObject *) &ci);

	packNames aht(cfile.Data(), "Analyze.html.code", ".html", "HTML documentation, class index");
	filesToCreate.Add((TObject *) &aht);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	this->WriteUtilityProtos();

	packNames * pp;
	TIterator * ppIter = filesToCreate.MakeIterator();
	while (pp = (packNames *) ppIter->Next()) {
		cf = pp->GetF() + pp->GetX();
		if (((this->GetAnalyzeOptions() & kAnaOptOverwrite) == 0) && (gSystem->AccessPathName(cf) == 0)) {
			gMrbLog->Err() << "File exists - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
			continue;
		}
		anaStrm.open(cf, ios::out);
		if (!anaStrm.good()) {	
			gMrbLog->Err() << gSystem->GetError() << " - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
			continue;
		}

		tf = pp->GetT();
		tf.Prepend(prefix.Data());
		tf1 = tf;
		TString fileSpec;
		TMrbSystem ux;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			tf = pp->GetT();
			tf2 = tf;
			ux.Which(fileSpec, templatePath.Data(), tf.Data());
		}
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "Template file not found -" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
			gMrbLog->Err()	<< "            Searching on path " << templatePath << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            for file " << tf1 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            or       " << tf2 << endl;
			gMrbLog->Flush();
			anaStrm.close();
			continue;
		}

		if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		}
	
		anaTemplateFile = fileSpec;

		if (!anaTmpl.Open(anaTemplateFile, &fLofAnalyzeTags)) {
			if (verboseMode) {
				gMrbLog->Err()  << "Skipping template file " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
			}
			continue;
		} else if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		}

		for (;;) {
			analyzeTag = anaTmpl.Next(line);
			if (anaTmpl.IsEof()) break;
			if (anaTmpl.IsError()) continue;
			if (anaTmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) anaStrm << line << endl;
			} else if (this->ExecUserMacro(&anaStrm, this, analyzeTag->GetName())) {
				continue;
			} else {
				switch (tagIdx = (TMrbConfig::EMrbAnalyzeTag) analyzeTag->GetIndex()) {
					case TMrbConfig::kAnaFile:
						anaStrm << anaTmpl.Encode(line, cfile) << endl;
						break;
					case TMrbConfig::kAnaNameLC:
						anaStrm << anaTmpl.Encode(line, this->GetName()) << endl;
						break;
					case TMrbConfig::kAnaNameUC:
						expName = this->GetName();
						expName(0,1).ToUpper();
						anaStrm << anaTmpl.Encode(line, expName) << endl;
						break;
					case TMrbConfig::kAnaTitle:
						anaStrm << anaTmpl.Encode(line, this->GetTitle()) << endl;
						break;
					case TMrbConfig::kAnaAuthor:
						anaStrm << anaTmpl.Encode(line, fAuthor) << endl;
						break;
					case TMrbConfig::kAnaUser:
						anaStrm << anaTmpl.Encode(line, fUser) << endl;
						break;
					case TMrbConfig::kAnaCreationDate:
						anaStrm << anaTmpl.Encode(line, fCreationDate) << endl;
						break;
					case TMrbConfig::kAnaHtmlMakeSetup:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								evtNameUC = evt->GetName();
								evtNameUC(0,1).ToUpper();
								evtNameUC.Prepend("TUsrEvt");
								TString evtDots = "";
								for (Int_t dots = 0; dots < 40 - evtNameUC.Length(); dots++) evtDots += ".";
								anaTmpl.InitializeCode("%EVT%");
								anaTmpl.Substitute("$evtClassName", evtNameUC);
								anaTmpl.Substitute("$evtTitle", evt->GetTitle());
								anaTmpl.Substitute("$evtTrigger", evt->GetTrigger());
								anaTmpl.Substitute("$evtDots", evtDots);
								anaTmpl.WriteCode(anaStrm);
								TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
								while (sevt = (TMrbSubevent *) sevtIter->Next()) {
									sevtNameUC = sevt->GetName();
									sevtNameUC(0,1).ToUpper();
									sevtNameUC.Prepend("TUsrSevt");
									TString sevtDots = "";
									for (Int_t dots = 0; dots < 40 - sevtNameUC.Length(); dots++) sevtDots += ".";
									anaTmpl.InitializeCode("%SEVT%");
									anaTmpl.Substitute("$sevtClassName", sevtNameUC);
									anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
									anaTmpl.Substitute("$sevtDots", sevtDots);
									anaTmpl.WriteCode(anaStrm);
									TIterator * modIter = sevt->GetLofModules()->MakeIterator();
									while (module = (TMrbModule *) modIter->Next()) {
										TString moduleDots = "";
										TString moduleClass = module->ClassName();
										for (Int_t dots = 0; dots < 40 - moduleClass.Length(); dots++) moduleDots += ".";
										anaTmpl.InitializeCode("%MOD%");
										anaTmpl.Substitute("$moduleClassName", moduleClass);
										anaTmpl.Substitute("$moduleName", module->GetName());
										anaTmpl.Substitute("$moduleTitle", module->GetTitle());
										anaTmpl.Substitute("$moduleDots", moduleDots);
										if (module->IsCamac())	anaTmpl.Substitute("$modulePosition", ((TMrbCamacModule *) module)->GetPosition());
										else					anaTmpl.Substitute("$modulePosition", ((TMrbVMEModule *) module)->GetPosition());
										anaTmpl.WriteCode(anaStrm);
									}
									anaTmpl.InitializeCode("%SEVTE%");
									anaTmpl.WriteCode(anaStrm);
								}
								anaTmpl.InitializeCode("%EVTE%");
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaClassImp:
					case TMrbConfig::kAnaPragmaLinkClasses:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								evtNameUC = evt->GetName();
								evtNameUC(0,1).ToUpper();
								evtNameUC.Prepend("TUsrEvt");
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$className", evtNameUC);
								anaTmpl.WriteCode(anaStrm);
							}
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameUC = sevt->GetName();
								sevtNameUC(0,1).ToUpper();
								sevtNameUC.Prepend("TUsrSevt");
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$className", sevtNameUC);
								anaTmpl.WriteCode(anaStrm);
							}
							TMrbNamedX * ucl;
							TIterator * iclIter = fLofUserClasses.MakeIterator();
							while (ucl = (TMrbNamedX *) iclIter->Next()) {
								if (ucl->GetIndex() & kIclOptUserClass) {
									anaTmpl.InitializeCode();
									anaTmpl.Substitute("$className", ucl->GetName());
									anaTmpl.WriteCode(anaStrm);
								}
							}   
						}
						break;
					case TMrbConfig::kAnaReservedEvents:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (evt->IsReservedEvent()) {
									anaTmpl.InitializeCode();
									evtNameUC = evt->GetName();
									evtNameUC(0,1).ToUpper();
									anaTmpl.Substitute("$className", evtNameUC.Data());
									anaTmpl.Substitute("$evtTitle", evt->GetTitle());
									anaTmpl.Substitute("$pointerName", evt->GetPointerName());
									anaTmpl.WriteCode(anaStrm);
								}
							}
							if (this->FindEvent(TMrbConfig::kTriggerStartAcq) == NULL) {
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$className", "Start");
								anaTmpl.Substitute("$evtTitle", "start acquisition, trigger 14");
								anaTmpl.Substitute("$pointerName", "StartEvent");
								anaTmpl.WriteCode(anaStrm);
							}
							if (this->FindEvent(TMrbConfig::kTriggerStopAcq) == NULL) {
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$className", "Stop");
								anaTmpl.Substitute("$evtTitle", "stop acquisition, trigger 15");
								anaTmpl.Substitute("$pointerName", "StopEvent");
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaIncludeXhitDefs:
						{
							TMrbNamedX * xhit;
							TIterator * xhitIter = fLofXhits.MakeIterator();
							while (xhit = (TMrbNamedX *) xhitIter->Next()) {
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$xhit", xhit->GetName());
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaIncludesAndDefs:
					case TMrbConfig::kAnaModuleSpecialEnum:
						{
							TObjArray onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									module->MakeAnalyzeCode(anaStrm, tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaUserDefinedGlobals:
						{
							TMrbNamedX * nx;
							TIterator * globIter = fLofGlobals.MakeIterator();
							while (nx = (TMrbNamedX *) globIter->Next()) {
								TString gName = nx->GetName();
								if (gName(0) != 'k') {
									anaTmpl.InitializeCode();
									anaTmpl.Substitute("$gName", gName.Data());
									anaTmpl.Substitute("$gComment", nx->GetTitle());
									switch(nx->GetIndex()) {
										case TMrbConfig::kGlobInt:
											anaTmpl.Substitute("$gSignature", "Int_t");
											anaTmpl.Substitute("$gInit",
												Form("%s = %d", gName.Data(), this->GetGlobI(gName.Data())));
											break;
										case TMrbConfig::kGlobFloat:
											anaTmpl.Substitute("$gSignature", "Float_t");
											anaTmpl.Substitute("$gInit",
												Form("%s = %f", gName.Data(), this->GetGlobF(gName.Data())));
											break;
										case TMrbConfig::kGlobDouble:
											anaTmpl.Substitute("$gSignature", "Double_t");
											anaTmpl.Substitute("$gInit",
												Form("%s = %lf", gName.Data(), this->GetGlobD(gName.Data())));
											break;
										case TMrbConfig::kGlobBool:
											anaTmpl.Substitute("$gSignature", "Bool_t");
											anaTmpl.Substitute("$gInit",
												Form("%s = %s", gName.Data(), this->GetGlobB(gName.Data()) ? "kTRUE" : "kFALSE"));
											break;
										case TMrbConfig::kGlobString:
											anaTmpl.Substitute("$gSignature", "TString");
											anaTmpl.Substitute("$gInit",
												Form("%s(\"%s\")", gName.Data(), this->GetGlobStr(gName.Data())));
											break;
									}
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaUserDefinedEnums:
						{
							TMrbNamedX * nx;
							TIterator * globIter = fLofGlobals.MakeIterator();
							while (nx = (TMrbNamedX *) globIter->Next()) {
								TString gName = nx->GetName();
								if (gName(0) == 'k') {
									if (nx->GetIndex() != TMrbConfig::kGlobInt) {
										gMrbLog->Err() << "Not an INTEGER - " << gName.Data() << endl;
										gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
									} else {
										anaTmpl.InitializeCode();
										anaTmpl.Substitute("$eName", gName.Data());
										anaTmpl.Substitute("$eVal", this->GetGlobI(gName.Data()));
										anaTmpl.Substitute("$eComment", nx->GetTitle());
										anaTmpl.WriteCode(anaStrm);
									}
								}
							}
						}
						break;
					case TMrbConfig::kAnaUserDefinedDefines:
						{
							TMrbNamedX * nx;
							TIterator * defIter = fLofDefines.MakeIterator();
							while (nx = (TMrbNamedX *) defIter->Next()) {
								TString dName = nx->GetName();
								TString dMode = nx->GetTitle();
								if (dMode.BeginsWith("Bool_t")) {
									if ((Bool_t) nx->GetIndex()) {
										anaTmpl.InitializeCode("%DB%");
										anaTmpl.Substitute("$dName", dName.Data());
										anaTmpl.WriteCode(anaStrm);
									}
								} else if (dMode.BeginsWith("Int_t")) {
									anaTmpl.InitializeCode("%DV%");
									anaTmpl.Substitute("$dName", dName.Data());
									anaTmpl.Substitute("$dVal", nx->GetIndex());
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaMakeClassNames:
						{
							Bool_t first = kTRUE;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								evtNameUC = evt->GetName();
								evtNameUC(0,1).ToUpper();
								if (!first) anaStrm << " \\" << endl;
								first = kFALSE;
								anaStrm << "\t\t\t\tTUsrEvt" << evtNameUC;
							}
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameUC = sevt->GetName();
								sevtNameUC(0,1).ToUpper();
								if (!first) anaStrm << " \\" << endl;
								first = kFALSE;
								anaStrm << "\t\t\t\tTUsrSevt" << sevtNameUC;
							}
							anaStrm << endl;
						}
						break;
					case TMrbConfig::kAnaVarDefinePointers:
					case TMrbConfig::kAnaVarClassInstance:
						if (gMrbLofUserVars != NULL) this->MakeAnalyzeCode(anaStrm, tagIdx, kIsVariable, anaTmpl);
						break;
					case TMrbConfig::kAnaVarArrDefinePointers:
					case TMrbConfig::kAnaVarArrClassInstance:
						if (gMrbLofUserVars != NULL) this->MakeAnalyzeCode(anaStrm, tagIdx, kIsVariable | kVarIsArray, anaTmpl);
						break;
					case TMrbConfig::kAnaWdwDefinePointers:
					case TMrbConfig::kAnaWdwClassInstance:
					case TMrbConfig::kAnaWdwAdjustPointers:
						if (gMrbLofUserVars != NULL) this->MakeAnalyzeCode(anaStrm, tagIdx, kIsWindow, anaTmpl);
						break;
					case TMrbConfig::kAnaFindVars:
						if (gMrbLofUserVars != NULL) this->MakeAnalyzeCode(anaStrm, tagIdx, kVarOrWindow, anaTmpl);
						break;
					case TMrbConfig::kAnaUserInitializeBeforeHB:
						if (fLofUserHistograms.First() == NULL) break;
					case TMrbConfig::kAnaUserInitializeAfterHB:
						if (this->UserCodeToBeIncluded()) {
							Bool_t udc = kFALSE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & TMrbConfig::kIclOptInitialize) == TMrbConfig::kIclOptInitialize) {
									TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
									TMrbNamedX * nx = (TMrbNamedX *) lofMethods->First();
									while (nx) {
										if ((nx->GetIndex() & TMrbConfig::kIclOptInitialize) == TMrbConfig::kIclOptInitialize) {
											anaTmpl.InitializeCode();
											TString method = nx->GetName();
											Int_t n = method.Index("::", 0);
											if (n >= 0) method = method(n + 2, method.Length() - n - 2);
											anaTmpl.Substitute("$initialize", method.Data());
											anaTmpl.WriteCode(anaStrm);
											udc = kTRUE;
											break;
										}
									}
									break;
								}
								if (udc) break;
							}
						}
						break;
					case TMrbConfig::kAnaUserReloadParams:
					case TMrbConfig::kAnaUserFinishRun:
					case TMrbConfig::kAnaUserMessages:
						if (this->UserCodeToBeIncluded()) {
							UInt_t iclOpt = 0xFFFFFFFF;
							if (tagIdx == kAnaUserReloadParams) 	iclOpt = TMrbConfig::kIclOptReloadParams;
							else if (tagIdx == kAnaUserFinishRun)	iclOpt = TMrbConfig::kIclOptFinishRun;
							else if (tagIdx == kAnaUserMessages)	iclOpt = TMrbConfig::kIclOptHandleMessages;
							Bool_t userCode = kFALSE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & iclOpt) == iclOpt) {
									userCode = kTRUE;
									break;
								}
							}
							if (userCode) break;
						}
						anaTmpl.InitializeCode();
						anaTmpl.WriteCode(anaStrm);
						break;
					case TMrbConfig::kAnaUserDummyMethods:
						if (this->UserCodeToBeIncluded()) {
							Bool_t userInit = kFALSE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & TMrbConfig::kIclOptInitialize) == TMrbConfig::kIclOptInitialize) {
									userInit = kTRUE;
									break;
								}
							}
							if (userInit) break;
						}
						anaTmpl.InitializeCode();
						anaTmpl.WriteCode(anaStrm);
						break;
					case TMrbConfig::kAnaUsingNameSpace:
						break;
					case TMrbConfig::kAnaUserGlobals:
						if (this->UserCodeToBeIncluded()) {
							Bool_t whdr = kTRUE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if (icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) {
									if (whdr) {
										anaTmpl.InitializeCode("%B%");
										anaTmpl.WriteCode(anaStrm);
										whdr = kFALSE;
									}
									anaTmpl.InitializeCode("%I%");
									anaTmpl.Substitute("$iclFile", icl->GetName());
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaMakeUserCxxFlags:
						if (this->UserCodeToBeIncluded()) {
							TList onceOnly;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if (icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) {
									TString iclPath = icl->GetTitle();
									if (iclPath.Length() > 0 && onceOnly.FindObject(iclPath.Data()) == NULL) {
										anaStrm << "CXXFLAGS\t\t+= -I" << iclPath << endl;
										onceOnly.Add(new TNamed(iclPath.Data(), ""));
									}
								}
							}
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaMakeUserHeaders:
						if (this->UserCodeToBeIncluded()) {
							Bool_t first = kTRUE;
							Bool_t found = kFALSE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if (icl->GetIndex()) {
									TMrbSystem ux;
									if ((icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) != 0
									&& (icl->GetIndex() & TMrbConfig::kIclOptUserLib) == 0) {
										TString iclPath = icl->GetTitle();
										if (!first) anaStrm << " \\" << endl;
										first = kFALSE;
										if (iclPath.Length() > 0) {
											anaStrm << "\t\t\t\t" << icl->GetTitle() << "/" << icl->GetName();
										} else {
											anaStrm << "\t\t\t\t" << icl->GetName();
										}
										found = kTRUE;
									}
								}
							}
							if (found) anaStrm << " \\" << endl;
						}
						break;
					case TMrbConfig::kAnaMakeUlibHeaders:
						anaStrm << "ULIB_HDRS\t=";
						if (this->UserCodeToBeIncluded()) {
							Bool_t first = kTRUE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if (icl->GetIndex()) {
									TMrbSystem ux;
									if ((icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) != 0
									&& (icl->GetIndex() & TMrbConfig::kIclOptUserLib) != 0) {
										TString iclPath = icl->GetTitle();
										if (!first) anaStrm << " \\" << endl;
										first = kFALSE;
										if (iclPath.Length() > 0) {
											anaStrm << "\t\t\t\t" << icl->GetTitle() << "/" << icl->GetName();
										} else {
											anaStrm << "\t\t\t\t" << icl->GetName();
										}
									}
								}
							}
							anaStrm << endl;
						}
						break;
					case TMrbConfig::kAnaMakeUserCode:
						anaStrm << "USER_CODE\t=";
						if (this->UserCodeToBeIncluded()) {
							Bool_t first = kTRUE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) == 0) {
									TString iclPath = icl->GetTitle();
									if (first) {
										anaStrm << "\t";
										first = kFALSE;
									} else {
										anaStrm << " \\" << endl;
										anaStrm << "\t\t\t\t";
									}
									if (iclPath.Length() > 0) anaStrm << icl->GetTitle() << "/";
									anaStrm << icl->GetName();
								}
							}
						}
						anaStrm << endl << endl;
						anaStrm << "USER_OBJS\t=";
						if (this->UserCodeToBeIncluded()) {
							Bool_t first = kTRUE;
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) == 0) {
									TString iclName = icl->GetName();
									iclName.ReplaceAll(".cxx", ".o");
									if (first) {
										anaStrm << "\t" << iclName;
										first = kFALSE;
									} else {
										anaStrm << " \\" << endl;
										anaStrm << "\t\t\t\t" << iclName;
									}
								}
							}
						}
						anaStrm << endl;
						break;
					case TMrbConfig::kAnaMakeUserLibs:
						anaStrm << "USER_LIBS\t=";
						if (this->UserLibsToBeIncluded()) {
							Bool_t first = kTRUE;
							TMrbNamedX * ulib;
							TIterator * libIter = fLofUserLibs.MakeIterator();
							while (ulib = (TMrbNamedX *) libIter->Next()) {
								if (first) {
									anaStrm << "\t-L" << ulib->GetTitle() << " -l" << ulib->GetName();
									first = kFALSE;
								} else {
									anaStrm << " \\" << endl;
									anaStrm << "\t\t\t\t-L" << ulib->GetTitle() << " -l" << ulib->GetName();
								}
							}
						}
						anaStrm << endl << endl;
						break;
					case TMrbConfig::kAnaMakeAll:
						if (this->UserLibsToBeIncluded()) {
							TMrbNamedX * ulib;
							Bool_t first = kTRUE;
							Bool_t found = kFALSE;
							TIterator * libIter = fLofUserLibs.MakeIterator();
							while (ulib = (TMrbNamedX *) libIter->Next()) {
								if (!first) anaStrm << " \\" << endl;
								first = kFALSE;
								anaStrm << "\t\t\t\tlib" << ulib->GetName() << ".so";
								found = kTRUE;
							}
						}
						if (found) anaStrm << " \\" << endl;
						break;
					case TMrbConfig::kAnaMakeClean:
						if (this->UserLibsToBeIncluded()) {
							TMrbNamedX * ulib;
							TMrbSystem ux;
							TIterator * libIter = fLofUserLibs.MakeIterator();
							while (ulib = (TMrbNamedX *) libIter->Next()) {
								TObjString * mkf = (TObjString *) ulib->GetAssignedObject();
								TString mkFile = mkf ? Form("-f %s", mkf->GetString().Data()) : "";
								anaStrm << "\t\t\t@cd " << ulib->GetTitle() << "; make " << mkFile << " clean" << endl;
							}
						}
						break;
					case TMrbConfig::kAnaMakeUlibRules:
						if (this->UserLibsToBeIncluded()) {
							TMrbNamedX * ulib;
							TMrbSystem ux;
							TIterator * libIter = fLofUserLibs.MakeIterator();
							while (ulib = (TMrbNamedX *) libIter->Next()) {
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$userLib", Form("lib%s.so", ulib->GetName()));
								anaTmpl.Substitute("$ulibDir", ulib->GetTitle());
								TObjString * mkf = (TObjString *) ulib->GetAssignedObject();
								TString mkFile = mkf ? Form("-f %s", mkf->GetString().Data()) : "";
								anaTmpl.Substitute("$ulibMake", mkFile);
								anaTmpl.WriteCode(anaStrm);
							}
						}
						anaStrm << endl << endl;
						break;
					case TMrbConfig::kAnaMakeUserRules:
						if (this->UserCodeToBeIncluded()) {
							TIterator * iclIter = fLofUserIncludes.MakeIterator();
							while (icl = (TMrbNamedX *) iclIter->Next()) {
								if ((icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) == 0) {
									TString iclPath = icl->GetTitle();
									if (iclPath.Length() > 0) iclPath += "/";
									TString srcName = icl->GetName();
									TString objName = icl->GetName();
									objName.ReplaceAll(".cxx", ".o");
									anaTmpl.InitializeCode();
									anaTmpl.Substitute("$userObj", objName);
									anaTmpl.Substitute("$userDir", (iclPath.Length() > 0) ? iclPath.Data() : "");
									anaTmpl.Substitute("$userCode", srcName);
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaMakeLibNew:
						anaStrm << anaTmpl.Encode(line, fUseMapFile ? "--new" : "") << endl;
						break;						
					case TMrbConfig::kAnaEventClassInstance:
						{
							Bool_t first = kTRUE;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (first) {
									anaTmpl.InitializeCode("%B%");
									anaTmpl.Substitute("$pointerName", evt->GetPointerName());
									anaTmpl.WriteCode(anaStrm);
								}
								first = kFALSE;
								evtNameLC = evt->GetName();
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								if (!evt->IsReservedEvent()) {
									anaTmpl.InitializeCode("%CX%");
									anaTmpl.Substitute("$pointerName", evt->GetPointerName());
									anaTmpl.WriteCode(anaStrm);
								}
								anaTmpl.InitializeCode("%C%");
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtTitle", evt->GetTitle());
								anaTmpl.Substitute("$pointerName", evt->GetPointerName());
								anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								anaTmpl.WriteCode(anaStrm);
								anaTmpl.InitializeCode("%HB%");
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$pointerName", evt->GetPointerName());
								anaTmpl.WriteCode(anaStrm);
								anaTmpl.InitializeCode("%E%");
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$pointerName", evt->GetPointerName());
								anaTmpl.WriteCode(anaStrm);
								if (!evt->IsReservedEvent()) {
									anaTmpl.InitializeCode("%EX%");
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaEventUserClassInstance:
						{
							Bool_t first = kTRUE;
							TMrbNamedX * ucl;
							TIterator * uclIter = fLofUserClasses.MakeIterator();
							while (ucl = (TMrbNamedX *) uclIter->Next()) {
								if (ucl->GetIndex() == kIclOptUserDefinedEvent) {
									if (first) {
										anaTmpl.InitializeCode("%B%");
										anaTmpl.WriteCode(anaStrm);
									}
									first = kFALSE;
									TString classNameUC = ucl->GetName();
									TString classNameLC = classNameUC;
									classNameLC(0,1).ToLower();
									anaTmpl.InitializeCode("%C%");
									anaTmpl.Substitute("$classNameLC", classNameLC);
									anaTmpl.Substitute("$classNameUC", classNameUC);
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaEventDefinePointers:
					case TMrbConfig::kAnaEventCreateTree:
					case TMrbConfig::kAnaEventInitializeTree:
					case TMrbConfig::kAnaEventSetWriteTree:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (!evt->IsReservedEvent()) {
									evtNameLC = evt->GetName();
									evtNameUC = evtNameLC;
									evtNameUC(0,1).ToUpper();
									anaTmpl.InitializeCode("%E%");
									anaTmpl.Substitute("$evtNameUC", evtNameUC);
									anaTmpl.Substitute("$evtNameLC", evtNameLC);
									anaTmpl.Substitute("$evtTitle", evt->GetTitle());
									anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
									anaTmpl.Substitute("$pointerName", evt->GetPointerName());
									anaTmpl.WriteCode(anaStrm);
								}
							}
							TMrbNamedX * ucl;
							TIterator * uclIter = fLofUserClasses.MakeIterator();
							while (ucl = (TMrbNamedX *) uclIter->Next()) {
								if (ucl->GetIndex() == kIclOptUserDefinedEvent) {
									TString classNameUC = ucl->GetName();
									TString classNameLC = classNameUC;
									classNameLC(0,1).ToLower();
									anaTmpl.InitializeCode("%U%");
									anaTmpl.Substitute("$classNameLC", classNameLC);
									anaTmpl.Substitute("$classNameUC", classNameUC);
									anaTmpl.WriteCode(anaStrm);
								}
							}
						} 
						break;
					case TMrbConfig::kAnaEventSetFakeMode:
					case TMrbConfig::kAnaEventSetReplayMode:
					case TMrbConfig::kAnaEventReplayTree:
					case TMrbConfig::kAnaEventSetScaleDown:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								switch (evt->GetTrigger()) {
									case TMrbConfig::kTriggerStartAcq:	evtNameLC = "startEvent"; break;
									case TMrbConfig::kTriggerStopAcq:	evtNameLC = "stopEvent"; break;
									default:							evtNameLC = evt->GetName(); break;
								}
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode("%E%");
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtTitle", evt->GetTitle());
								anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								anaTmpl.Substitute("$pointerName", evt->GetPointerName());
								anaTmpl.WriteCode(anaStrm);
							}
							TMrbNamedX * ucl;
							TIterator * uclIter = fLofUserClasses.MakeIterator();
							while (ucl = (TMrbNamedX *) uclIter->Next()) {
								if (ucl->GetIndex() == kIclOptUserDefinedEvent) {
									TString classNameUC = ucl->GetName();
									TString classNameLC = classNameUC;
									classNameLC(0,1).ToLower();
									anaTmpl.InitializeCode("%U%");
									anaTmpl.Substitute("$classNameLC", classNameLC);
									anaTmpl.Substitute("$classNameUC", classNameUC);
									anaTmpl.WriteCode(anaStrm);
								}
							}
						} 
						break;
					case TMrbConfig::kAnaEventIdEnum:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								evtNameLC = evt->GetName();
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtTitle", evt->GetTitle());
								anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaEventDispatchOverTrigger:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (!evt->IsReservedEvent()) {
									evtNameLC = evt->GetName();
									evtNameUC = evtNameLC;
									evtNameUC(0,1).ToUpper();
									anaTmpl.InitializeCode("%B%");
									anaTmpl.Substitute("$evtNameUC", evtNameUC);
									anaTmpl.Substitute("$evtNameLC", evtNameLC);
									anaTmpl.Substitute("$evtTitle", evt->GetTitle());
									anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
									anaTmpl.WriteCode(anaStrm);
									anaTmpl.InitializeCode("%P%");
									Bool_t udc = kFALSE;
									TString method = "Analyze";
									if (gMrbConfig->UserCodeToBeIncluded()) {
										TIterator * iclIter = fLofUserIncludes.MakeIterator();
										while (icl = (TMrbNamedX *) iclIter->Next()) {
											if ((icl->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
												TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
												TMrbNamedX * nx;
												TIterator * methIter = lofMethods->MakeIterator();
												while (nx = (TMrbNamedX *) methIter->Next()) {
													if ((nx->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
														TString m = nx->GetName();
														Int_t n = m.Index("::", 0);
														if (n > 0) {
															TString cl = m(0, n);
															TString evtClass = evtNameUC;
															evtClass.Prepend("TUsrEvt");
															if (evtClass.CompareTo(cl.Data()) == 0) {
																method = m(n + 2, m.Length() - n - 2);
																udc = kTRUE;
																break;
															}
														}
													}
												}
											}
											if (udc) break;
										}
									}
									anaTmpl.Substitute("$evtNameUC", evtNameUC);
									anaTmpl.Substitute("$evtNameLC", evtNameLC);
									anaTmpl.Substitute("$evtTitle", evt->GetTitle());
									anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
									anaTmpl.Substitute("$processEvent", method.Data());
									anaTmpl.WriteCode(anaStrm);
									anaTmpl.InitializeCode("%E%");
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaEventTriggerStartAcq:
					case TMrbConfig::kAnaEventTriggerStopAcq:
						{
							anaTmpl.InitializeCode("%B%");
							anaTmpl.WriteCode(anaStrm);
							Int_t trig = (tagIdx == TMrbConfig::kAnaEventTriggerStartAcq) ? TMrbConfig::kTriggerStartAcq : TMrbConfig::kTriggerStopAcq;
							evt = (TMrbEvent *) this->FindEvent(trig);
							if (evt) {
								anaTmpl.InitializeCode("%SF%");
								anaTmpl.WriteCode(anaStrm);
								Bool_t udc = kFALSE;
								TString method = "Analyze";
								if (gMrbConfig->UserCodeToBeIncluded()) {
									TIterator * iclIter = fLofUserIncludes.MakeIterator();
									while (icl = (TMrbNamedX *) iclIter->Next()) {
										if ((icl->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
											TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
											TMrbNamedX * nx;
											TIterator * methIter = lofMethods->MakeIterator();
											while (nx = (TMrbNamedX *) methIter->Next()) {
												if ((nx->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
													TString m = nx->GetName();
													Int_t n = m.Index("::", 0);
													if (n > 0) {
														TString cl = m(0, n);
														TString evtClass = evt->IsStartEvent() ? "TUsrEvtXstart" : "TUsrEvtXstop";
														if (evtClass.CompareTo(cl.Data()) == 0) {
															method = m(n + 2, m.Length() - n - 2);
															udc = kTRUE;
															break;
														}
													}
												}
											}
										}
										if (udc) break;
									}
								}
								anaTmpl.InitializeCode("%SP%");
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtTitle", evt->GetTitle());
								anaTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								anaTmpl.Substitute("$processEvent", method.Data());
								anaTmpl.WriteCode(anaStrm);
							} else {
								anaTmpl.InitializeCode("%N%");
								anaTmpl.WriteCode(anaStrm);
							}
							anaTmpl.InitializeCode("%E%");
							anaTmpl.WriteCode(anaStrm);
						}
						break;
					case TMrbConfig::kAnaEventIgnoreTrigger:
						ignTrigger = kFALSE;
						for (trg = 0; trg < kNofTriggers; trg++) {
							if (fTriggersToBeHandled[trg] == kTriggerIgnored) {
								ignTrigger = kTRUE;
								anaTmpl.InitializeCode("%C%");
								anaTmpl.Substitute("$trigNo", (Int_t) trg);
								anaTmpl.WriteCode(anaStrm);
							}
						}
						if (ignTrigger) {
							anaTmpl.InitializeCode("%E%");
							anaTmpl.WriteCode(anaStrm);
						}
						break;
					case TMrbConfig::kAnaInitializeLists:
						{
							anaTmpl.InitializeCode("%B%");
							nofParams = 0;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) nofParams += module->GetNofChannelsUsed();
							anaTmpl.Substitute("$nofModules", fNofModules);
							anaTmpl.Substitute("$nofParams", nofParams);
							anaTmpl.WriteCode(anaStrm);
							module = (TMrbModule *) fLofModules.First();
							nofParams = 0;
							modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetNofChannelsUsed() > 0) {
									anaTmpl.InitializeCode("%ML%");
									moduleNameLC = module->GetName();
									moduleNameUC = moduleNameLC;
									moduleNameUC(0,1).ToUpper();
									anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
									anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
									anaTmpl.Substitute("$moduleTitle", module->GetTitle());
									anaTmpl.Substitute("$paramIndex", nofParams);
									anaTmpl.Substitute("$nofParams", module->GetNofChannelsUsed());
									anaTmpl.WriteCode(anaStrm);
								}
								nofParams += module->GetNofChannelsUsed();
							}
						}
						break;
					case TMrbConfig::kAnaModuleTimeOffset:
						{
							found = kFALSE;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (module->GetTimeOffset() != 0) {
									if (!found) {
										anaTmpl.InitializeCode("%B%");
										anaTmpl.WriteCode(anaStrm);
										found = kTRUE;
									}
									anaTmpl.InitializeCode("%T%");
									moduleNameLC = module->GetName();
									moduleNameUC = moduleNameLC;
									moduleNameUC(0,1).ToUpper();
									anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
									anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
									anaTmpl.Substitute("$moduleTitle", module->GetTitle());
									anaTmpl.Substitute("$timeOffset", module->GetTimeOffset());
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaAddUserEnv:
						expName = this->GetName();
						expName(0,1).ToUpper();
						rcFile = ".";
						rcFile += expName;
						rcFile += "Config.rc";
						anaTmpl.InitializeCode();
						anaTmpl.Substitute("$rcFile", rcFile.Data());
						anaTmpl.WriteCode(anaStrm);
						break;
					case TMrbConfig::kAnaHistoDefinePointers:
						if (gMrbConfig->GetAnalyzeOptions() & kAnaOptHistograms) {
							sevt = (TMrbSubevent *) fLofSubevents.First();
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) sevt->MakeAnalyzeCode(anaStrm, tagIdx, NULL, anaTmpl);
						}
						if (fLofUserHistograms.First()) {
							anaTmpl.InitializeCode("%UDH%");
							anaTmpl.WriteCode(anaStrm);
							TMrbNamedX * h;
							TIterator * hIter = fLofUserHistograms.MakeIterator();
							while (h = (TMrbNamedX *) hIter->Next()) {
								anaTmpl.InitializeCode("%UDBH%");
								anaTmpl.Substitute("$hName", h->GetName());
								anaTmpl.Substitute("$hTitle", h->GetTitle());
								Int_t hType = h->GetIndex();
								if (hType == TMrbConfig::kHistoRate) hType = TMrbConfig::kHistoTH1F;
								anaTmpl.Substitute("$hType", (fLofHistoTypes.FindByIndex(hType))->GetName());
								anaTmpl.WriteCode(anaStrm);
							}
							if (fLofHistoArrays.First()) {
								TMrbNamedX * h;
								TIterator * hIter = fLofHistoArrays.MakeIterator();
								while (h = (TMrbNamedX *) hIter->Next()) {
									anaTmpl.InitializeCode("%UDHA%");
									anaTmpl.Substitute("$hArrayName", h->GetName());
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaHistoInitializeArrays:
						if (gMrbConfig->GetAnalyzeOptions() & kAnaOptHistograms) {
							sevt = (TMrbSubevent *) fLofSubevents.First();
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) sevt->MakeAnalyzeCode(anaStrm, tagIdx, NULL, anaTmpl);
							if (fLofHistoArrays.First()) {
								TMrbNamedX * hArray;
								TIterator * hIter = fLofHistoArrays.MakeIterator();
								while (hArray = (TMrbNamedX *) hIter->Next()) {
									TObjArray * lofHistos = (TObjArray *) hArray->GetAssignedObject();
									Int_t nofHistos = lofHistos->GetEntriesFast();
									anaTmpl.InitializeCode("%U%");
									anaTmpl.Substitute("$hArrayName", hArray->GetName());
									anaTmpl.Substitute("$nofHistos", nofHistos);
									anaTmpl.WriteCode(anaStrm);
								}
							}
						}
						break;
					case TMrbConfig::kAnaHistoBookUserDefined:
						if (fLofUserHistograms.First()) {
							anaTmpl.InitializeCode("%B%");
							anaTmpl.WriteCode(anaStrm);
							TMrbNamedX * h;
							TIterator * hIter = fLofUserHistograms.MakeIterator();
							while (h = (TMrbNamedX *) hIter->Next()) {
								Bool_t hasArgStr = (h->GetAssignedObject())->InheritsFrom("TObjString");
								TMrbNamedX * cnd = (TMrbNamedX *) fLofHistoConditions.FindObject(h->GetName());
								if (hasArgStr) {
									if (cnd == NULL) {
										anaTmpl.InitializeCode("%UDS%");
									} else {
										anaTmpl.InitializeCode("%UDSC%");
										anaTmpl.Substitute("$condition", cnd->GetTitle());
									}
								} else {
									if (cnd == NULL) {
										anaTmpl.InitializeCode((h->GetIndex() & TMrbConfig::kHistoTH1) ? "%UD1%" : "%UD2%");
									} else {
										anaTmpl.InitializeCode((h->GetIndex() & TMrbConfig::kHistoTH1) ? "%UD1C%" : "%UD2C%");
										anaTmpl.Substitute("$condition", cnd->GetTitle());
									}
								}
								anaTmpl.Substitute("$hName", h->GetName());
								anaTmpl.Substitute("$hTitle", h->GetTitle());
								if (hasArgStr) {
									TString argStr = ((TObjString *) h->GetAssignedObject())->GetString();
									anaTmpl.Substitute("$hType", (fLofHistoTypes.FindByIndex(h->GetIndex()))->GetName());
									anaTmpl.Substitute("$args", argStr);
								} else {
									TMrbNamedArrayD * a = (TMrbNamedArrayD *) h->GetAssignedObject();
									anaTmpl.Substitute("$hType", a->GetName());
									Double_t * ap = a->GetArray();
									anaTmpl.Substitute("$binSizeX", (Int_t) *ap++);
									anaTmpl.Substitute("$lowerX", *ap++);
									anaTmpl.Substitute("$upperX", *ap++);
									if (h->GetIndex() & TMrbConfig::kHistoTH2) {
										anaTmpl.Substitute("$binSizeY", (Int_t) *ap++);
										anaTmpl.Substitute("$lowerY", *ap++);
										anaTmpl.Substitute("$upperY", *ap++);
									}
								}
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;

					case TMrbConfig::kAnaHistoFillArrays:
						if (fLofSubevents.First()) {
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								if (sevt->HistosToBeAllocated() && sevt->IsInArrayMode()) {
									TString listName = sevt->GetName();
									listName += ".histlist";
									ofstream list(listName.Data(), ios::out);
									if (!list.good()) {
										gMrbLog->Err() << gSystem->GetError() << " - " << listName << endl;
										gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
									} else {
										TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
										while (param =  (TMrbModuleChannel *) paramIter->Next()) {
											TString paramName = param->GetName();
											paramName(0,1).ToUpper();
											paramName.Prepend("h");
											list << paramName << endl;
										}
										list.close();
										if (verboseMode) {
											gMrbLog->Out()  << "Creating histo list file - " << listName
															<< " (" << sevt->GetNofParams() << " entries)"
															<< endl;
											gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
										}
									}
								}
							}
						}
						this->CreateHistoArrays();
						if (fLofHistoArrays.First()) {
							anaTmpl.InitializeCode("%B%");
							anaTmpl.WriteCode(anaStrm);
							TMrbNamedX * hArray;
							TIterator * hIter = fLofHistoArrays.MakeIterator();
							while (hArray = (TMrbNamedX *) hIter->Next()) {
								ofstream list(hArray->GetTitle(), ios::out);
								if (!list.good()) {
									gMrbLog->Err() << gSystem->GetError() << " - " << hArray->GetTitle() << endl;
									gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
								} else {
									TObjArray * lofHistos = (TObjArray *) hArray->GetAssignedObject();
									TMrbNamedX * h;
									TIterator * hIter = lofHistos->MakeIterator();
									while (h = (TMrbNamedX *) hIter->Next()) {
										if (this->HistogramExists(h->GetName())) {
											anaTmpl.InitializeCode("%FHA%");
											anaTmpl.Substitute("$hArrayName", hArray->GetName());
											anaTmpl.Substitute("$hName", h->GetName());
											anaTmpl.Substitute("$hTitle", h->GetTitle());
											anaTmpl.WriteCode(anaStrm);
											list << h->GetName() << endl;
										} else {
											gMrbLog->Err() << "No such histogram - " << h->GetName() << endl;
											gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
										}
									}
									if (verboseMode) {
										gMrbLog->Out()  << "Creating histo list file - " << hArray->GetTitle()
														<< " (" << lofHistos->GetEntriesFast() << " entries)"
														<< endl;
										gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
									}
									list.close();
								}
							}
						}
						break;
					case TMrbConfig::kAnaSevtClassDef:
					case TMrbConfig::kAnaSevtClassMethods:
						{
							TList onceOnly;
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								if (onceOnly.FindObject(sevt->GetCommonCodeFile()) == NULL) {
									this->MakeAnalyzeCode(anaStrm, sevt->ClassName(), sevt->GetCommonCodeFile(), tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(sevt->GetCommonCodeFile(), ""));
								sevt->MakeAnalyzeCode(anaStrm, tagIdx, pp->GetX());
							}
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaEventClassDef:
					case TMrbConfig::kAnaEventClassMethods:
						{
							TList onceOnly;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (onceOnly.FindObject(evt->GetCommonCodeFile()) == NULL) {
									this->MakeAnalyzeCode(anaStrm, evt->ClassName(), evt->GetCommonCodeFile(), tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(evt->GetCommonCodeFile(), ""));
								evt->MakeAnalyzeCode(anaStrm, tagIdx, pp->GetX());
							}
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaEventUserClassMethods:
						{
							TMrbNamedX * ucl;
							TIterator * uclIter = fLofUserClasses.MakeIterator();
							while (ucl = (TMrbNamedX *) uclIter->Next()) {
								if (ucl->GetIndex() == kIclOptUserDefinedEvent) {
									this->CreateUserEvent(anaStrm, ucl->GetName(), kFALSE, kTRUE);
								}
							}
						}
						break;
					case TMrbConfig::kAnaEventSetBranchStatus:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) evt->MakeAnalyzeCode(anaStrm, tagIdx, anaTmpl);
						}
						break;
					case TMrbConfig::kAnaSevtSerialEnum:
						{
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameLC = sevt->GetName();
								sevtNameUC = sevtNameLC;
								sevtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode(sevt->SerialToBeCreated() ? "%S%" : "%NS%");
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								anaTmpl.Substitute("$sevtSerial", sevt->GetSerial());
								anaTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
								anaTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
								anaTmpl.Substitute("$sevtHexSerial", sevt->GetSerial(), 16);
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaSevtBitsEnum:
						{
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameLC = sevt->GetName();
								sevtNameUC = sevtNameLC;
								sevtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode(sevt->SerialToBeCreated() ? "%S%" : "%NS%");
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								anaTmpl.Substitute("$sevtBit", (Int_t) (0x1 << (sevt->GetSerial() - 1)), 16);
								anaTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
								anaTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
								anaTmpl.Substitute("$sevtHexBit", sevt->GetSerial(), 16);
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaSevtIndicesEnum:
						{
							header = kFALSE;
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameLC = sevt->GetName();
								sevtNameUC = sevtNameLC;
								sevtNameUC(0,1).ToUpper();
								if (sevt->IsInArrayMode()) {
									Int_t parNo = 0;
									first = kTRUE;
									if (!header) {
										anaTmpl.InitializeCode("%B%");
										anaTmpl.WriteCode(anaStrm);
										header = kTRUE;
									}
									TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
									while (param = (TMrbModuleChannel *) paramIter->Next()) {
										paramNameLC = param->GetName();
										paramNameUC = paramNameLC;
										paramNameUC(0,1).ToUpper();
										anaTmpl.InitializeCode(first ? "%H%" : "%X%");
										first = kFALSE;
										anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
										anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
										anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
										anaTmpl.Substitute("$sevtSerial", sevt->GetSerial());
										anaTmpl.Substitute("$paramNameLC", paramNameLC.Data());
										anaTmpl.Substitute("$paramNameUC", paramNameUC.Data());
										anaTmpl.Substitute("$parNo", parNo);
										anaTmpl.WriteCode(anaStrm);
										parNo++;
									}
								}		
							}
							if (header) {
								anaTmpl.InitializeCode("%E%");
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaModuleIdEnum:
						{
							TList onceOnly;
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (onceOnly.FindObject(module->ClassName()) == NULL) {
									anaTmpl.InitializeCode();
									TString moduleType = module->GetMnemonic();
									moduleType(0,1).ToUpper();
									anaTmpl.Substitute("$moduleType", moduleType);
									anaTmpl.Substitute("$moduleID", module->GetModuleID()->GetIndex(), 16);
									anaTmpl.Substitute("$moduleTitle", module->GetTitle());
									anaTmpl.WriteCode(anaStrm);
								}
								onceOnly.Add(new TNamed(module->ClassName(), ""));
							}   
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaModuleSerialEnum:
						{
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								moduleNameLC = module->GetName();
								moduleNameUC = moduleNameLC;
								moduleNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
								anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
								anaTmpl.Substitute("$moduleTitle", module->GetTitle());
								anaTmpl.Substitute("$moduleSerial", module->GetSerial());
								anaTmpl.WriteCode(anaStrm);
							}
						}
						break;
					case TMrbConfig::kAnaIncludeEvtSevtModGlobals:
					case TMrbConfig::kAnaInitializeEvtSevtMods:
						{
							TList onceOnly;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								if (onceOnly.FindObject(evt->GetCommonCodeFile()) == NULL) {
									this->MakeAnalyzeCode(anaStrm, evt->ClassName(), evt->GetCommonCodeFile(), tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(evt->GetCommonCodeFile(), ""));
							}
							onceOnly.Delete();
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								if (onceOnly.FindObject(sevt->GetCommonCodeFile()) == NULL) {
									this->MakeAnalyzeCode(anaStrm, sevt->ClassName(), sevt->GetCommonCodeFile(), tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(sevt->GetCommonCodeFile(), ""));
							}
							onceOnly.Delete();
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								if (onceOnly.FindObject(module->GetCommonCodeFile()) == NULL) {
									this->MakeAnalyzeCode(anaStrm, module->ClassName(), module->GetCommonCodeFile(), tagIdx, pp->GetX());
								}
								onceOnly.Add(new TNamed(module->GetCommonCodeFile(), ""));
							}
							onceOnly.Delete();
						}
						break;
					case TMrbConfig::kAnaLoadUserLibs:
						{
							TString libName;
							if (this->UserLibsToBeIncluded()) {
								TMrbNamedX * icl;
								TIterator * iclIter = fLofUserLibs.MakeIterator();
								while (icl = (TMrbNamedX *) iclIter->Next()) {
									anaTmpl.InitializeCode();
									TString iclPath = icl->GetTitle();
									if (iclPath.Length() > 0) {
										iclPath += "/";
										anaTmpl.Substitute("$libPath", iclPath);
									} else {
										anaTmpl.Substitute("$libPath", "");
									}
									libName = "lib";
									libName += icl->GetName();
									anaTmpl.Substitute("$userLib", libName);
									anaTmpl.WriteCode(anaStrm);
								}
							}
							libName = this->GetName();
							libName(0,1).ToUpper();
							libName.Prepend("lib");
							libName += "Analyze";
							anaTmpl.InitializeCode();
							anaTmpl.Substitute("$libPath", "");
							anaTmpl.Substitute("$userLib", libName);
							anaTmpl.WriteCode(anaStrm);
						}
						break;
				}
			}
		}
		anaStrm.close();
		gMrbLog->Out() << "[" << cf << ": " << pp->GetC() << "]" << endl;
		gMrbLog->Flush("", "", setblue);
	}

// let's now check if there is a tree output anywhere to prevent user from getting sad when replaying data

	FILE * grep = gSystem->OpenPipe("grep 'fTreeOut->Fill' *.cxx */*.cxx 2>/dev/null", "r");
	char tmp[200];
	if (fread(tmp, 1, 200, grep) == 0) {
		gMrbLog->Err() << "There is NO TREE OUTPUT in your code" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		gMrbLog->Err()	<< "You should include a statement" << endl
						<< "                               >>> "
						<< setblack
						<< "if (this->TreeToBeWritten()) fTreeOut->Fill();" << setred << " <<<" << endl
						<< "                               somewhere in your Analyze() code" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}

	return(kTRUE);
}

Bool_t TMrbConfig::MakeAnalyzeCode(ofstream & AnaStrm,	TMrbConfig::EMrbAnalyzeTag TagIndex,
													UInt_t VarType,
													TMrbTemplate & Template,
													const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeAnalyzeCode
// Purpose:        Generate user-defined code for variables/windows
// Arguments:      ofstream & AnaStrm            -- where to output code
//                 EMrbAnalyzeTag TagIndex       -- tag word index
//                 UInt_t VarType                -- variable type
//                 TMrbTemplate * Template       -- template
//                 Char_t * Prefix               -- prefix to select template code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates code for all variables/windows in specified list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * obj;
	TMrbVariable * varp;
	TMrbWindow * wdwp;
	UInt_t id, type;

	TString varName;
	TString className;

	if (gMrbLofUserVars == NULL) return(kFALSE);

	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	obj = gMrbLofUserVars->First();
	while (obj) {
		id = obj->GetUniqueID();
		if (id & VarType) {
			type = id & kIsWindow;
			if ( (type = (id & kIsWindow)) ) {
				Template.InitializeCode((type == kWindow2D) ? "%2D%" : "%1D%");
			} else {
				Template.InitializeCode();
			}
			Template.Substitute("$className", obj->ClassName());
			if ( (type = (id & kIsVariable)) ) {
				varp = (TMrbVariable *) obj;
				varName = varp->GetName();
				varName(0,1).ToLower();
				Template.Substitute("$orgVarName", varp->GetName());
				Template.Substitute("$varName", varName);
				Template.Substitute("$varSize", (Int_t) varp->GetSize());
				if (id & kVarIsArray) {
					switch (type) {
						case kVarI:
							Template.Substitute("$varInit", ((TMrbVarArrayI *) varp)->Get(0)); break;
						case kVarF:
							Template.Substitute("$varInit", ((TMrbVarArrayF *) varp)->Get(0)); break;
					}
				} else {
					switch (type) {
						case kVarI:
							Template.Substitute("$varInit", ((TMrbVarI *) varp)->Get()); break;
						case kVarF:
							Template.Substitute("$varInit", ((TMrbVarF *) varp)->Get()); break;
						case kVarS:
							{
								TString vInit = "\"";
								vInit += ((TMrbVarS *) varp)->Get();
								vInit += "\"";
								Template.Substitute("$varInit", vInit.Data());
								break;
							}
					}
				}
			} else if ( (type = (id & kIsWindow)) ) {
				wdwp = (TMrbWindow *) obj;
				varName = wdwp->GetName();
				varName(0,1).ToLower();
				Template.Substitute("$orgVarName", wdwp->GetName());
				Template.Substitute("$varName", varName);
				switch (type) {
					case kWindowI:
						Template.Substitute("$varLower", ((TMrbWindowI *) wdwp)->GetLowerLimit());
						Template.Substitute("$varUpper", ((TMrbWindowI *) wdwp)->GetUpperLimit());
						break;
					case kWindowF:
						Template.Substitute("$varLower", ((TMrbWindowF *) wdwp)->GetLowerLimit());
						Template.Substitute("$varUpper", ((TMrbWindowF *) wdwp)->GetUpperLimit());
						break;
					case kWindow2D:
						break;
				}
			}

			switch (TagIndex) {
				case TMrbConfig::kAnaVarArrClassInstance:
				case TMrbConfig::kAnaVarArrDefinePointers:
					if (id & kVarIsArray) Template.WriteCode(AnaStrm);
					break;
				case TMrbConfig::kAnaVarDefinePointers:
				case TMrbConfig::kAnaVarClassInstance:
					if (!(id & kVarIsArray)) Template.WriteCode(AnaStrm);
					break;
				case TMrbConfig::kAnaWdwDefinePointers:
				case TMrbConfig::kAnaWdwClassInstance:
					if (id & kIsWindow) Template.WriteCode(AnaStrm);
					break;
				case TMrbConfig::kAnaWdwAdjustPointers:
					if (id == kWindow2D) Template.WriteCode(AnaStrm);
					break;
				case TMrbConfig::kAnaFindVars:
					Template.WriteCode(AnaStrm);
					break;
			}
		}
		obj = gMrbLofUserVars->After(obj);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::MakeAnalyzeCode(ofstream & AnaStrm, const Char_t * ClassName, const Char_t * CodeFile,
										TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::MakeAnalyzeCode
// Purpose:        Generate class-specific code for user's analysis
// Arguments:      ofstream & AnaStrm          -- where to output code
//                 Char_t * ClassName          -- class name
//                 Char_t * CodeFile           -- common code file
//                 EMrbAnalyzeTag TagIndex     -- tag word index
//                 Char_t * Extension          -- file extension
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates class-specific code.
//                 Searches for following files:
//                 (1)   <CodeFile>.<Extension>.code
//                 (2)   <Class>_Common.<Extension>.code
//
//                 This method is optional and doesn't generate an error
//                 if no template file has been found.
//                 It is also used to generate special hit objects.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;

	TMrbNamedX * analyzeTag;

	TString anaTemplateFile;
	TString templatePath;

	TString tf;
	
	TMrbTemplate anaTmpl;
	
	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	TString fileSpec = "";
	TMrbSystem ux;
	if (CodeFile != NULL) {
		tf = CodeFile;
		tf += Extension;
		tf += ".code";
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
	}
	if (fileSpec.IsNull()) {
		tf = ClassName;
		tf.ReplaceAll("TMrb", "");
		tf += "_Common";
		tf += Extension;
		tf += ".code";
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
	}
	if (fileSpec.IsNull()) return(kTRUE);

	if (!this->TagToBeProcessed(tf.Data(), TagIndex)) { 	// will be processed only once
		if (verboseMode) {
			gMrbLog->Out()  << "[" << ClassName << "] Template file " << fileSpec
							<< ": Tag " << fLofAnalyzeTags.FindByIndex(TagIndex)->GetName()
							<< "(" << TagIndex << ") already processed" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		}
		return(kTRUE);
	}

	this->AddToTagList(tf.Data(), TagIndex);
	
 	anaTemplateFile = fileSpec;

	if (!anaTmpl.Open(anaTemplateFile, &gMrbConfig->fLofAnalyzeTags)) {
		if (verboseMode) {
			gMrbLog->Err()  << "[" << ClassName << "] Skipping template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		}
		return(kFALSE);
	} else if (verboseMode) {
		gMrbLog->Out()  << "[" << ClassName << "] Using template file " << fileSpec << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}


	for (;;) {
		analyzeTag = anaTmpl.Next(line);
		if (anaTmpl.IsEof()) break;
		if (anaTmpl.IsError()) continue;
		if (anaTmpl.Status() & TMrbTemplate::kNoTag) continue;
		if (TagIndex == analyzeTag->GetIndex()) {
			if (!this->ExecUserMacro(&AnaStrm, this, analyzeTag->GetName())) {
				switch (TagIndex) {
					default:
						anaTmpl.InitializeCode();
						anaTmpl.WriteCode(AnaStrm);
						break;
				}
				return(kTRUE);
			}
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::MakeConfigCode(const Char_t * CodeFile, Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeConfigCode
// Purpose:        Rebuild config file from objects in memory
// Arguments:      Char_t * CodeFile         -- name of code file to be generated
//                 Option_t * Options          -- how to generate code: default ""
//                                              Overwrite: overwrite existing config file
//                                              Verbose: be verbose
// Results:
// Exceptions:
// Description:    Opens a file for user-defined config code. Rebuilds config file from
//                 config objects in memory. Useful when reading config data from root file.
//
//                 Template file should contain tags like %%TAG_NAME%%
//                 They will be expanded to real code by MakeConfigCode and its descendants.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;
	TString cfile;
	TString cf, tf, tf1, tf2;

	TString expName;

	TMrbEvent * evt;
	TString evtNameUC;
	TString evtNameLC;

	TMrbSubevent * sevt = NULL;
	TString sevtNameUC;
	TString sevtNameLC;

	TMrbModule * module;
	TString moduleNameUC;
	TString moduleNameLC;

	TMrbCamacScaler * scaler;
	TString scalerNameUC;
	TString scalerNameLC;

	TMrbVariable * var, * lvar;
	TString lofVars;
	TString vType;

	TMrbWindow * wdw, * lwdw;
	TString lofWdws;
	TString wType;

	TMrbNamedX * configTag;
	TMrbConfig::EMrbConfigTag tagIdx;

	TString templatePath;
	TString cfgTemplateFile;
	TString prefix;
	
	ofstream cfgStrm;

	TMrbTemplate cfgTmpl;

	TObjArray filesToCreate;

	TMrbLofNamedX configOptions;

	fConfigOptions = configOptions.CheckPatternShort(this->ClassName(), "MakeConfigCode", Options, kMrbConfigOptions);
	if (fConfigOptions == TMrbLofNamedX::kIllIndexBit) return(kFALSE);

	if (fNofEvents == 0) {
		gMrbLog->Err() << "No events/triggers defined" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
		return(kFALSE);
	}

	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);

	prefix = this->GetName();
	prefix(0,1).ToUpper();
	
	if (*CodeFile == '\0') {
		cfile = prefix;
		cfile += "Config";
	} else {
		cfile = CodeFile;
	}

	packNames acfg(cfile.Data(), "AnalyzeConfig.C.code", ".C", "ROOTCint code to generate exp config");
	filesToCreate.Add((TObject *) &acfg);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	packNames * pp;
	TIterator * ppIter = filesToCreate.MakeIterator();
	while (pp = (packNames *) ppIter->Next()) {
		cf = pp->GetF() + pp->GetX();
		if (((this->GetConfigOptions() & kCfgOptOverwrite) == 0) && (gSystem->AccessPathName(cf) == 0)) {
			gMrbLog->Err() << "File exists - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
			continue;
		}
		cfgStrm.open(cf, ios::out);
		if (!cfgStrm.good()) {	
			gMrbLog->Err() << gSystem->GetError() << " - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
			continue;
		}

		tf = pp->GetT();
		tf.Prepend(prefix.Data());
		tf1 = tf;
		TString fileSpec;
		TMrbSystem ux;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			tf = pp->GetT();
			tf2 = tf;
			ux.Which(fileSpec, templatePath.Data(), tf.Data());
		}
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "Template file not found -" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
			gMrbLog->Err()	<< "            Searching on path " << templatePath << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            for file " << tf1 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            or       " << tf2 << endl;
			gMrbLog->Flush();
			cfgStrm.close();
			continue;
		}

		cfgTemplateFile = fileSpec;

		if (!cfgTmpl.Open(cfgTemplateFile, &fLofConfigTags)) {
			if (verboseMode) {
				gMrbLog->Err()  << "Skipping template file " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
			}
			continue;
		} else if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeConfigCode");
		}

		for (;;) {
			configTag = cfgTmpl.Next(line);
			if (cfgTmpl.IsEof()) break;
			if (cfgTmpl.IsError()) continue;
			if (cfgTmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) cfgStrm << line << endl;
			} else if (this->ExecUserMacro(&cfgStrm, this, configTag->GetName())) {
				continue;
			} else {
				switch (tagIdx = (TMrbConfig::EMrbConfigTag) configTag->GetIndex()) {
					case TMrbConfig::kCfgFile:
						cfgStrm << cfgTmpl.Encode(line, cfile) << endl;
						break;
					case TMrbConfig::kCfgNameLC:
						cfgStrm << cfgTmpl.Encode(line, GetName()) << endl;
						break;
					case TMrbConfig::kCfgNameUC:
						expName = this->GetName();
						expName(0,1).ToUpper();
						cfgStrm << cfgTmpl.Encode(line, expName) << endl;
						break;
					case TMrbConfig::kCfgTitle:
						cfgStrm << cfgTmpl.Encode(line, GetTitle()) << endl;
						break;
					case TMrbConfig::kCfgAuthor:
						cfgStrm << cfgTmpl.Encode(line, fAuthor) << endl;
						break;
					case TMrbConfig::kCfgCreationDate:
						cfgStrm << cfgTmpl.Encode(line, fCreationDate) << endl;
						break;
					case TMrbConfig::kCfgCreateConfig:
						{
							expName = this->GetName();
							expName(0,1).ToUpper();
							cfgTmpl.InitializeCode();
							cfgTmpl.Substitute("$cfgClass", ClassName());
							cfgTmpl.Substitute("$cfgNameLC", GetName());
							cfgTmpl.Substitute("$cfgNameUC", expName);
							cfgTmpl.Substitute("$cfgTitle", GetTitle());
							cfgTmpl.WriteCode(cfgStrm);
						}
						break;
					case TMrbConfig::kCfgDefineEvents:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								evtNameLC = this->GetName();
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$evtNameLC", evtNameLC);
								cfgTmpl.Substitute("$evtNameUC", evtNameUC);
								cfgTmpl.Substitute("$evtTitle", evt->GetTitle());
								cfgTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgDefineSubevents:
						{
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameUC = sevt->GetName();
								sevtNameUC(0,1).ToUpper();
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$sevtClass", sevt->ClassName());
								cfgTmpl.Substitute("$sevtNameLC", sevt->GetName());
								cfgTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgAssignParams:
						{
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) sevt->MakeConfigCode(cfgStrm, tagIdx, cfgTmpl);
						}
						break;
					case TMrbConfig::kCfgDefineVariables:
						if (gMrbLofUserVars != NULL) {
							var = (TMrbVariable *) gMrbLofUserVars->First(kIsVariable);
							lvar = var;
							lofVars = "";
							while (var) {
								if (lofVars.Length() > 65 || var->GetType() != lvar->GetType()) {
									lofVars = lofVars.Strip(TString::kBoth);
									if (lofVars.Length() > 0) {
										cfgTmpl.InitializeCode();
										cfgTmpl.Substitute("$cfgNameLC", this->GetName());
										cfgTmpl.Substitute("$lofVars", lofVars);
										cfgTmpl.Substitute("$varType", vType);
										cfgTmpl.WriteCode(cfgStrm);
									}
									lofVars = "";
									lvar = var;
								}
								if (var->GetType() == lvar->GetType()) {
									switch (lvar->GetType()) {
										case kVarI:	vType = "I"; break;
										case kVarF:	vType = "F"; break;
										case kVarB:	vType = "B"; break;
										case kVarS:	vType = "S"; break;
									}
									lofVars += var->GetName();
									if (var->IsArray()) {
										lofVars += "[";
										TMrbString vSize(var->GetSize());
										lofVars += vSize;
										lofVars += "]";
									}
									lofVars += " ";
									lvar = var;
									var = (TMrbVariable *) gMrbLofUserVars->After((TObject *) var, kIsVariable);
								}
							}
							lofVars = lofVars.Strip(TString::kBoth);
							if (lofVars.Length() > 0) {
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$cfgNameLC", this->GetName());
								cfgTmpl.Substitute("$lofVars", lofVars);
								cfgTmpl.Substitute("$varType", vType);
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgDefineWindows:
						if (gMrbLofUserVars != NULL) {
							wdw = (TMrbWindow *) gMrbLofUserVars->First(kIsWindow);
							lwdw = wdw;
							lofWdws = "";
							while (wdw) {
								if (lofWdws.Length() > 65 || wdw->GetType() != lwdw->GetType()) {
									lofWdws = lofWdws.Strip(TString:: kBoth);
									if (lofWdws.Length() > 0) {
										cfgTmpl.InitializeCode();
										cfgTmpl.Substitute("$cfgNameLC", this->GetName());
										cfgTmpl.Substitute("$lofWdws", lofWdws);
										cfgTmpl.Substitute("$wdwType", wType);
										cfgTmpl.WriteCode(cfgStrm);
									}
									lofWdws = "";
									lwdw = wdw;
								}
								if (wdw->GetType() == lwdw->GetType()) {
									switch (lwdw->GetType()) {
										case kWindowI:	wType = "I"; break;
										case kWindowF:	wType = "F"; break;
										case kWindow2D:	wType = "C"; break;
									}
									lofWdws += wdw->GetName();
									lofWdws += " ";
									lwdw = wdw;
									wdw = (TMrbWindow *) gMrbLofUserVars->After((TObject *) wdw, kIsWindow);
								}
							}
							lofWdws = lofWdws.Strip(TString::kBoth);
							if (lofWdws.Length() > 0) {
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$cfgNameLC", this->GetName());
								cfgTmpl.Substitute("$lofWdws", lofWdws);
								cfgTmpl.Substitute("$wdwType", wType);
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgDefineModules:
						{
							module = (TMrbModule *) fLofModules.First();
							TIterator * modIter = fLofModules.MakeIterator();
							while ((module = (TMrbModule *) modIter->Next()) && (module->GetType()->GetIndex() & TMrbConfig::kModuleListMode)) {
								moduleNameUC = module->GetName();
								moduleNameUC(0,1).ToUpper();
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$moduleClass", module->ClassName());
								cfgTmpl.Substitute("$moduleNameLC", module->GetName());
								cfgTmpl.Substitute("$moduleTitle", module->GetTitle());
								if (module->IsCamac())
										cfgTmpl.Substitute("$modulePosition", ((TMrbCamacModule *) module)->GetPosition());
								else if (module->IsVME())
										cfgTmpl.Substitute("$modulePosition", ((TMrbVMEModule *) module)->GetPosition());
								else	cfgTmpl.Substitute("$modulePosition", "");
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgConnectToEvent:
						{
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) evt->MakeConfigCode(cfgStrm, tagIdx, cfgTmpl);
						}
						break;
					case TMrbConfig::kCfgDefineScalers:
						{
							TIterator * scaIter = fLofScalers.MakeIterator();
							while (scaler = (TMrbCamacScaler *) scaIter->Next()) {
								scalerNameUC = scaler->GetName();
								scalerNameUC(0,1).ToUpper();
								cfgTmpl.InitializeCode();
								cfgTmpl.Substitute("$scalerClass", scaler->ClassName());
								cfgTmpl.Substitute("$scalerNameLC", scaler->GetName());
								cfgTmpl.Substitute("$scalerTitle", scaler->GetTitle());
								cfgTmpl.Substitute("$scalerPosition", scaler->GetPosition());
								cfgTmpl.WriteCode(cfgStrm);
							}
						}
						break;
					case TMrbConfig::kCfgMakeCode:
						cfgTmpl.InitializeCode();
						cfgTmpl.Substitute("$cfgFile", cfile);
						cfgTmpl.Substitute("$cfgNameLC", this->GetName());
						cfgTmpl.WriteCode(cfgStrm);
						break;
				}
			}
		}
		cfgStrm.close();
		gMrbLog->Out() << "[" << cf << ": " << pp->GetC() << "]" << endl;
		gMrbLog->Flush("", "", setblue);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::MakeRcFile(const Char_t * CodeFile, const Char_t * ResourceName, Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeRcFile
// Purpose:        Create rc file
// Arguments:      Char_t * CodeFile         -- name of code file to be generated
//                 Char_t * ResourceName     -- main resource
//                 Option_t * Options        -- how to generate code: default ""
//                                              Overwrite: overwrite existing config file
//                                              Verbose: be verbose
// Results:
// Exceptions:
// Description:    Opens a file to store user's settings as environment data
//
//                 Template file should contain tags like %%TAG_NAME%%
//                 They will be expanded to real code by MakeRcFile.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;
	TString cfile;
	TString cf, tf, tf1, tf2;

	Int_t evtNo, sevtNo, modNo;

	TString resourceName;
	TString expName;

	TString iniTag;
	
	TMrbEvent * evt;
	TString evtNameUC;
	TString evtNameLC;

	TMrbSubevent * sevt;
	TString sevtNameUC;
	TString sevtNameLC;

	TMrbModule * module;
	TString moduleNameUC;
	TString moduleNameLC;

	TMrbNamedX * rcTag;
	TMrbConfig::EMrbRcFileTag tagIdx;

	TString lofSubevents;
	TString lofModules;

	TString templatePath;
	TString rcTemplateFile;
	TString prefix;
	
	ofstream rcStrm;

	TMrbTemplate rcTmpl;

	TObjArray filesToCreate;

	TMrbLofNamedX rcOptions;

	fRcFileOptions = rcOptions.CheckPatternShort(this->ClassName(), "MakeRcFile", Options, kMrbRcFileOptions);
	if (fRcFileOptions == TMrbLofNamedX::kIllIndexBit) return(kFALSE);

	if (!this->CheckConfig()) return(kFALSE);		// check if config consistent

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetRcFileOptions() & kRcOptVerbose) != 0);

	resourceName = (*ResourceName == '\0') ? this->ClassName() : ResourceName;

	prefix = this->GetName();
	prefix(0,1).ToUpper();
	
	cfile = ".";
	cfile += prefix;
	cfile += "Config";

	packNames arc(cfile.Data(), "Config.rc.code", ".rc", "Environment settings");
	filesToCreate.Add((TObject *) &arc);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	packNames * pp;
	TIterator * ppIter = filesToCreate.MakeIterator();
	while (pp = (packNames *) ppIter->Next()) {
		cf = pp->GetF() + pp->GetX();
		if (((this->GetRcFileOptions() & kRcOptOverwrite) == 0) && (gSystem->AccessPathName(cf) == 0)) {
			gMrbLog->Err() << "File exists - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeRcFile");
			continue;
		}
		rcStrm.open(cf, ios::out);
		if (!rcStrm.good()) {	
			gMrbLog->Err() << gSystem->GetError() << " - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "MakeRcFile");
			continue;
		}

		tf = pp->GetT();
		tf.Prepend(prefix.Data());
		tf1 = tf;
		TString fileSpec;
		TMrbSystem ux;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			tf = pp->GetT();
			tf2 = tf;
			ux.Which(fileSpec, templatePath.Data(), tf.Data());
		}
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "Template file not found -" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeRcFile");
			gMrbLog->Err()	<< "            Searching on path " << templatePath << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            for file " << tf1 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "            or       " << tf2 << endl;
			gMrbLog->Flush();
			rcStrm.close();
			continue;
		}

		rcTemplateFile = fileSpec;

		if (!rcTmpl.Open(rcTemplateFile, &fLofRcFileTags)) {
			if (verboseMode) {
				gMrbLog->Err()  << "Skipping template file " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "MakeRcFile");
			}
			continue;
		} else if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "MakeRcFile");
		}

		for (;;) {
			rcTag = rcTmpl.Next(line);
			if (rcTmpl.IsEof()) break;
			if (rcTmpl.IsError()) continue;
			if (rcTmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("#-") != 0) rcStrm << line << endl;
			} else if (this->ExecUserMacro(&rcStrm, this, rcTag->GetName())) {
				continue;
			} else {
				switch (tagIdx = (TMrbConfig::EMrbRcFileTag) rcTag->GetIndex()) {
					case TMrbConfig::kRcExpData:
						rcTmpl.InitializeCode();
						rcTmpl.Substitute("$resource", resourceName.Data());
						rcTmpl.Substitute("$expName", this->GetName());
						rcTmpl.Substitute("$expTitle", this->GetTitle());
						rcTmpl.Substitute("$nofEvents", fNofEvents);
						rcTmpl.Substitute("$lofEvents", this->GetLofEventsAsString(lofModules));
						rcTmpl.Substitute("$nofSubevents", fNofSubevents);
						rcTmpl.Substitute("$lofSubevents", this->GetLofSubeventsAsString(lofModules));
						rcTmpl.Substitute("$nofModules", fNofModules);
						rcTmpl.Substitute("$lofModules", this->GetLofModulesAsString(lofModules));
						rcTmpl.Substitute("$nofCrates", this->GetNofCrates());
						rcTmpl.Substitute("$cratePattern", (Int_t) this->GetCratePattern(), 16);
						rcTmpl.Substitute("$author", fAuthor);
						rcTmpl.Substitute("$creationDate", fCreationDate);
						rcTmpl.Substitute("$className", this->ClassName());
						rcTmpl.WriteCode(rcStrm);
						break;
					case TMrbConfig::kRcEvtData:
						{
							evtNo = 0;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								iniTag = (gMrbConfig->GetRcFileOptions() & kRcOptByName) ? "%NAM%" : "%NUM%";
								evtNameUC = evt->GetName();
								evtNameUC(0,1).ToUpper();
								rcTmpl.InitializeCode(iniTag.Data());
								rcTmpl.Substitute("$resource", resourceName.Data());
								rcTmpl.Substitute("$evtNo", evtNo);
								rcTmpl.Substitute("$evtNameLC", evt->GetName());
								rcTmpl.Substitute("$evtNameUC", evtNameUC.Data());
								rcTmpl.Substitute("$evtTitle", evt->GetTitle());
								rcTmpl.Substitute("$trigNo", (Int_t) evt->GetTrigger());
								rcTmpl.Substitute("$nofSubevents", evt->GetNofSubevents());
								rcTmpl.Substitute("$lofSubevents", evt->GetLofSubeventsAsString(lofModules));
								rcTmpl.Substitute("$evtType", (Int_t) evt->GetType());
								rcTmpl.Substitute("$evtSubtype", (Int_t) evt->GetSubtype());
								rcTmpl.Substitute("$className", evt->ClassName());
								rcTmpl.WriteCode(rcStrm);
								iniTag = (gMrbConfig->GetRcFileOptions() & kRcOptByName) ? "%NAMH%" : "%NUMH%";
								rcTmpl.InitializeCode(iniTag.Data());
								rcTmpl.Substitute("$resource", resourceName.Data());
								rcTmpl.Substitute("$evtNo", evtNo);
								rcTmpl.Substitute("$evtNameLC", evt->GetName());
								rcTmpl.Substitute("$evtNameUC", evtNameUC.Data());
								rcTmpl.Substitute("$nofEntries", evt->GetSizeOfHitBuffer());
								rcTmpl.Substitute("$highWater", evt->GetHBHighWaterLimit());
								rcTmpl.WriteCode(rcStrm);
								evt->MakeRcFile(rcStrm, tagIdx, resourceName.Data());
								evtNo++;
							}
						}
						break;
					case TMrbConfig::kRcSevtData:
						{
							sevtNo = 0;
							iniTag = (gMrbConfig->GetRcFileOptions() & kRcOptByName) ? "%NAM%" : "%NUM%";
							TIterator * sevtIter = fLofSubevents.MakeIterator();
							while (sevt = (TMrbSubevent *) sevtIter->Next()) {
								sevtNameUC = sevt->GetName();
								sevtNameUC(0,1).ToUpper();
								rcTmpl.InitializeCode(iniTag.Data());
								rcTmpl.Substitute("$resource", resourceName.Data());
								rcTmpl.Substitute("$sevtNo", sevtNo);
								rcTmpl.Substitute("$sevtNameLC", sevt->GetName());
								rcTmpl.Substitute("$sevtNameUC", sevtNameUC.Data());
								rcTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								rcTmpl.Substitute("$nofModules", sevt->GetNofModules());
								rcTmpl.Substitute("$lofModules", sevt->GetLofModulesAsString(lofModules));
								rcTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
								rcTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
								rcTmpl.Substitute("$sevtSerial", sevt->GetSerial());
								rcTmpl.Substitute("$className", sevt->ClassName());
								rcTmpl.WriteCode(rcStrm);
								sevt->MakeRcFile(rcStrm, tagIdx, resourceName.Data());
								sevtNo++;
							}
						}
						break;
					case TMrbConfig::kRcModuleData:
						{
							modNo = 0;
							iniTag = (this->GetRcFileOptions() & kRcOptByName) ? "%NAM%" : "%NUM%";
							TIterator * modIter = fLofModules.MakeIterator();
							while (module = (TMrbModule *) modIter->Next()) {
								moduleNameUC = module->GetName();
								moduleNameUC(0,1).ToUpper();
								rcTmpl.InitializeCode(iniTag.Data());
								rcTmpl.Substitute("$resource", resourceName.Data());
								rcTmpl.Substitute("$modNo", modNo);
								rcTmpl.Substitute("$moduleNameLC", module->GetName());
								rcTmpl.Substitute("$moduleNameUC", moduleNameUC.Data());
								rcTmpl.Substitute("$moduleTitle", module->GetTitle());
								rcTmpl.Substitute("$moduleSerial", module->GetSerial());
								rcTmpl.Substitute("$moduleType", module->GetType()->GetIndex(), 16);
								rcTmpl.Substitute("$className", module->ClassName());
								rcTmpl.Substitute("$nofChannelsUsed", module->GetNofChannelsUsed());
								rcTmpl.Substitute("$nofChannels", module->GetNofChannels());
								rcTmpl.Substitute("$channelPattern", (Int_t) module->GetPatternOfChannelsUsed(), 16);
								rcTmpl.Substitute("$nofSubdevices", module->GetNofSubDevices());
								rcTmpl.Substitute("$subDevice", module->GetSubDevice());
								if (module->IsCamac()) {
										rcTmpl.Substitute("$interface", "CAMAC");
										rcTmpl.Substitute("$moduleAddr", ((TMrbCamacModule *) module)->GetPosition());
										rcTmpl.Substitute("$moduleCrate", ((TMrbCamacModule *) module)->GetCrate());
										rcTmpl.Substitute("$moduleStation", ((TMrbCamacModule *) module)->GetStation());
								} else if (module->IsVME()) {
										rcTmpl.Substitute("$interface", "VME");
										rcTmpl.Substitute("$moduleAddr", ((TMrbVMEModule *) module)->GetPosition());
										rcTmpl.Substitute("$moduleCrate", ((TMrbCamacModule *) module)->GetCrate());
										rcTmpl.Substitute("$moduleStation", 0);
								}
								rcTmpl.WriteCode(rcStrm);
								module->MakeRcFile(rcStrm, tagIdx, resourceName.Data());
								modNo++;
							}
						}
						break;
					case TMrbConfig::kRcUserGlobals:
						{
							TMrbNamedX * ug;
							TIterator * globIter = fLofGlobals.MakeIterator();
							while (ug = (TMrbNamedX *) globIter->Next()) {
								if ((EMrbGlobalType) ug->GetIndex() != TMrbConfig::kGlobObject) {
									rcTmpl.InitializeCode();
									TString ugName = ug->GetName();
									if (ugName(0) == 'k') ugName = ugName(1, ugName.Length() - 1);
									ugName(0,1).ToUpper();
									rcTmpl.Substitute("$resource", resourceName.Data());
									rcTmpl.Substitute("$globalName", ugName);
									switch ((EMrbGlobalType) ug->GetIndex()) {
										case TMrbConfig::kGlobInt:
											rcTmpl.Substitute("$globalValue", this->GetGlobI(ug->GetName()));
											break;
										case TMrbConfig::kGlobFloat:
											rcTmpl.Substitute("$globalValue", this->GetGlobF(ug->GetName()));
											break;
										case TMrbConfig::kGlobDouble:
											rcTmpl.Substitute("$globalValue", this->GetGlobD(ug->GetName()));
											break;
										case TMrbConfig::kGlobBool:
											rcTmpl.Substitute("$globalValue", this->GetGlobB(ug->GetName()) ? "TRUE" : "FALSE");
											break;
										case TMrbConfig::kGlobString:
											rcTmpl.Substitute("$globalValue", this->GetGlobStr(ug->GetName()));
											break;
									}
								}
								rcTmpl.WriteCode(rcStrm);
							}
						}
						break;
				}
			}
		}
		rcStrm.close();
		if (*CodeFile != '\0') {
			cfile = CodeFile;
			cfile += ".rc";
			gSystem->Unlink(cfile.Data());
			gSystem->Link(cf.Data(), cfile.Data());
			cf= cfile;
		}
		gMrbLog->Out() << "[" << cf << ": " << pp->GetC() << "]" << endl;
		gMrbLog->Flush("", "", setblue);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::CallUserMacro(const Char_t * MacroName, Bool_t AclicFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CallUserMacro
// Purpose:        Define user macro to be called
// Arguments:      Char_t * Macroname        -- name of code file to be generated
//                 Bool_t AclicFlag          -- use ACliC to compile user macro
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Looks for a file <MacroName> to be called when generating code.
//                 Tries to compile it by ACliC of flag set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fUserMacro = MacroName;
	if (fUserMacro.Length() == 0) fUserMacro = "UserMacro.C";
	
	TString macroPath = gEnv->GetValue("Root.MacroPath", ".:$HOME/rootmacros:$MARABOU/macros");
	gSystem->ExpandPathName(macroPath);
	TString fileSpec;
	TMrbSystem ux;
	ux.Which(fileSpec, macroPath.Data(), fUserMacro.Data());
	if (fileSpec.IsNull()) {
		gMrbLog->Err()	<< "User macro not found -" << endl;
		gMrbLog->Flush(this->ClassName(), "CallUserMacro");
		gMrbLog->Err()	<< "            Searching on path " << macroPath << endl;
		gMrbLog->Flush();
		gMrbLog->Err()	<< "            for file " << fUserMacro << endl;
		gMrbLog->Flush();
		fUserMacroToBeCalled = kFALSE;
		this->CreateUserMacro();
	} else {
		fUserMacroCmd = fUserMacro;
		fUserMacroCmd.Remove(fUserMacroCmd.Index(".C"), 100);
		fUserMacroToBeCalled = kTRUE;
		TString aclic = fileSpec;
		if (AclicFlag) {
			TString aclicInclude = gSystem->GetIncludePath();
			TString mrbInclude = gSystem->ExpandPathName("$MARABOU/include");
			if (!aclicInclude.Contains(mrbInclude.Data())) {
				aclicInclude += " -I";
				aclicInclude += mrbInclude;
				gSystem->SetIncludePath(aclicInclude.Data());
			}

// now check if root and marabou libs have changed
// so we have to recompile anyway
			TMrbSystem ux;
			Long_t aclicTime;
			Long_t dmy, modTime;
			Long64_t dmy64;
			TString suffix = "+g";
			TString timeStamp;
			ux.GetDirName(timeStamp, aclic.Data());
			timeStamp += "/.UserMacro"; 						// (1) check if there is a timestamp called ".UserMacro"
			if (gSystem->GetPathInfo(timeStamp.Data(), &dmy, &dmy64, &dmy, &aclicTime) == 0) {
				TEnv * e = new TEnv(timeStamp.Data());			// (2) try to read defs for $ROOTSYS and $MARABOU then
				TString rootSys = e->GetValue("ROOTSYS", "");
				TString marabou = e->GetValue("MARABOU", "");
				if (rootSys.CompareTo(gSystem->Getenv("ROOTSYS")) == 0
				&&	marabou.CompareTo(gSystem->Getenv("MARABOU")) == 0) {	// if defs are as they should be
					TObjArray lofLibs;										// (3) check timestamp against system libs
					Int_t nofLibs = ux.FindFile(lofLibs, "*.so", "$MARABOU/lib:$ROOTSYS/lib");
					for (Int_t i = 0; i < nofLibs; i++) {
						TString fName = ((TObjString *) lofLibs[i])->GetString();
						gSystem->GetPathInfo(fName.Data(), &dmy, &dmy64, &dmy, &modTime);
						if (modTime > aclicTime) {				// found a more recent system lib -> recompile UserMacro.C
							suffix = "++g";
							break;
						}
					}
				} else {								// $ROOTSYS or $MARABOU have changed since last call -> recompile UserMacro.C 
					suffix = "++g";
				}
			} else {
				suffix = "++g"; 						// there is no timestamp ".UserMacro" -> recompile UserMacro.C
			}
			aclic += suffix;
			ofstream um(timeStamp.Data(), ios::out);	// write current values of $ROOTSYS and $MARABOU to .UserMacro
			if (um.good()) {
				um << "ROOTSYS: " << gSystem->Getenv("ROOTSYS") << endl;
				um << "MARABOU: " << gSystem->Getenv("MARABOU") << endl;
				um.close();
			}

			if (gMrbConfig->IsVerbose()) {
				gMrbLog->Out()  << "Compiling and/or loading user macro \"" << fileSpec
								<< "\" (option \"" << suffix << "\")"<< endl;
				gMrbLog->Flush(this->ClassName(), "CallUserMacro");
			}
		} else {
			if (gMrbConfig->IsVerbose()) {
				gMrbLog->Out()  << "Loading user macro \"" << fileSpec << "\" (interpreter mode)"<< endl;
				gMrbLog->Flush(this->ClassName(), "CallUserMacro");
			}
		}	
		Int_t errCode;
		Int_t sts = gROOT->LoadMacro(aclic.Data(), &errCode);
		if (sts == -1 || errCode != 0) {
			gMrbLog->Err()  << "Compile  step failed for user macro \"" << fileSpec << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "CallUserMacro");
			return(kFALSE);
		} else if (gMrbConfig->IsVerbose()) {
			gMrbLog->Out()  << "Initializing user macro \"" << fileSpec << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "CallUserMacro");
		}	
		TMrbString cmd = fUserMacroCmd;
		cmd += "Init()";
		gROOT->ProcessLine(cmd.Data());
	}
	return(fUserMacroToBeCalled);
}
	
Bool_t TMrbConfig::CreateUserMacro() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CreateUserMacro
// Purpose:        Create prototype of an user macro if none exists
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a prototype of an user macro.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);
	TString fileSpec;
	TMrbSystem ux;
	ux.Which(fileSpec, templatePath.Data(), "UserMacro.C.code");
	if (!fileSpec.IsNull()) {
		ofstream macroStrm(fUserMacro.Data(), ios::out);
		TMrbTemplate macroTmpl;
		if (macroStrm.good() && macroTmpl.Open(fileSpec.Data(), &fLofUserMacroTags)) {			
			for (;;) {
				TString line;
				TMrbNamedX * macroTag = macroTmpl.Next(line);
				if (macroTmpl.IsEof()) break;
				if (macroTmpl.IsError()) continue;
				if (macroTmpl.Status() & TMrbTemplate::kNoTag) {
					if (line.Index("#-") != 0) macroStrm << line << endl;
				} else {
					switch (macroTag->GetIndex()) {
						case TMrbConfig::kUmaFile:
							macroStrm << macroTmpl.Encode(line, fUserMacro.Data()) << endl;
							break;
						case TMrbConfig::kUmaNameLC:
							macroStrm << macroTmpl.Encode(line, this->GetName()) << endl;
							break;
						case TMrbConfig::kUmaNameUC:
							{
								TString macroName = this->GetName();
								macroName(0,1).ToUpper();
								macroStrm << macroTmpl.Encode(line, macroName) << endl;
							}
							break;
						case TMrbConfig::kUmaTitle:
							macroStrm << macroTmpl.Encode(line, this->GetTitle()) << endl;
							break;
						case TMrbConfig::kUmaAuthor:
							macroStrm << macroTmpl.Encode(line, fAuthor) << endl;
							break;
						case TMrbConfig::kUmaCreationDate:
							macroStrm << macroTmpl.Encode(line, fCreationDate) << endl;
							break;
					}
				}
			}
			macroStrm.close();
			gMrbLog->Out() << "Generating prototype for user macro \"" << fUserMacro << "\" (has to be edited)" << endl;
			gMrbLog->Flush(this->ClassName(), "CreateUserMacro");
			return(kTRUE);
		}	
	} else {
		gMrbLog->Err() << "Can't generate prototype for user macro \"" << fUserMacro << "\" - file \"UserMacro.C.code\" not found" << endl;
		gMrbLog->Flush(this->ClassName(), "CreateUserMacro");
		return(kFALSE);
	}
	return(kFALSE);
}
	
Bool_t TMrbConfig::ExecUserMacro(ofstream * Strm, TObject * CfgObject, const Char_t * TagWord) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ExecUserMacro
// Purpose:        Execute user macro
// Arguments:      ofStream * Strm         -- output stream
//                 TObject * CfgObject     -- config object (event, subevent, module)
//                 Char_t * TagWord        -- tag word
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes user macro given by method "CallUserMacro".
// Keywords:
//////////////////////////////////////////////////////////////////////////////
	
	Bool_t result;
	TMrbString cmd;
		
	if (fUserMacroToBeCalled) {
		cmd = fUserMacroCmd;
		cmd.SetBase(16);
		cmd += "((ofstream *) ";
		cmd += Form("%#lx", (ULong_t) Strm);
		cmd += ", (TMrbConfig *) ";
		cmd += Form("%#lx", (ULong_t) this);
		cmd += ", (TObject *) ";
		cmd += Form("%#lx", (ULong_t) CfgObject);
		cmd += ", \"";
		cmd += TagWord;
		cmd += "\", (Bool_t *) ";
		cmd += Form("%#lx", (ULong_t) &result);
		cmd += ")";
		cmd.ResetBase();
		gROOT->ProcessLine(cmd.Data());
		return(result);
	} else {
		return(kFALSE);
	}
}
							
Bool_t TMrbConfig::WriteDeadTime(const Char_t * Scaler, Int_t Interval) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::WriteDeadTime
// Purpose:        Enable dead-time output
// Arguments:      Char_t * Scaler   -- scaler name
//                 Int_t Interval    -- number of events a new dead time event will be created
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables dead-time information to be included in raw data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->DeadTimeToBeWritten()) {
		cout	<< setred
				<< this->ClassName() << "::WriteDeadTime(): Dead time scaler in "
				<< ((TMrbCamacScaler *) fDeadTimeScaler)->GetPosition() << " already activated, interval = "
				<< fDeadTimeInterval << " events"
				<< setblack << endl;
		return(kFALSE);
	}

	TMrbModule * module = this->FindScaler(Scaler);
	if (module == NULL) {
		gMrbLog->Err()	<< "No such scaler - " << Scaler << endl;
		gMrbLog->Flush(this->ClassName(), "WriteDeadTime");
		return(kFALSE);
	}

	if (Interval <= 0) {
		gMrbLog->Err()	<< "Illegal dead time interval - " << Interval << endl;
		gMrbLog->Flush(this->ClassName(), "WriteDeadTime");
		return(kFALSE);
	}

	TMrbScaler * sca = (TMrbScaler *) module;
	sca->SetDTScaler();
	fDeadTimeInterval = Interval;
	fDeadTimeScaler = sca;
	return(kTRUE);
}

Bool_t TMrbConfig::IncludeUserCode(const Char_t * IclPath, const Char_t * UserFile, Bool_t AutoGenFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IncludeUserCode
// Purpose:        Specify user code to be included
// Arguments:      const Char_t * IclPath      -- where to find user's includes
//                 Char_t * UserFile           -- name of user file
//                 Bool_t AutoGenFlag          -- kTRUE if (header) file auto-generated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Specifies user-definded code to be included.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString userPath = IclPath;
	TString userFile = UserFile;
	TMrbLofNamedX iclOptions;

	TMrbSystem ux;

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);
    iclOptions.AddNamedX(kMrbIncludeOptions);

	if (userPath.Length() == 0) userPath = gSystem->WorkingDirectory();
	else gSystem->ExpandPathName(userPath);

	TRegexp rxh("\\.h$");
	TRegexp rxcxx("\\.cxx$");

	if (fConfigChecked) {
		gMrbLog->Err()	<< "Statement out of phase - must be called *BEFORE* MakeReadoutCode()/MakeAnalyzeCode()" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
		return(kFALSE);
	}

	if (userFile.Index(rxh, 0) == -1 && userFile.Index(rxcxx, 0) == -1) {
		gMrbLog->Err()	<< "Wrong extension - " << userFile << " (should be \".cxx\" or \".h\")" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
		return(kFALSE);
	}

	TString fileSpec;
	ux.Which(fileSpec, userPath.Data(), userFile.Data());

	if (fileSpec.IsNull()) {
		if (AutoGenFlag && (userFile.Index(rxh, 0) >= 0)) {
			gMrbLog->Out()	<< "User-defined header file not found - "
							<< userPath << "/" << userFile
							<< " (expected to be auto-generated afterwards)" << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
			fLofUserIncludes.AddNamedX((Int_t) TMrbConfig::kIclOptHeaderFile, userFile.Data(), userFile.Data());
		} else {
			gMrbLog->Err()	<< "User code file not found - " << userPath << "/" << userFile << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
			return(kFALSE);
		}
	}

	ux.GetDirName(userPath, fileSpec.Data());
	userPath = ux.GetRelPath(userPath);

	if (userFile.Index(rxh, 0) >= 0) {
		fLofUserIncludes.AddNamedX((Int_t) TMrbConfig::kIclOptHeaderFile, userFile.Data(), userPath.Data());
		if (verboseMode) {
			gMrbLog->Out()  << "Using header file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
		}
	} else if (userFile.Index(rxcxx, 0) >= 0) {
		TString ptPath = fileSpec;
		ptPath.ReplaceAll(".cxx", ".h");
		ifstream f(ptPath.Data(), ios::in);
		if (!f.good()) {
			gMrbLog->Err() << "Prototype file missing - " << ptPath << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
			return(kFALSE);
		} else {
			if (verboseMode) {
				gMrbLog->Out()  << "Using code file " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
				gMrbLog->Out()  << "Using prototype file " << ptPath << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
			}
			TMrbString line;
			UInt_t iclOpts = 0;
			TMrbLofNamedX * lofMethods = new TMrbLofNamedX(userFile.Data());
			Int_t lineNo = 0;
			for (;;) {
				line.ReadLine(f, kFALSE);
				if (f.eof()) {
					f.close();
					break;
				}
				lineNo++;
				line.ReplaceWhiteSpace();
				line.ReplaceAll(" ::", "::");
				line.ReplaceAll(":: ", "::");
				line.ReplaceAll(" (", "(");
				if (line.Contains("(") && line.Contains(")")) {
					TString method = "";
					TObjArray splitted;
					if (line.Contains("::")) {
						splitted.Delete();
						Int_t n = line.Split(splitted, " ");
						for (Int_t i = 0; i < n; i++) {
							method = ((TObjString *) splitted[i])->GetString();
							if (method.Contains("::")) break;
						}
						method.Resize(method.Index("(", 0));
						n = method.Index("::", 0);
						TString scope = method;
						scope.Resize(n);
						TString mth = method;
						mth = mth(n + 2, 1000);
						if (scope.CompareTo("TMrbAnalyze") == 0) {
							if (mth.CompareTo("InitializeUserCode") == 0) {
								iclOpts |= TMrbConfig::kIclOptInitialize;
								lofMethods->AddNamedX(TMrbConfig::kIclOptInitialize, method.Data(), line.Data());
							} else if (mth.CompareTo("ReloadParams") == 0) {
								iclOpts |= TMrbConfig::kIclOptReloadParams;
								lofMethods->AddNamedX(TMrbConfig::kIclOptReloadParams, method.Data(), line.Data());
							} else if (mth.CompareTo("FinishRun") == 0) {
								iclOpts |= TMrbConfig::kIclOptFinishRun;
								lofMethods->AddNamedX(TMrbConfig::kIclOptFinishRun, method.Data(), line.Data());
							} else if (mth.CompareTo("HandleMessages") == 0) {
								iclOpts |= TMrbConfig::kIclOptHandleMessages;
								lofMethods->AddNamedX(TMrbConfig::kIclOptHandleMessages, method.Data(), line.Data());
							} else {
								gMrbLog->Err()  << "User code file " << ptPath << " (line " << lineNo
											<< "): Syntax error -" << endl
											<< "                               Unknown method " << method << "\"" << endl;
								gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
							}
						} else if (scope.Contains("TUsrEvt")) {
							Bool_t ok = kFALSE;
							TMrbEvent * evt;
							TIterator * evtIter = fLofEvents.MakeIterator();
							while (evt = (TMrbEvent *) evtIter->Next()) {
								TString evtName = evt->GetName();
								evtName(0,1).ToUpper();
								evtName.Prepend("TUsrEvt");
								if (scope.CompareTo(evtName.Data()) == 0) {
									ok = kTRUE;
									break;
								}
							}
							if (ok) {
								TString lineUC = line;
								lineUC.ToUpper();
								Bool_t knownMethod = kFALSE;
								TMrbNamedX * icl;
								TIterator * iclIter = iclOptions.MakeIterator();
								while (icl = (TMrbNamedX *) iclIter->Next()) {
									if (icl->GetIndex() & TMrbConfig::kIclOptClassTUsrEvent) {
										TString tag = icl->GetName();
										tag.Prepend("%%");
										tag += "%%";
										if (lineUC.Contains(tag.Data())) {
											iclOpts |= icl->GetIndex();
											lofMethods->AddNamedX(icl->GetIndex(), method.Data(), line.Data());
											Int_t n1 = line.Index("%%", 0);
											Int_t n2 = line.Index("%%", n1 + 2);
											TString keyWord = line(n1, n2 + 2);
											TString keyWordUC = keyWord;
											keyWordUC.ToUpper();
											if (verboseMode) {
												gMrbLog->Out()  << "Adding method " << method << " (key word " << keyWord << ")" << endl;
												gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
											}
											line.Remove(n1, n2 - n1 + 2);
											if (keyWordUC.CompareTo("%%ANALYZE%%") == 0) {
												gMrbLog->Wrn()  << "User code file " << ptPath << " (line " << lineNo
																<< "): Warning -" << endl
																<< "                               key word \"" << keyWord
																<< "\" is obsolete - should be replaced by %%ProcessEvent%%" << endl;
												gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
											}
											knownMethod = kTRUE;
											break;
										}
									}
								}
								if (!knownMethod) {
									Int_t n1;
									Int_t n2;
									if ((n1 = line.Index("%%", 0)) >= 0 && (n2 = line.Index("%%", n1 + 2)) > 0) {
										gMrbLog->Err()  << "User code file " << ptPath << " (line " << lineNo
														<< "): Syntax error -" << endl
														<< "                               Unknown key word \"" << line(n1, n2 + 2) << "\"" << endl;
										gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
									} else {
										iclOpts |= TMrbConfig::kIclOptEventMethod;
										lofMethods->AddNamedX(TMrbConfig::kIclOptEventMethod, method.Data(), line.Data());
										if (verboseMode) {
											gMrbLog->Out()  << "Adding method " << method << endl;
											gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
										}
									}
								}
							} else {
								gMrbLog->Err()  << "User code file " << ptPath << " (line " << lineNo
												<< "): Syntax error -" << endl
												<< "                               Wrong scope operator \"" << scope
												<< "::\" (should be \"TUsrEvt<EventName>::\")" << endl;
								gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
							}
						} else {
							Bool_t isUserClass = kFALSE;
							TMrbNamedX * nx;
							TIterator * ucIter = fLofUserClasses.MakeIterator();
							while (nx = (TMrbNamedX *) ucIter->Next()) {
								if (scope.CompareTo(nx->GetName()) == 0) {
									isUserClass = kTRUE;
									break;
								}
							}
							if (!isUserClass) {
								gMrbLog->Err()  << "User code file " << ptPath << " (line " << lineNo
												<< "): Syntax error -" << endl
												<< "                               Unknown scope operator \"" << scope << "\"" << endl;
								gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
							}
						}
					} else {
						splitted.Delete();
						Int_t n = line.Split(splitted, " ");
						for (Int_t i = 0; i < n; i++) {
							method = ((TObjString *) splitted[i])->GetString();
							if (method.Contains("(")) break;
						}
						method.Resize(method.Index("("));
						iclOpts |= TMrbConfig::kIclOptUtilities;
						lofMethods->AddNamedX(TMrbConfig::kIclOptUtilities, method.Data(), line.Data());
						if (verboseMode) {
							gMrbLog->Out()  << "Adding utility function " << method << endl;
							gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
						}
					}
				}
			}
			fileSpec = ux.GetRelPath(userPath);
			fLofUserIncludes.AddNamedX(iclOpts, userFile.Data(), userPath.Data(), lofMethods);
		}
	} else {
		gMrbLog->Err()	<< "User code file has wrong extension - " << userFile
						<< " (should be \".cxx\" or \".h\")" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserCode");
		return(kFALSE);
	}

	return(kTRUE);
}

Bool_t TMrbConfig::WriteUtilityProtos() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::WriteUtilityProtos
// Purpose:        Write header file for utilities
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates file UtilityProtos.h
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);

	if (this->UserCodeToBeIncluded()) {
		TString protoFile;
		Bool_t first = kTRUE;
		ofstream f;
		TMrbNamedX * icl;
		TIterator * iclIter = fLofUserIncludes.MakeIterator();
		while (icl = (TMrbNamedX *) iclIter->Next()) {
			if (icl->GetIndex() & TMrbConfig::kIclOptUtilities) {
				TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
				TMrbNamedX * nx;
				TIterator * methIter = lofMethods->MakeIterator();
				while (nx = (TMrbNamedX *) methIter->Next()) {
					if (nx->GetIndex() & TMrbConfig::kIclOptUtilities) {
						if (first) {
							protoFile = this->GetName();
							protoFile(0,1).ToUpper();
							protoFile += "UtilityProtos.h";
							f.open(protoFile.Data(), ios::out);
							if (!f.good()) {
								gMrbLog->Err()	<< "Can't write file - " << protoFile
												<< " (utility prototypes)" << setblack << endl;
								gMrbLog->Flush(this->ClassName(), "WriteUtilityProtos");
								return(kFALSE);
							}
						}
						first = kFALSE;
						f << nx->GetTitle() << endl;
					}
				}
			}
		}
		if (!first) {
			f.close();
			fLofUserIncludes.AddNamedX((Int_t) TMrbConfig::kIclOptHeaderFile, protoFile.Data(), "");
			if (verboseMode) {
				gMrbLog->Out()  << "Using prototype defs in file " << protoFile << endl;
				gMrbLog->Flush(this->ClassName(), "WriteUtilityProtos");
			}
		}			
	}
	return(kTRUE);
}
	
Bool_t TMrbConfig::IncludeUserLib(const Char_t * IclPath, const Char_t * UserLib, Bool_t MakeIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IncludeUserLib
// Purpose:        Specify user library to be included
// Arguments:      const Char_t * IclPath      -- where to find user's includes
//                 Char_t * UserLib            -- name of user library
//                 Bool_t MakeIt               -- include entry in make if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Specifies user-definded library to be included.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString userPath = IclPath;
	TString userLib = UserLib;

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);

	TRegexp rxlib("^lib");
	TRegexp rxso("\\.so$");

	TMrbSystem ux;

	if (fConfigChecked) {
		gMrbLog->Err()	<< "Statement out of phase - must be called *BEFORE* MakeReadoutCode()/MakeAnalyzeCode()" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
		return(kFALSE);
	}

	if (userLib.Index(rxlib, 0) == -1 || userLib.Index(rxso, 0) == -1) {
		gMrbLog->Err()	<< "Wrong library name - " << userLib
						<< " (should be \"libXYZ.so\")" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
		return(kFALSE);
	}

	TString ldLibraryPath = gSystem->Getenv("LD_LIBRARY_PATH");
	TString libPath = gSystem->ExpandPathName(userPath.Data());
	if (!ldLibraryPath.Contains(libPath.Data())) {
		gMrbLog->Wrn()	<< libPath << " is missing in your library path" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
		gMrbLog->Wrn()	<< endl
						<< "                      Execute command" << endl
						<< "                      >>> "
						<< setblack
						<< "export LD_LIBRARY_PATH="
						<< libPath
						<< ":$LD_LIBRARY_PATH" << setmagenta << " <<<" << endl
						<< "                      or add it to your profile permanently" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
	}

	TString libSo = userLib;
	userLib = userLib(3, userLib.Length() - 6);

	if (userPath.Length() == 0) userPath = gSystem->WorkingDirectory();
	else gSystem->ExpandPathName(userPath);

	TString fileSpec;
	ux.Which(fileSpec, userPath.Data(), libSo.Data());

	TString libSoPath;
	if (fileSpec.IsNull()) {
		libSoPath = userPath;
		libSoPath += "/";
		libSoPath = userLib;
		if (MakeIt) {
			gMrbLog->Wrn()	<< "User library not found - lib" << libSoPath << ".so" << endl
							<< "                              Has to be provided at load time" << endl;
		} else {
			gMrbLog->Wrn()	<< "User library not found - " << libSoPath << endl
							<< "                              Will be built during make step" << endl;
		}
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
	} else {
		libSoPath = fileSpec;
	}
	libSoPath = ux.GetRelPath(libSoPath);

	TString incPath = userPath;
	TString srcPath = userPath;
	Bool_t incX	= ux.IsDirectory(Form("%s/inc", userPath.Data()));
	Bool_t srcX	= ux.IsDirectory(Form("%s/src", userPath.Data()));
	if (incX) {
		if (srcX) {
			incPath = Form("%s/inc", userPath.Data());
			srcPath = Form("%s/src", userPath.Data());
		} else {
			gMrbLog->Err()	<< "[" << UserLib << "] Found subdirectory \"" << userPath << "/inc\" but no \"" << userPath << "/src\" - can't continue" << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
			return(kFALSE);
		}
	}

	TObjArray lofHfiles;
	lofHfiles.Delete();
	Int_t nofHfiles =  ux.FindFile(lofHfiles, "*.h", incPath.Data(), kTRUE);
	if (nofHfiles == 0) {
		gMrbLog->Err()	<< "No header file(s) found - " << incPath << "/*.h" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
		return(kFALSE);
	}

	for (Int_t i = 0; i < nofHfiles; i++) {
		TString hFile = ((TObjString *) lofHfiles[i])->GetString();
		ifstream f(hFile.Data(), ios::in);
		if (!f.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - "  << hFile << " (skipped)" << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
			continue;
		} else {
			hFile = ux.GetRelPath(hFile, incPath.Data());
			fLofUserIncludes.AddNamedX((Int_t) (TMrbConfig::kIclOptHeaderFile | TMrbConfig::kIclOptUserLib), hFile.Data(), incPath.Data());
			if (verboseMode) {
				gMrbLog->Out()  << "Using header file " << incPath << "/" << hFile << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
			}
			TString line;
			for (;;) {
				line.ReadLine(f, kFALSE);
				if (f.eof()) {
					f.close();
					break;
				}
				if (line.Contains("class ")) {
					Int_t n1 = line.Index("class ", 0) + sizeof("class ") - 1;
					Int_t n2 = line.Index(":", n1);
					if (n2 == -1) continue;
					TString userClass = line(n1, n2 - n1);
					userClass = userClass.Strip(TString::kBoth);
					this->AddUserClass(TMrbConfig::kIclOptUserLib, userClass.Data());
					if (verboseMode) {
						gMrbLog->Out()  << "Adding user class " << userClass << endl;
						gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
					}
				}
			}
		}
	}

	TObjString * mkf = NULL;
	if (MakeIt) {
		TString mkFile1 = Form("%s/%s.mk", userPath.Data(), userLib.Data());
		if (ux.IsRegular(mkFile1.Data())) {
			mkf = new TObjString(Form("%s.mk", userLib.Data()));
			if (verboseMode) {
				gMrbLog->Out()  << "[" << UserLib << "] Using makefile " << mkFile1 << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
			}
		} else {
			TString mkFile2 = Form("%s/Makefile", userPath.Data());
			if (ux.IsRegular(mkFile2.Data())) {
				if (verboseMode) {
					gMrbLog->Out()  << "[" << UserLib << "] Using makefile " << mkFile2 << endl;
					gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
				}
			} else {
				TString mkFile3 = Form("%s/makefile", userPath.Data());
				if (ux.IsRegular(mkFile3.Data())) {
					if (verboseMode) {
						gMrbLog->Out()  << "[" << UserLib << "] Using makefile " << mkFile3 << endl;
						gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
					}
				} else {
					gMrbLog->Err()	<< "[" << UserLib << "] No makefile found - neither \"" << mkFile1
									<< "\" nor \"" << mkFile2 << "\" nor \"" << mkFile3 << "\"" << endl;
					gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
				}
			}
		}
	}

	fLofUserLibs.AddNamedX(MakeIt ? 1 : 0, userLib.Data(), userPath.Data(), mkf);
	if (verboseMode) {
			gMrbLog->Out() << "Using user library " << libSoPath;
			if (MakeIt) gMrbLog->Out() << " (will be rebuilt during make step)";
			gMrbLog->Out() << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserLib");
	}

	return(kTRUE);
}

Bool_t TMrbConfig::IncludeUserClass(const Char_t * IclPath, const Char_t * UserFile, Bool_t UserDefinedEvent) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IncludeUserClass
// Purpose:        Specify user class to be included
// Arguments:      const Char_t * IclPath      -- where to find user's includes
//                 Char_t * UserFile           -- name of user file
//                 Bool_t UserDefinedEvent     -- if kTRUE: use as event class
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Specifies user-definded class to be included.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString userPath = IclPath;
	TString userFile = UserFile;

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);

	if (userPath.Length() == 0) userPath = gSystem->WorkingDirectory();
	else gSystem->ExpandPathName(userPath);

	TRegexp rxh("\\.h$");
	TRegexp rxcxx("\\.cxx$");

	if (fConfigChecked) {
		gMrbLog->Err()	<< "Statement out of phase - must be called *BEFORE* MakeReadoutCode()/MakeAnalyzeCode()" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		return(kFALSE);
	}

	if (userFile.Index(rxh, 0) == -1 && userFile.Index(rxcxx, 0) == -1) {
		gMrbLog->Err()	<< "Wrong extension - " << userFile << " (should be \".cxx\" or \".h\")" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		return(kFALSE);
	}

	TString uevtName = userFile;
	Int_t n = (uevtName.EndsWith(".cxx")) ? 4 : 2;
	uevtName.Resize(uevtName.Length() - n);

	TString fileSpec;
	TMrbSystem ux;
	ux.Which(fileSpec, userPath.Data(), userFile.Data());

	EMrbIncludeOptions classOpt = UserDefinedEvent ? TMrbConfig::kIclOptUserDefinedEvent : TMrbConfig::kIclOptUserClass;

	TString ptPath;

	if (fileSpec.IsNull()) {
		if (UserDefinedEvent) {
			ptPath = userPath;
			ptPath += "/";
			ptPath += userFile;
			fileSpec = ptPath.Data();
		} else {
			gMrbLog->Err()	<< "User code file not found - " << userPath << "/" << userFile << endl;
			gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
			return(kFALSE);
		}
	}
	
	ptPath = fileSpec;
	ptPath.ReplaceAll(".cxx", ".h");
	if (gSystem->AccessPathName(ptPath.Data()) != 0) {
		gMrbLog->Err() << "Can't include user class - prototype file \"" << ptPath << "\" missing" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		if (UserDefinedEvent) {
			ofstream uevtStrm(ptPath.Data(), ios::out);
			if (!uevtStrm.good()) {
				gMrbLog->Err() << gSystem->GetError() << " - "  << ptPath << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
				return(kFALSE);
			}
			Bool_t ok = this->CreateUserEvent(uevtStrm, uevtName.Data(), kTRUE, kFALSE);
			uevtStrm.close();
			if (ok) {
				gMrbLog->Out() << "Generating prototype file \"" << ptPath << "\" for user event \"" << uevtName << "\" (has to be edited)" << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
			} else {
				gMrbLog->Err() << "Can't generate prototype file \"" << userFile << "\" for user event \"" << uevtName << "\"" << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
				return(kFALSE);
			}
		} else {
			return(kFALSE);
		}
	} else if (verboseMode) {
		gMrbLog->Out()  << "Using prototype file " << ptPath << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
	}

	ptPath = fileSpec;
	ptPath.ReplaceAll(".h", ".cxx");
	if (gSystem->AccessPathName(ptPath.Data()) != 0) {
		gMrbLog->Err() << "Can't include user class - code file \"" << ptPath << "\" missing" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		if (UserDefinedEvent) {
			ofstream uevtStrm(ptPath.Data(), ios::out);
			if (!uevtStrm.good()) {
				gMrbLog->Err() << gSystem->GetError() << " - "  << ptPath << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
				return(kFALSE);
			}
			Bool_t ok = this->CreateUserEvent(uevtStrm, uevtName.Data(), kFALSE, kFALSE);
			uevtStrm.close();
			if (ok) {
				gMrbLog->Out() << "Generating code file \"" << ptPath << "\" for user event \"" << uevtName << "\" (has to be edited)" << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
			} else {
				gMrbLog->Err() << "Can't generate code file \"" << userFile << "\" for user event \"" << uevtName << "\"" << endl;
				gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
				return(kFALSE);
			}
		} else {
			return(kFALSE);
		}
	} else if (verboseMode) {
		gMrbLog->Out()  << "Using code file " << ptPath << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
	}

	userPath = ux.GetRelPath(userPath);
	userFile.ReplaceAll(".h", ".cxx");
	fLofUserIncludes.AddNamedX(classOpt, userFile.Data(), userPath.Data());
	userFile.ReplaceAll(".cxx", ".h");
	fLofUserIncludes.AddNamedX(TMrbConfig::kIclOptHeaderFile, userFile.Data(), userPath.Data());

	ptPath = fileSpec;
	ptPath.ReplaceAll(".cxx", ".h");
	ifstream f(ptPath.Data(), ios::in);
	Bool_t classOk = kFALSE;
	if (!f.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - "  << userFile << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		return(kFALSE);
	} else {
		TString line;
		for (;;) {
			line.ReadLine(f, kFALSE);
			if (f.eof()) {
				f.close();
				break;
			}
			Int_t idx = line.Index("//", 0);
			if (idx == -1) idx = line.Index("/*", 0);
			if (idx != -1) line.Resize(idx);
			line = line.Strip(TString::kBoth);
			if (line.BeginsWith("class ")) {
				Int_t n1 = line.Index("class ", 0) + sizeof("class ") - 1;
				Int_t n2 = line.Index(":", n1);
				if (n2 == -1) n2 = line.Index("{", n1);
				if (n2 == -1) n2 = line.Length();
				TString userClass = line(n1, n2 - n1);
				userClass = userClass.Strip(TString::kBoth);
				if (UserDefinedEvent) {
					if (classOk) {
						gMrbLog->Wrn()	<< "[" << userFile << "] More than one class def - class \"" << userClass << "\" not used as event class" << endl;
						gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
						continue;
					}
					if (uevtName.CompareTo(userClass.Data()) != 0) {
						gMrbLog->Wrn()	<< "[" << userFile << "] Class name \"" << userClass
										<< "\" different from file name - not used as event class" << endl;
						gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
						continue;
					}
				}
				if (verboseMode) {
					gMrbLog->Out()  << "[" << userFile << "] Defining user class \"" << userClass << "\"" << endl;
					gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
				}
				this->AddUserClass(classOpt, userClass.Data());
				classOk = kTRUE;
			}
		}
	}
	
	if (!classOk) {
		gMrbLog->Err()  << "Prototype file " << ptPath << " doesn't contain any class definition" << endl;
		gMrbLog->Flush(this->ClassName(), "IncludeUserClass");
		return(kFALSE);
	}

	return(kTRUE);
}

Bool_t TMrbConfig::CreateUserEvent(ofstream & OutStrm, const Char_t * UserEvent, Bool_t CreateProto, Bool_t SystemPart) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CreateUserEvent
// Purpose:        Create files/methods for a user-defined event class
// Arguments:      ofstream OutStrm    -- output stream
//                 Char_t * UserEvent  -- event name
//                 Bool_t CreateProto  -- create prototypes for header file
//                 Bool_t SystemPart   -- kTRUE if system part is to be generated
//                                        kFALSE for user part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates files & methods for a user-defined event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	TString uevtName = UserEvent;

	TString tmplFile;
	if (CreateProto) {
		tmplFile = "UserEvent.h.code";
	} else if (SystemPart) {
		tmplFile = "UserEventSystem.cxx.code";
	} else {
		tmplFile = "UserEventUser.cxx.code";
	}

	TString fileSpec ;
	TMrbSystem ux;
	ux.Which(fileSpec, templatePath.Data(), tmplFile.Data());
	if (!fileSpec.IsNull()) {
		TMrbTemplate uevtTmpl;
		if (uevtTmpl.Open(fileSpec.Data(), &fLofUserEventTags)) {			
			for (;;) {
				TString line;
				TMrbNamedX * uevtTag = uevtTmpl.Next(line);
				if (uevtTmpl.IsEof()) break;
				if (uevtTmpl.IsError()) continue;
				if (uevtTmpl.Status() & TMrbTemplate::kNoTag) {
					if (line.Index("#-") != 0) OutStrm << line << endl;
				} else {
					switch (uevtTag->GetIndex()) {
						case TMrbConfig::kUevNameLC:
							{
								TString nameLC = uevtName;
								nameLC(0,1).ToLower();
								OutStrm << uevtTmpl.Encode(line,nameLC) << endl;
							}
							break;
						case TMrbConfig::kUevNameUC:
							{
								TString nameUC = uevtName;
								nameUC(0,1).ToUpper();
								OutStrm << uevtTmpl.Encode(line, nameUC) << endl;
							}
							break;
						case TMrbConfig::kUevConfigLC:
							OutStrm << uevtTmpl.Encode(line, this->GetName()) << endl;
							break;
						case TMrbConfig::kUevConfigUC:
							{
								TString nameUC = this->GetName();
								nameUC(0,1).ToUpper();
								OutStrm << uevtTmpl.Encode(line, nameUC) << endl;
							}
							break;
						case TMrbConfig::kUevTitle:
							OutStrm << uevtTmpl.Encode(line, this->GetTitle()) << endl;
							break;
						case TMrbConfig::kUevAuthor:
							OutStrm << uevtTmpl.Encode(line, fAuthor) << endl;
							break;
						case TMrbConfig::kUevCreationDate:
							OutStrm << uevtTmpl.Encode(line, fCreationDate) << endl;
							break;
					}
				}
			}
			return(kTRUE);
		}	
	} else {
		return(kFALSE);
	}
	return(kFALSE);
}
	
Bool_t TMrbConfig::CreateXhit(TMrbNamedX * Xhit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CreateXhit
// Purpose:        Create files/methods for a user-defined hit class
// Arguments:      TMrbNamedX * Xhit   -- hit specs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates files & methods for a user-defined hit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ofstream outStrm;

	TString templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	TMrbTemplate xHitTmpl;

	TObjArray filesToCreate;
	TString cf, tf;

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0) ;

	packNames acxx(Xhit->GetName(), "Xhit.cxx.code", ".cxx", "Extended hit (code)");
	filesToCreate.Add((TObject *) &acxx);

	packNames ah(Xhit->GetName(), "Xhit.h.code", ".h", "Extended hit (protos)");
	filesToCreate.Add((TObject *) &ah);

	packNames * pp;
	TIterator * ppIter = filesToCreate.MakeIterator();
	while (pp = (packNames *) ppIter->Next()) {
		cf = pp->GetF() + pp->GetX();
		outStrm.open(cf, ios::out);
		if (!outStrm.good()) {	
			gMrbLog->Err() << gSystem->GetError() << " - " << cf << endl;
			gMrbLog->Flush(this->ClassName(), "CreateXhit");
			continue;
		}

		tf = pp->GetT();
		TString fileSpec;
		TMrbSystem ux;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "Template file not found - " << tf << endl;
			gMrbLog->Flush(this->ClassName(), "CreateXhit");
			gMrbLog->Err()	<< "            Searching on path " << templatePath << endl;
			gMrbLog->Flush();
			outStrm.close();
			continue;
		}

		if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "CreateXhit");
		}
	
		TString xHitTemplateFile = fileSpec;

		if (!xHitTmpl.Open(xHitTemplateFile, &fLofXhitTags)) {
			if (verboseMode) {
				gMrbLog->Err()  << "Skipping template file " << fileSpec << endl;
				gMrbLog->Flush(this->ClassName(), "CreateXhit");
			}
			continue;
		} else if (verboseMode) {
			gMrbLog->Out()  << "Using template file " << fileSpec << endl;
			gMrbLog->Flush(this->ClassName(), "CreateXhit");
		}

		for (;;) {
			TString line;
			TMrbNamedX * xHitTag = xHitTmpl.Next(line);
			if (xHitTmpl.IsEof()) break;
			if (xHitTmpl.IsError()) continue;
			if (xHitTmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("#-") != 0) outStrm << line << endl;
			} else {
				switch (xHitTag->GetIndex()) {
					case TMrbConfig::kXhitNameLC:
						{
							TString nameLC = Xhit->GetName();
							nameLC(0,1).ToLower();
							outStrm << xHitTmpl.Encode(line,nameLC) << endl;
						}
						break;
					case TMrbConfig::kXhitNameUC:
						{
							TString nameUC = Xhit->GetName();
							nameUC(0,1).ToUpper();
							outStrm << xHitTmpl.Encode(line, nameUC) << endl;
						}
						break;
					case TMrbConfig::kXhitConfigLC:
						outStrm << xHitTmpl.Encode(line, this->GetName()) << endl;
						break;
					case TMrbConfig::kXhitConfigUC:
						{
							TString nameUC = this->GetName();
							nameUC(0,1).ToUpper();
							outStrm << xHitTmpl.Encode(line, nameUC) << endl;
						}
						break;
					case TMrbConfig::kXhitTitle:
						outStrm << xHitTmpl.Encode(line, this->GetTitle()) << endl;
						break;
					case TMrbConfig::kXhitAuthor:
						outStrm << xHitTmpl.Encode(line, fAuthor) << endl;
						break;
					case TMrbConfig::kXhitCreationDate:
						outStrm << xHitTmpl.Encode(line, fCreationDate) << endl;
						break;
					case TMrbConfig::kXhitDataLength:
						{
							TMrbString dl = Xhit->GetIndex();
							outStrm << xHitTmpl.Encode(line, dl.Data()) << endl;
						}
						break;
				}
			}
		}	
		outStrm.close();
		gMrbLog->Out() << "[" << cf << ": " << pp->GetC() << "]" << endl;
		gMrbLog->Flush("", "", setblue);
	}
	return(kTRUE);
}
	
void TMrbConfig::AddEvent(TMrbEvent * Evt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddEvent
// Purpose:        Add event to list
// Arguments:      TMrbEvent * Evt   -- event
// Results:        --
// Exceptions:
// Description:    Adds an event to list of events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofEvents.Add(Evt);
	fNofEvents++;
}

void TMrbConfig::AddSubevent(TMrbSubevent * Sevt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddSubevent
// Purpose:        Add subevent to list
// Arguments:      TMrbEvent * Sevt   -- subevent
// Results:        --
// Exceptions:
// Description:    Adds a subevent to list of subevents
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofSubevents.Add(Sevt);
	fNofSubevents++;
}

void TMrbConfig::AddScaler(TMrbModule * Scaler) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddScaler
// Purpose:        Add scaler to list
// Arguments:      TMrbModule * Scaler   -- scaler
// Results:        --
// Exceptions:
// Description:    Adds a scaler to list of scalers
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofScalers.Add(Scaler);
	fNofScalers++;
}

void TMrbConfig::AddMux(TMrbModule * Multiplexer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddMux
// Purpose:        Add (Mesytec) multiplexer to list
// Arguments:      TMrbMesytec_Mux16 * Multiplexer   -- multiplexer
// Results:        --
// Exceptions:
// Description:    Adds a multiplexer to list of multiplexers
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofMuxs.Add(Multiplexer);
	fNofMuxs++;
}

void TMrbConfig::AddToTagList(const Char_t * CodeFile, Int_t TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddToTagList
// Purpose:        Add a tag to be processed only once
// Arguments:      Char_t * CodeFile         -- code file
//                 Int_t TagIndex            -- tag index
// Results:        --
// Exceptions:
// Description:    Keeps track of once-only code files.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString tag;

	tag = CodeFile;
	tag += ":";
	tag += TagIndex;
	fLofOnceOnlyTags.Add(new TObjString(tag.Data()));
}

Bool_t TMrbConfig::TagToBeProcessed(const Char_t * CodeFile, Int_t TagIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::TagTobeProcessed
// Purpose:        Check if tag has to be processed
// Arguments:      Char_t * CodeFile         -- code file
//                 Int_t TagIndex            -- tag index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Searches for file in the list of once-only code files.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString tag;

	tag = CodeFile;
	tag += ":";
	tag += TagIndex;
	return(fLofOnceOnlyTags.FindObject(tag.Data()) == NULL);
}

Bool_t TMrbConfig::ExecRootMacro(const Char_t * Macro) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ExecRootMacro
// Purpose:        Execute ROOT macro
// Arguments:      Char_t * Macro   -- macro name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if macro file is present. Execs via gROOT->Macro()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString macroPath = gEnv->GetValue("Unix.*.Root.MacroPath", ".:macros:$(MARABOU)/macros");
	gSystem->ExpandPathName(macroPath);
	TString macroFile;
	TMrbSystem ux;
	ux.Which(macroFile, macroPath.Data(), Macro);
	if (macroFile.IsNull()) {
        gMrbLog->Err() << "Macro file not found - " << Macro << " (searched on .rootrc:Unix.*.Root.MacroPath)" << endl;
		gMrbLog->Flush(this->ClassName(), "ExecRootMacro");
		return(kFALSE);
	}
	gROOT->Macro(macroFile.Data());
	return(kTRUE);
}

void TMrbConfig::PrintToFile(Char_t * File) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::PrintToFile
// Purpose:        Output current config to file
// Arguments:      Char_t * File  -- file name
// Results:        --
// Exceptions:
// Description:    Outputs current configuration to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ofstream f(File, ios::out);
	if (!f.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << File << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
	} else {
		Print(f);
		gMrbLog->Out()	<< "[" << File << ": Printout of config data]" << endl;
		gMrbLog->Flush("", "", setblue);
		f.close();
	}
}

void TMrbConfig::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Print
// Purpose:        Output current config
// Arguments:      ofstream & OutStrm   -- output stream
//                 Char_t * Prefix  -- prefix to be output in front of text
// Results:        --
// Exceptions:
// Description:    Outputs current configuration to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;

	OutStrm << Prefix << endl;
	OutStrm << Prefix << "Exp Configuration:" << endl;
	OutStrm << Prefix << "|  Name          : " << GetName() << endl;
	OutStrm << Prefix << "|  Title         : " << GetTitle() << endl;
	OutStrm << Prefix << "|  Events/Trigs  : " << fNofEvents << endl;
	OutStrm << Prefix << "|  Subevents     : " << fNofSubevents << endl;
	if (fNofEvents > 0) {
		TIterator * evtIter = fLofEvents.MakeIterator();
		while (evt = (TMrbEvent *) evtIter->Next()) evt->Print(OutStrm, Prefix);
	}
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->IsCamac()) {
			((TMrbCamacModule *) module)->Print(OutStrm, Prefix);
		} else {
			((TMrbVMEModule *) module)->Print(OutStrm, Prefix);
		}
	}
}

Int_t TMrbConfig::PrintErrors(Bool_t ErrorsOnly) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::PrintErrors
// Purpose:        Print error summary
// Arguments:      Bool_t ErrorsOnly   -- print error summary only
// Results:        Int_t NofErrors     -- number of errors
// Exceptions:
// Description:    Outputs logger contents
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray err;
	Int_t nofErrors = gMrbLog->GetErrors(err);
	TObjArray wrn;
	Int_t nofWarnings = (ErrorsOnly) ? 0 : gMrbLog->GetWarnings(wrn);

	if (nofErrors == 0 && nofWarnings == 0) {
		cout	<< setgreen << "[No errors during config step]" << setblack << endl;
		return(0);
	}

	if (nofErrors > 0) {
		cerr	<< setred << endl
				<< "ERROR(S) while processing config script:" << setblack << endl;
		gMrbLog->Print(0, "Error");
	}

	if (ErrorsOnly) return(nofErrors);

	if (nofWarnings > 0) {
		cerr	<< setmagenta << endl
				<< "WARNING(S) while processing config script:" << setblack << endl;
		gMrbLog->Print(0, "Warning");
	}
	if (nofErrors || nofWarnings) cerr	<< setblack << endl;
	return(nofErrors);
}

Int_t TMrbConfig::GetNofErrors() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetNofErrors
// Purpose:        Return number of errors
// Arguments:      --
// Results:        Int_t NofErrors   -- number of errors
// Exceptions:
// Description:    Returns number of errors
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMrbLog->GetNofEntries((UInt_t) TMrbLogMessage::kMrbMsgError));
}

void TMrbConfig::Version() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Version
// Purpose:        Output version
// Arguments:      --
// Results:        
// Exceptions:
// Description:    Welcome to TMrbConfig.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString vtext;
	TString frame;
	Int_t fl, i1, i2;

	frame = "+-------------------------------------------------+";
	vtext = "TMrbConfig v%I% (%G%)";
	fl = frame.Length() - 2;
	i1 = (fl - vtext.Length())/2;
	i2 = fl - vtext.Length() - i1;

	cout << "  " << frame << endl;

	cout << "  |";
	cout << setw(i1) << " ";
	cout << vtext;
	cout << setw(i2) << " ";
	cout << "|" << endl; 

	cout << "  |  MARaBOU classes to describe exp configuration  |" << endl;
	cout << "  |                  (c) R. Lutter                  |" << endl;
	cout << "  " << frame << endl << endl;
}

Bool_t TMrbConfig::DefineVariables(const Char_t * VarType, const Char_t * VarDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineVariables
// Purpose:        Defines a set of user variables
// Arguments:      Char_t * VarType     -- type: I(nteger), F(loat) or S(tring)
//                 Char_t * VarDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of user variables.
//                 VarDefs may be a single variable name or a set of names,
//                 separated by blanks.
//                 To define an indexed variable (array variable) user may
//                 append the array size in [...].
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    TMrbLofNamedX varTypes;
    TMrbNamedX * vType;
	Bool_t sts;

	TObject * proto = NULL;

    varTypes.AddNamedX(kMrbVariableTypes);
	varTypes.SetPatternMode();
    vType = varTypes.FindByName(VarType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
    if (vType == NULL) {
        gMrbLog->Err() << "Illegal variable type - " << VarType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineVariables");
        return(kFALSE);
    }

	switch (vType->GetIndex()) {
		case kVarI:	proto = (TObject *) new TMrbVarI("Proto", 0); break;
		case kVarF:	proto = (TObject *) new TMrbVarF("Proto", 0.); break;
		case kVarS:	proto = (TObject *) new TMrbVarS("Proto", ""); break;
	}

	sts = DefineVarOrWdw(vType, proto, VarDefs);
	delete proto;
	return(sts);
}

Bool_t TMrbConfig::DefineVariables(const Char_t * VarType, Int_t Value, const Char_t * VarDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineVariables
// Purpose:        Defines a set of user variables
// Arguments:      Char_t * VarType     -- type: I(nteger), F(loat) or S(tring)
//                 Int_t Value          -- initial value
//                 Char_t * VarDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of variables and initializes all with a given
//                 value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    TMrbLofNamedX varTypes;
    TMrbNamedX * vType;
	Bool_t sts;

	TObject * proto = NULL;

    varTypes.AddNamedX(kMrbVariableTypes);
	varTypes.SetPatternMode();
    vType = varTypes.FindByName(VarType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
    if (vType == NULL) {
        gMrbLog->Err() << "Illegal variable type - " << VarType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineVariables");
		return(kFALSE);
    }

	switch (vType->GetIndex()) {
		case kVarI:	proto = (TObject *) new TMrbVarI("Proto", Value); break;
		case kVarF:	proto = (TObject *) new TMrbVarF("Proto", (Float_t) Value); break;
		case kVarS:
			gMrbLog->Err() << "String variables can't be initialized with " << Value << endl;
			gMrbLog->Flush(this->ClassName(), "DefineVariables");
			return(kFALSE);
	}

	sts = DefineVarOrWdw(vType, proto, VarDefs);
	delete proto;
	return(sts);
}

Bool_t TMrbConfig::DefineVariables(const Char_t * VarType, Double_t Value, const Char_t * VarDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineVariables
// Purpose:        Defines a set of user variables
// Arguments:      Char_t * VarType     -- type: I(nteger), F(loat) or S(tring)
//                 Float_t Value        -- initial value
//                 Char_t * VarDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of variables and initializes all with a given
//                 value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    TMrbLofNamedX varTypes;
    TMrbNamedX * vType;
	Bool_t sts;

	TObject * proto = NULL;

    varTypes.AddNamedX(kMrbVariableTypes);
	varTypes.SetPatternMode();
    vType = varTypes.FindByName(VarType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
    if (vType == NULL) {
        gMrbLog->Err() << "Illegal variable type - " << VarType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineVariables");
        return(kFALSE);
    }

	switch (vType->GetIndex()) {
		case kVarI:	proto = (TObject *) new TMrbVarI("Proto", (Int_t) (Value + .5)); break;
		case kVarF:	proto = (TObject *) new TMrbVarF("Proto", (Float_t) Value); break;
		case kVarS:
			gMrbLog->Err() << "String variables can't be initialized with " << Value << endl;
			gMrbLog->Flush(this->ClassName(), "DefineVariables");
			return(kFALSE);
	}

	sts = DefineVarOrWdw(vType, proto, VarDefs);
	delete proto;
	return(sts);
}

Bool_t TMrbConfig::DefineVariables(const Char_t * VarType, const Char_t * Value, const Char_t * VarDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineVariables
// Purpose:        Defines a set of user variables
// Arguments:      Char_t * VarType     -- type: I(nteger), F(loat) or S(tring)
//                 Char_t Value         -- initial value
//                 Char_t * VarDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of variables and initializes all with a given
//                 value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    TMrbLofNamedX varTypes;
    TMrbNamedX * vType;
	Bool_t sts;

	TObject * proto = NULL;

    varTypes.AddNamedX(kMrbVariableTypes);
	varTypes.SetPatternMode();
    vType = varTypes.FindByName(VarType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
    if (vType == NULL) {
        gMrbLog->Err() << "Illegal variable type - " << VarType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineVariables");
        return(kFALSE);
    }

	switch (vType->GetIndex()) {
		case kVarI:
		case kVarF:
			gMrbLog->Err() << "Int/Float variables can't be initialized with string " << Value << endl;
			gMrbLog->Flush(this->ClassName(), "DefineVariables");
			return(kFALSE);
		case kVarS:	proto = (TObject *) new TMrbVarS("Proto", Value); break;
	}

	sts = DefineVarOrWdw(vType, proto, VarDefs);
	delete proto;
	return(sts);
}

Bool_t TMrbConfig::DefineWindows(const Char_t * WdwType, const Char_t * WdwDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineWindows
// Purpose:        Defines a set of windows (1- or 2-dim)
// Arguments:      Char_t * WdwType     -- type: I(nteger), F(loat), or C(ut)
//                 Char_t * WdwDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of windows (1- or 2-dim).
//                 WdwDefs may be a single window name or a set of names,
//                 separated by blanks.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX wdwTypes;
	TMrbNamedX * wType;

	Bool_t sts;
	TObject * proto = NULL;

	wdwTypes.AddNamedX(kMrbWindowTypes);
	wdwTypes.SetPatternMode();
	wType = wdwTypes.FindByName(WdwType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (wType == NULL) {
		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineWindows");
		return(kFALSE);
	}

	switch (wType->GetIndex()) {
		case kWindowI:	proto = (TObject *) new TMrbWindowI("Proto", 0, 0); break;
		case kWindowF:	proto = (TObject *) new TMrbWindowF("Proto", 0., 0.); break;
		case kWindow2D: {
							Double_t x = 0.;
							Double_t y = 0.;
							proto = (TObject *) new TMrbWindow2D("Proto", 1, &x, &y);
						}
						break;
		default:		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
						gMrbLog->Flush(this->ClassName(), "DefineWindows");
						return(kFALSE);
	}

	sts = DefineVarOrWdw(wType, proto, WdwDefs);
	delete proto;

	return(sts);
}

Bool_t TMrbConfig::DefineWindows(const Char_t * WdwType, Int_t Xlower, Int_t Xupper, const Char_t * WdwDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineWindows
// Purpose:        Defines a set 1-dim windows
// Arguments:      Char_t * WdwType     -- type: I(nteger) or F(loat)
//                 Int_t Xlower         -- lower limit
//                 Int_t Xupper         -- upper limit
//                 Char_t * WdwDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of 1-dim windows.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX wdwTypes;
	TMrbNamedX * wType;

	Bool_t sts;
	TObject * proto = NULL;

	wdwTypes.AddNamedX(kMrbWindowTypes);
	wdwTypes.SetPatternMode();
	wType = wdwTypes.FindByName(WdwType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (wType == NULL) {
		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineWindows");
		return(kFALSE);
	}

	switch (wType->GetIndex()) {
		case kWindowI:	proto = (TObject *) new TMrbWindowI("Proto", Xlower, Xupper); break;
		case kWindowF:	proto = (TObject *) new TMrbWindowF("Proto", (Float_t) Xlower, (Float_t) Xupper);
								break;
		default:		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
						gMrbLog->Flush(this->ClassName(), "DefineWindows");
						return(kFALSE);
	}

	sts = DefineVarOrWdw(wType, proto, WdwDefs);
	delete proto;

	return(sts);
}

Bool_t TMrbConfig::DefineWindows(const Char_t * WdwType, Double_t Xlower, Double_t Xupper, const Char_t * WdwDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineWindows
// Purpose:        Defines a set 1-dim windows
// Arguments:      Char_t * WdwType     -- type: I(nteger) or F(loat)
//                 Double_t Xlower      -- lower limit
//                 Double_t Xupper      -- upper limit
//                 Char_t * WdwDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of 1-dim windows.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX wdwTypes;
	TMrbNamedX * wType;

	Bool_t sts;
	TObject * proto = NULL;

	wdwTypes.AddNamedX(kMrbWindowTypes);
	wdwTypes.SetPatternMode();
	wType = wdwTypes.FindByName(WdwType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (wType == NULL) {
		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineWindows");
		return(kFALSE);
	}

	switch (wType->GetIndex()) {
		case kWindowI:	proto = (TObject *) new TMrbWindowI("Proto",
															(Int_t) (Xlower + .5),
															(Int_t) (Xupper + .5));
								break;
		case kWindowF:	proto = (TObject *) new TMrbWindowF("Proto", Xlower, Xupper); break;
		default:		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
						gMrbLog->Flush(this->ClassName(), "DefineWindows");
						return(kFALSE);
	}

	sts = DefineVarOrWdw(wType, proto, WdwDefs);
	delete proto;

	return(sts);
}

Bool_t TMrbConfig::DefineWindows(const Char_t * WdwType, Int_t Npoints, Double_t * X, Double_t * Y, const Char_t * WdwDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineWindows
// Purpose:        Defines a set 1-dim windows
// Arguments:      Char_t * WdwType     -- type: C(ut)
//                 Int_t Npoints        -- number of polygon points
//                 Double_t * X         -- coordinates X
//                 Double_t * Y         -- coordinates Y
//                 Char_t * WdwDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of 2-dim windows (cuts).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX wdwTypes;
	TMrbNamedX * wType;

	Bool_t sts;
	TObject * proto = NULL;

	wdwTypes.AddNamedX(kMrbWindowTypes);
	wdwTypes.SetPatternMode();
	wType = wdwTypes.FindByName(WdwType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (wType == NULL) {
		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
		gMrbLog->Flush(this->ClassName(), "DefineWindows");
		return(kFALSE);
	}

	switch (wType->GetIndex()) {
		case kWindow2D:	proto = (TObject *) new TMrbWindow2D("Proto", Npoints, X, Y);
								break;
		case kWindowI:
		case kWindowF:
		default:		gMrbLog->Err() << "Illegal window type - " << WdwType << endl;
						gMrbLog->Flush(this->ClassName(), "DefineWindows");
						return(kFALSE);
	}

	sts = DefineVarOrWdw(wType, proto, WdwDefs);
	delete proto;

	return(sts);
}

Bool_t TMrbConfig::DefineVarOrWdw(TMrbNamedX * VarType, TObject * VarProto, const Char_t * VarDefs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::DefineVarOrWdw
// Purpose:        Define variables, variable arrays, and windows
// Arguments:      TMrbNamedX * VarType -- type: variable or window,
//                                               array or not,
//                                               integer, float, or string
//                 TObject * VarProto   -- prototype
//                 Char_t * VarDefs     -- definition string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Real var/wdw definitions take place here.
//                 Instantiate = kTRUE for variables as they will be allocated directly.
//                 It may be kFALSE for windows, esp. for 2-dim ones.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString varName;
	TString varIndex;
	UInt_t vType;
	Int_t vIndex;
	Int_t n1, n2;

	istringstream vline(VarDefs);

	vType = VarType->GetIndex();

	for (;;) {
		vline >> varName;
		if (varName.Length() <= 0) break;
		n1 = varName.Index("[");
		if (n1 == 0) {
			gMrbLog->Err() << "Spurious array index - " << varName << endl;
			gMrbLog->Flush(this->ClassName(), "DefineVarOrWdw");
			continue;
		}
		if (n1 > 0) {
			if (vType & kIsWindow) {
				gMrbLog->Err() << "Windows may NOT be indexed - " << varName << endl;
				gMrbLog->Flush(this->ClassName(), "DefineVarOrWdw");
				continue;
			}
			varIndex = varName(n1 + 1, 1000);
			n2 = varIndex.Index("]");
			if (n2 <= 0) {
				gMrbLog->Err() << "Illegal array index - " << varName << endl;
				gMrbLog->Flush(this->ClassName(), "DefineVarOrWdw");
				continue;
			}
			varIndex = varIndex(0, n2);
			varIndex = varIndex.Strip();
			varIndex += " ";					// to keep istringstream happy: s.good() needs a blank at end of input stream ...
			istringstream s(varIndex.Data());
			s >> vIndex;
			if (!s.good()) {
				gMrbLog->Err() << "Illegal array index - " << varName << endl;
				gMrbLog->Flush(this->ClassName(), "DefineVarOrWdw");
				continue;
			}
			varName = varName(0, n1);
			varName = varName.Strip();
			switch (vType) {
				case kVarI:
					new TMrbVarArrayI(varName.Data(), vIndex, ((TMrbVarI *) VarProto)->Get());
					break;
				case kVarF:
					new TMrbVarArrayF(varName.Data(), vIndex, ((TMrbVarF *) VarProto)->Get());
					break;
			}
		} else {
			switch (vType) {
				case kVarI:
					new TMrbVarI(varName.Data(), ((TMrbVarI *) VarProto)->Get()); break;
				case kVarF:
					new TMrbVarF(varName.Data(), ((TMrbVarF *) VarProto)->Get()); break;
				case kVarS:
					new TMrbVarS(varName.Data(), ((TMrbVarS *) VarProto)->Get()); break;
				case kWindowI:
					new TMrbWindowI(varName.Data(), ((TMrbWindowI *) VarProto)->GetLowerLimit(),
												((TMrbWindowI *) VarProto)->GetUpperLimit());
					break;
				case kWindowF:
					new TMrbWindowF(varName.Data(), ((TMrbWindowF *) VarProto)->GetLowerLimit(),
												((TMrbWindowF *) VarProto)->GetUpperLimit());
					break;
				case kWindow2D:
					Double_t x = 0.;
					Double_t y = 0.;
					new TMrbWindow2D(varName.Data(), 1, &x, &y); break;
			}
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																Int_t Xbin, Double_t Xlow, Double_t Xup,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Xbin              -- bins in X
//                 Double_t Xlow           -- lower edge X
//                 Double_t Xup            -- upper edge X
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString hType;
	TArrayD argList(3);

	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	hType = HistoType;
	hType.ToUpper();
	TMrbNamedX * histoType = fLofHistoTypes.FindByName(hType.Data());
	if (histoType == NULL) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal histogram type - " << HistoType << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	if ((histoType->GetIndex() & TMrbConfig::kHistoTH1) == 0) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal arg list - not a TH1" << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	argList[0] = (Double_t) Xbin;
	argList[1] = Xlow;
	argList[2] = Xup;

	TMrbNamedArrayD * a = new TMrbNamedArrayD(histoType->GetName(), histoType->GetTitle(), 3, argList.GetArray());
	fLofUserHistograms.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, HistoTitle, a));
	if (Condition != NULL && *Condition != '\0') fLofHistoConditions.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, Condition));
	return(kTRUE);
}

Bool_t TMrbConfig::BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																Int_t Xbin, Double_t Xlow, Double_t Xup,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t *  ArrayName     -- name of histo array
//                 Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Xbin              -- bins in X
//                 Double_t Xlow           -- lower edge X
//                 Double_t Xup            -- upper edge X
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	if (this->AddHistoToArray(ArrayName, HistoName) == NULL) return(kFALSE);
	return(this->BookHistogram(HistoType, HistoName, HistoTitle, Xbin, Xlow, Xup, Condition));
}

Bool_t TMrbConfig::BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																Int_t Xbin, Double_t Xlow, Double_t Xup,
																Int_t Ybin, Double_t Ylow, Double_t Yup,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Xbin              -- bins in X
//                 Double_t Xlow           -- lower edge X
//                 Double_t Xup            -- upper edge X
//                 Int_t Ybin              -- bins in Y
//                 Double_t Ylow           -- lower edge Y
//                 Double_t Yup            -- upper edge Y
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString hType;
	TArrayD argList(6);

	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	hType = HistoType;
	hType.ToUpper();
	TMrbNamedX * histoType = fLofHistoTypes.FindByName(hType.Data());
	if (histoType == NULL) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal histogram type - " << HistoType << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	if ((histoType->GetIndex() & TMrbConfig::kHistoTH2) == 0) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal arg list - not a TH2" << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	argList[0] = (Double_t) Xbin;
	argList[1] = Xlow;
	argList[2] = Xup;
	argList[3] = (Double_t) Ybin;
	argList[4] = Ylow;
	argList[5] = Yup;

	TMrbNamedArrayD * a = new TMrbNamedArrayD(histoType->GetName(), histoType->GetTitle(), 6, argList.GetArray());
	fLofUserHistograms.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, HistoTitle, a));
	if (Condition != NULL && *Condition != '\0') fLofHistoConditions.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, Condition));
	return(kTRUE);
}

Bool_t TMrbConfig::BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																Int_t Xbin, Double_t Xlow, Double_t Xup,
																Int_t Ybin, Double_t Ylow, Double_t Yup,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t *  ArrayName     -- name of histo array
//                 Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Xbin              -- bins in X
//                 Double_t Xlow           -- lower edge X
//                 Double_t Xup            -- upper edge X
//                 Int_t Ybin              -- bins in Y
//                 Double_t Ylow           -- lower edge Y
//                 Double_t Yup            -- upper edge Y
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	if (this->AddHistoToArray(ArrayName, HistoName) == NULL) return(kFALSE);
	return(this->BookHistogram(HistoType, HistoName, HistoTitle, Xbin, Xlow, Xup, Ybin, Ylow, Yup, Condition));
}

Bool_t TMrbConfig::BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																const Char_t * Args,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Char_t * Args           -- arglist
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString hType;

	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	hType = HistoType;
	hType.ToUpper();
	TMrbNamedX * histoType = fLofHistoTypes.FindByName(hType.Data());
	if (histoType == NULL) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal histogram type - " << HistoType << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	TMrbString argStr = Args;
	TObjArray args;
	Int_t nargs = argStr.Split(args, ",");
	if ((histoType->GetIndex() & TMrbConfig::kHistoTH1) && (nargs != 3)) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal number of args (" << nargs << ") - not a TH1" << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	} else if ((histoType->GetIndex() & TMrbConfig::kHistoTH2) && (nargs != 6)) {
		gMrbLog->Err() << "[" << HistoName << "] Illegal number of args (" << nargs << ") - not a TH2" << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	TObjString * a = new TObjString(argStr.Data());
	fLofUserHistograms.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, HistoTitle, a));
	if (Condition != NULL && *Condition != '\0') fLofHistoConditions.Add(new TMrbNamedX(histoType->GetIndex(), HistoName, Condition));
	return(kTRUE);
}

Bool_t TMrbConfig::BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
																const Char_t * Args,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookHistogram
// Purpose:        Define a histogram to be booked
// Arguments:      Char_t *  ArrayName     -- name of histo array
//                 Char_t * HistoType      -- histo type (TH1X, TH2X)
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Char_t * Args           -- arglist
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookHistogram");
		return(kFALSE);
	}

	if (this->AddHistoToArray(ArrayName, HistoName) == NULL) return(kFALSE);
	return(this->BookHistogram(HistoType, HistoName, HistoTitle, Args, Condition));
}

Bool_t TMrbConfig::BookRateHistogram(const Char_t * HistoName, const Char_t * HistoTitle,
													Int_t Scale, Int_t Range, Bool_t Loop,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookRateHistogram
// Purpose:        Define a rate histogram to be booked
// Arguments:      Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Scale             -- time scale in micros
//                 Int_t Range             -- range
//                 Bool_t Loop             -- restart at end of range if kTRUE
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books rate histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TArrayD argList(5);

	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookRateHistogram");
		return(kFALSE);
	}

	TMrbNamedX * histoType = fLofHistoTypes.FindByIndex(TMrbConfig::kHistoRate);

	switch (Scale) {
		case 1:
		case 1000:
		case 1000000:	break;
		default:
			gMrbLog->Err() << "Illegal scale - " << Scale << " (should be 1=musec, 1000=msec, or 1000000=sec)" << endl;
			gMrbLog->Flush(this->ClassName(), "BookRateHistogram");
			return(kFALSE);
	}

	this->WriteTimeStamp();

	argList[0] = 1.;
	argList[1] = 0.;
	argList[2] = (Double_t) Range;
	argList[3] = (Double_t) Scale;
	argList[4] = Loop ? 1. : 0.;

	TMrbNamedArrayD * a = new TMrbNamedArrayD("TH1F", histoType->GetTitle(), 5, argList.GetArray());
	fLofUserHistograms.Add(new TMrbNamedX(kHistoRate, HistoName, HistoTitle, a));
	if (Condition != NULL && *Condition != '\0') fLofHistoConditions.Add(new TMrbNamedX(kHistoRate, HistoName, Condition));
	return(kTRUE);
}

Bool_t TMrbConfig::BookRateHistogram(const Char_t * ArrayName, const Char_t * HistoName, const Char_t * HistoTitle,
													Int_t Scale, Int_t Range, Bool_t Loop,
																const Char_t * Condition) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::BookRateHistogram
// Purpose:        Define a rate histogram to be booked
// Arguments:      Char_t *  ArrayName     -- name of histo array
//                 Char_t * HistoName      -- name
//                 Char_t * HistoTitle     -- title
//                 Int_t Scale             -- time scale in micros
//                 Int_t Range             -- range
//                 Bool_t Loop             -- restart at end of range if kTRUE
//                 Char_t * Condition      -- booking condition
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books user-defined histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	if (fLofUserHistograms.FindObject(HistoName)) {
		gMrbLog->Err() << "Histogram already booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "BookRateHistogram");
		return(kFALSE);
	}

	if (this->AddHistoToArray(ArrayName, HistoName) == NULL) return(kFALSE);
	return(this->BookRateHistogram(HistoName, HistoTitle, Scale, Range, Loop, Condition));
}

TMrbNamedX * TMrbConfig::AddHistoToArray(const Char_t * ArrayName, const Char_t * HistoName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::AddHistoToArray
// Purpose:        Add a histogram to an array
// Arguments:      Char_t * ArrayName -- name of array
//                 Char_t * HistoName -- name of histogram
// Results:        TMrbNamedX * Array -- array specs
// Exceptions:     
// Description:    Adds a histogram to given array.
//                 Creates array if not yet existing.
//                 Returns array specs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray * lofHistos;

	TString arrayName = ArrayName;
	TString listName = arrayName;
	arrayName(0,1).ToUpper();
	arrayName.Prepend("h");
	arrayName += "Array";
	listName += ".histlist";
	TMrbNamedX * hArray = (TMrbNamedX *) fLofHistoArrays.FindObject(arrayName.Data());
	if (hArray == NULL) {
		lofHistos = new TObjArray();
		hArray = new TMrbNamedX(0, arrayName.Data(), listName.Data(), lofHistos);
		fLofHistoArrays.Add(hArray);
	} else {
		lofHistos = (TObjArray *) hArray->GetAssignedObject();
	}
	lofHistos->Add(new TMrbNamedX(0, HistoName));
	return(hArray);
}

TMrbNamedX * TMrbConfig::FindHistoArray(const Char_t * HistoName, TMrbNamedX * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::FindHistoArray
// Purpose:        Find the array a histogram belongs to
// Arguments:      Char_t * HistoName -- name of histogram
//                 TMrbNamedX * After -- array to start with
// Results:        TMrbNamedX * Array -- array 
// Exceptions:     
// Description:    Checks if a histogram is assigned to an array.
//                 Returns array specs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = (After == NULL);
	TMrbNamedX * hArray;
	TIterator * arrIter = fLofHistoArrays.MakeIterator();
	while (hArray = (TMrbNamedX *) arrIter->Next()) {
		if (ok) {
			TObjArray * lofHistos = (TObjArray *) hArray->GetAssignedObject();
			TMrbNamedX * h;
			TIterator * hIter = lofHistos->MakeIterator();
			while (h = (TMrbNamedX *) hIter->Next()) {
				TString hName = h->GetName();
				if (hName.CompareTo(HistoName) == 0) return(hArray);
			}
		} else {
			ok = (hArray == After);
		}
	}
	return(NULL);
}

TMrbConfig * TMrbConfig::ReadFromFile(const Char_t * RootFile,  Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ReadFromFile
// Purpose:        Read config objects from root file
// Arguments:      Char_t * RootFile      -- name of root file
//                 Option_t * Options       -- options
// Results:        TMrbConfig * NewConfig -- points to new config read from file
// Exceptions:     returns NULL after errors
// Description:    Restores config from root file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cf;
	TFile * cfgFile;
	TList * keyList;
	TKey * key;
	TMrbConfig * newConfig;

	TString cfgClass;
	TString cfgName;

	TMrbLofUserVars * lofVars;

	TMrbLofNamedX configOption;

	fConfigOptions = configOption.CheckPatternShort(this->ClassName(), "ReadFromFile", Options, kMrbConfigOptions);
	if (fConfigOptions == TMrbLofNamedX::kIllIndexBit) return(NULL);

	if (*RootFile == '\0') {
		cf = this->GetName();
		cf(0,1).ToUpper();
		cf += "Config.root";
		RootFile = cf.Data();
	}

	cfgFile = new TFile(RootFile, "READ");
	if (!cfgFile->IsOpen()) {	
		gMrbLog->Err() << "Can't open root file " << RootFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFromFile");
		return(NULL);
	}

	keyList = cfgFile->GetListOfKeys();
	cfgName = "notFound";
	cfgClass = this->ClassName();
	TIterator * keyIter = keyList->MakeIterator();
	while (key = (TKey *) keyIter->Next()) {
		if (cfgClass.CompareTo(key->GetClassName()) == 0) {
			if (cfgName.CompareTo("notFound") == 0) {
				cfgName = key->GetName();
			} else if (cfgName.CompareTo(key->GetName()) != 0) {
				gMrbLog->Err() << RootFile << "More than 1 class " << this->ClassName() << " object" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				cfgFile->Close();
				return(NULL);
			}
		}
	}
	if (cfgName.CompareTo("notFound") == 0) {
		gMrbLog->Err() << RootFile << ": No such object - class " << this->ClassName() << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFromFile");
		cfgFile->Close();
		return(NULL);
	}

	cout << "[Reading MARaBOU config \"" << cfgName << "\" from file " << RootFile << "]" << endl;

	newConfig = (TMrbConfig *) cfgFile->Get(cfgName);
	gMrbConfig = newConfig;

	lofVars = (TMrbLofUserVars *) cfgFile->Get("SystemVars");
	if (lofVars) gMrbLofUserVars = lofVars;

	cfgFile->Close();

	delete this;

	return(newConfig);
}

Bool_t TMrbConfig::WriteToFile(const Char_t * ConfigFile,  Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::WriteToFile
// Purpose:        Write config objects to root file
// Arguments:      Char_t * ConfigFile    -- name of config file
//                 Option_t * Options       -- options
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Saves any objects defined during the configuration pass.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cf;
	TFile * cfgFile;
	TList * objList;
	TObject * obj;
	Int_t nofObjects;

	TMrbLofNamedX configOptions;

	fConfigOptions = configOptions.CheckPatternShort(this->ClassName(), "WriteToFile", Options, kMrbConfigOptions);
	if (fConfigOptions == TMrbLofNamedX::kIllIndexBit) return(kFALSE);

	objList = (TList *) gDirectory->GetList();
	nofObjects = objList->GetSize();
	if (nofObjects == 0) {
		gMrbLog->Err() << "No config objects found - file " << ConfigFile << " not written" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteToFile");
		return(kFALSE);
	}

	Bool_t verboseMode = (this->IsVerbose() || (this->GetConfigOptions() & kCfgOptVerbose) != 0);

	if (*ConfigFile == '\0') {
		cf = this->GetName();
		cf(0,1).ToUpper();
		cf += "Config.root";
		ConfigFile = cf.Data();
	}

	if (GetConfigOptions() & kCfgOptOverwrite) {
		cfgFile = new TFile(ConfigFile, "RECREATE");
	} else {
		cfgFile = new TFile(ConfigFile, "NEW");
	}
	if (!cfgFile->IsOpen()) {	
		gMrbLog->Err() << "Can't write config file " << ConfigFile << endl;
		gMrbLog->Flush(this->ClassName(), "WriteToFile");
		return(kFALSE);
	}

	if (verboseMode) {
		TIterator * objIter = objList->MakeIterator();
		while (obj = objIter->Next()) {
			gMrbLog->Out()	<< "Writing " << obj->ClassName() << " " << obj->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteToFile");
		}
	}

	objList->Write();
	cfgFile->Close();
	gMrbLog->Out() << "[" << ConfigFile << ": Configuration written - " << nofObjects << " object(s)]" << endl;
	gMrbLog->Flush("", "", setblue);
	return(kTRUE);
}

void TMrbConfig::SetGlobalAddress() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::SetGlobalAddress
// Purpose:        Set global address gMrbConfig
// Arguments:      
// Results:        
// Exceptions:
// Description:    Sets global variable gMrbConfig to `this'.
//                 May be used when reading a config from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMrbConfig = this;
};

const Char_t * TMrbConfig::GetAuthor() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetAuthor
// Purpose:        Get author's name from ldap database
// Arguments: 
// Results:        
// Exceptions:
// Description:    Reads the author's name via python-ldap.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fUser = gSystem->Getenv("USER");
	fAuthor = fUser;
	TString marabouAtHome = "$MARABOU/data/marabouAtHome";
	gSystem->ExpandPathName(marabouAtHome);
	Bool_t atHome = !gSystem->AccessPathName(marabouAtHome.Data());
	if (atHome) {
		TString cmd = Form("%s/scripts/getFromLDAP.py %s %s 2>/dev/null", gSystem->Getenv("MARABOU"), fUser.Data(), "cn");
		FILE * ldap = gSystem->OpenPipe(cmd.Data(), "r");
		if (ldap) {
			Char_t result[512];
			fread(result, 1, 512, ldap);
			fclose(ldap);
			fAuthor = (result[0] == '\0') ? fUser : result;
			Int_t x = fAuthor.Index("\n", 0);
			if (x > 0) fAuthor(x) = '\0';
		}
	}
	return(fAuthor.Data());
}

const Char_t * TMrbConfig::GetMailAddr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetMailAddr
// Purpose:        Get author's mail address from ldap database
// Arguments:      
// Results:        
// Exceptions:
// Description:    Reads the author's name via python-ldap.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fUser = gSystem->Getenv("USER");
	fMailAddr = "";
	TString cmd = Form("%s/scripts/getFromLDAP.py %s %s 2>/dev/null", gSystem->Getenv("MARABOU"), fUser.Data(), "mail");
	FILE * ldap = gSystem->OpenPipe(cmd.Data(), "r");
	if (ldap) {
		Char_t result[512];
		fread(result, 1, 512, ldap);
		fclose(ldap);
		fMailAddr = result;
		Int_t x = fMailAddr.Index("\n", 0);
		if (x > 0) fMailAddr(x) = '\0';
	}
	return(fMailAddr.Data());
}

Bool_t TMrbConfig::NameNotLegal(const Char_t * ObjType, const Char_t * ObjName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::NameNotLegal
// Purpose:        Check if name is legal within MARaBOU
// Arguments:      Char_t * ObjType     -- type of object to be checked
//                 Char_t * ObjName     -- name to be checked
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if name is pure ascii.
//                 The only special char allowed is "_" (underscore)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * cp;
	Char_t c;
		
	TString nm = ObjName;
	cp = nm.Data();
	for (Int_t i = 0; i < nm.Length(); i++) {
		c = *cp++;
		if (c >= 'A' && c <= 'Z') continue;
		if (c >= 'a' && c <= 'z') continue;
		if (c >= '0' && c <= '9') continue;
		if (c == '_') continue;
		gMrbLog->Err() << "Illegal char \"" << c << "\" in " << ObjType << " name - " << ObjName << endl;
		gMrbLog->Flush(this->ClassName(), "NameNotLegal");
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbConfig::CompileAnalyzeCode(Bool_t CleanFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CompileAnalyzeCode
// Purpose:        Compile code generated by MakeAnalyzeCode() method
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Compiles analysis code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString compileCmd = this->GetName();
	compileCmd(0,1).ToUpper();
	compileCmd.Prepend("make -f ");
	compileCmd += "Analyze.mk";
	if (CleanFlag) compileCmd += " clean all";
	gMrbLog->Out() << "[M_analyze: Compiling analyze code from source]" << endl;
	gMrbLog->Flush("", "", setblue);
	gSystem->Exec(compileCmd.Data());
	return(kTRUE);
}

Bool_t TMrbConfig::CompileReadoutCode(const Char_t * Host, Bool_t CleanFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CompileReadoutCode
// Purpose:        Compile code generated by MakeReadoutCode() method
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Compiles readout code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(kTRUE);
}

Bool_t TMrbConfig::UpdateMbsSetup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::UpdateMbsSetup
// Purpose:        Update .mbssetup data base
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates some entries in .mbssetup
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMbsSetup * mbsSetup;
	Int_t nofReadouts = 1;

	gMrbLog->Out() << "[.mbssetup: Definitions to perform MBS setup]" << endl;
	gMrbLog->Flush("", "", setblue);

	if (!gSystem->AccessPathName(".mbssetup")) {
		mbsSetup = new TMbsSetup(".mbssetup");
	} else {
		mbsSetup = new TMbsSetup();
	}

	if (!gSystem->AccessPathName(".mbssetup-localdefs")) {
		mbsSetup->GetEnv()->ReadFile(".mbssetup-localdefs", kEnvChange);
		gMrbLog->Out() << "[.mbssetup: Merging local defs from file \".mbssetup-localdefs\"]" << endl;
		gMrbLog->Flush("", "", setblue);
	}

	TString mbsVersion = this->GetMbsVersion(kTRUE, kTRUE);
	if (mbsVersion.IsNull()) return(kFALSE);
	TString lynxVersion = this->GetLynxVersion(kTRUE);
	if (lynxVersion.IsNull()) return(kFALSE);
	TString procType = this->GetProcType(kTRUE);
	if (procType.IsNull()) return(kFALSE);

	mbsSetup->Set("MbsVersion", mbsVersion.Data());
	mbsSetup->Set("LynxVersion", lynxVersion.Data());

	TString trigMode = gEnv->GetValue("TMbsSetup.TriggerMode", "");
	if (!trigMode.IsNull()) {
		mbsSetup->ReadoutProc(0)->TriggerModule()->SetTriggerMode(trigMode.Data());
		gMrbLog->Out() << "Setting MBS trigger mode to \"" << trigMode << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateMbsSetup", setblue);
	}

	UInt_t cratePattern = this->GetCratePattern();
	TArrayI c(5);
	c.Reset(-1);
	Int_t n = 0;
	EMrbControllerType ctrlType = kControllerUnused;
	Bool_t useCamac = kFALSE;
	for (Int_t bit = 0; bit < 5; bit++) {
		if (cratePattern & (1 << bit)) {
			c[n] = bit;
			if (bit > 0) useCamac = kTRUE;
			if (useCamac && (ctrlType == kControllerUnused)) ctrlType = this->GetControllerType(bit);
			n++;
		}
	}
	mbsSetup->SetNofReadouts(nofReadouts);
	if (ctrlType != kControllerUnused) mbsSetup->ReadoutProc(0)->SetController((EMbsControllerType) ctrlType);
	mbsSetup->ReadoutProc(0)->SetCratesToBeRead(c[0], c[1], c[2], c[3], c[4]);

	if (fSevtSize > 0) {
		TMrbEvent * evt = (TMrbEvent *) fLofEvents.First();
		TIterator * evtIter = fLofEvents.MakeIterator();
		while (evt = (TMrbEvent *) evtIter->Next()) mbsSetup->ReadoutProc(0)->SetSevtSize(evt->GetTrigger(), fSevtSize);
	}
		
	mbsSetup->Save();
	delete mbsSetup;
	return(kTRUE);
}

void TMrbConfig::MakeGlobal(const Char_t * Name, Int_t IntVal, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeGlobal
// Purpose:        Define a global var (const)
// Arguments:      Char_t * Name     -- variable name
//                 Int_t IntVal      -- value
// Results:        --
// Exceptions:
// Description:    Defines an integer const.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t * v = new Int_t;
	*v = IntVal;
	fLofGlobals.AddNamedX(new TMrbNamedX(TMrbConfig::kGlobInt, Name, Comment, (TObject *) v));
}

void TMrbConfig::MakeGlobal(const Char_t * Name, Float_t FloatVal, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeGlobal
// Purpose:        Define a global var (const)
// Arguments:      Char_t * Name     -- variable name
//                 Float_t FloatVal  -- value
// Results:        --
// Exceptions:
// Description:    Defines a float const.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Float_t * v = new Float_t;
	*v = FloatVal;
	fLofGlobals.AddNamedX(new TMrbNamedX(TMrbConfig::kGlobFloat, Name, Comment, (TObject *) v));
}

void TMrbConfig::MakeGlobal(const Char_t * Name, Double_t DblVal, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeGlobal
// Purpose:        Define a global var (const)
// Arguments:      Char_t * Name      -- variable name
//                 Double_t DblVal    -- value
// Results:        --
// Exceptions:
// Description:    Defines a double const.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t * v = new Double_t;
	*v = DblVal;
	fLofGlobals.AddNamedX(new TMrbNamedX(TMrbConfig::kGlobDouble, Name, Comment, (TObject *) v));
}

void TMrbConfig::MakeGlobal(const Char_t * Name, Bool_t BoolVal, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeGlobal
// Purpose:        Define a global var (const)
// Arguments:      Char_t * Name      -- variable name
//                 Bool_t BoolVal     -- value
// Results:        --
// Exceptions:
// Description:    Defines a boolean const.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t * v = new Bool_t;
	*v = BoolVal;
	fLofGlobals.AddNamedX(new TMrbNamedX(TMrbConfig::kGlobBool, Name, Comment, (TObject *) v));
}

void TMrbConfig::MakeGlobal(const Char_t * Name, const Char_t * Str, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeGlobal
// Purpose:        Define a global var (const)
// Arguments:      Char_t * Name      -- variable name
//                 Char_t * StrVal    -- value
// Results:        --
// Exceptions:
// Description:    Defines a string const.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString * v = new TString(Str);
	fLofGlobals.AddNamedX(new TMrbNamedX(TMrbConfig::kGlobString, Name, Comment, (TObject *) v));
}

Bool_t TMrbConfig::GetGlobal(const Char_t * Name, Int_t & IntVar) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobal
// Purpose:        Get value of global var (Int_t)
// Arguments:      Char_t * Name     -- variable name
//                 Int_t & IntVar    -- where to write int value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobInt) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a INTEGER" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	IntVar = *((Int_t *) nx->GetAssignedObject());
	return(kTRUE);
};

Int_t TMrbConfig::GetGlobI(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobI
// Purpose:        Get value of global var (Int_t)
// Arguments:      Char_t * Name   -- variable name
// Results:        Int_t IntVal    -- value
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobI");
		return(0);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobInt) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a INTEGER" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobI");
		return(0);
	}
	return(*((Int_t *) nx->GetAssignedObject()));
};

Bool_t TMrbConfig::GetGlobal(const Char_t * Name, Float_t & FloatVar) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobal
// Purpose:        Get value of global var (Float_t)
// Arguments:      Char_t * Name       -- variable name
//                 Float_t & FloatVar  -- where to write float value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobFloat) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a FLOAT" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	FloatVar = *((Int_t *) nx->GetAssignedObject());
	return(kTRUE);
};

Float_t TMrbConfig::GetGlobF(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobF
// Purpose:        Get value of global var (Float_t)
// Arguments:      Char_t * Name      -- variable name
// Results:        Float_t FloatVal   -- value
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobF");
		return(0.);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobFloat) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a FLOAT" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobF");
		return(0.);
	}
	return(*((Float_t *) nx->GetAssignedObject()));
};

Bool_t TMrbConfig::GetGlobal(const Char_t * Name, Double_t & DblVar) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobal
// Purpose:        Get value of global var (Double_t)
// Arguments:      Char_t * Name        -- variable name
//                 Double_t & DblVar    -- where to write double value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobDouble) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a DOUBLE" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	DblVar = *((Double_t *) nx->GetAssignedObject());
	return(kTRUE);
};

Double_t TMrbConfig::GetGlobD(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobD
// Purpose:        Get value of global var (Double_t)
// Arguments:      Char_t * Name    -- variable name
// Results:        Double_t DblVal  -- value
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobD");
		return(0.);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobDouble) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a DOUBLE" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobD");
		return(0.);
	}
	return(*((Double_t *) nx->GetAssignedObject()));
};

Bool_t TMrbConfig::GetGlobal(const Char_t * Name, Bool_t & BoolVar) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobal
// Purpose:        Get value of global var (Bool_t)
// Arguments:      Char_t * Name      -- variable name
//                 Bool_t & BoolVar   -- where to write boolean value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobBool) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a BOOLEAN" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	BoolVar = *((Int_t *) nx->GetAssignedObject());
	return(kTRUE);
};

Bool_t TMrbConfig::GetGlobB(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobB
// Purpose:        Get value of global var (Bool_t)
// Arguments:      Char_t * Name      -- variable name
// Results:        Bool_t BoolVal     -- value
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobB");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobBool) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a BOOLEAN" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobB");
		return(kFALSE);
	}
	return(*((Bool_t *) nx->GetAssignedObject()));
};

Bool_t TMrbConfig::GetGlobal(const Char_t * Name, TString & Str) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobal
// Purpose:        Get value of global var (Char_t *)
// Arguments:      Char_t * Name     -- variable name
//                 TString & Str     -- where to write string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobString) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a STRING" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobal");
		return(kFALSE);
	}
	TString * str = (TString *) nx->GetAssignedObject();
	Str = str->Data();
	return(kTRUE);
};

const Char_t * TMrbConfig::GetGlobStr(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetGlobStr
// Purpose:        Get value of global var (Char_t *)
// Arguments:      Char_t * Name     -- variable name
// Results:        Char_t * Value    -- value
// Exceptions:
// Description:    Gets value of a global variable 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGlobals.FindByName(Name);
	if (nx == NULL) {
		gMrbLog->Err() << "No such global var - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobStr");
		return(NULL);
	}
	if (nx->GetIndex() != TMrbConfig::kGlobString) {
		gMrbLog->Err() << "Illegal data type - " << Name
						<< " is not a STRING" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGlobStr");
		return(NULL);
	}
	TString * str = (TString *) nx->GetAssignedObject();
	return(str->Data());
};

void TMrbConfig::MakeDefined(const Char_t * Name, Int_t Value, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeDefined
// Purpose:        Provide a #define
// Arguments:      Char_t * Name     -- variable name
//                 Int_t Value       -- value
//                 Char_t * Comment  -- comment
// Results:        --
// Exceptions:
// Description:    Issues a #define statement.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString dName = Name;
	if (!dName.BeginsWith("__")) dName.Prepend("__");
	if (!dName.EndsWith("__")) dName += "__";
	TString dComment = Comment;
	dComment = dComment.Strip(TString::kBoth);
	if (dComment.Length() == 0) {
		dComment = "Int_t";
		if (this->IsVerbose()) {
			gMrbLog->Out()  << "Setting cpp #define flag " << dName << " to " << Value << endl;
			gMrbLog->Flush(this->ClassName(), "MakeDefined");
		}
	} else {
		gMrbLog->Out()  << "[" << dName << "] " << dComment.Data() << " = "<< Value << endl;
		gMrbLog->Flush(this->ClassName(), "MakeDefined", setgreen);
		dComment.Prepend("Int_t|");
	}
	fLofDefines.AddNamedX(new TMrbNamedX(Value, dName.Data(), dComment.Data()));
}

void TMrbConfig::MakeDefined(const Char_t * Name, Bool_t Defined, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeDefined
// Purpose:        Provide a #define
// Arguments:      Char_t * Name     -- variable name
//                 Bool_t Defined    -- kTRUE/kFALSE
//                 Char_t * Comment  -- comment
// Results:        --
// Exceptions:
// Description:    Issues a #define statement.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString dName = Name;
	if (!dName.BeginsWith("__")) dName.Prepend("__");
	if (!dName.EndsWith("__")) dName += "__";
	TString dComment = Comment;
	dComment = dComment.Strip(TString::kBoth);
	if (dComment.Length() == 0) {
		dComment = "Bool_t";
		if (this->IsVerbose()) {
			gMrbLog->Out()  << "Setting cpp #define flag " << dName << " to " << (Defined ? "TRUE" : "FALSE") << endl;
			gMrbLog->Flush(this->ClassName(), "MakeDefined");
		}
	} else {
		TString trueStr = setblue;
		trueStr += "TRUE";
		trueStr += setblack;
		TString falseStr = setred;
		falseStr += "FALSE";
		falseStr += setblack;
		gMrbLog->Out()  << "[" << dName << "] " << dComment.Data() << " : "<< (Defined ? trueStr : falseStr) << endl;
		gMrbLog->Flush(this->ClassName(), "MakeDefined", setgreen);
		dComment.Prepend("Bool_t|");
	}
	fLofDefines.AddNamedX(new TMrbNamedX((Int_t) Defined, dName.Data(), dComment.Data()));
}

Bool_t TMrbConfig::IsDefined(const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IsDefined
// Purpose:        Check if #defined
// Arguments:      Char_t * Name   -- name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if there is a #define.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofDefines.FindByName(Name);
	if (nx == NULL || nx->GetIndex() == 0)	return(kFALSE);
	else									return(kTRUE);
	return(kFALSE);
}

Bool_t TMrbConfig::IsDefined(const Char_t * Name, Int_t & Value) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IsDefined
// Purpose:        Check if #defined
// Arguments:      Char_t * Name   -- name
//                 Int_t & Value   -- return value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if there is a #define. Returns value if any.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofDefines.FindByName(Name);
	Value = (Int_t) kFALSE;
	if (nx == NULL) return(kFALSE);
	Value = nx->GetIndex();
	TString name = nx->GetName();
	if (name.BeginsWith("Int_t")) return(kTRUE);
	if (name.BeginsWith("Bool_t")) return(nx->GetIndex() != 0);
	return(kFALSE);
}

Int_t TMrbConfig::GetNofModules(const Char_t * Pattern) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetNofModules
// Purpose:        Get number of modules within given pattern
// Arguments:      Char_t * Pattern   -- wild card pattern
// Results:        Int NofModules     -- number of modules found
// Exceptions:
// Description:    Returns number of modules within given wild card
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TRegexp wild(Pattern, kTRUE);
	Int_t nofModules = 0;
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		TString mName = module->GetName();
		if (mName.Index(wild, 0) >= 0) nofModules++;
	}
	return(nofModules);
}

Bool_t TMrbConfig::CheckConfig() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CheckConfig
// Purpose:        Check if config consistent
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if config defs are consistent.
//                 Protected method called automatically.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fConfigChecked) return(fConfigOk);		// all done

	Int_t nofErrors = 0;

	nofErrors += this->CheckMbsBranchSettings();

	TString mbsVersion = this->GetMbsVersion(kTRUE, kTRUE);
	if (mbsVersion.IsNull()) return(kFALSE);
	TString lynxVersion = this->GetLynxVersion(kTRUE);
	if (lynxVersion.IsNull()) return(kFALSE);
	TString procType = this->GetProcType(kTRUE);
	if (procType.IsNull()) return(kFALSE);

	TString lv = "";
	if (fMbsVersion.CompareTo("2.2") == 0) {
		lv = "2.5";
	} else if (fMbsVersion.CompareTo("4.2") == 0) {
		lv = "3.1";
	} else if (fMbsVersion.CompareTo("4.3") == 0) {
		lv = "3.1";
	} else if (fMbsVersion.CompareTo("4.5") == 0) {
		lv = "3.1";
	} else {
		gMrbLog->Err() << "Wrong MBS version - " << fMbsVersion << "; set TMbsSetup.MbsVersion in .rootrc properly" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckConfig");
		nofErrors++;
	}

	if (fLynxVersion.CompareTo("2.5") != 0 && fLynxVersion.CompareTo("3.1") != 0) {
		gMrbLog->Err() << "Wrong Lynx version - " << fLynxVersion << endl;
		gMrbLog->Flush(this->ClassName(), "CheckConfig");
		fLynxVersion = "";
		nofErrors++;
	}

	if (this->GetNofEvents() == 0) {
		gMrbLog->Err()	<< "No events/triggers defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckConfig");
		nofErrors++;
	} else {
		if (this->IsVerbose()) {
			gMrbLog->Out()  << "Number of events/triggers:    " << fNofEvents << endl;
			gMrbLog->Flush(this->ClassName(), "CheckConfig");
		}
		TMrbEvent * evt;
		TIterator * evtIter = fLofEvents.MakeIterator();
		while (evt = (TMrbEvent *) evtIter->Next()) {
			if (evt->GetNofSubevents() == 0) {
				gMrbLog->Err()	<< "Event \"" << evt->GetName()
								<< "\" (trigger " << evt->GetTrigger()
								<< "): No subevents defined" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckConfig");
				nofErrors++;
			} else {
				if (this->IsVerbose()) {
					gMrbLog->Out()	<< "Event \"" << evt->GetName()
									<< "\" (trigger " << evt->GetTrigger()
									<< ") has " << evt->GetNofSubevents() << " subevent(s)" << endl;
					gMrbLog->Flush(this->ClassName(), "CheckConfig");
				}
				TMrbSubevent * sevt;
				TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
				while (sevt = (TMrbSubevent *) sevtIter->Next()) {
					if (sevt->NeedsModulesToBeAssigned()) {
						if (sevt->GetNofModules() == 0) {
							gMrbLog->Err()	<< "[Event " << evt->GetName() << "] Subevent \"" << sevt->GetName()
											<< "\" (serial " << sevt->GetSerial()
											<< "): No modules assigned" << endl;
							gMrbLog->Flush(this->ClassName(), "CheckConfig");
							nofErrors++;
						} else if (this->IsVerbose()) {
							gMrbLog->Out()	<< "[Event " << evt->GetName() << "] Subevent \"" << sevt->GetName()
											<< "\" (serial " << sevt->GetSerial()
											<< ") has " << sevt->GetNofModules() << " module(s) assigned" << endl;
							gMrbLog->Flush(this->ClassName(), "CheckConfig");
						}
						if (!sevt->HistosToBeAllocated()) {
							gMrbLog->Wrn()	<< "[Event " << evt->GetName() << "] Subevent \"" << sevt->GetName()
											<< "\" (serial " << sevt->GetSerial()
											<< ") has " << sevt->GetNofModules()
											<< " module(s) assigned - but no histogram(s) allocated" << endl;
							gMrbLog->Flush(this->ClassName(), "CheckConfig");
						}
					}
					if (sevt->GetNofParams() == 0) {
						gMrbLog->Err()	<< "[Event " << evt->GetName() << "] Subevent \"" << sevt->GetName()
										<< "\" (serial " << sevt->GetSerial()
										<< "): No params assigned" << endl;
						gMrbLog->Flush(this->ClassName(), "CheckConfig");
						nofErrors++;
					} else if (this->IsVerbose()) {
						gMrbLog->Out()	<< "[Event " << evt->GetName() << "] Subevent \"" << sevt->GetName()
										<< "\" (serial " << sevt->GetSerial()
										<< ") has " << sevt->GetNofParams() << " param(s) assigned" << endl;
						gMrbLog->Flush(this->ClassName(), "CheckConfig");
					}
				}
			}
		}
	}

	TMrbSubevent *sevt;
	TIterator * sIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sIter->Next()) {
		if (sevt->GetNofEvents() == 0) {
			gMrbLog->Err()	<< "Subevent \"" << sevt->GetName()
							<< "\" (serial " << sevt->GetSerial()
							<< "): Not used by any event" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckConfig");
			nofErrors++;
		}
	}

	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		Int_t mtype = (module->GetType())->GetIndex();
		if ((mtype & TMrbConfig::kModuleListMode)
			&& !(mtype & TMrbConfig::kModuleScaler)
			&& !(mtype & TMrbConfig::kModuleRaw)) {
			if (module->GetNofChannelsUsed() == 0) {
				cout << "@@@ " << mtype << " " << (module->GetType())->GetName() << endl;
				gMrbLog->Err()	<< "Module \"" << module->GetName()
								<< "\" (serial " << module->GetSerial()
								<< "): Not used by any subevent" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckConfig");
				nofErrors++;
			} else if (this->IsVerbose()) {
				gMrbLog->Out()	<< "Module \"" << module->GetName()
								<< "\" (serial " << module->GetSerial()
								<< "): " << module->GetNofChannelsUsed()
								<< " channel(s) used out of " << module->GetNofChannels() << endl;
				gMrbLog->Flush(this->ClassName(), "CheckConfig");
			}
		}
		if (!this->CheckModuleAddress(module, kFALSE)) nofErrors++;
	}

	TString camacContr = gEnv->GetValue("TMbsSetup.CamacController", "");
	if (camacContr.IsNull()) camacContr = gEnv->GetValue("TMrbEsone.Controller", "");

	UInt_t camacPattern = this->GetCratePattern(kCrateCamac);
	if (camacPattern != 0) {
		Int_t crate = 0;
		while (camacPattern) {
			if (camacPattern & 1) {
				if (crate >= 1) {
					EMrbControllerType contrType = this->GetControllerType(crate);
					if (contrType == TMrbConfig::kControllerUnused) {
						TString cc = gEnv->GetValue(Form("TMbsSetup.CamacController.C%d", crate), "");
						if (cc.IsNull()) {
							if (!camacContr.IsNull()) this->SetControllerType(Form("C%d", crate), camacContr.Data());
						} else {
							this->SetControllerType(Form("C%d", crate), cc.Data());
						}
					}
					contrType = this->GetControllerType(crate);
					if (contrType == TMrbConfig::kControllerUnused) {
						gMrbLog->Err() << "Type of CAMAC controller missing for crate C" << crate << endl;
						gMrbLog->Flush(this->ClassName(), "CheckConfig");
						nofErrors++;
					}
				}
			}
			crate++;
			camacPattern >>= 1;
		}
	}

	if (nofErrors == 0) {
		gMrbLog->Out()	<< "Check done - no inconsistencies encountered" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckConfig", setblue);
	} else {
		gMrbLog->Err()	<< "Check done: " << nofErrors << " inconsistencies encountered - config step aborted" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckConfig");
	}

	fConfigChecked = kTRUE;
	fConfigOk = (nofErrors == 0);
	return(fConfigOk);
}

Bool_t TMrbConfig::SetMbsBranch(TMrbNamedX & MbsBranch, Int_t MbsBranchNo, const Char_t * MbsBranchName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::SetMbsBranch
// Purpose:        Assign mbs branch to events, subevents, and modules
// Arguments:      TMrbNamedX & MbsBranch       -- mbs branch to be set
//                 Int_t MbsBranchNo            -- corresponding branch number
//                 Char_t * MbsBranchName       -- branch name to be used
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets branch data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;

	MbsBranch.Set(-1, "none");

	TString branchName;
	if (MbsBranchName == NULL || *MbsBranchName == '\0') {
		branchName = "branch";
		branchName += MbsBranchNo;
	} else {
		branchName = MbsBranchName;
	}

	TMrbNamedX * br = fLofMbsBranches.FindByIndex(MbsBranchNo);
	if (br) {
		MbsBranch.Set(br);
	} else {
		if (MbsBranchNo < 0) {
			gMrbLog->Err()	<< "Illegal mbs branch number - " << MbsBranchNo << endl;
			gMrbLog->Flush(this->ClassName(), "SetMbsBranch");
			ok = kFALSE;
		}
		MbsBranch.Set(MbsBranchNo, branchName.Data());
		fLofMbsBranches.AddNamedX(MbsBranchNo, branchName.Data());
	}

	return(ok);
}

Int_t TMrbConfig::CheckMbsBranchSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CheckMbsBranchSettings
// Purpose:        Check if branch settings are consistent
// Arguments:      --
// Results:        Int_t NofErrors       -- number of errors encountered
// Exceptions:
// Description:    Checks mbs branch settings:
//                 if all modules belonging to a subevent have same branch
//                 if all subevents of an event have same branch
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofErrors = 0;

	TMrbEvent * evt;
	TMrbSubevent * sevt;
	TMrbModule * module;

	TIterator * evtIter;
	TIterator * sevtIter;
	TIterator * modIter;

	if (fLofMbsBranches.GetEntriesFast() == 0) return(0);		// no branch settigs at all

// check branch settings for events
	evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) {
		if (evt->GetMbsBranchNo() != -1) {	// if event is assigned to a branch all subevents have to have same branch number
			sevtIter = evt->GetLofSubevents()->MakeIterator();	// step down thru subevents
			while (sevt = (TMrbSubevent *) sevtIter->Next()) {
				Int_t branchNo = sevt->GetMbsBranchNo();
				if (branchNo == -1) { 							// branch not yet set - pass event branch to this subevent
					sevt->SetMbsBranch(evt->GetMbsBranch());
				} else if (branchNo != evt->GetMbsBranchNo()) {	// branches for subevent and event differ
					gMrbLog->Err()	<< "Differing branches - subevent " << sevt->GetName() << ": "
									<< sevt->GetMbsBranch()->GetName() << "(" << sevt->GetMbsBranch()->GetIndex() << ")"
									<< " <--> event " << evt->GetName() << ": "
									<< evt->GetMbsBranch()->GetName() << "(" << evt->GetMbsBranch()->GetIndex() << ")"
									<< endl;
					gMrbLog->Flush(this->ClassName(), "CheckMbsBranchSettings");
					nofErrors++;
				}
			}
		}
	}

// check branch settings for subevents
	sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if (sevt->GetMbsBranchNo() == -1) {			// if there are any branch settings (entries > 0) each subevent has to be assigned to a branch
			gMrbLog->Err()	<< "Subevent not assigned to any mbs branch - " << sevt->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "CheckMbsBranchSettings");
			nofErrors++;
		} else {
			TIterator * modIter = sevt->GetLofModules()->MakeIterator();		// step down thru modules
			while (module = (TMrbModule *) modIter->Next()) {
				Int_t branchNo = module->GetMbsBranchNo();
				if (branchNo == -1) { 							// branch not yet set - pass event branch to this subevent
					module->SetMbsBranch(sevt->GetMbsBranch());
				} else if (branchNo != sevt->GetMbsBranchNo()) {	// branches for subevent and event differ
					gMrbLog->Err()	<< "Differing branches - module " << module->GetName() << ": "
									<< module->GetMbsBranch()->GetName() << "(" << module->GetMbsBranch()->GetIndex() << ")"
									<< " <--> subevent " << sevt->GetName() << ": "
									<< sevt->GetMbsBranch()->GetName() << "(" << sevt->GetMbsBranch()->GetIndex() << ")"
									<< endl;
					gMrbLog->Flush(this->ClassName(), "CheckMbsBranchSettings");
					nofErrors++;
				}
			}
		}
	}

// check branch settings for modules
	modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->GetMbsBranchNo() == -1) {			// if there are any branch settings (entries > 0) each subevent has to be assigned to a branch
			gMrbLog->Err()	<< "Module not assigned to any mbs branch - " << module->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "CheckMbsBranchSettings");
			nofErrors++;
		}
	}

	return(nofErrors);
}

Bool_t TMrbConfig::CreateHistoArrays() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::CreateHistoArrays
// Purpose:        Create histogram arrays if needed
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates histo arrays as well .histlist files
//                 for modules, subevents, and events.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) {
		if (evt->HistoArrayToBeCreated()) {
			TMrbSubevent * sevt;
			TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
			while (sevt = (TMrbSubevent *) sevtIter->Next()) {
				if (sevt->HistosToBeAllocated()) {
					TMrbModuleChannel * param;
					TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
					while (param = (TMrbModuleChannel *) paramIter->Next()) {
						TString pName = param->GetName();
						pName(0,1).ToUpper();
						pName.Prepend("h");
						this->AddHistoToArray(evt->GetHistoArrayName(), pName.Data());
					}					
				}
			}
		}
	}

	TMrbSubevent * sevt;
	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if (sevt->HistosToBeAllocated() && sevt->HistoArrayToBeCreated()) {
			TMrbModuleChannel * param;
			TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
			while (param = (TMrbModuleChannel *) paramIter->Next()) {
				TString pName = param->GetName();
				pName(0,1).ToUpper();
				pName.Prepend("h");
				this->AddHistoToArray(sevt->GetHistoArrayName(), pName.Data());
			}					
		}
	}

	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->HistoArrayToBeCreated()) {
			TMrbModuleChannel * param;
			TIterator * paramIter = module->GetLofChannels()->MakeIterator();
			while (param = (TMrbModuleChannel *) paramIter->Next()) {
				TString pName = param->GetName();
				pName(0,1).ToUpper();
				pName.Prepend("h");
				this->AddHistoToArray(module->GetHistoArrayName(), pName.Data());
			}					
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::WriteMuxConfig(const Char_t * CfgFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::WriteMuxConfig
// Purpose:        Write multiplexer configuration to file
// Arguments:      Char_t * CfgFile   -- filename
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes mux config data using TEnv.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNofMuxs == 0) {
		gMrbLog->Err()	<< "No MUX defined" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteMuxConfig");
		return(kFALSE);
	}

	TEnv * mux = new TEnv(CfgFile);
	mux->SetValue("TMrbConfig.Mux.NofMultiplexers", fNofMuxs);

	Int_t minMux = 1000;
	Int_t maxMux = 0;
	TIterator * mIter = fLofMuxs.MakeIterator();
	TMrbMesytec_Mux16 * m;
	TString mstr = "";
	while (m = (TMrbMesytec_Mux16 *) mIter->Next()) {
		if (!mstr.IsNull()) mstr += ":";
		Int_t serial = m->GetModuleSerial();
		if (serial > maxMux) maxMux = serial;
		if (serial < minMux) minMux = serial;
		mstr += m->GetName();
	}
	mux->SetValue("TMrbConfig.Mux.LofMultiplexers", mstr.Data());
	mux->SetValue("TMrbConfig.Mux.Smin", minMux);
	mux->SetValue("TMrbConfig.Mux.Smax", maxMux);

	mIter = fLofMuxs.MakeIterator();
	Int_t nmux = 0;
	Int_t moduleChans = 0;
	Int_t moduleRange = 0;
	while (m = (TMrbMesytec_Mux16 *) mIter->Next()) {
		if (moduleChans == 0) moduleChans = m->GetModule()->GetNofChannels();
		if (moduleChans != m->GetModule()->GetNofChannels()) {
			gMrbLog->Err()	<< m->GetName() << "/" << m->GetModule()->GetName()
							<< ": Number of channels changed - " << m->GetModule()->GetNofChannels()
							<< " (should be " << moduleChans << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteMuxConfig");
		}
		if (moduleRange == 0) moduleRange = m->GetModule()->GetRange();
		if (moduleRange != m->GetModule()->GetRange()) {
			gMrbLog->Err()	<< m->GetName() << "/" << m->GetModule()->GetName()
							<< ": Modole range changed - " << m->GetModule()->GetRange()
							<< " (should be " << moduleRange << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteMuxConfig");
		}
		mux->SetValue(Form("TMrbConfig.Mux.%d.Name", nmux), m->GetName());
		mux->SetValue(Form("TMrbConfig.Mux.%d.Module", nmux), m->GetModule()->GetName());
		mux->SetValue(Form("TMrbConfig.Mux.%d.Serial", nmux), m->GetSerial());
		mux->SetValue(Form("TMrbConfig.Mux.%d.AdcSerial", nmux), m->GetModuleSerial());
		mux->SetValue(Form("TMrbConfig.Mux.%d.NofParams", nmux), moduleChans);
		mux->SetValue(Form("TMrbConfig.Mux.%d.Serial", nmux), m->GetSerial());
		mux->SetValue(Form("TMrbConfig.Mux.%d.FirstChannel", nmux), m->GetFirstChannel());
		mux->SetValue(Form("TMrbConfig.Mux.%d.LookupFile", nmux), Form("%s.lkp", m->GetName()));
		m->WriteLookup(Form("%s.lkp", m->GetName()));
		nmux++;
	}
	mux->SetValue("TMrbConfig.Mux.NofParams", moduleChans);
	mux->SetValue("TMrbConfig.Mux.ModuleRange", moduleRange);
	mux->SaveLevel(kEnvLocal);
	return(kTRUE);	
}

const Char_t * TMrbConfig::GetLofEventsAsString(TString & LofEvents) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetLofEventsAsString
// Purpose:        Return a colon-separated string with event names
// Arguments:      TString & LofEvents    -- where to store event names
// Results:        Char_t * LofEvents     -- string containing event names
// Exceptions:
// Description:    Returns a colon-separated string with event names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;

	LofEvents = "";
	if (fNofEvents > 0) {
		evt = (TMrbEvent *) fLofEvents.First();
		while (evt) {
			if (LofEvents.Length() > 0) LofEvents += ":";
			LofEvents += evt->GetName();
			evt = (TMrbEvent *) fLofEvents.After(evt);
		}
	}
	return(LofEvents.Data());
}

const Char_t * TMrbConfig::GetLofSubeventsAsString(TString & LofSubevents) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetLofSubeventsAsString
// Purpose:        Return a colon-separated string with subevent names
// Arguments:      TString & LofSubevents    -- where to store subevent names
// Results:        Char_t * LofSubevents     -- string containing subevent names
// Exceptions:
// Description:    Returns a colon-separated string with subevent names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;

	LofSubevents = "";
	if (fNofSubevents > 0) {
		sevt = (TMrbSubevent *) fLofSubevents.First();
		while (sevt) {
			if (LofSubevents.Length() > 0) LofSubevents += ":";
			LofSubevents += sevt->GetName();
			sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
		}
	}
	return(LofSubevents.Data());
}

const Char_t * TMrbConfig::GetLofModulesAsString(TString & LofModules) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::GetLofModulesAsString
// Purpose:        Return a colon-separated string with module names
// Arguments:      TString & LofModules      -- where to store module names
// Results:        Char_t * LofModules       -- string containing module names
// Exceptions:
// Description:    Returns a colon-separated string with module names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	LofModules = "";
	if (fNofModules > 0) {
		module = (TMrbModule *) fLofModules.First();
		while (module) {
			if (LofModules.Length() > 0) LofModules += ":";
			LofModules += module->GetName();
			module = (TMrbModule *) fLofModules.After(module);
		}
	}
	return(LofModules.Data());
}

