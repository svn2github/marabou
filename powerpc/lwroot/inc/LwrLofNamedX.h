#ifndef __LwrLofNamedX_h__
#define __LwrLofNamedX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLofNamedX.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMrbLofNamedX<br>
//! 				A list of TMrbnamedX objects
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrList.h"
#include "LwrString.h"
#include "LwrLynxOsSystem.h"
#include "LwrNamedX.h"

class TMrbLofNamedX: public TList {

	public:
		enum EMrbFindMode		{	kFindExact		=	BIT(0), 	// needs exact matching of key names
									kFindUnique 	= 	BIT(1), 	// key names may be abbreviated uniquely
									kFindIgnoreCase =	BIT(2)		// ignore case while comparing key names
								};

		enum					{	kPatternBit 	=	BIT(31)	};
		enum					{	kIllIndexBit	=	BIT(30)	};

	public:
		//! Constructor
		//! \param[in]	Name		-- list name
		//! \param[in]	PatternMode -- kTRUE if bitwise indices
		TMrbLofNamedX(const Char_t * Name = NULL, Bool_t PatternMode = kFALSE);

		//! Constructor
		//! \param[in]	NamedX		-- ptr to a (NULL-terminated) list of structs {index, name, title}
		//! \param[in]	PatternMode -- kTRUE if bitwise indices
		TMrbLofNamedX(const SMrbNamedX * NamedX, Bool_t PatternMode = kFALSE); 		// explicit ctor

		//! Constructor
		//! \param[in]	NamedX		-- ptr to a (NULL-terminated) list of structs {index, name}
		//! \param[in]	PatternMode -- kTRUE if bitwise indices
		TMrbLofNamedX(const SMrbNamedXShort * NamedX, Bool_t PatternMode = kFALSE);

		//! Destructor
		~TMrbLofNamedX() { this->Delete(); };										// dtor: delete heap objects

		//! Set list name
		//! \param[in]	Name	-- list name
		inline void SetName(const Char_t * Name) { fName = Name; };

		//! Get list name
		//! \retval	Name	-- list name
		inline const Char_t * GetName() const { return(fName); };

		//! Set list title
		//! \param[in]	Title	-- list title
		inline void SetTitle(const Char_t * Title) { fTitle = Title; };

		//! Get list title
		//! \retval	Title	-- list title
		inline const Char_t * GetTitle() const { return(fTitle); };

		//! Set name and title of the list
		//! \param[in]	Name	-- list name
		//! \param[in]	Title	-- list title
		inline void SetObject(const Char_t * Name, const Char_t * Title) { fName = Name; fTitle = Title; };

		//! Set pattern mode
		//! \param[in]	Flag	-- treats indices bitwise if kTRUE
		inline void SetPatternMode(Bool_t Flag = kTRUE) { fPatternMode = Flag; };

		//! Test if pattern mode
		//! \return 	TRUE or FALSE
		inline Bool_t IsPatternMode() const { return(fPatternMode); };

		void AddNamedX(const SMrbNamedX * NamedX);

		void AddNamedX(const SMrbNamedXShort * NamedX);

		void AddNamedX(TMrbNamedX * NamedX);

		void AddNamedX(TMrbLofNamedX * LofNamedX);

		TMrbNamedX * AddNamedX(Int_t Index, const Char_t * Name, const Char_t * Title = "", TObject * Object = NULL);

																					// find an index by its name
		TMrbNamedX * FindByName(const Char_t * ShortName, UInt_t FindMode = TMrbLofNamedX::kFindExact) const;
		TMrbNamedX * FindByIndex(Int_t Index, Int_t Mask = 0xFFFFFFFF) const; 			// find an index
		
		UInt_t FindPattern(const Char_t * IndexString,								// find a compound index
							UInt_t FindMode = TMrbLofNamedX::kFindExact,
							const Char_t * Delim = ":") const;

		UInt_t CheckPattern(const Char_t * ClassName, const Char_t * Method, const Char_t * IndexString,
							const SMrbNamedX * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);

		//! Check pattern (a ":"-separated string) against list elements
		//! \param[in]	ClassName	-- calling class
		//! \param[in]	Method		-- calling method
		//! \param[in]	IndexString -- string to be checked
		//! \param[in]	NamedX		-- [opt] list elements to be added \a before checking
		//! \param[in]	Mode		-- checking mode
		inline UInt_t CheckPatternShort(const Char_t * ClassName, const Char_t * Method,
							const Char_t * IndexString,
							const SMrbNamedXShort * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase) {
			if (NamedX != NULL) this->AddNamedX(NamedX);
			return(this->CheckPattern(ClassName, Method, IndexString, NULL, Mode));
		};

		const Char_t * Pattern2String(TString & IndexString, UInt_t Pattern, const Char_t * Delim = ":") const;

		UInt_t GetMask() const;											// return mask of all bits in list

		void Print(ostream & Out, const Char_t * Prefix = "", UInt_t Mask = 0xffffffff) const;	// show list of indices

		//! Print list contents
		inline void Print() const { Print(cout, "", 0xffffffff); };

		void PrintNames(ostream & Out = cout, 						// print names in one line
						const Char_t * Prefix = "",
						UInt_t Mask = 0xffffffff,
						Bool_t CrFlag = kTRUE) const;


		inline const Char_t * ClassName() const { return "TMrbLofNamedX"; };

	protected:
		Bool_t fPatternMode;	//!< kTRUE if bitwise indices
		TString fName;			//!< list name
		TString fTitle; 		//!< list title
};

#endif
