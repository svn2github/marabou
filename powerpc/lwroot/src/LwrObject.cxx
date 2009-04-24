//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrObjString.cxx
//! \brief			Light Weight ROOT: TObject
//! \details		Class definitions for ROOT under LynxOs: TObject
//! 				Mother of all ROOT objects<br>
//! 				The TObject class provides default behaviour and protocol for all
//! 				objects in the ROOT system. Every object which inherits
//! 				from TObject can be stored in the ROOT collection classes.
//////////////////////////////////////////////////////////////////////////////

//__________________________________________________________[DOXYGEN MAINPAGE]
//////////////////////////////////////////////////////////////////////////////
//! \mainpage
//! Light Weight ROOT<br>
//! A selection of ROOT objects to be used with LynxOs.
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.6 $     
//! $Date: 2009-04-24 08:46:41 $
//////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "LwrObject.h"

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details	TObject constructor.<br>
//! 			It sets the two data words of TObject to their
//! 			initial values.<br>
//! 			The unique ID is set to 0 and the status word is
//! 			set depending if the object is created on the stack or allocated
//! 			on the heap.<br>
//! 			Status word:
//!				<ul>
//! 			<li>	bits 31 ... 24	-- reserved for system usage
//! 			<li>	bits 23 ... 0	-- may be set by user
//! 			</ul>
/////////////////////////////////////////////////////////////////////////////

TObject::TObject() :   fUniqueID(0), fBits(kNotDeleted) {}

//__________________________________________________________________[C++ ctor]
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
      Obj.fBits  = fBits;
      Obj.fBits &= ~kIsOnHeap;
   }
   Obj.fBits &= ~kIsReferenced;
   Obj.fBits &= ~kCanDelete;
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
