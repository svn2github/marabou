#ifndef __TMrbDGFData_h__
#define __TMrbDGFData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFData.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGFData        -- data base to store DSP and FPGA data
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"
#include "TString.h"
#include "TArrayS.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData
// Purpose:        Data base to hold DSP / FPGA data
// Methods:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFData : public TObject {

	friend class TMrbDGF;

	public:

		// buffer sizes
		enum					{	kFPGAMaxSize			=	0x10000			};
		enum					{	kDSPMaxSize				=	0x8000			};

		// file types for DSP and FPGA code files
		enum	EMrbDGFFileType {	kFileUndef				=	1,
										kFileAscii				=	2,
										kFileBinary 			=	3
									};

		// FPGA types
		enum	EMrbFPGAType 	{	kSystemFPGA,
										kFippiFPGA
									};

		// layout of parameter section in DSP memory
		enum					{	kNofDSPInputParams		=	256 			};
		enum					{	kNofDSPOutputParams		=	160 			};

		enum					{	kNofDSPInparModule		=	64	 			};
		enum					{	kNofDSPInparChannel		=	48	 			};

		enum					{	kDSPInparStartAddr	 	=	0x4000			};
		enum					{	kDSPOutparStartAddr	 	=	kDSPInparStartAddr + kNofDSPInputParams		};
		
		// bit definitions in the control/status reg of the CAMAC Fippi FPGA
		enum EMrbCamacCSR		{
										kRunEna 				=	BIT(0),
										kNewRun 					=	BIT(1),
										kEnaLAM 					=	BIT(3),
										kDSPReset				=	BIT(4),
										kDSPError				=	BIT(12),
										kActive 					=	BIT(13),
										kLAMActive				=	BIT(14)
									};
		enum					{	kCamacCSRMask			=	kRunEna | kNewRun | kEnaLAM |
																	kDSPError | kActive | kLAMActive
								};

		// bit definitions in the control/status reg of the CAMAC System FPGA
		enum EMrbCamacICSR		{
											kSystemFPGAReset		 	=	BIT(0),
											kFippiFPGA0Reset 			=	BIT(4),
											kFippiFPGA1Reset 			=	BIT(5),
											kFippiFPGA2Reset 			=	BIT(6),
											kFippiFPGA3Reset 			=	BIT(7),
											kConnectDSPFromRight 	 	=	BIT(8),
											kConnectDSPFromLeft			=	BIT(9),
											kTerminateDSP	 			=	BIT(10),
											kConnectFastFromRight		=	BIT(11),
											kConnectFastFromLeft		=	BIT(12),
											kTerminateFast				=	BIT(13),
											kSwitchBusNormal			=	0
										};
		enum					{	kFippiFPGAReset 		=	kFippiFPGA0Reset | kFippiFPGA1Reset |
																	kFippiFPGA2Reset | kFippiFPGA3Reset
								};
		enum					{	kFPGAReset  				=	kSystemFPGAReset | kFippiFPGAReset };
		enum					{	kConnectDSPTrigger		=	kConnectDSPFromRight | kConnectDSPFromLeft |
																		kTerminateDSP
								};
		enum					{	kConnectFastTrigger		=	kConnectFastFromRight | kConnectFastFromLeft |
																		kTerminateFast
								};
		enum					{	kSwitchBus				=	kConnectDSPTrigger | kConnectFastTrigger };
		enum					{	kCamacICSRMask			=	kFPGAReset | kSwitchBus };

		// bit definitions in parameter MODCSRA: module control/status reg A
		enum EMrbModCSRA		{	kWriteLevel1	=	BIT(0)	};

		// bit definitions in parameter MODCSRB: module control/status reg B (reserved)
		enum EMrbModCSRB		{};

		// value definitions for parameter RUNTASK: defines kind of run task to be started
		enum EMrbRunTask		{
										kRunControl		=	0,
										kRunLinear		=	0x100,
										kRunCircular	=	0x101,
										kRunPSA 		=	0x102,
										kRunShort 		=	0x103,
										kRunMCA 		=	0x301,
									};

		// value definitions for parameter CONTROLTASK: defines control tasks
		enum EMrbControlTask	{
										kProgramDACs 		=	0,
										kConnectInputs		=	1,
										kDisconnectInputs	=	2,
										kCalibrate			=	3,
										kSampleADCs 	 	=	4,
										kUpdateFPGA 		=	5,
										kReadHistoFirstPage =	9,
										kReadHistoNextPage	=	10
									};

		// bit definitions in parameter CHANCSRA: control/status reg A for channel X
		enum EMrbChanCSRA		{
										kGroupTriggerOnly	=	BIT(0),
										kIndivLiveTime		=	BIT(1),
										kGoodChannel		=	BIT(2),
										kReadAlways 		=	BIT(3),
										kEnableTrigger		=	BIT(4),
										kTriggerPositive	=	BIT(5),
										kGFLTValidate		=	BIT(6),
										kHistoEnergies		=	BIT(7),
										kHistoBaselines 	=	BIT(8),
										kCorrBallDeficit	=	BIT(9),
										kComputeCFT 		=	BIT(10),
										kEnaMultiplicity	=	BIT(11),
										kBipolarSignals 	=	BIT(15)
									};

		enum					{	kChanCSRAMask		=	0xffff			};

		// bit definitions in parameter CHANCSRB: control/status reg B for channel X (reserved)
		enum EMrbChanCSRB		{};

		// bit definitions: software status (data)
		enum EMrbDGFStatusDBits	{
											kSystemFPGACodeRead		=	BIT(2),
											kFippiFPGARevDCodeRead	=	BIT(3),
											kFippiFPGARevECodeRead	=	BIT(4),
											kDSPCodeRead 				=	BIT(5),
											kParamNamesRead			=	BIT(6)
										};

		// number of channels per module
		enum					{	kNofChannels		=	4			};
		enum					{	kNofMCAPages		=	8			};
		enum					{	kMCAPageSize		=	32 * 1024	};
		enum					{	kUntrigTraceLength	=	8192		};
		enum					{	kChannelPattern	=	0xF };

		// module revision
		enum EMrbDGFRevision	{	kRevUnknown 		= -1,
									kOrigRevD			= 3,
									kOrigRevE			= 4,
									kRevD				= 0,
									kRevE				= 1
								};
		enum					{	kNofRevs			= kRevE - kRevD + 1 };

	public:
		TMrbDGFData();				 					// default ctor

		~TMrbDGFData() { Reset(); };					// default dtor

		// FPGA section
		Int_t ReadFPGACodeBinary(EMrbFPGAType FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE);	// read FPGA data (binary) from file
		Int_t ReadFPGACodeBinary(const Char_t * FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE);
		Int_t ReadFPGACodeAscii(EMrbFPGAType FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE); 	// read FPGA data (ascii) from file
		Int_t ReadFPGACodeAscii(const Char_t * FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE);
		Int_t ReadFPGACode(EMrbFPGAType FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE);	// read FPGA data depending on file ext
		Int_t ReadFPGACode(const Char_t * FPGAType, const Char_t * CodeFile = "", Int_t Rev = kRevUnknown, Bool_t Forced = kFALSE);
		Bool_t FPGACodeRead(EMrbFPGAType FPGAType, Int_t Rev = kRevUnknown);
		Bool_t FPGACodeRead(const Char_t * FPGAType, Int_t Rev = kRevUnknown);
		inline Int_t GetFPGACodeSize(EMrbFPGAType FPGAType, Int_t Rev = kRevUnknown) {
			return (FPGAType == kSystemFPGA ? fSystemFPGASize : fFippiFPGASize[Rev]);
		};
		inline UShort_t * GetFPGACodeAddr(EMrbFPGAType FPGAType, Int_t Rev = kRevUnknown) {
			return (FPGAType == kSystemFPGA ? (UShort_t *) fSystemFPGACode.GetArray() : (UShort_t *) fFippiFPGACode[Rev].GetArray());
		};
				
		// DSP section
		Int_t ReadDSPCodeBinary(const Char_t * CodeFile = "", Bool_t Forced = kFALSE); 	// read DSP data (binary) from file
		Int_t ReadDSPCodeAscii(const Char_t * CodeFile = "", Bool_t Forced = kFALSE);	// read DSP data (ascii) from file
		Int_t ReadDSPCode(const Char_t * CodeFile = "", Bool_t Forced = kFALSE);		// read DSP data depending on file ext
		inline Bool_t DSPCodeRead() { return((fStatusD & kDSPCodeRead) != 0); };
		inline Int_t GetDSPCodeSize() { return(fDSPSize); };
		UShort_t * GetDSPCodeAddr() { return((UShort_t *) fDSPCode.GetArray()); };
		
		
		// parameter section
		Int_t ReadNameTable(const Char_t * ParamFile = "", Bool_t Forced = kFALSE);		// read parameter names from file
		inline TMrbNamedX * FindParam(const Char_t * ParamName) { return((TMrbNamedX *) fParamNames.FindByName(ParamName)); };
		inline TMrbNamedX * FindParam(Int_t Offset) { return((TMrbNamedX *) fParamNames.FindByIndex(Offset)); };
		TMrbNamedX * FindParam(Int_t Channel, const Char_t * ParamName);

		inline TMrbNamedX * FirstParam() { return((TMrbNamedX *) fParamNames.First()); };
		inline TMrbNamedX * NextParam(TMrbNamedX * Param) { return((TMrbNamedX *) fParamNames.After(Param)); };
		inline Bool_t ParamNamesRead() { return((fStatusD & kParamNamesRead) != 0); };
		inline Int_t GetNofParams() { return(fNofParams); };
		inline TMrbLofNamedX * GetLofParamNames() { return(&fParamNames); };
		
		inline Int_t GetXiaRelease() { return(fXiaRelease); };					// return xia release
		void PrintXiaRelease();
		
		virtual void Print();												// show status
		inline void SetLocal(Bool_t LocalFlag) { fLocalData = LocalFlag; };	// local data base?

		inline void SetVerboseMode(Bool_t VerboseFlag = kTRUE) { fVerboseMode = VerboseFlag;};	// verbose mode on/off
		inline Bool_t IsVerbose() { return(fVerboseMode); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbDGFData.html&"); };

	public:																// public lists of bits & bytes
		TMrbLofNamedX fLofDGFStatusDBits;								// soft status (data)
		TMrbLofNamedX fLofDGFFileTypes;								// file types for DSP and FPGA code files
		TMrbLofNamedX fLofFPGATypes;								// FPGA types: system or fippi
		TMrbLofNamedX fLofCamacCSRBits;								// camac control & status
		TMrbLofNamedX fLofCamacICSRBits;							// ... (system)
		TMrbLofNamedX fLofModCSRABits; 								// module control & status A
		TMrbLofNamedX fLofChanCSRABits;								// channel control & status A
		TMrbLofNamedX fLofRunTasks;									// list of run tasks	
		TMrbLofNamedX fLofControlTasks;								// list of control tasks

	protected:
		void Reset();												// reset data base
		void Setup();												// initialize data base
		Bool_t CheckXiaRelease();									// check release
		
	protected:
		Bool_t fVerboseMode;				// common verbosity flag for all DGF modules

		UInt_t fXiaRelease;					// xia release
	
		UInt_t fStatusD;					// soft status (data)
		Bool_t fLocalData;					// kTRUE if data base is local

		TString fDSPFile;					// name of DSP code file
		EMrbDGFFileType fDSPType;			// type of DSP file: ascii or binary
		Int_t fDSPSize;						// actual size of DSP code
		TArrayS fDSPCode;					// DSP code: 32 bit words, stored in 2 * UShort_t

		TString fSystemFPGAFile;			// name of FPGA code file (SYSTEM)
		EMrbDGFFileType fSystemFPGAType;	// type of FPGA file: ascii or binary
		Int_t fSystemFPGASize;				// actual size of FPGA code
		TArrayS fSystemFPGACode;			// FPGA code: byte oriented, stored in UShort_t(LSB), MSB=0

		TString fFippiFPGAFile[kNofRevs];	// name of FPGA code file (FIPPI)
		EMrbDGFFileType fFippiFPGAType[kNofRevs];	// type of FPGA file: ascii or binary
		Int_t fFippiFPGASize[kNofRevs];		// actual size of FPGA code
		TArrayS fFippiFPGACode[kNofRevs];	// FPGA code: byte oriented, stored in UShort_t(LSB), MSB=0

		TString fParamFile; 				// file containing param names
		Int_t fNofParams;					// number of params
		TMrbLofNamedX fParamNames; 			// DSP param table, pairs of (ParamName, Offset)

	ClassDef(TMrbDGFData, 1)		// [XIA DGF-4C] Data base common to all DGF modules
};

#endif
