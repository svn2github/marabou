#ifndef __LwrNamedX_h__
#define __LwrNamedX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamedX.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMrbNamedX<br>
//! 				A TNamed object together with an index
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.6 $
//! $Date: 2010-12-27 09:02:14 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>

#include "LwrTypes.h"
#include "LwrObject.h"
#include "LwrNamed.h"
#include "LwrLynxOsSystem.h"
#include "LwrString.h"

struct sMrbNamedX			//!< a name with an index
{
	Int_t Index;			//!< index
	Char_t * ShortName; 	//!< (short) name
	Char_t * LongName;		//!< title, long name
};

typedef struct sMrbNamedX SMrbNamedX;

struct sMrbNamedXShort {	//!< a name with an index
	Int_t Index;			//!< index
	Char_t * ShortName; 	//!< (short) name
};

typedef struct sMrbNamedXShort SMrbNamedXShort;

class TMrbNamedX: public TNamed {

	public:
		//! Default constructor
		TMrbNamedX() {
			fHasTitle = kFALSE;
			fIndex = -1;
			fObject = NULL;
		};

		//! Constructor
		//! \param[in]	Index		-- index
		//! \param[in]	ShortName	-- (short) name
		//! \param[in]	LongName	-- title, long name
		//! \param[in]	Obj 		-- associated object
		TMrbNamedX(Int_t Index,
					const Char_t * ShortName,
					const Char_t * LongName = NULL,
					TObject * Obj = NULL) : TNamed(ShortName, LongName ? LongName : ShortName) { 	// ctor
			fHasTitle = (LongName != NULL);
			fIndex = Index;
			fSortedByName = kFALSE;
			fObject = Obj;
		};

		//! Destructor
		~TMrbNamedX() {};

																				// return short name + long name (+ index)
		const Char_t * GetFullName(TString & FullName, Int_t Base = 10, Bool_t IndexFlag = kFALSE);

		//! Return index
		inline Int_t GetIndex() { return(fIndex); };

		//! Change index
		//! \param[in]	Index	-- new index
		inline void ChangeIndex(Int_t Index) { fIndex = Index; };

		//! Assign object
		//! \param[in]	Obj 	-- object to be associated
		inline void AssignObject(TObject * Obj) { fObject = Obj; };

		//! Return ptr to assigned object
		inline TObject * GetAssignedObject() const { return(fObject); };

		//! Set data
		//! \param[in]	Index		-- index
		//! \param[in]	ShortName	-- (short) name
		//! \param[in]	LongName	-- title, long name
		inline void Set(Int_t Index, const Char_t * ShortName = NULL, const Char_t * LongName = NULL) {
			fHasTitle = (LongName != NULL);
			fIndex = Index;
			if (ShortName) this->SetName(ShortName);
			this->SetTitle(LongName ? LongName : this->GetName());
		};

		//! Copy data from other object
		//! \param[in]	NamedX		-- object to be copied
		inline void Set(TMrbNamedX * NamedX) {
			this->Set(NamedX->GetIndex(), NamedX->GetName(), NamedX->GetTitle());

		};

		//! Change index
		//! \param[in]	Index	-- new index
		inline void SetIndex(Int_t Index) { fIndex = Index; };

		//! Check if long name given
		//! \return TRUE or FALSE
		inline Bool_t HasTitle() const { return(fHasTitle); };

		void Print(ostream & Out, Int_t Base = 10, Bool_t CrFlag = kTRUE);

		//! Print data to cout
		//! \param[in]	Base	-- numerical base to convert index to
		inline void Print(Int_t Base) { Print(cout, Base, kTRUE); };

		//! Print data to cout, index decimal
		inline void Print() { Print(cout, 10, kTRUE); };

		//! Objects are sortable
		//! \return TRUE
		inline Bool_t IsSortable() const { return(kTRUE); };

		//! Sort list members by name
		//! \param[in]	SortFlag	-- sort by name if kTRUE, by index otherwise
		inline void SortByName(Bool_t SortFlag = kTRUE) { fSortedByName = SortFlag; };

		//! Test if to be sorted by name
		//! \return TRUE or FALSE
		inline Bool_t IsSortedByName() const { return(fSortedByName); };

		Int_t Compare(const TMrbNamedX * Nx); 	// ordering relation

	protected:
		Bool_t fHasTitle;			//!< kTRUE if long name given
		Int_t fIndex;				//!< index
		Bool_t fSortedByName;		//!< kTRUE if to be sorted by name
		TObject * fObject;			//!< assigned object
};

#endif
