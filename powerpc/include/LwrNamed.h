#ifndef __LwrNamed_h__
#define __LwrNamed_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamed.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TNamed<br>
//! 				Base class for a named object (name, title)
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-18 13:14:45 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrObject.h"
#include "LwrString.h"

class TNamed : public TObject {

protected:
   TString   fName;            //!< object name
   TString   fTitle;           //!< object title

public:
   TNamed(): fName(), fTitle() { }
   TNamed(const Char_t * Name, const Char_t * Title) : fName(Name), fTitle(Title) { }
   TNamed(const TString & Name, const TString & Title) : fName(Name), fTitle(Title) { }
   TNamed(const TNamed & Named);
   TNamed& operator=(const TNamed & Rhs);
   virtual ~TNamed() { }
   virtual const Char_t * GetName() const { return fName; }
   virtual const Char_t * GetTitle() const { return fTitle; }
   virtual Int_t Compare(const TObject * Obj) const;
   virtual Bool_t   IsSortable() const { return kTRUE; }
   virtual void     SetName(const Char_t * Name);
   virtual void     SetNameTitle(const Char_t * Name, const Char_t * Title);
   virtual void     SetTitle(const Char_t * Title="");

   inline const Char_t * ClassName() const { return "TNamed"; };
};

#endif
