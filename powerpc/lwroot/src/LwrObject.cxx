//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TObject                                                              //
//                                                                      //
// Mother of all ROOT objects.                                          //
//                                                                      //
// The TObject class provides default behaviour and protocol for all    //
// objects in the ROOT system. It provides protocol for object I/O,     //
// error handling, sorting, inspection, printing, drawing, etc.         //
// Every object which inherits from TObject can be stored in the        //
// ROOT collection classes.                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "LwrObject.h"

//______________________________________________________________________________
TObject::TObject() :   fUniqueID(0), fBits(kNotDeleted)
{
   // TObject constructor. It sets the two data words of TObject to their
   // initial values. The unique ID is set to 0 and the status word is
   // set depending if the object is created on the stack or allocated
   // on the heap. Of the status word the high 8 bits are reserved for
   // system usage and the low 24 bits are user settable. Depending on
   // the ROOT environment variable "Root.MemStat" (see TEnv.h) the object
   // is added to the global TObjectTable for bookkeeping.
}

//______________________________________________________________________________
TObject::TObject(const TObject &obj)
{
   // TObject copy ctor.

   fUniqueID = obj.fUniqueID;  // when really unique don't copy
   fBits     = obj.fBits;

   fBits &= ~kIsReferenced;
   fBits &= ~kCanDelete;
}

//______________________________________________________________________________
TObject& TObject::operator=(const TObject &rhs)
{
   // TObject assignment operator.

   if (this != &rhs) {
      fUniqueID = rhs.fUniqueID;  // when really unique don't copy
      if (IsOnHeap()) {           // test uses fBits so don't move next line
         fBits  = rhs.fBits;
         fBits |= kIsOnHeap;
      } else {
         fBits  = rhs.fBits;
         fBits &= ~kIsOnHeap;
      }
      fBits &= ~kIsReferenced;
      fBits &= ~kCanDelete;
   }
   return *this;
}

//______________________________________________________________________________
void TObject::Copy(TObject &obj) const
{
   // Copy this to obj.

   obj.fUniqueID = fUniqueID;   // when really unique don't copy
   if (obj.IsOnHeap()) {        // test uses fBits so don't move next line
      obj.fBits  = fBits;
      obj.fBits |= kIsOnHeap;
   } else {
      obj.fBits  = fBits;
      obj.fBits &= ~kIsOnHeap;
   }
   obj.fBits &= ~kIsReferenced;
   obj.fBits &= ~kCanDelete;
}

//______________________________________________________________________________
TObject::~TObject()
{
   // TObject destructor. Removes object from all canvases and object browsers
   // if observer bit is on and remove from the global object table.

   fBits &= ~kNotDeleted;
}

//______________________________________________________________________________
const char * TObject::ClassName() const
{
   // Returns name of class to which the object belongs.

   return "TObject";
}

//______________________________________________________________________________
const char *TObject::GetName() const
{
   // Returns name of object. This default method returns the class name.
   // Classes that give objects a name should override this method.

   return "TObject";
}

//______________________________________________________________________________
UInt_t TObject::GetUniqueID() const
{
   // Return the unique object id.

   return fUniqueID;
}

//______________________________________________________________________________
const char *TObject::GetTitle() const
{
   // Returns title of object. This default method returns the class title
   // (i.e. description). Classes that give objects a title should override
   // this method.

   return "Base object (\"non-ROOT\" version)";
}

//______________________________________________________________________________
Bool_t TObject::IsFolder() const
{
   // Returns kTRUE in case object contains browsable objects (like containers
   // or lists of other objects).

   return kFALSE;
}
//______________________________________________________________________________
Bool_t TObject::IsEqual(const TObject *obj) const
{
   // Default equal comparison (objects are equal if they have the same
   // address in memory). More complicated classes might want to override
   // this function.

   return obj == this;
}

//______________________________________________________________________________
void TObject::SetBit(UInt_t f, Bool_t set)
{
   // Set or unset the user status bits as specified in f.

   if (set)
      SetBit(f);
   else
      ResetBit(f);
}

//______________________________________________________________________________
void TObject::SetUniqueID(UInt_t uid)
{
   // Set the unique object id.

   fUniqueID = uid;
}
