//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbLofNamedX.cxx
// Purpose:        MARaBOU utilities: A list of TMrbNamedX objects
// Description:    Implements class methods to manage a list of TMrbNamedX objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "TObjString.h"
#include "SetColor.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"

ClassImp(TMrbLofNamedX)

extern TMrbLogger * gMrbLog;			// access to message logging

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
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

	namedX = (TMrbNamedX *) LofNamedX->First();
	while (namedX) {
		this->Add(	new TMrbNamedX( namedX->GetIndex(),
									namedX->GetName(),
									namedX->HasTitle() ? namedX->GetTitle() : NULL,
									namedX->GetAssignedObject()));
		namedX = (TMrbNamedX *) LofNamedX->After(namedX);
	}
}

void TMrbLofNamedX::AddNamedX(const Char_t * NameString, const Char_t * Separator, Bool_t PatternMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::AddNamedX
// Purpose:        Append indices from a name string
// Arguments:      Char_t * NameString    -- list of names separated bei a delimiter
//                 Char_t * Separator     -- separator used in name string
//                 Bool_t PatternMode     -- turn pattern mode on/off
// Results:        --
// Exceptions:
// Description:    Adds a list of indices taken from a charater string.
//                 Format is
//                          name1[=idx1]:...:nameN[=idxN]
//                 ":" is the default delimiter, but may be defined by arg "Separator"
//                 If "=idx" is omitted the index will be incremented (shifted if
//                 pattern mode on) starting with the index of the last list member.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx, n;
	TMrbString nameString;
	TObjArray lofSubStr;
	TMrbString subStr;
	Int_t nstr;
	TMrbNamedX * np;
	TString prefix;

	np = (TMrbNamedX *) this->Last();
	if (np == NULL) 		idx = 1;
	else if (PatternMode)	idx = np->GetIndex() << 1;
	else					idx = np->GetIndex() + 1;

	nameString = NameString;
	nstr = nameString.Split(lofSubStr, Separator);
	for (Int_t i = 0; i < nstr; i++) {
		subStr = ((TObjString *) lofSubStr[i])->GetString().Data();
		n = subStr.Index("=", 0);
		if (n != -1) {
			subStr.SplitOffInteger(prefix, idx);
			subStr = (TString) subStr(0, n);
		}
		this->AddNamedX(new TMrbNamedX(idx, subStr.Data()));
		if (PatternMode) idx <<= 1; else idx++;
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
	TMrbNamedX * xPnt;
	TMrbNamedX * same;
	Int_t index, n;
	Bool_t negFlag;
	Bool_t firstBit;
	TMrbNamedX * k;
	TMrbString fullName;
	Int_t base;
	
	if (this->GetLast() == -1) {
		gMrbLog->Err() << "No indices defined" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	base = this->IsPatternMode() ? 16 : 10;

	xPnt = (TMrbNamedX *) First();
	if (fPatternMode) {
		while (xPnt) {
			if ((xPnt->GetIndex() & Mask) == (UInt_t) xPnt->GetIndex()) {
				Out << Prefix << xPnt->GetFullName(fullName);
				index = xPnt->GetIndex();
				if (index & TMrbLofNamedX::kPatternBit) {
					index = ~index;
					negFlag = kTRUE;
					Out << " (~0x" << setbase(16) << index << ")";
				} else {
					negFlag = kFALSE;
					Out << " (0x" << setbase(16) << index << ")";
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
			xPnt = (TMrbNamedX *) After(xPnt);
		}
	} else {
		while (xPnt) {
			Out << Prefix;
			xPnt->Print(Out, base, kFALSE);
			same = this->FindByIndex(xPnt->GetIndex());
			if (same != xPnt) Out << " (same as " << same->GetName() << ")";
			Out << endl;
			xPnt = (TMrbNamedX *) After(xPnt);
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

	if (fPatternMode) {
		xPnt = (TMrbNamedX *) First();
		Out << Prefix;
		while (xPnt) {
			if (xPnt->GetIndex() & TMrbLofNamedX::kPatternBit) {
				if ((~xPnt->GetIndex() & Mask) == 0) Out << xPnt->GetName() << " ";
			} else {
				if ((xPnt->GetIndex() & Mask) == (UInt_t) xPnt->GetIndex()) Out << xPnt->GetName() << " ";
			}
			xPnt = (TMrbNamedX *) After(xPnt);
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

	TMrbNamedX * xPnt;
	TMrbNamedX * namedX;
	TString shortName;
	TString xName;
	Int_t lng;
	Bool_t dialogFlag;

	TString::ECaseCompare cmp;

	cmp = (FindMode & TMrbLofNamedX::kFindIgnoreCase) ? TString::kIgnoreCase : TString::kExact;

	dialogFlag = FindByDialog(shortName, ShortName, FindMode);

	namedX = NULL;

	for(;;) {
		xPnt = (TMrbNamedX *) First();

		if (FindMode & TMrbLofNamedX::kFindUnique) {
			lng = shortName.Length();
			while (xPnt) {
				xName = xPnt->GetName();
				xName.Resize(lng);
				if (xName.CompareTo(shortName, cmp) == 0) {
					if (namedX != NULL) {
						namedX = NULL;
						break;
					}
					namedX = xPnt;
				}
				xPnt = (TMrbNamedX *) After(xPnt);
			}
		} else {
			while (xPnt) {
				xName = xPnt->GetName();
				if (xName.CompareTo(shortName, cmp) == 0) {
					return(xPnt);
				}
				xPnt = (TMrbNamedX *) After(xPnt);
			}
		}
		if (dialogFlag && namedX == NULL) {
			gMrbLog->Err() << "Illegal index - " << shortName << endl;
			gMrbLog->Flush(this->ClassName(), "FindByName");
			FindByDialog(shortName, ShortName, FindMode);
		} else {
			return(namedX);
		}
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

	xPnt = (TMrbNamedX *) First();
	while (xPnt) {
		if ((xPnt->GetIndex() & Mask) == Index) return(xPnt);
		xPnt = (TMrbNamedX *) After(xPnt);
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
	Bool_t dialogFlag;
	Int_t index;

	if (!fPatternMode) {
		if (fName.IsNull()) {
			gMrbLog->Err() << "Index list not in pattern mode" << endl;
		} else {
			gMrbLog->Err() << fName << " not in pattern mode" << endl;
		}
		gMrbLog->Flush(this->ClassName(), "FindPattern");
		return(TMrbLofNamedX::kIllIndexBit);
	}

	dialogFlag = FindByDialog(xString, IndexString, FindMode, Separator);

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
		if (dialogFlag && pattern == TMrbLofNamedX::kIllIndexBit) {
			gMrbLog->Err() << "Illegal index string - " << xString << endl;
			gMrbLog->Flush(this->ClassName(), "FindPattern");
			FindByDialog(xString, IndexString, FindMode, Separator);
		} else {
			return(pattern);
		}
	}
}

Bool_t TMrbLofNamedX::FindByDialog(TString & Result,
						const TString & ShortName, const UInt_t FindMode, const Char_t * Separator) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX::FindMultipleKey
// Purpose:        Find a compound index
// Arguments:      TString & ShortName   -- index name
//                 TString & Result      -- user's input
//                 UInt_t FindMode       -- method how to find an index
//                 Char_t * Separator    -- separating string
// Results:        Bool_t IsDialog       -- flag indicating dialog mode
// Exceptions:
// Description:    Checks index name for "?" and enters dialog mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString shortName;
	TString sepStr;

	shortName = ShortName;
	shortName = shortName.Strip(TString::kBoth);
	if (shortName.IsNull()) return(kFALSE);

	if (shortName[0] != '?') {
		Result = shortName;
		return(kFALSE); 		// not in dialog mode
	}

	cout << endl;
	if (!shortName(1, shortName.Length() - 1).IsNull()) {
		cout	<< shortName(1, shortName.Length() - 1) << ":" << endl;
	} else if (!fName.IsNull()) {
		cout	<< fName << ":" << endl;
	} else {
		cout 	<< "List of named indices:" << endl;
	}

	sepStr = Separator;

	this->Print(cout, "     > ");
	if (sepStr.Length() != 0)  cout << "Index patterns should be separated by \"" << sepStr << "\"" << endl;

	cout << "Indices are ";
	if (FindMode & TMrbLofNamedX::kFindIgnoreCase) cout << "NOT ";
	cout << "case sensitive ";
	
	cout << "and may ";
	if (!(FindMode & TMrbLofNamedX::kFindUnique)) cout << "NOT ";
	cout << "be abbreviated" << endl;

	if (sepStr.Length() == 0)	cout << "Enter index name >> ";
	else						cout << "Enter index name(s) >> ";
	cout << flush;

	Result.ReadLine(cin);
	return(kTRUE);
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

	Bool_t cmpl;
	UInt_t indexBits;
	TMrbNamedX * index;

	if (!fPatternMode) {
		if (fName.IsNull()) {
			gMrbLog->Err() << "Index list not in pattern mode" << endl;
		} else {
			gMrbLog->Err() << fName << " not in pattern mode" << endl;
		}
		gMrbLog->Flush(this->ClassName(), "Pattern2String");
		return("");
	}

	if (Pattern & (UInt_t) TMrbLofNamedX::kPatternBit) {
		indexBits = ~Pattern;
		cmpl = kTRUE;
	} else {
		indexBits = Pattern;
		cmpl = kFALSE;
	}

	index = (TMrbNamedX *) this->First();
	IndexString.Resize(0);
	while (index) {
		UInt_t ind = index->GetIndex();
		if ((indexBits & ind) == ind) {
			if (IndexString.Length() > 0) IndexString += Separator;
			if (cmpl) IndexString += "~";
			IndexString += index->GetName();
		}
		index = (TMrbNamedX *) this->After(index);
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
		gMrbLog->Err() << "Illegal index string - " << IndexString << endl;
		gMrbLog->Flush(ClassName, Method);
		if (this->GetLast() != -1) {
			cerr	<< setred
					<< this->ClassName() << "::CheckPattern(): Legal index names are - "
					<< endl;
			this->Print(cerr, "       -> ");
			cerr	<< setblack << endl;
		} else {
			cerr	<< setred
					<< this->ClassName() << "::CheckPattern(): No legal index names present"
					<< setblack << endl;
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

	TMrbNamedX * xPnt;
	UInt_t mask;

	if (!IsPatternMode()) return(0xffffffff);

	xPnt = (TMrbNamedX *) First();
	mask = 0;
	while (xPnt) {
		mask |= xPnt->GetIndex();
		xPnt = (TMrbNamedX *) After(xPnt);
	}
	return(mask);
}
