#ifndef __TMrbEsone_h__
#define __TMrbEsone_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/inc/TMrbEsone.h
// Purpose:        Interface to the ESONE client library
// Class:          TMrbEsone            -- base class
// Description:    Class definitions to establish an
//                 ESONE client/server connection.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"
#include "TArrayI.h"
#include "TArrayL.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TMrbEsoneCommon.h"
#include "TMrbEsoneCNAF.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone
// Purpose:        Esone client within MARaBOU
// Description:    Creates an ESONE client for MARaBOU.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEsone : public TObject {

	public:
		enum						{	kDefaultCrate		=	999 		};
		enum						{	kMaxNofCnafs		=	1024 		};

		enum EMrbCamacController	{	kCC_CBV 			=	C_CBV,
										kCC_CC32			=	C_CC32
									};
		
		enum EMrbCamacAction		{	kCANone 			= 	0x0,
										kCASingle			=	BIT(0),									
										kCAMultiple			=	BIT(1),									
										kCAAddrScan 		=	BIT(2),
										kCABlockXfer 		=	BIT(3),
										kCAQDriven			=	BIT(4),
										kCA16Bit	 		=	BIT(5),
										kCA24Bit	 		=	BIT(6),
										kCA_cfsa			=	kCASingle | kCA24Bit,
										kCA_cssa			=	kCASingle | kCA16Bit,
										kCA_cfga			=	kCAMultiple | kCA24Bit,
										kCA_csga			=	kCAMultiple | kCA16Bit,
										kCA_cfmad			=	kCAAddrScan | kCA24Bit,
										kCA_csmad			=	kCAAddrScan | kCA16Bit,
										kCA_cfubc			=	kCABlockXfer | kCA24Bit,
										kCA_csubc			=	kCABlockXfer | kCA16Bit,
										kCA_cfubr			=	kCABlockXfer | kCAQDriven | kCA24Bit,
										kCA_csubr			=	kCABlockXfer | kCAQDriven | kCA16Bit
									};

		enum EMrbCamacQX			{	kQFlag				=	BIT(0),
										kXFlag				=	BIT(1)
									};

		enum						{	kBroadCast_N_CC32 	=	N(25)	};
		enum						{	kFastCamac_F_CC32 	=	F(5)	};
		enum						{	kAutoRead_CC32	 	=	BIT(12)	};
		
	public:

		TMrbEsone(Bool_t Offline = kFALSE) {								// default ctor
			if (!Reset(Offline)) MakeZombie();
		};		

		TMrbEsone(const Char_t * HostName, Bool_t Offline = kFALSE) {		// ctor: connect to host
			if (Reset(Offline)) ConnectToHost(HostName);
			else MakeZombie();
		};

		~TMrbEsone() {};											// default dtor

		Bool_t ClearDW(Int_t Crate = TMrbEsone::kDefaultCrate);		// cccc: clear dataway (CAMAC-C)
																	// ccci: set/clear dataway inhibit (CAMAC-I)
		Bool_t SetDWInhibit(Int_t Crate = TMrbEsone::kDefaultCrate, Bool_t Flag = kTRUE);
		Bool_t InitDW(Int_t Crate = TMrbEsone::kDefaultCrate);		// cccz: initialize dataway (CAMAC-Z)

		Bool_t StartServer(const Char_t * HostName, 				// start esone server
				const Char_t * SetupPath = "/mbsusr/marabou/esone",
				const Char_t * MbsVersion = "deve",
				Bool_t PrmFlag = kFALSE);

		CamacHost_t ConnectToHost(const Char_t * HostName); 		// ccopen: connect to camac host

		Int_t ExecCnaf(const Char_t * Cnaf, CamacData_t Data = kNoData, Bool_t D16Flag = kFALSE);		// cfsa/cssa: single camac action
		Int_t ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, CamacData_t Data = kNoData, Bool_t D16Flag = kFALSE);

		Int_t ExecCnafList(Int_t NofCnafs = -1, Bool_t D16Flag = kFALSE);		// cfga/csga: multiple camac action

		Int_t AddressScan(const Char_t * Start, const Char_t * Stop, Int_t WordCount = -1, Bool_t D16Flag = kFALSE);		// cfmad/csmad: adress scan
		Int_t AddressScan(Int_t Crate, Int_t Start, Int_t Stop, CamacFunction_t Function, Int_t WordCount = -1, Bool_t D16Flag = kFALSE);

																	// cfubc/csubc, cfubr/csubr: block transfer
		Int_t BlockXfer(const Char_t * Cnaf, Int_t NofCnafs = -1, Bool_t D16Flag = kFALSE, Bool_t QXfer = kFALSE);
		Int_t BlockXfer(Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, Int_t NofCnafs = -1, Bool_t D16Flag = kFALSE, Bool_t QXfer = kFALSE);

		Bool_t DWInhibit(Int_t Crate = TMrbEsone::kDefaultCrate);	// ctci: test dataway inhibit

		CamacStatus_t GetStatus(Bool_t &, Bool_t &, CamacError_t &, TString &); // ctstat: get status of prev camac action
		const Char_t * GetError();

		void ResetBuffers(Int_t NofCnafs);							// reset internal buffers
		Bool_t SetNofCnafs(Int_t NofCnafs);  						// set actual number of cnafs in list

		inline void ClearTally() { fTally = -1; };					// reset tally count
		Int_t GetTally() { return(fTally); };						// get resulting word count

		void ClearCnaf();											// clear cnaf list
		Bool_t SetCnaf(Int_t Index, const Char_t * Cnaf, CamacData_t Data = kNoData);	// set cnaf list at given index
		Bool_t SetCnaf(Int_t Index, Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, CamacData_t Data = kNoData);
		Bool_t GetCnaf(Int_t Index, TString & Host, TString & Cnaf, CamacData_t & Data); 		// get cnaf list entry
		const Char_t * GetCnaf(Int_t Index = -1, Bool_t DataFlag = kFALSE);
		TMrbNamedX * GetCnafType(Int_t Index = -1);							// get cnaf type
		inline CamacReg_t * GetCAddr() { return((CamacReg_t *) fCnaf.GetArray()); };		// return array addr

		void ClearData();											// clear data buffer
		Bool_t SetData(Int_t Index, CamacData_t Data);				// set data at given index
		Bool_t SetData(CamacDArray & Data, Int_t Size = -1); 		// ... from ext TArrayI
		Bool_t SetData(CamacData_t * Data, Int_t Size = -1); 		// ... from ext UInt_t array
		Bool_t SetData(UShort_t * Data, Int_t Size = -1);	 		// ... from ext UShort_t array
		CamacData_t GetData(Int_t Index = -1);						// get data at given index
		inline CamacData_t * GetDAddr() { return((CamacData_t *) fData.GetArray()); };	// return array addr

		void ClearFunction();										// clear function list
		Bool_t SetFunction(Int_t, CamacFunction_t);					// set function at given index
		CamacFunction_t GetFunction(Int_t Index = -1);				// get function at given index
		inline CamacFunction_t * GetFAddr() { return((CamacFunction_t *) fFunction.GetArray()); };	// return array addr

		void ClearQX(); 						 					// clear Q/X buffer
		Bool_t GetQ(Int_t Index = -1);								// get Q at given index
		Bool_t GetX(Int_t Index = -1);								// get X at given index
		inline CamacQX_t * GetQXAddr() { return((CamacQX_t *) fQX.GetArray()); };	// return array addr

		inline TMrbNamedX * GetAction() { return(fLofCamacActions.FindByIndex(fAction)); }; 	// return last action

		TMrbNamedX * GetHost(CamacHost_t HostAddr); 						// get host by addr
		TMrbNamedX * GetHost(const Char_t * HostName); 						// get host by name

		inline void SetOffline(Bool_t Offline = kTRUE) { fOffline = Offline; };
		inline Bool_t IsOffline() { return(fOffline); };			// hardware access simulated by software?
		
		Bool_t SetAutoRead(Bool_t AutoRead = kTRUE);				// enable/disable auto-read (CC32 only)
		Bool_t ReadDoubleWord(Bool_t ReadDW = kTRUE);				// enable/disable 32 bit read (CC32 only)
		
		Int_t HasBroadCast();										// test broadcast capability (returns station N)
		Bool_t SetBroadCast(Int_t Crate, UInt_t BroadCast);			// write broadcast register
		inline Bool_t ClearBroadCast(Int_t Crate) { return(SetBroadCast(Crate, 0)); };
		Bool_t AddBroadCast(Int_t Station); 						// add station to broadcast mask
		UInt_t GetBroadCast();										// get bc mask register
		inline void UseBroadCast(Bool_t Flag) { fUseBroadCast = Flag; };	// turn bc on/off (if possible)
						
		Int_t HasFastCamac();										// test fast camac capability (returns function F)

		void Print(Int_t From = -1, Int_t To = -1);							// print cnaf(s)
		void PrintError(const Char_t * EsoneCall, const Char_t * Method);	// print error
		void PrintStatus(); 												// print status reg

		Bool_t ReadFromFile(const Char_t * FileName) { return(kTRUE); };	// read cnaf list from file (not yet implemented)

		inline const Char_t * GetControllerName() { return(fController.GetName()); };	// name/index of camac controller used
		inline Int_t GetControllerIndex() { return(fController.GetIndex()); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEsone.html&"); };

		inline void SetVerboseMode(Bool_t VerboseFlag = kTRUE) { fVerboseMode = VerboseFlag; };	// verbose mode on/off
		inline Bool_t IsVerbose() { return(fVerboseMode); };

		inline void Abort() { fAborted = kTRUE; };
		inline Bool_t IsAborted() { return(fAborted); };

		public:
		TMrbLofNamedX fLofCamacActions;								// list of camac actions
		TMrbLofNamedX fLofCNAFNames;								// list of camac registers
		TMrbLofNamedX fLofCNAFTypes;								// list of cnaf types
		TMrbLofNamedX fLofControllers;								// list of camac controllers available

	protected:
		Bool_t Reset(Bool_t Offline = kFALSE);						// reset internal data base
		Bool_t CheckConnection(const Char_t * Method);				// check if connected to host
		Int_t CheckCrate(Int_t Crate, const Char_t * Method); 		// check if crate number is legal
		Bool_t CheckIndex(Int_t Index, const Char_t * Method);		// check index
		Int_t CheckSize(Int_t Size, const Char_t * Method); 		// check data size
		Int_t CheckList(Int_t NofCnafs, const Char_t * Method) { return(0); }; 	// check cnaf list (not yet implemented)

		inline void SetCB(Int_t Repeat = 0, Int_t Tally = 0, Int_t Lam = 0, Int_t DMA = 0)  {
			fCB[0] = Repeat; fCB[1] = Tally; fCB[2] = Lam; fCB[3] = DMA;
		};

		void PrintResults(const Char_t * Method, TMrbEsoneCNAF * c1 = NULL, TMrbEsoneCNAF * c2 = NULL);

	protected:
		Bool_t fVerboseMode;						// kTRUE if verbose mode on
		Bool_t fUseBroadCast;						// kTRUE if broadcast mode is to be used
		Bool_t fOffline;							// access camac hardware?
		Bool_t fAborted;							// kTRUE if operation aborted

		TMrbNamedX fController; 					// camac controller used (name+index)
		
		TString fHost;								// host name
		TString fHostInet;							// full inet address
		CamacHost_t fHostAddr;						// ESONE host address

		TString fCnafAscii; 						// cnaf (ascii representation)

		Int_t fCrate;								// crate number

		TString fError; 							// error message
		CamacError_t fErrorCode;					// error code

		CamacRegArray fCnaf;						// cnafs
		CamacFArray fFunction;						// functions
		CamacDArray fData;							// data
		CamacQXArray fQX;							// Q/X
		Int_t fMaxNofCnafs;							// max number of cnafs
		Int_t fCurNofCnafs;							// actual number of cnafs
		Int_t fTally;								// resulting number of data words

		Int_t fCB[4];								// control block

		EMrbCamacAction fAction;					// type of last action

	ClassDef(TMrbEsone, 1)		// [Access to CAMAC] Establish connection via ESONE calls
};

#endif
