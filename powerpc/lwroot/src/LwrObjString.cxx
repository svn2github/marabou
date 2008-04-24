//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TObjString                                                           //
//                                                                      //
// Collectable string class. This is a TObject containing a TString.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter, Apr 2008                                                  //
//////////////////////////////////////////////////////////////////////////

#include "LwrObjString.h"

//______________________________________________________________________________
Int_t TObjString::Compare(const TObject *obj) const
{
   // string compare the argument with this object.
   if (this == obj) return 0;
   return fString.CompareTo(((TObjString*)obj)->fString);
}

//______________________________________________________________________________
Bool_t TObjString::IsEqual(const TObject *obj) const
{
   // return kTRUE if the argument has the same content as this object.

   if (this == obj) return kTRUE;
   return fString == ((TObjString*)obj)->fString;
}
