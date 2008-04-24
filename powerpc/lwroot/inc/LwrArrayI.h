#ifndef __LwrArrayI_h__
#define __LwrArrayI_h__

#include "LwrArray.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArrayI                                                              //
//                                                                      //
// Array of integers (32 bits per element).                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TArrayI : public TArray {

public:
   Int_t    *fArray;       //[fN] Array of fN 32 bit integers

   TArrayI();
   TArrayI(Int_t n);
   TArrayI(Int_t n, const Int_t *array);
   TArrayI(const TArrayI &array);
   TArrayI    &operator=(const TArrayI &rhs);
   virtual    ~TArrayI();

   void         Adopt(Int_t n, Int_t *array);
   void         AddAt(Int_t i, Int_t idx);
   Int_t        At(Int_t i) const ;
   void         Copy(TArrayI &array) const {array.Set(fN,fArray);}
   const Int_t *GetArray() const { return fArray; }
   Int_t       *GetArray() { return fArray; }
   Double_t     GetAt(Int_t i) const { return At(i); }
   Stat_t       GetSum() const {Stat_t sum=0; for (Int_t i=0;i<fN;i++) sum+=fArray[i]; return sum;}
   void         Reset()           {memset(fArray, 0, fN*sizeof(Int_t));}
   void         Reset(Int_t val)  {for (Int_t i=0;i<fN;i++) fArray[i] = val;}
   void         Set(Int_t n);
   void         Set(Int_t n, const Int_t *array);
   void         SetAt(Double_t v, Int_t i) { AddAt((Int_t)v, i); }
   Int_t       &operator[](Int_t i);
   Int_t        operator[](Int_t i) const;
};

inline Int_t TArrayI::At(Int_t i) const
{
   if (!BoundsOk("TArrayI::At", i)) return 0;
   return fArray[i];
}

inline Int_t &TArrayI::operator[](Int_t i)
{
   if (!BoundsOk("TArrayI::operator[]", i))
      i = 0;
   return fArray[i];
}

inline Int_t TArrayI::operator[](Int_t i) const
{
   if (!BoundsOk("TArrayI::operator[]", i)) return 0;
   return fArray[i];
}

#endif
