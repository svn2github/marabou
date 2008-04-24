#ifndef __LwrObjString_h__
#define __LwrObjString_h__


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TObjString                                                           //
//                                                                      //
// Collectable string class. This is a TObject containing a TString.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter, Apr 2008                                                  //
//////////////////////////////////////////////////////////////////////////

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

