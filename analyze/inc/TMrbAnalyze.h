#ifndef __TMrbAnalyze_h__
#define __TMrbAnalyze_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze.h
// Purpose:        Base class for user's analyze process
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbAnalyze.h,v 1.51 2007-08-08 07:39:00 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TH1.h"
#include "TList.h"
#include "TString.h"
#include "TMapFile.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"

// we include all of the .h files here, so user may include this file only in his code
#include "TMrbLofUserVars.h"
#include "TMrbHistoListEntry.h"
#include "TMrbParamListEntry.h"
#include "TMrbModuleListEntry.h"
#include "TMrbCalibrationListEntry.h"
#include "TMrbDCorrListEntry.h"
#include "TMrbIOSpec.h"
#include "TUsrDeadTime.h"
#include "TUsrEvent.h"
#include "TUsrEvtStart.h"
#include "TUsrEvtStop.h"
#include "TUsrHBX.h"
#include "TUsrHitBuffer.h"
#include "TUsrHit.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze
// Purpose:        Main class to define user's analyze methods
// Description:    Defines methods to perform user's analysis.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbAnalyze : public TObject {

	public:
		enum				{	kMB 			=	(1024 * 1024)	};

		enum 				{	kTimeUnitsPerSec	=	10000		};
		enum				{	kTimeNsecsPerUnit	=	100000		};

		enum				{	kBranchSplitLevel	=	99			};
		enum				{	kBranchBufferSize	=	32000		};

		enum				{	kUpdateTime 	=	8				};

		enum				{	kMaxPolyDegree	=	5				};

		enum EMrbRunStatus	{	M_ABSENT,
								M_STARTING,
								M_RUNNING,
								M_PAUSING,
								M_STOPPING,
								M_STOPPED
							};

	public:
		TMrbAnalyze() {};										// default ctor
		TMrbAnalyze(TMrbIOSpec * IOSpec);						// ctor
		~TMrbAnalyze() { fLofIOSpecs.Delete(); };				// dtor

		TMrbAnalyze(const TMrbAnalyze &) {};					// dummy copy ctor

		void SetScaleDown(Int_t ScaleDown = 1);					// set (global) scale-down factor
		TH1F * UpdateRateHistory();  							// add a bin to the rate history
		TH1F * UpdateDTimeHistory();  							// update the dead-time history
		Bool_t TestRunStatus(); 								// wait for run flag
		Bool_t Initialize(TMrbIOSpec * IOSpec);					// init objects here
		Bool_t InitializeUserCode(TMrbIOSpec * IOSpec, Bool_t BeforeHB = kFALSE);	// user may init additional objects here
		Bool_t AdjustWindowPointers();							// adjust wdw pointers after reload

		Bool_t HandleMessage(const Char_t * ArgList); 			// handle messages to M_analyze

		Bool_t OpenRootFile(const Char_t * FileName);			// open input file
		Bool_t OpenRootFile(TMrbIOSpec * IOSpec);

		Bool_t ReplayEvents(TMrbIOSpec * IOSpec);				// read events from root tree

		Bool_t WriteRootTree(const Char_t * FileName); 			// open ROOT file to store raw data
		Bool_t WriteRootTree(TMrbIOSpec * IOSpec);

		inline void SetFileSize(Int_t FileSize = 0) {			// set size of output file to approx. FileSize MBs
			fFileSize = kMB * FileSize;
		};

		Bool_t CloseRootTree(TMrbIOSpec * IOSpec = NULL); 		// close output file

		inline Int_t GetScaleDown() const { return(fScaleDown); };	// get current scale down
		inline void SetRunId(Int_t RunId) { fRunId = RunId; };	// define run id
		inline Int_t GetRunId() const { return(fRunId); };				// get current run id
		inline void SetBranchSize(Int_t BranchSize = TMrbAnalyze::kBranchBufferSize) { fBranchSize = BranchSize; };	// set buffer size for branches
		inline void SetSplitLevel(Bool_t SplitLevel = TMrbAnalyze::kBranchSplitLevel) { fSplitLevel = SplitLevel; };	// define split level
		inline Int_t GetBranchSize() const { return(fBranchSize); }; 	// get current buffer size
		inline Int_t GetSplitLevel() const { return(fSplitLevel); };		//get split mode
		inline void SetRunStatus(EMrbRunStatus RunStatus) { fRunStatus = RunStatus; };	// set status flag
		inline EMrbRunStatus GetRunStatus() const { return(fRunStatus); };	// get current status
		inline Int_t IsToBeUpdated() { 				// test and reset update flag
			Bool_t u = fUpdateFlag;
			fUpdateFlag = 0;
			return(u);
		};
		inline void SetUpdateFlag() { fUpdateFlag++; };		// set update flag
		inline Int_t GetEventsProcessed() const { return(fEventsProcessed); };	// get current event count
		inline void SetReplayMode(Bool_t Flag = kTRUE) { fReplayMode = Flag; }; // set replay mode
		inline Bool_t IsReplayMode() const { return(fReplayMode); }; 			// get current replay mode
		inline void IncrEventCount() { fEventsProcessed++; };				// increment event count
		inline void ResetEventCount() { 									// reset event count
			fEventsProcessed = 0;
			fEventsProcPrev = 0;
		};
		inline Bool_t IsModScaleDown() const { return((fEventsProcessed % fScaleDown) == 0); };

		inline Bool_t TreeToBeWritten() const { return(fWriteRootTree); };		// root data to be written?

		void MarkHistogramsWithTime();								// write a time stamp to all histograms

		Int_t OpenFileList(TString &, TMrbIOSpec *);				// open a list of files for replay
		Int_t ProcessFileList();									// process entries in fLofIOSpecs

		Bool_t ReloadParams(const Char_t * ParamFile);				// reload params from file
		Bool_t ReloadParams(TMrbIOSpec * IOSpec);
		Bool_t ReloadVarsAndWdws(const Char_t * ParamFile); 		// reload vars and wdws
		Bool_t ReloadVarsAndWdws(TMrbIOSpec * IOSpec);

		Bool_t FinishRun(TMrbIOSpec * IOSpec, Bool_t BeforeSH = kTRUE);	// do some work at end of run

		inline void AddIOSpec(TMrbIOSpec * IOSpec) { fLofIOSpecs.Add(IOSpec); };	// add an i/o spec to list
		inline TMrbIOSpec * GetNextIOSpec(TMrbIOSpec * IOSpec) {					// get (next) i/o spec
			if (IOSpec == NULL) return((TMrbIOSpec *) fLofIOSpecs.First());
			else				return((TMrbIOSpec *) fLofIOSpecs.After(IOSpec));
		};

		Int_t SaveHistograms(const Char_t * Pattern, const Char_t * HistFile);		// save mmap data to histo file
		Int_t SaveHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec);

		Int_t ClearHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec = NULL);	// clear histos in shared memory

		inline void SetCurIOSpec(TMrbIOSpec * IOSpec) { fCurIOSpec = IOSpec; };
		inline TMrbIOSpec * GetCurIOSpec() { return(fCurIOSpec); };
		inline TList * GetLofIOSpecs() { return(&fLofIOSpecs); };

		inline void SetMapFile(TMapFile * MapFile, Int_t Size) { 		// store mmap addr & size
			fMapFile = MapFile;
			fMapFileSize = Size;
		};
		Int_t GetSizeOfMappedObjects(TMapFile * MapFile) const;			// check size of objects in map file

																	// bookkeeping: manage lists of modules, params, and histos
		void InitializeLists(Int_t NofModules, Int_t NofParams);	// init lists
		const Char_t * GetModuleName(Int_t ModuleIndex) const;			// get module name by index
		const Char_t * GetModuleTitle(Int_t ModuleIndex) const;			// get module title by index
		Int_t GetModuleIndex(const Char_t * ModuleName) const;			// get module index by name
		const Char_t * GetParamName(Int_t ModuleIndex, Int_t RelParamIndex) const;	// get param name by relative index
		const Char_t * GetParamName(Int_t AbsParamIndex) const;			// get param name by absolute index
		Int_t GetParamIndex(const Char_t * ParamName, Bool_t AbsFlag = kTRUE) const;	// get param index by name (rel or abs)
		Int_t GetParamIndex(Int_t ModuleIndex, Int_t RelParamIndex) const;			// get absolute param index
		TH1 * GetHistoAddr(const Char_t * HistoName) const;				// get histogram addr by name
		TH1 * GetHistoAddr(Int_t ModuleIndex, Int_t RelParamIndex) const; // get histogram addr by relative param index
		TH1 * GetHistoAddr(Int_t AbsParamIndex) const;					// get histogram addr by absolute param index
		TH1 * GetHistoFromList(TObjArray & HistoList, Int_t ModuleNumber, Int_t RelParamIndex) const;	// take histo from list
		TH1 * GetHistoFromList(TObjArray & HistoList, Int_t AbsParamIndex) const;	// take histo from list
		TMrbHistoListEntry * GetHistoListEntry(const Char_t * HistoName) const;	// get entry in histo list by name
		TObject * GetParamAddr(const Char_t * ParamName) const; 			// get param addr by name
		TObject * GetParamAddr(Int_t ModuleIndex, Int_t RelParamIndex) const; // get param addr by relative param index
		TObject * GetParamAddr(Int_t AbsParamIndex) const;					// get param addr by absolute param index
		Bool_t AddModuleToList(const Char_t * ModuleName, const Char_t * ModuleTitle,
												Int_t ModuleIndex, Int_t AbsParamIndex,
												Int_t NofParams, Int_t TimeOffset = 0);
		Bool_t AddParamToList(const Char_t * ParamName, TObject * ParamAddr, Int_t ModuleIndex, Int_t RelParamIndex);
		Bool_t AddHistoToList(TH1 * HistoAddr, Int_t ModuleIndex, Int_t RelParamIndex);

		TMrbModuleListEntry * GetModuleListEntry(Int_t ModuleIndex) const;
		TMrbModuleListEntry * GetModuleListEntry(const Char_t * ModuleName) const;

		void ResetModuleHits(Int_t StartIndex = 0, Int_t StopIndex = 0);

		Int_t ReadCalibrationFromFile(const Char_t * CalibrationFile);		// read calibration data from file
		Bool_t AddCalibrationToList(TF1 * CalibrationAddr, Int_t ModuleIndex, Int_t RelParamIndex); // add calibration
		Bool_t AddCalibrationToList(TF1 * CalibrationAddr, Int_t AbsParamIndex);
		TF1 * AddCalibrationToList(const Char_t * Name, const Char_t * Formula, Double_t Xmin, Double_t Xmax, Int_t ModuleIndex, Int_t RelParamIndex); // add calibration
		TF1 * GetCalibration(const Char_t * CalibrationName) const; 				// get calibration by name
		TF1 * GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex) const;		// get calibration by module + param
		TF1 * GetCalibration(Int_t AbsParamIndex) const;							// get calibration by absolute param index
		TF1 * GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Gain, Double_t & Offset) const;
		TF1 * GetCalibration(Int_t AbsParamIndex, Double_t & Gain, Double_t & Offset) const;
		TF1 * GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex, TArrayD & Coeffs) const;
		TF1 * GetCalibration(Int_t AbsParamIndex, TArrayD & Coeffs) const;

		TMrbCalibrationListEntry * GetCalibrationListEntry(Int_t ModuleIndex, Int_t RelParamIndex) const;
		TMrbCalibrationListEntry * GetCalibrationListEntry(Int_t AbsParamIndex) const;

		void PrintCalibration(ostream & Out) const;
		void PrintCalibration(ostream & Out, Int_t ModuleIndex, Int_t RelParamIndex) const;
		void PrintCalibration(ostream & Out, Int_t AbsParamIndex) const;
		void PrintCalibration(ostream & Out, const Char_t * CalibrationName) const;
		void PrintCalibration(ostream & Out, TMrbNamedX * CalX) const;
		inline void PrintCalibration() const { this->PrintCalibration(cout); };
		inline void PrintCalibration(Int_t ModuleIndex, Int_t RelParamIndex) const { this->PrintCalibration(cout, ModuleIndex, RelParamIndex); };
		inline void PrintCalibration(Int_t AbsParamIndex) const { this->PrintCalibration(cout, AbsParamIndex); };
		inline void PrintCalibration(const Char_t * CalibrationName) const { this->PrintCalibration(cout, CalibrationName); };
	
		Int_t ReadDCorrFromFile(const Char_t * DCorrFile);		// read calibration data from file
		Bool_t AddDCorrToList(TF1 * DCorrAddr, Int_t ModuleIndex, Int_t RelParamIndex); // add calibration
		Bool_t AddDCorrToList(TF1 * DCorrAddr, Int_t AbsParamIndex);
		TF1 * AddDCorrToList(const Char_t * Name, const Char_t * Formula, Double_t Xmin, Double_t Xmax, Int_t ModuleIndex, Int_t RelParamIndex); // add calibration
		TF1 * GetDCorr(const Char_t * DCorrName) const; 				// get calibration by name
		TF1 * GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex) const;		// get calibration by module + param
		TF1 * GetDCorr(Int_t AbsParamIndex) const;							// get calibration by absolute param index
		TF1 * GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Factor) const;
		TF1 * GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Beta, Double_t & Angle) const;
		TF1 * GetDCorr(Int_t AbsParamIndex, Double_t & Factor) const;
		TF1 * GetDCorr(Int_t AbsParamIndex, Double_t & Beta, Double_t & Angle) const;

		TMrbDCorrListEntry * GetDCorrListEntry(Int_t ModuleIndex, Int_t RelParamIndex) const;
		TMrbDCorrListEntry * GetDCorrListEntry(Int_t AbsParamIndex) const;

		inline Int_t GetNofModules() const { return(fNofModules); };		// number of modules
		inline Int_t GetNofParams() const { return(fNofParams); };		// number of params

		Bool_t SetTimeOffset(Int_t ModuleNumber, Int_t Offset); 	// set time offset
		Int_t GetTimeOffset(Int_t ModuleNumber) const;					// get time offset

		inline void SetDumpCount(Int_t Count) { fDumpCount = Count; };
		inline Int_t GetDumpCount() const { return(fDumpCount); };
		Bool_t DumpData(const Char_t * Prefix, Int_t Index, const Char_t * CallingClass, const Char_t * CallingMethod,
															const Char_t * Msg, const UShort_t * DataPtr, Int_t DataWC);
		
		Bool_t AddResourcesFromFile(const Char_t * ResourceFile);	// add user's resource defs to gEnv
		const Char_t * GetResource(const Char_t * Resource);		// make up full resource name
				
		inline void SetVerboseMode(Bool_t VerboseFlag = kTRUE) { fVerboseMode = VerboseFlag; };
		inline Bool_t IsVerbose() const { return(fVerboseMode); };

		inline void SetFakeMode(Bool_t FakeMode = kTRUE) { fFakeMode = FakeMode; }; 	// fake mode
		inline Bool_t IsFakeMode() const { return(fFakeMode); };

		inline TMrbNamedX * FindModule(const Char_t * ModuleName) const { return(fModuleList.FindByName(ModuleName)); };
		inline TMrbNamedX * FindParam(const Char_t * ParamName) const { return(fParamList.FindByName(ParamName)); };
		inline TMrbNamedX * FindHisto(const Char_t * HistoName, Bool_t SingleFlag = kFALSE) const {
			return(SingleFlag ? fSingleList.FindByName(HistoName) : fHistoList.FindByName(HistoName));
		};
		
		void PrintLists(ostream & out = cout) const;				// print modules, params, histos ...
		void PrintLists(const Char_t * FileName) const;

		void PrintStartStop(TUsrEvtStart * Start = NULL, TUsrEvtStop * StopEvent = NULL) const;		// output start/stop time stamps

		inline TMrbLogger * GetMessageLogger() const { return(fMessageLogger); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t fVerboseMode;		// kTRUE if verbose mode
		TMrbLogger * fMessageLogger; //! addr of message logger
			
		Bool_t fWriteRootTree;		// kTRUE if root data are to be written
		TFile * fRootFileOut;		// root file to store data trees
		Int_t fFileSize;			// file size in MB

		Bool_t fReplayMode; 		// kTRUE if replay mode
		TFile * fRootFileIn;		// root file to read tree data from (replay mode only)

		Bool_t fFakeMode; 			// kTRUE if fake mode

		Int_t fDumpCount;			// number of data records to be dumped: 0 -> never, N -> N times, -1 -> always 
		
		Int_t fNofModules;			// number of modules
		Int_t fNofParams;			// number of parameters
		Int_t fScaleDown;			// global scale-down factor
		Int_t fRunId;				// current run id
		Int_t fBranchSize;  		// buffer size for branches
		Int_t fSplitLevel;			// split mode
		EMrbRunStatus fRunStatus;	// kTRUE: program is to be terminated (after ^C)
		Int_t fUpdateFlag; 			// > 0: mmap objects are to be updated

		Int_t fEventsProcessed; 	// events processed so far
		Int_t fTimeOfLastUpdate;	// update time is stored here
		Int_t fEventsProcPrev; 		// event count of last update
		Int_t fHistFileVersion;    // version number of (autosaved) hist file

		TMapFile * fMapFile;		// shared memory mmap file
		Int_t fMapFileSize;			// ... size

		TMrbIOSpec * fCurIOSpec;	// current i/o spec
		TList fLofIOSpecs;			// list of i/o specs
		
		TString fResourceFile;		// user's resource defs
		TString fResourceName;
		TString fResourceString;

		TMrbLofNamedX fModuleList;		// list of modules, indexed by serial number
		TMrbLofNamedX fParamList;		// list of params, indexed by param number
		TMrbLofNamedX fHistoList;		// list of histograms, indexed by param number
		TMrbLofNamedX fSingleList;		// list of single histograms, indexed by param number
		TMrbLofNamedX fCalibrationList;	// list of calibrations, indexed by param number
		TMrbLofNamedX fDCorrList;		// list of doppler corrections, indexed by param number

	ClassDef(TMrbAnalyze, 1)	// [Analyze] Describe user's analysis
};

#endif
