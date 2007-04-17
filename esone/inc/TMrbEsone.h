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
// Revision:       $Id: TMrbEsone.h,v 1.10 2007-04-17 11:22:02 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"
#include "TArrayI.h"
#include "TArrayL.h"

#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TMrbEsoneCommon.h"
#include "TMrbEsoneCnaf.h"

#include "TMrbC2Lynx.h"

#define C_CBV	5
#define C_CC32	11

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone
// Purpose:        Esone client within MARaBOU
// Description:    Creates an ESONE client for MARaBOU.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEsone : public TObject {

	public:
		enum EMrbEsoneServer	{	kES_MBS 					=	0,
									kES_MARABOU					=	1
								};
		
		enum EMrbCamacController	{	kCC_CBV 				=	C_CBV,
										kCC_CC32				=	C_CC32
									};
		
		enum					{	kBroadCastSetMask_N_CC32 	=	N(26)	};
		enum					{	kBroadCastExecCmd_N_CC32 	=	N(25)	};
		enum					{	kFastCamac_F	 			=	F(5)	};
		enum					{	kAutoRead_CC32	 			=	BIT(12)	};
		enum					{	kLastError					=	-1		};
	
	public:

		TMrbEsone(Bool_t Offline = kFALSE) {								// default ctor
			if (!this->Reset(Offline)) MakeZombie();
		};		

		TMrbEsone(const Char_t * HostName, Bool_t Offline = kFALSE) {		// ctor: connect to host
			if (this->Reset(Offline)) this->ConnectToHost(HostName);
			else this->MakeZombie();
		};

		~TMrbEsone() {};											// default dtor

		Bool_t StartServer(const Char_t * HostName); 				// start esone server

// cccc: clear dataway (CAMAC-C)
		Bool_t ClearDW(Int_t Crate);		
// cccz: initialize dataway (CAMAC-Z)
		Bool_t InitDW(Int_t Crate);
// ccci: set/clear dataway inhibit (CAMAC-I)
		Bool_t SetDWInhibit(Int_t Crate, Bool_t Flag = kTRUE);
// ctci: test dataway inhibit
		Bool_t DWIsInhibited(Int_t Crate);
// ccopen: connect to camac host
		UInt_t ConnectToHost(const Char_t * HostName, Bool_t Reconnect = kFALSE);
// cfsa/cssa: single camac action
		Bool_t ExecCnaf(const Char_t * Cnaf, Bool_t D16Flag = kFALSE);
		Bool_t ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Bool_t D16Flag = kFALSE);
		Bool_t ExecCnaf(const Char_t * Cnaf, Int_t & Data, Bool_t D16Flag = kFALSE);
		Bool_t ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Int_t & Data, Bool_t D16Flag = kFALSE);
		Bool_t ExecCnaf(TMrbEsoneCnaf & Cnaf, Bool_t D16Flag = kFALSE);
// cfga/csga: multiple camac action
		Int_t ExecCnafList(TObjArray & CnafList, Bool_t D16Flag = kFALSE);
// cfmad/csmad: adress scan
		Int_t AddressScan(const Char_t * Start, const Char_t * Stop, TArrayI & Data, TObjArray & Results, Bool_t D16Flag = kFALSE);
		Int_t AddressScan(Int_t Crate, Int_t Start, Int_t Stop, Int_t Function, TArrayI & Data, TObjArray & Results, Bool_t D16Flag = kFALSE);
		Int_t AddressScan(TMrbEsoneCnaf & Start, TMrbEsoneCnaf & Stop, TArrayI & Data, TObjArray & Results, Bool_t D16Flag = kFALSE);
// cfubc/csubc, cfubr/csubr: block transfer
		Int_t BlockXfer(const Char_t * Cnaf, TArrayI & Data, Int_t Start = 0, Int_t NofWords = -1, Bool_t D16Flag = kFALSE, Bool_t QXfer = kFALSE);
		Int_t BlockXfer(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, TArrayI & Data, Int_t Start = 0, Int_t NofWords = -1, Bool_t D16Flag = kFALSE, Bool_t QXfer = kFALSE);
		Int_t BlockXfer(TMrbEsoneCnaf & Cnaf, TArrayI & Data, Int_t Start = 0, Int_t NofWords = -1, Bool_t D16Flag = kFALSE, Bool_t QXfer = kFALSE);

// ctstat: get status of last camac action
		UInt_t GetStatus(Bool_t & XFlag, Bool_t & QFlag, Int_t & ErrorCode, TString & Error);
		const Char_t * GetError(TString & Error);

// camac host
		TMrbNamedX * GetHost(UInt_t HostAddr); 								// get host by addr
		TMrbNamedX * GetHost(const Char_t * HostName); 						// get host by name
		inline void SetOffline(Bool_t Offline = kTRUE) { fOffline = Offline; };
		inline Bool_t IsOffline() { return(fOffline); };			// hardware access simulated by software?
		
// camac data
		inline void ClearStatus() { fStatus = 0; };
		inline void SetX() { fStatus |= (UInt_t) kEsoneX; };
		inline void SetQ() { fStatus |= (UInt_t) kEsoneQ; };
		inline void SetXQ() { fStatus |= (UInt_t) (kEsoneQ|kEsoneX); };
		inline void SetError() { fStatus |= (UInt_t) kEsoneError; };

		inline Bool_t GetX() { return(IS_X(fStatus)); };
		inline Bool_t GetQ() { return(IS_Q(fStatus)); };
		inline Bool_t IsError() { return(IS_ERROR(fStatus)); };

		inline TMrbNamedX * GetAction() { return(fAction); };	// return action
		inline void SetAction(EMrbEsoneCamacAction Action) { fAction = fLofCamacActions.FindByIndex(Action); };

// broadcast
		Bool_t HasBroadCast();										// test broadcast capability
		Bool_t HasBroadCast(Int_t & NsetMask, Int_t & NexecCmd);
		Bool_t SetBroadCast(Int_t Crate, UInt_t BroadCast);			// write broadcast register
		inline Bool_t ClearBroadCast(Int_t Crate) { return(this->SetBroadCast(Crate, 0)); };
		Bool_t AddToBroadCast(Int_t Crate, Int_t Station); 			// add station to broadcast mask
		Bool_t RemoveFromBroadCast(Int_t Crate, Int_t Station); 	// remove station from broadcast mask
		UInt_t GetBroadCast(Int_t Crate);							// get bc mask register
		inline void UseBroadCast(Bool_t Flag) { fUseBroadCast = Flag; };	// turn bc on/off (if possible)
						
// special camac functions
		Bool_t HasFastCamac();										// test fast camac capability (returns function F)
		Bool_t SetAutoRead(Int_t Crate, Bool_t AutoRead = kTRUE);	// enable/disable auto-read (CC32 only)
		Bool_t ReadDoubleWord(Int_t Crate, Bool_t ReadDW = kTRUE);	// enable/disable 32 bit read (CC32 only)
		
// misc
		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(TMrbEsoneCnaf & Cnaf);									// print single cnaf
		void Print(TObjArray & CnafList, Int_t From = 0, Int_t To = -1);	// print cnaf from list
		void PrintError(const Char_t * EsoneCall, const Char_t * Method);	// print error
		void PrintStatus(const Char_t * Method); 							// print status of last camac action

		Bool_t ReadFromFile(const Char_t * FileName) { return(kTRUE); };	// read cnaf list from file (not yet implemented)

		inline const Char_t * GetServerName() { return(fServerType.GetName()); };	// name/index of camac controller used
		inline Int_t GetServerIndex() { return(fServerType.GetIndex()); };
		inline Bool_t IsMarabouServer() { return(fServerType.GetIndex() == kES_MARABOU); };
		inline Bool_t IsMbsServer() { return(fServerType.GetIndex() == kES_MBS); };
		
		inline TSocket * GetSocket() { return(fSocket); };

		inline const Char_t * GetControllerName() { return(fController.GetName()); };	// name/index of camac controller used
		inline Int_t GetControllerIndex() { return(fController.GetIndex()); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		inline void SetVerboseMode(Bool_t VerboseFlag = kTRUE) { fVerboseMode = VerboseFlag; };	// verbose mode on/off
		inline Bool_t IsVerbose() { return(fVerboseMode); };

		inline void Abort() { fAborted = kTRUE; };
		inline Bool_t IsAborted() { return(fAborted); };

		inline Bool_t IsConnected() { return(fHostAddr > 0); };

		inline void SetSingleStep(Bool_t Flag = kTRUE) { fSingleStep = Flag; };
		inline Bool_t IsSingleStep() { return(fSingleStep); };

	public:
		TMrbLofNamedX fLofCamacActions;								// list of camac actions
		TMrbLofNamedX fLofCnafNames;								// list of camac registers
		TMrbLofNamedX fLofCnafTypes;								// list of cnaf types
		TMrbLofNamedX fLofEsoneServers;								// list of esone servers available
		TMrbLofNamedX fLofControllers;								// list of camac controllers available
		TMrbLofNamedX fLofCamacFunctions;							// list of camac function types

	protected:
		Bool_t StartMbsServer(const Char_t * HostName); 			// start esone server (MBS)
		Bool_t StartMarabouServer(const Char_t * HostName); 		// ... (MARaBOU)

		Bool_t Reset(Bool_t Offline = kFALSE);						// reset internal data base
		Bool_t CheckConnection(const Char_t * Method);				// check if connected to host
		Bool_t CheckCrate(Int_t Crate, const Char_t * Method); 		// check if crate number is legal

		void PrintResults(const Char_t * Method, TMrbEsoneCnaf & Cnaf);
		void PrintResults(const Char_t * Method, TObjArray & CnafList);

	protected:																	// ESONE calls
		Bool_t EsoneCDREG(UInt_t & Handle, Int_t Crate, Int_t Station = 0, Int_t SubAddr = 0);	// [cdreg] encode cnaf + host addr	
		Bool_t EsoneCDREG(UInt_t & Handle, TMrbEsoneCnaf & Cnaf);	
		Bool_t EsoneCDCTRL();													// [cdctrl] define controller	
		Bool_t EsoneCCCC(Int_t Crate);											// [cccc] camac "c" (clear dataway)
		Bool_t EsoneCCOPEN(const Char_t * HostName, UInt_t & HostAddress);		// [ccopen] camac connect
		Bool_t EsoneCCCI(Int_t Crate, Bool_t Inhibit = kTRUE);					// [ccci] camac "i" (inhibit dataway)
		Bool_t EsoneCTCI(Int_t Crate);											// [ctci] test dw inh
		Bool_t EsoneCCCZ(Int_t Crate);											// [cccz] camac "z" (init dataway)
		UInt_t EsoneCTSTAT();													// [ctstat] test status
		Bool_t EsoneCXSA(TMrbEsoneCnaf & Cnaf, Bool_t D16Flag);					// [cssa, cfsa] exec single cnaf
		Int_t EsoneCXGA(TObjArray & CnafList, Bool_t D16Flag);					// [csga, cfga] exec cnaf list
		Int_t EsoneCXMAD(TMrbEsoneCnaf & Start, TMrbEsoneCnaf & Stop,			// [csmad, cfmad] adress scan
									TArrayI & Data, TObjArray & Results, Bool_t D16Flag);
																				// [csubr, cfubr, csubc, cfubc] block xfer
		Int_t EsoneCXUBX(TMrbEsoneCnaf & Cnaf, TArrayI & Data, Int_t First, Int_t Last, Bool_t D16Flag, Bool_t QXfer);

		Bool_t EsoneSpecial(EMrbEsoneCnafType Type, Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Int_t & Data, Bool_t D16Flag);

		const Char_t * EsoneCERROR(TString & ErrMsg, Int_t ErrorCode, Bool_t DateFlag);
 
		inline void SetCB(TArrayI & CB, Int_t Repeat = 0, Int_t Tally = 0, Int_t Lam = 0, Int_t DMA = 0)  {
			CB[0] = Repeat; CB[1] = Tally; CB[2] = Lam; CB[3] = DMA;
		};

	protected:
		Bool_t fVerboseMode;						// kTRUE if verbose mode on
		Bool_t fUseBroadCast;						// kTRUE if broadcast mode is to be used
		Bool_t fOffline;							// access camac hardware?
		Bool_t fAborted;							// kTRUE if operation aborted
		Bool_t fSingleStep; 						// kTRUE if in single step mode

		TMrbNamedX * fAction;						// last camac action
		UInt_t fStatus;								// status of last camac action

		TMrbNamedX fServerType; 					// server type (name+index)
		TMrbNamedX fController; 					// camac controller used (name+index)
		
		TString fHost;								// host name
		TString fHostInet;							// full inet address
		Int_t fHostAddr;							// ESONE host address
		TString fSetupPath;							// where to fond setup files
		TString fServerPath;						// where to start server from
		TString fServerProg;						// server program
		TMrbC2Lynx * fLynxClient;					// client to lynxos/vme
		TSocket * fSocket;							//! connection to server
		Int_t fPort;								// port number
		Int_t fNofCrates;							// number of crates in use
		Int_t fCC32Base; 							// base address (CC32 only)
		Int_t fBufferSize;							// tcp buffer size		

	ClassDef(TMrbEsone, 1)		// [Access to CAMAC] Establish connection via ESONE calls
};

#endif
