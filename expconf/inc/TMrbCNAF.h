#ifndef __TMrbCNAF_h__
#define __TMrbCNAF_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCNAF.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCNAF             -- base class to handle cnafs
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
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

#include "TNamed.h"
#include "TString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF
// Purpose:        Handle Camac CNAF definitions
// Description:    Converts Camac [B]CNAFs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCNAF : public TNamed {

	public:
		enum EMrbCNAF		{	kCnafBranch 	=	BIT(0),			// cnaf code names
								kCnafCrate		=	BIT(1),
								kCnafStation	=	BIT(2),
								kCnafAddr		=	BIT(3),
								kCnafFunction	=	BIT(4),
								kCnafData		=	BIT(5),
								kCnafRange		=	BIT(6),
								kCnafAny		=	0xffffffff
							};

	public:
		TMrbCNAF() {						// ctor: allow any cnaf part to be defined
			fMayBeSet = kCnafAny;
			fMustBeSet = kCnafStation;			// at least station number must be given
			fIsSet = 0;
		};

		~TMrbCNAF() {}; 				// default dtor

		Bool_t Ascii2Int(const Char_t * Cnaf); 	// decode B.C.N.A.F from ascii to int values
		UInt_t Int2Camac() const { return(0); }; 		// encode int values to hardware format
		const Char_t * Int2Ascii(TString & CnafAscii, Bool_t StripBorC = kTRUE) const;	// convert to ascii

		Bool_t Set(const Char_t * Cnaf, Int_t Value);		// set value by name
		Bool_t Set(EMrbCNAF Cnaf, Int_t Value); 			// set value by enum

		Int_t Get(const Char_t * Cnaf) const;						// get value by name
		Int_t Get(EMrbCNAF Cnaf) const;							// get value by enum

		inline Bool_t SetBit(UInt_t MayBeSet, UInt_t MustBeSet) {  // define legal cnaf components
			fMayBeSet = MayBeSet;
			fMustBeSet = MustBeSet;
			return(kTRUE);
		};

		inline UInt_t IsSet() const { return(fIsSet); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCNAF.html&"); };

	protected:
		TString fAscii; 			// ascii representation

		UInt_t fMayBeSet; 			// bit mask to describe which cnaf components MAY be set
		UInt_t fMustBeSet;			// bit mask to describe which components MUST be set
		UInt_t fIsSet;				// bit mask to show which cnaf components have ACTUALLY been set

		Int_t fBranch;				// Camac branch: Bxx
		Int_t fCrate;				// Camac crate: Cxx
		Int_t fStation; 			// Camac station: N1 .. N31
		Int_t fAddr;				// Camac subaddress: A0 .. A15
		Int_t fAddr2;				// ... address range
		Int_t fFunction;			// Camac function: F0 .. F31
		Int_t fData;				// Camac data

	ClassDef(TMrbCNAF, 1)		// [Config] Structure of a CAMAC command C.N.A.F
};

#endif
