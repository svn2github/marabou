#ifndef HPR_FHMARKER
#define HPR_FHMARKER
#include "TMarker.h"

class FhMarker : public TMarker {

public:
   FhMarker(Float_t x, Float_t y, Int_t type) : TMarker(x, y, type){};

   ~FhMarker() {};

   Bool_t IsEqual(const TObject *obj) const 
                 {return GetX() == ((FhMarker*)obj)->GetX();};

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(const TObject *obj) const { if (GetX() == ((FhMarker*)obj)->GetX())
                                      return 0;
                                  else if  (GetX() < (( FhMarker*)obj)->GetX()) 
                                      return -1;
                                  else
                                      return +1;
                                 };
};
#endif
