#ifndef __TMrbConfig_h__
#define __TMrbConfig_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbConfig.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbConfig           -- generate MARaBOU configuration
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"
#include "TNamed.h"
#include "TString.h"
#include "TObjString.h"
#include "TArrayI.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"

#include "TMrbCNAF.h"
#include "TMrbLogger.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           packNames
//////////////////////////////////////////////////////////////////////////////

class packNames : public TObject {
	public:
		packNames(const Char_t * f, const Char_t * t, const Char_t * x, const Char_t * c) { F = f; T = t; X = x; C = c; };
		~packNames() {};
		inline TString & GetF() { return(F); };
		inline TString & GetT() { return(T); };
		inline TString & GetX() { return(X); };
		inline TString & GetC() { return(C); };
	protected:
		TString F; TString T; TString X; TString C;
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig
// Purpose:        Define configuration and generate code
// Description:    Creates an experimental configuration and generates code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbConfig : public TNamed {

	public:
		enum EMrbReadoutTag 	{	kRdoPath			=	1,			// readout tags
									kRdoFile,
									kRdoNameLC,
									kRdoNameUC,
									kRdoCreationDate,
									kRdoAuthor,
									kRdoUser,
									kRdoTitle,
									kRdoSetup,
									kRdoCamacController,
									kRdoDefineTriggers,
									kRdoDefinePointers,
									kRdoDeviceTables,
									kRdoInitPointers,
									kRdoInitModules,
									kRdoOnStartAcquisition,
									kRdoOnStopAcquisition,
									kRdoOnTriggerXX,
									kRdoIgnoreTriggerXX,
									kRdoWriteSetup,
									kRdoIncludesAndDefs,
									kRdoUtilities,
									kRdoDefinePrototypes,
									kRdoDefineGlobals,
									kRdoDefineGlobalsOnce,
									kRdoDefineLocalVarsInit,
									kRdoDefineLocalVarsReadout,
									kRdoClearModule
								};
						
		enum EMrbAnalyzeTag 	{	kAnaFile			=	1,			// analyze tags
									kAnaNameLC,
									kAnaNameUC,
									kAnaTitle,
									kAnaCreationDate,
									kAnaAuthor,
									kAnaUser,
									kAnaHtmlMakeSetup,
									kAnaIncludesAndDefs,
									kAnaPragmaLinkClasses,
									kAnaClassImp,
									kAnaMakeClassNames,
									kAnaInitializeLists,
									kAnaAddUserEnv,
									kAnaFindVars,
									kAnaEventClassDef,
									kAnaEventClassMethods,
									kAnaEventClassInstance,
									kAnaEventDefinePointers,
									kAnaEventNameLC,
									kAnaEventNameUC,
									kAnaEventTitle,
									kAnaEventAutoSave,
									kAnaEventTrigger,
									kAnaEventIdEnum,
									kAnaEventPrivateData,
									kAnaEventUserMethods,
									kAnaEventUserData,
									kAnaEventDispatchOverTrigger,
									kAnaEventIgnoreTrigger,
									kAnaEventBookParams,
									kAnaEventBookHistograms,
									kAnaEventSetupSevtList,
									kAnaEventAllocHitBuffer,
									kAnaEventSetFakeMode,
									kAnaEventCreateTree,
									kAnaEventAddBranches,
									kAnaEventSetScaleDown,
									kAnaEventInitializeTree,
									kAnaEventSetReplayMode,
									kAnaEventInitializeBranches,
									kAnaEventSetBranchStatus,
									kAnaEventReplayTree,
									kAnaEventFirstSubevent,
									kAnaEventBuildEvent,
									kAnaEventAnalyze,
									kAnaEvtResetData,
									kAnaSevtNameLC,
									kAnaSevtNameUC,
									kAnaSevtTitle,
									kAnaSevtSetName,
									kAnaSevtSerial,
									kAnaSevtSerialEnum,
									kAnaSevtBitsEnum,
									kAnaSevtIndicesEnum,
									kAnaSevtClassDef,
									kAnaSevtClassMethods,
									kAnaSevtClassInstance,
									kAnaSevtUserMethods,
									kAnaSevtUserData,
									kAnaSevtFriends,
									kAnaSevtCtor,
									kAnaSevtDispatchOverType,
									kAnaSevtDispatchOverSerial,
									kAnaSevtDefineAddr,
									kAnaSevtPrivateData,
									kAnaSevtPrivateHistograms,
									kAnaSevtFillSubevent,
									kAnaSevtBookParams,
									kAnaSevtBookHistograms,
									kAnaSevtFillHistograms,
									kAnaSevtInitializeBranch,
									kAnaSevtResetData,
									kAnaModuleSerialEnum,
									kAnaHistoDefinePointers,
									kAnaHistoInitializeArrays,
									kAnaHistoBookUserDefined,
									kAnaVarDefinePointers,
									kAnaVarClassInstance,
									kAnaVarArrDefinePointers,
									kAnaVarArrClassInstance,
									kAnaWdwDefinePointers,
									kAnaWdwClassInstance,
									kAnaUserDefsAndProtos,
									kAnaUserInitialize,
									kAnaUserReloadParams,
									kAnaUserBookParams,
									kAnaUserBookHistograms,
									kAnaUserGlobals,
									kAnaUserUtilities,
									kAnaUserMessages,
									kAnaMakeUserHeaders,
									kAnaMakeUserCode,
									kAnaIncludeEvtSevtModGlobals,
									kAnaInitializeEvtSevtMods
								};

		enum EMrbConfigTag  	{	kCfgFile				=	1,		// config tags
									kCfgNameLC,
									kCfgNameUC,
									kCfgTitle,
									kCfgAuthor,
									kCfgCreationDate,
									kCfgCreateConfig,
									kCfgDefineEvents,
									kCfgDefineSubevents,
									kCfgDefineModules,
									kCfgAssignParams,
									kCfgConnectToEvent,
									kCfgWriteTimeStamp,
									kCfgWriteDeadTime,
									kCfgDefineVariables,
									kCfgDefineWindows,
									kCfgDefineScalers,
									kCfgMakeCode
								};

		enum EMrbRcFileTag  	{	kRcExpData				=	1,		// rc file tags
									kRcEvtData,
									kRcSevtData,
									kRcModuleData
								};

		enum EMrbUserMacroTag  	{	kUmaFile				=	1,		// user macro tags
									kUmaNameLC,
									kUmaNameUC,
									kUmaTitle,
									kUmaAuthor,
									kUmaCreationDate
								};

		enum					{	kNoOptionSpecified		= 0x80000000	};

		enum EMrbReadoutOptions	{	kRdoOptOverwrite		=	BIT(0),
									kRdoOptVerbose			=	BIT(1)		};		// readout options
		enum					{	kRdoOptDefault			=	0			};

		enum EMrbAnalyzeOptions	{	kAnaOptSubevents		=	BIT(0),		// analyze options
									kAnaOptParamsByName 	=	BIT(1),
									kAnaOptHistograms		=	BIT(2),
									kAnaOptLeaves			=	BIT(3),
									kAnaOptOverwrite		=	BIT(4),
									kAnaOptReplayMode		=	BIT(5),
									kAnaOptEventBuilder 	=	BIT(6),
									kAnaOptFillHistosAfter	=	BIT(7),
									kAnaOptVerbose			=	BIT(8)
								};
		enum					{	kAnaOptDefault			=	kAnaOptSubevents |
																kAnaOptParamsByName |
																kAnaOptHistograms};

		enum EMrbConfigOptions	{	kCfgOptOverwrite		=	BIT(0),		// config options
									kCfgOptVerbose			=	BIT(1)
								};
		enum					{	kCfgOptDefault			=	0	}; 

		enum EMrbRcFileOptions	{	kRcOptOverwrite			=	BIT(0),		// rc file options
									kRcOptByName			=	BIT(1),
									kRcOptVerbose			=	BIT(2)
								};
		enum					{	kRcOptDefault			=	kRcOptByName	}; 

		enum EMrbIncludeOptions	{	kIclOptGlobals				=	BIT(0),		// include options
									kIclOptInitialize	 		=	BIT(1),
									kIclOptReloadParams			=	BIT(2),
									kIclOptBookHistograms		=	BIT(3),
									kIclOptBookParams			=	BIT(4),
									kIclOptBuildRootEvent		=	BIT(5),
									kIclOptAnalyze				=	BIT(6),
									kIclOptEventBuilder			=	BIT(7),
									kIclOptUtilities			=	BIT(8),
									kIclOptDefsAndProtos		=	BIT(9),
									kIclOptHandleUserMessages	=	BIT(10),
									kIclOptByEventName			=	BIT(13)
								};
		enum					{	kIclOptDefault			=	kIclOptGlobals
																| kIclOptInitialize
																| kIclOptReloadParams
																| kIclOptBookHistograms
																| kIclOptBookParams
																| kIclOptAnalyze
																| kIclOptUtilities
																| kIclOptDefsAndProtos
								};

		enum					{	kNofCrates			=	100			};	// max number of crates
		enum					{	kNofControllers		=	2			};	// max number of camac controllers
		enum					{	kNofTriggers		=	16			};	// max number of triggers
		enum					{	kAutoSave			=	10000000	};	// auto save after 10 MB

		enum EMrbTriggerStatus	{	kTriggerUnused			=	0,		// trigger status
									kTriggerAssigned,
									kTriggerPattern,
									kTriggerReserved,
									kTriggerIgnored,
									kTriggerOutOfRange
								};

		enum EMrbReservedTriggers	{
									kTriggerStartAcq	=	14, 	// reserved trigger numbers
									kTriggerStopAcq 	=	15
								};

		enum					{	kRdoOptEvtDefault	=	0	};					// default readout options
		enum					{	kAnaOptEvtDefault	=	kAnaOptSubevents | kAnaOptHistograms	};	// default analyze options
		enum					{	kCfgOptEvtDefault	=	0	};					// default config options

		enum					{	kRdoOptSevtDefault	=	0	};										// default readout options
		enum					{	kAnaOptSevtDefault	=	kAnaOptParamsByName | kAnaOptHistograms };	// default analyze options
		enum					{	kCfgOptSevtDefault	=	0	};										// default config options

		enum EMrbModuleTag	 	{	kModuleDefs			=	1,	// special readout tags
									kModuleInitModule,
									kModuleInitChannel,
									kModuleReadChannel,
									kModuleIncrementChannel,
									kModuleSkipChannels,
									kModuleWriteSubaddr,
									kModuleStoreData,
									kModuleClearModule,
									kModuleReadModule,
									kModuleSetupReadout,
									kModuleFinishReadout,
									kModuleStartAcquisition,
									kModuleStopAcquisition,
									kModuleStartAcquisitionGroup,
									kModuleStopAcquisitionGroup,
									kModuleUtilities,
									kModuleDefineGlobals,
									kModuleDefineGlobalsOnce,
									kModuleDefineLocalVarsInit,
									kModuleDefineLocalVarsReadout,
									kModuleDefinePrototypes
								};
						
		enum EMrbChannelStatus	{
									kChannelSingle  	=	1,			// camac channel status
									kChannelArray,
									kChannelArrElem
								};

		enum EMrbDataType		{	kDataChar			=	BIT(0),			// data types
									kDataUShort 		=	BIT(1),
									kDataUInt			=	BIT(2),
									kDataULong			=	BIT(3),
									kDataFloat 			=	BIT(4)
								};

		enum EMrbCrateType  	{	kCrateUnused		=	0,			// crate types
									kCrateCamac 		=	BIT(0),
									kCrateVME			=	BIT(1),
									kCrateAny			=	kCrateCamac | kCrateVME
								};

		enum EMrbControllerType {	kControllerUnused	=	0,			// controller types (camac)
									kControllerCBV 		=	5,			// (see MBS manual!)
									kControllerCC32		=	20
								};

		enum EMrbHistoType  	{	kHistoTH1			=	BIT(10), 	// histogram types
									kHistoTH2			=	BIT(11),
									kHistoTHC			=	BIT(1),
									kHistoTHS			=	BIT(2),
									kHistoTHF			=	BIT(3),
									kHistoTHD			=	BIT(4),
									kHistoTH1C			=	kHistoTH1 | kHistoTHC,
									kHistoTH1S			=	kHistoTH1 | kHistoTHS,
									kHistoTH1F			=	kHistoTH1 | kHistoTHF,
									kHistoTH1D			=	kHistoTH1 | kHistoTHD,
									kHistoTH2C			=	kHistoTH2 | kHistoTHC,
									kHistoTH2S			=	kHistoTH2 | kHistoTHS,
									kHistoTH2F			=	kHistoTH2 | kHistoTHF,
									kHistoTH2D			=	kHistoTH2 | kHistoTHD
								};

		enum EMrbModuleType  	{	kModuleRaw			=	TMrbConfig::kCrateUnused,		// module types
									kModuleCamac		=	TMrbConfig::kCrateCamac,
									kModuleVME			=	TMrbConfig::kCrateVME,
									kModuleListMode		=	BIT(2),
									kModuleSingleMode	=	BIT(3),
									kModuleScaler		=	BIT(4),
									kModuleControl		=	BIT(5)
								};

		enum EMrbManufacturers	{	kManufactOther		=	0,
									kManufactSilena 	=	BIT(12),
									kManufactOrtec		=	BIT(13),
									kManufactLeCroy 	=	BIT(14),
									kManufactSen		=	BIT(15),
									kManufactTUM		=	BIT(16),
									kManufactNE 		=	BIT(17),
									kManufactCaen		=	BIT(18),
									kManufactGanelec	=	BIT(19),
									kManufactXia		=	BIT(20),
									kManufactAcromag	=	BIT(21),
									kManufactKinetics	=	BIT(22)
								};

		enum EMrbModuleID		{	kModuleSilena4418V		=   kManufactSilena + 1,  // modules ids
									kModuleSilena4418T		=	kManufactSilena + 2,
									kModuleUct8904			=	kManufactTUM + 3,
									kModuleSen16P2047		=	kManufactSen + 4,
									kModuleSilena7420Smdw	=	kManufactSilena + 5,
									kModuleLeCroy2228A		=	kManufactLeCroy + 6,
									kModuleLeCroy4434		=	kManufactLeCroy + 7,
									kModuleAdcd9701 		=	kManufactTUM + 8,
									kModuleSen2010			=	kManufactSen + 9,
									kModuleNE9041			=	kManufactNE + 10,
									kModuleCaenV260 		=	kManufactCaen + 11,
									kModuleCaenV556 		=	kManufactCaen + 12,
									kModuleCaenV785 		=	kManufactCaen + 13,
									kModuleGanelec_Q1612F 	=	kManufactGanelec + 14,
									kModuleOrtec_413A	 	=	kManufactOrtec + 15,
									kModuleXia_DGF_4C	 	=	kManufactXia + 16,
									kModuleAcromag_IP341	 =	kManufactAcromag + 17,
									kModuleLeCroy4432		 =	kManufactLeCroy + 18,
									kModuleLeCroy4448		 =	kManufactLeCroy + 19,
									kModuleKinetics3655		 =	kManufactKinetics + 20,
									kModuleCaenV775 		=	kManufactCaen + 21,
									kModuleCaenV820 		=	kManufactCaen + 22,
									kModuleUserDefined	 	=	kManufactOther,
									kModuleSoftModule	 	=	kManufactOther + 1
								};

		enum EMrbScalerMode  	{	kScalerCamac		=	TMrbConfig::kCrateCamac,		// scaler types
									kScalerVME			=	TMrbConfig::kCrateVME,
									kScalerListMode		=	TMrbConfig::kModuleListMode,
									kScalerSingleMode	=	TMrbConfig::kModuleSingleMode,
									kScalerDeadTime 	=	BIT(8),
									kScalerExternalGate =	BIT(9)
								};

		enum EMrbScalerFunction {	kScalerFctInit		=	0,		// scaler functions
									kScalerFctClear 	=	1,
									kScalerFctRead		=	2,
									kScalerFctDeadTime	=	3,
									kScalerFctWrite		=	4
								};
		enum					{	kNofScalerFunctions =	kScalerFctWrite + 1 };

		enum					{	kRdoHeaderBit		=	BIT(31)	};

	public:

		TMrbConfig() {};										// default ctor

		TMrbConfig(const Char_t * CfgName, const Char_t * CfgTitle = "");	// create config

		~TMrbConfig() { 										// dtor
			fLofEvents.Delete();								// delete objects stored in TMrbLofNamedXs
			fLofSubevents.Delete();
			fLofModules.Delete();
			fLofScalers.Delete();
			fLofReadoutTags.Delete();
			fLofAnalyzeTags.Delete();
			fLofConfigTags.Delete();
			fCNAFNames.Delete();
			fLofModuleTags.Delete();
			fLofModuleIDs.Delete();
			fLofUserClasses.Delete();
			fLofOnceOnlyTags.Delete();
		};

		TMrbConfig(const TMrbConfig &) {};				// default copy ctor

														// generate user-defined readout code
		Bool_t MakeReadoutCode(const Char_t * CodeFile = "", Option_t * Options = "");
								
 														// generate class defs and methods
		Bool_t MakeAnalyzeCode(const Char_t * CodeFile = "", Option_t * Options = "Subevents:byName:Histograms");
		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex,
											UInt_t VarType, TMrbTemplate & Template, const Char_t * Prefix = NULL);
		
														// include class specific code
		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, const Char_t * ClassName, const Char_t * CodeFile,
											TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);

 														// generate config file from memory
		Bool_t MakeConfigCode(const Char_t * CodeFile = "", Option_t * Options = "");

 														// generate rc file
		Bool_t MakeRcFile(const Char_t * CodeFile = "", const Char_t * ResourceName = "", Option_t * Options = "byName");

		Bool_t CallUserMacro(const Char_t * MacroName = "");				// call user macro
		Bool_t ExecUserMacro(ofstream * Strm, TObject * CfgObject, const Char_t * TagWord);
				
		Bool_t CompileReadoutCode(const Char_t * Host, Bool_t CleanFlag = kTRUE);	// compile readout code
		Bool_t CompileAnalyzeCode(Bool_t CleanFlag = kTRUE);						// compile analysis code

		void Print(ostream & OutStrm, const Char_t * Prefix = "");			// show data
		inline virtual void Print() { Print(cout, ""); };

		Int_t GetNofErrors();							// number of errors
		Int_t PrintErrors(); 							// print error summary
		
		inline void AddEvent(TObject * Evt) {				 				// add a new event
			fLofEvents.Add(Evt);
			fNofEvents++;
		};

		inline TObject * FindEvent(const Char_t * EvtName) {				// find an event
			return (fLofEvents.FindObject(EvtName));
		};

		TObject * FindEvent(Int_t Trigger);									// find event by its trigger

		inline void AddSubevent(TObject * Sevt) {							// add a new subevent
			fLofSubevents.Add(Sevt);
			fNofSubevents++;
		};

		inline TObject * NextSubevent(TObject * After = NULL) {					// get next subevent from list
			return((After == NULL) ? fLofSubevents.First() : fLofSubevents.After(After));
		};

		inline TObject * FindSubevent(const Char_t * SevtName) {			// find a subevent
			return (fLofSubevents.FindObject(SevtName));
		};

		TObject * FindSubevent(TClass * Class, TObject * After = NULL);		// find a subevent by class type

		TObject * FindSubevent(Int_t SevtSerial);							// find subevent by its serial number

		inline Int_t AssignSevtSerial() { return(fNofSubevents + 1); }; 	// get unique serial number for a subevent
			
		inline void AddModule(TObject * Module) { 							// add a new module
			fLofModules.Add(Module);
			fNofModules++;
		};

		inline TObject * NextModule(TObject * After = NULL) {						// get next module from list
			return((After == NULL) ? fLofModules.First() : fLofModules.After(After));
		};

		inline TObject * FindModule(const Char_t * ModuleName) {			// find module by its name
			return (fLofModules.FindObject(ModuleName));
		};

		TObject * FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TObject * After = NULL); 		// find (next) module by its id
		TObject * FindModuleByType(UInt_t ModuleType, TObject * After = NULL); 	// find (next) module by its type
		TObject * FindModuleByCrate(Int_t Crate, TObject * After = NULL);		// find (next) module by crate number
		TObject * FindModuleBySerial(Int_t ModuleSerial);						// find module by its unique serial

		inline Int_t AssignModuleSerial() { return(fNofModules + 1); }; 		// set unique serial number for a module
			
		Bool_t CheckModuleAddress(TObject * Module);							// check if module address or position legal

		inline Bool_t HasCamacModules() { return(FindModuleByType(TMrbConfig::kModuleCamac) != NULL); };	// camac?
		inline Bool_t HasVMEModules() { return(FindModuleByType(TMrbConfig::kModuleVME) != NULL); };		// vme?

		inline void  AddScaler(TObject * Scaler) {			 					// add a new scaler
			fLofScalers.Add(Scaler);
			fNofScalers++;
		};

		TObject * FindParam(const Char_t * ParamName);							// find a param 

		inline TObject * FindScaler(const Char_t * ScalerName) {				// find a scaler
			return (fLofScalers.FindObject(ScalerName));
		};
		TObject * FindScalerByCrate(Int_t Crate, TObject * After = NULL);		// find (next) scaler in a given crate

		void GetAuthor();						// author's name

		inline UInt_t GetReadoutOptions() { return(fReadoutOptions); }; // return MakeReadoutCode() options
		inline UInt_t GetAnalyzeOptions() { return(fAnalyzeOptions); }; // return MakeAnalyzeCode() options
		inline UInt_t GetConfigOptions() { return(fConfigOptions); };	// return MakeConfigCode() options
		inline UInt_t GetRcFileOptions() { return(fRcFileOptions); };	// return MakeRcFile() options

		inline Bool_t IsVerbose() { return(fVerboseMode); }; 			// verbose mode?
		
		Bool_t DefineVariables(const Char_t * VarType, const Char_t * VarDefs); 	// define a set of variables
		Bool_t DefineVariables(const Char_t * VarType, Int_t Value, const Char_t * VarDefs);
		Bool_t DefineVariables(const Char_t * VarType, Float_t Value, const Char_t * VarDefs);
		Bool_t DefineVariables(const Char_t * VarType, const Char_t * Value, const Char_t * VarDefs);

		Bool_t DefineWindows(const Char_t * WdwType, const Char_t * WdwDefs);	 	// define 1-dim windows
		Bool_t DefineWindows(const Char_t * WdwType, Int_t Xlower, Int_t Xupper, const Char_t * WdwDefs);
		Bool_t DefineWindows(const Char_t * WdwType, Float_t Xlower, Float_t Xupper, const Char_t * WdwDefs);

		inline Bool_t LongParamNamesToBeUsed() { return(fLongParamNames); };	// long names to guarantee uniqueness?
		inline void UseLongParamNames(Bool_t Flag = kTRUE) { fLongParamNames = Flag; };

		inline void WriteTimeStamp() { fWriteTimeStamp = kTRUE; };	// write a time stamp
		inline Bool_t TimeStampToBeWritten() { return(fWriteTimeStamp); };

		Bool_t WriteDeadTime(const Char_t * Scaler, Int_t Interval = 1000);				// write a dead time events
		inline Bool_t DeadTimeToBeWritten() { return(fDeadTimeInterval > 0); }; 	// check if dead time enabled
		inline Int_t GetDeadTimeInterval() { return(fDeadTimeInterval); };			// get interval
		inline TObject * GetDeadTimeScaler() { return(fDeadTimeScaler); };			// get scaler def

																					// include user-specific code
		Bool_t IncludeUserCode(const Char_t * Path, const Char_t * Prefix, Option_t * Options);
		inline Bool_t IncludeUserCode(const Char_t * Prefix = "", Option_t * Options = "Default") {
									return(this->IncludeUserCode("", Prefix, Options));
		};

		inline Bool_t UserCodeToBeIncluded() { return(fLofUserIncludes.Last() >= 0); };
		inline TMrbLofNamedX * GetLofUserIncludes() { return(&fLofUserIncludes); };

		void AddUserClass(const Char_t * Name); 									// add a user class
			
		inline TMrbLogger * GetMessageLogger() { return(fMessageLogger); };
		
		void Version();																// output welcome tex

		TMrbConfig * ReadFromFile(const Char_t * ConfigFile = "", Option_t * Options = ""); // read config from root file
		Bool_t WriteToFile(const Char_t * ConfigFile = "", Option_t * Options = "");		// write config objects to root file

		void SetGlobalAddress();							// set global address gMrbConfig

		inline void  SetMultiBorC(TMrbCNAF::EMrbCNAF MultiBorC) { 	// store multi branch/crate status
			fMultiBorC |= MultiBorC;
		};

		inline void SetCrateType(Int_t Crate, EMrbCrateType CrateType) { fCrateTable[Crate] = CrateType; };
		inline EMrbCrateType GetCrateType(Int_t Crate) { return((EMrbCrateType) fCrateTable[Crate]); };
		Int_t FindCrate(Int_t After = -1);												// find next crate
		Int_t GetNofCrates(EMrbCrateType CrateType = kCrateAny);															// return number of crates
		UInt_t GetCratePattern(EMrbCrateType CrateType = kCrateAny);														// return crate numbers as bit pattern
		
		Bool_t SetControllerType(Int_t Crate, const Char_t * Type);
		Bool_t SetControllerType(const Char_t * Crate, const Char_t * Type);
		inline void SetControllerType(Int_t Crate, EMrbControllerType Type) { fControllerTable[Crate] = Type; };
		inline EMrbControllerType GetControllerType(Int_t Crate) { return((EMrbControllerType) fControllerTable[Crate]); };
																	// handle trigger patterns
		Bool_t HandleMultipleTriggers(Int_t T1 = 0, Int_t T2 = 0, Int_t T3 = 0, Int_t T4 = 0, Int_t T5 = 0);
		TMrbConfig::EMrbTriggerStatus GetTriggerStatus(Int_t Trigger); 		// get trigger status
		void UpdateTriggerTable(Int_t Trigger = 0);		// update trigger table: valid patterns, overlapping, etc.

		void AddToTagList(const Char_t * CodeFile, Int_t TagIndex); 		// add file:tag to be processed once
		Bool_t TagToBeProcessed(const Char_t * CodeFile, Int_t TagIndex);	// check if tag has already been processed

		Bool_t BookHistogram(const Char_t * Type, const Char_t * Name, const Char_t * Title,
									Int_t A0, Int_t A1 = -1, Int_t A2 = -1, Int_t A3 = -1,
									Int_t A4 = -1, Int_t A5 = -1, Int_t A6 = -1, Int_t A7 = -1);


		inline Int_t GetNofModules() { return(fNofModules); };
		
		inline TMrbLofNamedX * GetLofEvents() { return(&fLofEvents); };		// get address of ...
		inline TMrbLofNamedX * GetLofSubevents() { return(&fLofSubevents); };
		inline TMrbLofNamedX * GetLofModules() { return(&fLofModules); };
		inline TMrbLofNamedX * GetLofScalers() { return(&fLofScalers); };
		inline TMrbLofNamedX * GetLofModuleIDs() { return(&fLofModuleIDs); };
		inline TMrbLofNamedX * GetLofModuleTypes() { return(&fLofModuleTypes); };
		inline TMrbLofNamedX * GetLofDataTypes() { return(&fLofDataTypes); };
		inline TMrbLofNamedX * GetLofHistoTypes() { return(&fLofHistoTypes); };

		Bool_t NameNotLegal(const Char_t * ObjType, const Char_t * ObjName);	// check if name is legal within MARaBOU

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbConfig.html&"); };

	public: 									// public lists of key words:
		TMrbLofNamedX fLofDataTypes;			// ... data types
		TMrbLofNamedX fLofCrateTypes;			// ... crate types
		TMrbLofNamedX fLofControllerTypes;		// ... camac controllers
		TMrbLofNamedX fLofModuleTypes;			// ... module types
		TMrbLofNamedX fLofReadoutTags;			// ... readout tags
		TMrbLofNamedX fLofAnalyzeTags;			// ... analyze tags
		TMrbLofNamedX fLofConfigTags;			// ... config tags
		TMrbLofNamedX fLofRcFileTags;			// ... rc file tags
		TMrbLofNamedX fLofUserMacroTags;		// ... user macro tags
		TMrbLofNamedX fCNAFNames;				// ... cnaf key words
		TMrbLofNamedX fLofModuleTags;			// ... camac tags
		TMrbLofNamedX fLofModuleIDs;			// ... camac modules available
		TMrbLofNamedX fLofHistoTypes;			// ... histogram types

	protected:
		Bool_t DefineVarOrWdw(TMrbNamedX * VarType, TObject * VarProto, const Char_t * VarDefs);	// common part of var/wdw definition
		
	protected:
		Bool_t fVerboseMode;				// verbose flag
		TMrbLogger * fMessageLogger;		//! addr of message logger
			
		Int_t fNofEvents;					// list of events
		TMrbLofNamedX fLofEvents;

		TArrayI fCrateTable;				// crate table (camac and VME)
		TArrayI fControllerTable;			// controller table (camac only)

		Int_t fNofSubevents;				// list of subevents
		TMrbLofNamedX fLofSubevents;

		Int_t fNofModules;					// list of modules
		TMrbLofNamedX fLofModules;

		Bool_t fLongParamNames; 			// use long param names: i.e. <subevent><param>

		Int_t fNofScalers;					// list of scalers
		TMrbLofNamedX fLofScalers;

		UInt_t fMultiBorC;					// multi branch? multi crate?

		UInt_t fReadoutOptions; 			// options used in MakeReadoutCode()
		UInt_t fAnalyzeOptions; 			// ... in MakeAnalyzeCode()
		UInt_t fConfigOptions;  			// ... in MakeConfigCode()
		UInt_t fRcFileOptions;  			// ... in MakeRcFile()

		TMrbLofNamedX fLofUserIncludes;		// list of user-specific files to be included

		Bool_t fWriteTimeStamp; 			// kTRUE if user wants a time stamp to be added to each event

		Int_t fDeadTimeInterval;			// number of events a new dead time event will be created
		TObject * fDeadTimeScaler;	 		// dead-time scaler

		TString fCreationDate;				// creation date & time
		TString fUser;						// user name
		TString fAuthor;

		UInt_t fTriggerMask;				// trigger bits defined so far
		Bool_t fSingleBitTriggersOnly;		// triggers 1,2,4, or 8 only
		TArrayI fTriggersToBeHandled;		// trigger patterns to be handled

		Bool_t fUserMacroToBeCalled;		// call user macro
		TString fUserMacro; 				// macro name
		TMrbString fUserMacroCmd;				// ... command
				
		TList fLofUserClasses; 				// list of classes added by user
		TList fLofOnceOnlyTags; 			// list of tags already processed
		TList fLofUserHistograms;			// list of user-defined histograms
		
	ClassDef(TMrbConfig, 1) 	// [Config] Base class to describe an experimental setup in MARaBOU
};	

#endif
