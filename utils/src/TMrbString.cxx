//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbString.cxx
// Purpose:        MARaBOU utilities: type conversion for strings
// Description:    Implements type conversion for TString objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
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
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fBase = TMrbString::kDefaultBase;
}

TMrbString::TMrbString(const Char_t * Str) : TString(Str) {
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fBase = TMrbString::kDefaultBase;
}

TMrbString & TMrbString::FromInteger(Int_t IntVal, Int_t Width, Char_t PadChar, Int_t Base, Bool_t BaseId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromInteger
// Purpose:        Convert integer to string
// Arguments:      Int_t IntVal   -- integer value
//                 Int_t Width    -- number of digits
//                 Char_t PadChar -- how to pad leading blanks
//                 Int_t Base     -- conversion base (2, 8, 10, 16)
//                 Bool_t BaseId  -- prepend base identifier (0b, 0, 0x) if kTRUE
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->Resize(0);
	if (Base == 0) Base = fBase;
	if (Base == 2) {
		while (IntVal) { this->Prepend((IntVal & 1) ? '1' : '0'); IntVal >>= 1; }
		if (BaseId) this->Prepend("0b");
	} else {
		ostringstream * s = new ostringstream();
		switch (Base) {
			case 8:
				if (BaseId) { *s << "0"; Width -= 1; PadChar = '0'; }
				if (Width > 0) *s << setw(Width);
				if (PadChar != '\0') *s << setfill(PadChar);
				*s << setbase(8) << IntVal << ends;
				break;

			case 16:
				if (BaseId) { *s << "0x"; Width -= 2; PadChar = '0'; }
				if (Width > 0) *s << setw(Width);
				if (PadChar != '\0') *s << setfill(PadChar);
				*s << setbase(16) << setiosflags(ios::uppercase) << IntVal << resetiosflags(ios::uppercase) << ends;
				break;

			case 10:
				if (Width > 0) *s << setw(Width);
				if (PadChar != '\0') *s << setfill(PadChar);
				*s << IntVal << ends;
				break;

			default:
				gMrbLog->Err() << "Not a legal integer base - " << Base << endl;
				gMrbLog->Flush("TMrbString", "FromInteger");
				return(*this);
		}
		this->Insert(0, s->str().c_str());
//		s->rdbuf()->freeze(0);
		delete s;
	}
	return(*this);
}

TMrbString & TMrbString::AppendInteger(Int_t IntVal, Int_t Width, Char_t PadChar, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendInteger
// Purpose:        Append integer to string
// Arguments:      Int_t IntVal   -- integer value
//                 Int_t Width    -- number of digits
//                 Char_t PadChar -- hwo to pad leading blanks
//                 Int_t Base     -- conversion base (2, 8, 10, 16)
// Results:        --
// Exceptions:
// Description:    Appends an integer value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Base == 0) Base = fBase;
	TMrbString s(IntVal, Width, '0', Base, kFALSE);
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

	Int_t i;
	Int_t idx;
	Int_t baseId;
	Bool_t isSigned;
	TString pf;

	IntVal = 0;

	if (Base == 0) Base = fBase;
	idx = this->CheckInteger(pf, Base, baseId, isSigned);
	pf = pf.Strip(TString::kBoth);
	if (idx == -1 || pf.Length() != 0) {
		gMrbLog->Err() << "No a legal integer (base " << Base << ") - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToInteger");
		return(kFALSE);
	}

	if (baseId == 2) {
		for (i = idx; i < this->Length(); i++) {
			IntVal <<= 1;
			if (fData[i] == '1') IntVal += 1;
		}
	} else {
		istringstream s(&fData[idx]);
		switch (baseId) {
			case 8: 	s >> setbase(8) >> IntVal; break;
			case 16:	s >> setbase(16) >> IntVal; break;
			case 10:	s >> IntVal; break;
			default:	gMrbLog->Err() << "Not a legal integer base " << Base << endl;
						gMrbLog->Flush("TMrbString", "ToInteger");
						IntVal = 0;
						return(kFALSE);
		}
	}	
	if (isSigned) IntVal = -IntVal;
	return(kTRUE);
}

Int_t TMrbString::SplitOffInteger(TString & Prefix, Bool_t & IsSigned, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffInteger
// Purpose:        Return trailing integer
// Arguments:      TString & Prefix    -- leading substring
//                 Bool_t & IsSigned   -- kTRUE if signed number
//                 Int_t Base          -- numerical base
// Results:        Int_t IntVal        -- trailing integer part (unsigned)
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pf;
	Int_t i;
	Int_t n;
	Int_t idx;
	Int_t baseId;

	if (Base == 0) Base = fBase;
	idx = this->CheckInteger(pf, Base, baseId, IsSigned);
	if (idx == -1) {
		gMrbLog->Err() << "No trailing integer (base " << Base << ") - " << fData << endl;
		gMrbLog->Flush("TMrbString", "SplitOffInteger");
		Prefix = fData;
		return(0);
	}
	n = 0;
	if (baseId == 2) {
		for (i = idx; i < this->Length(); i++) {
			n <<= 1;
			if (fData[i] == '1') n += 1;
		}
	} else {
		istringstream s(&fData[idx]);
		switch (baseId) {
			case 8: 	s >> setbase(8) >> n; break;
			case 16:	s >> setbase(16) >> n; break;
			case 10:	s >> n; break;
			default:	gMrbLog->Err() << "Not a legal integer base " << Base << endl;
						gMrbLog->Flush("TMrbString", "ToInteger");
						return(0);
		}
	}	

	Prefix.Replace(0, Prefix.Length(), pf.Data());
	return(n);
}

Int_t TMrbString::Increment(Int_t Increment, Int_t Base, Bool_t WithSign) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing integer
// Arguments:      Int_t Increment     -- increment
//                 Int_t Base          -- numerical base
//                 Bool_t WithSign     -- include sign if present
// Results:        Int_t IntVal        -- integer value
// Exceptions:
// Description:    Increments the trailing integer of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString prf;
	Int_t n, ns;
	Bool_t isSigned;

	if (Base == 0) Base = fBase;
	n = this->SplitOffInteger(prf, isSigned, Base);
	if (prf.Length() > 0) ns = this->Length() - prf.Length(); else ns = 0;
	if (isSigned) {
		if (WithSign) {
			if (n == 0) {
				n = Increment;
				if (n >= 0) ns--;
			} else {
				n = -n + Increment;
			}
		} else {
			n += Increment;
			if (n < 0) n = 0;
			prf += '-';
			ns--;
		}
	} else {
		n += Increment;
	}
	this->Replace(0, this->Length(), prf);
	this->AppendInteger(n, ns, Base);
	return(n);
}

TMrbString & TMrbString::FromDouble(Double_t DblVal, Int_t Width, Char_t PadChar, Int_t Precision) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromDouble
// Purpose:        Convert double to string
// Arguments:      Int_t DblVal     -- double value
//                 Int_t Width      -- number of digits
//                 Char_t PadChar   -- how to pad blanks
//                 Int_t Precision  -- precision
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t precision = (Precision > 0) ? Precision : TMrbString::kDefaultPrecision;

	ostringstream * s = new ostringstream();
	if (Width > 0) *s << setw(Width) << setfill(PadChar);
	*s << setprecision(precision) << DblVal << ends;
	this->Replace(0, this->Length(), s->str().c_str());
//	s->rdbuf()->freeze(0);
	delete s;
    return(*this);
}

TMrbString & TMrbString::AppendDouble(Double_t DblVal, Int_t Width, Char_t PadChar, Int_t Precision) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendDouble
// Purpose:        Append double to string
// Arguments:      Double_t DblVal    -- integer value
//                 Int_t Width        -- number of digits
//                 Char_t PadChar     -- how to pad blanks
//                 Int_t Precision    -- precision
// Results:        --
// Exceptions:
// Description:    Appends a double value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t precision = (Precision > 0) ? Precision : TMrbString::kDefaultPrecision;

	TMrbString s(DblVal, Width, PadChar, precision);
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

	Int_t idx;
	Bool_t isSigned;
	TString pf;

	idx = this->CheckDouble(pf, isSigned);
	pf = pf.Strip(TString::kBoth);

	DblVal = 0.;

	if (idx == -1 || pf.Length() != 0) {
		gMrbLog->Err() << "Not a legal double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		return(kFALSE);
	}

	istringstream s(&fData[idx]);
	s >> DblVal;
	if (isSigned) DblVal = -DblVal;
	return(kTRUE);
}

Double_t TMrbString::SplitOffDouble(TString & Prefix, Bool_t & IsSigned) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffDouble
// Purpose:        Return trailing double
// Arguments:      TString & Prefix    -- leading substring
//                 Bool_t & IsSigned   -- ktrue if signed number
// Results:        Double_t DblVal     -- trailing double part (unsigned)
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pf;
	Int_t idx;
	Double_t d;

	idx = this->CheckDouble(pf, IsSigned);
	if (idx == -1) {
		gMrbLog->Err() << "No trailing double - " << fData << endl;
		gMrbLog->Flush("TMrbString", "SplitOffDouble");
		Prefix = fData;
		return(0.);
	}

	istringstream s(&fData[idx]);
	s >> d;
	Prefix.Replace(0, Prefix.Length(), pf.Data());
	return(d);
}

Double_t TMrbString::Increment(Double_t Increment, Bool_t WithSign) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing double
// Arguments:      Double_t Increment     -- increment
//                 Bool_t WithSign        -- allow number to be signed
// Results:        Double_t IntVal        -- double value
// Exceptions:
// Description:    Increments the trailing double of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString prf;
	Int_t ns;
	Double_t d;
	Bool_t isSigned;

	d = this->SplitOffDouble(prf, isSigned);
	if (prf.Length() > 0) ns = this->Length() - prf.Length(); else ns = 0;
	if (isSigned) {
		if (WithSign) {
			if (d == 0.) {
				d = Increment;
				if (d >= 0.) ns--;
			} else {
				d = -d + Increment;
			}
		} else {
			d += Increment;
			if (d < 0.) d = 0.;
			prf += '-';
			ns--;
		}
	} else {
		d += Increment;
	}
	this->Replace(0, this->Length(), prf);
	this->AppendDouble(d, ns);
	return(d);
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
	Char_t whiteSpace;
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

Int_t TMrbString::CheckInteger(TString & Prefix, Int_t Base, Int_t & BaseId, Bool_t & IsSigned) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckInteger
// Purpose:        Check if string is valid integer
// Arguments:      TString & Prefix       -- where to store prefix
//                 Int_t Base             -- expected base
//                 Int_t & BaseId         -- actual base id (0x, 0b)
//                 Bool_t & IsSigned      -- kTRUE if signed number
// Results:        Int_t Index            -- where integer number starts
// Exceptions:
// Description:    Test if string chars are legal integer digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idxBin, idxOct, idxDec, idxHex;
	Bool_t isBin, isOct, isDec, isHex;
	Int_t i;
	Int_t n = 0;
	Int_t idx = 0;
	Int_t code;

	IsSigned = kFALSE;
	BaseId = Base;

	if (this->Length() == 0) return(0);

	idxBin = idxOct = idxDec = idxHex = -1;
	isBin = isOct = isDec = isHex = kTRUE;

	for (i = this->Length(); i > 0; i--) {
		n = i - 1;
		code = fData[n];
		if (code >= '0' && code <= '9') {
			if (code <= '7' && isOct) idxOct = n; else isOct = kFALSE;
			if ((code == '0' || code == '1') && isBin) idxBin = n; else isBin = kFALSE;
			if (isDec) idxDec = n;
			if (isHex) idxHex = n;
		} else if ((code >= 'a' && code <= 'f') || (code >= 'A' && code <= 'F')) {
			isBin = isOct = isDec = kFALSE;
			if (isHex) idxHex = n;
		} else if (code == '-') {
			IsSigned = kTRUE;
			break;
		} else if ((code == 'b' || code == 'B') && fData[n - 1] == '0' && idxBin == n + 1) {
			n--;
			if (fData[n - 1] == '-') { n--; IsSigned = kTRUE; }
			Prefix.Replace(0, Prefix.Length(), fData, n);
			BaseId = 2;
			return(idxBin);
		} else if ((code == 'x' || code == 'X') && fData[n - 1] == '0' && idxHex == n + 1) {
			n--;
			if (fData[n - 1] == '-') { n--; IsSigned = kTRUE; }
			Prefix.Replace(0, Prefix.Length(), fData, n);
			BaseId = 16;
			return(idxHex);
		} else break;
	}

	if (fData[n] == '0') {
		if (isBin) BaseId = 2;
		else if (isOct) BaseId = 8;
	}

	switch (Base) {
		case 2: 	idx = idxBin; break;
		case 8: 	idx = idxOct; break;
		case 10:	idx = idxDec; break;
		case 16: 	idx = idxHex; break;
	}

	if (idx == -1) {
		Prefix = fData;
		return(-1);
	} else {
		n = idx;
		if (fData[idx - 1] == '-') n--;
		Prefix.Replace(0, Prefix.Length(), fData, n);
		return(idx);
	}
}

Int_t TMrbString::CheckDouble(TString & Prefix, Bool_t & IsSigned) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckDouble
// Purpose:        Check if string is valid integer
// Arguments:      TString & Prefix       -- where to store prefix
// Results:        Int_t Index            -- where double number starts
// Exceptions:
// Description:    Test if string chars are legal double digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t dot;
	Int_t i, n;
	Int_t code;
	Int_t idxDbl;
	Int_t lng;

	if ((lng = this->Length()) == 0) return(0);

	if ((n = this->Index("e-", 0)) > 0) lng = n;
	else if ((n = this->Index("e+", 0)) > 0) lng = n;

	dot = kFALSE;
	IsSigned = kFALSE;
	idxDbl = -1;

	for (i = lng; i > 0; i--) {
		n = i - 1;
		code = fData[n];
		if (code >= '0' && code <= '9')	{
			idxDbl = n;
		} else if (code == '.') {
			if (dot) break;
			dot = kTRUE;
			idxDbl = n;
		} else if (code == '-') {
			IsSigned = kTRUE;
			break;
		} else break;
	}

	if (idxDbl == -1) {
		Prefix = fData;
		return(-1);
	} else {
		n = idxDbl;
		if (fData[n - 1] == '-') n--;
		Prefix.Replace(0, Prefix.Length(), fData, n);
	}
	return(idxDbl);
}
