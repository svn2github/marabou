//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/src/TMrbEsoneCNAF.cxx
// Purpose:        ESONE client class
// Description:    Implements class methods to interface the ESONE client library
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"

#include "TMrbEsoneCNAF.h"
#include "TMrbEsoneCommon.h"

#include "SetColor.h"

ClassImp(TMrbEsoneCNAF)

Bool_t TMrbEsoneCNAF::Ascii2Int(const Char_t * Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Ascii2Int
// Purpose:        Decode a CNAF definition from Ascii to Int
// Arguments:      Char_t * Cnaf  -- CNAF definition
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes a CNAF from Ascii to Integer.
//                 Input is expected to be a dot-separated string: Cxx.Nxx.Axx.Fxx [D=xxx].
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString cnaf;
	TString cnafChars;
	TString cnafPart;
	TString cnafId;
	TString cnafData, data;
	Int_t cnafNum;
	Int_t base;
	Bool_t ok;
	TMrbEsoneCNAF tmpCnaf;

	tmpCnaf.Reset();

	cnaf = Cnaf;
	cnaf.ToUpper();

	cnafData.Resize(0);
	n1 = cnaf.Index("D=");
	if (n1 >= 0) {
		n1 += 2;
		cnafData = cnaf(n1, cnaf.Length() - n1);
		cnaf.Resize(n1);
	}
	cnaf += ".";
 
	cnafChars = "CNAFcnaf";

	n1 = 0;
	ok = kTRUE;

	for (;;) {
		n2 = cnaf.Index(".", n1);
		if (n2 == -1) break;
		cnafPart = cnaf(n1, n2 - n1);
		cnafPart = cnafPart.Strip(TString::kBoth);
		cnafId = cnafPart(0);
		if (cnafChars.Index(cnafId.Data()) == -1) {
			cerr	<< setred
					<< this->ClassName() << "::Ascii2Int(): Illegal CNAF syntax - " << cnafPart
					<< setblack << endl;
			return(kFALSE);
		}
		cnafId = cnafPart(1, n2 - n1 - 1);
		cnafNum = -1;
		istrstream c(cnafId.Data());
		c >> cnafNum;
		if (cnafNum == -1) {
			cerr	<< setred
					<< this->ClassName() << "::Ascii2Int(): Illegal CNAF syntax - " << cnafPart
					<< setblack << endl;
			return(kFALSE);
		}

		switch (cnafPart(0)) {
			case 'C':
				if (!tmpCnaf.Set(TMrbEsoneCNAF::kCnafCrate, cnafNum)) {
					cerr	<< setred
							<< this->ClassName() << "::Ascii2Int(): Illegal crate - " << cnafPart
							<< setblack << endl;
					ok = kFALSE;
				}
				break;

			case 'N':
				if (!tmpCnaf.Set(TMrbEsoneCNAF::kCnafStation, cnafNum)) {
					cerr	<< setred
							<< this->ClassName() << "::Ascii2Int(): Illegal station - " << cnafPart
							<< setblack << endl;
					ok = kFALSE;
				}
				break;

			case 'A':
				if (!tmpCnaf.Set(TMrbEsoneCNAF::kCnafAddr, cnafNum)) {
					cerr	<< setred
							<< this->ClassName() << "::Ascii2Int(): Illegal subaddr - " << cnafPart
							<< setblack << endl;
					ok = kFALSE;
				}
				break;

			case 'F':
				if (!tmpCnaf.Set(TMrbEsoneCNAF::kCnafFunction, cnafNum)) {
					cerr	<< setred
							<< this->ClassName() << "::Ascii2Int(): Illegal function - " << cnafPart
							<< setblack << endl;
					ok = kFALSE;
				}
				break;
		}
		n1 = n2 + 1;
	}

	if (!cnafData.IsNull()) {
		if (cnafData(0) == '0') {
			base = 8;
			n1 = 1;
			if (cnafData(1) == 'X') {
				base = 16;
				n1 = 2;
			}
		} else {
			base = 10;
			n1 = 0;
		}
		data = cnafData(n1, cnafData.Length() - n1);
		istrstream c(data.Data());
		switch (base) {
			case 8:		c >> setbase(8) >> cnafNum;
			case 10:	c >> cnafNum;
			case 16:	c >> setbase(16) >> cnafNum;
		}
		if (cnafNum == -1) {
			cerr	<< setred
					<< this->ClassName() << "::Ascii2Int(): Illegal CNAF syntax - D=" << cnafData
					<< setblack << endl;
			return(kFALSE);
		}
		if (!tmpCnaf.Set(TMrbEsoneCNAF::kCnafData, cnafNum)) {
			cerr	<< setred
					<< this->ClassName() << "::Ascii2Int(): Illegal data - D=" << cnafData
					<< setblack << endl;
			ok = kFALSE;
		}
	}

	if (!ok) return(kFALSE);

	fCrate = tmpCnaf.fCrate;
	fStation = tmpCnaf.fStation;
	fAddr = tmpCnaf.fAddr;
	fFunction = tmpCnaf.fFunction;
	fData = tmpCnaf.fData;
	fType = tmpCnaf.fType;

	return(kTRUE);
}

const Char_t * TMrbEsoneCNAF::Int2Ascii(Bool_t DataFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Int2Ascii
// Purpose:        Convert cnaf values to ascii format
// Arguments:      Bool_t DataFlag  -- kTRUE if data should be included
// Results:        Char_t * Cnaf    -- cnaf's ascii representation
// Exceptions:
// Description:    Generates the ascii representation of a cnaf
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * dot;

	fAscii.Resize(0);
	dot = "";
	ostrstream * str = new ostrstream();

	if (fCrate >= 0) {
		*str << dot << "C" << fCrate;
		dot = ".";
	}

	if (fStation > 0) {
		*str << dot << "N" << fStation;
		dot = ".";
	}

	if (fAddr >= 0) {
		*str << dot << "A" << fAddr;
		dot = ".";
	}

	if (fFunction >= 0) {
		*str << dot << "F" << fFunction;
	}

	if (DataFlag && (fData != kNoData)) {
		*str << " data=" << fData << "|0x" << setbase(16) << fData << setbase(10);
	}

	*str << ends;
	fAscii = str->str();
	str->rdbuf()->freeze(0);
	delete str;

	return(fAscii.Data());
}

Bool_t TMrbEsoneCNAF::Set(EMrbEsoneCNAF Cnaf, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Set
// Purpose:        Set cnaf values
// Arguments:      EMrbCNAF Ecnaf  -- cnaf component (enumeration)
//                 Int_t Value     -- cnaf value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Cnaf) {
		case TMrbEsoneCNAF::kCnafCrate:
			if (Value >= 0 && Value <= kMaxNofCrates) {
				fCrate = Value;
				return(kTRUE);
			}
		case TMrbEsoneCNAF::kCnafStation:
			if (Value >= 0 && Value <= 31) {
				fStation = Value;
				return(kTRUE);
			}
		case TMrbEsoneCNAF::kCnafAddr:
			if (Value >= 0 && Value <= 15) {
				fAddr = Value;
				return(kTRUE);
			}
		case TMrbEsoneCNAF::kCnafFunction:
			if (Value >= 0 && Value <= 31) {
				fFunction = Value;
				if (IS_F_READ(Value)) 				fType = TMrbEsoneCNAF::kCnafTypeRead;
				else if (IS_F_WRITE(Value)) 		fType = TMrbEsoneCNAF::kCnafTypeWrite;
				else if (IS_F_CNTL(Value)) {
					if (Value == 8 || Value == 27)	fType = TMrbEsoneCNAF::kCnafTypeReadStatus;
					else							fType = TMrbEsoneCNAF::kCnafTypeControl;
				} else return(kFALSE);
				return(kTRUE);
			}
		case TMrbEsoneCNAF::kCnafData:
			if (fType == TMrbEsoneCNAF::kCnafTypeWrite) {
				fData = Value;
				return(kTRUE);
			}
		default:	break;
	}
	return(kFALSE);
}

Bool_t TMrbEsoneCNAF::Set(Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, UInt_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Set
// Purpose:        Set cnaf values
// Arguments:      Int_t Crate                -- crate number
//                 Int_t Station              -- station
//                 Int_t Subaddr              -- subaddress
//                 CamacFunction_t Function   -- function code
//                 UInt_t Data                -- data
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	ok = kTRUE;

	fCrate = -1;	if (Crate >= 0 && Crate <= kMaxNofCrates) fCrate = Crate; else ok = kFALSE;
	fStation = -1;	if (Station >= 0 && Station <= 31) fStation = Station; else ok = kFALSE;
	fAddr = -1; 	if (Subaddr >= 0 && Subaddr <= 15) fAddr = Subaddr; else ok = kFALSE;
	fFunction = -1; if (Function >= 0 && Function <= 31) fFunction = Function; else ok = kFALSE;
	if (ok) {
		if (IS_F_READ(fFunction)) 					fType = TMrbEsoneCNAF::kCnafTypeRead;
		else if (IS_F_WRITE(fFunction)) 			fType = TMrbEsoneCNAF::kCnafTypeWrite;
		else if (IS_F_CNTL(fFunction)) {
			if (fFunction == 8 || fFunction == 27)	fType = TMrbEsoneCNAF::kCnafTypeReadStatus;
			else									fType = TMrbEsoneCNAF::kCnafTypeControl;
		} else fType = kCnafTypeUndefined;
	} else fType = kCnafTypeUndefined;

	if (fType == TMrbEsoneCNAF::kCnafTypeWrite && (Data != 0xffffffff)) fData = Data; else fData = kNoData;
	return(ok);
}

Int_t TMrbEsoneCNAF::Get(EMrbEsoneCNAF Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Get
// Purpose:        Get cnaf values
// Arguments:      EMrbEsoneCNAF Cnaf  -- cnaf component (enumeration)
// Results:        Int_t Value         -- cnaf value
// Exceptions:
// Description:    Gets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Cnaf) {
		case kCnafCrate:	return(fCrate);
		case kCnafStation:	return(fStation);
		case kCnafAddr: 	return(fAddr);
		case kCnafFunction: return(fFunction);
		case kCnafData: 	return(fData);
		default:			return(-1);
	}
	return(-1);
}

void TMrbEsoneCNAF::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Reset
// Purpose:        Reset data
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets internal data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fAscii.Resize(0);

	fCrate = -1;
	fStation = -1;
	fAddr = -1;
	fFunction = -1;
	fData = kNoData;
	fType = kCnafTypeUndefined;
}

CamacReg_t TMrbEsoneCNAF::Int2Esone(CamacHost_t HostAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::Int2Esone
// Purpose:        Convert cnaf to ESONE format
// Arguments:      CamacHost_t HostAddr   -- camac host address
// Results:        CamacReg_t             -- converted cnaf id
// Exceptions:
// Description:    Converts cnaf to ESONE format.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacReg_t creg;
	Int_t c, n, a;

	c = (fCrate == -1) ? 0 : fCrate;
	n = (fStation == -1) ? 0 : fStation;
	a = (fAddr == -1) ? 0 : fAddr;

	cdreg(&creg, HostAddr, c, n, a);
	return(creg);
}

Bool_t TMrbEsoneCNAF::CheckCnaf(UInt_t CnafBits) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::CheckCnaf
// Purpose:        Check cnaf for completeness & consistency
// Arguments:      UInt_t CnafBits   -- bits defining cnaf parts
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks a cnaf for completeness & consistency.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	ok = kTRUE;

	if (CnafBits & TMrbEsoneCNAF::kCnafCrate) {
		if (fCrate == -1) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Incomplete CNAF def \""
					<< this->Int2Ascii() << "\" - C missing"
					<< setblack << endl;
			ok = kFALSE;
		} else if (fCrate < 0 || fCrate > kMaxNofCrates) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Wrong CNAF def \""
					<< this->Int2Ascii() << "\" - illegal C"
					<< setblack << endl;
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCNAF::kCnafStation) {
		if (fStation == -1) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Incomplete CNAF def \""
					<< this->Int2Ascii() << "\" - N missing"
					<< setblack << endl;
			ok = kFALSE;
		} else if (fStation <= 0 || fStation > 31) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Wrong CNAF def \""
					<< this->Int2Ascii() << "\" - illegal N"
					<< setblack << endl;
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCNAF::kCnafAddr) {
		if (fAddr == -1) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Incomplete CNAF def \""
					<< this->Int2Ascii() << "\" - A missing"
					<< setblack << endl;
			ok = kFALSE;
		} else if (fAddr < 0 || fAddr > 15) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Wrong CNAF def \""
					<< this->Int2Ascii() << "\" - illegal A"
					<< setblack << endl;
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCNAF::kCnafFunction) {
		if (fFunction == -1) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Incomplete CNAF def \""
					<< this->Int2Ascii() << "\" - F missing"
					<< setblack << endl;
			ok = kFALSE;
		} else if (fFunction < 0 || fFunction > 31) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Wrong CNAF def \""
					<< this->Int2Ascii() << "\" - illegal F"
					<< setblack << endl;
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCNAF::kCnafData) {
		if (fType == TMrbEsoneCNAF::kCnafTypeWrite && (fData == kNoData)) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Incomplete CNAF def \""
					<< this->Int2Ascii() << "\" - data missing"
					<< setblack << endl;
			ok = kFALSE;
		} else if (fType != TMrbEsoneCNAF::kCnafTypeWrite && (fData != kNoData)) {
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Wrong CNAF def \""
					<< this->Int2Ascii() << "\" - can't take data"
					<< setblack << endl;
			ok = kFALSE;
		}
	}
	return(ok);
}

Bool_t TMrbEsoneCNAF::CheckCnaf(UInt_t CnafBit, Int_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCNAF::CheckCnaf
// Purpose:        Check cnaf component for consistency
// Arguments:      UInt_t CnafBit   -- single bit defining a cnaf component
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks a cnaf component for its validity.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	ok = kTRUE;

	switch (CnafBit) {
		case TMrbEsoneCNAF::kCnafCrate:
			if (Data < 0 || Data > kMaxNofCrates) {
				cerr	<< setred
						<< this->ClassName() << "::CheckCnaf(): Illegal crate - C" << Data
						<< ", should be in [0," << kMaxNofCrates << "]"
						<< setblack << endl;
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCNAF::kCnafStation:
			if (Data <= 0 || Data > 31) {
				cerr	<< setred
						<< this->ClassName() << "::CheckCnaf(): Illegal station - N" << Data
						<< ", should be in [1,31]"
						<< setblack << endl;
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCNAF::kCnafAddr:
			if (Data < 0 || Data > 15) {
				cerr	<< setred
						<< this->ClassName() << "::CheckCnaf(): Illegal subaddress - A" << Data
						<< ", should be in [0,15]"
						<< setblack << endl;
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCNAF::kCnafFunction:
			if (Data < 0 || Data > 31) {
				cerr	<< setred
						<< this->ClassName() << "::CheckCnaf(): Illegal function code - F"
						<< ", should be in [0,31]"
						<< setblack << endl;
				ok = kFALSE;
			}
			break;

		default:
			cerr	<< setred
					<< this->ClassName() << "::CheckCnaf(): Illegal CNAF specifier - " << CnafBit
					<< setblack << endl;
			ok = kFALSE;
	}
	return(ok);
}
