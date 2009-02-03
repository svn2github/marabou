#ifndef __LwrNamedX_h__
#define __LwrNamedX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamedX.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMrbNamedX
//! 				A TNamed object together with an index
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>

#include "LwrTypes.h"
#include "LwrObject.h"
#include "LwrNamed.h"
#include "LwrLynxOsSystem.h"
#include "LwrString.h"

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
			fSortedByName = kFALSE;
			fObject = Obj;
		};

		~TMrbNamedX() {};				// default dtor

																				// return short name + long name (+ index)
		const Char_t * GetFullName(TString & FullName, Int_t Base = 10, Bool_t IndexFlag = kFALSE) const;

		inline Int_t GetIndex() const { return(fIndex); };							// return index
		inline void ChangeIndex(Int_t Index) { fIndex = Index; };					// change index
		inline void AssignObject(TObject * Obj) { fObject = Obj; }; 			// assign an object to this index
		inline TObject * GetAssignedObject() const { return(fObject); };				// return addr of assigned object

		inline void Set(Int_t Index, const Char_t * ShortName = NULL, const Char_t * LongName = NULL) { // set values
			fHasTitle = (LongName != NULL);
			fIndex = Index;
			if (ShortName) this->SetName(ShortName);
			this->SetTitle(LongName ? LongName : this->GetName());
		};
		inline void Set(TMrbNamedX * NamedX) {
			this->Set(NamedX->GetIndex(), NamedX->GetName(), NamedX->GetTitle());
		};
		inline void SetIndex(Int_t Index) { fIndex = Index; };

		inline Bool_t HasTitle() const { return(fHasTitle); }; 				// title (long name) given?

		void Print(ostream & Out, Int_t Base = 10, Bool_t CrFlag = kTRUE) const; 	// print data
		inline void Print(Int_t Base) const { Print(cout, Base, kTRUE); };
		inline void Print() const { Print(cout, 10, kTRUE); };

		inline Bool_t IsSortable() const { return(kTRUE); };							// entries may be sorted
		inline void SortByName(Bool_t SortFlag = kTRUE) { fSortedByName = SortFlag; };	// by name or by index
		inline Bool_t IsSortedByName() const { return(fSortedByName); };
		Int_t Compare(const TObject * Nx) const; 							// how to compare TMrbNamedX objects
		
	protected:
		Bool_t fHasTitle;
		Int_t fIndex;
		Bool_t fSortedByName;
		TObject * fObject;
};

#endif
