//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrObjString.cxx
//! \brief			Light Weight ROOT: TObjString
//! \details		Class definitions for ROOT under LynxOs: TObjString
//! 				Collectable string class<br>
//! 				This is a TObject containing a TString.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-17 09:27:16 $
//////////////////////////////////////////////////////////////////////////////


#include "LwrObjString.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Compares strings.<br>
//! 				Return value is < 0, == 0, or > 0
//! \param[in] 		Obj			-- object (=TObjString) to be compared
//! \retval 		Relation	-- relation between objects
/////////////////////////////////////////////////////////////////////////////

Int_t TObjString::Compare(const TObject * Obj) const
{
   if (this == Obj) return 0;
   return fString.CompareTo(((TObjString*) Obj)->fString);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Checks for equalness.<br>
//! \param[in] 		Obj			-- object (=TObjString) to be compared
//! \return 		TRUE or FALSE
/////////////////////////////////////////////////////////////////////////////

Bool_t TObjString::IsEqual(const TObject * Obj) const
{
   if (this == Obj) return kTRUE;
   return fString == ((TObjString*) Obj)->fString;
}
