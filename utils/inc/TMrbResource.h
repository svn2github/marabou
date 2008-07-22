#ifndef __TMrbResource_h__
#define __TMrbResource_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbResource.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbResource       -- access to TEnv objects
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbResource.h,v 1.1 2008-07-22 08:42:20 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-07-22 08:42:20 $
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
		inline const Char_t * Get(const Char_t * Res1, Int_t Index, const Char_t * Res3, const Char_t * Default)
																	{ return(this->Get(Res1, Form("%d", Index), Res3, Default)); };

		Int_t Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t Default);
		inline Int_t Get(const Char_t * Res1, Int_t Index, const Char_t * Res3, Int_t Default)
																	{ return(this->Get(Res1, Form("%d", Index), Res3, Default)); };

		Bool_t Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t Default);
		inline Bool_t Get(const Char_t * Res1, Int_t Index, const Char_t * Res3, Bool_t Default)
																	{ return(this->Get(Res1, Form("%d", Index), Res3, Default)); };

		TMrbNamedX * Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbLofNamedX * List);
		inline TMrbNamedX * Get(const Char_t * Res1, Int_t Index, const Char_t * Res3, TMrbLofNamedX * List)
																	{ return(this->Get(Res1, Form("%d", Index), Res3, List)); };

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
