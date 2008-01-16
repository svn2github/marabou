#include "TROOT.h"
#include "Riostream.h"
#include "THprSplineX.h"
#include "HTCanvas.h"

ClassImp(THprSplineX)
//________________________________________________________________________

THprSplineX::THprSplineX(Int_t npoints, Double_t *x, Double_t *y,
         Float_t *sf, Float_t prec, Bool_t closed)
           : TSplineX(npoints, x, y, sf, prec, closed)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprSplineX::Paint(const Option_t *opt)
{
   HTCanvas * htc = dynamic_cast<HTCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      Double_t x, y;
      Float_t sf;
      for (Int_t ip = 0; ip < GetNofControlPoints(); ip++) {
         GetControlPoint(ip, &x, &y, &sf);
         SetControlPoint(ip, htc->PutOnGridX(x), htc->PutOnGridY(y),sf);
      }
      NeedReCompute();
//      cout << "THprSplineX htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TSplineX::Paint(opt);
}
