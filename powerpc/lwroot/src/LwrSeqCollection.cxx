//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrSeqCollection.cxx
//! \brief			Light Weight ROOT: TSeqCollection
//! \details		Class definitions for ROOT under LynxOs: TSeqCollection
//! 				Sequenceable collection abstract base class.<br>
//! 				TSeqCollections have an ordering relation,
//! 				i.e. there is a first and last element.
//!
//! \author Otto.Schaile
//!
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.7 $     
//! $Date: 2009-09-10 13:20:10 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrSeqCollection.h"
#include "LwrCollection.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns index of object in collection.<br>
//! 				Returns -1 when object not found.
//! 				Uses member IsEqual() to find object.
//! \param[in] 		Obj		-- object to be searched for
//! \retval 		Idx		-- index in collection
/////////////////////////////////////////////////////////////////////////////

Int_t TSeqCollection::IndexOf(const TObject * Obj) const
{

   Int_t   idx = 0;
   TIter   next(this);
   TObject *ob;

   while ((ob = next())) {
      if (ob->IsEqual(Obj)) return idx;
      idx++;
   }
   return -1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Compares two objects in the collection.<br>
//! 				Uses member function Compare() of object A.
//! \param[in] 		ObjA		-- object A to be compared with
//! \param[in] 		ObjB		-- object B
//! \retval 		Relation 	-- relation given by Compare()
/////////////////////////////////////////////////////////////////////////////

Int_t TSeqCollection::ObjCompare(TObject * ObjA, TObject * ObjB)
{

   if (ObjA == 0 && ObjB == 0) return 0;
   if (ObjA == 0) return 1;
   if (ObjB == 0) return -1;
   return ObjA->Compare(ObjB);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sort array of TObject pointers using a quicksort algorithm.<br>
//! 				Uses ObjCompare() to compare objects.
//! \param[in] 		ObjArr		-- array of objects
//! \param[in]		First		-- first index
//! \param[in]		Last		-- last index
/////////////////////////////////////////////////////////////////////////////

void TSeqCollection::QSort(TObject ** ObjArr, Int_t First, Int_t Last)
{

   static TObject *tmp;
   static int i;           // "static" to save stack space
   int j;

   while (Last - First > 1) {
      i = First;
      j = Last;
      for (;;) {
         while (++i < Last && ObjCompare(ObjArr[i], ObjArr[First]) < 0);
         while (--j > First && ObjCompare(ObjArr[j], ObjArr[First]) > 0);
         if (i >= j) break;

         tmp  = ObjArr[i];
         ObjArr[i] = ObjArr[j];
         ObjArr[j] = tmp;
      }
      if (j == First) {
         ++First;
         continue;
      }
      tmp = ObjArr[First];
      ObjArr[First] = ObjArr[j];
      ObjArr[j] = tmp;
      if (j - First < Last - (j + 1)) {
         this->QSort(ObjArr, First, j);
         First = j + 1;
      } else {
         this->QSort(ObjArr, j + 1, Last);
         Last = j;
      }
   }
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sorts array ObjArrA of TObject pointers using a quicksort algorithm.<br>
//! 				Array ObjArrB will be sorted just like ObjArrA
//! 				(i.e. ObjArrA determines the sort).<br>
//! 				Uses ObjCompare() to compare objects.
//! \param[in] 		ObjArrA		-- array of objects A
//! \param[in] 		ObjArrB		-- array of objects B
//! \param[in]		First		-- first index
//! \param[in]		Last		-- last index
/////////////////////////////////////////////////////////////////////////////

void TSeqCollection::QSort(TObject ** ObjArrA, TObject ** ObjArrB, Int_t First, Int_t Last)
{

   static TObject *tmp1, *tmp2;
   static int i;           // "static" to save stack space
   int j;

   while (Last - First > 1) {
      i = First;
      j = Last;
      for (;;) {
         while (++i < Last && ObjCompare(ObjArrA[i], ObjArrA[First]) < 0);
         while (--j > First && ObjCompare(ObjArrA[j], ObjArrA[First]) > 0);
         if (i >= j) break;

         tmp1 = ObjArrA[i]; tmp2 = ObjArrB[i];
         ObjArrA[i] = ObjArrA[j]; ObjArrB[i] = ObjArrB[j];
         ObjArrA[j] = tmp1; ObjArrB[j] = tmp2;
      }
      if (j == First) {
         ++First;
         continue;
      }
      tmp1 = ObjArrA[First]; tmp2 = ObjArrB[First];
      ObjArrA[First] = ObjArrA[j]; ObjArrB[First] = ObjArrB[j];
      ObjArrA[j] = tmp1;     ObjArrB[j] = tmp2;
      if (j - First < Last - (j + 1)) {
         this->QSort(ObjArrA, ObjArrB, First, j);
         First = j + 1;
      } else {
         this->QSort(ObjArrA, ObjArrB, j + 1, Last);
         Last = j;
      }
   }
}
