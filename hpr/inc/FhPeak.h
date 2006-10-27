#ifndef HPR_FHPEAK
#define HPR_FHPEAK
#include <iostream>
#include <iomanip>
#include "TObject.h"
#include "TList.h"
//#include "FhRegion.h"

namespace std {} using namespace std;


class FhRegion;

class FhPeak : public TObject {

private:
   Double_t fNominalEnergy;
   Double_t fCalibratedEnergy;
   Double_t fEstimate;
   Double_t fContent;
   Double_t fMean;
   Double_t fWidth;
   Double_t fTailContent;
   Double_t fTailWidth;
   Bool_t  fUsed;
   FhRegion *fRegion;    //!dont stream
public:
   FhPeak(){};
   FhPeak(Double_t pos);

   ~FhPeak() {};
   Double_t GetNominalEnergy() const    {return fNominalEnergy;};
   Double_t GetCalibratedEnergy() const {return fCalibratedEnergy;};
   Double_t GetContent()                {return fContent;};
   Double_t GetMean() const             {return fMean;};
   Double_t GetWidth() const            {return fWidth;};
   Double_t GetTailContent() const      {return fTailContent;};
   Bool_t  GetUsed() const             {return fUsed;};


   void SetNominalEnergy(Double_t val)   {fNominalEnergy=val;};
   void SetCalibratedEnergy(Double_t val){fCalibratedEnergy=val;};
   void SetContent(Double_t val)         {fContent=val;};
   void SetMean(Double_t val)            {fMean=val;};
   void SetWidth(Double_t val)           {fWidth=val;};
   void SetTailContent(Double_t val)     {fTailContent=val;};
   void SetUsed(Bool_t  used)           {fUsed = used;};

   Bool_t IsEqual(const TObject *obj) const {
      return GetMean() == ((FhPeak*)obj)->GetMean();
   };

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(const TObject *obj) const;
   void Print(Option_t * Option) const { TObject::Print(Option); }
   void Print();
   void PrintHeadLine();
ClassDef(FhPeak,1)
};

#endif
