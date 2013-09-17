#ifndef __LwrIterator_h__
#define __LwrIterator_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrIterator.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TIterator<br>
//! 				Iterator abstract base class.<br>
//! 				This base class provides the interface for collection iterators.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-03 13:30:30 $
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
