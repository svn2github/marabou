//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrObjString.cxx
//! \brief			Light Weight ROOT: TObject
//! \details		Class definitions for ROOT under LynxOs: TObject
//! 				Mother of all ROOT objects<br>
//! 				The TObject class provides default behaviour and protocol for all
//! 				objects in the ROOT system. Every object which inherits
//! 				from TObject can be stored in the ROOT collection classes.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-17 09:27:16 $
//////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "LwrObject.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		TObject constructor. It sets the two data words of TObject to their
//! 				initial values. The unique ID is set to 0 and the status word is
//! 				set depending if the object is created on the stack or allocated
//! 				on the heap. Of the status word the high 8 bits are reserved for
//! 				system usage and the low 24 bits are user settable.
/////////////////////////////////////////////////////////////////////////////

TObject::TObject() :   fUniqueID(0), fBits(kNotDeleted) {}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		TObject copy constructor
//! \param[in]		Obj 	-- object to be copied
/////////////////////////////////////////////////////////////////////////////

TObject::TObject(const TObject & Obj)
{
   fUniqueID = Obj.fUniqueID;
   fBits     = Obj.fBits;

   fBits &= ~kIsReferenced;
   fBits &= ~kCanDelete;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		TObject assignment operator
//! \param[in]		Rhs 	-- object on right hand side
/////////////////////////////////////////////////////////////////////////////

TObject & TObject::operator=(const TObject & Rhs)
{
   if (this != &Rhs) {  			// when really unique don't copy
      fUniqueID = Rhs.fUniqueID;
      if (IsOnHeap()) {
         fBits  = Rhs.fBits;
         fBits |= kIsOnHeap;
      } else {
         fBits  = Rhs.fBits;
         fBits &= ~kIsOnHeap;
      }
      fBits &= ~kIsReferenced;
      fBits &= ~kCanDelete;
   }
   return *this;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Copy object
//! \param[in]		Obj 	-- object to hold the copy
/////////////////////////////////////////////////////////////////////////////

void TObject::Copy(TObject & Obj) const
{
   Obj.fUniqueID = fUniqueID;
   if (Obj.IsOnHeap()) {
      Obj.fBits  = fBits;
      Obj.fBits |= kIsOnHeap;
   } else {
      obj.fBits  = fBits;
      obj.fBits &= ~kIsOnHeap;
   }
   obj.fBits &= ~kIsReferenced;
   obj.fBits &= ~kCanDelete;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		TObject destructor
/////////////////////////////////////////////////////////////////////////////

TObject::~TObject()
{
   fBits &= ~kNotDeleted;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns class name "TObject"
/////////////////////////////////////////////////////////////////////////////

const Char_t * TObject::ClassName() const { return "TObject"; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns name of object.<br>
//! 				Classes that give objects a name should override this method.
/////////////////////////////////////////////////////////////////////////////

const Char_t * TObject::GetName() const { return "TObject"; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns the unique object id
/////////////////////////////////////////////////////////////////////////////

UInt_t TObject::GetUniqueID() const { return fUniqueID; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns title of object.<br>
//! 				Classes that give objects a name should override this method.
/////////////////////////////////////////////////////////////////////////////

const char *TObject::GetTitle() const { return "Base object (\"non-ROOT\" version)"; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns TRUE in case object contains browsable objects
//! 				(like containers or lists of other objects).
/////////////////////////////////////////////////////////////////////////////

Bool_t TObject::IsFolder() const {  return kFALSE; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Default equal comparison (objects are equal if they have
//! 				the same address in memory).<BR>
//! 				More complicated classes might want to override this function.
//! \return 		TRUE or FALSE
/////////////////////////////////////////////////////////////////////////////

Bool_t TObject::IsEqual(const TObject *obj) const { return obj == this; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets or unsets the user status bits as specified
//! \param[in]		Bits		-- bits to be (un)set
//! \param[in]		SetFlag 	-- TRUE = set, FALSE = unset	
/////////////////////////////////////////////////////////////////////////////

void TObject::SetBit(UInt_t Bits, Bool_t SetFlag) { if (SetFlag) SetBit(Bits); else ResetBit(Bits); }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets id as specified
//! \param[in]		Uid			-- user id bits
/////////////////////////////////////////////////////////////////////////////

void TObject::SetUniqueID(UInt_t Uid) { fUniqueID = Uid; }
