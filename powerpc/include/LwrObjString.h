#ifndef __LwrObjString_h__
#define __LwrObjString_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrObjString.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TObjString<br>
//! 				Collectable string class: a TObject containing a TString.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrObject.h"
#include "LwrString.h"


class TObjString : public TObject {

private:
   TString    fString;       // wrapped TString

public:
   TObjString(const char *s = "") : fString(s) { }
   TObjString(const TObjString &s) : TObject(), fString(s.fString) { }
   ~TObjString() { }
   Int_t       Compare(const TObject *obj) const;
   const char *GetName() const { return fString; }
   Bool_t      IsSortable() const { return kTRUE; }
   Bool_t      IsEqual(const TObject *obj) const;
   void        SetString(const char *s) { fString = s; }
   TString     GetString() const { return fString; }
   TString    &String() { return fString; }

   inline const Char_t * ClassName() const { return "TObjString"; };
};

#endif

