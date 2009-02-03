#ifndef __LwrIterator_h__
#define __LwrIterator_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrIterator.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TIterator
//! 				Iterator abstract base class. This base class provides
//! 				the interface for collection iterators.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

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
   inline const Char_t * ClassName() const { return "TIterator"; };
};

#endif
