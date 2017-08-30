//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLofNamedX.cxx
//! \brief			Light Weight ROOT: TMrbLofNamedX
//! \details		Class definitions for ROOT under LynxOs: TMrbLofNamedX
//!                 A list of TMrbNamedX objects
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.6 $     
//! $Date: 2009-05-26 13:07:42 $
//////////////////////////////////////////////////////////////////////////////


#ifdef CPU_TYPE_RIO2
#include <iostream.h>
#include <iomanip.h>
#include <cstdlib.h>
#else
#include <iostream>
#include <iomanip>
#include <cstdlib>
#endif


#include "LwrString.h"
#include "LwrObjString.h"
#include "LwrLofNamedX.h"
#include "LwrLogger.h"
#include "SetColor.h"

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a TMrbLofNamedX object
//! \param[in]		Name			-- list name
//! \param[in]		PatternMode		-- TRUE if TMrbNamedX indices consist
//! 									of bit patterns
/////////////////////////////////////////////////////////////////////////////

TMrbLofNamedX::TMrbLofNamedX(const Char_t * Name, Bool_t PatternMode)
{
	fName = Name ? Name : this->ClassName();
	fPatternMode = PatternMode;
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a TMrbLofNamedX object<br>
//! 				Will be filled from a list of structs {index,shortName,longName}.<br>
//! 				List must be terminated by an empty element {0,NULL,NULL}.
//! \param[in]		NamedX			-- ptr to a list of structs
//! \param[in]		PatternMode		-- TRUE if TMrbNamedX indices consist
//! 									of bit patterns
/////////////////////////////////////////////////////////////////////////////

TMrbLofNamedX::TMrbLofNamedX(const SMrbNamedX * NamedX, Bool_t PatternMode)
{
	fPatternMode = PatternMode;
	this->AddNamedX(NamedX);
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a TMrbLofNamedX object
//! 				Will be filled from a list of structs {index,shortName}.<br>
//! 				List must be terminated by an empty element {0,NULL}.
//! \param[in]		NamedX			-- ptr to a list of structs
//! \param[in]		PatternMode		-- TRUE if TMrbNamedX indices consist
//! 									of bit patterns
/////////////////////////////////////////////////////////////////////////////

TMrbLofNamedX::TMrbLofNamedX(const SMrbNamedXShort * NamedX, Bool_t PatternMode)
{
	fPatternMode = PatternMode;
	this->AddNamedX(NamedX);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Adds one or more elements to the list.<br>
//! 				Takes elements from a list of structs {index,shortName,longName}.<br>
//! 				Input terminates on an empty element {0,NULL,NULL}.
//! \param[in]		NamedX			-- ptr to a list of structs
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::AddNamedX(const SMrbNamedX * NamedX)
{
	TMrbNamedX * namedX;

	while (NamedX->ShortName) {
		namedX = new TMrbNamedX(NamedX->Index, NamedX->ShortName, NamedX->LongName);
		this->Add(namedX);
		NamedX++;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Adds one or more elements to the list.<br>
//! 				Takes elements from a list of structs {index,shortName}.<br>
//! 				Input terminates on an empty element {0,NULL}.
//! \param[in]		NamedX			-- ptr to a list of structs
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::AddNamedX(const SMrbNamedXShort * NamedX)
{
	TMrbNamedX * namedX;

	while (NamedX->ShortName) {
		namedX = new TMrbNamedX(NamedX->Index, NamedX->ShortName);
		this->Add(namedX);
		NamedX++;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Adds a single element
//! \param[in]		NamedX			-- ptr to TMrbNamedX object
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::AddNamedX(TMrbNamedX * NamedX)
{
	TMrbNamedX * namedX;

	namedX = new TMrbNamedX(	NamedX->GetIndex(),
								NamedX->GetName(),
								NamedX->HasTitle() ? NamedX->GetTitle() : NULL,
								NamedX->GetAssignedObject());
	this->Add(namedX);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Adds a single element
//! \param[in]		Index		-- index
//! \param[in]		Name		-- (short) name
//! \param[in]		Title		-- title (= long name)
//! \param[in]		Object		-- ptr to object to be assigned
//! \retval 		NamedX		-- address of new list element
//////////////////////////////////////////////////////////////////////////////

TMrbNamedX * TMrbLofNamedX::AddNamedX(Int_t Index, const Char_t * Name, const Char_t * Title, TObject * Object)
{
	TMrbNamedX * namedX;

	namedX = new TMrbNamedX(Index, Name, Title, Object);
	this->Add(namedX);
	return(namedX);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Merges two lists
//! \param[in]		LofNamedX		-- list to be appended
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::AddNamedX(TMrbLofNamedX * LofNamedX)
{
	TMrbNamedX * namedX;

	TIterator * iter = LofNamedX->MakeIterator();
	while (namedX = (TMrbNamedX *) iter->Next()) {
		TMrbNamedX * nx = new TMrbNamedX( namedX->GetIndex(),
									namedX->GetName(),
									namedX->HasTitle() ? namedX->GetTitle() : NULL,
									namedX->GetAssignedObject());
		this->Add(nx);
	}
	delete iter;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs list elements to stream
//! \param[in]		Out			-- output stream
//! \param[in]		Prefix		-- prefix to be prepended
//! \param[in]		Mask		-- bit mask to select elements
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::Print(ostream & Out, const Char_t * Prefix, UInt_t Mask) const
{
	Int_t nbits;
	TMrbNamedX * same;
	Int_t index, n;
	Bool_t negFlag;
	Bool_t firstBit;
	TMrbNamedX * k;
	
	if (this->GetEntries() == 0) {
		cerr << setred << this->ClassName() << "::Print(): No indices defined" << setblack << endl;
		return;
	}

	Int_t base = this->IsPatternMode() ? 16 : 10;

	TString fullName;
	TMrbNamedX * xPnt;
	TIterator * iter = this->MakeIterator();
	if (fPatternMode) {
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			if ((xPnt->GetIndex() & Mask) == (UInt_t) xPnt->GetIndex()) {
				Out << Prefix << xPnt->GetFullName(fullName);
				index = xPnt->GetIndex();
				if (index & TMrbLofNamedX::kPatternBit) {
					index = ~index;
					negFlag = kTRUE;
					Out << " (~" << index << ")";
				} else {
					negFlag = kFALSE;
					Out << " (" << index << ")";
				}
				nbits = 0;
				n = index;
				while ((n > 0) && (nbits <= 1)) {
					if (n & 1) nbits++;
					n >>= 1;
				}
				if (negFlag || nbits >= 2) {
					Out << " = ";
					if (negFlag) Out << "~";
					if (nbits >= 2) Out << "( ";
					n = 1;
					firstBit = kTRUE;
					while (index > 0) {
						if (index & 1) {
							k = this->FindByIndex(n);
							if (k != NULL) {
								if (!firstBit) Out << " | ";
								firstBit = kFALSE;
								Out << k->GetName();
							}
						}
						n <<= 1;
						index >>= 1;
					}
					if (nbits >= 2) Out << " )";
				} else {
					same = this->FindByIndex(xPnt->GetIndex());
					if (same != xPnt) Out << " (same as " << same->GetName() << ")";
				}
				Out << endl;
			}
		}
//		delete iter;
	} else {
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			Out << Prefix;
			xPnt->Print(Out, base, kFALSE);
			same = this->FindByIndex(xPnt->GetIndex());
			if (same != xPnt) Out << " (same as " << same->GetName() << ")";
			Out << endl;
		}
//		delete iter;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs element names to stream
//! \param[in]		Out			-- output stream
//! \param[in]		Prefix		-- prefix to be prepended
//! \param[in]		Mask		-- bit mask to select elements
//! \param[in]		CrFlag		-- append \a crlf if TRUE
//////////////////////////////////////////////////////////////////////////////

void TMrbLofNamedX::PrintNames(ostream & Out, const Char_t * Prefix, UInt_t Mask, Bool_t CrFlag) const
{
	TMrbNamedX * xPnt;
	if (fPatternMode) {
		Out << Prefix;
		TIterator * iter = this->MakeIterator();
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			if (xPnt->GetIndex() & TMrbLofNamedX::kPatternBit) {
				if ((~xPnt->GetIndex() & Mask) == 0) Out << xPnt->GetName() << " ";
			} else {
				if ((xPnt->GetIndex() & Mask) == (UInt_t) xPnt->GetIndex()) Out << xPnt->GetName() << " ";
			}
		}
		delete iter;
		if (CrFlag) Out << endl; else Out << flush;
	} else {
		xPnt = this->FindByIndex(Mask);
		if (xPnt != NULL) {
			Out << Prefix << xPnt->GetName() << " ";
			if (CrFlag) Out << endl; else Out << flush;
		}
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Searches for a given element
//! 				\b FindMode can be
//! 				<ul>
//! 				<li>	\a kFindExact		-- needs exact matching of key names
//! 				<li>	\a kFindUnique		-- names may be abbreviated uniquely
//! 				<li>	\a kFindIgnoreCase	-- ignore case while comparing names
//! 				</ul>
//! \param[in]		ShortName	-- element name
//! \param[in]		FindMode	-- mode
//! \retval 		NxElem		-- ptr to element
//////////////////////////////////////////////////////////////////////////////

TMrbNamedX * TMrbLofNamedX::FindByName(const Char_t * ShortName, UInt_t FindMode) const
{
	TString shortName;
	TString xName;
	Int_t lng;

	TString::ECaseCompare cmp = (FindMode & TMrbLofNamedX::kFindIgnoreCase) ? TString::kIgnoreCase : TString::kExact;

	shortName = ShortName;
	
	TMrbNamedX * namedX = NULL;

	TMrbNamedX * xPnt;
	TIterator * iter = this->MakeIterator();
	if (FindMode & TMrbLofNamedX::kFindUnique) {
		lng = shortName.Length();
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			xName = xPnt->GetName();
			xName.Resize(lng);
			if (xName.CompareTo(shortName, cmp) == 0) {
				if (namedX != NULL) { delete iter; return NULL; }
				namedX = xPnt;
			}
		}
		delete iter;
		return(namedX);
	} else {
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			xName = xPnt->GetName();
			if (xName.CompareTo(shortName, cmp) == 0) {
				delete iter;
				return(xPnt);
			}
		}
		delete iter;
		return(NULL);
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Searches for a given index
//! \param[in]		Index		-- element index
//! \param[in]		Mask		-- bit mask
//! \retval 		NxElem		-- ptr to element
//////////////////////////////////////////////////////////////////////////////

TMrbNamedX * TMrbLofNamedX::FindByIndex(Int_t Index, Int_t Mask) const
{
	TMrbNamedX * xPnt;

	TIterator * iter = this->MakeIterator();
	while (xPnt = (TMrbNamedX *) iter->Next()) {
		if ((xPnt->GetIndex() & Mask) == Index) { delete iter; return xPnt; }
	}
	delete iter;
	return(NULL);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Searches for a compound.<br>
//! 				A compound is a string containing one or more names,
//! 				separated by \a Delim. Default separator is ":".<br>
//! 				\b FindMode can be
//! 				<ul>
//! 				<li>	\a kFindExact		-- needs exact matching of key names
//! 				<li>	\a kFindUnique		-- names may be abbreviated uniquely
//! 				<li>	\a kFindIgnoreCase	-- ignore case while comparing names
//! 				</ul>
//! \param[in]		Compound		-- string containing names
//! \param[in]		FindMode		-- mode
//! \param[in]		Delim			-- separator
//! \retval 		Pattern 		-- bit pattern
//////////////////////////////////////////////////////////////////////////////

UInt_t TMrbLofNamedX::FindPattern(const Char_t * Compound, UInt_t FindMode, const Char_t * Delim) const
{
	register Int_t start, end, lng, sepl;
	UInt_t pattern;
	TMrbNamedX * kp;
	TString sepStr;
	TString xString;
	Int_t index;

	if (!fPatternMode) {
		if (fName.IsNull()) {
			cerr << setred << this->ClassName() << "::FindPattern(): List not in pattern mode" << setblack << endl;
		} else {
			cerr << setred << this->ClassName() << "::FindPattern(): Not in pattern mode - " << fName.Data() << setblack << endl;
		}
		return(TMrbLofNamedX::kIllIndexBit);
	}

	sepStr = Delim;
	sepl = sepStr.Length();
	pattern = 0;

	xString = xString.Strip();
	if (xString.IsNull()) return(0);

	for (;;) {
		start = 0;
		end = 0;
		while (end != -1) {
			end = xString.Index(sepStr.Data(), start);
			lng = (end == -1) ? xString.Length() - start : end - start;
			TString subString = xString(start, lng);
			kp = FindByName(subString, FindMode);
			if (kp == NULL) {
				pattern = TMrbLofNamedX::kIllIndexBit;
				break;
			}
			index = kp->GetIndex();
			if (fPatternMode && (index & TMrbLofNamedX::kPatternBit)) {
				pattern &= index;
			} else {
				pattern |= index;
			}
			start = end + sepl;
		}
		if (pattern == TMrbLofNamedX::kIllIndexBit) {
			cerr << setred << this->ClassName() << "::FindPattern(): Illegal index string - " << xString.Data() << setblack << endl;
		} else {
			return(pattern);
		}
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Concatenates names according to bit pattern.<br>
//! \param[out]		Compound		-- string containing names
//! \param[in]		Pattern 		-- bit pattern
//! \param[in]		Separator		-- separator
//! \retval 		Compound 		-- ptr to string
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMrbLofNamedX::Pattern2String(TString & Compound, UInt_t Pattern, const Char_t * Separator) const
{
	if (!fPatternMode) {
		if (fName.IsNull()) {
			cerr << setred << this->ClassName() << "::Pattern2String(): List not in pattern mode" << setblack << endl;
		} else {
			cerr << setred << this->ClassName() << "::Pattern2String(): Not in pattern mode - " << fName.Data() << setblack << endl;
		}
		return("");
	}

	UInt_t indexBits;
	Bool_t cmpl;
	if (Pattern & (UInt_t) TMrbLofNamedX::kPatternBit) {
		indexBits = ~Pattern;
		cmpl = kTRUE;
	} else {
		indexBits = Pattern;
		cmpl = kFALSE;
	}

	TMrbNamedX * index;
	Compound.Resize(0);
	TIterator * iter = this->MakeIterator();
	while (index = (TMrbNamedX *) iter->Next()) {
		UInt_t ind = index->GetIndex();
		if ((indexBits & ind) == ind) {
			if (Compound.Length() > 0) Compound += Separator;
			if (cmpl) Compound += "~";
			Compound += index->GetName();
		}
	}
	delete iter;
	return(Compound.Data());
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Checks compound string and returns corresponding bit pattern.<br>
//! 				Adds elements in \b NamedX \a before checking
//! \param[in]		ClassName		-- class name of caller
//! \param[in]		Method			-- calling method
//! \param[in]		Compound		-- string to be checked
//! \param[in]		NamedX			-- struct containing elements to be added
//! \param[in]		Mode			-- search mode
//! \retval 		Pattern 		-- bit pattern
//////////////////////////////////////////////////////////////////////////////

UInt_t TMrbLofNamedX::CheckPattern(const Char_t * ClassName, const Char_t * Method, const Char_t * Compound, const SMrbNamedX * NamedX, UInt_t Mode)
{
	UInt_t opt;

	if (NamedX != NULL) this->AddNamedX(NamedX);
	this->SetPatternMode();
	opt = this->FindPattern(Compound, Mode);
	if (opt == TMrbLofNamedX::kIllIndexBit) {
		cerr << setred << this->ClassName() << "::CheckPattern(): Illegal compound string - " << Compound << setblack << endl;
		if (this->GetEntries() > 0) {
			cerr << setred << this->ClassName() << "::CheckPattern(): Legal names are - " << setblack << endl;
			this->Print(cerr, "       -> ");
		} else {
			cerr << setred << this->ClassName() << "::CheckPattern(): No elements in list" << setblack << endl;
		}
	}
	return(opt);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns a pattern of all bits in this list
//! \retval 		Pattern 		-- bit pattern
//////////////////////////////////////////////////////////////////////////////

UInt_t TMrbLofNamedX::GetMask() const
{
	if (!IsPatternMode()) return(0xffffffff);

	TMrbNamedX * xPnt;
	UInt_t mask = 0;
	TIterator * iter = this->MakeIterator();
	while (xPnt = (TMrbNamedX *) iter->Next()) mask |= xPnt->GetIndex();
	delete iter;
	return(mask);
}
