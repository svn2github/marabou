#ifndef __TMrbNamedX_h__
#define __TMrbNamedX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbNamedX.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbNamedX    -- a TNamed with an index assigned
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TObject.h"
#include "TNamed.h"
#include "TSystem.h"

#include "TMrbString.h"

//_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           SMrbNamedXShort, SMrbNamedX
// Purpose:        Define a named index
// Struct members: Char_t * sname    -- short name
//                 Char_t * lname    -- long name
//                 Int_t index       -- index
// Description:    SMrbNamedX defines a triple (index,sname,lname),
//                 SMrbNamedXShort a pair (index,sname).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

struct sMrbNamedX {
	Int_t Index;
	Char_t * ShortName;
	Char_t * LongName;
};

typedef struct sMrbNamedX SMrbNamedX;

struct sMrbNamedXShort {
	Int_t Index;
	Char_t * ShortName;
};

typedef struct sMrbNamedXShort SMrbNamedXShort;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedX
// Purpose:        A TNamed having an index and an opt. object assigned
// Description:    Defines a triple (index,sname,lname).
//                 a TObject may be assigned to it.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbNamedX: public TNamed {

	public:
		TMrbNamedX() {					// default ctor
			fHasTitle = kFALSE;
			fIndex = -1;
			fObject = NULL;
		};

		TMrbNamedX(Int_t Index,			// ctor: index, short name, long name (opt.), and associated object (opt.)
					const Char_t * ShortName,
					const Char_t * LongName = NULL,
					TObject * Obj = NULL) : TNamed(ShortName, LongName ? LongName : ShortName) { 	// ctor
			fHasTitle = (LongName != NULL);
			fIndex = Index;
			fObject = Obj;
		};

		~TMrbNamedX() {};				// default dtor

																				// return short name + long name (+ index)
		const Char_t * GetFullName(TMrbString & FullName, Int_t Base = 10, Bool_t IndexFlag = kFALSE);

		inline Int_t GetIndex() { return(fIndex); };							// return index
		inline void AssignObject(TObject * Obj) { fObject = Obj; }; 			// assign an object to this index
		inline TObject * GetAssignedObject() { return(fObject); };				// return addr of assigned object

		inline void Set(Int_t Index, const Char_t * ShortName, const Char_t * LongName = NULL) { // set values
			fHasTitle = (LongName != NULL);
			fIndex = Index;
			this->SetName(ShortName);
			this->SetTitle(LongName ? LongName : ShortName);
		};
		inline void Set(TMrbNamedX * NamedX) {
			this->Set(NamedX->GetIndex(), NamedX->GetName(), NamedX->GetTitle());
		};

		inline Bool_t HasTitle() { return(fHasTitle); }; 				// title (long name) given?

		void Print(ostream & Out, Int_t Base = 10, Bool_t CrFlag = kTRUE); 	// print data
		inline void Print(Int_t Base) { Print(cout, Base, kTRUE); };
		inline void Print() { Print(cout, 10, kTRUE); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbNamedX.html&"); };

	protected:
		Bool_t fHasTitle;
		Int_t fIndex;
		TObject * fObject;

	ClassDef(TMrbNamedX, 1) 	// [Utils] A named index: <Index, Name, Title [,Object]>
};

#endif
