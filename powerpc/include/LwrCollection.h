#ifndef __LwrCollection_h__
#define __LwrCollection_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrCollection.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TCollection<br>
//! 				Collection abstract base class.<br>
//! 				This class inherits from TObject because we want to be able
//!					to have collections of collections.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrObject.h"
#include "LwrIterator.h"
#include "LwrString.h"

const Bool_t kIterForward  = kTRUE;
const Bool_t kIterBackward = !kIterForward;

class TCollection : public TObject {

private:

   TCollection(const TCollection &);    //private and not-implemented, collections
   void operator=(const TCollection &); //are too complex to be automatically copied

protected:
   enum { kIsOwner = BIT(14) };

   TString   fName;               //name of the collection
   Int_t     fSize;               //number of elements in collection

   TCollection() : fName(), fSize(0) { }

public:
   enum { kInitCapacity = 16, kInitHashTableCapacity = 17 };

   virtual            ~TCollection() { }
   virtual void       Add(TObject *obj) = 0;
   void               AddVector(TObject *obj1, ...);
   virtual void       AddAll(const TCollection *col);
   Int_t              Capacity() const { return fSize; }
   virtual void       Clear(Option_t *option="") = 0;
   Int_t              Compare(const TObject *obj) const;
   Bool_t             Contains(const char *name) const { return FindObject(name) != 0; }
   Bool_t             Contains(const TObject *obj) const { return FindObject(obj) != 0; }
   virtual void       Delete(Option_t *option="") = 0;
   virtual TObject   *FindObject(const char *name) const;
   TObject           *operator()(const char *name) const;
   virtual TObject   *FindObject(const TObject *obj) const;
   virtual Int_t      GetEntries() const { return GetSize(); }
   virtual const char *GetName() const;
   virtual TObject  **GetObjectRef(const TObject *obj) const = 0;
   virtual Int_t      GetSize() const { return fSize; }
   Bool_t             IsArgNull(const char *where, const TObject *obj) const;
   virtual Bool_t     IsEmpty() const { return GetSize() <= 0; }
   virtual Bool_t     IsFolder() const { return kTRUE; }
   Bool_t             IsOwner() const { return TestBit(kIsOwner); }
   Bool_t             IsSortable() const { return kTRUE; }
   virtual TIterator *MakeIterator(Bool_t dir = kIterForward) const = 0;
   virtual TIterator *MakeReverseIterator() const { return MakeIterator(kIterBackward); }
   virtual TObject   *Remove(TObject *obj) = 0;
   virtual void       RemoveAll(TCollection *col);
   void               RemoveAll() { Clear(); }
   void               SetCurrentCollection();
   void               SetName(const char *name) { fName = name; }
   virtual void       SetOwner(Bool_t enable = kTRUE);

   static TCollection  *GetCurrentCollection();

   inline const Char_t * ClassName() const { return "TCollection"; };
   inline const Char_t * GetName() { return "TCollection"; };
   inline const Char_t * GetTitle() { return "Base collection object (\"non-ROOT\" version)"; };
};


//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TIter
//! 				Iterator wrapper.
//! 				Type of iterator used depends on type of collection.
//////////////////////////////////////////////////////////////////////////////

class TIter {

private:
   TIterator    *fIterator;         //collection iterator

protected:
   TIter() : fIterator(0) { }

public:
   TIter(const TCollection *col, Bool_t dir = kIterForward)
        : fIterator(col ? col->MakeIterator(dir) : 0) { }
   TIter(TIterator *it) : fIterator(it) { }
   TIter(const TIter &iter);
   TIter &operator=(const TIter &rhs);
   virtual            ~TIter() {};
   TObject           *operator()() { return fIterator ? fIterator->Next() : 0; }
   TObject           *Next() { return fIterator ? fIterator->Next() : 0; }
   const TCollection *GetCollection() const { return fIterator ? fIterator->GetCollection() : 0; }
   Option_t          *GetOption() const { return fIterator ? fIterator->GetOption() : ""; }
   void               Reset() { if (fIterator) fIterator->Reset(); }
};


//---- R__FOR_EACH macro -------------------------------------------------------

// Macro to loop over all elements of a list of type "type" while executing
// procedure "proc" on each element

#define R__FOR_EACH(type,proc) \
    SetCurrentCollection(); \
    TIter _NAME3_(nxt_,type,proc)(TCollection::GetCurrentCollection()); \
    type *_NAME3_(obj_,type,proc); \
    while ((_NAME3_(obj_,type,proc) = (type*) _NAME3_(nxt_,type,proc)())) \
       _NAME3_(obj_,type,proc)->proc

// deprecated macros (will be removed in next release)
#define ForEach(type,proc) \
    SetCurrentCollection(); \
    printf("please change ForEach to R__FOR_EACH in %s at line %d\n", __FILE__, __LINE__); \
    TIter _NAME3_(nxt_,type,proc)(TCollection::GetCurrentCollection()); \
    type *_NAME3_(obj_,type,proc); \
    while ((_NAME3_(obj_,type,proc) = (type*) _NAME3_(nxt_,type,proc)())) \
       _NAME3_(obj_,type,proc)->proc

#endif
