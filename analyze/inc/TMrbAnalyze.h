#ifndef __TMrbAnalyze_h__
#define __TMrbAnalyze_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze.h
// Purpose:        Define base class for user's analyze process
// Description:
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TDatime.h"
#include "TMapFile.h"
#include "TArrayI.h"

#include "TMrbTransport.h"
#include "TMrbVar.h"
#include "TMrbWdw.h"
#include "TMrbLofUserVars.h"
#include "TMrbVarWdwCommon.h"

#include "TMrbLogger.h"

enum				{	kMB 			=	(1024 * 1024)	};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec
// Purpose:        Specify i/o data
// Description:    Describes how to process input data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbIOSpec : public TObject {

	public:
		enum EMrbInputMode		{	kInputNone			=	0,
											kInputMBS			=	0x1,
											kInputFile			=	0x2,
											kInputTCP			=	kInputMBS | 0x4,
											kInputRoot			=	kInputFile | 0x8,
											kInputLMD			=	kInputMBS | kInputFile | 0x10,
											kInputMED			=	kInputMBS | kInputFile | 0x20,
											kInputList			=	kInputRoot | 0x40,
											kInputSync			=	kInputTCP | 0x80,
											kInputAsync 		=	kInputTCP | 0x100
										};

		enum EMrbOutputMode 	{	kOutputNone 				=	0,
											kOutputMBS					=	0x1,
											kOutputOpen 				=	0x2,
											kOutputWriteRootTree		=	0x4,
											kOutputClose				=	0x8,
											kOutputWriteLMDFormat	=	kOutputMBS | 0x10,
											kOutputWriteMEDFormat	=	kOutputMBS | 0x20
										};

		enum EMrbHistoMode	 	{	kHistoNone			=	0,
											kHistoAdd			=	0x1,
											kHistoSave			=	0x2,
											kHistoClear			=	0x4,
										};

		enum EMrbParamMode		{	kParamNone			=	0,
											kParamReload		=	0x1,
											kParamReloadAscii	=	kParamReload | 0x2,
											kParamLoaded		=	0x4
										};

	public:
		TMrbIOSpec() {								// default ctor
			fTimeStampFlag = kFALSE;
			fInputMode = TMrbIOSpec::kInputNone;
			fOutputMode = TMrbIOSpec::kOutputNone;
			fParamMode = TMrbIOSpec::kParamNone;
			fHistoMode = TMrbIOSpec::kHistoNone;
		};

		~TMrbIOSpec() {};							// default dtor

		void SetInputFile(const Char_t * InputFile, EMrbInputMode InputMode = TMrbIOSpec::kInputNone) {
			fInputFile = InputFile;
			fInputMode = InputMode;
		};
		inline const Char_t * GetInputFile() const { return(fInputFile.Data()); };

		void SetOutputFile(const Char_t * OutputFile, EMrbOutputMode OutputMode = TMrbIOSpec::kOutputNone) {
			fOutputFile = OutputFile;
			fOutputMode = OutputMode;
		};
		inline const Char_t * GetOutputFile() const { return(fOutputFile.Data()); };

		void SetHistoFile(const Char_t * HistoFile, EMrbHistoMode HistoMode = TMrbIOSpec::kHistoNone) {
			fHistoFile = HistoFile;
			fHistoMode = HistoMode;
		};
		inline const Char_t * GetHistoFile() const { return(fHistoFile.Data()); };

		void SetParamFile(const Char_t * ParamFile, EMrbParamMode ParamMode = TMrbIOSpec::kParamNone) {
			fParamFile = ParamFile;
			fParamMode = ParamMode;
		};
		inline const Char_t * GetParamFile() const { return(fParamFile.Data()); };

		Bool_t SetStartStop(TString &, TString &);
		Bool_t SetStartStop(Bool_t TstampFlag, Int_t Start, Int_t Stop) {
			fTimeStampFlag = TstampFlag;
			fStartEvent = Start;
			fStopEvent = Stop;
			return(kTRUE);
		};

		inline Int_t GetStartEvent() const { return(fStartEvent); };
		inline Int_t GetStopEvent() const { return(fStopEvent); };

		Bool_t CheckStartStop(TString &, Int_t &, Bool_t &);
		void ConvertToTimeStamp(TString &, Int_t) const;

		inline Bool_t IsTimeStampMode() const { return(fTimeStampFlag); };
		inline EMrbInputMode GetInputMode() const { return(fInputMode); };
		inline void SetInputMode(TMrbIOSpec::EMrbInputMode Mode) { fInputMode = Mode; };
		inline EMrbOutputMode GetOutputMode() const { return(fOutputMode); };
		inline void SetOutputMode(TMrbIOSpec::EMrbOutputMode Mode) { fOutputMode = Mode; };
		inline EMrbParamMode GetParamMode() const { return(fParamMode); };
		inline void SetParamMode(TMrbIOSpec::EMrbParamMode Mode) { fParamMode = Mode; };
		inline EMrbHistoMode GetHistoMode() const { return(fHistoMode); };
		inline void SetHistoMode(TMrbIOSpec::EMrbHistoMode Mode) { fHistoMode = Mode; };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & out = cout) const;				// output current settings

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbIOSpec.html&"); };

	protected:
		Bool_t fTimeStampFlag;		// kTRUE if start/stop refers to time stamp rather than event count
		Int_t fStartEvent;  		// event number / time stamp to start with
		Int_t fStopEvent;			// event number / time stamp to end with

		EMrbInputMode fInputMode;	// MBS? TCP? File? List?
		TString fInputFile;			// name of input file

		EMrbOutputMode fOutputMode; // open? close? write tree?
		TString fOutputFile;		// name of output file

		EMrbHistoMode fHistoMode; 	// add? save?
		TString fHistoFile; 		// name of histogram file

		EMrbParamMode fParamMode;	// reload?
		TString fParamFile; 		// name of param file

	ClassDef(TMrbIOSpec, 1) 	// [Analyze] I/O specifications
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCalibrationListEntry
// Purpose:        An entry in user's list of calibration functions
// Description:    Bookkeeping: Connects calibrations to modules and params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbCalibrationListEntry : public TObject {

	public:
		TMrbCalibrationListEntry(	TMrbNamedX * Module = NULL,								// ctor
									TMrbNamedX * Param = NULL,
									TF1 * Address = NULL) : fModule(Module),
															fParam(Param),
															fAddress(Address) {};
		virtual ~TMrbCalibrationListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TMrbNamedX * GetParam() const { return(fParam); };
		inline void SetParam(TMrbNamedX * Param) { fParam = Param; };
		inline TF1 * GetAddress() const { return(fAddress); };
		inline void SetAddress(TF1 * Address) { fAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TMrbNamedX * fParam;
		TF1 * fAddress;

	ClassDef(TMrbCalibrationListEntry, 0) 	// [Analyze] List of calibration functions
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbParamListEntry
// Purpose:        An entry in user's param list
// Description:    Bookkeeping: Connects params to modules and histos
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbParamListEntry : public TObject {

	public:
		TMrbParamListEntry( TMrbNamedX * Module = NULL,								// ctor
							TObject * Address = NULL,
							TH1 * HistoAddress = NULL,
							TH1 * SingleAddress = NULL) :	fModule(Module),
															fAddress(Address),
															fHistoAddress(HistoAddress),
															fSingleAddress(SingleAddress) {};
		virtual ~TMrbParamListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TObject * GetAddress() const { return(fAddress); };
		inline void SetAddress(TObject * Address) { fAddress = Address; };
		inline TH1 * GetHistoAddress(Bool_t SingleFlag = kFALSE) const {
			return(SingleFlag ? fSingleAddress : fHistoAddress);
		};
		inline void SetHistoAddress(TH1 * Address, Bool_t SingleFlag = kFALSE) {
			if (SingleFlag) fSingleAddress = Address;
			else			fHistoAddress = Address;
		};
		inline TF1 * GetCalibrationAddress() const { return(fCalibrationAddress); };
		inline void SetCalibrationAddress(TF1 * Address) { fCalibrationAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TObject * fAddress;
		TH1 * fHistoAddress;
		TH1 * fSingleAddress;
		TF1 * fCalibrationAddress;

	ClassDef(TMrbParamListEntry, 0) 	// [Analyze] List of params
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistoListEntry
// Purpose:        An entry in user's list of histograms
// Description:    Bookkeeping: Connects histograms to modules and params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbHistoListEntry : public TObject {

	public:
		TMrbHistoListEntry( TMrbNamedX * Module = NULL,								// ctor
							TMrbNamedX * Param = NULL,
							TH1 * Address = NULL) :	fModule(Module),
													fParam(Param),
													fAddress(Address) {};
		virtual ~TMrbHistoListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TMrbNamedX * GetParam() const { return(fParam); };
		inline void SetParam(TMrbNamedX * Param) { fParam = Param; };
		inline TH1 * GetAddress() const { return(fAddress); };
		inline void SetAddress(TH1 * Address) { fAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TMrbNamedX * fParam;
		TH1 * fAddress;

	ClassDef(TMrbHistoListEntry, 0) 	// [Analyze] List of histograms
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry
// Purpose:        An entry in user's module list
// Description:    Bookkeeping: Connects modules to params & histos
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbModuleListEntry : public TObject {

	public:
		TMrbModuleListEntry(	Int_t NofParams = 0,						// ctor
								Int_t IndexOfFirstParam = 0,
								Int_t TimeOffset = 0,
								TMrbNamedX * FirstParam = NULL,
								TMrbNamedX * FirstHisto = NULL,
								TMrbNamedX * FirstSingle = NULL) :
										fNofParams(NofParams),
										fIndexOfFirstParam(IndexOfFirstParam),
										fTimeOffset(TimeOffset),
										fFirstParam(FirstParam),
										fFirstHisto(FirstHisto),
										fFirstSingle(FirstSingle) {
											fNofEventHits = 0;
											fNofChannelHits.Set(NofParams);
											fNofChannelHits.Reset();
										};

		virtual ~TMrbModuleListEntry() {}; 									// dtor

		inline Int_t GetNofParams() const { return(fNofParams); };
		inline void SetNofParams(Int_t NofParams) { fNofParams = NofParams; };
		inline Int_t GetTimeOffset() const { return(fTimeOffset); };
		inline void SetTimeOffset(Int_t TimeOffset) { fTimeOffset = TimeOffset; };
		inline Int_t GetIndexOfFirstParam() const { return(fIndexOfFirstParam); };
		inline void SetIndexOfFirstParam(Int_t FirstParam) { fIndexOfFirstParam = FirstParam; };
		inline TMrbNamedX * GetFirstParam() const { return(fFirstParam); };
		inline void SetFirstParam(TMrbNamedX * FirstParam) { fFirstParam = FirstParam; };
		inline TMrbNamedX * GetFirstHisto(Bool_t SingleFlag = kFALSE) const {
			return(SingleFlag ? fFirstSingle : fFirstHisto);
		};
		inline void SetFirstHisto(TMrbNamedX * FirstHisto, Bool_t SingleFlag = kFALSE) {
			if (SingleFlag) 	fFirstSingle = FirstHisto;
			else				fFirstHisto = FirstHisto;
		};

		void ResetChannelHits(Int_t Channel = -1);
		inline void ResetEventHits() { fNofEventHits = 0; };
		inline void ResetAllHits() { fNofEventHits = 0; fNofChannelHits.Reset(); };
		inline void IncrEventHits() { fNofEventHits++; };
		void IncrChannelHits(Int_t Channel);
		inline Int_t GetEventHits() { return(fNofEventHits); };
		Int_t GetChannelHits(Int_t Channel);

	protected:
		Int_t fNofParams;
		Int_t fIndexOfFirstParam;
		Int_t fTimeOffset;
		Int_t fNofEventHits;
		TArrayI fNofChannelHits;
		TMrbNamedX * fFirstParam;
		TMrbNamedX * fFirstHisto;
		TMrbNamedX * fFirstSingle;

	ClassDef(TMrbModuleListEntry, 0) 	// [Analyze] List of modules
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Class to store hit data
// Description:    Describes a single hit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrHit : public TObject {

	public:
		enum	{	kHitFastTrigger	=	0	};				// hit data layout
		enum	{	kHitEnergy		=	1	};				// depending on param RUNTASK
		enum	{	kHitXiaPSA		=	2	};
		enum	{	kHitUserPSA		=	3	};
		enum	{	kHitGSLTHi		=	4	};
		enum	{	kHitGSLTMi		=	5	};
		enum	{	kHitGSLTLo		=	6	};
		enum	{	kHitRaw 		=	7	};
		enum	{	kHitLast		=	kHitRaw	};
		enum	{	kMaxHitData 	=	kHitLast + 1	};

		enum	{	kHitPSA1		=	2	};					// martin lauer's PSA array
		enum	{	kHitPSA_T0		=	kHitPSA1	};
		enum	{	kHitPSA2		=	3	};
		enum	{	kHitPSA_Tslope	=	kHitPSA2	};
		enum	{	kHitPSA3		=	4	};
		enum	{	kHitPSA_Qmax	=	kHitPSA3	};
		enum	{	kHitPSA4		=	5	};
		enum	{	kHitPSA_Tqmax	=	kHitPSA4	};
		enum	{	kHitPSA5		=	6	};
		enum	{	kHitPSA_Error	=	kHitPSA5	};
		enum	{	kHitPSA6		=	7	};
		enum	{	kHitPSA_T90		=	kHitPSA6	};

public:
		TUsrHit() {};
		TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData);	// ctor
		TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData);	// ctor
		~TUsrHit() {};														// default dtor
		
		virtual Int_t Compare(const TObject * Hit) const; 				// hits have to be sortable in time
		Int_t Compare(long long ChannelTime) const;
		
		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, Bool_t PrintNames = kFALSE) const;		// print hit data
		inline void Print(Bool_t PrintNames = kFALSE) const { Print(cout, PrintNames); };
		
		inline Int_t GetBufferNumber() const { return(fBufferNumber); };		// get data members
		inline Int_t GetEventNumber() const { return(fEventNumber); };
		inline Int_t GetModuleNumber() const { return(fModuleNumber); };
		inline Int_t GetChannel() const { return(fChannel); };

		inline long long GetChannelTime() const { return(fChannelTime); };		// get event time
		const Char_t * ChannelTime2Ascii(TString & TimeString) const;

		inline UShort_t * GetDataAddr() { return(fData); };
		inline UShort_t GetData(Int_t Index) const { return(fData[Index]); };

		inline UShort_t GetEnergy() const { return(fData[kHitEnergy]); };
		inline UShort_t GetFastTrigger() const { return(fData[kHitFastTrigger]); };
		Double_t GetCalEnergy(Bool_t Randomize = kTRUE) const;

		virtual inline Bool_t IsSortable() const { return(kTRUE); };	// hit may be sorted by time stamp
				
		inline void SetBufferNumber(Int_t BufferNumber) { fBufferNumber = BufferNumber; };		// set data members
		inline void SetEventNumber(Int_t EventNumber) { fEventNumber = EventNumber; };
		inline void SetModuleNumber(Int_t ModuleNumber) { fModuleNumber = ModuleNumber; };
		inline void SetChannel(Int_t Channel) { fChannel = Channel; };
		inline long long SetChannelTime(UShort_t * ChannelTime) {
			fChannelTime = 0;
			for (Int_t i = 0; i < 3; i++) fChannelTime = (fChannelTime << 16) | *ChannelTime++;
			return(fChannelTime);
		};
		inline long long SetChannelTime(UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime) {
			fChannelTime = ((unsigned long long)BufferTimeHi << 32) | ((unsigned long long)EventTimeHi << 16) | (unsigned long long)FastTrigTime;
			return(fChannelTime);
		};
		inline long long SetChannelTime(TUsrHit * Hit) {
			fChannelTime = Hit->GetChannelTime();
			return(fChannelTime);
		};

		inline void CopyData(UShort_t * Data, Int_t NofData, Int_t Offset = 0) {		// copy hit data
			if (Data) for (Int_t i = 0; i < NofData; i++) fData[i + Offset] = *Data++;
		};

		inline void SetData(UShort_t Data, Int_t Offset = TUsrHit::kHitEnergy) { fData[Offset] = Data; }

		inline void CopyData(TUsrHit * Hit, Int_t NofData, Int_t Offset = 0) {
			UShort_t * dp = Hit->GetDataAddr() + Offset;
			for (Int_t i = 0; i < NofData; i++) fData[i + Offset] = *dp++;
		};

		inline void ClearData() { memset(fData, 0, kMaxHitData * sizeof(UShort_t)); };	// clear data

	protected:		
		Int_t fBufferNumber; 			// buffer number
		Int_t fEventNumber; 			// event index within buffer
		Int_t fModuleNumber;			// module serial number
		Int_t fChannel; 				// module channel
		long long fChannelTime; 		// time stamp generated by dgf clock bus (48 bits unsigned!)
		Int_t fNofData; 				// number of data words
		UShort_t fData[kMaxHitData]; 	// data (fast trigger, energy, etc)
		
	ClassDef(TUsrHit, 1)				// [Analyze] Hit
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer
// Purpose:        A buffer to store hit data
// Description:    TClonesArray to store hit data.
//                 Entries are indexed by module and channel number.
//                 Sorting may be done according to time stamp.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrHitBuffer : public TObject {

	public:
		TUsrHitBuffer(Int_t maxent = 2500) {
   		fNofEntries = maxent;
   		fNofHits = 0;
   		fHits = NULL;

 //  		fHits = new TClonesArray("TUsrHit", fNofEntries);
};

		virtual ~TUsrHitBuffer() {
			if (fHits) delete fHits;
		};										// default dtor

		inline void SetName(const Char_t * BufferName) { fBufName = BufferName; };
		inline const Char_t * GetName() const { return fBufName.Data(); };

		Int_t AllocClonesArray(Int_t NofEntries, Int_t HighWater);	// allocate TClonesArray to store hits

		void Reset();												// reset hit list

		TUsrHit * AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel, 			// add a new hit
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData);
		TUsrHit * AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel, 
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData);
		Bool_t AddHit(TUsrHit * Hit);					   //  add ready made hit
		Bool_t RemoveHit(TUsrHit * Hit);					// remove hit
		Bool_t RemoveHit(Int_t Index);

		inline Int_t GetNofEntries() const { return(fNofEntries); };
		inline Int_t GetNofHits() const { return(fNofHits); };
		
		inline void SetHighWater(Int_t HighWater) { fHighWater = (HighWater >= fNofEntries) ? 0 : HighWater; };
		inline Int_t GetHighWater() const { return(fHighWater); };
		Bool_t IsHighWater(Bool_t Verbose = kFALSE) const;
				
		inline void Sort(Int_t UpTo = kMaxInt) { fHits->Sort(UpTo); };		// sort entries by time

		inline TClonesArray * GetCA() { return(fHits); };

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, Int_t Begin = 0, Int_t End = -1) const;						// print data
		inline void Print(Int_t Begin = 0, Int_t End = -1) const { Print(cout, Begin, End); };
		
	protected:
		TString fBufName;						// buffer name
		Int_t fNofEntries;				   // max number of entries
		Int_t fNofHits; 						// current number of hits
		Int_t fOffset;							// [obsolete, for compatibility reasons only]
		Int_t fHighWater;						// high water margin
		TClonesArray * fHits;			   // array containing hit data
//		TClonesArray * fHits;			   //-> array containing hit data
			
	ClassDef(TUsrHitBuffer, 1)					// [Analyze] Hit buffer
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX
// Purpose:        A wrapper to access hit buffer elements
// Description:    Provides means to access hit buffer data
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrHBX : public TObject {

	public:
		TUsrHBX(TObject * Event, TUsrHitBuffer * HitBuffer, Int_t Window = 0);		// ctor
		~TUsrHBX() {};										// default dtor

		inline const Char_t * GetName() const { return(fHitBuffer->GetName()); };

		inline Int_t GetNofEntries() const { return(fHitBuffer->GetNofEntries()); };
		inline Int_t GetNofHits() const { return(fHitBuffer->GetNofHits()); };
		
		inline void SetWindow(Int_t Window) { fWindow = Window; };
		inline Int_t GetWindow() const { return(fWindow); };

		inline void SetIndex(Int_t Index) { fCurIndex = Index; };
		inline Int_t GetIndex() const { return(fCurIndex); };
		inline void ResetIndex() { fCurIndex = 0; fResetDone = kTRUE; };

		TUsrHit * FindHit(TUsrHit & HitProfile) ;								// search for a given hit
		TUsrHit * At(Int_t Index) const { return ((TUsrHit *) fHits->At(Index)); }; 	// return hit at given index
		
		TUsrHit * FindEvent(Int_t EventNumber); 							// find an event
		TUsrHit * FindNextEvent();											// find first hit of next event

		inline Bool_t AtEnd() const { return(fCurIndex >= this->GetNofHits()); }; // kTRUE if at end of buffer

		Bool_t HitInWindow(TUsrHit * Hit0) const; 								// check if hit in time window

		TUsrHit * NextHit();												// get next hit
		TUsrHit * CurHit(); 												// get current hit

		inline TUsrHitBuffer * GetHitBuffer() const { return(fHitBuffer); };

		inline void Sort(Int_t UpTo = kMaxInt) { fHits->Sort(UpTo); };		// sort entries by time
		
	protected:
		TObject * fEvent;						// event / trigger
		Bool_t fResetDone;						// kTRUE after ResetIndex()
		Int_t fCurIndex;						// current index in buffer
		Int_t fWindow;							// time stamp window
		TUsrHitBuffer * fHitBuffer; 			//! hit buffer address
		TClonesArray * fHits;					//! access to hit array
			
	ClassDef(TUsrHBX, 1)				// [Analyze] Hit buffer index
};
	
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze
// Purpose:        Main class to define user's analyze methods
// Description:    Defines methods to perform user's analysis.
//                 (*) replay mode only
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbAnalyze : public TObject {

	public:
		enum 				{	kTimeUnitsPerSec	=	10000		};
		enum				{	kTimeNsecsPerUnit	=	100000		};

		enum				{	kBranchSplitLevel	=	99			};
		enum				{	kBranchBufferSize	=	32000		};

		enum				{	kUpdateTime 	=	8				};

		enum EMrbRunStatus	{	M_ABSENT,
								M_STARTING,
								M_RUNNING,
								M_PAUSING,
								M_STOPPING,
								M_STOPPED
							};

	public:
		TMrbAnalyze(TMrbIOSpec * IOSpec);						// default ctor
		~TMrbAnalyze() { fLofIOSpecs.Delete(); };				// dtor

		TMrbAnalyze(const TMrbAnalyze &) {};					// dummy copy ctor

		void SetScaleDown(Int_t ScaleDown = 1);					// set (global) scale-down factor
		TH1F * UpdateRateHistory();  							// add a bin to the rate history
		TH1F * UpdateDTimeHistory();  							// update the dead-time history
		Bool_t TestRunStatus(); 								// wait for run flag
		Bool_t Initialize(TMrbIOSpec * IOSpec);					// init objects here
		Bool_t InitializeUserCode(TMrbIOSpec * IOSpec);			// user may init additional objects here
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

		inline void AddIOSpec(TMrbIOSpec * IOSpec) { fLofIOSpecs.Add(IOSpec); };	// add an i/o spec to list
		inline TMrbIOSpec * GetNextIOSpec(TMrbIOSpec * IOSpec) {					// get (next) i/o spec
			if (IOSpec == NULL) return((TMrbIOSpec *) fLofIOSpecs.First());
			else				return((TMrbIOSpec *) fLofIOSpecs.After(IOSpec));
		};

		Int_t SaveHistograms(const Char_t * Pattern, const Char_t * HistFile);		// save mmap data to histo file
		Int_t SaveHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec);

		Int_t ClearHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec = NULL);	// clear histos in shared memory

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

		void PrintStartStop() const;		// output start/stop time stamps

		inline TMrbLogger * GetMessageLogger() const { return(fMessageLogger); };
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbAnalyze.html&"); };

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

		TMapFile * fMapFile;		// share memory mmap file
		Int_t fMapFileSize;			// ... size

		TList fLofIOSpecs;			// list of i/o specs

		TString fResourceFile;		// user's resource defs
		TString fResourceName;
		TString fResourceString;

		TMrbLofNamedX fModuleList;		// list of modules, indexed by serial number
		TMrbLofNamedX fParamList;		// list of params, indexed by param number
		TMrbLofNamedX fHistoList;		// list of histograms, indexed by param number
		TMrbLofNamedX fSingleList;		// list of single histograms, indexed by param number
		TMrbLofNamedX fCalibrationList;	// list of calibrations, indexed by param number

	ClassDef(TMrbAnalyze, 1)	// [Analyze] Describe user's analysis
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent
// Purpose:        Base class for user-defined events
// Description:    Defines methods common to all user-specific events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrEvent : public TObject {

	public:
		TUsrEvent(); 								// default ctor
		~TUsrEvent() { fLofSubevents.Delete(); fLofHBXs.Delete(); };	// dtor

		const UShort_t * NextSubevent(const MBSDataIO * BaseAddr, Int_t NofWords = 0, Bool_t RawMode = kFALSE) const;	// get next subevent
		UInt_t NextSubeventHeader(const MBSDataIO * BaseAddr) const;
		const UShort_t * NextSubeventData(const MBSDataIO * BaseAddr, Int_t NofWords = 0, Bool_t RawMode = kFALSE) const;

		inline void SetScaleDown(Int_t ScaleDown) { fScaleDown = ScaleDown; }; // set individual scale down
		inline Int_t GetScaleDown() const { return(fScaleDown); };	// get current scale down

		inline Int_t GetClockSecs() const { return(fClockSecs); };	// return event time (secs since 1-Jan-70)
		inline Int_t GetClockNsecs() const { return(fClockNsecs); };	// return event time (nanosecs of current second)
		inline Int_t GetTimeStamp() const { return(GetUniqueID()); };	// return time stamp (100 microsecs since start)
		inline Int_t GetTimeRS() const { return(fTimeRS); };			// get time (ROOT style)
		Int_t CalcTimeDiff(TUsrEvent * Event) const;					// calc time diff with respect to event

		inline TTree * GetTreeIn() const { return(fTreeIn); }; 	// return tree addr (input)
		inline TTree * GetTreeOut() const { return(fTreeOut); }; 	// ... (output)

		inline void SetReplayMode(const Bool_t ReplayMode = kTRUE) {
			fReplayMode = (fTreeIn == NULL) ? kFALSE : ReplayMode;
		};
		inline Bool_t IsReplayMode() const { return(fReplayMode); };

		inline void SetFakeMode(const Bool_t FakeMode = kTRUE) {
			fReplayMode = fFakeMode = FakeMode;
		};
		inline Bool_t IsFakeMode() const { return(fFakeMode); };

		inline Int_t GetNofEvents() const { return(fNofEvents); };
		inline Bool_t IsModScaleDown() const { return((fNofEvents % fScaleDown) == 0); };

																					// list of subevents
		inline void AddSubevent(TObject * Subevent, Int_t SevtSerial = -1) {		// add subevent to list
				if (SevtSerial == -1)	fLofSubevents.AddLast(Subevent);
				else					fLofSubevents.AddAtAndExpand(Subevent, SevtSerial);
		};
		inline TObjArray * GetLofSubevents() { return(&fLofSubevents); };
		inline TObject * GetSubevent(Int_t SevtSerial) { return(fLofSubevents.At(SevtSerial)); };	// get it from list
		inline Int_t GetNofSubevents() const { return(fLofSubevents.GetSize()); };

																	// list of hit buffers
		inline void AddHBX(TUsrHBX * HBX, Int_t SevtSerial = -1) {	// add hit buffer wrapper to list
				if (SevtSerial == -1)	fLofHBXs.AddLast(HBX);
				else					fLofHBXs.AddAtAndExpand(HBX, SevtSerial);
		};
		inline TUsrHBX * GetHBX(Int_t SevtSerial) { return((TUsrHBX *) fLofHBXs.At(SevtSerial)); }; // get it from list

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TUsrEvent.html&"); };

	protected:
		Int_t CalcTimeRS(); 				// convert time to ROOT style
		inline void SetTimeStamp(Int_t TimeStamp) { SetUniqueID(TimeStamp); };

	protected:
		TObject * fBranch;					//! ptr to event to be used in TTree::Branch()

		TTree * fTreeOut;					// tree to store data in ROOT format
		TTree * fTreeIn;					// tree to read data from ROOT file
		Bool_t fReplayMode; 				// kTRUE if replay mode on
		Bool_t fFakeMode; 					// kTRUE if fake mode on

		Int_t fNofEntries; 					// number of entries in ROOT tree (replay mode only)

		UInt_t fType;						// event type
		UInt_t fSubtype;					// ... subtype
		Int_t fTrigger;						// trigger number
		Int_t fEventNumber;					// event number provided by MBS
		Int_t fNofEvents; 					// number of events collected so far
		Int_t fScaleDown;					// individual scale down factor
		Int_t fClockRes;					// clock resolution in nsecs
		Int_t fClockSecs; 					// seconds since January 1, 1970
		Int_t fClockNsecs;  				// nano secs of current second
		Int_t fTimeRS;						// time (ROOT style)

		TObjArray fLofHBXs; 				// list of hit buffer wrappers, indexed by hit buffer
		TObjArray fLofSubevents;			// list of subevents

	ClassDef(TUsrEvent, 1)		// [Analyze] User's event structure
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart
// Purpose:        Define an event for "Start Acquisition"
// Description:    Event assigned to trigger 14 (Start Acquisition)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEvtStart : public TUsrEvent {

	public:
		TUsrEvtStart(); 				// ctor
		~TUsrEvtStart() {};				// default dtor

		Bool_t ExtractTimeStamp(const s_vehe *, const MBSDataIO *);
		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print();
		Bool_t CreateTree();
		Bool_t InitializeTree(TFile * RootFile);
		inline Int_t GetTime() const { return(fClockSecs); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TUsrEvtStart.html&"); };

	ClassDef(TUsrEvtStart, 1)		// [Analyze] Event type "START ACQUISITION"
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop
// Purpose:        Define an event for "Stop Acquisition"
// Description:    Event assigned to trigger 15 (Stop Acquisition)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEvtStop : public TUsrEvent {

	public:
		TUsrEvtStop();  				// ctor
		~TUsrEvtStop() {};				// default dtor

		Bool_t ExtractTimeStamp(const s_vehe *, const MBSDataIO *);
		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print();
		Bool_t CreateTree();
		Bool_t InitializeTree(TFile * RootFile);
		inline Int_t GetTime() const { return(fClockSecs); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TUsrEvtStop.html&"); };

	ClassDef(TUsrEvtStop, 1)		// [Analyze] Event type "STOP ACQUISITION"
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime
// Purpose:        Class to store dead-time data
// Description:    Defines methods for dead-time (sub)events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrDeadTime : public TObject {

	public:
		TUsrDeadTime() {							// ctor
			fTimeStamp = 0;
			fTotalEvents = 0;
			fAccuEvents = 0;
			fScalerContents = 0;
			fDeadTime = 0;
		};

		~TUsrDeadTime() {}; 						// default dtor

		void Set(Int_t TimeStamp, Int_t TotalEvents, Int_t AccuEvents, Int_t ScalerContents);
		inline Float_t Get() const { return(fDeadTime); };			// get dead time in %

		inline Int_t GetTimeStamp() const { return(fTimeStamp); };	// return time stamp (100 microsecs since start)
		inline Int_t GetTotalEvents() const { return(fTotalEvents); }; 	// get event number

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print();

		inline void SetReplayMode(const Bool_t ReplayMode = kTRUE) {
			fReplayMode = (fTreeIn == NULL) ? kFALSE : ReplayMode;
		};
		inline Bool_t IsReplayMode() const { return(fReplayMode); };

		Bool_t CreateTree();										// create tree to hold dead-time data
		Bool_t InitializeTree(TFile * RootFile);					// initialize dead-time tree for replay
		inline TTree * GetTreeIn() const { return(fTreeIn); };
		inline TTree * GetTreeOut() const { return(fTreeOut); };
	
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TUsrDeadTime.html&"); };

	protected:
		Int_t fTimeStamp;					// time stamp (since start)
		Int_t fTotalEvents; 				// event count
		Int_t fAccuEvents;					// accumulated events
		Int_t fScalerContents;				// contents of dead-tim scaler
		Float_t fDeadTime;					// dead time (%)

		Bool_t fReplayMode;

		TTree * fTreeIn;					//! dead time (input)
		TTree * fTreeOut;					//! ... (output)
		TObject * fBranch;					//! ptr to dead-time obj to be used in TTree::Branch()

	ClassDef(TUsrDeadTime, 1)		// [Analyze] Keep track of dead time
};

#endif
