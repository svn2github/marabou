#include "TROOT.h"
#include "THistPainter.h"
#include "TH2.h"
#include "TF2.h"
#include "TF3.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TFrame.h"
#include "Hoption.h"
#include "Hparam.h"
#include "TStyle.h"
#include <iostream>

namespace std {} using namespace std;

Hoption_t Hoption;
Hparam_t  Hparam;

//______________________________________________________________________________

void THistPainter::PaintColorLevels(Option_t *)
{
//    *-*-*-*-*-*Control function to draw a table as a color plot*-*-*-*-*-*
//               ================================================
//
//       For each cell (i,j) a box is drawn with a color proportional
//       to the cell content.
//       The color table used is defined in the current style (gStyle).
//       The color palette in TStyle can be modified via TStyle::SeTPaletteAxis.
//
//    *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   Double_t z, zc, xk, xstep, yk, ystep, xlow, xup, ylow, yup;

   Double_t zmin = fH->GetMinimum();
   Double_t zmax = fH->GetMaximum();
   if (Hoption.Logz) {
      if (zmin > 0) {
         zmin = TMath::Log10(zmin);
         zmax = TMath::Log10(zmax);
      } else { 
         return;
      }
   }

   Double_t dz = zmax - zmin;
   if (dz <= 0) return;

   Style_t fillsav   = fH->GetFillStyle();
   Style_t colsav    = fH->GetFillColor();
   fH->SetFillStyle(1001);
   fH->TAttFill::Modify();

//                  Initialize the levels on the Z axis
   Int_t ncolors  = gStyle->GetNumberOfColors();
   Int_t ndiv   = fH->GetContour();
   if (ndiv == 0 ) {
      ndiv = gStyle->GetNumberContours();
      fH->SetContour(ndiv);
   }
   Int_t ndivz  = TMath::Abs(ndiv);
   TArrayD ucl(ndiv);                           // << Otto
   if (fH->TestBit(TH1::kUserContour) == 0) {
      fH->SetContour(ndiv);
   } else {
//      ucl.Set(ndiv);
      for (Int_t k = 0; k < ndiv; k++) { 
        ucl[k] = fH->GetContourLevelPad(k);   
//       ucl[k] = fH->GetContourLevel(k);        // << Otto
//       if (Hoption.Logz) {
//           if (ucl[k] > 0) ucl[k] = TMath::Log10(ucl[k]); // << Otto
//           else            ucl[k] = zmin;                 // << Otto
//         }

//       cout << "ucl " << ucl[k] << endl;
      }
   }
   Double_t scale = ndivz/dz;

   Int_t color;
   for (Int_t j=Hparam.yfirst; j<=Hparam.ylast;j++) {
      yk    = fYaxis->GetBinLowEdge(j);
      ystep = fYaxis->GetBinWidth(j);
//      if (!(j%10)) cout << "chy: " << j << endl;
      for (Int_t i=Hparam.xfirst; i<=Hparam.xlast;i++) {
         Int_t bin  = j*(fXaxis->GetNbins()+2) + i;
         xk    = fXaxis->GetBinLowEdge(i);
         xstep = fXaxis->GetBinWidth(i);
         if (!IsInside(xk+0.5*xstep,yk+0.5*ystep)) continue;
         z     = fH->GetBinContent(bin);
         if (z == 0) continue; // do not draw the empty bins
         if (Hoption.Logz) {
            if (z > 0) z = TMath::Log10(z);
            else       z = zmin;
         }
         if (z < zmin) continue;
         xup  = xk + xstep;
         xlow = xk;
         if (Hoption.Logx) {
            if (xup > 0)  xup  = TMath::Log10(xup);
            else continue;
            if (xlow > 0) xlow = TMath::Log10(xlow);
            else continue;
         }
         yup  = yk + ystep;
         ylow = yk;
         if (Hoption.Logy) {
            if (yup > 0)  yup  = TMath::Log10(yup);
            else continue;
            if (ylow > 0) ylow = TMath::Log10(ylow);
            else continue;
         }
         if (xlow < gPad->GetUxmin()) xlow = gPad->GetUxmin();
         if (ylow < gPad->GetUymin()) ylow = gPad->GetUymin();
         if (xup  > gPad->GetUxmax()) xup  = gPad->GetUxmax();
         if (yup  > gPad->GetUymax()) yup  = gPad->GetUymax();

         if (fH->TestBit(TH1::kUserContour)) {
            zc = ucl[0];
//            zc = fH->GetContourLevelPad(0);
            if (z < zc) continue;
            color = -1;
//            cout << "GetC: z:  " << z<< endl;
            for (Int_t k=0; k<ndiv; k++) {
//               cout << "GetC: " << k << endl;
               zc = ucl[k];
//               cout << "GetC: zc, color:  " << zc << " " << color << endl;
//               zc = fH->GetContourLevelPad(k);
	            if (z < zc) {
                  continue;
               } else {
                  color++;
               }
            }
         } else {
            color = Int_t(0.01+(z-zmin)*scale);
         }

         Int_t theColor = Int_t((color+0.99)*Float_t(ncolors)/Float_t(ndivz));
//         if (z >= zmax) theColor = ncolors-1;
         if (theColor > ncolors-1) theColor = ncolors-1;
         fH->SetFillColor(gStyle->GetColorPalette(theColor));
         fH->TAttFill::Modify();
         gPad->PaintBox(xlow, ylow, xup, yup);
      }
   }

   if (Hoption.Zscale) PaintPalette();

   fH->SetFillStyle(fillsav);
   fH->SetFillColor(colsav);
   fH->TAttFill::Modify();

}
//________________________________________________________________________

Double_t TH1::GetContourLevelPad(Int_t level) const
{
// Return the value of contour number "level" in Pad coordinates ie: if the Pad
// is in log scale along Z it returns le log of the contour level value.  
// see GetContour to return the array of all contour levels

  if (level <0 || level >= fContour.fN) return 0;
  Double_t zlevel = fContour.fArray[level];

  // In case of user defined contours and Pad in log scale along Z,
  // fContour.fArray doesn't contain the log of the contour whereas it does
  // in case of equidistant contours.
  if (gPad && gPad->GetLogz() && TestBit(kUserContour)) {
     if (zlevel <= 0) return 0;
     zlevel = TMath::Log10(zlevel);
  }
  return zlevel;
}
