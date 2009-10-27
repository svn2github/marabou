#ifndef __LwrObject_h__
#define __LwrObject_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrObject.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TObject<br>
//! 				The TObject class provides default behaviour and
//! 				protocol for all objects in the ROOT system.<br>
//! 				It provides protocol for object I/O, error handling,
//! 				sorting, inspection, printing, drawing, etc.<br>
//! 				Every object which inherits from TObject can be stored
//! 				in the ROOT collection classes.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $
//! $Date: 2009-10-27 13:30:44 $
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>

#include "LwrTypes.h"

class TObject {

private:
   UInt_t         fUniqueID;   //object unique identifier
   UInt_t         fBits;       //bit field status word

protected:
   void MakeZombie() { fBits |= kZombie; }

public:
   //----- Global bits (can be set for any object and should not be reused).
   //----- Bits 0 - 13 are reserved as global bits. Bits 14 - 23 can be used
   //----- in different class hierarchies (make sure there is no overlap in
   //----- any given hierarchy).
   enum EStatusBits {
      kCanDelete        = BIT(0),   // if object in a list can be deleted
      kMustCleanup      = BIT(3),   // if object destructor must call RecursiveRemove()
      kObjInCanvas      = BIT(3),   // for backward compatibility only, use kMustCleanup
      kIsReferenced     = BIT(4),   // if object is referenced by a TRef or TRefArray
      kHasUUID          = BIT(5),   // if object has a TUUID (its fUniqueID=UUIDNumber)
      kCannotPick       = BIT(6),   // if object in a pad cannot be picked
      kNoContextMenu    = BIT(8),   // if object does not want context menu
      kInvalidObject    = BIT(13)   // if object ctor succeeded but object should not be used
   };

   //----- Private bits, clients can only test but not change them
   enum {
      kIsOnHeap      = 0x01000000,    // object is on heap
      kNotDeleted    = 0x02000000,    // object has not been deleted
      kZombie        = 0x04000000,    // object ctor failed
      kBitMask       = 0x00ffffff
   };

   //----- Write() options
   enum {
      kSingleKey     = BIT(0),        // write collection with single key
      kOverwrite     = BIT(1),        // overwrite existing object with same name
      kWriteDelete   = BIT(2)         // write object, then delete previous key with same name
   };

   TObject();
   TObject(const TObject &object);
   TObject &operator=(const TObject &rhs);
   virtual ~TObject();

   virtual const char *ClassName() const;
   virtual Int_t       Compare(const TObject *obj) const { return 1; };
   virtual void        Copy(TObject &object) const;
   virtual TObject    *FindObject(const char *name) const { return NULL; };
   virtual TObject    *FindObject(const TObject *obj) const { return NULL; };
   virtual UInt_t      GetUniqueID() const;
   virtual const char *GetName() const;
   virtual Option_t   *GetOption() const { return ""; }
   virtual const char *GetTitle() const;
   virtual ULong_t     Hash() const;
   virtual Bool_t      IsFolder() const;
   virtual Bool_t      IsEqual(const TObject *obj) const;
   virtual Bool_t      IsSortable() const { return kFALSE; }
           Bool_t      IsOnHeap() const { return TestBit(kIsOnHeap); }
           Bool_t      IsZombie() const { return TestBit(kZombie); }
   virtual void        SetUniqueID(UInt_t uid);

   //----- bit manipulation
   void     SetBit(UInt_t f, Bool_t set);
   void     SetBit(UInt_t f) { fBits |= f & kBitMask; }
   void     ResetBit(UInt_t f) { fBits &= ~(f & kBitMask); }
   Bool_t   TestBit(UInt_t f) const { return (Bool_t) ((fBits & f) != 0); }
   Int_t    TestBits(UInt_t f) const { return (Int_t) (fBits & f); }
   void     InvertBit(UInt_t f) { fBits ^= f & kBitMask; }

   //---- static functions
   static Long_t    GetDtorOnly();
   static void      SetDtorOnly(void *obj);
   static Bool_t    GetObjectStat();
   static void      SetObjectStat(Bool_t stat);

};

// Global bits (can be set for any object and should not be reused).
// Only here for backward compatibility reasons.
// For detailed description see TObject::EStatusBits above.
enum EObjBits {
   kCanDelete        = TObject::kCanDelete,
   kMustCleanup      = TObject::kMustCleanup,
   kObjInCanvas      = TObject::kObjInCanvas,
   kIsReferenced     = TObject::kIsReferenced,
   kHasUUID          = TObject::kHasUUID,
   kCannotPick       = TObject::kCannotPick,
   kNoContextMenu    = TObject::kNoContextMenu,
   kInvalidObject    = TObject::kInvalidObject
};

#endif
