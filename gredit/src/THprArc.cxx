#include "TROOT.h"
#include "Riostream.h"
#include "THprArc.h"
#include "HTCanvas.h"

ClassImp(THprArc)

//________________________________________________________________________

THprArc::THprArc(Double_t x1, Double_t y1, Double_t rad,
             Double_t phimin, Double_t phimax)
           : TArc(x1, y1, rad, phimin, phimax)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprArc::Paint(const Option_t *opt)
{
   HTCanvas * htc = dynamic_cast<HTCanvas*>(gPad);
   if (GetMustAlign() && htc && htc->GetUseEditGrid()) {
      SetR1(htc->PutOnGridX(GetR1()));
      SetX1(htc->PutOnGridX(GetX1()));
      SetY1(htc->PutOnGridY(GetY1()));
   }
   if (GetVisibility() == 1)
      TArc::Paint(opt);
}
