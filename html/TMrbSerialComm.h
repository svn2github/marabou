#ifndef __TMrbSerialComm_h__
#define __TMrbSerialComm_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbSerialComm.h
// Purpose:        Class defs to control a serial I/O line
// Description:    Class definitions to control I/O from/to a serial port.
//                 Inspired by a class c_SerialComm written by Mario Schubert
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"

#include "TNamed.h"
#include "TString.h"
#include "TMrbString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm
// Purpose:        Connect to RS232 line
// Description:    Opens a connection to RS232
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSerialComm : public TNamed {

	public:
		enum EMrbParity 	{	kMrbParityNone,
								kMrbParityOdd,
								kMrbParityEven
							};

 	public:
		TMrbSerialComm(const Char_t * Device);
		virtual ~TMrbSerialComm() { this->Close(); };

		void Setup(	Int_t BaudRate,
					Int_t NofDataBits = 8,
					EMrbParity Parity = kMrbParityNone,
					Int_t NofStopBits = 1,
					Bool_t UseRtsCts = kFALSE,
					Bool_t IgnoreCR = kTRUE);
		Bool_t Open();
		Bool_t Close();
		Int_t ReadData(TMrbString & Data);
		Int_t WriteData(const Char_t * Data);
		Int_t WriteSingleBytes(const Char_t * Data, Double_t Delay = -1);
		Int_t WriteWithDelay(const Char_t * Data, Double_t Delay = -1);

		inline void UseRtsCts(Bool_t Flag) { fUseRtsCts = Flag; };
		inline void IgnoreCR(Bool_t Flag) { fIgnoreCR = Flag; };

		inline void SetBaudRate(Int_t BaudRate) { fBaudRate = BaudRate; };
		inline void SetBits(Int_t DataBits, Int_t StopBits) { fNofDataBits = DataBits; fNofStopBits = StopBits; };
		inline void SetParity(EMrbParity Parity) { fParity = Parity; };
		inline void SetDelay(Double_t Delay) { fDelay = Delay; };

		inline void SetLineTerm1(Char_t Term) { fLineTerm1 = Term; };
		inline void SetLineTerm2(Char_t Term) { fLineTerm2 = Term; };
		inline void SetPrefix(const Char_t * Prefix) { fPrefix = Prefix; };
		inline void SetPostfix(const Char_t * Postfix, Bool_t CrFlag = kTRUE) { fPostfix = Postfix; if (CrFlag) fPostfix += "\r"; };

		inline const Char_t * GetPort() { return(this->GetTitle()); };

		inline Bool_t IsOpen() { return(fFd != -1); };

		virtual void Print();			// print settings

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSerialComm.html&"); };

	protected:
		const Char_t * MakePrintable(TString & PrintString, const Char_t * String);

	protected:
		Int_t fFd;						// i/o channel id from open(2)
		void * fOldTIO; 				//! old settings will be saved here
		void * fNewTIO; 				//! new settings
		Int_t fBaudRate;				// baud rate
		Bool_t fUseRtsCts;				// kTRUE if RTS/CTS to be used
		Int_t fNofDataBits; 			// number of data bits
		Int_t fNofStopBits; 			// number of stop bits
		EMrbParity fParity; 			// parity settings
		Bool_t fIgnoreCR;				// kTRUE if <CR> to be ignored
		Double_t fDelay;				// delay [s]
		Char_t fLineTerm1;				// line termination
		Char_t fLineTerm2;				// ...
		TString fPrefix;				// send prefix
		TString fPostfix;				// send postfix
		Int_t fBytesReceived;			// total of bytes received
		Int_t fBytesTransmitted;		// ... transmitted

	ClassDef(TMrbSerialComm, 1) 		// [Utils] I/O from/to serial port
};

#endif
