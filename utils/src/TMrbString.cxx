//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbString.cxx
// Purpose:        MARaBOU utilities: type conversion for strings
// Description:    Implements type conversion for TString objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbString.cxx,v 1.14 2006-08-10 07:56:30 Rudolf.Lutter Exp $       
// Date:           $Date: 2006-08-10 07:56:30 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TObjString.h"
#include "TObjArray.h"

#include "TMrbString.h"
#include "TMrbLogger.h"
#include "SetColor.h"

ClassImp(TMrbString)

extern TMrbLogger * gMrbLog;			// access to message logger

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString
// Purpose:        Provide type conversion
// Description:    Type conversion for strings
//                 Based on original TString class.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TMrbString::TMrbString() {
	fBase = TMrbString::kDefaultBase;
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

TMrbString::TMrbString(const Char_t * Str) : TString(Str) {
	fBase = TMrbString::kDefaultBase;
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

TMrbString & TMrbString::FromInteger(Int_t IntVal,
							Int_t Width, Int_t Base, Bool_t PadZero, Bool_t AddBasePrefix, Bool_t LowerCaseHex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromInteger
// Purpose:        Convert integer to string
// Arguments:      Int_t IntVal          -- integer value
//                 Int_t Width           -- number of digits
//                 Int_t Base            -- conversion base (2, 8, 10, 16)
//                 Bool_t PadZero        -- pad leading blanks with zero
//                 Bool_t AddBasePrefix  -- prepend base identifier (0b, 0, 0x) if kTRUE
//                 Bool_t LowerCaseHex   -- use lc chars for hex representation
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t hex, prf;
	Char_t f[100], s[100];

	prf = AddBasePrefix ? '#' : ' ';
	hex = LowerCaseHex ? 'x' : 'X';

	this->Resize(0);

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "FromInteger")) return(*this);

	if (Base == 2) {
		while (IntVal) { this->Prepend((IntVal & 1) ? '1' : '0'); IntVal >>= 1; }
		if (AddBasePrefix) this->Prepend("0b");
	} else {
		switch (Base) {
			case 8:
				if (PadZero) {
					if (Width > 0)	sprintf(f, "%%%c0%do", prf, Width);
					else			sprintf(f, "%%%c0o", prf);
				} else {
					if (Width > 0)	sprintf(f, "%%%c%do", prf, Width);
					else			sprintf(f, "%%%co", prf);
				}
				break;

			case 16:
				if (PadZero) {
					if (Width > 0)	sprintf(f, "%%%c0%d%c", prf, Width, hex);
					else			sprintf(f, "%%%c0%c", prf, hex);
				} else {
					if (Width > 0)	sprintf(f, "%%%c%d%c", prf, Width, hex);
					else			sprintf(f, "%%%c%c", prf, hex);
				}
				break;

			case 10:
				if (PadZero) {
					if (Width > 0)	sprintf(f, "%%0%dd", Width);
					else			sprintf(f, "%%0d");
				} else {
					if (Width > 0)	sprintf(f, "%%%dd", Width);
					else			sprintf(f, "%%d");
				}
				break;
		}
		sprintf(s, f, IntVal); 
		this->Insert(0, s);
	}
	return(*this);
}

TMrbString & TMrbString::AppendInteger(Int_t IntVal,
								Int_t Width, Int_t Base, Bool_t PadZero, Bool_t AddBasePrefix, Bool_t LowerCaseHex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendInteger
// Purpose:        Append integer to string
// Arguments:      Int_t IntVal          -- integer value
//                 Int_t Width           -- number of digits
//                 Int_t Base            -- conversion base (2, 8, 10, 16)
//                 Bool_t PadZero        -- pad leading blanks with zero
//                 Bool_t AddBasePrefix  -- add base prefix if kTRUEs
//                 Bool_t LowerCaseHex   -- use lc chars for hex representation
// Results:        --
// Exceptions:
// Description:    Appends an integer value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "AppendInteger")) return(*this);

	TMrbString s(IntVal, Width, Base, PadZero, AddBasePrefix, LowerCaseHex);
	this->Append(s.Data());
	return(*this);
}

Bool_t TMrbString::ToInteger(Int_t & IntVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ToInteger
// Purpose:        Convert string to integer
// Arguments:      Int_t & IntVal   -- integer representation
//                 Int_t Base       -- force conversion to given base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Converts a string to its integer value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	IntVal = 0;

	if (!this->CheckBase(Base, "ToInteger")) return(kFALSE);

	Int_t idx = this->Index("0b", 0);
	if (idx != -1) {
		Base = 2;
		idx += 2;
	} else {
		idx = 0;
	}
	
	IntVal = (Int_t) strtoul(&fData[idx], &endptr, Base);

	if (*endptr != '\0') {
		gMrbLog->Err() << "Not a legal integer - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToInteger");
		IntVal = 0;
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbString::SplitOffInteger(TString & Prefix, Int_t & IntVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffInteger
// Purpose:        Return trailing integer
// Arguments:      TString & Prefix    -- leading substring
//                 Int_t IntVal        -- trailing integer part
//                 Int_t Base          -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "SplitOffInteger")) return(kFALSE);

	Int_t idx = this->CheckInteger(Base);
	if (idx == -1) {
		gMrbLog->Err() << "No trailing integer (base " << Base << ") - " << fData << endl;
		gMrbLog->Flush("TMrbString", "SplitOffInteger");
		Prefix = fData;
		return(kFALSE);
	}

	TMrbString s(&fData[idx]);
	if (!s.ToInteger(intVal, Base)) {
		Prefix = fData;
		return(kFALSE);
	}

	Prefix = (*this)(0, idx);
	IntVal = intVal;
	return(kTRUE);
}

Bool_t TMrbString::Increment(Int_t Increment, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing integer
// Arguments:      Int_t Increment     -- increment
//                 Int_t Base          -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Increments the trailing integer of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;
	Bool_t basePrefix = kFALSE;

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "Increment")) return(-1);

	if (!this->ToInteger(intVal, Base)) return(kFALSE);
	intVal += Increment;
	switch (Base) {
		case 2: basePrefix = (this->Index("0b", 0) == 0); break;
		case 8: basePrefix = (this->Index("0", 0) == 0); break;
		case 10: basePrefix = kFALSE; break;
		case 16: basePrefix = (this->Index("0x", 0) == 0); break;
	}
	return(this->FromInteger(intVal, 0, ' ', Base, basePrefix));
}

TMrbString & TMrbString::FromDouble(Double_t DblVal, Int_t Width, Int_t Precision, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromDouble
// Purpose:        Convert double to string
// Arguments:      Int_t DblVal     -- double value
//                 Int_t Width      -- number of digits
//                 Int_t Precision  -- precision
//                 Bool_t PadZero   -- pad leading blanks with zero
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t f[100], s[100];

	if (PadZero) {
		if (Width > 0)	sprintf(f, "%%0%d.%df", Width, Precision);
		else			sprintf(f, "%%0%df", Precision);
	} else {
		if (Width > 0)	sprintf(f, "%%%d.%df", Width, Precision);
		else			sprintf(f, "%%%df", Precision);
	}
	sprintf(s, f, DblVal); 
	this->Insert(0, s);
    return(*this);
}

TMrbString & TMrbString::AppendDouble(Double_t DblVal, Int_t Width, Int_t Precision, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendDouble
// Purpose:        Append double to string
// Arguments:      Double_t DblVal    -- integer value
//                 Int_t Width        -- number of digits
//                 Int_t Precision    -- precision
//                 Bool_t PadZero   -- pad leading blanks with zero
// Results:        --
// Exceptions:
// Description:    Appends a double value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString s(DblVal, Width, Precision, PadZero);
	this->Append(s.Data());
	return(*this);
}

Bool_t TMrbString::ToDouble(Double_t & DblVal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ToDouble
// Purpose:        Convert string to double
// Arguments:      Double_t & DblVal   -- double representation
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Converts a string to its double value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	DblVal = 0;

	Int_t idx = this->CheckDouble();
	if (idx == -1) {
		gMrbLog->Err() << "Not a legal double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		return(kFALSE);
	}

	TString pf = (*this)(0, idx);
	pf = pf.Strip(TString::kBoth);

	if (idx == -1 || pf.Length() != 0) {
		gMrbLog->Err() << "Not a legal double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		DblVal = 0.;
		return(kFALSE);
	}

	DblVal = strtod(&fData[idx], &endptr);

	if (*endptr != '\0') {
		gMrbLog->Err() << "Not a legal double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		DblVal = 0.;
		return(kFALSE);
	}
 	return(kTRUE);
}

Bool_t TMrbString::SplitOffDouble(TString & Prefix, Double_t DblVal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffDouble
// Purpose:        Return trailing double
// Arguments:      TString & Prefix    -- leading substring
//                 Double_t DblVal     -- trailing double part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	Double_t dblVal;

	idx = this->CheckDouble();
	if (idx == -1) {
		gMrbLog->Err() << "No trailing double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "SplitOffDouble");
		Prefix = fData;
		return(kFALSE);
	}

	TMrbString s = &fData[idx];
	if (!s.ToDouble(dblVal)) {
		Prefix = fData;
		return(kFALSE);
	}

	Prefix = (*this)(0, idx);
	DblVal = dblVal;
	return(kTRUE);
}

Bool_t TMrbString::Increment(Double_t Increment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing double
// Arguments:      Double_t Increment     -- increment
// Results:        Double_t IntVal        -- double value
// Exceptions:
// Description:    Increments the trailing double of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pf;
	Double_t dblVal;

	if (!this->ToDouble(dblVal)) return(kFALSE);
	dblVal += Increment;
	return(this->FromDouble(dblVal));
}

Int_t TMrbString::Split(TObjArray & LofSubStrings, const Char_t * Separator, Bool_t RemoveWhiteSpace) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Split
// Purpose:        Split string into substrings
// Arguments:      TObjArray & LofSubStrings   -- list of substrings
//                 Char_t * Separator          -- separator between substrings
// Results:        Int_t NofSubStrings         -- number of substrings
// Exceptions:
// Description:    Appends substrings to list.
//                 Substrings have to be converted to TObjString to get stored.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString sepStr;
	Int_t nstr, sepl, lng;
	TString subStr;
	Char_t whiteSpace = ' ';
	TMrbString tmpStr;
	TMrbString * sp;

	sepStr = Separator;
	sepl = sepStr.Length();

	if (RemoveWhiteSpace) {
		tmpStr = this->Copy();
		whiteSpace = (sepStr.CompareTo("\t") == 0) ? '\t' : ' ';
		tmpStr.ReplaceWhiteSpace(whiteSpace);
		sp = &tmpStr;
	} else {
		sp = this;
	}

	n1 = 0;
	nstr = 0;
	for (;;) {
		n2 = sp->Index(sepStr.Data(), n1);
		lng = (n2 == -1) ? sp->Length() - n1 : n2 - n1;
		subStr = (*sp)(n1, lng);
		if (subStr.Length() > 0) {
			if (RemoveWhiteSpace) subStr = subStr.Strip(TString::kBoth, whiteSpace);
			LofSubStrings.Add(new TObjString(subStr.Data()));
			nstr++;
		} else {
			LofSubStrings.Add(new TObjString(" "));
			nstr++;
		}
		if (n2 == -1) break;
		n1 = n2 + sepl;
	}
	return(nstr);
}

Int_t TMrbString::ReplaceWhiteSpace(const Char_t WhiteSpace) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ReplaceWhiteSpace
// Purpose:        Replace white spaces
// Arguments:      Char_t WhiteSpace    -- white space to be used as replacement
// Results:        Int_t NofChars       -- resulting length
// Exceptions:
// Description:    Replaces multiple white spaces (blanks or tabs) with given character
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t out;
	Bool_t wasWhiteSpace;
	Char_t c;

	out = 0;
	wasWhiteSpace = kFALSE;
	for (Int_t i = 0; i < this->Length(); i++) {
		c = (*this)(i);
		if (c == ' ' || c == '\t') {
			if (!wasWhiteSpace) {
				wasWhiteSpace = kTRUE;
				(*this)(out++) = WhiteSpace;
			}
		} else {
			wasWhiteSpace = kFALSE;
			(*this)(out++) = c;
		}
	}
	this->Resize(out);
	return(out);
}


void TMrbString::Expand(const Char_t Escape) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Expand
// Purpose:        Expand escape sequences
// Arguments:      Char_t Escape    -- escape char
// Results:        --
// Exceptions:
// Description:    Looks for given escape char
//                 and calls method "ProcessEscapeSequence".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString esc;
	TString replStr;

	esc = Escape;

	n1 = 0;
	for (;;) {
		n2 = this->Index(esc, n1);
		if (n2 == -1) break;
		if (n2 == this->Length() - 1) {
			this->Replace(n2, 1, "");
			break;
		}
		this->ProcessEscapeSequence(fData[n2 + 1], replStr);
		this->Replace(n2, 2, replStr);
		n1 = n2 + 1;
	}
}

void TMrbString::ProcessEscapeSequence(Char_t CharId, TString & Replacement) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ProcessEscapeSequence
// Purpose:        Expand escape sequences
// Arguments:      Char_t CharId          -- escape sequence char
//                 TString & Replacement  -- replacement
// Results:        --
// Exceptions:
// Description:    User's method to replace escape sequences.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Replacement.Resize(0);		// nullify
}

Int_t TMrbString::CheckInteger(Int_t Base) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckInteger
// Purpose:        Check if string is valid integer
// Arguments:      Int_t Base             -- expected base
// Results:        Int_t Index            -- where integer number starts
// Exceptions:     returns -1 on error
// Description:    Test if string chars are legal integer digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = -1;

	if (this->Length() > 0) {
		switch (Base) {
			case 2:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = fData[i];
					if (code == '0' && code == '1') idx = i; else break;
				}
				break;
			case 8:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = fData[i];
					if (code == '-') { idx = i; break; }
					else if (code >= '0' && code <= '7') idx = i;
					else break;
				}
				break;
			case 10:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = fData[i];
					if (code == '-') { idx = i; break; }
					else if (code >= '0' && code <= '9') idx = i;
					else break;
				}
				break;
			case 16:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = fData[i];
					if (code == '-') { idx = i; break; }
					else if (	(code >= '0' && code <= '9')
							||	(code >= 'a' && code <= 'f')
							||	(code >= 'A' && code <= 'F')) idx = i;
					else break;
				}
				break;
			default:
				gMrbLog->Err() << "Illegal base spec - " << Base << endl;
				gMrbLog->Flush("TMrbString", "CheckInteger");
				break;
		}
	}
	return(idx);
}

Int_t TMrbString::CheckDouble() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckDouble
// Purpose:        Check if string is valid integer
// Arguments:      --
// Results:        Int_t Index            -- where double number starts
// Exceptions:     returns -1 on error
// Description:    Test if string chars are legal double digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = -1;

	Int_t lng = this->Length();

	if (lng > 0) {
		Int_t n;
		if ((n = this->Index("e-", 0)) > 0) lng = n;
		else if ((n = this->Index("e+", 0)) > 0) lng = n;

		Bool_t dot = kFALSE;
		for (Int_t i = lng - 1; i >= 0; i--) {
			Char_t code = fData[i];
			if (code == '-') { idx = i; break; }
			if (code >= '0' && code <= '9')	{
				idx = i;
			} else if (code == '.') {
				if (dot) break;
				dot = kTRUE;
				idx = i;
			} else break;
		}
	}
	return(idx);
}

Bool_t TMrbString::CheckBase(Int_t Base, Char_t * Method) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckBase
// Purpose:        Check if numerical base is valid
// Arguments:      Int_t Base             -- base value ([0], 2, 8, 10, 16)
//                 Char_t * Method        -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Test if numerical base value is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Base) {
		case 0:
		case 2:
		case 8:
		case 10:
		case 16:	return(kTRUE);
	}
	gMrbLog->Err() << "Illegal base spec - " << Base << endl;
	gMrbLog->Flush("TMrbString", Method);
	return(kFALSE);
}


