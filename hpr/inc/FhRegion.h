#ifndef HPR_FHREGION
#define HPR_FHREGION
#include "iostream.h"
#include "iomanip.h"
#include "TObject.h"
#include "TList.h"
#include "FhPeak.h"

const Int_t kMAXPEAK = 3;

class FhRegion : public TObject {

private:
   Float_t fLowEdge;
   Float_t fUpperEdge;
   Int_t   fNpeaks;
   TList * fPeaks;  //!dont stream

public:
   FhRegion(){};
   FhRegion(Float_t low, Float_t up);

   ~FhRegion(){};

   Int_t AddPeak(FhPeak *p){
      if(fNpeaks >= kMAXPEAK){
        cout << "FhRegion: MAXPEAK exceeded" << endl;
      } else {
         fPeaks->Add(p);
      }
      return fNpeaks;
   };
   Float_t GetLowEdge()    {return fLowEdge;};
   Float_t GetUpperEdge()  {return fUpperEdge;};
   void SetLowEdge(Float_t x)    {fLowEdge = x;};
   void SetUpperEdge(Float_t x)  {fUpperEdge = x;};

   Bool_t IsEqual(TObject *obj) 
                 {return GetLowEdge() == ((FhRegion*)obj)->GetLowEdge();};

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(TObject *obj) { 
      if       (GetLowEdge() == ((FhRegion*)obj)->GetLowEdge()) return  0;
      else if  (GetLowEdge() < (( FhRegion*)obj)->GetLowEdge()) return -1;
      else                                                      return +1;
   };
ClassDef(FhRegion,1)
};

#endif
