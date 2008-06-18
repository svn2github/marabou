//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           LwrLofNamedX.cxx
// Purpose:        MARaBOU utilities: A list of TMrbNamedX objects
// Description:    Implements class methods to manage a list of TMrbNamedX objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: LwrLofNamedX.cxx,v 1.2 2008-06-16 15:00:21 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                      //
// R. Lutter                                                              //
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include "iostream.h"
#include "iomanip.h"

#include "LwrObjString.h"
#include "LwrSetColor.h"
#include "LwrLofNamedX.h"
#include "LwrLogger.h"

TMrbLofNamedX::TMrbLofNamedX(const Char_t * Name, Bool_t PatternMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX
// Purpose:        Define a list of named indices
// Arguments:      SMrbNamedX * Named       -- list of indices to be stored
//                 Bool_t PatternMode       -- kTRUE is list contains bit patterns
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fName = Name ? Name : this->ClassName();
	fPatternMode = PatternMode;
}

TMrbLofNamedX::TMrbLofNamedX(const SMrbNamedX * NamedX, Bool_t PatternMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX
// Purpose:        Define a list of named indices
// Arguments:      SMrbNamedX * Named       -- list of indices to be stored
//                 Bool_t PatternMode       -- kTRUE is list contains bit patterns
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fPatternMode = PatternMode;
	this->AddNamedX(NamedX);
}

TMrbLofNamedX::TMrbLofNamedX(const SMrbNamedXShort * NamedX, Bool_t PatternMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX
// Purpose:        Define a list of named indices
// Arguments:      SMrbNamedXShort * Named  -- list of indices to be stored (short names only)
//                 Bool_t PatternMode       -- kTRUE is list contains bit patterns
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fPatternMode = PatternMode;
	this->AddNamedX(NamedX);
}

void TMrbLofNamedX::AddNamedX(const SMrbNamedX * NamedX) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Add one ore more indices to list
// Arguments:      SMrbNamedX * NamedX     -- list of indices to be added
// Results:        --
// Exceptions:
// Description:    Adds one or more indices to the list.
//                 Input list must be terminated by element {0, NULL, NULL}.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	while (NamedX->ShortName) {
		namedX = new TMrbNamedX(NamedX->Index, NamedX->ShortName, NamedX->LongName);
		this->Add(namedX);
		NamedX++;
	}
}

void TMrbLofNamedX::AddNamedX(const SMrbNamedXShort * NamedX) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Add one ore more indices to list
// Arguments:      SMrbNamedX * NamedX     -- list of indices to be added
//                                            (short names only)
// Results:        --
// Exceptions:
// Description:    Adds one or more indices to the list.
//                 Input list must be terminated by element {0, NULL}.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	while (NamedX->ShortName) {
		namedX = new TMrbNamedX(NamedX->Index, NamedX->ShortName);
		this->Add(namedX);
		NamedX++;
	}
}

void TMrbLofNamedX::AddNamedX(TMrbNamedX * NamedX) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Add a single index
// Arguments:      TMrbNamedX * NamedX       -- pointer to named index
// Results:        --
// Exceptions:
// Description:    Adds a single index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	namedX = new TMrbNamedX(	NamedX->GetIndex(),
								NamedX->GetName(),
								NamedX->HasTitle() ? NamedX->GetTitle() : NULL,
								NamedX->GetAssignedObject());
	this->Add(namedX);
}

TMrbNamedX * TMrbLofNamedX::AddNamedX(Int_t Index, const Char_t * Name, const Char_t * Title, TObject * Object) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Add a single index
// Arguments:      Int_t Index               -- index
//                 const Char_t * Name       -- name
//                 const Char_t * Title      -- title
//                 TObject * Object          -- object to be assigned
// Results:        TMrbNamedX * NamedX       -- address of new list entry
// Exceptions:
// Description:    Adds a single index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	namedX = new TMrbNamedX(Index, Name, Title, Object);
	this->Add(namedX);
	return(namedX);
}

void TMrbLofNamedX::AddNamedX(TMrbLofNamedX * LofNamedX) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Append a list of named indices
// Arguments:      TMrbLofNamedX * LofNamedX     -- list of indices to be added.
// Results:        --
// Exceptions:
// Description:    Adds an existing index list to the list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	TIterator * iter = LofNamedX->MakeIterator();
	while (namedX = (TMrbNamedX *) iter->Next()) {
		TMrbNamedX * nx = new TMrbNamedX( namedX->GetIndex(),
									namedX->GetName(),
									namedX->HasTitle() ? namedX->GetTitle() : NULL,
									namedX->GetAssignedObject());
		this->Add(nx);
	}
}

void TMrbLofNamedX::Print(ostream & Out, const Char_t * Prefix, UInt_t Mask) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::Print
// Purpose:        Print index data
// Arguments:      ostream & Out   -- where to send it
//                 Char_t * Prefix -- prefix to be prepended
//                 UInt_t Mask     -- bit mask
// Results:        --
// Exceptions:
// Description:    Outputs index data to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

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
	} else {
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			Out << Prefix;
			xPnt->Print(Out, base, kFALSE);
			same = this->FindByIndex(xPnt->GetIndex());
			if (same != xPnt) Out << " (same as " << same->GetName() << ")";
			Out << endl;
		}
	}
}

void TMrbLofNamedX::PrintNames(ostream & Out, const Char_t * Prefix, UInt_t Mask, Bool_t CrFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::PrintNames
// Purpose:        Print index names
// Arguments:      ostream & Out   -- where to send it
//                 Char_t * Prefix -- prefix to be prepended
//                 UInt_t Mask     -- bit mask
//                 Bool_t CrFlag   -- kTRUE outputs <crlf>
// Results:        --
// Exceptions:
// Description:    Outputs index names to stream Out.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * xPnt;
	TIterator * iter = this->MakeIterator();
	if (fPatternMode) {
		Out << Prefix;
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			if (xPnt->GetIndex() & TMrbLofNamedX::kPatternBit) {
				if ((~xPnt->GetIndex() & Mask) == 0) Out << xPnt->GetName() << " ";
			} else {
				if ((xPnt->GetIndex() & Mask) == (UInt_t) xPnt->GetIndex()) Out << xPnt->GetName() << " ";
			}
		}
		if (CrFlag) Out << endl; else Out << flush;
	} else {
		xPnt = this->FindByIndex(Mask);
		if (xPnt != NULL) {
			Out << Prefix << xPnt->GetName() << " ";
			if (CrFlag) Out << endl; else Out << flush;
		}
	}
}

TMrbNamedX * TMrbLofNamedX::FindByName(const Char_t * ShortName, UInt_t FindMode) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::FindByName
// Purpose:        Find an index by its name
// Arguments:      Char_t * ShortName    -- short name
//                 UInt_t FindMode       -- method how to search
// Results:        TMrbNamedX * NamedX   -- result
// Exceptions:
// Description:    Searches for a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

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
				if (namedX != NULL) return(NULL);
				namedX = xPnt;
			}
		}
		return(namedX);
	} else {
		while (xPnt = (TMrbNamedX *) iter->Next()) {
			xName = xPnt->GetName();
			if (xName.CompareTo(shortName, cmp) == 0) {
				return(xPnt);
			}
		}
		return(NULL);
	}
}

TMrbNamedX * TMrbLofNamedX::FindByIndex(Int_t Index, Int_t Mask) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::FindByIndex
// Purpose:        Find an named index by its index number
// Arguments:      Int_t Index          -- index
//                 Int_t Mask           -- mask
// Results:        TMrbNamedX * NamedX  -- pointer to index data
// Exceptions:
// Description:    Searches for a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * xPnt;

	TIterator * iter = this->MakeIterator();
	while (xPnt = (TMrbNamedX *) iter->Next()) {
		if ((xPnt->GetIndex() & Mask) == Index) return(xPnt);
	}
	return(NULL);
}

UInt_t TMrbLofNamedX::FindPattern(const Char_t * IndexString, UInt_t FindMode, const Char_t * Separator) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::FindPattern
// Purpose:        Find a compound index
// Arguments:      Char_t * IndexString  -- index string
//                 UInt_t FindMode       -- method how to search
//                 Char_t * Separator    -- separating string
// Results:        UInt_t Pattern        -- pattern containing index bits
// Exceptions:
// Description:    Searches for a compound index. Default delimiter is ":".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register Int_t start, end, lng, sepl;
	UInt_t pattern;
	TMrbNamedX * kp;
	TString sepStr;
	TString xString;
	Int_t index;

	if (!fPatternMode) {
		if (fName.IsNull()) {
			cerr << setred << this->ClassName() << "::FindPattern(): Index list not in pattern mode" << setblack << endl;
		} else {
			cerr << setred << this->ClassName() << "::FindPattern(): Not in pattern mode" << setblack << endl;
		}
		return(TMrbLofNamedX::kIllIndexBit);
	}

	sepStr = Separator;
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

const Char_t * TMrbLofNamedX::Pattern2String(TString & IndexString,
										UInt_t Pattern, const Char_t * Separator) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::Pattern2String
// Purpose:        Build a string from a bit pattern
// Arguments:      TString & IndexString  -- resulting index string
//                 UInt_t Pattern         -- pattern containing index bits
//                 Char_t * Separator     -- separator between bits
// Results:        Char_t * IndexString   -- resulting index string
// Exceptions:
// Description:    Builds an index string from index bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!fPatternMode) {
		if (fName.IsNull()) {
			cerr << setred << this->ClassName() << "::Pattern2String(): Index list not in pattern mode" << setblack << endl;
		} else {
			cerr << setred << this->ClassName() << "::Pattern2String(): Not in pattern mode" << setblack << endl;
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
	IndexString.Resize(0);
	TIterator * iter = this->MakeIterator();
	while (index = (TMrbNamedX *) iter->Next()) {
		UInt_t ind = index->GetIndex();
		if ((indexBits & ind) == ind) {
			if (IndexString.Length() > 0) IndexString += Separator;
			if (cmpl) IndexString += "~";
			IndexString += index->GetName();
		}
	}
	return(IndexString.Data());
}

UInt_t TMrbLofNamedX::CheckPattern(const Char_t * ClassName, const Char_t * Method,
							const Char_t * IndexString, const SMrbNamedX * NamedX, UInt_t Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::CheckPattern
// Purpose:        Check a named index
// Arguments:      Char_t * ClassName        -- class name of caller
//                 Char_t * Method           -- calling method
//                 Char_t * IndexString      -- index names given
//                 SMrbNamedX * NamedX       -- list of legal indices
//                 UInt_t Mode               -- mode how to search
// Results:        UInt_t Pattern            -- pattern with index bits
// Exceptions:
// Description:    Checks string and returns corresponding bit pattern.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;

	if (NamedX != NULL) this->AddNamedX(NamedX);
	this->SetPatternMode();
	opt = this->FindPattern(IndexString, Mode);
	if (opt == TMrbLofNamedX::kIllIndexBit) {
		cerr << setred << this->ClassName() << "::CheckPattern(): Illegal index string - " << IndexString << setblack << endl;
		if (this->GetEntries() > 0) {
			cerr << setred << this->ClassName() << "::CheckPattern(): Legal index names are - " << setblack << endl;
			this->Print(cerr, "       -> ");
		} else {
			cerr << setred << this->ClassName() << "::CheckPattern(): No legal index names present" << setblack << endl;
		}
	}
	return(opt);
}

UInt_t TMrbLofNamedX::GetMask() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::GetMask
// Purpose:        Return bit pattern
// Arguments:      --
// Results:        UInt_t Mask   -- bit pattern
// Exceptions:
// Description:    Returns a pattern of all bits in this list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!IsPatternMode()) return(0xffffffff);

	TMrbNamedX * xPnt;
	UInt_t mask = 0;
	TIterator * iter = this->MakeIterator();
	while (xPnt = (TMrbNamedX *) iter->Next()) mask |= xPnt->GetIndex();
	return(mask);
}