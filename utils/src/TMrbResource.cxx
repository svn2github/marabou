//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbResource.cxx
// Purpose:        Handle resource files
// Description:    Implements class methods to handle resource files
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbResource.cxx,v 1.1 2008-07-22 08:42:20 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TSystem.h"
#include "TObjString.h"
#include "TIterator.h"

#include "TMrbResource.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbResource)

extern TMrbLogger * gMrbLog;					// MARaBOU's message logger

TMrbResource::TMrbResource(const Char_t * Prefix, const Char_t * ResourceFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource
// Purpose:        Access to ROOT environment
// Arguments:      Char_t * Prefix         -- prefix
//                 Char_t * ResourceFile   -- ROOT env file
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (gSystem->AccessPathName(ResourceFile)) {
		gMrbLog->Err() << gSystem->GetError() << "-" << ResourceFile << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		fEnv = new TEnv(ResourceFile);
		fRcName = ResourceFile;
		fLofPrefixes.Delete();
		fLastPrefix = "";
		this->AddPrefix(Prefix);
	}
}

TMrbResource::TMrbResource(const Char_t * Prefix, TEnv * Environment) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource
// Purpose:        Access to ROOT environment
// Arguments:      Char_t * Prefix         -- prefix
//                 TEnv * Environment      -- ROOT env
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (Environment == NULL) Environment = gEnv;
	fEnv = Environment;
	fRcName = fEnv->GetRcName();
	fLofPrefixes.Delete();
	fLastPrefix = "";
	this->AddPrefix(Prefix);
}

void TMrbResource::AddPrefix(const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::AddPrefix()
// Purpose:        Add prefix to list
// Arguments:      Char_t * Prefix   -- prefix or :-separated list of prefs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds one or more prefixes to list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString prefStr = Prefix;
	Int_t from = 0;
	TString prefix;
	while (prefStr.Tokenize(prefix, from, ":")) fLofPrefixes.Add(new TObjString(prefix.Data()));
}

const Char_t * TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1      -- resource 1
//                 Char_t * Res2      -- resource 2
//                 Char_t * Res3      -- resource 3
//                 Char_t * Default   -- default value
// Results:        Char_t * Result    -- result
// Exceptions:
// Description:    Reads an env value
//                 Full resource name will be built from
//                 <Prefix>.<Res1>.<Res2>.<Res3>
//                 Example: "DGFControl.Module.DGF11.Crate"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	if (r) {
		TString s;
		if (this->ToString(r, s)) return(s.Data());
	}
	return(Default);
}

Int_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Results:        Int_t Result    -- result
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	if (r) {
		Int_t n;
		if (this->ToInteger(r, n)) return(n);
	}
	return(Default);
}

Bool_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Results:        Bool_t Result    -- result
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	return(r ? fEnv->GetValue(r, Default) : Default);
}

TMrbNamedX * TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbLofNamedX * List) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1         -- resource 1
//                 Char_t * Res2         -- resource 2
//                 Char_t * Res3         -- resource 3
//                 TMrbLofNamedX * List  -- list of pairs N,X
// Results:        TMrbNamedX * Nx       -- result
// Exceptions:
// Description:    Reads an env value and searches for it in list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intRes;
	TString strRes;
	TString res;
	res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) return(NULL);
	if (!this->Convert(res, strRes, intRes)) return(NULL);
	TMrbNamedX * nx;
	if (intRes != 0xaffec0c0) nx = List->FindByIndex(intRes); else nx = List->FindByName(strRes);
	if (nx == NULL) return(kFALSE);
	if (intRes != 0xaffec0c0 && !strRes.IsNull()) {
		strRes.ToLower();
		TString x = nx->GetName();
		x.ToLower();
		if (!x.BeginsWith(strRes.Data())) {
			gMrbLog->Err()	<< "Mismatch - env=" << intRes << "/" << strRes
							<< " <--> list=" << nx->GetIndex() << "/" << nx->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "Get");
			return(NULL);
		}
	}
	return(nx);
}

const Char_t * TMrbResource::Find(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Find()
// Purpose:        Check if resource present
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- resource part 2
//                 Char_t * Res3      -- resource part 3
// Results:        Char_t * Resource  -- resulting resource
// Exceptions:
// Description:    Checks if environment contains a resource
//                 <Prefix1>.<Res1>.<ItemName>.<ResName>
//                 <Prefix1> will be taken from list of prefixes
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r = "";
	if (Res1) r += Form(".%s", Res1);
	if (Res2 && *Res2 != '\0') r += Form(".%s", Res2);
	if (Res3) r += Form(".%s", Res3);
	if (r.IsNull()) return(NULL);

	if (!fLastPrefix.IsNull()) {
		fResource = Form("%s%s", fLastPrefix.Data(), r.Data());
		if (fEnv->Lookup(fResource.Data())) return(fResource.Data());
	}

	TIterator * iter = fLofPrefixes.MakeIterator();
	TObjString * p;
	while(p = (TObjString *) iter->Next()) {
		fResource = Form("%s%s", p->GetString().Data(), r.Data());
		if (fEnv->Lookup(fResource.Data())) {
			fLastPrefix = p->GetString();
			return(fResource.Data());
		}
	}
	return(NULL);
}

Bool_t TMrbResource::Convert(const Char_t * Resource, TString & StrResult, Int_t & IntResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Convert()
// Purpose:        Convert resource
// Arguments:      Char_t * Resource    -- resource
//                 TString & StrResult  -- where to store ascii part
//                 Int_t & IntResult    -- where to store integer part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks for "x(n)" or "n(x)" and splits value into parts
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr = NULL;
	StrResult = "";
	IntResult = 0;

	TString r = fEnv->GetValue(Resource, "");
	if (r.IsNull()) return(kFALSE);
	StrResult = r;
	Int_t n1 = r.Index("(");
	Int_t n2 = r.Index(")");
	if (n1 < 0 && n2 < 0) {
		r = r.Strip(TString::kBoth);
		r.ToLower();
		if (r.IsDigit()) {
			IntResult = (Int_t) strtoul(r.Data(), &endptr, r.BeginsWith("0") ? 8 : 10);
		} else if (r.BeginsWith("0x")) {
			IntResult = (Int_t) strtoul(r.Data() + 2, &endptr, 16);
		} else if (r.BeginsWith("0b")) {
			IntResult = (Int_t) strtoul(r.Data() + 2, &endptr, 2);
		}
		if (endptr == NULL) {
			IntResult = 0xaffec0c0;
			return(kTRUE);
		}
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << r << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		StrResult = "";
		return(kTRUE);
	}
	if ((n1 > 0) && (n2 < 0 || n2 < n1)) return(kFALSE);
	if ((n2 > 0) && (n1 < 0)) return(kFALSE);
	TString s1 = r(0, n1); s1 = s1.Strip(TString::kBoth);
	TString s2 = r(n1 + 1, n2 - n1 - 1); s2 = s2.Strip(TString::kBoth);
	if (s1.IsDigit()) {
		IntResult = (Int_t) strtoul(s1.Data(), &endptr, s1.BeginsWith("0") ? 8 : 10);
	} else if (s1.BeginsWith("0x") || s1.BeginsWith("0X")) {
		IntResult = (Int_t) strtoul(s1.Data() + 2, &endptr, 16);
	} else if (s1.BeginsWith("0b") || s1.BeginsWith("0B")) {
		IntResult = (Int_t) strtoul(s1.Data() + 2, &endptr, 2);
	}
	if (endptr != NULL) {
		StrResult = s2;
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << s1 << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		return(kTRUE);
	}
	if (s2.IsDigit()) {
		IntResult = (Int_t) strtoul(s2.Data(), &endptr, s2.BeginsWith("0") ? 8 : 10);
	} else if (s2.BeginsWith("0x") || s2.BeginsWith("0X")) {
		IntResult = (Int_t) strtoul(s2.Data() + 2, &endptr, 16);
	} else if (s2.BeginsWith("0b") || s2.BeginsWith("0B")) {
		IntResult = (Int_t) strtoul(s2.Data() + 2, &endptr, 2);
	}
	if (endptr != NULL) {
		StrResult = s1;
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << s2 << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		return(kTRUE);
	}
	StrResult = r;
	IntResult = 0xaffec0c0;
	gMrbLog->Wrn() << "Can't extract integer part - " << r << endl;
	gMrbLog->Flush(this->ClassName(), "Convert");
	return(kTRUE);
}

Bool_t TMrbResource::ToInteger(const Char_t * Resource, Int_t & IntResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::ToInteger()
// Purpose:        Convert to integer
// Arguments:      Char_t * Resource    -- resource
//                 Int_t & IntResult    -- where to store integer part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to get the integer part of a resource
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	IntResult = 0;
	TString s;
	Int_t n = 0xaffec0c0;
	if (!this->Convert(Resource, s, n)) return(kFALSE);
	if (n == 0xaffec0c0) return(kFALSE);
	IntResult = n;
	return(kTRUE);
}

Bool_t TMrbResource::ToString(const Char_t * Resource, TString & StrResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::ToString()
// Purpose:        Convert to string
// Arguments:      Char_t * Resource      -- resource
//                 TString & StrResult    -- where to store string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to get a resource as string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	StrResult = "";
	Int_t n;
	TString s = "\001";
	if (!this->Convert(Resource, s, n)) return(kFALSE);
	if (s(0) == '\001') return(kFALSE);
	StrResult = s;
	return(kTRUE);
}
