#include "TROOT.h"
#include "Riostream.h"
#include "THprLatex.h"
#include "HTCanvas.h"

ClassImp(THprLatex)
//________________________________________________________________________

THprLatex::THprLatex(Double_t x, Double_t y, const Char_t * text)
           : TLatex(x, y, text)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprLatex::Paint(const Option_t *opt)
{
   HTCanvas * htc = dynamic_cast<HTCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX(htc->PutOnGridX(GetX()));
      SetY(htc->PutOnGridY(GetY()));
   }
   if (GetVisibility() == 1)
      TLatex::Paint(opt);
}
