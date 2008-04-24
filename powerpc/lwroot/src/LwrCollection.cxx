//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Collection abstract base class. This class describes the base        //
// protocol all collection classes have to implement. The ROOT          //
// collection classes always store pointers to objects that inherit     //
// from TObject. They never adopt the objects. Therefore, it is the     //
// user's responsability to take care of deleting the actual objects    //
// once they are not needed anymore. In exceptional cases, when the     //
// user is 100% sure nothing else is referencing the objects in the     //
// collection, one can delete all objects and the collection at the     //
// same time using the Delete() function.                               //
//                                                                      //
// Collections can be iterated using an iterator object (see            //
// TIterator). Depending on the concrete collection class there may be  //
// some additional methods of iterating. See the repective classes.     //
//                                                                      //
// TCollection inherits from TObject since we want to be able to have   //
// collections of collections.                                          //
//                                                                      //
// In a later release the collections may become templatized.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include "LwrCollection.h"
#include "LwrVarargs.h"

//______________________________________________________________________________
void TCollection::AddAll(const TCollection *col)
{
   // Add all objects from collection col to this collection.

   TIter next(col);
   TObject *obj;

   while ((obj = next()))
      Add(obj);
}

//______________________________________________________________________________
void TCollection::AddVector(TObject *va_(obj1), ...)
{
   // Add all arguments to the collection. The list of objects must be
   // temrinated by 0, e.g.: l.AddVector(o1, o2, o3, o4, 0);

   va_list ap;
   va_start(ap, va_(obj1));
   TObject *obj;

   Add(va_(obj1));
   while ((obj = va_arg(ap, TObject *)))
      Add(obj);
   va_end(ap);
}

//______________________________________________________________________________
Int_t TCollection::Compare(const TObject *obj) const
{
   // Compare two TCollection objects. Returns 0 when equal, -1 when this is
   // smaller and +1 when bigger (like strcmp()).

   if (this == obj) return 0;
   return fName.CompareTo(obj->GetName());
}

//______________________________________________________________________________
TObject *TCollection::FindObject(const char *name) const
{
   // Find an object in this collection using its name. Requires a sequential
   // scan till the object has been found. Returns 0 if object with specified
   // name is not found.

   TIter next(this);
   TObject *obj;

   while ((obj = next()))
      if (!strcmp(name, obj->GetName())) return obj;
   return 0;
}

//______________________________________________________________________________
TObject *TCollection::operator()(const char *name) const
{
  // Find an object in this collection by name.

   return FindObject(name);
}

//______________________________________________________________________________
TObject *TCollection::FindObject(const TObject *obj) const
{
   // Find an object in this collection using the object's IsEqual()
   // member function. Requires a sequential scan till the object has
   // been found. Returns 0 if object is not found.
   // Typically this function is overridden by a more efficient version
   // in concrete collection classes (e.g. THashTable).

   TIter next(this);
   TObject *ob;

   while ((ob = next()))
      if (ob->IsEqual(obj)) return ob;
   return 0;
}

//______________________________________________________________________________
const char *TCollection::GetName() const
{
  // Return name of this collection.
  // if no name, return the collection class name.

   if (fName.Length() > 0) return fName.Data();
   return ClassName();
}

//______________________________________________________________________________
Bool_t  TCollection::IsArgNull(const char *where, const TObject *obj) const
{
   // Returns true if object is a null pointer.

	if (obj) return kFALSE;
	cerr << where << ": argument is a null pointer" << endl;
	return kTRUE;
}

//______________________________________________________________________________
void TCollection::RemoveAll(TCollection *col)
{
   // Remove all objects in collection col from this collection.

   TIter next(col);
   TObject *obj;

   while ((obj = next()))
      Remove(obj);
}

//______________________________________________________________________________
void TCollection::SetOwner(Bool_t enable)
{
   // Set whether this collection is the owner (enable==true)
   // of its content.  If it is the owner of its contents,
   // these objects will be deleted whenever the collection itself
   // is delete.   The objects might also be deleted or destructed when Clear
   // is called (depending on the collection).

   if (enable)
      SetBit(kIsOwner);
   else
      ResetBit(kIsOwner);
}

//______________________________________________________________________________
TIter::TIter(const TIter &iter)
{
   // Copy a TIter. This involves allocating a new TIterator of the right
   // sub class and assigning it with the original.

   if (iter.fIterator) {
      fIterator = iter.GetCollection()->MakeIterator();
      fIterator->operator=(*iter.fIterator);
   } else
      fIterator = 0;
}

//______________________________________________________________________________
TIter &TIter::operator=(const TIter &rhs)
{
   // Assigning an TIter to another. This involves allocatiing a new TIterator
   // of the right sub class and assigning it with the original.

   if (this != &rhs) {
      if (rhs.fIterator) {
         delete fIterator;
         fIterator = rhs.GetCollection()->MakeIterator();
         fIterator->operator=(*rhs.fIterator);
      }
   }
   return *this;
}
