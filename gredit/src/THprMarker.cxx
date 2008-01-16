#include "TROOT.h"
#include "Riostream.h"
#include "THprMarker.h"
#include "HTCanvas.h"

ClassImp(THprMarker)
//________________________________________________________________________

THprMarker::THprMarker(Double_t x, Double_t y, Style_t style)
           : TMarker(x, y, style)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprMarker::Paint(const Option_t *opt)
{
   HTCanvas * htc = dynamic_cast<HTCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX(htc->PutOnGridX(GetX()));
      SetY(htc->PutOnGridY(GetY()));
   }
   if (GetVisibility() == 1)
      TMarker::Paint(opt);
}
