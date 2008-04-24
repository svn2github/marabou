//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TNamed                                                               //
//                                                                      //
// The TNamed class is the base class for all named ROOT classes        //
// A TNamed contains the essential elements (name, title)               //
// to identify a derived object in containers, directories and files.   //
// Most member functions defined in this base class are in general      //
// overridden by the derived classes.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"

//______________________________________________________________________________
Int_t TNamed::Compare(const TObject *obj) const
{
   // Compare two TNamed objects. Returns 0 when equal, -1 when this is
   // smaller and +1 when bigger (like strcmp).

   if (this == obj) return 0;
   return fName.CompareTo(obj->GetName());
}

//______________________________________________________________________________
void TNamed::SetName(const char *name)
{
   // Change (i.e. set) the name of the TNamed.
   // WARNING: if the object is a member of a THashTable or THashList container
   // the container must be Rehash()'ed after SetName(). For example the list
   // of objects in the current directory is a THashList.

   fName = name;
}

//______________________________________________________________________________
void TNamed::SetNameTitle(const char *name, const char *title)
{
   // Change (i.e. set) all the TNamed parameters (name and title).
   // WARNING: if the name is changed and the object is a member of a
   // THashTable or THashList container the container must be Rehash()'ed
   // after SetName(). For example the list of objects in the current
   // directory is a THashList.

   fName  = name;
   fTitle = title;
}

//______________________________________________________________________________
void TNamed::SetTitle(const char *title)
{
   // Change (i.e. set) the title of the TNamed.

   fTitle = title;
}
