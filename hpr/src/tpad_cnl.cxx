#include "Riostream.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TPad.h"
#include "TStyle.h"

#include "TArrow.h"
#include "TCurlyArc.h"
//______________________________________________________________________________
void TPad::CreateNewLine(Int_t event, Int_t px, Int_t py, Int_t mode)
{
//*-*-*-*-*-*-*-*-*-*-*Create a new arrow in this pad*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ==============================
//
//  Function called by TPad::ExecuteEvent when the action "Create NewArrow"
//  has been selected in the Canvas pulldown menu.
//
//  Click left button to indicate arrow starting position.
//  Release left button to terminate the arrow.
//
//

   static Double_t x0, y0, x1, y1;

   static Int_t pxold, pyold;
   static Int_t px0, py0;
   static Int_t linedrawn;
   TLine *line;
   TArrow *arrow;
   TCurlyLine *cline;
   Double_t radius, phimin,phimax;

   switch (event) {

   case kButton1Down:
      gVirtualX->SetLineColor(-1);
      x0 = gPad->AbsPixeltoX(px);
      y0 = gPad->AbsPixeltoY(py);
      px0   = px; py0   = py;
      pxold = px; pyold = py;
      linedrawn = 0;
      break;

   case kButton1Motion:
      if (linedrawn) gVirtualX->DrawLine(px0, py0, pxold, pyold);
      pxold = px;
      pyold = py;
      linedrawn = 1;
      gVirtualX->DrawLine(px0, py0, pxold, pyold);
      break;

   case kButton1Up:
      if (px == px0 && py == py0) break;
      x1 = gPad->AbsPixeltoX(px);
      y1 = gPad->AbsPixeltoY(py);
      gPad->Modified(kTRUE);

      if (mode == kLine) {
         line = new TLine(x0,y0,x1,y1);
         line->Draw();
//         line->SetLineColor(gStyle->GetLineColor());
//         line->SetLineWidth(gStyle->GetLineWidth());
 //        line->SetLineStyle(gStyle->GetLineStyle());
         gPad->GetCanvas()->Selected((TPad*)gPad, line, event);
      }
      if (mode == kArrow) {
         TString aopt  = gEnv->GetValue("HistPresent.ArrowOption", ">");
         Float_t aang  = gEnv->GetValue("HistPresent.ArrowAngle", 60.);
         Float_t asiz  = gEnv->GetValue("HistPresent.ArrowSize", 0.05);
         Int_t   afill = gEnv->GetValue("HistPresent.ArrowFill", 1);
         Int_t   acol  = gEnv->GetValue("HistPresent.ArrowColor", 1);
         Int_t   awid  = gEnv->GetValue("HistPresent.ArrowWidth", 1);
         Int_t   asty  = gEnv->GetValue("HistPresent.ArrowStyle", 1);
         arrow = new TArrow(x0,y0,x1,y1,asiz,aopt.Data());
         arrow->SetAngle(aang);
         arrow->SetFillStyle(afill);
         arrow->Draw();
         arrow->SetLineColor(acol);
         arrow->SetFillColor(acol);
         arrow->SetLineWidth(awid);
         arrow->SetLineStyle(asty);
         gPad->GetCanvas()->Selected((TPad*)gPad, arrow, event);
      }
      if (mode == kCurlyLine || mode == kCurlyArc) {
         Double_t rd      = gEnv->GetValue("HistPresent.CurlyAmplitude", 0.025);
         Double_t wl      = gEnv->GetValue("HistPresent.CurlyWaveLength",0.01);
         Int_t    acol    = gEnv->GetValue("HistPresent.CurlyColor", 1);
         Int_t    awid  = gEnv->GetValue("HistPresent.CurlyWidth", 1);
         Int_t    asty  = gEnv->GetValue("HistPresent.CurlyStyle", 1);
         Int_t    iscurly = gEnv->GetValue("HistPresent.IsCurly",0);
         if (mode == kCurlyLine) {
            cline = new TCurlyLine(x0,y0,x1,y1, wl, rd);
         } else {
            radius = TMath::Sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
            phimin = 0;
            phimax = 360;
            cline = new TCurlyArc(x0,y0,radius,phimin,phimax, wl, rd);
         }
         if (iscurly) cline->SetCurly();
         else         cline->SetWavy();
         cline->SetLineColor(acol);
         cline->SetLineWidth(awid);
         cline->SetLineStyle(asty);
         cline->SetWaveLength(wl);
         cline->SetAmplitude(rd);
         cline->Draw();
         gPad->GetCanvas()->Selected((TPad*)gPad, cline, event);
      }
      gROOT->SetEditorMode();
      break;
   }
}
