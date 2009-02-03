#ifndef __LwrNamed_h__
#define __LwrNamed_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamed.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TNamed
//! 				Base class for a named object (name, title)
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrObject.h"
#include "LwrString.h"

class TNamed : public TObject {

protected:
   TString   fName;            //object identifier
   TString   fTitle;           //object title

public:
   TNamed(): fName(), fTitle() { }
   TNamed(const char *name, const char *title) : fName(name), fTitle(title) { }
   TNamed(const TString &name, const TString &title) : fName(name), fTitle(title) { }
   TNamed(const TNamed &named);
   TNamed& operator=(const TNamed& rhs);
   virtual ~TNamed() { }
   virtual const char  *GetName() const { return fName; }
   virtual const char  *GetTitle() const { return fTitle; }
   virtual Int_t Compare(const TObject *obj) const;
   virtual Bool_t   IsSortable() const { return kTRUE; }
   virtual void     SetName(const char *name);
   virtual void     SetNameTitle(const char *name, const char *title);
   virtual void     SetTitle(const char *title="");

   inline const Char_t * ClassName() const { return "TNamed"; };
};

#endif
