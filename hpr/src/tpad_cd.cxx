#include <iostream>
#include "TROOT.h"
//*KEEP,TSystem.
#include "TSystem.h"
//*KEEP,TCanvas.
#include "TCanvas.h"
#include "TVirtualX.h"
void TPad::cd(Int_t subpadnumber)
{
//*-*-*-*-*-*-*-*-*Set Current Pad*-*-*-*-*-**-*-*-*-*-*-*-*
//*-*              ====================
//  When a canvas/pad is divided via TPad::Divide, one can directly
//  set the current path to one of the subdivisions.
//  See TPad::Divide for the convention to number subpads.
//  For example:
//    c1.Divide(2,3); // create 6 pads (2 divisions along x, 3 along y).
//    To set the current pad to the bottom right pad, do
//    c1.cd(6);
//  Note1:  c1.cd() is equivalent to c1.cd(0) and sets the current pad
//          to c1 itself.
//  Note2:  after a statement like c1.cd(6), the global variable gPad
//          points to the current pad. One can use gPad to set attributes
//          of the current pad.

   if (!subpadnumber) {
      gPad = this;
      if (!gPad->IsBatch()){
         gVirtualX->SelectWindow(fPixmapID);
//         cout << "SelectWindow(fPixmapID)\n";
      }
      return;
   }

   TObject *obj;
   TIter    next(GetListOfPrimitives());
   while ((obj = next())) {
      if (obj->InheritsFrom(TPad::Class())) {
         Int_t n = ((TPad*)obj)->GetNumber();
         if (n == subpadnumber) {
            ((TPad*)obj)->cd();
            return;
         }
      }
   }
}

