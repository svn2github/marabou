//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/src/TMrbEsoneCnaf.cxx
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

#include "TMrbLogger.h"
#include "TMrbEsoneCnaf.h"
#include "TMrbEsoneCommon.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

ClassImp(TMrbEsoneCnaf)

void TMrbEsoneCnaf::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::Reset
// Purpose:        Reset data
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets internal data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("esone.log");
	
	fAscii.Resize(0);

	fCrate = -1;
	fStation = -1;
	fAddr = -1;
	fFunction = -1;
	fDataRead = kEsoneNoData;
	fDataWrite = kEsoneNoData;
	fType = TMrbEsoneCnaf::kCnafTypeUndefined;
	fAction = TMrbEsoneCnaf::kCANone;
}

Bool_t TMrbEsoneCnaf::Ascii2Int(const Char_t * Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::Ascii2Int
// Purpose:        Decode a CNAF definition from Ascii to Int
// Arguments:      Char_t * Cnaf  -- Cnaf definition
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
	TMrbEsoneCnaf tmpCnaf;

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
			gMrbLog->Err() << "[" << Cnaf << "] Illegal CNAF syntax - " << cnafPart << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			return(kFALSE);
		}
		cnafId = cnafPart(1, n2 - n1 - 1);
		cnafNum = -1;
		istrstream c(cnafId.Data());
		c >> cnafNum;
		if (cnafNum == -1) {
			gMrbLog->Err() << "[" << Cnaf << "] Illegal CNAF syntax - " << cnafPart << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			return(kFALSE);
		}

		switch (cnafPart(0)) {
			case 'C':
				if (!tmpCnaf.SetC(cnafNum)) {
					gMrbLog->Err() << "{" << Cnaf << "] Illegal crate - " << cnafPart << endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
					ok = kFALSE;
				}
				break;

			case 'N':
				if (!tmpCnaf.SetN(cnafNum)) {
					gMrbLog->Err() << "[" << Cnaf << "] Illegal station - " << cnafPart << endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
					ok = kFALSE;
				}
				break;

			case 'A':
				if (!tmpCnaf.SetA(cnafNum)) {
					gMrbLog->Err() << "[" << Cnaf << "] Illegal subaddr - " << cnafPart << endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
					ok = kFALSE;
				}
				break;

			case 'F':
				if (!tmpCnaf.SetF(cnafNum)) {
					gMrbLog->Err() << "[" << Cnaf << "] Illegal function - " << cnafPart<< endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
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
			gMrbLog->Err() << "[" << Cnaf << "] Illegal CNAF syntax - D=" << cnafData << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			return(kFALSE);
		}
		if (!tmpCnaf.SetData(cnafNum)) {
			gMrbLog->Err() << "[" << Cnaf << "] Illegal data - D=" << cnafData << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			ok = kFALSE;
		}
	}

	if (!ok) return(kFALSE);

	fCrate = tmpCnaf.fCrate;
	fStation = tmpCnaf.fStation;
	fAddr = tmpCnaf.fAddr;
	fFunction = tmpCnaf.fFunction;
	fDataRead = tmpCnaf.fDataRead;
	fDataWrite = tmpCnaf.fDataWrite;
	fType = tmpCnaf.fType;

	return(kTRUE);
}

const Char_t * TMrbEsoneCnaf::Int2Ascii(Bool_t DataFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::Int2Ascii
// Purpose:        Convert cnaf values to ascii format
// Arguments:      Bool_t DataFlag  -- kTRUE if data should be included
// Results:        Char_t * Cnaf    -- cnaf's ascii representation
// Exceptions:
// Description:    Generates the ascii representation of a cnaf
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * dot;
	Int_t cData;

	fAscii.Resize(0);
	dot = "";

	if (fCrate >= 0) {
		fAscii += dot;
		fAscii += "C";
		fAscii += fCrate;
		dot = ".";
	}

	if (fStation > 0) {
		fAscii += dot;
		fAscii += "N";
		fAscii += fStation;
		dot = ".";
	}

	if (fAddr >= 0) {
		fAscii += dot;
		fAscii += "A";
		fAscii += fAddr;
		dot = ".";
	}

	if (fFunction >= 0) {
		fAscii += dot;
		fAscii += "F";
		fAscii += fFunction;
	}

	if (DataFlag) {
		if (this->IsRead()) 		cData = fDataRead;
		else if (this->IsWrite())	cData = fDataWrite;
		else						cData = kEsoneNoData;
		if (cData != kEsoneNoData) {
			fAscii += " data=";
			fAscii += cData;
			fAscii += "|0x";
			fAscii.AppendInteger(cData, 0, ' ', 16);
		}
	}

	return(fAscii.Data());
}

Bool_t TMrbEsoneCnaf::SetC(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::SetC
// Purpose:        Set crate C
// Arguments:      Int_t Crate    -- crate number C
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines crate number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fCrate = -1;
	if (Crate >= 0 && Crate <= kEsoneNofCrates) {
		fCrate = Crate;
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbEsoneCnaf::SetN(Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::SetN
// Purpose:        Set station N
// Arguments:      Int_t Station    -- station number N
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines station number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fStation = -1;
	if (Station >= 0 && Station <= 31) {
		fStation = Station;
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbEsoneCnaf::SetA(Int_t Addr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::SetA
// Purpose:        Set subaddress A
// Arguments:      Int_t Addr    -- subaddress A
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines camac subaddress.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fAddr = -1;
	if (Addr >= 0 && Addr <= 15) {
		fAddr = Addr;
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbEsoneCnaf::SetF(Int_t Function) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::SetF
// Purpose:        Set function F
// Arguments:      Int_t Function    -- function F
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines camac function & type of call.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFunction = -1;
	fType = TMrbEsoneCnaf::kCnafTypeUndefined;

	if (Function >= 0 && Function <= 31) {
		fFunction = Function;
		if (IS_F_READ(Function)) 					fType = TMrbEsoneCnaf::kCnafTypeRead;
		else if (IS_F_WRITE(Function)) 				fType = TMrbEsoneCnaf::kCnafTypeWrite;
		else if (IS_F_CNTL(Function)) {
			if (Function == 8 || Function == 27)	fType = TMrbEsoneCnaf::kCnafTypeReadStatus;
			else									fType = TMrbEsoneCnaf::kCnafTypeControl;
		} else return(kFALSE);
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbEsoneCnaf::SetData(Int_t Data, EMrbCnafType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::SetData
// Purpose:        Set data
// Arguments:      Int_t Data         -- data word
//                 EMrbCnafType Type  -- type of data (read / write)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines camac data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Type == kCnafTypeUndefined) {
		if (this->IsRead()) 			fDataRead = Data;
		else if (this->IsWrite())		fDataWrite = Data;
		else if (this->IsControl()) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - CONTROL cnaf can't take data" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(kFALSE);
		} else {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - neither READ nor WRITE" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		if (Type & (kCnafTypeRead | kCnafTypeControl))	fDataRead = Data;
		if (Type & kCnafTypeWrite)						fDataWrite = Data;
		return(kTRUE);
	}
	return(kFALSE); 	// will never get here ...
}

Bool_t TMrbEsoneCnaf::ClearData(EMrbCnafType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::ClearData
// Purpose:        Clear data
// Arguments:      EMrbCnafType Type  -- type of data (read / write)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears data field.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Type == kCnafTypeUndefined) {
		if (this->IsRead() || this->IsControl()) 	fDataRead = kEsoneNoData;
		else if (this->IsWrite())					fDataWrite = kEsoneNoData;
		else {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - type undefined" << endl;
			gMrbLog->Flush(this->ClassName(), "ClearData");
			return(kFALSE);
		}
	} else {
		if (Type & (kCnafTypeRead | kCnafTypeControl))	fDataRead = kEsoneNoData;
		if (Type & kCnafTypeWrite)						fDataWrite = kEsoneNoData;
		return(kTRUE);
	}
	return(kFALSE); 	// will never get here ...
}

Int_t TMrbEsoneCnaf::GetData(EMrbCnafType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::GetData
// Purpose:        Get data
// Arguments:      EMrbCnafType Type  -- type of data (read / write)
// Results:        Int_t Data         -- data word
// Exceptions:
// Description:    Returns data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Type == kCnafTypeUndefined) {
		if (this->IsRead()) 			return(fDataRead & kEsoneDataMask);
		else if (this->IsWrite())		return(fDataWrite & kEsoneDataMask);
		else if (this->IsControl()) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - CONTROL cnaf doesn't have data" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(-1);
		} else {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - neither READ nor WRITE" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(-1);
		}
	} else {
		if (Type == kCnafTypeRead)			return(fDataRead & kEsoneDataMask);
		else if (Type == kCnafTypeWrite)	return(fDataWrite & kEsoneDataMask);
		else if (Type == kCnafTypeControl) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - CONTROL cnaf doesn't have data" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(-1);
		} else {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong cnaf type (0x"
							<< setbase(16) << Type << setbase(10)
							<< ") - neither READ nor WRITE" << endl;
			gMrbLog->Flush(this->ClassName(), "SetData");
			return(-1);
		}
	}
	return(-1); 	// will never get here ...
}

Bool_t TMrbEsoneCnaf::GetData(Int_t & DataRead, Int_t & DataWrite) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::GetData
// Purpose:        Get data
// Arguments:      Int_t & DataRead         -- data word (read)
//                 Int_t & DataWrite        -- data word (write)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Returns data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	DataRead = fDataRead;
	DataWrite = fDataWrite;
	return(kTRUE);
}

Bool_t TMrbEsoneCnaf::Set(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Int_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::Set
// Purpose:        Set cnaf values
// Arguments:      Int_t Crate      -- crate number
//                 Int_t Station    -- station
//                 Int_t Subaddr    -- subaddress
//                 Int_t Function   -- function code
//                 Int_t Data       -- data
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	ok = kTRUE;

	if (!this->SetC(Crate)) ok = kFALSE;
	if (!this->SetN(Station)) ok = kFALSE;
	if (!this->SetA(Subaddr)) ok = kFALSE;
	if (!this->SetF(Function)) ok = kFALSE;
	if (!this->SetData(Data)) ok = kFALSE;
	return(ok);
}

Bool_t TMrbEsoneCnaf::CheckCnaf(UInt_t CnafBits) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::CheckCnaf
// Purpose:        Check cnaf for completeness & consistency
// Arguments:      UInt_t CnafBits   -- bits defining cnaf parts
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks a cnaf for completeness & consistency.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	ok = kTRUE;

	if (CnafBits & TMrbEsoneCnaf::kCnafCrate) {
		if (fCrate == -1) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Incomplete CNAF def - C missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fCrate <= 0 || fCrate > kEsoneNofCrates) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - illegal crate C" << fCrate
							<< " (should be in [1," << kEsoneNofCrates << "])" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCnaf::kCnafStation) {
		if (fStation == -1) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Incomplete CNAF def - N missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fStation <= 0 || fStation > 31) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - illegal station N" << fStation
							<< " (should be in [1,31])" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCnaf::kCnafAddr) {
		if (fAddr == -1) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Incomplete CNAF def - A missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fAddr < 0 || fAddr > 15) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - illegal subaddr A" << fAddr
							<< " (should be in [0,15])" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCnaf::kCnafFunction) {
		if (fFunction == -1) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Incomplete CNAF def - F missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fFunction < 0 || fFunction > 31) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Wrong CNAF def - illegal subaddr A" << fAddr
							<< " (should be in [0,31])" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		}
	}
	if (CnafBits & TMrbEsoneCnaf::kCnafData) {
		if (fType == TMrbEsoneCnaf::kCnafTypeWrite && ((fDataWrite & kEsoneNoData) != 0)) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Incomplete CNAF def - WRITE cnaf needs data" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fType == TMrbEsoneCnaf::kCnafTypeRead && ((fDataRead & kEsoneNoData) == 0)) {
			gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Illegal CNAF def - READ cnaf can't take data" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
		} else if (fType == TMrbEsoneCnaf::kCnafTypeControl) {
			if (((fDataRead & kEsoneNoData) == 0) || ((fDataWrite & kEsoneNoData) == 0)) {
				gMrbLog->Err()	<< "[" << this->Int2Ascii()
								<< "] Illegal CNAF def - CONTROL cnaf can't take data" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckCnaf");
				ok = kFALSE;
			}
		}
	}
	return(ok);
}

Bool_t TMrbEsoneCnaf::CheckCnaf(UInt_t CnafBit, Int_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf::CheckCnaf
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
		case TMrbEsoneCnaf::kCnafCrate:
			if (Data <= 0 || Data > kEsoneNofCrates) {
				gMrbLog->Err()	<< "[" << this->Int2Ascii()
								<< "] Illegal crate - C" << Data
								<< " (should be in [1," << kEsoneNofCrates << "])" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckCnaf");
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCnaf::kCnafStation:
			if (Data <= 0 || Data > 31) {
				gMrbLog->Err()	<< "[" << this->Int2Ascii()
								<< "] Illegal station - N" << Data
								<< " (should be in [1,31])" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckCnaf");
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCnaf::kCnafAddr:
			if (Data < 0 || Data > 15) {
				gMrbLog->Err()	<< "[" << this->Int2Ascii()
								<< "] Illegal subaddr - A" << Data
								<< " (should be in [0,15])" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckCnaf");
				ok = kFALSE;
			}
			break;

		case TMrbEsoneCnaf::kCnafFunction:
			if (Data < 0 || Data > 31) {
				gMrbLog->Err()	<< "[" << this->Int2Ascii()
							<< "] Illegal function code - F"
							<< " (should be in [0,31])" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckCnaf");
				ok = kFALSE;
			}
			break;

		default:
			gMrbLog->Err()	<< "Illegal CNAF specifier - " << CnafBit << endl;
			gMrbLog->Flush(this->ClassName(), "CheckCnaf");
			ok = kFALSE;
	}
	return(ok);
}
