#ifndef HPR_FHMARKER
#define HPR_FHMARKER
#include "TMarker.h"
#include "TObjArray.h"
#include <iostream>

class FhMarkerList : public TObjArray {
protected:
public:
   FhMarkerList(Int_t s = TCollection::kInitCapacity, Int_t lowerBound = 0) 
      : TObjArray(s, lowerBound){SetName("FhMarkerList");};

   ~FhMarkerList() {};
//   const char * GetName(){return fName;};
   void SavePrimitive(ostream & out, Option_t *)
      { out << "   TList * FhMarkerList = new TList();" << std::endl;
      }
ClassDef(FhMarkerList, 0)
};

class FhMarker : public TMarker {

public:
   FhMarker() {};
   FhMarker(Float_t x, Float_t y = 0, Int_t type = 0) : TMarker(x, y, type){};

   ~FhMarker() {};

//   Bool_t IsEqual(const TObject *obj) const; 
//                 {return GetX() == ((FhMarker*)obj)->GetX();};

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(const TObject *obj) const;
/*
{ if (GetX() == ((FhMarker*)obj)->GetX())
                                      return 0;
                                  else if  (GetX() < (( FhMarker*)obj)->GetX()) 
                                      return -1;
                                  else
                                      return +1;
                                 };
*/
ClassDef(FhMarker, 0)
};
#endif
