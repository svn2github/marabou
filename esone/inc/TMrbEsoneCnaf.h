#ifndef __TMrbEsoneCnaf_h__
#define __TMrbEsoneCnaf_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/inc/TMrbEsoneCnaf.h
// Purpose:        Interface to the ESONE client library
// Class:          TMrbEsoneCnaf         -- C.N.A.F handling
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

#include "TMrbString.h"
#include "TMrbEsoneCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf
// Purpose:        Handle Camac CNAF definitions
// Methods:        Ascii2Int  -- decode B.C.N.A.F to int values
// Description:    Converts Camac [B]CNAFs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEsoneCnaf : public TObject {

	public:
		enum EMrbEsoneCnaf		{	kCnafBranch 		=	BIT(0),			// cnaf code names
									kCnafCrate			=	BIT(1),
									kCnafStation		=	BIT(2),
									kCnafAddr			=	BIT(3),
									kCnafFunction		=	BIT(4),
									kCnafData			=	BIT(5),
									kCnafQ				=	BIT(6),
									kCnafX				=	BIT(7)
								};

		enum					{	kEsoneCnafBits		=	kCnafCrate | kCnafStation | kCnafAddr | kCnafFunction | kCnafData	};

	public:
		TMrbEsoneCnaf() { Reset(); };						// default ctor
		~TMrbEsoneCnaf() {}; 								// default dtor

		Bool_t Ascii2Int(const Char_t * Cnaf); 				// decode C.N.A.F from ascii to int values
		const Char_t * Int2Ascii(Bool_t DataFlag = kTRUE);	// convert to ascii

		Bool_t Set(Int_t Crate, Int_t Station = -1, Int_t Subaddr = -1, Int_t Function = -1, Int_t Data = kEsoneNoData);	// set cnaf
		Bool_t SetC(Int_t Crate);
		Bool_t SetN(Int_t Station);
		Bool_t SetA(Int_t Addr);
		Bool_t SetF(Int_t Function);
		Bool_t SetData(Int_t Data, EMrbEsoneCnafType Type = kCnafTypeUndefined);
		inline void ClearStatus() { fStatus = 0; };
		Bool_t ClearData(EMrbEsoneCnafType Type = kCnafTypeUndefined);
		inline void SetX() { fStatus |= kEsoneX; };
		inline void SetQ() { fStatus |= kEsoneQ; };
		inline void SetXQ() { fStatus |= (kEsoneQ|kEsoneX); };
		inline void SetError() { fStatus |= kEsoneError; };

		inline Int_t GetC() { return(fCrate); };									// get cnaf
		inline Int_t GetN() { return(fStation); };
		inline Int_t GetA() { return(fAddr); };
		inline Int_t GetF() { return(fFunction); };
		Int_t GetData(EMrbEsoneCnafType Type = kCnafTypeUndefined);
		Bool_t GetData(Int_t & DataRead, Int_t & DataWrite);
		inline Bool_t GetX() { return(IS_X(fStatus)); };
		inline Bool_t GetQ() { return(IS_Q(fStatus)); };
		inline Bool_t IsError() { return(IS_ERROR(fStatus)); };

		inline EMrbEsoneCnafType GetType() { return(fType); };

		inline EMrbEsoneCamacAction GetAction() { return(fAction); };	// return action
		inline void SetAction(EMrbEsoneCamacAction Action) { fAction = Action; };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEsoneCnaf.html&"); };

		void Reset();										// reset data
		Bool_t CheckCnaf(UInt_t CnafBits = kEsoneCnafBits);	// check for missing cnaf components
		Bool_t CheckCnaf(UInt_t CnafBit, Int_t Data);		// check for valid cnaf component

		inline Bool_t IsWrite() { return(fType == kCnafTypeWrite); };
		inline Bool_t IsRead() { return(fType == kCnafTypeRead); };
		inline Bool_t IsControl() { return(fType == kCnafTypeControl); };
		inline Bool_t IsReadStatus() { return(fType == kCnafTypeReadStatus); };

	protected:
		TMrbString fAscii; 				// ascii representation

		Int_t fCrate;					// Camac crate: Cxx
		Int_t fStation; 				// Camac station: N1 .. N31
		Int_t fAddr;					// Camac subaddress: A0 .. A15
		Int_t fFunction;				// Camac function: F0 .. F31
		Int_t fDataRead;				// Camac data (read)
		Int_t fDataWrite;				// Camac data (write)

		UInt_t fStatus;					// X, Q, error

		EMrbEsoneCamacAction fAction;	// Camac action

		EMrbEsoneCnafType fType; 		// Camac read / control / write

	ClassDef(TMrbEsoneCnaf, 1)		// [CAMAC Access] Camac command C.N.A.F
};

#endif
