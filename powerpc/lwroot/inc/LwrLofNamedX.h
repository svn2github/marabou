#ifndef __LwrLofNamedX_h__
#define __LwrLofNamedX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           LwrLofNamedX.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbLofNamedX    -- a list of named indices
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: LwrLofNamedX.h,v 1.2 2008-07-02 07:03:20 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrList.h"
#include "LwrString.h"
#include "LwrLynxOsSystem.h"
#include "LwrNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX
// Purpose:        Define a list named indices
// Description:    Class to manage a list of TMrbNamedX objects.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofNamedX: public TList {

	public:
		enum EMrbFindMode		{	kFindExact		=	BIT(0), 	// needs exact matching of key names
									kFindUnique 	= 	BIT(1), 	// key names may be abbreviated uniquely
									kFindIgnoreCase =	BIT(2)		// ignore case while comparing key names
								};

		enum					{	kPatternBit 	=	BIT(31)	};
		enum					{	kIllIndexBit	=	BIT(30)	};

	public:
		TMrbLofNamedX(const Char_t * Name = NULL, Bool_t PatternMode = kFALSE); 	// default ctor

		TMrbLofNamedX(const SMrbNamedX * NamedX, Bool_t PatternMode = kFALSE); 		// explicit ctor
		TMrbLofNamedX(const SMrbNamedXShort * NamedX, Bool_t PatternMode = kFALSE);

		~TMrbLofNamedX() { this->Delete(); };										// dtor: delete heap objects

		inline void SetName(const Char_t * Name) { fName = Name; };
		inline const Char_t * GetName() const { return(fName); };
		inline void SetTitle(const Char_t * Title) { fTitle = Title; };
		inline const Char_t * GetTitle() const { return(fTitle); };
		inline void SetObject(const Char_t * Name, const Char_t * Title) { fName = Name; fTitle = Title; };

		inline void SetPatternMode(Bool_t Flag = kTRUE) { fPatternMode = Flag; };
		inline Bool_t IsPatternMode() const { return(fPatternMode); };

		void AddNamedX(const SMrbNamedX * NamedX);							// add one or more indices
		void AddNamedX(const SMrbNamedXShort * NamedX);					 	// ... (short names only)
		void AddNamedX(TMrbNamedX * NamedX); 								// add a single index to list (clone it)
		void AddNamedX(TMrbLofNamedX * LofNamedX); 	 						// append a list of indices
		TMrbNamedX * AddNamedX(Int_t Index, const Char_t * Name, const Char_t * Title = "", TObject * Object = NULL);

																					// find an index by its name
		TMrbNamedX * FindByName(const Char_t * ShortName, UInt_t FindMode = TMrbLofNamedX::kFindExact) const;
		TMrbNamedX * FindByIndex(Int_t Index, Int_t Mask = 0xFFFFFFFF) const; 			// find an index
		
		UInt_t FindPattern(const Char_t * IndexString,								// find a compound index
							UInt_t FindMode = TMrbLofNamedX::kFindExact,
							const Char_t * Delim = ":") const;

		UInt_t CheckPattern(const Char_t * ClassName, const Char_t * Method, const Char_t * IndexString,	// check index string
							const SMrbNamedX * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);

		inline UInt_t CheckPatternShort(const Char_t * ClassName, const Char_t * Method, const Char_t * IndexString,
							const SMrbNamedXShort * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase) {
			if (NamedX != NULL) this->AddNamedX(NamedX);
			return(this->CheckPattern(ClassName, Method, IndexString, NULL, Mode));
		};

		const Char_t * Pattern2String(TString & IndexString, UInt_t Pattern, const Char_t * Delim = ":") const;

		UInt_t GetMask() const;											// return mask of all bits in list

		void Print(ostream & Out, const Char_t * Prefix = "", UInt_t Mask = 0xffffffff) const;	// show list of indices
		inline void Print() const { Print(cout, "", 0xffffffff); };

		void PrintNames(ostream & Out = cout, 						// print names in one line
						const Char_t * Prefix = "",
						UInt_t Mask = 0xffffffff,
						Bool_t CrFlag = kTRUE) const;


		inline const Char_t * ClassName() const { return "TMrbLofNamedX"; };

	protected:
		Bool_t fPatternMode;
		TString fName;
		TString fTitle;
};

#endif
