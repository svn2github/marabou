#ifndef __TMrbEsoneCNAF_h__
#define __TMrbEsoneCNAF_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/inc/TMrbEsoneCNAF.h
// Purpose:        Interface to the ESONE client library
// Class:          TMrbEsoneCNAF         -- C.N.A.F handling
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

#include "TMrbEsoneCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF
// Purpose:        Handle Camac CNAF definitions
// Methods:        Ascii2Int  -- decode B.C.N.A.F to int values
// Description:    Converts Camac [B]CNAFs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEsoneCNAF : public TObject {

	public:
		enum EMrbEsoneCNAF		{	kCnafBranch 		=	BIT(0),			// cnaf code names
									kCnafCrate			=	BIT(1),
									kCnafStation		=	BIT(2),
									kCnafAddr			=	BIT(3),
									kCnafFunction		=	BIT(4),
									kCnafData			=	BIT(5),
								};

		enum EMrbCnafType		{	kCnafTypeUndefined	=	0,				// cnaf types
									kCnafTypeRead,
									kCnafTypeControl,
									kCnafTypeWrite,
									kCnafTypeReadStatus
								};

	public:
		TMrbEsoneCNAF() { Reset(); };						// default ctor
		~TMrbEsoneCNAF() {}; 								// default dtor

		Bool_t Ascii2Int(const Char_t * Cnaf); 				// decode C.N.A.F from ascii to int values
		const Char_t * Int2Ascii(Bool_t DataFlag = kTRUE);	// convert to ascii

		CamacReg_t Int2Esone(CamacHost_t HostAddr);			// convert to ESONE format

		Bool_t Set(EMrbEsoneCNAF Cnaf, Int_t Value);		// set value by enum
		Bool_t Set(Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, UInt_t Data = kNoData);	// set complete cnaf
		Int_t Get(EMrbEsoneCNAF Cnaf);						// get value by enum

		inline EMrbCnafType GetType() { return(fType); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEsoneCNAF.html&"); };

		void Reset();										// reset data
		Bool_t CheckCnaf(UInt_t CnafBits = kNoData);		// check for missing cnaf components
		Bool_t CheckCnaf(UInt_t CnafBit, Int_t Data);		// check for valid cnaf component

		inline Bool_t IsWrite() { return(fType == TMrbEsoneCNAF::kCnafTypeWrite); };
		inline Bool_t IsRead() { return(fType == TMrbEsoneCNAF::kCnafTypeRead); };
		inline Bool_t IsControl() { return(fType == TMrbEsoneCNAF::kCnafTypeControl); };

	protected:
		TString fAscii; 				// ascii representation

		Int_t fCrate;					// Camac crate: Cxx
		Int_t fStation; 				// Camac station: N1 .. N31
		Int_t fAddr;					// Camac subaddress: A0 .. A15
		Int_t fFunction;				// Camac function: F0 .. F31
		UInt_t fData;					// Camac data

		EMrbCnafType fType; 			// Camac read / control / write

	ClassDef(TMrbEsoneCNAF, 1)		// [CAMAC Access] Camac command C.N.A.F
};

#endif
