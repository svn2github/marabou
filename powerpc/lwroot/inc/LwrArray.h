// @(#)root/cont:$Id: LwrArray.h,v 1.1 2008-04-24 11:40:21 Rudolf.Lutter Exp $
// Author: Fons Rademakers   21/10/97

#ifndef __LwrArray_h__
#define __LwrArray_h__


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArray                                                               //
//                                                                      //
// Abstract array base class. Used by TArrayC, TArrayS, TArrayI,        //
// TArrayL, TArrayF and TArrayD.                                        //
// Data member is public for historical reasons.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include <string.h>

class TArray {

protected:
   Bool_t        OutOfBoundsError(const char *where, Int_t i) const;
   inline Bool_t BoundsOk(const char *where, Int_t at) const
   {
      return (at < 0 || at >= fN)
                  ? OutOfBoundsError(where, at)
                  : kTRUE;
   };


public:
   Int_t     fN;            //Number of array elements

   TArray(): fN(0) { }
   TArray(Int_t n): fN(n) { }
   TArray(const TArray &a): fN(a.fN) { }
   TArray         &operator=(const TArray &rhs) 
     {if(this!=&rhs) fN = rhs.fN; return *this; }
   virtual        ~TArray() { fN = 0; }

   Int_t          GetSize() const { return fN; }
   virtual void   Set(Int_t n) = 0;

   virtual Double_t GetAt(Int_t i) const = 0;
   virtual void   SetAt(Double_t v, Int_t i) = 0;

};

#endif
