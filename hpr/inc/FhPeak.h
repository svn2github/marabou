#ifndef HPR_FHPEAK
#define HPR_FHPEAK
#include <iostream>
#include <iomanip>
#include "TObject.h"
#include "TList.h"
//#include "FhRegion.h"

using namespace std;


class FhRegion;

class FhPeak : public TObject {

private:
   Float_t fNominalEnergy;
   Float_t fCalibratedEnergy;
   Float_t fEstimate;
   Float_t fContent;
   Float_t fMean;
   Float_t fWidth;
   Float_t fTailContent;
   Float_t fTailWidth;
   Bool_t  fUsed;
   FhRegion *fRegion;    //!dont stream
public:
   FhPeak(){};
   FhPeak(Float_t pos);

   ~FhPeak() {};
   Float_t GetNominalEnergy() const    {return fNominalEnergy;};
   Float_t GetCalibratedEnergy() const {return fCalibratedEnergy;};
   Float_t GetContent()                {return fContent;};
   Float_t GetMean() const             {return fMean;};
   Float_t GetWidth() const            {return fWidth;};
   Float_t GetTailContent() const      {return fTailContent;};
   Bool_t  GetUsed() const             {return fUsed;};


   void SetNominalEnergy(Float_t val)   {fNominalEnergy=val;};
   void SetCalibratedEnergy(Float_t val){fCalibratedEnergy=val;};
   void SetContent(Float_t val)         {fContent=val;};
   void SetMean(Float_t val)            {fMean=val;};
   void SetWidth(Float_t val)           {fWidth=val;};
   void SetTailContent(Float_t val)     {fTailContent=val;};
   void SetUsed(Bool_t  used)           {fUsed = used;};

   Bool_t IsEqual(TObject *obj) 
                 {return GetMean() == ((FhPeak*)obj)->GetMean();};

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(const TObject *obj)const;
   void Print();
   void PrintHeadLine();
ClassDef(FhPeak,1)
};

#endif
