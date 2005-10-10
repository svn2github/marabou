#ifndef __TMrbConfig_h__
#define __TMrbConfig_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbConfig.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbConfig           -- generate MARaBOU configuration
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbConfig.h,v 1.60 2005-10-10 06:30:06 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

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
		inline const TString & GetF() const { return(F); };
		inline const TString & GetT() const { return(T); };
		inline const TString & GetX() const { return(X); };
		inline const TString & GetC() const { return(C); };
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
									kRdoInclude,
									kRdoLibs,
									kRdoPosix,
									kRdoPosixLib,
									kRdoLynxPlatform,
									kRdoDebug,
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
									kRdoInitEnvironment,
									kRdoInitCommonCode,
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
									kRdoUserDefinedDefines,
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
									kAnaModuleTimeOffset,
									kAnaAddUserEnv,
									kAnaFindVars,
									kAnaEventClassDef,
									kAnaEventClassMethods,
									kAnaEventUserClassMethods,
									kAnaEventClassInstance,
									kAnaEventUserClassInstance,
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
									kAnaEventMethodToProcessEvent,
									kAnaEventDispatchOverTrigger,
									kAnaEventIgnoreTrigger,
									kAnaEventTriggerStartAcq,
									kAnaEventTriggerStopAcq,
									kAnaEventReplayEvent,
									kAnaEventBookParams,
									kAnaEventBookHistograms,
									kAnaEventFillHistograms,
									kAnaEventFillRateHistograms,
									kAnaEventSetupSevtList,
									kAnaEventAllocHitBuffer,
									kAnaEventSetFakeMode,
									kAnaEventCreateTree,
									kAnaEventAddBranches,
									kAnaEventSetScaleDown,
									kAnaEventInitializeTree,
									kAnaEventSetReplayMode,
									kAnaEventSetWriteTree,
									kAnaEventInitializeBranches,
									kAnaEventSetBranchStatus,
									kAnaEventReplayTree,
									kAnaEventFirstSubevent,
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
									kAnaSevtGetAddr,
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
									kAnaModuleIdEnum,
									kAnaModuleSerialEnum,
									kAnaModuleSpecialEnum,
									kAnaHistoDefinePointers,
									kAnaHistoInitializeArrays,
									kAnaHistoBookUserDefined,
									kAnaHistoFillArrays,
									kAnaVarDefinePointers,
									kAnaVarClassInstance,
									kAnaVarArrDefinePointers,
									kAnaVarArrClassInstance,
									kAnaWdwDefinePointers,
									kAnaWdwClassInstance,
									kAnaWdwAdjustPointers,
									kAnaUserInitializeBeforeHB,
									kAnaUserInitializeAfterHB,
									kAnaUserReloadParams,
									kAnaUserBookParams,
									kAnaUserBookHistograms,
									kAnaUserGlobals,
									kAnaUserUtilities,
									kAnaUserMessages,
									kAnaUserDummyMethods,
									kAnaUsingNameSpace,
									kAnaUserDefinedGlobals,
									kAnaUserDefinedEnums,
									kAnaUserDefinedDefines,
									kAnaMakeUserCxxFlags,
									kAnaMakeUserHeaders,
									kAnaMakeUserCode,
									kAnaMakeUserLibs,
									kAnaMakeUserRules,
									kAnaMakeLibNew,
									kAnaIncludeEvtSevtModGlobals,
									kAnaInitializeEvtSevtMods,
									kAnaLoadUserLibs,
									kAnaSpecialHitDef,
									kAnaSpecialHitMethods
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
									kRcModuleData,
									kRcUserGlobals
								};

		enum EMrbUserMacroTag  	{	kUmaFile				=	1,		// user macro tags
									kUmaNameLC,
									kUmaNameUC,
									kUmaTitle,
									kUmaAuthor,
									kUmaCreationDate
								};

		enum EMrbUserEventTag  	{	kUevFile				=	1,		// user event tags
									kUevNameLC,
									kUevNameUC,
									kUevConfigLC,
									kUevConfigUC,
									kUevTitle,
									kUevAuthor,
									kUevCreationDate
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
									kAnaOptVerbose			=	BIT(7)
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

		enum EMrbIncludeOptions	{	kIclOptHeaderFile			=	BIT(0),		// include options
									kIclOptUserMethod			=	BIT(1),
									kIclOptClassTMrbAnalyze		=	BIT(2),
									kIclOptClassTUsrEvent		=	BIT(3),
									kIclOptUserLib				=	BIT(4),
									kIclOptUserClass			=	BIT(5),
									kIclOptInitialize			=	kIclOptUserMethod | kIclOptClassTMrbAnalyze | BIT(6),
									kIclOptReloadParams			=	kIclOptUserMethod | kIclOptClassTMrbAnalyze | BIT(7),
									kIclOptHandleMessages		=	kIclOptUserMethod | kIclOptClassTMrbAnalyze | BIT(8),
									kIclOptBookHistograms		=	kIclOptUserMethod | kIclOptClassTUsrEvent | BIT(9),
									kIclOptBookParams			=	kIclOptUserMethod | kIclOptClassTUsrEvent | BIT(10),
									kIclOptProcessEvent			=	kIclOptUserMethod | kIclOptClassTUsrEvent | BIT(11),
									kIclOptEventMethod			=	kIclOptUserMethod | kIclOptClassTUsrEvent | BIT(12),
									kIclOptUserDefinedEvent		=	kIclOptUserClass | BIT(13),
									kIclOptUtilities			=	BIT(14)
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
									kModuleInitCommonCode,
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

		enum EMrbGlobalType		{	kGlobInt			=	0,			// global types
									kGlobFloat,
									kGlobDouble,
									kGlobBool,
									kGlobString,
									kGlobObject
								};

		enum EMrbCrateType  	{	kCrateUnused		=	0,			// crate types
									kCrateCamac 		=	BIT(0),
									kCrateVME			=	BIT(1),
									kCrateAny			=	kCrateCamac | kCrateVME
								};

		enum EMrbControllerType {	kControllerUnused	=	0,			// controller types (camac)
									kControllerCBV 		=	5,			// (see MBS manual!)
									kControllerCC32		=	11
								};

		enum EMrbHistoType  	{	kHistoTH1			=	BIT(10), 	// histogram types
									kHistoTH2			=	BIT(11),
									kHistoTHC			=	BIT(1),
									kHistoTHS			=	BIT(2),
									kHistoTHF			=	BIT(3),
									kHistoTHD			=	BIT(4),
									kHistoTHR			=	BIT(5),
									kHistoTH1C			=	kHistoTH1 | kHistoTHC,
									kHistoTH1S			=	kHistoTH1 | kHistoTHS,
									kHistoTH1F			=	kHistoTH1 | kHistoTHF,
									kHistoTH1D			=	kHistoTH1 | kHistoTHD,
									kHistoTH2C			=	kHistoTH2 | kHistoTHC,
									kHistoTH2S			=	kHistoTH2 | kHistoTHS,
									kHistoTH2F			=	kHistoTH2 | kHistoTHF,
									kHistoTH2D			=	kHistoTH2 | kHistoTHD,
									kHistoRate			=	kHistoTH1 | kHistoTHF | kHistoTHR
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
									kManufactKinetics	=	BIT(22),
									kManufactMpiHD		=	BIT(23),
									kManufactSis		=	BIT(24),
									kManufactCologne	=	BIT(25)
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
									kModuleMpiHD_IOReg 		=	kManufactMpiHD + 23,
									kModuleSis_3600 		=	kManufactSis + 24,
									kModuleSis_3801 		=	kManufactSis + 25,
									kModuleSis_3820 		=	kManufactSis + 26,
									kModuleLeCroy_1176		=	kManufactLeCroy + 27,
									kModuleLeCroy_2280		=	kManufactLeCroy + 28,
									kModuleCologne_CPTM		=	kManufactCologne + 29,
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
			fLofOnceOnlyTags.Delete();
			fLofUserIncludes.Delete();
			fLofUserLibs.Delete();
			fLofUserClasses.Delete();
		};

		TMrbConfig(const TMrbConfig &) : TNamed() {};				// default copy ctor

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

		Bool_t CallUserMacro(const Char_t * MacroName = "", Bool_t AclicFlag = kTRUE);				// call user macro
		Bool_t ExecUserMacro(ofstream * Strm, TObject * CfgObject, const Char_t * TagWord) const;
				
		Bool_t CompileReadoutCode(const Char_t * Host, Bool_t CleanFlag = kTRUE) const;	// compile readout code
		Bool_t CompileAnalyzeCode(Bool_t CleanFlag = kTRUE) const;						// compile analysis code

		inline void SetSevtSize(Int_t Size) { fSevtSize = Size; }; 	// set max size of subevent (for all events/triggers)
		Bool_t UpdateMbsSetup();									// update .mbssetup database if online mode

		Bool_t ExecRootMacro(const Char_t * Macro);

		void Print(Char_t * File) const;
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;			// show data
		inline virtual void Print() const { Print(cout, ""); };
		Int_t GetNofErrors() const;							// number of errors
		Int_t PrintErrors(Bool_t ErrorsOnly = kTRUE) const; // print error summary
		
		inline void AddEvent(TObject * Evt) {				 				// add a new event
			fLofEvents.Add(Evt);
			fNofEvents++;
		};

		inline TObject * FindEvent(const Char_t * EvtName) const {				// find an event
			return (fLofEvents.FindObject(EvtName));
		};

		TObject * FindEvent(Int_t Trigger) const;									// find event by its trigger

		inline Int_t GetNofEvents() { return(fNofEvents); };

		inline void AddSubevent(TObject * Sevt) {							// add a new subevent
			fLofSubevents.Add(Sevt);
			fNofSubevents++;
		};

		inline TObject * NextSubevent(TObject * After = NULL) const {					// get next subevent from list
			return((After == NULL) ? fLofSubevents.First() : fLofSubevents.After(After));
		};

		inline TObject * FindSubevent(const Char_t * SevtName) const {			// find a subevent
			return (fLofSubevents.FindObject(SevtName));
		};

		TObject * FindSubevent(TClass * Class, TObject * After = NULL) const;		// find a subevent by class type

		TObject * FindSubevent(Int_t SevtSerial) const;							// find subevent by its serial number

		inline Int_t AssignSevtSerial() const { return(fNofSubevents + 1); }; 	// get unique serial number for a subevent
			
		inline void AddModule(TObject * Module) { 							// add a new module
			fLofModules.Add(Module);
			fNofModules++;
		};

		inline TObject * NextModule(TObject * After = NULL) const {						// get next module from list
			return((After == NULL) ? fLofModules.First() : fLofModules.After(After));
		};

		inline TObject * FindModule(const Char_t * ModuleName) const {			// find module by its name
			return (fLofModules.FindObject(ModuleName));
		};

		TObject * FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TObject * After = NULL) const; 		// find (next) module by its id
		TObject * FindModuleByType(UInt_t ModuleType, TObject * After = NULL) const; 	// find (next) module by its type
		TObject * FindModuleByCrate(Int_t Crate, TObject * After = NULL) const;		// find (next) module by crate number
		TObject * FindModuleBySerial(Int_t ModuleSerial) const;						// find module by its unique serial

		inline Int_t AssignModuleSerial() const { return(fNofModules + 1); }; 		// set unique serial number for a module
			
		Bool_t CheckModuleAddress(TObject * Module) const;							// check if module address or position legal

		inline Bool_t HasCamacModules() const { return(FindModuleByType(TMrbConfig::kModuleCamac) != NULL); };	// camac?
		inline Bool_t HasVMEModules() const { return(FindModuleByType(TMrbConfig::kModuleVME) != NULL); };		// vme?

		inline void  AddScaler(TObject * Scaler) {			 					// add a new scaler
			fLofScalers.Add(Scaler);
			fNofScalers++;
		};

		TObject * FindParam(const Char_t * ParamName) const;							// find a param 

		Bool_t HistogramExists(const Char_t * HistoName) const;						// check if histo exists

		inline TObject * FindScaler(const Char_t * ScalerName) const {				// find a scaler
			return (fLofScalers.FindObject(ScalerName));
		};
		TObject * FindScalerByCrate(Int_t Crate, TObject * After = NULL) const;		// find (next) scaler in a given crate

		void GetAuthor();						// author's name

		inline UInt_t GetReadoutOptions() const { return(fReadoutOptions); }; // return MakeReadoutCode() options
		inline UInt_t GetAnalyzeOptions() const { return(fAnalyzeOptions); }; // return MakeAnalyzeCode() options
		inline UInt_t GetConfigOptions() const { return(fConfigOptions); };	// return MakeConfigCode() options
		inline UInt_t GetRcFileOptions() const { return(fRcFileOptions); };	// return MakeRcFile() options

		inline Bool_t IsVerbose() const { return(fVerboseMode); }; 			// verbose mode?
		
		Bool_t DefineVariables(const Char_t * VarType, const Char_t * VarDefs); 	// define a set of variables
		Bool_t DefineVariables(const Char_t * VarType, Int_t Value, const Char_t * VarDefs);
		Bool_t DefineVariables(const Char_t * VarType, Double_t Value, const Char_t * VarDefs);
		Bool_t DefineVariables(const Char_t * VarType, const Char_t * Value, const Char_t * VarDefs);

		Bool_t DefineWindows(const Char_t * WdwType, const Char_t * WdwDefs);	 	// define windows
		Bool_t DefineWindows(const Char_t * WdwType, Int_t Xlower, Int_t Xupper, const Char_t * WdwDefs);
		Bool_t DefineWindows(const Char_t * WdwType, Double_t Xlower, Double_t Xupper, const Char_t * WdwDefs);
		Bool_t DefineWindows(const Char_t * WdwType, Int_t Npoints, Double_t * X, Double_t * Y, const Char_t * WdwDefs);

		inline Bool_t LongParamNamesToBeUsed() const { return(fLongParamNames); };	// long names to guarantee uniqueness?
		inline void UseLongParamNames(Bool_t Flag = kTRUE) { fLongParamNames = Flag; };

		inline void WriteTimeStamp() { fWriteTimeStamp = kTRUE; };	// write a time stamp
		inline Bool_t TimeStampToBeWritten() const { return(fWriteTimeStamp); };

		Bool_t WriteDeadTime(const Char_t * Scaler, Int_t Interval = 1000);				// write a dead time events
		inline Bool_t DeadTimeToBeWritten() const { return(fDeadTimeInterval > 0); }; 	// check if dead time enabled
		inline Int_t GetDeadTimeInterval() const { return(fDeadTimeInterval); };			// get interval
		inline TObject * GetDeadTimeScaler() const { return(fDeadTimeScaler); };			// get scaler def

																					// include user-specific code
		Bool_t IncludeUserCode(const Char_t * IclPath, const Char_t * UserFile, Bool_t AutoGenFlag = kFALSE);
		inline Bool_t IncludeUserCode(const Char_t * UserFile, Bool_t AutoGenFlag = kFALSE) {
			return(this->IncludeUserCode("", UserFile, AutoGenFlag));
		};
		inline Bool_t UserCodeToBeIncluded() const { return(fLofUserIncludes.Last() >= 0); };

		Bool_t IncludeUserLib(const Char_t * IclPath, const Char_t * UserLib);
		inline Bool_t IncludeUserLib(const Char_t * UserLib) {
			return(this->IncludeUserLib("", UserLib));
		}
		inline Bool_t UserLibsToBeIncluded() const { return(fLofUserLibs.Last() >= 0); };

		Bool_t IncludeUserClass(const Char_t * IclPath, const Char_t * UserClass,
								Bool_t UserDefinedEvent = kFALSE);
		inline Bool_t IncludeUserClass(const Char_t * UserClass, Bool_t UserDefinedEvent = kFALSE) {
			return(this->IncludeUserClass("", UserClass, UserDefinedEvent));
		}
		inline Bool_t UserClassesToBeIncluded() const { return(fLofUserClasses.Last() >= 0); };

		inline void AddUserClass(EMrbIncludeOptions Opt, const Char_t * Name, const Char_t * Path = NULL) { 							// add a user class
			if (fLofUserClasses.FindByName(Name) == NULL) fLofUserClasses.AddNamedX((Int_t) Opt, Name, Path);
		};
		inline void AddUserClass(const Char_t * Name) { this->AddUserClass((EMrbIncludeOptions) 0, Name); };
			
		inline TMrbLogger * GetMessageLogger() const { return(fMessageLogger); };
		
		void Version() const;																// output welcome tex

		TMrbConfig * ReadFromFile(const Char_t * ConfigFile = "", Option_t * Options = ""); // read config from root file
		Bool_t WriteToFile(const Char_t * ConfigFile = "", Option_t * Options = "");		// write config objects to root file

		void SetGlobalAddress();							// set global address gMrbConfig

		inline void UseMapFile(Bool_t MapFlag = kTRUE) { fUseMapFile = MapFlag; };

		inline void  SetMultiBorC(TMrbCNAF::EMrbCNAF MultiBorC) { 	// store multi branch/crate status
			fMultiBorC |= MultiBorC;
		};

		inline void SetCrateType(Int_t Crate, EMrbCrateType CrateType) { fCrateTable[Crate] = CrateType; };
		inline EMrbCrateType GetCrateType(Int_t Crate) const { return((EMrbCrateType) fCrateTable[Crate]); };
		Int_t FindCrate(Int_t After = -1) const;												// find next crate
		Int_t GetNofCrates(EMrbCrateType CrateType = kCrateAny) const;															// return number of crates
		UInt_t GetCratePattern(EMrbCrateType CrateType = kCrateAny) const;														// return crate numbers as bit pattern
		
		Bool_t SetControllerType(Int_t Crate, const Char_t * Type);
		Bool_t SetControllerType(const Char_t * Crate, const Char_t * Type);
		inline void SetControllerType(Int_t Crate, EMrbControllerType Type) { fControllerTable[Crate] = Type; };
		inline EMrbControllerType GetControllerType(Int_t Crate) const { return((EMrbControllerType) fControllerTable[Crate]); };
																	// handle trigger patterns
		Bool_t HandleMultipleTriggers(Int_t T1 = 0, Int_t T2 = 0, Int_t T3 = 0, Int_t T4 = 0, Int_t T5 = 0);
		TMrbConfig::EMrbTriggerStatus GetTriggerStatus(Int_t Trigger) const; 		// get trigger status
		void UpdateTriggerTable(Int_t Trigger = 0);		// update trigger table: valid patterns, overlapping, etc.

		void AddToTagList(const Char_t * CodeFile, Int_t TagIndex); 		// add file:tag to be processed once
		Bool_t TagToBeProcessed(const Char_t * CodeFile, Int_t TagIndex) const;	// check if tag has already been processed

		Bool_t BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Xbin, Double_t Xlow, Double_t Xup, const Char_t * Condition = NULL);
		Bool_t BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Xbin, Double_t Xlow, Double_t Xup, const Char_t * Condition = NULL);

		Bool_t BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Xbin, Double_t Xlow, Double_t Xup,
									Int_t Ybin, Double_t Ylow, Double_t Yup, const Char_t * Condition = NULL);
		Bool_t BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Xbin, Double_t Xlow, Double_t Xup,
									Int_t Ybin, Double_t Ylow, Double_t Yup, const Char_t * Condition = NULL);

		Bool_t BookHistogram(const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									const Char_t * Args, const Char_t * Condition);

		Bool_t BookHistogram(const Char_t * ArrayName, const Char_t * HistoType, const Char_t * HistoName, const Char_t * HistoTitle,
									const Char_t * Args, const Char_t * Condition);

		Bool_t BookHistograms(const Char_t * Event, const Char_t * Subevent, const Char_t * Condition = NULL);

		Bool_t BookRateHistogram(const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Scale, Int_t Range, Bool_t Loop = kFALSE, const Char_t * Condition = NULL);
		Bool_t BookRateHistogram(const Char_t * ArrayName, const Char_t * HistoName, const Char_t * HistoTitle,
									Int_t Scale, Int_t Range, Bool_t Loop = kFALSE, const Char_t * Condition = NULL);

		TMrbNamedX * AddHistoToArray(const Char_t * ArrayName, const Char_t * HistoName);
		TMrbNamedX * FindHistoArray(const Char_t * HistoName, TMrbNamedX * After = NULL) const;	// find array histo is to be assigned to

		inline Int_t GetNofModules() const { return(fNofModules); };
		Int_t GetNofModules(const Char_t * Pattern) const;
		
		inline TObjArray * GetLofEvents() { return(&fLofEvents); };		// get address of ...
		inline TObjArray * GetLofSubevents() { return(&fLofSubevents); };
		inline TObjArray * GetLofModules() { return(&fLofModules); };
		inline TObjArray * GetLofScalers(){ return(&fLofScalers); };
		inline TObjArray * GetLofUserHistograms() { return(&fLofUserHistograms); };
		inline TObjArray * GetLofSpecialHits(){ return(&fLofSpecialHits); };
		inline TMrbLofNamedX * GetLofModuleIDs() { return(&fLofModuleIDs); };
		inline TMrbLofNamedX * GetLofModuleTypes() { return(&fLofModuleTypes); };
		inline TMrbLofNamedX * GetLofDataTypes() { return(&fLofDataTypes); };
		inline TMrbLofNamedX * GetLofHistoTypes() { return(&fLofHistoTypes); };
		inline TMrbLofNamedX * GetLofGlobals() { return(&fLofGlobals); };
		inline TMrbLofNamedX * GetLofUserIncludes() { return(&fLofUserIncludes); };
		inline TMrbLofNamedX * GetLofUserClasses() { return(&fLofUserClasses); };
		inline TMrbLofNamedX * GetLofUserLibs() { return(&fLofUserLibs); };

		Bool_t NameNotLegal(const Char_t * ObjType, const Char_t * ObjName) const;	// check if name is legal within MARaBOU

		inline void MakeGlobal(const Char_t * Name, Int_t * IntVar, const Char_t * Comment = "") { fLofGlobals.AddNamedX(new TMrbNamedX(kGlobInt, Name, Comment, (TObject *) IntVar)); };
		inline void MakeGlobal(const Char_t * Name, Float_t * FloatVar, const Char_t * Comment = "") { fLofGlobals.AddNamedX(new TMrbNamedX(kGlobFloat, Name, Comment, (TObject *) FloatVar)); };
		inline void MakeGlobal(const Char_t * Name, Double_t * DblVar, const Char_t * Comment = "") { fLofGlobals.AddNamedX(new TMrbNamedX(kGlobDouble, Name, Comment, (TObject *) DblVar)); };
		inline void MakeGlobal(const Char_t * Name, Bool_t * BoolVar, const Char_t * Comment = "") { fLofGlobals.AddNamedX(new TMrbNamedX(kGlobBool, Name, Comment, (TObject *) BoolVar)); };
		inline void MakeGlobal(const Char_t * Name, TString & Str, const Char_t * Comment = "") { fLofGlobals.AddNamedX(new TMrbNamedX(kGlobString, Name, Comment, (TObject *) &Str)); };

		void MakeGlobal(const Char_t * Name, Int_t IntVal, const Char_t * Comment = "");
		void MakeGlobal(const Char_t * Name, Float_t FloatVal, const Char_t * Comment = "");
		void MakeGlobal(const Char_t * Name, Double_t DblVal, const Char_t * Comment = "");
		void MakeGlobal(const Char_t * Name, Bool_t BoolVal, const Char_t * Comment = "");
		void MakeGlobal(const Char_t * Name, const Char_t * Str, const Char_t * Comment = "");

		Bool_t GetGlobal(const Char_t * Name, Int_t & IntVar) const;
		Bool_t GetGlobal(const Char_t * Name, Float_t & FloatVar) const;
		Bool_t GetGlobal(const Char_t * Name, Double_t & DblVar) const;
		Bool_t GetGlobal(const Char_t * Name, Bool_t & BoolVar) const;
		Bool_t GetGlobal(const Char_t * Name, TString & Str) const;
		Int_t GetGlobI(const Char_t * Name) const;
		Float_t GetGlobF(const Char_t * Name) const;
		Double_t GetGlobD(const Char_t * Name) const;
		Bool_t GetGlobB(const Char_t * Name) const;
		const Char_t * GetGlobStr(const Char_t * Name) const;

		void MakeDefined(const Char_t * Name, Int_t Value, const Char_t * Comment = "");
		void MakeDefined(const Char_t * Name, Bool_t Defined = kTRUE, const Char_t * Comment = "");
		Bool_t IsDefined(const Char_t * Name, Int_t & Value) const;
		Bool_t IsDefined(const Char_t * Name) const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	public: 									// public lists of key words:
		TMrbLofNamedX fLofDataTypes;			//! ... data types
		TMrbLofNamedX fLofCrateTypes;			//! ... crate types
		TMrbLofNamedX fLofControllerTypes;		//! ... camac controllers
		TMrbLofNamedX fLofModuleTypes;			//! ... module types
		TMrbLofNamedX fLofReadoutTags;			//! ... readout tags
		TMrbLofNamedX fLofAnalyzeTags;			//! ... analyze tags
		TMrbLofNamedX fLofConfigTags;			//! ... config tags
		TMrbLofNamedX fLofRcFileTags;			//! ... rc file tags
		TMrbLofNamedX fLofUserMacroTags;		//! ... user macro tags
		TMrbLofNamedX fLofUserEventTags;		//! ... user event tags
		TMrbLofNamedX fCNAFNames;				//! ... cnaf key words
		TMrbLofNamedX fLofModuleTags;			//! ... camac tags
		TMrbLofNamedX fLofModuleIDs;			//! ... camac modules available
		TMrbLofNamedX fLofHistoTypes;			//! ... histogram types
		TMrbLofNamedX fLofGlobals;				//! list of global vars
		TMrbLofNamedX fLofDefines;				//! list of #define statements

	protected:
		Bool_t CreateUserMacro();
		Bool_t CreateUserEvent(ofstream & OutStrm, const Char_t * UserEvent, Bool_t CreateProto, Bool_t SystemPart);
		Bool_t DefineVarOrWdw(TMrbNamedX * VarType, TObject * VarProto, const Char_t * VarDefs);
		Bool_t WriteUtilityProtos();
		Bool_t CheckConfig();
		
	protected:
		Bool_t fVerboseMode;				// verbose flag
		TMrbLogger * fMessageLogger;		//! addr of message logger
			
		Int_t fNofEvents;					// list of events
		TObjArray fLofEvents;

		TArrayI fCrateTable;				// crate table (camac and VME)
		TArrayI fControllerTable;			// controller table (camac only)

		Int_t fNofSubevents;				// list of subevents
		TObjArray fLofSubevents;

		Int_t fNofModules;					// list of modules
		TObjArray fLofModules;

		Bool_t fLongParamNames; 			// use long param names: i.e. <subevent><param>

		Bool_t fUseMapFile; 				// kTRUE if TMap should be used

		Int_t fNofScalers;					// list of scalers
		TObjArray fLofScalers;

		UInt_t fMultiBorC;					// multi branch? multi crate?

		UInt_t fReadoutOptions; 			// options used in MakeReadoutCode()
		UInt_t fAnalyzeOptions; 			// ... in MakeAnalyzeCode()
		UInt_t fConfigOptions;  			// ... in MakeConfigCode()
		UInt_t fRcFileOptions;  			// ... in MakeRcFile()

		TMrbLofNamedX fLofUserIncludes;		// list of user-specific files to be included
		TMrbLofNamedX fLofUserLibs; 		// list of user-specific libraries to be included
		TMrbLofNamedX fLofUserClasses; 		// list of classes added by user

		Bool_t fWriteTimeStamp; 			// kTRUE if user wants a time stamp to be added to each event

		Int_t fDeadTimeInterval;			// number of events a new dead time event will be created
		TObject * fDeadTimeScaler;	 		// dead-time scaler

		TString fCreationDate;				// creation date & time
		TString fUser;						// user name
		TString fAuthor;

		UInt_t fTriggerMask;				// trigger bits defined so far
		Bool_t fSingleBitTriggersOnly;		// triggers 1,2,4, or 8 only
		TArrayI fTriggersToBeHandled;		// trigger patterns to be handled

		Int_t fSevtSize;					// subevent size to be used in .mbssetup

		Bool_t fUserMacroToBeCalled;		// call user macro
		TString fUserMacro; 				// macro name
		TMrbString fUserMacroCmd;				// ... command
				
		TObjArray fLofOnceOnlyTags; 		// list of tags already processed
		TObjArray fLofUserHistograms;		// list of user-defined histograms
		TObjArray fLofHistoArrays;			// list of histogram arrays
		TObjArray fLofHistoConditions;		// list of histogram booking conds
		TObjArray fLofSpecialHits;			// list of special hit objects
		
		Bool_t fConfigChecked;				// kTRUE if consistency check done
		Bool_t fConfigOk;					// kTRUE config consistent

	ClassDef(TMrbConfig, 1) 	// [Config] Base class to describe an experimental setup in MARaBOU
};	

#endif
