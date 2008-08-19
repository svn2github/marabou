#ifndef __TMrbResource_h__
#define __TMrbResource_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbResource.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbResource       -- access to TEnv objects
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbResource.h,v 1.2 2008-08-18 08:18:57 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-08-18 08:18:57 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource
// Purpose:        Access to ROOT environment
// Description:    Get() and Set() methods to access or modify TEnv objects
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbResource : public TObject {

	public:
		TMrbResource() {};								// default ctor
		TMrbResource(const Char_t * Prefix, TEnv * Environment = NULL);
		TMrbResource(const Char_t * Prefix, const Char_t * ResourceFile);

		~TMrbResource() {}; 							// default dtor
		TMrbResource(const TMrbResource &) {};			// default copy ctor

		const Char_t * Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, const Char_t * Default);
		Int_t Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t Default);
		Bool_t Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t Default);
		Double_t Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Double_t Default);
		TMrbNamedX * Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbLofNamedX * List);

		inline const Char_t * Get(const Char_t * Res, const Char_t * Default) { return(this->Get(Res, NULL, NULL, Default)); };
		inline Int_t Get(const Char_t * Res, Int_t Default) { return(this->Get(Res, NULL, NULL, Default)); };
		inline Bool_t Get(const Char_t * Res, Bool_t Default) { return(this->Get(Res, NULL, NULL, Default)); };
		inline Double_t Get(const Char_t * Res, Double_t Default) { return(this->Get(Res, NULL, NULL, Default)); };
		inline TMrbNamedX * Get(const Char_t * Res, TMrbLofNamedX * List) { return(this->Get(Res, NULL, NULL, List)); };

		void Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, const Char_t * TrueFalse);
		void Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t TrueFalse);
		void Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t IntVal, const Char_t * StrVal = NULL);
		void Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Double_t Value);
		void Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbNamedX * Nx);

		inline void Set(const Char_t * Res, const Char_t * TrueFalse) { this->Set(Res, NULL, NULL, TrueFalse); };
		inline void Set(const Char_t * Res, Bool_t TrueFalse) { this->Set(Res, NULL, NULL, TrueFalse); };
		inline void Set(const Char_t * Res, Int_t IntVal, const Char_t * StrVal = NULL) { this->Set(Res, NULL, NULL, IntVal, StrVal); };
		inline void Set(const Char_t * Res, Double_t Value) { this->Set(Res, NULL, NULL, Value); };
		inline void Set(const Char_t * Res, TMrbNamedX * Nx) { this->Set(Res, NULL, NULL, Nx); };

		inline void Write() {	fEnv->SaveLevel(kEnvChange);
								fEnv->SaveLevel(kEnvLocal);
								fEnv->SaveLevel(kEnvUser); };

		void AddPrefix(const Char_t * Prefix);

		const Char_t * Find(const Char_t * Res1, const Char_t * Res2 = NULL, const Char_t * Res3 = NULL);
		inline const Char_t * Find(const Char_t * Res1, Int_t Index, const Char_t * Res3)
														{ return(this->Find(Res1, Form("%d", Index), Res3)); };

		inline TEnv * Env() { return(fEnv); };
		inline const Char_t * GetRcName() { return(fRcName.Data()); };
		inline TObjArray * LofPrefixes() { return(&fLofPrefixes); };

	protected:
		Bool_t Convert(const Char_t * Resource, TString & StrResult, Int_t & IntResult);
		Bool_t ToInteger(const Char_t * Resource, Int_t & IntResult);
		Bool_t ToString(const Char_t * Resource, TString & StrResult);

	protected:
		TEnv * fEnv;				// ROOT's TEnv object
		TString fRcName;			// name of rc file
		TObjArray fLofPrefixes; 	// one or more prefixes may be scanned
		TString fLastPrefix;		// last prefix used

		TString fResource;			// temp storage
		TString fResult;			// ...

	ClassDef(TMrbResource, 1)		// [Utils] Access to ROOT environment
};

#endif