#include "FhMarker.h"

ClassImp(FhMarkerList)
ClassImp(FhMarker)

Bool_t FhMarker::IsEqual(const TObject *obj) const 
              {return GetX() == ((FhMarker*)obj)->GetX();};

Int_t FhMarker::Compare(const TObject *obj) const { if (GetX() == ((FhMarker*)obj)->GetX())
                                   return 0;
                               else if  (GetX() < (( FhMarker*)obj)->GetX()) 
                                   return -1;
                               else
                                   return +1;
                              };
