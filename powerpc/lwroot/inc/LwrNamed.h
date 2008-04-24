#ifndef __LwrNamed_h__
#define __LwrNamed_h__


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TNamed                                                               //
//                                                                      //
// The basis for a named object (name, title).                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////


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
