//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \brief			Light Weight ROOT: TObjString
//! \details		Class definitions for ROOT under LynxOs: TObjString
//! 				Collectable string class<br>
//! 				This is a TObject containing a TString.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-18 13:14:45 $
//////////////////////////////////////////////////////////////////////////////


#include "LwrObjString.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Compares two TObjString objects.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	0 when strings are equal
//! 				<li>	-1 if string pointed to by \b this is smaller
//! 				<li>	+1 if string pointed to by \b this is bigger
//! 						than string in \b Obj (like strcmp)
//! 				</ul>
//! \param[in]		Obj				-- object to be compared
//! \retval 		Relation		-- {0, -1, +1}
//////////////////////////////////////////////////////////////////////////////

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
