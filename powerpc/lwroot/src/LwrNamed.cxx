//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamed.cxx
//! \brief			Light Weight ROOT: TNamed
//! \details		The TNamed class is the base class for all named ROOT classes.<br>
//! 				A TNamed contains the essential elements (name, title)
//! 				to identify a derived object in containers, directories and files.<br>
//! 				Most member functions defined in this base class are in general
//! 				overridden by the derived classes.
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-04-24 08:48:01 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Compares two TNamed objects.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	0 if objects are equal
//! 				<li>	-1 if name pointed to by \b this is smaller
//! 				<li>	+1 if name pointed to by \b this is bigger
//! 						than name of \b Obj (like strcmp).
//! 				</ul>
//! \param[in]		Obj				-- TNamed object to be compared
//! \retval 		Relation		-- {0, -1, +1}
//////////////////////////////////////////////////////////////////////////////

Int_t TNamed::Compare(const TObject * Obj) const
{
   if (this == Obj) return 0;
   return fName.CompareTo(Obj->GetName());
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets the name of TNamed object
//! \param[in]		Name			-- name to be set
//////////////////////////////////////////////////////////////////////////////

void TNamed::SetName(const Char_t * Name) { fName = Name; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets name and title of TNamed object
//! \param[in]		Name			-- name to be set
//! \param[in]		Title			-- title to be set
//////////////////////////////////////////////////////////////////////////////

void TNamed::SetNameTitle(const Char_t * Name, const Char_t * Title) { fName  = Name; fTitle = Title; }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets the title of TNamed object
//! \param[in]		Title			-- title to be set
//////////////////////////////////////////////////////////////////////////////

void TNamed::SetTitle(const Char_t * Title) { fTitle = Title; }
