#ifndef __TMrbString_h__
#define __TMrbString_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbString.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbString     -- a TString providing number conversion
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TString.h"
#include "TSystem.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString
// Purpose:        Provide type conversion
// Description:    Type conversion for strings
//                 Based on original TString class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbString : public TString {

	public:
		enum		{	kDefaultBase		=	10	};
		enum		{	kDefaultPrecision	=	4	};

	public:
		TMrbString();									// default ctor
		TMrbString(const Char_t * Str);					// ctor: same as TString

		TMrbString(Int_t IntVal,							// a string consisting of an integer
					Int_t Width = 0,
					Char_t PadChar = ' ',
					Int_t Base = TMrbString::kDefaultBase,
					Bool_t AddBasePrefix = kTRUE,
					Bool_t LowerCaseHex = kTRUE) {
						fBase = Base;
						this->FromInteger(IntVal, Width, PadChar, Base, AddBasePrefix, LowerCaseHex);
					};

		TMrbString(Double_t DblVal, 						// a string consisting of a double
					Int_t Width = 0,
					Char_t PadChar = ' ',
					Int_t Precision = TMrbString::kDefaultPrecision) {
						fBase = TMrbString::kDefaultBase;
						this->FromDouble(DblVal, Width, PadChar, Precision);
					};

		TMrbString(const Char_t * Prefix, 						// string + trailing integer
					Int_t IntVal,
					Int_t Width = 0,
					Char_t PadChar = '0',
					Int_t Base = TMrbString::kDefaultBase,
					Bool_t AddBasePrefix = kTRUE,
					Bool_t LowerCaseHex = kTRUE) : TString(Prefix) {
						fBase = Base;
						this->AppendInteger(IntVal, Width, PadChar, Base, AddBasePrefix, LowerCaseHex);
					};

		TMrbString(const Char_t * Prefix, 						// string + trailing double
					Double_t DblVal,
					Int_t Width = 0,
					Char_t PadChar = '0',
					Int_t Precision = TMrbString::kDefaultPrecision) : TString(Prefix) {
						fBase = TMrbString::kDefaultBase;
						this->AppendDouble(DblVal, Width, PadChar, Precision);
					};

		~TMrbString() {};									// default dtor

		TMrbString & FromInteger(Int_t IntVal,				// convert integer to string
									Int_t Width = 0,
									Char_t PadChar = ' ',
									Int_t Base = 0,
									Bool_t AddBasePrefix = kTRUE,
									Bool_t LowerCaseHex = kTRUE);

		TMrbString & AppendInteger(Int_t IntVal,			// append integer to string
									Int_t Width = 0,
									Char_t PadChar = ' ',
									Int_t Base = 0,
									Bool_t AddBasePrefix = kTRUE,
									Bool_t LowerCaseHex = kTRUE);

		Bool_t ToInteger(Int_t & IntVal, Int_t Base = 0);	// convert string to integer

		TMrbString & FromDouble(Double_t DblVal, 			// convert double to string
									Int_t Width = 0,
									Char_t PadChar = ' ',
									Int_t Precision = TMrbString::kDefaultPrecision);

		TMrbString & AppendDouble(Double_t DblVal,			// append double to string
									Int_t Width = 0,
									Char_t PadChar = '0',
									Int_t Precision = TMrbString::kDefaultPrecision);

		Bool_t ToDouble(Double_t & DblVal);					// convert string -> double

		Bool_t SplitOffInteger(TString & Prefix,			// split off trailing integer
									Int_t & IntVal,
									Int_t Base = TMrbString::kDefaultBase);

		Bool_t Increment(Int_t Increment = 1,				// increment trailing integer
									Int_t Base = TMrbString::kDefaultBase);

		Bool_t SplitOffDouble(TString & Prefix, Double_t DblVal);		// split off trailing double

		Bool_t Increment(Double_t Increment);				// increment trailing double

		Int_t Split(TObjArray & LofSubStrings, const Char_t * Separator = ":", Bool_t RemoveWhiteSpace = kFALSE); // split into substrings

		Int_t ReplaceWhiteSpace(const Char_t WhiteSpace = ' ');		// replace multiple white spaces (blanks or tabs) by given one

		void Expand(const Char_t Escape = '%');									// expand string
		virtual void ProcessEscapeSequence(Char_t CharId, TString & Replacement);

 		inline TMrbString & operator=(const Int_t n) { return(FromInteger(n)); };
		inline TMrbString & operator=(const Double_t d) { return(FromDouble(d)); };
		inline TMrbString & operator+=(const Int_t n) { return(AppendInteger(n)); };
		inline TMrbString & operator+=(const Double_t d) { return(AppendDouble(d)); };

		inline TMrbString & operator=(const Char_t * s) { return((TMrbString &) this->Replace(0, Length(), s, strlen(s))); };
		inline TMrbString & operator=(const TMrbString & s) { return((TMrbString &) this->Replace(0, Length(), s.Data(), s.Length())); };
		inline TMrbString & operator+=(const Char_t * s) { return((TMrbString &) this->Append(s, strlen(s))); };
		inline TMrbString & operator+=(const TString & s) { return((TMrbString &) this->Append(s.Data(), s.Length())); };

		inline TMrbString & operator=(const TSubString & s) { TString tmp = s; *this = tmp; return(*this); };

		inline const Char_t * Fill(Int_t n, const Char_t * Pattern = " ", Int_t Pos = 0) { if (n > 0) { for (; n--;) Insert(Pos, Pattern); } return(fData); };

		inline void SetBase(Int_t Base) { fBase = Base; };
		inline void ResetBase() { fBase = TMrbString::kDefaultBase; };

 	protected:
		Bool_t CheckBase(Int_t Base, Char_t * Method) const;	// check base
		Int_t CheckInteger(Int_t Base) const;					// check if string valid integer
		Int_t CheckDouble() const; 								// check if string valid double

	protected:
		Int_t fBase;
		
	ClassDef(TMrbString, 0) 	// [Utils] Strings with integer/float numbers
};

#endif
