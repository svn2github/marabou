//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCNAF.cxx
// Purpose:        MARaBOU configuration: CNAF encode/decode
// Description:    Implements class methods to handle CNAFs
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

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCNAF.h"

#include "SetColor.h"

extern TMrbConfig *gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCNAF)

Bool_t TMrbCNAF::Ascii2Int(const Char_t * BorCNAF) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Ascii2Int
// Purpose:        Decode a CNAF definition from Ascii to Int
// Arguments:      Char_t * BorCNAF  -- [B]CNAF definition
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes a [B]CNAF from Ascii to Integer.
//                 Input is expected to be a dot-separated string: [Bxx.]Cxx.Nxx.Axx.Fxx .
//                 Input is compared against bits in fMayBeSet
//                 (which is set to "all legal" initially).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register char * sp1, * sp2;
	Char_t cnaf[64];
	Char_t number[64];
	Int_t n, ndum;
	Int_t nn;
	Bool_t iscnaf, ok;
	TMrbCNAF *thisCnaf;

	thisCnaf = new TMrbCNAF();

	sprintf(cnaf, "%s.", BorCNAF);
	sp1 = cnaf;
	while (*sp1) *sp1++ = toupper(*sp1);
	sp1 = cnaf;
	ok = kTRUE;

	for (;;) {
		while (isspace(*sp1)) sp1++;
		sp2 = strchr(sp1, '.');
		if (sp2 == NULL) break;
		*sp2 = '\0';
		if (isalpha(*sp1)) {
			sp1++;
			iscnaf = kTRUE;
		} else {
			iscnaf = kFALSE;
		}
		sprintf(number, "%s 1", sp1);
		n = sscanf(number, "%d %d\n", &nn, &ndum);
		if (n == 2 && ndum == 1) {
			if (iscnaf) {
				switch (*(sp1 - 1)) {
					case 'B':
						if (!thisCnaf->Set(kCnafBranch, nn)) {
							gMrbLog->Err() << "Illegal branch - B" << nn << endl;
							gMrbLog->Flush(this->ClassName(), "Ascii2Int");
							ok = kFALSE;
						}
						break;
					case 'C':
						if (!thisCnaf->Set(kCnafCrate, nn)) {
							gMrbLog->Err() << "Illegal crate - C" << nn << endl;
							gMrbLog->Flush(this->ClassName(), "Ascii2Int");
							ok = kFALSE;
						}
						break;
					case 'N':
						if (!thisCnaf->Set(kCnafStation, nn)) {
							gMrbLog->Err() << "Illegal station - N" << nn << endl;
							gMrbLog->Flush(this->ClassName(), "Ascii2Int");
							ok = kFALSE;
						}
						break;
					case 'A':
						if (!thisCnaf->Set(kCnafAddr, nn)) {
							gMrbLog->Err() << "Illegal subaddr - A" << nn << endl;
							gMrbLog->Flush(this->ClassName(), "Ascii2Int");
							ok = kFALSE;
						}
						break;
					case 'F':
						if (!thisCnaf->Set(kCnafFunction, nn)) {
							gMrbLog->Err() << "Illegal function - F" << nn << endl;
							gMrbLog->Flush(this->ClassName(), "Ascii2Int");
							ok = kFALSE;
						}
						break;
					default:
						gMrbLog->Err() << "Illegal syntax - " << sp1 << endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
						ok = kFALSE;
				}
			} else {
				if (!thisCnaf->Set(kCnafData, nn)) {
					gMrbLog->Err() << "Illegal data - " << nn << endl;
					gMrbLog->Flush(this->ClassName(), "Ascii2Int");
					ok = kFALSE;
				}
			}
		} else {
			gMrbLog->Err() << "Not a number - " << sp1 << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			ok = kFALSE;
		}

		sp1 = sp2 + 1;
	}

	if (ok) {
		UInt_t isSet = thisCnaf->IsSet();
		if ((isSet & fMayBeSet) != isSet) {
			gMrbLog->Err() << "Illegal CNAF spec - " << BorCNAF << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			ok = kFALSE;
		}

		if ((isSet & fMustBeSet) != fMustBeSet) {
			gMrbLog->Err() << "Incomplete CNAF spec - " << BorCNAF << endl;
			gMrbLog->Flush(this->ClassName(), "Ascii2Int");
			ok = kFALSE;
		}
	}

	if (ok) {
		fIsSet = thisCnaf->IsSet();
		fBranch = thisCnaf->Get(kCnafBranch);
		fCrate = thisCnaf->Get(kCnafCrate);
		fStation = thisCnaf->Get(kCnafStation);
		fAddr = thisCnaf->Get(kCnafAddr);
		fFunction = thisCnaf->Get(kCnafFunction);
		fData = thisCnaf->Get(kCnafData);
	}

	delete thisCnaf;

	return(ok);
}

const Char_t * TMrbCNAF::Int2Ascii(Bool_t StripBorC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Int2Ascii
// Purpose:        Convert cnaf values to ascii format
// Arguments:      Bool_t StripBorC	-- if kTRUE, strips off B0 and C0
// Results:        Char_t *BCNAF    -- cnaf's ascii representation
// Exceptions:
// Description:    Generates the ascii presentation of a cnaf
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * dot;

	fAscii.Remove(0);
	dot = "";
	ostrstream * str = new ostrstream();

	if ((fIsSet & kCnafBranch) && ((fBranch > 0) || !StripBorC)) {
		*str << dot << "B" << fBranch;
		dot = ".";
	}

	if ((fIsSet & kCnafCrate) && ((fCrate > 0) || !StripBorC)) {
		*str << dot << "C" << fCrate;
		dot = ".";
	}

	if (fIsSet & kCnafStation) {
		*str << dot << "N" << fStation;
		dot = ".";
	}

	if (fIsSet & kCnafAddr) {
		*str << dot << "A" << fAddr;
		dot = ".";
	}

	if (fIsSet & kCnafFunction) {
		*str << dot << "F" << fFunction;
	}

	if (fIsSet & kCnafData) {
		*str << " data=" << fData;
	}

	*str << ends;
	fAscii = str->str();
	str->rdbuf()->freeze(0);
	delete str;

	return(fAscii.Data());
}

Bool_t TMrbCNAF::Set(const Char_t * Scnaf, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Set
// Purpose:        Set cnaf values
// Arguments:      Char_t * Scnaf  -- cnaf component (ascii representation)
//                 Int_t Value     -- cnaf value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register TMrbNamedX *kp;

	if ((kp = gMrbConfig->fCNAFNames.FindByName(Scnaf, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase)) != NULL) {
		return(Set((TMrbCNAF::EMrbCNAF) kp->GetIndex(), Value));
	} else {
		gMrbLog->Err() << "Illegal key word - " << Scnaf << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbCNAF::Set(TMrbCNAF::EMrbCNAF Ecnaf, Int_t value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Set
// Purpose:        Set cnaf values
// Arguments:      EMrbCNAF Ecnaf  -- cnaf component (enumeration)
//                 Int_t Value     -- cnaf value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Ecnaf) {
		case kCnafBranch:
			if ((fMayBeSet & kCnafBranch) && (value >= 0 && value <= 7)) {
				fBranch = value;
				fIsSet |= kCnafBranch;
				return(kTRUE);
			}
		case kCnafCrate:
			if ((fMayBeSet & kCnafCrate) && (value >= 0 && value <= 7)) {
				fCrate = value;
				fIsSet |= kCnafCrate;
				return(kTRUE);
			}
		case kCnafStation:
			if ((fMayBeSet & kCnafStation) && (value >= 0 && value <= 31)) {
				fStation = value;
				fIsSet |= kCnafStation;
				return(kTRUE);
			}
		case kCnafAddr:
			if ((fMayBeSet & kCnafAddr) && (value >= 0 && value <= 15)) {
				fAddr = value;
				fIsSet |= kCnafAddr;
				return(kTRUE);
			}
		case kCnafFunction:
			if ((fMayBeSet & kCnafFunction) && (value >= 0 && value <= 31)) {
				fFunction = value;
				fIsSet |= kCnafFunction;
				if (value >= 16 && value <= 23) {
					fMustBeSet |= kCnafData;
				} else {
					fMayBeSet &= ~kCnafData;
				}
				return(kTRUE);
			}
		case kCnafData:
			if (fMayBeSet & kCnafData) {
				fData = value;
				fIsSet |= kCnafData;
				return(kTRUE);
			}
		default:	break;
	}
	return(kFALSE);
}

Int_t TMrbCNAF::Get(const Char_t * Scnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Get
// Purpose:        Get cnaf values
// Arguments:      Char_t * Scnaf  -- cnaf component (ascii representation)
// Results:        Int_t Value     -- cnaf value
// Exceptions:
// Description:    Gets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register TMrbNamedX *kp;

	if ((kp = gMrbConfig->fCNAFNames.FindByName(Scnaf, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase)) != NULL) {
		return(Get((TMrbCNAF::EMrbCNAF) kp->GetIndex()));
	} else {
		gMrbLog->Err() << "Illegal key word - " << Scnaf << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(kFALSE);
	}
}

Int_t TMrbCNAF::Get(TMrbCNAF::EMrbCNAF Ecnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCNAF::Get
// Purpose:        Get cnaf values
// Arguments:      EMrbCNAF Ecnaf  -- cnaf component (enumeration)
// Results:        Int_t Value     -- cnaf value
// Exceptions:
// Description:    Gets some cnaf values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fIsSet & Ecnaf) {
		switch (Ecnaf) {
			case kCnafBranch:	return(fBranch);
			case kCnafCrate:	return(fCrate);
			case kCnafStation:	return(fStation);
			case kCnafAddr: 	return(fAddr);
			case kCnafFunction: return(fFunction);
			case kCnafData: 	return(fData);
			default:		return(-1);
		}
	}
	return(-1);
}
