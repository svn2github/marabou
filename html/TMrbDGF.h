#ifndef __TMrbDGF_h__
#define __TMrbDGF_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGF.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGF            -- base class
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <time.h>

#include "TROOT.h"
#include "TNamed.h"
#include "TEnv.h"
#include "TH1.h"
#include "TF1.h"
#include "TArrayS.h"

#include "TMrbLofNamedX.h"
#include "TMrbEsone.h"

#include "TMrbDGFData.h"
#include "TMrbDGFEventBuffer.h"
#include "TMrbDGFHistogramBuffer.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF
// Purpose:        Class defs describing module XIA DGF-4C
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGF : public TNamed {

	public:

		// bit definitions: software status (module)
		enum EMrbDGFStatusMBits	{	kSetupOk				=	BIT(0),
									kConnected				=	BIT(1),
									kOffline				=	BIT(2),
									kLocalData				=	BIT(3),
									kSystemFPGACodeLoaded 	=	BIT(4),
									kFippiFPGACodeLoaded 	=	BIT(5),
									kDSPCodeLoaded			=	BIT(6),
									kParamValuesRead		=	BIT(7),
									kParamsIncoreChanged	=	BIT(8),
									kDSPRunning				=	BIT(9),
									kDSPAborted				=	BIT(10)
								};

		// bit operations
		enum EMrbBitOp	 		{	kBitSet 			=	1,
									kBitOr,
									kBitMask,
									kBitClear
								};

		// wait status
		enum EMrbWaitStatus 	{	kWaitError 			=	-3,
									kWaitTimedOut		=	-2,
									kWaitAborted		=	-1,
									kWaitInProgress 	=	0,
									kWaitOk 			=	1
								};
		
		// timeout interval (times 500 ms)
		enum					{	kTimeout			=	1000			};

		// length of calibration array
		enum					{	kCalibrationSize	=	1024			};

		// temp param save/restore
		enum EMrbDGFSaveIdx 	{	kSaveTrace,
									kSaveTraceAsIs,
									kSaveUntrigTrace,
									kSaveDacRamp,
									kSaveTaufit
								};

	public:
		TMrbDGF(	const Char_t * DgfName = "DGF-4C") : TNamed(DgfName, "Xia DGF-4C") {	// default ctor
			fStatusM = 0;
		};
		
		TMrbDGF(	const Char_t * DGFName, 							// ctor: host, crate, station
					const Char_t * HostName, Int_t Crate, Int_t Station,
					Bool_t Connect = kTRUE, Bool_t Offline = kFALSE,
					Bool_t LocalData = kFALSE);

		TMrbDGF(	const Char_t * DGFName,
					const Char_t * HostName, const Char_t * CamacAddr,
					Bool_t Connect = kTRUE, Bool_t Offline = kFALSE,
					Bool_t LocalData = kFALSE);							// ctor: host, camac addr

		~TMrbDGF() {
						gROOT->GetList()->Remove(this);
						ResetDGF();
					}; 	// default dtor

		// FPGA section
		Bool_t DownloadFPGACode(TMrbDGFData::EMrbFPGAType FPGAType);	// download code to system or fippi FPGA
		Bool_t DownloadFPGACode(const Char_t * FPGAType);
		Bool_t FPGACodeLoaded(TMrbDGFData::EMrbFPGAType FPGAType);		// test if download ok
		Bool_t FPGACodeLoaded(const Char_t * FPGAType);

		Bool_t SetSwitchBusDefault();									// set switch bus register
		Bool_t SetSwitchBus(UInt_t Bits = 0, TMrbDGF::EMrbBitOp BitOp = kBitSet);
		Bool_t SetSwitchBus(const Char_t * DSPTrigger, const Char_t * FastTrigger = NULL);
		UInt_t GetSwitchBus(); // get

		// DSP section
		Bool_t DownloadDSPCode();										// download code to DSP
		Bool_t UploadDSPCode(TArrayS & DSPCode);						// load code back from DSP to memory
		inline Bool_t DSPCodeLoaded() { return((fStatusM & TMrbDGF::kDSPCodeLoaded) != 0); };	// test if download ok
		void ClearRegisters();											// initialize main dsp registers										

		// parameter section
		Bool_t ReadParamMemory(Bool_t ReadInputParams = kTRUE, Bool_t ReadOutputParams = kFALSE);	// read params from DSP
		Bool_t ReadParamMemory(Int_t Channel);							// read params for a single channel
		Bool_t ReadParamMemory(const Char_t * ParamName);				// read params for a group of channels
		Bool_t ReadParamMemory(UInt_t FromParam, UInt_t ToParam);		// read part of the param memory

		Bool_t WriteParamMemory(Bool_t Reprogram = kTRUE);				// write params back to DSP (data section only)
		Bool_t WriteParamMemory(Int_t Channel); 						// write params for a single channel
		Bool_t WriteParamMemory(const Char_t * ParamName); 				// write params for a group of channels

		Bool_t CopyParamMemory(TMrbDGF * Module, Bool_t CopyInputParams = kTRUE, Bool_t CopyOutputParams = kFALSE);	// copy params to dgf
		Bool_t CopyParamMemory(TMrbDGF * Module, Int_t Channel);				// copy a single channel
		Bool_t CopyParamMemory(TMrbDGF * Module, const Char_t * ParamName); 	// copy a group of channels

		inline Short_t * GetParamAddr() { return(fParams.GetArray()); };	// addr of param array

		inline Bool_t ParamValuesRead() { return((fStatusM & TMrbDGF::kParamValuesRead) != 0); };
		inline void SetParamValuesRead(Bool_t Flag = kTRUE) {
					if (Flag)	fStatusM |= TMrbDGF::kParamValuesRead;
					else		fStatusM &= ~TMrbDGF::kParamValuesRead;
		};
		inline Bool_t ParamsIncoreChanged() { return((fStatusM & TMrbDGF::kParamsIncoreChanged) != 0); };
		inline void SetParamsIncoreChanged(Bool_t Flag = kTRUE) {
					if (Flag)	fStatusM |= TMrbDGF::kParamsIncoreChanged;
					else		fStatusM &= ~TMrbDGF::kParamsIncoreChanged;
		};

		Bool_t SetParValue(const Char_t * ParamName, Int_t Value, Bool_t UpdateDSP = kTRUE);	// set param value
		Int_t GetParValue(const Char_t * ParamName, Bool_t ReadFromDSP = kTRUE);				// get param
		Bool_t SetParValue(Int_t Channel, const Char_t * ParamName, Int_t Value, Bool_t UpdateDSP = kTRUE);	// single chan
		Int_t GetParValue(Int_t Channel, const Char_t * ParamName, Bool_t ReadFromDSP = kTRUE);

		Bool_t PrintParamsToFile(const Char_t * FileName = "", const Char_t * ParamName = "*");
		Bool_t PrintParamsToFile(const Char_t * FileName, Int_t Channel);
		Bool_t PrintParams(ostream & OutStrm, const Char_t * ParamName = "*");	// print param(s) chosen by name
		Bool_t PrintParams(ostream & OutStrm, Int_t Channel); 					// print params for a given channel
		inline Bool_t PrintParams(const Char_t * ParamName = "*") { return(PrintParams(cout, ParamName)); };
		inline Bool_t PrintParams(Int_t Channel) { return(PrintParams(cout, Channel)); };
		
		Bool_t PrintStatus(ostream & OutStrm, Int_t Channel = -1);				// output register contents
		inline Bool_t PrintStatus(Int_t Channel = -1) { return(PrintStatus(cout, Channel)); };
		 
		Int_t SaveParams(const Char_t * ParamFile, Bool_t ReadFromDSP = kTRUE);		// save params to file
		Int_t LoadParams(const Char_t * ParamFile, Bool_t UpdateDSP = kTRUE);		// read params from file
		Int_t LoadParamsToEnv(TEnv * Env, const Char_t * ParamFile);				// read param values to ROOT environment
		Int_t SaveValues(const Char_t * ValueFile, Bool_t ReadFromDSP = kTRUE);		// save values to file
		Bool_t SaveParams(TArrayS & TempStorage);									// save params temporarily
		Bool_t RestoreParams(TArrayS & TempStorage);								// restore params temp storage
		Bool_t SaveParams(EMrbDGFSaveIdx SaveIdx);
		Bool_t RestoreParams(EMrbDGFSaveIdx SaveIdx);

		// param calculations
		Double_t Rounded(Double_t Value); 										// round value
		UInt_t * ToFixedPoint(Double_t Value, UInt_t * DspFixPt);				// convert from double to DSP fixed point
		Double_t FromFixedPoint(UInt_t * DspFixPt);								// convert from DSP fixed point to double
		UInt_t * ToDouble(Double_t Value, UInt_t * DspDouble);					// convert from double to DSP double

		// module CSRA
		Bool_t SetModCSRA(UInt_t Bits, TMrbDGF::EMrbBitOp BitOp, Bool_t UpdateDSP = kTRUE);	// set / clear
		UInt_t GetModCSRA(Bool_t ReadFromDSP = kTRUE); 							// read

		// coinc pattern
		Bool_t SetCoincPattern(UInt_t Pattern, Bool_t UpdateDSP = kTRUE);		// set coinc pattern word
		UInt_t GetCoincPattern(Bool_t ReadFromDSP = kTRUE); 					// read coinc pattern word
		Bool_t ModifyCoincPattern(UInt_t Pattern, EMrbBitOp AddOrRemove, Bool_t UpdateDSP = kTRUE); // add/remove single hit pattern
		Bool_t CoincPatternSet(UInt_t Pattern, Bool_t ReadFromDSP = kTRUE);		// test hit pattern

		// channel CSR
		Bool_t SetChanCSRA(Int_t Channel, UInt_t Bits, TMrbDGF::EMrbBitOp BitOp, Bool_t UpdateDSP = kTRUE);	// set
		UInt_t GetChanCSRA(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 										// read

		// "good channel" bit
		Bool_t SetChannelMask(const Char_t * ChannelMask, Bool_t UpdateDSP = kTRUE);	// set mask of good channels
		Bool_t SetChannelMask(UInt_t ChannelMask, Bool_t UpdateDSP = kTRUE);
		UInt_t GetChannelMask(Bool_t ReadFromDSP = kTRUE); 								// read mask of good (=active) channels
		Bool_t SetGoodChannel(Int_t Channel, EMrbBitOp SetOrClear = kBitOr, Bool_t UpdateDSP = kTRUE);	// set/clear single channel
		Bool_t IsGoodChannel(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 				// test if channel active
		Int_t GetNofGoodChannels(Bool_t ReadFromDSP = kTRUE);							// get number of good channels
		void PrintChannelMask(Bool_t CrFlag, Bool_t ReadFromDSP = kTRUE);				// print channel mask
		inline Bool_t ClearChannelMask(Bool_t UpdateDSP = kTRUE) { return(SetChannelMask((UInt_t) 0, UpdateDSP)); };

		// trigger bit
		Bool_t SetTriggerMask(const Char_t * ChannelMask, const Char_t * Polarity, Bool_t UpdateDSP = kTRUE);
		Bool_t SetTriggerMask(UInt_t ChannelMask, Int_t Polarity, Bool_t UpdateDSP = kTRUE);	// set trigger bits
		Bool_t SetTriggerMask(UInt_t ChannelMask, const Char_t * Polarity, Bool_t UpdateDSP = kTRUE);
		UInt_t GetTriggerMask(Bool_t ReadFromDSP = kTRUE); 								// read trigger bits
		Bool_t SetTriggerBit(Int_t Channel, Int_t Polarity, EMrbBitOp SetOrClear = kBitOr, Bool_t UpdateDSP = kTRUE);
		Bool_t SetTriggerBit(Int_t Channel, const Char_t * Polarity, EMrbBitOp SetOrClear = kBitOr, Bool_t UpdateDSP = kTRUE);
		Bool_t SetTriggerPolarity(Int_t Channel, Int_t Polarity, Bool_t UpdateDSP = kTRUE);	// set polarity							
		Int_t GetTriggerPolarity(Int_t Channel, Bool_t ReadFromDSP = kTRUE);			// get polarity							
		Bool_t HasTriggerBitSet(Int_t Channel, Bool_t ReadFromDSP = kTRUE);				// test trigger bit
		void PrintTriggerMask(Bool_t CrFlag, Bool_t ReadFromDSP = kTRUE);				// print trigger mask
		inline Bool_t ClearTriggerMask(Bool_t UpdateDSP = kTRUE) { return(SetTriggerMask((UInt_t) 0, -1, UpdateDSP)); };

		Bool_t SetXwait(Int_t Xwait = 0, Bool_t UpdateDSP = kTRUE); 					// set/get wait states
		
		Bool_t SetSynchWait(Bool_t SynchFlag = kTRUE, Bool_t UpdateDSP = kTRUE); 		// set/clear synch wait flag
		Bool_t SetInSynch(Bool_t SynchFlag = kTRUE, Bool_t UpdateDSP = kTRUE); 			// set/clear clock synch
		
		Bool_t SetDelay(Int_t Channel, Double_t Delay, Bool_t UpdateDSP = kTRUE);		// set delay
		Double_t GetDelay(Int_t Channel, Bool_t ReadFromDSP = kTRUE);					// get delay

		Int_t SetGain(Int_t Channel, Double_t Gain, Bool_t UpdateDSP = kTRUE); 			// set gain
		Double_t GetGain(Int_t Channel, Bool_t ReadFromDSP = kTRUE);					// get gain

		Int_t SetOffset(Int_t Channel, Double_t Offset, Bool_t UpdateDSP=kFALSE);		// set offset
		Double_t GetOffset(Int_t Channel, Bool_t ReadFromDSP = kTRUE);					// get offset

		Bool_t UpdateSlowFilter(Int_t Channel, Bool_t UpdateDSP = kTRUE);				// update slow filter

		Bool_t ResetPGTime(Int_t Channel, Bool_t UpdateDSP = kTRUE); 					// reset peak & gap times (energy)
		Bool_t SetPeakTime(Int_t Channel, Double_t PeakTime, Bool_t UpdateDSP = kTRUE); 		// set energy peak time
		Double_t GetPeakTime(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 						// get energy peak time
		Bool_t SetGapTime(Int_t Channel, Double_t GapTime, Bool_t UpdateDSP = kTRUE);			// set energy gap time
		Double_t GetGapTime(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 						// get energy gap time

		Bool_t ResetFastPGTime(Int_t Channel, Bool_t UpdateDSP = kTRUE); 				// reset peak & gap times (trigger)
		Bool_t SetFastPeakTime(Int_t Channel, Double_t PeakTime, Bool_t UpdateDSP = kTRUE); 	// set trigger peak time
		Double_t GetFastPeakTime(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 					// get trigger peak time
		Bool_t SetFastGapTime(Int_t Channel, Double_t GapTime, Bool_t UpdateDSP = kTRUE);		// set trigger gap time
		Double_t GetFastGapTime(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 					// get trigger gap time
		void SetFastFilter(Int_t Channel, Int_t Length, Int_t Gap, Bool_t UpdateDSP);			// (method obsolete)

		Double_t GetDeltaT(Bool_t ReadFromDSP = kTRUE);								// get time slice
		Int_t CalcCoincWait(Bool_t ReadFromDSP = kTRUE);							// calculate coinc time window
		Bool_t SetCoincWait(Bool_t ReadFromDSP = kTRUE);							// write coinc time to dsp

		Bool_t SetThreshold(Int_t Channel, Int_t Threshold, Bool_t UpdateDSP = kTRUE);			// set threshold
		Int_t GetThreshold(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 							// get threshold

		Bool_t SetTraceLength(Int_t Channel, Double_t TraceLength, Bool_t UpdateDSP = kTRUE);	// set trace length in us
		Double_t GetTraceLength(Int_t Channel, Bool_t ReadFromDSP = kTRUE);						// get trace length

		Bool_t SetTau(Int_t Channel, Double_t Tau, Bool_t UpdateDSP = kTRUE);					// set tau
		Double_t GetTau(Int_t Channel, Bool_t ReadFromDSP = kTRUE); 							// get tau value													// get tau value

		Bool_t SetBinning(Int_t Channel, Int_t BsWeight, Int_t EnBin, Int_t BsBin, Bool_t UpdateDSP = kTRUE);	// set binning

		// data acquisition
		Int_t GetTrace( UInt_t TriggerMask, const Char_t * Polarity, UInt_t GoodChannels,		// take a trace
						Int_t NofTraces, Int_t TraceLength, TMrbDGFEventBuffer & Data,
						Int_t XwaitStates = 0);
		Int_t GetTrace( Int_t TraceLength, TMrbDGFEventBuffer & Data, Int_t XwaitStates = 0);
			Bool_t GetTrace_Init(UInt_t TriggerMask, const Char_t * Polarity, UInt_t GoodChannels,
									Int_t NofTraces, Int_t TraceLength, Int_t XwaitStates = 0);
			Bool_t GetTrace_Init(Int_t TraceLength, Int_t XwaitStates = 0);
			Bool_t GetTrace_Start();
			Int_t GetTrace_Stop(TMrbDGFEventBuffer & Data, Int_t SecsToWait = 5);

		Int_t GetUntrigTrace(TArrayI & Buffer, Int_t XwaitStates = 0);			// take untriggered traces
			Bool_t GetUntrigTrace_Init(TArrayI & Buffer, Int_t XwaitStates = 0);
			Bool_t GetUntrigTrace_Start(Int_t Channel);
			Int_t GetUntrigTrace_Stop(Int_t Channel, TArrayI & Buffer, Int_t SecsToWait = 5);

		Int_t GetDacRamp(TArrayI & Buffer);										// calibrate dac offsets
			Bool_t GetDacRamp_Init();
			Bool_t GetDacRamp_Start();
			Int_t GetDacRamp_Stop(TArrayI & Buffer, Int_t SecsToWait = 10);

 		Int_t ReadEventBuffer(TMrbDGFEventBuffer & Buffer); 				// get data from event buffer
		Bool_t AccuHistograms(Int_t Time, const Char_t * Scale = "secs", Bool_t AccuBL = kTRUE);	// accumulate histogram
		Int_t ReadHistogramBuffer(TMrbDGFHistogramBuffer & Buffer); 							// get mca data

		// tau fit
		Double_t TauFit(Int_t Channel, Int_t NofTraces, Int_t TraceLength, Double_t A0, Double_t A1,
												Int_t From = 0, Int_t To = 0,
												Stat_t TraceError = 0, Stat_t ChiSquare = 5., 
												Bool_t DrawIt = kTRUE); 			// perform tau fit

		// run control
		Bool_t StartRun(Bool_t ClearMCA = kTRUE);									// start run / clear MCA data
		Bool_t StopRun();															// stop run
		inline void AbortRun() { fStatusM |= TMrbDGF::kDSPAborted; };				// abort run
		inline Bool_t IsAborted() { return((fStatusM & TMrbDGF::kDSPAborted) != 0); };
				
		// misc
		inline Bool_t SetupOk() { return((fStatusM & TMrbDGF::kSetupOk) != 0); };	// test setup

		Bool_t Connect();															// connect to camac host
		inline Bool_t IsConnected() { return((fStatusM & TMrbDGF::kConnected) != 0); };	// test connection

		Bool_t SetCamacHost(const Char_t * Host); 										// host, crate, station
		Bool_t SetCrate(Int_t Crate);
		Bool_t SetStation(Int_t Station);
		inline const Char_t * GetCamacHost() { return(fCamacHost.Data()); };
		inline Int_t GetCrate() { return(fCrate); };
		inline Int_t GetStation() { return(fStation); };

		inline void SetClusterID(Int_t ClusterSerial = 0, const Char_t * ClusterColor = "", const Char_t * SegmentID = "") {
			fClusterID.Set(ClusterSerial, ClusterColor, SegmentID);
		};
		inline TMrbNamedX * GetClusterID() { return(&fClusterID); };
		const Char_t * GetClusterInfo(TMrbString & Info);
		
		inline TMrbNamedX * GetRevision() { return(&fRevision); }; 					// module revision
		inline void SetRevision(Int_t ManufactIndex = 0);

		inline void SetModuleNumber(Int_t ModNum) { fModNum = ModNum; };			// module number
		inline Int_t GetModuleNumber() { return(fModNum); };

		inline UInt_t GetStatus() { return(fStatusM); };							// soft status
		inline void SetStatus(UInt_t Status) { fStatusM = Status; };
		inline void Wait(Int_t Msecs = 100) { gSystem->Sleep(Msecs); };				// wait for DGF to settle down
		EMrbWaitStatus WaitActive(Int_t Timeout = 10);								// wait for active bit to drop
		inline EMrbWaitStatus GetWaitStatus() { return(fStatusW); }; 				// get wait status
		
		inline Bool_t IsOffline() { return((fStatusM & TMrbDGF::kOffline) != 0); };
		inline Bool_t HasLocalDataBase() { return((fStatusM & TMrbDGF::kLocalData) != 0); };

		void Print(ostream & OutStrm);												// print status
		virtual inline void Print() { Print(cout); };
		
		inline TMrbDGFData * Data() { return(fDGFData); };							// data handle
		inline TMrbEsone * Camac() { return(&fCamac); }; 							// camac handle
		inline UShort_t * GetPAddr() { return((UShort_t *) fParams.GetArray()); };	// addr of param data

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbDGF.html&"); };

		void SetVerboseMode(Bool_t VerboseFlag = kTRUE);				// verbose mode on/off

																		// camac access:
		Bool_t WriteICSR(UInt_t Bits);									//		write ICSR (Control Status Reg)
		Bool_t WriteCSR(UInt_t Bits);									//		write CSR (Control Status Reg)
		Bool_t WriteTSAR(UInt_t Data);									//		write TSAR (Transfer Start Address Reg)
		Bool_t WriteWCR(Int_t WordCount); 								//		write WCR (Word Count Reg)
		UInt_t ReadICSR();												//		read ICSR
		UInt_t ReadCSR();												//		read CSR
		UInt_t ReadTSAR();												//		read TSAR
		Int_t ReadWCR();												//		read WCR

	protected:
		void ResetDGF();												// reset data base
		void SetupDGF(Bool_t LocalData = kFALSE);						// initialize data base

		Bool_t CheckConnect(const Char_t * Method);						// check if connected to CAMAC
		Bool_t CheckActive(const Char_t * Method);						// check if active bit is set
		Bool_t CheckChannel(const Char_t * Method, Int_t Channel);		// check channel number
		Bool_t CheckChannelMask(const Char_t * Method);				 	// check if any channel active
		Bool_t CheckTriggerMask(const Char_t * Method);				 	// check if any trigger defined
		Bool_t CheckTriggerPolarity(const Char_t * Method); 			// check if trigger polarity defined

																		// write param start addr
		inline Bool_t WritePSA(Int_t Offset) { return(WriteTSAR(TMrbDGFData::kDSPInparStartAddr + Offset)); };

		Bool_t CheckBufferSize(const Char_t * Method, Int_t BufSize); 		// check buffer size

		Int_t ReadHistogramBufferPage();

	public:
		TMrbLofNamedX fLofDGFStatusMBits;								// soft status bits (module)
		TMrbLofNamedX fLofDGFSwitchBusModes;							// switch bus modes

	protected:
		UInt_t fXiaRelease;												// xia release
	
		UInt_t fStatusM; 												// soft status (module)
		EMrbWaitStatus fStatusW;										// wait status
		
		Int_t fModNum;													// module number

		TMrbEsone fCamac;												//! camac handle via esone rpc
		TString fCamacHost; 											// host name
		Int_t fCrate;													// crate number
		Int_t fStation;													// camac station

		TMrbNamedX fClusterID;											// cluster id
		TMrbNamedX fRevision;											// revision

		TArrayS fParams;												// DSP's parameter section
		
		UInt_t fSwitchBus;												// switchbus value
		
		TH1F * fTauDistr;												//! tau distribution
		TF1 * fGaussian;												//! gaussian fit

		TMrbDGFData * fDGFData; 										// pointer to DGF data base

		TArrayS fTmpParTrace;						// temp param storage
		TArrayS fTmpParUntrigTrace;
		TArrayS fTmpParDacRamp;
		TArrayS fTmpParTaufit;

	ClassDef(TMrbDGF, 1)		// [XIA DGF-4C] Base class for DGF modules
};

#endif
