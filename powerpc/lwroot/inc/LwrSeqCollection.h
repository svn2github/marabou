#ifndef __LwrSeqCollection_h__
#define __LwrSeqCollection_h__


//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrSeqCollection.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TSeqCollection<br>
//! 				Sequenceable collection abstract base class.<br>
//! 				TSeqCollections have an ordering relation,
//! 				i.e. there is a first and last element.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrCollection.h"


class TSeqCollection : public TCollection {

protected:
   Bool_t            fSorted;    // true if collection has been sorted

   TSeqCollection() : fSorted(kFALSE) { }
   virtual void      Changed() { fSorted = kFALSE; }

public:
   virtual           ~TSeqCollection() { }
   virtual void      Add(TObject *obj) { AddLast(obj); }
   virtual void      AddFirst(TObject *obj) = 0;
   virtual void      AddLast(TObject *obj) = 0;
   virtual void      AddAt(TObject *obj, Int_t idx) = 0;
   virtual void      AddAfter(const TObject *after, TObject *obj) = 0;
   virtual void      AddBefore(const TObject *before, TObject *obj) = 0;
   virtual void      RemoveFirst() { Remove(First()); }
   virtual void      RemoveLast() { Remove(Last()); }
   virtual TObject  *RemoveAt(Int_t idx) { return Remove(At(idx)); }
   virtual void      RemoveAfter(TObject *after) { Remove(After(after)); }
   virtual void      RemoveBefore(TObject *before) { Remove(Before(before)); }

   virtual TObject  *At(Int_t idx) const = 0;
   virtual TObject  *Before(const TObject *obj) const = 0;
   virtual TObject  *After(const TObject *obj) const = 0;
   virtual TObject  *First() const = 0;
   virtual TObject  *Last() const = 0;
   Int_t             LastIndex() const { return GetSize() - 1; }
   virtual Int_t     IndexOf(const TObject *obj) const;
   virtual Bool_t    IsSorted() const { return fSorted; }
   void              UnSort() { fSorted = kFALSE; }

   static Int_t      ObjCompare(TObject *a, TObject *b);
   static void       QSort(TObject **a, Int_t first, Int_t last);
   static void       QSort(TObject **a, TObject **b, Int_t first, Int_t last);

   inline const Char_t * ClassName() const { return "TSeqCollection"; };
};

#endif
