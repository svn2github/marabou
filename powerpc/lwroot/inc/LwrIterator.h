#ifndef __LwrIterator_h__
#define __LwrIterator_h__


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TIterator                                                            //
//                                                                      //
// Iterator abstract base class. This base class provides the interface //
// for collection iterators.                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter, Apr 2008                                                  //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"

class TCollection;
class TObject;

class TIterator {

protected:
   TIterator() { }
   TIterator(const TIterator &) { }

public:
   virtual TIterator &operator=(const TIterator &) { return *this; }
   virtual ~TIterator() { }
   virtual const TCollection *GetCollection() const = 0;
   virtual Option_t *GetOption() const { return ""; }
   virtual TObject *Next() = 0;
   virtual void Reset() = 0;
   TObject *operator()() { return Next(); }
};

#endif
