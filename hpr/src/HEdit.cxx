#include "TArc.h"
#include "TArrow.h"
#include "TArrayI.h"
#include "TDialogCanvas.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "TEllipse.h"
#include "TEnv.h"
#include "TF1.h"
#include "TGaxis.h"
#include "TRegexp.h"
#include "TImage.h"
#include "TAttImage.h"
#include "TContextMenu.h"

#include "HistPresent.h"
#include "support.h"
#include "HTCanvas.h"
#include "HandleMenus.h"

#include "TMrbString.h"
#include "TGMrbTableFrame.h"
#include "EditMarker.h"
#include "GroupOfGObjects.h"
#include "TGMrbInputDialog.h"
#include "TGMrbValuesAndText.h"
#include "TGMrbGetTextAlignment.h"
#include "HprImage.h"
#include "HprEditBits.h"
#include "TSplineX.h"
#include "TSplineXDialog.h"
#include "TCurlyLineArrow.h"
#include <fstream>

//______________________________________________________________________________
Bool_t SloppyInside(TCutG * cut, Double_t x, Double_t y)
{
//   cut->Dump();
   Double_t m = 0.1;
   if (cut->IsInside(x,y)) return kTRUE;
   if (cut->IsInside(x + m,y + m)) return kTRUE;
   if (cut->IsInside(x - m,y - m)) return kTRUE;
   if (cut->IsInside(x + m,y - m)) return kTRUE;
   if (cut->IsInside(x - m,y + m)) return kTRUE;
   return kFALSE;
}  
//______________________________________________________________________________

Double_t MinElement(Int_t n, Double_t * x){
   Double_t min = x[0];
   for (Int_t i = 1; i < n; i++) {
      if (x[i] < min) min = x[i];
   }
   return min;
}
//______________________________________________________________________________

Double_t MaxElement(Int_t n, Double_t * x){
   Double_t max = x[0];
   for (Int_t i = 1; i < n; i++) {
      if (x[i] > max) max = x[i];
   }
   return max;
}
//_______________________________________________________________________________________

void SetAllArrowSizes(TList *list, Double_t size,  Bool_t abs) 
{ 
   TObject * obj;
   TIter next(list);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)obj;
         Double_t s = size;
         if (!abs) s *=  b->GetArrowSize(); 
         b->SetArrowSize(s);
      }
   }
}
//_______________________________________________________________________________________

void SetAllCurlySizes(TList *list, Double_t wl, Double_t amp, Bool_t abs) 
{ 
   TObject * obj;
   TIter next(list);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj;
         Double_t w = wl;
         if (!abs) w *=  b->GetWaveLength();
         b->SetWaveLength(w);
         Double_t a = amp;
         if (!abs) a *=  b->GetAmplitude(); 
         b->SetAmplitude(a);
      }
   }
}
//_______________________________________________________________________________________

void SetAllTextSizes(TList *list, Double_t size, Bool_t abs) 
{ 
   TObject * obj;
   TIter next(list);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)obj;
         Double_t s = size;
         if (!abs) s *=  b->GetTextSize(); 
         b->SetTextSize(s);
      }
   }
}
//______________________________________________________________________________

void HTCanvas::InitEditCommands()
{
   Int_t win_width = 160;
   TList * labels = new TList;
   TList * methods = new TList;
   Bool_t has_xgrabsc = kFALSE;
   char * xgrabsc = gSystem->Which(gSystem->Getenv("PATH"), "xgrabsc");
   if (xgrabsc != NULL) {
      has_xgrabsc = kTRUE;
      delete xgrabsc;
   }
   if (has_xgrabsc) 
      labels->Add(new TObjString("Grab image from screen"));
   labels->Add(new TObjString("Insert image (gif, jpg"));
   labels->Add(new TObjString("Insert histogram "));
   labels->Add(new TObjString("Insert graph"));
   labels->Add(new TObjString("Insert Function"));
   labels->Add(new TObjString("Feynman diagram menu"));
   labels->Add(new TObjString("Text (Latex) from file"));
   labels->Add(new TObjString("Text (Latex) from keyboard"));
   labels->Add(new TObjString("Insert compound object"));
   labels->Add(new TObjString("Draw an axis"));
   labels->Add(new TObjString("Draw a TSplineX"));
   labels->Add(new TObjString("Draw CurlyLine with Arrow"));
   labels->Add(new TObjString("Define a box shaped region"));
   labels->Add(new TObjString("Define polygone region"));

   labels->Add(new TObjString("Mark as compound object"));
   labels->Add(new TObjString("Extract as compound obj"));
   labels->Add(new TObjString("Delete enclosed objects"));
   labels->Add(new TObjString("Modify objects by keyboard"));

   labels->Add(new TObjString("Zoom In"));
   labels->Add(new TObjString("Zoom Out"));
   labels->Add(new TObjString("UnZoom"));

   if (has_xgrabsc) 
      methods->Add(new TObjString("GrabImage()"));
   methods->Add(new TObjString("InsertImage()"));
   methods->Add(new TObjString("InsertHist()"));
   methods->Add(new TObjString("InsertGraph()"));
   methods->Add(new TObjString("InsertFunction()"));
   methods->Add(new TObjString("FeynmanDiagMenu()"));
   methods->Add(new TObjString("InsertTextF()"));
   methods->Add(new TObjString("InsertTextK()"));
   methods->Add(new TObjString("InsertGObjects()"));
   methods->Add(new TObjString("InsertAxis()"));
   methods->Add(new TObjString("InsertTSplineX()"));
   methods->Add(new TObjString("InsertCurlyArrow()"));
   methods->Add(new TObjString("DefineBox()"));
   methods->Add(new TObjString("DefinePolygone()"));
   methods->Add(new TObjString("MarkGObjects()"));
   methods->Add(new TObjString("ExtractGObjectsE()"));
   methods->Add(new TObjString("DeleteObjects()"));
   methods->Add(new TObjString("ModifyObjects()"));
   methods->Add(new TObjString("ZoomIn()"));
   methods->Add(new TObjString("ZoomOut()"));
   methods->Add(new TObjString("UnZoom()"));
   fEditCommands = 
   new HprEditCommands(fRootCanvas, win_width, this, this->ClassName(),
                       labels, methods);
}
//______________________________________________________________________________

void HTCanvas::ModifyObjects()
{
   Int_t win_width = 160;
   TList * labels = new TList;
   TList * methods = new TList;

   labels->Add(new TObjString("List all objects"));
   labels->Add(new TObjString("Modify Texts"));
   labels->Add(new TObjString("Modify Arrows"));
   labels->Add(new TObjString("Modify CurlyLines"));
   labels->Add(new TObjString("Modify CurlyArcs"));
   labels->Add(new TObjString("Modify Arcs"));
   labels->Add(new TObjString("Modify Ellipses"));
   labels->Add(new TObjString("Modify Markers"));
   labels->Add(new TObjString("Modify Paves"));
   labels->Add(new TObjString("Modify Pads"));
   labels->Add(new TObjString("Modify Graphs"));

   methods->Add(new TObjString("ListAllObjects()"));
   methods->Add(new TObjString("ModifyTexts()"));
   methods->Add(new TObjString("ModifyArrows()"));
   methods->Add(new TObjString("ModifyCurlyLines()"));
   methods->Add(new TObjString("ModifyCurlyArcs()"));
   methods->Add(new TObjString("ModifyArcs()"));
   methods->Add(new TObjString("ModifyEllipses()"));
   methods->Add(new TObjString("ModifyMarkers()"));
   methods->Add(new TObjString("ModifyPaves()"));
   methods->Add(new TObjString("ModifyPads()"));
   methods->Add(new TObjString("ModifyGraphs()"));

   fModifyCommands = 
   new HprEditCommands(fRootCanvas, win_width, this, this->ClassName(),
                       labels, methods);
 }
//______________________________________________________________________________

void HTCanvas::ListAllObjects()
{
   TOrdCollection * row_lab = new TOrdCollection;
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 2;
   TIter next(gPad->GetListOfPrimitives());
//	TObjString * os;
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->IsA() == EditMarker::Class()) continue;
      if (obj->TestBit(kCantEdit)) continue;
      nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No objects in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(2 * nrows);
   Double_t x = 0, y = 0;
   const Double_t far = 1000000;
   for (Int_t i = 0; i < nrows; i++) {
      flags[i] = 1; 
      flags[i + nrows] = 0; 
   }
   Int_t ind = 0;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == EditMarker::Class()) continue;
      if (obj->TestBit(kCantEdit)) continue;
      row_lab->Add(new TObjString(obj->ClassName()));
      if (obj->IsA() == TMarker::Class()) {
         TMarker * m = (TMarker*)obj;
         x = m->GetX();
         y = m->GetY();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TArc::Class()){
         TArc * m = (TArc*)obj;
         x = m->GetX1();
         y = m->GetY1();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TEllipse::Class()) {
         TEllipse * m = (TEllipse*)obj;
         x = m->GetX1();
         y = m->GetY1();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TArrow::Class()) {
         TArrow * m = (TArrow*)obj;
         x = m->GetX1();
         y = m->GetY1();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TCurlyLine::Class()) {
         TCurlyLine * m = (TCurlyLine*)obj;
         x = m->GetStartX();
         y = m->GetStartY();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TCurlyArc::Class()) {
         TCurlyArc * m = (TCurlyArc*)obj;
         x = m->GetStartX();
         y = m->GetStartY();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TLatex::Class()) {
         TLatex * m = (TLatex*)obj;
         x = m->GetX();
         y = m->GetY();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->InheritsFrom("TPave")) {
         TPave * m = (TPave*)obj;
         x = m->GetX1();
         y = m->GetY1();
         if (x > far) {x -= far; flags[ind] = 0;}

      } else if (obj->IsA() == TPad::Class()) {
         TPad * m = (TPad*)obj;
         x = m->GetAbsXlowNDC();
         y = m->GetAbsYlowNDC();
//         convert to user 
         x = x * (gPad->GetX2() - gPad->GetX1());
         y = y * (gPad->GetY2() - gPad->GetY1());
         if (x > far) {x -= far; flags[ind] = 0;}
      }

      values[ind + 0 * nrows] = x;
      values[ind + 1 * nrows] = y;
      ind++;
   }
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Visible"));
   col_lab->Add(new TObjString("Popup"));

   Int_t ret = -1;
   Int_t prec = 3;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "All objects", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, row_lab, &flags);
   if (ret < 0) return;
   ind = 0;
   Double_t x1, x2, y1, y2;
   Bool_t changed = kTRUE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == EditMarker::Class()) continue;
      if (obj->TestBit(kCantEdit)) continue;
      if (ind >= nrows)break;
      if (obj->IsA() == TMarker::Class()) {
         TMarker * m = (TMarker*)obj;
         x1 = m->GetX();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
         }
         m->SetX(x1);
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TArc::Class()) {
         TArc * m = (TArc*)obj;
         x1 = m->GetX1();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
         }
         m->SetX1(x1);
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TEllipse::Class()) {
         TEllipse * m = (TEllipse*)obj;
         x1 = m->GetX1();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
         }
         m->SetX1(x1);
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TArrow::Class()) {
         TArrow * m = (TArrow*)obj;
         x1 = m->GetX1();
         x2 = m->GetX2();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
            if (x2 < far) x2 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
            if (x2 > far) x2 -= far;    // move back
         }
         m->SetX1(x1);
         m->SetX2(x2);
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TCurlyLine::Class()) {
         TCurlyLine * m = (TCurlyLine*)obj;
         x1 = m->GetStartX();
         x2 = m->GetEndX();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
            if (x2 < far) x2 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
            if (x2 > far) x2 -= far;    // move back
         }
         m->SetStartPoint(x1, m->GetStartY());
         m->SetEndPoint  (x2, m->GetEndY());
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TCurlyArc::Class()) {
         TCurlyArc * m = (TCurlyArc*)obj;
         x1 = m->GetStartX();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
         }
         m->SetStartPoint(x1, m->GetStartY());
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TLatex::Class()) {
         TLatex * m = (TLatex*)obj;
         x1 = m->GetX();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
         }
         m->SetX(x1);
         if (flags[ind + nrows] != 0) 
            m->Pop();

      } else if (obj->IsA() == TPad::Class()) {
         TPad * m = (TPad*)obj;
         x1 = m->GetAbsXlowNDC();
         x2 = x1 + m->GetAbsWNDC();
         y1 = m->GetAbsYlowNDC();
         y2 = y1 + m->GetAbsHNDC();
         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
            if (x2 < far) x2 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
            if (x2 > far) x2 -= far;    // move back
         }
         m->SetPad(x1, y1, x2, y2);
         if (flags[ind + nrows] != 0) 
            m->Pop();
      } else if (obj->InheritsFrom("TPave")) {
         TPave * m = (TPave*)obj;
         x1 = m->GetX1();
         x2 = m->GetX2();
         x1 = (x1 - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());
         x2 = (x2 - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());

         if (flags[ind] == 0) { 
            if (x1 < far) x1 += far;    // move away
            if (x2 < far) x2 += far;    // move away
         } else {
            if (x1 > far) x1 -= far;    // move back
            if (x2 > far) x2 -= far;    // move back
         }
         m->SetX1NDC(x1);
         m->SetX2NDC(x2); 
         if (flags[ind + nrows] != 0) 
            m->Pop();
      }
      ind++;
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyTexts()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 4;
   TIter next(gPad->GetListOfPrimitives());
	TObjString * os;
   TObject * obj;
   TLatex * text;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TLatex")) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No text in active pad", fRootCanvas); 
      return;
   }
   TObjArray vt(ncols * nrows);
   TOrdCollection values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->InheritsFrom("TLatex")) {
         text = (TLatex*)obj;

         os = new TObjString(text->GetTitle());
         vt[ind + 0 * nrows] = (TObject*)os;

         TMrbString sx(text->GetX(), prec);
         os = new TObjString(sx.Data());
         vt[ind + 1 * nrows] = (TObject*)os;

         TMrbString sy(text->GetY(), prec);
         os = new TObjString(sy.Data());
         vt[ind + 2 * nrows] = (TObject*)os;

         TMrbString ss(text->GetTextSize(), prec);
         os = new TObjString(ss.Data());
         vt[ind + 3 * nrows] = (TObject*)os;
         ind++;
      }    
   }
   for (Int_t j = 0; j < nrows*ncols; j++) {
      values.Add(vt[j]);
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("Text"));
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Size"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableFrame(win, &ret, "Text", itemwidth,
                          ncols, nrows, &values, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   Double_t val;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->InheritsFrom("TLatex")) {
         if (flags[ind] >0) {
            text = (TLatex*)obj;
            os = (TObjString *) values.At(ind + 0 * nrows);
            text->SetTitle(os->GetString().Data());

            os = (TObjString *) values.At(ind + 1 * nrows);
            TMrbString sx(os->GetString().Data());
            sx.ToDouble(val);
            text->SetX(val);

            os = (TObjString *) values.At(ind + 2 * nrows);
            TMrbString sy(os->GetString().Data());
            sy.ToDouble(val);
            text->SetY(val);

            os = (TObjString *) values.At(ind +3 * nrows);
            TMrbString ss(os->GetString().Data());
            ss.ToDouble(val);
            text->SetTextSize(val);
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyCurlyLines()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 6;
   TIter next(gPad->GetListOfPrimitives());
//	TObjString * os;
   TObject * obj;
   TCurlyLine * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TCurlyLine::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No CurlyLine in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
//   TOrdCollection values(3 * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TCurlyLine::Class()) {
         cl = (TCurlyLine*)obj;
         
         values[ind + 0 * nrows] = cl->GetStartX();
         values[ind + 1 * nrows] = cl->GetStartY();
         values[ind + 2 * nrows] = cl->GetEndX();
         values[ind + 3 * nrows] = cl->GetEndY();
         values[ind + 4 * nrows] = cl->GetWaveLength();
         values[ind + 5 * nrows] = cl->GetAmplitude();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("StartX"));
   col_lab->Add(new TObjString("StartY"));
   col_lab->Add(new TObjString("EndX"));
   col_lab->Add(new TObjString("EndY"));
   col_lab->Add(new TObjString("WaveLength"));
   col_lab->Add(new TObjString("Amplitude"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "CurlyLine", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TCurlyLine::Class()) {
         if (flags[ind] >0) {
            cl = (TCurlyLine*)obj;
            cl->SetStartPoint(values[ind + 0 * nrows],values[ind + 1 * nrows]); 
            cl->SetEndPoint  (values[ind + 2 * nrows],values[ind + 3 * nrows]); 
            cl->SetWaveLength(values[ind + 4 * nrows]); 
            cl->SetAmplitude (values[ind + 5 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyCurlyArcs()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 7;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TCurlyArc * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TCurlyArc::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No CurlyArc in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TCurlyArc::Class()) {
         cl = (TCurlyArc*)obj;
         
         values[ind + 0 * nrows] = cl->GetStartX();
         values[ind + 1 * nrows] = cl->GetStartY();
         values[ind + 2 * nrows] = cl->GetRadius();
         values[ind + 3 * nrows] = cl->GetPhimin();
         values[ind + 4 * nrows] = cl->GetPhimax();
         values[ind + 5 * nrows] = cl->GetWaveLength();
         values[ind + 6 * nrows] = cl->GetAmplitude();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("Center X"));
   col_lab->Add(new TObjString("Center Y"));
   col_lab->Add(new TObjString("Radius"));
   col_lab->Add(new TObjString("Start Phi"));
   col_lab->Add(new TObjString("End Phi"));
   col_lab->Add(new TObjString("WaveLength"));
   col_lab->Add(new TObjString("Amplitude"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "CurlyArc", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TCurlyArc::Class()) {
         if (flags[ind] >0) {
            cl = (TCurlyArc*)obj;
            cl->SetCenter(values[ind + 0 * nrows],values[ind + 1 * nrows]); 
            cl->SetRadius  (values[ind + 2 * nrows]); 
            cl->SetPhimin  (values[ind + 3 * nrows]); 
            cl->SetPhimax  (values[ind + 4 * nrows]); 
            cl->SetWaveLength(values[ind + 5 * nrows]); 
            cl->SetAmplitude (values[ind + 6 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyArrows()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 6;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TArrow * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TArrow::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Arrow in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TArrow::Class()) {
         cl = (TArrow*)obj;
         
         values[ind + 0 * nrows] = cl->GetX1();
         values[ind + 1 * nrows] = cl->GetY1();
         values[ind + 2 * nrows] = cl->GetX2();
         values[ind + 3 * nrows] = cl->GetY2();
         values[ind + 4 * nrows] = cl->GetArrowSize();
         values[ind + 5 * nrows] = cl->GetAngle();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("X1"));
   col_lab->Add(new TObjString("Y1"));
   col_lab->Add(new TObjString("X2"));
   col_lab->Add(new TObjString("Y2"));
   col_lab->Add(new TObjString("Size"));
   col_lab->Add(new TObjString("Angle"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Arrow", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TArrow::Class()) {
         if (flags[ind] >0) {
            cl = (TArrow*)obj;
            cl->SetX1(values[ind + 0 * nrows]);
            cl->SetY1(values[ind + 1 * nrows]); 
            cl->SetX2(values[ind + 2 * nrows]); 
            cl->SetY2(values[ind + 3 * nrows]); 
            cl->SetArrowSize (values[ind + 4 * nrows]); 
            cl->SetAngle(values[ind + 5 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyArcs()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 5;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TArc * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TArc::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Arc in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TArc::Class()) {
         cl = (TArc*)obj;
         
         values[ind + 0 * nrows] = cl->GetX1();
         values[ind + 1 * nrows] = cl->GetY1();
         values[ind + 2 * nrows] = cl->GetR1();
         values[ind + 3 * nrows] = cl->GetPhimin();
         values[ind + 4 * nrows] = cl->GetPhimax();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("Center X"));
   col_lab->Add(new TObjString("Center Y"));
   col_lab->Add(new TObjString("Radius"));
   col_lab->Add(new TObjString("Start Phi"));
   col_lab->Add(new TObjString("End Phi"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Arc", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TArc::Class()) {
         if (flags[ind] >0) {
            cl = (TArc*)obj;
            cl->SetX1(values[ind + 0 * nrows]);
            cl->SetY1(values[ind + 1 * nrows]); 
            cl->SetR1  (values[ind + 2 * nrows]); 
            cl->SetPhimin  (values[ind + 3 * nrows]); 
            cl->SetPhimax  (values[ind + 4 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyEllipses()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 7;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TEllipse * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TEllipse::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Ellipse in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TEllipse::Class()) {
         cl = (TEllipse*)obj;
         
         values[ind + 0 * nrows] = cl->GetX1();
         values[ind + 1 * nrows] = cl->GetY1();
         values[ind + 2 * nrows] = cl->GetR1();
         values[ind + 3 * nrows] = cl->GetR2();
         values[ind + 4 * nrows] = cl->GetPhimin();
         values[ind + 5 * nrows] = cl->GetPhimax();
         values[ind + 6 * nrows] = cl->GetTheta();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("Center X"));
   col_lab->Add(new TObjString("Center Y"));
   col_lab->Add(new TObjString("R1"));
   col_lab->Add(new TObjString("R2"));
   col_lab->Add(new TObjString("Start Phi"));
   col_lab->Add(new TObjString("End Phi"));
   col_lab->Add(new TObjString("Theta"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Ellipse", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TEllipse::Class()) {
         if (flags[ind] >0) {
            cl = (TEllipse*)obj;
            cl->SetX1(values[ind + 0 * nrows]);
            cl->SetY1(values[ind + 1 * nrows]); 
            cl->SetR1  (values[ind + 2 * nrows]); 
            cl->SetR2  (values[ind + 3 * nrows]); 
            cl->SetPhimin  (values[ind + 4 * nrows]); 
            cl->SetPhimax  (values[ind + 5 * nrows]); 
            cl->SetTheta   (values[ind + 6 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyMarkers()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 3;
   TIter next(gPad->GetListOfPrimitives());
//	TObjString * os;
   TObject * obj;
   TMarker * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TMarker::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Marker in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TMarker::Class()) {
         cl = (TMarker*)obj;
         
         values[ind + 0 * nrows] = cl->GetX();
         values[ind + 1 * nrows] = cl->GetY();
         values[ind + 2 * nrows] = cl->GetMarkerSize();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Size"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Marker", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TMarker::Class()) {
         if (flags[ind] >0) {
            cl = (TMarker*)obj;
            cl->SetX(values[ind + 0 * nrows]);
            cl->SetY(values[ind + 1 * nrows]); 
            cl->SetMarkerSize (values[ind + 2 * nrows]); 
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyPads()
{
//   if (gPad != this) {
//      WarnBox("Cant modify subpads
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 4;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TPad * b;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TPad")) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Pad in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Double_t x1, y1, x2, y2;
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->InheritsFrom("TPad")) {
         b = (TPad*)obj;
         x1 = b->GetAbsXlowNDC();
         y1 = b->GetAbsYlowNDC();
         x2 = x1 + b->GetAbsWNDC();
         y2 = y1 + b->GetAbsHNDC();
//         convert to user 
         x1 = x1 * (gPad->GetX2() - gPad->GetX1());
         y1 = y1 * (gPad->GetY2() - gPad->GetY1());
         x2 = x2 * (gPad->GetX2() - gPad->GetX1());
         y2 = y2 * (gPad->GetY2() - gPad->GetY1());
         values[ind + 0 * nrows] = x1;
         values[ind + 1 * nrows] = y1;
         values[ind + 2 * nrows] = x2 - x1;
         values[ind + 3 * nrows] = y2 - y1;
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Width"));
   col_lab->Add(new TObjString("Height"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Pad", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->InheritsFrom("TPad")) {
         if (flags[ind] >0) {
            b = (TPad*)obj;
            x1 = (values[ind + 0 * nrows] - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());
            y1 = (values[ind + 1 * nrows] - gPad->GetY1()) / (gPad->GetY2() - gPad->GetY1());
            x2 = x1 + (values[ind + 2 * nrows] - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());
            y2 = y1 + (values[ind + 3 * nrows] - gPad->GetY1()) / (gPad->GetY2() - gPad->GetY1());
            b->SetPad(x1, y1, x2, y2);
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyPaves()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 4;
   TIter next(gPad->GetListOfPrimitives());
   TObject * obj;
   TPave * cl;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TPave")) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Pave in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->InheritsFrom("TPave")) {
         cl = (TPave*)obj;
         
         values[ind + 0 * nrows] = cl->GetX1();
         values[ind + 1 * nrows] = cl->GetY1();
         values[ind + 2 * nrows] = cl->GetX2() - cl->GetX1();
         values[ind + 3 * nrows] = cl->GetY2() - cl->GetY1();
         ind++;
      }    
   }
   for (Int_t i = 0; i < nrows; i++) flags[i] = 1; 
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Width"));
   col_lab->Add(new TObjString("Height"));
   col_lab->Add(new TObjString("Apply"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Pave", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags);
   if (ret < 0) return;
   ind = 0;
   Double_t x1;
   Double_t y1;
   Double_t x2;
   Double_t y2;
   Bool_t changed = kFALSE;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->InheritsFrom("TPave")) {
         if (flags[ind] >0) {
            TPave * b = (TPave*)obj;
            x1 = (values[ind + 0 * nrows] - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());
            y1 = (values[ind + 1 * nrows] - gPad->GetY1()) / (gPad->GetY2() - gPad->GetY1());
            x2 = x1 + (values[ind + 2 * nrows] - gPad->GetX1()) / (gPad->GetX2() - gPad->GetX1());
            y2 = y1 + (values[ind + 3 * nrows] - gPad->GetY1()) / (gPad->GetY2() - gPad->GetY1());
            b->SetX1NDC(x1);
            b->SetY1NDC(y1); 
            b->SetX2NDC(x2); 
            b->SetY2NDC(y2);
            changed = kTRUE;
         }
         ind++;
      }    
   }
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::ModifyGraphs()
{
   TOrdCollection * col_lab = new TOrdCollection;
   Int_t nrows = 0;
   Int_t ncols = 3;
   TIter next(gPad->GetListOfPrimitives());
//	TObjString * os;
   TObject * obj;
   TGraph * cl;
   while ( (obj = next()) ) {
      if (obj->IsA() == TGraph::Class()) nrows++;
   }
   if (nrows <= 0) {
      WarnBox("No Graph in active pad", fRootCanvas); 
      return;
   }
   TArrayD values(ncols * nrows);
   TArrayI flags(nrows);
   Int_t ind = 0;
   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->IsA() == TGraph::Class()) {
         cl = (TGraph*)obj;
         Double_t * x = cl->GetX();
         Double_t * y = cl->GetY();
         values[ind + 0 * nrows] = x[0];
         values[ind + 1 * nrows] = y[0];
         values[ind + 2 * nrows] = cl->GetN();
         ind++;
      }    
   }
   flags[0] = 1; 
   col_lab->Add(new TObjString("X of first point"));
   col_lab->Add(new TObjString("Y of first point"));
   col_lab->Add(new TObjString("Npoints"));
   col_lab->Add(new TObjString("Select"));
   Int_t ret = 0;
   Int_t itemwidth = 100;
   const TGWindow * win = (TGWindow*)fRootCanvas;
   new TGMrbTableOfDoubles(win, &ret, "Select Graph", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab, NULL, &flags, nrows);
   if (ret < 0) return;
   ind = 0;
   Bool_t changed = kFALSE;
   cl = NULL;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->IsA() == TGraph::Class()) {
         if (flags[ind] >0) {
            cl = (TGraph*)obj;
            break;
         }
         ind++;
      }    
   }
   if (cl == NULL) {
      cout << "No selection" << endl;
      return;
   }
   changed = kTRUE;
   col_lab->Clear();
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   nrows = cl->GetN();
   ncols = 2;
   values.Set(2 * nrows);
   Double_t * x = cl->GetX();
   Double_t * y = cl->GetY();
   for(Int_t i = 0; i < nrows; i++) {
      values[i] = x[i];
      values[i + nrows] = y[i];
   } 
   ret = 0;
   new TGMrbTableOfDoubles(win, &ret, "Edit Graph", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab);
   if (ret < 0) return;
   for(Int_t i = 0; i < nrows; i++) {
      x[i] = values[i];
      y[i] = values[i + nrows];
   }
   
   if (changed) {
      gPad->Modified();
      gPad->Update();
   }
}
//______________________________________________________________________________

void HTCanvas::DefineBox()
{
   TObject * obj = gPad->WaitPrimitive("TPave");
//   TObject * obj = gPad->GetListOfPrimitives()->Last();
   if (obj->IsA() == TPave::Class()) {
      TPave * p = (TPave*)obj;
      Double_t x[5];
      Double_t y[5];
      x[0] =  p->GetX1();
      x[1] =  p->GetX2();
      x[2] =  x[1];
      x[3] =  x[0];
      x[4] =  x[0];
      y[0] =  p->GetY1();
      y[1] =  y[0];
      y[2] =  p->GetY2();
      y[3] =  y[2];
      y[4] =  y[0];
      TObject * otcut = gPad->GetListOfPrimitives()->FindObject("CUTG");
      if (otcut) delete otcut;
      TCutG * cut = new TCutG("CUTG", 5, &x[0], &y[0]);
      delete obj;
      cut->Draw();
      cut->SetLineWidth(1);
      cut->SetLineStyle(2);
      cut->SetLineColor(1);
      Update();
   } else {
       cout << "Error getting TPave" << endl;
   }
}
//______________________________________________________________________________

void HTCanvas::DefinePolygone()
{
   TObject * obj = gPad->GetListOfPrimitives()->FindObject("CUTG");
   if (obj) delete obj;
   obj = gPad->WaitPrimitive("CUTG", "CutG");
//   obj = gPad->GetListOfPrimitives()->Last();
   if (obj->IsA() == TCutG::Class()) {
       ((TCutG*)obj)->SetLineWidth(1);
       ((TCutG*)obj)->SetLineStyle(2);
       ((TCutG*)obj)->SetLineColor(1);
        Update();
   } else {
       cout << "Error getting TCutG" << endl;
   }
}     
//______________________________________________________________________________

void HTCanvas::ZoomIn()
{
   if (GetWw() > 15000 || GetWh() > 15000) {
      cout << "No more zoom possible" << endl;
      return;
   }
   SetCanvasSize(GetWw() << 1, GetWh() << 1);
   Update();
}
//______________________________________________________________________________

void HTCanvas::ZoomOut()
{
   if (GetWw() > fOrigWw) {
      SetCanvasSize(GetWw() >> 1, GetWh() >> 1);
      Update();
   }
}
//______________________________________________________________________________

void HTCanvas::UnZoom()
{
      SetCanvasSize(fOrigWw, fOrigWh);
      Update();
}
//______________________________________________________________________________

Double_t HTCanvas::PutOnGridX(Double_t x)
{
   if (fEditGridX ==  0) return x;
   Int_t n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
   return (Double_t)n * fEditGridX;
}   
//______________________________________________________________________________

Double_t HTCanvas::PutOnGridY(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Int_t n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
   return (Double_t)n * fEditGridY;
}   
//______________________________________________________________________________
void  HTCanvas::SetUseEditGrid(Bool_t use) 
{
   fUseEditGrid = use;
   Int_t temp = 0;
   if (use) temp = 1;
   TEnv env(".rootrc");
   env.SetValue("HistPresent.UseEditGrid", temp);
   env.SaveLevel(kEnvUser);
};
//______________________________________________________________________________

void HTCanvas::SetEditGrid(Double_t x, Double_t y, Double_t xvis, Double_t yvis)
{
   fEditGridX = x; 
   fEditGridY = y;
   fVisibleGridX = xvis; 
   fVisibleGridY = yvis;
//   SetUseEditGrid(kTRUE);
   DrawEditGrid(kTRUE);
}
//______________________________________________________________________________

void HTCanvas::DrawEditGrid(Bool_t visible)
{
	Double_t dx, dy;
	if (visible) {
       dx = fVisibleGridX;
       dy = fVisibleGridY;
	} else {
       dx = fEditGridX;
       dy = fEditGridY;
	}
	if (dx <= 0 || dy <= 0) return;
   Double_t xl, yl, xh, yh;
   GetRange(xl, yl, xh, yh);

   Double_t x = xl;
   Double_t y;
   EditMarker * em;
   Int_t ix = 0, iy = 0;
   while (x <= xh) {
      y = yl;   
      while (y <= yh) {
         Int_t mstyle = 2;
         Double_t msize =0.4;
         if ( !(ix%5) && !(iy%5) )msize = 0.7;
         if ( !(ix%10) && !(iy%10) ){ 
            msize = 0.9;
            mstyle = 28;
         }
         em = new EditMarker(x, y, msize, mstyle);
         em->Draw();
         y += dy;
         iy += 1;
      }
      x += dx;
      ix += 1;
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::RemoveTSplineXsPolyLines()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) ((TSplineX*)obj)->RemovePolyLines();
   }
//   Update();
}
//______________________________________________________________________________

void HTCanvas::DrawTSplineXsParallelGraphs()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) {
         ((TSplineX*)obj)->NeedReCompute();
         ((TSplineX*)obj)->DrawParallelGraphs();
      }
   }
   Modified();
   Update();
}
//______________________________________________________________________________

void HTCanvas::DrawControlGraphs()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   this->cd();
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) ((TSplineX*)obj)->DrawControlPoints();
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::RemoveControlGraphs()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   this->cd();
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) ((TSplineX*)obj)->RemoveControlPoints();
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::RemoveParallelGraphs()
{
   TList * lop = GetListOfPrimitives();
   TObject * obj;
   while ( (obj = lop->FindObject("ParallelG")) ){
      lop->Remove(obj);
   }
//   Modified();
//   Update();
}
//______________________________________________________________________________

void HTCanvas::RemoveEditGrid()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->GetUniqueID() == 0xaffec0c0) delete obj;
   }
//   Update();
}
//______________________________________________________________________________

void HTCanvas::WritePrimitives()
{
   Bool_t ok;
   static Int_t sernr = 1;
//   TString name = "drawing";
   TString name = "p";
   name += sernr++;
   name =
       GetString("Save picture with name", name.Data(), &ok,
                 (TGWindow*)fRootCanvas);
   if (!ok)
      return;
   if (OpenWorkFile(fRootCanvas)) {
      RemoveEditGrid();
      RemoveControlGraphs();
      RemoveTSplineXsPolyLines();
      RemoveParallelGraphs();
      gPad->GetListOfPrimitives()->ls();
      Write(name.Data());
 //     GetListOfPrimitives()->Write(name.Data(), 1);
      CloseWorkFile();
      DrawTSplineXsParallelGraphs();
   }
}
///______________________________________________________________________________

void HTCanvas::SetVisibilityOfEnclosingCuts(Bool_t visible)
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->TestBit(kIsEnclosingCut))
        ((GroupOfGObjects*)obj)->SetVisible(visible);  
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::InsertHist()
{
   if (!fHistPresent) return;
   TPad* pad = GetEmptyPad();
   if (pad) {
     gROOT->SetSelectedPad(pad);
   } else {
      WarnBox("Please create a new Pad in this Canvas", fRootCanvas); 
      return;
   }   
   static void *valp[25];
   Int_t ind = 0;
//      if (GetListOfPrimitives()->Contains(selected)) {
   TList *row_lab = new TList(); 
   static TString fname;
   static TString gname;
   static TString drawopt;
   static Int_t select_from_list = 1;
   static Double_t scale = 1;

   ind = 0;
   row_lab->Add(new TObjString("StringValue_Name of ROOT file"));
   valp[ind++] = &fname;
   row_lab->Add(new TObjString("StringValue_Name of Histogram"));
   valp[ind++] = &gname;
   row_lab->Add(new TObjString("CheckButton_Select hist from Filelist"));
   valp[ind++] = &select_from_list;
   row_lab->Add(new TObjString("DoubleValue_Scale factor for labels titles etc."));
   valp[ind++] = &scale;
   row_lab->Add(new TObjString("StringValue_Drawing option"));
   valp[ind++] = &drawopt;
   
   Bool_t ok; 
   Int_t itemwidth = 320;

   ok = GetStringExt("Insert Hist Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   TH1* hist = 0;
   
   if (select_from_list > 0) {
      if (!fHistPresent) {
         cout << "No HistPresent" << endl;
         return;
      }
      if (fHistPresent->fSelectHist->GetSize() != 1) {
         WarnBox("Please select exactly 1 histogram", fRootCanvas); 
         return;
      } else {
         hist = fHistPresent->GetSelHistAt(0, NULL, kTRUE);
      }
   } else {
      if (gname.Length()) {
         if (fname.Length() > 0) {
            TFile * rfile = new TFile(fname.Data());
            if (!rfile->IsOpen()) {
               cout << "Cant open file: " << fname.Data() << endl;
               return;
            }
            hist = (TH1*)rfile->Get(gname.Data());
         } else {
            hist = (TH1F*)gROOT->FindObject(gname.Data());
         }
         if (!hist) {
            cout << "Cant find histogram: " << gname.Data() << endl;
            return;
         } else {
            hist->SetDirectory(gROOT);
         }
      } else {
         cout << "No Histogram defined" << endl;
      }      
   }
   if (!hist) return;
   TString hn = hist->GetName();
   if (hn.Index(";") > 0) { 
      hn.Resize(hn.Index(";"));
      hist->SetName(hn);
   }
   pad->cd();
   if (TMath::Abs(scale -1) > 0.0001) {
      TAxis * a = hist->GetXaxis();
      if (a) {
         a->SetLabelSize( scale * a->GetLabelSize());
         a->SetTickLength( scale * a->GetTickLength());
      }
      a = hist->GetYaxis();
      if (a) {
         a->SetLabelSize( scale * a->GetLabelSize());
         a->SetTickLength( scale * a->GetTickLength());
      }
   }
   hist->Draw(drawopt.Data());
   if (fHistPresent && drawopt.Length() == 0) {
      if (hist->GetDimension() == 1) {
         if (fHistPresent->fShowContour)
            drawopt = "";
         if (fHistPresent->fShowErrors)
            drawopt += "e1";
         if (fHistPresent->fFill1Dim) {
            hist->SetFillStyle(1001);
            hist->SetFillColor(fHistPresent->f1DimFillColor);
         } else
            hist->SetFillStyle(0);
      } else if (hist->GetDimension() == 2) {
         drawopt = fHistPresent->fDrawOpt2Dim->Data();
      }
      hist->SetOption(drawopt.Data());
      hist->SetDrawOption(drawopt.Data());
      pad->Modified();
   }
   Update();
}
//______________________________________________________________________________

TPad*  HTCanvas::GetEmptyPad()
{
   TIter next(this->GetListOfPrimitives());
   TObject * obj;
   TPad* pad  = NULL;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TPad")) {
         pad = (TPad*)obj;
         if (pad->GetListOfPrimitives()->GetSize() == 0) {
            pad->cd(0);
            gROOT->SetSelectedPad(pad);
            return pad;
         }
      }
   }
   return NULL;
}
//______________________________________________________________________________

void HTCanvas::InsertGraph()
{
   TPad* pad = GetEmptyPad();
   if (pad) {
     gROOT->SetSelectedPad(pad);
   } else {
      WarnBox("Please create a new Pad in this Canvas", fRootCanvas); 
      return;
   }   
//      if (GetListOfPrimitives()->Contains(selected)) {
   static void *valp[25];
   Int_t ind = 0;
   TList *row_lab = new TList(); 
   static TString fname;
   static TString gname;
   static TString goption;
   static Int_t select_from_list = 1;
   static Double_t scale = 1;
   if (fHistPresent) goption = fHistPresent->fDrawOptGraph;
   else goption = "AL";

   row_lab->Add(new TObjString("StringValue_Name of ROOT file"));
   valp[ind++] = &fname;
   row_lab->Add(new TObjString("StringValue_Name of TGraph"));
   valp[ind++] = &gname;
   row_lab->Add(new TObjString("CheckButton_Select graph from Filelist"));
   valp[ind++] = &select_from_list;
   row_lab->Add(new TObjString("DoubleValue_Scale factor for labels titles etc."));
   valp[ind++] = &scale;
   row_lab->Add(new TObjString("StringValue_Drawing option"));
   valp[ind++] = &goption;
    
   Bool_t ok; 
   Int_t itemwidth = 320;

   ok = GetStringExt("Insert Graph Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   TGraph* graph = 0;
   
   if (select_from_list > 0) {
      if (!fHistPresent) {
         cout << "No HistPresent" << endl;
         return;
      }
      if (fHistPresent->fSelectGraph->GetSize() != 1) {
         WarnBox("Please select exactly 1 graph", fRootCanvas); 
         return;
      } else {
         graph = fHistPresent->GetSelGraphAt(0);
      }
   } else {
      if (fname.Length() > 0 && gname.Length()) {
         TFile * rfile = new TFile(fname.Data());
         if (!rfile->IsOpen()) {
            cout << "Cant open file: " << fname.Data() << endl;
            return;
         }
         graph = (TGraph*)rfile->Get(gname.Data());
         if (!graph) {
            cout << "Cant find graph: " << gname.Data() << endl;
            return;
         }
      }
   }
   if (!graph) return;

//   graph->Print();
   pad->cd();
   if (TMath::Abs(scale -1) > 0.0001) {
      TAxis * a = graph->GetXaxis();
      if (a) {
         a->SetLabelSize( scale * a->GetLabelSize());
         a->SetTickLength( scale * a->GetTickLength());
      }
      a = graph->GetYaxis();
      if (a) {
         a->SetLabelSize( scale * a->GetLabelSize());
         a->SetTickLength( scale * a->GetTickLength());
      }
   }
   graph->Draw(goption.Data());
   pad->Modified();
   Update();
}

//______________________________________________________________________________

void HTCanvas::GrabImage()
{
   static void *valp[25];
   Int_t ind = 0;
   TList *row_lab = new TList(); 
   static Int_t fix_size = 1;
   static Short_t align    = 11;
   static Int_t delay    = 1;
   static Int_t xw = 100;
   static Int_t yw = 100;
   static Int_t sernr = 0;
   static Int_t query_ok = 1;
   static TString pname;
   pname = "pict_";
   pname += sernr;
   pname += ".gif";

   row_lab->Add(new TObjString("CheckButton_Fixed size"));
   valp[ind++] = &fix_size;
   row_lab->Add(new TObjString("AlignSelect_Alignment"));
   valp[ind++] = &align;
   row_lab->Add(new TObjString("PlainIntVal_Delay [sec]"));
   valp[ind++] = &delay;
   row_lab->Add(new TObjString("PlainIntVal_X Width"));
   valp[ind++] = &xw;
   row_lab->Add(new TObjString("PlainIntVal_Y Width"));
   valp[ind++] = &yw;
   row_lab->Add(new TObjString("StringValue_Picture name"));
   valp[ind++] = &sernr;
   row_lab->Add(new TObjString("CheckButton_Ask for ok after grab"));
   valp[ind++] = &query_ok;
 
   Int_t itemwidth = 320;
   Bool_t ok = kFALSE;

   ok = GetStringExt("Grab picture", NULL, itemwidth, fRootCanvas,
                   NULL, NULL, row_lab, valp);
   if (!ok) return;

 
   cout << "Mark pick area" << endl;

   TString cmd("xgrabsc -ppm -verbose 1> /dev/null 2> xgrabsc.log");
   gSystem->Exec(cmd.Data());

   Int_t xleg = 0, yleg = 0, xwg = 0, ywg = 0;
   ifstream str("xgrabsc.log");
   TString line;
   TString number;
   while (1) {
      line.ReadLine(str);
      if(str.eof()) break;
      if (!line.Contains("bounding box")) continue;
      Int_t ip;  
      ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
      number = line(0,ip);
      xleg = number.Atoi();

      ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
      number = line(0,ip);
      yleg = number.Atoi();
  
      ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
      number = line(0,ip);
      xwg = number.Atoi();

      ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index("]");
      number = line(0,ip);
      ywg = number.Atoi();
      break;
   }
//   str.close();
   if (xwg == 0) {
      cout << "Couldnt get coords" << endl;
      return;
   }
// alignment
   if (fix_size != 0) {
      if        (align%10 == 1) {
         yleg = yleg + ywg - yw;
      } else if (align%10 == 2) {
         yleg = yleg + (ywg - yw) / 2;
      }
      if        (align/10 == 3) {
         xleg = xleg + xwg - xw;
      } else if (align/10 == 2) {
         xleg = xleg + (xwg - xw) / 2;
      }
      xwg = xw;
      ywg = yw;
   }
   
   cmd = "xgrabsc -ppm -coords ";
   cmd += xwg; cmd += "x"; cmd += ywg; 
   cmd += "+";  cmd += xleg;  
   cmd += "+"; cmd += yleg; 
   cmd += " | convert - ";
   cmd += pname.Data();
   cmd += " &";

   cout << "Cmd: " << cmd << endl;
   if (delay > 0) {
      cout << "Waiting " << delay << " seconds before grab" << endl;
      Int_t wt = 10 * delay;
      while (wt-- >= 0) {
         gSystem->ProcessEvents();
         gSystem->Sleep(100);
      }
   }
   gSystem->Exec(cmd.Data());
   gSystem->ProcessEvents();

   if (query_ok != 0) {
      gSystem->Exec(cmd.Data());
      cmd = "kuickshow ";
      cmd += pname.Data();
      cmd += "&";
      gSystem->Exec(cmd.Data());
      Bool_t accept = QuestionBox("Is it ok?",fRootCanvas);
      cmd = "killall kuickshow";
      gSystem->Exec(cmd.Data());
      if (!accept) return;
   }
   cout << "Mark position where to put (lower left corner)" << endl; 
   TObject * obj = gPad->WaitPrimitive("TMarker");

   if (obj->IsA() == TMarker::Class()) {
      TMarker * ma = (TMarker*)obj;
      Double_t x1, y1, x2, y2, xr1, yr1, xr2, yr2;
      x1 = ma->GetX();
      y1 = ma->GetY();
      delete ma;
      x2 = x1 + AbsPixeltoX(xwg);
      y2 = y1 + AbsPixeltoY(ywg);

      gPad->GetRange(xr1, yr1, xr2, yr2);
      x1 = (x1 - xr1) / (xr2 - xr1);
      y1 = (y1 - yr1) / (yr2 - yr1);
      x2 = x1 + (Double_t) xwg / (Double_t)gPad->GetWw();
      y2 = y1 + (Double_t) ywg / (Double_t)gPad->GetWh();

      TPad * pad = new TPad(pname.Data(), "For HprImage", 
                            x1, y1, x2, y2); 
      pad->Draw(); 
      HprImage * hprimg = new HprImage(pname.Data(), pad);
      gROOT->SetSelectedPad(pad);
      pad->cd();
      hprimg->Draw("xxx");
      this->cd();
      Update();
   }
}
//______________________________________________________________________________

void HTCanvas::InsertImage()
{
   TPad* pad = 0;
   const char hist_file[] = {"images_hist.txt"};
   Bool_t ok = kFALSE;
   static TString name;
   Int_t itemwidth = 320;
   ofstream hfile(hist_file);
   const char *fname;
   void* dirp=gSystem->OpenDirectory(".");
   TRegexp dotGif = "\\.gif$";   
   TRegexp dotJpg = "\\.jpg$"; 
   TRegexp dotPng = "\\.png$"; 
   Long_t id, size, flags, modtime;
   while ( (fname=gSystem->GetDirEntry(dirp)) ) {
      TString sname(fname);
      if (!sname.BeginsWith("temp_") && 
          (sname.Index(dotGif)>0 || sname.Index(dotJpg)>0 
         || sname.Index(dotPng)>0)) {
         size = 0;
         gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
         if (size <= 0)
            cout << "Warning, empty file: " << fname << endl;
         else 
            hfile << fname << endl;
         if (name.Length() < 1) name = fname;
      }
   }
   TList *row_lab = new TList(); 
   static void *valp[25];
   Int_t ind = 0;
   static Int_t new_pad = 1;
   static Int_t fix_h = 0;
   static Int_t fix_w = 1;
   static Int_t fix_wh = 0;
   static Int_t offset_x = 0;
   static Int_t offset_y = 0;

   row_lab->Add(new TObjString("CheckButton_Use a new pad"));
   valp[ind++] = &new_pad;
   row_lab->Add(new TObjString("RadioButton_Preserve defined height"));
   valp[ind++] = &fix_h;
   row_lab->Add(new TObjString("RadioButton_Preserve defined width"));
   valp[ind++] = &fix_w;
   row_lab->Add(new TObjString("RadioButton_Preserve width and height"));
   valp[ind++] = &fix_wh;
   row_lab->Add(new TObjString("PlainIntVal_Offset X"));
   valp[ind++] = &offset_x;
   row_lab->Add(new TObjString("PlainIntVal_Offset Y"));
   valp[ind++] = &offset_y;

    ok = GetStringExt("Picture name", &name, itemwidth, fRootCanvas,
                   hist_file, NULL, row_lab, valp);
   if (!ok) return;

//      TImage *img = TImage::Open(name.Data());
   HprImage * hprimg = new HprImage(name.Data(), pad);
   TImage *img = hprimg->GetImage();
   if (!img) {
      cout << "Could not create an image... exit" << endl;
      return;
   }
   Double_t img_width = (Double_t )img->GetWidth();
   Double_t img_height = (Double_t )img->GetHeight();
  
   if (new_pad) {
      pad = GetEmptyPad();
      if (pad) {
         gROOT->SetSelectedPad(pad);
   		Double_t aspect_ratio = img_height * this->GetXsizeReal() 
                        		/ (img_width* this->GetYsizeReal());

   		if (fix_w) {  
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetWNDC(),
                  		pad->GetYlowNDC() + pad->GetWNDC() * aspect_ratio);
   		} else if (fix_h) {  
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetHNDC() / aspect_ratio,
                  		pad->GetYlowNDC() + pad->GetHNDC());
   		}

   		pad->SetTopMargin(.0);
   		pad->SetBottomMargin(0.0);
   		pad->SetLeftMargin(0.0);
   		pad->SetRightMargin(0.0);
//  
         pad->Range(0,0, (GetUxmax() - GetUxmin())* pad->GetWNDC()
                       , (GetUymax() - GetUymin())* pad->GetHNDC());
         
      } else {
         WarnBox("Please create a new Pad in this Canvas", fRootCanvas); 
         return;
      }   
   }
   cout << "InsertImage(): " <<  gPad->GetXlowNDC() << " " << gPad->GetYlowNDC() << " "
        <<  gPad->GetWNDC()    << " " << gPad->GetHNDC()    << endl;
   cout << "Image size, X,Y: " << img_width
                        << " " << img_height << endl;
   img->SetConstRatio(kTRUE);
   img->SetEditable(kTRUE);
   img->SetImageQuality(TAttImage::kImgBest);
   TString drawopt;
   if (offset_x == 0 && offset_y == 0) {
      drawopt= "xxx";
   } else {
      drawopt = "T";
      drawopt += offset_x;
      drawopt += ",";
      drawopt += offset_x;
      drawopt += ",#ffffff";
   }
   cout << drawopt << endl;
   hprimg->Draw(drawopt);
//   img->Draw("xxx");
//   hprimg->Paint();
   Update();
}

//______________________________________________________________________________

Int_t HTCanvas::MarkGObjects()
{
   return ExtractGObjects(kTRUE);
}
//______________________________________________________________________________

Int_t HTCanvas::ExtractGObjectsE()
{
    return ExtractGObjects(kFALSE);
}
//______________________________________________________________________________

Int_t HTCanvas::ExtractGObjects(Bool_t markonly)
{
   TCutG * cut = (TCutG *)FindObject("CUTG");
   if (!cut) {
      WarnBox("Define a graphical cut first", fRootCanvas); 
      return -1;
   }
   static Int_t serNr = 1;
   Bool_t ok;
   TString name = "pict_";
   name += serNr;
   serNr++;
tryagain:
   name =
   GetString("Name of object (must be unique)", name.Data(), &ok,
        (TGWindow*)fRootCanvas);
   if (fGObjectGroups) {
      if ( fGObjectGroups->FindObject(name) ) {
         WarnBox("Object with this name already exists");
         goto tryagain;
      }
   }     
   GroupOfGObjects * gg = new GroupOfGObjects(name.Data(), 0. ,0., NULL);
   Double_t * x = cut->GetX();
   Double_t * y = cut->GetY();
   gg->fXLowEdge = MinElement(cut->GetN(), x);
   gg->fXUpEdge  = MaxElement(cut->GetN(), x);
   gg->fYLowEdge = MinElement(cut->GetN(), y);
   gg->fYUpEdge  = MaxElement(cut->GetN(), y);
//   gg->fXLowEdge = TMath::MinElement(cut->GetN(), x);
//   gg->fXUpEdge  = TMath::MaxElement(cut->GetN(), x);
//   gg->fYLowEdge = TMath::MinElement(cut->GetN(), y);
//   gg->fYUpEdge  = TMath::MaxElement(cut->GetN(), y);
   Double_t xoff, yoff;
   if (markonly) {
      xoff = 0;
      yoff = 0;
   } else {
      xoff = 0.5 * (gg->fXUpEdge + gg->fXLowEdge);
      yoff = 0.5 * (gg->fYUpEdge + gg->fYLowEdge);
   }
   gg->fXorigin = xoff;
   gg->fYorigin = yoff;
//   gg->Dump();
   gg->SetEnclosingCut(cut);
   TObject * obj;
   TObjOptLink *lnk = (TObjOptLink*)GetListOfPrimitives()->FirstLink();
//   TIter next(GetListOfPrimitives());
   while ( lnk ) {
      obj = lnk->GetObject();
      if (obj == cut        // the enclosing TCutG itself
          || obj->InheritsFrom("EditMarker")
          || obj->InheritsFrom("GroupOfGObjects")) {
         lnk = (TObjOptLink*)lnk->Next();
         continue;
      }
      if (obj->InheritsFrom("TPave")) {
         TPave * b = (TPave*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX1(), b->GetY2())
            |cut->IsInside(b->GetX2(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            if (!markonly) {
               b = (TPave*)obj->Clone();

               b->SetX1NDC((b->GetX1() - gg->fXLowEdge) / (gg->fXUpEdge - gg->fXLowEdge));
               b->SetX2NDC((b->GetX2() - gg->fXLowEdge) / (gg->fXUpEdge - gg->fXLowEdge));
               b->SetY1NDC((b->GetY1() - gg->fYLowEdge) / (gg->fYUpEdge - gg->fYLowEdge));
               b->SetY2NDC((b->GetY2() - gg->fYLowEdge) / (gg->fYUpEdge - gg->fYLowEdge));
            }
            gg->AddMember(b,  lnk->GetOption());
         }
         
      } else if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX1(), b->GetY2())
            |cut->IsInside(b->GetX2(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            if (!markonly) {
               b = (TBox*)obj->Clone();
               b->SetX1(b->GetX1() - xoff);
               b->SetX2(b->GetX2() - xoff);
               b->SetY1(b->GetY1() - yoff);
               b->SetY2(b->GetY2() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }
         
      } else if (obj->InheritsFrom("TPad")) {
         TPad * b = (TPad*)obj;
         Double_t x1 = b->GetAbsXlowNDC();
         Double_t y1 = b->GetAbsYlowNDC();
         Double_t x2 = x1 + b->GetAbsWNDC();
         Double_t y2 = y1 + b->GetAbsHNDC();
//         convert to user 
         x1 = x1 * (GetX2() - GetX1());
         y1 = y1 * (GetY2() - GetY1());
         x2 = x2 * (GetX2() - GetX1());
         y2 = y2 * (GetY2() - GetY1());

         if (cut->IsInside(x1, y1)
            |cut->IsInside(x1, y2)
            |cut->IsInside(x2, y1)
            |cut->IsInside(x2, y2) ) {
            if (!markonly) {
               b = (TPad*)obj->Clone();
               x1 = (x1 - gg->fXLowEdge) / (gg->fXUpEdge - gg->fXLowEdge);
               x2 = (x2 - gg->fXLowEdge) / (gg->fXUpEdge - gg->fXLowEdge);
               y1 = (y1 - gg->fYLowEdge) / (gg->fYUpEdge - gg->fYLowEdge);
               y2 = (y2 - gg->fYLowEdge) / (gg->fYUpEdge - gg->fYLowEdge);
               b->SetPad(x1, y1, x2, y2);
            }
            gg->AddMember(b,  lnk->GetOption());
         }
         
      } else if (obj->InheritsFrom("TLine")){
         TLine * b = (TLine*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            if (!markonly) {
               b = (TLine*)obj->Clone();
               b->SetX1(b->GetX1() - xoff);
               b->SetX2(b->GetX2() - xoff);
               b->SetY1(b->GetY1() - yoff);
               b->SetY2(b->GetY2() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }

      } else if (obj->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            if (!markonly) {
               b = (TArrow*)obj->Clone();
               b->SetX1(b->GetX1() - xoff);
               b->SetX2(b->GetX2() - xoff);
               b->SetY1(b->GetY1() - yoff);
               b->SetY2(b->GetY2() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }

      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj;
         if (cut->IsInside(b->GetStartX(), b->GetStartY())
            |cut->IsInside(b->GetEndX(), b->GetEndY()) ) {
            if (!markonly) {
               b = (TCurlyLine*)obj->Clone();
               b->SetStartPoint(b->GetStartX() - xoff, b->GetStartY() - yoff);
               b->SetEndPoint(b->GetEndX() - xoff, b->GetEndY() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }
      } else if (obj->InheritsFrom("TMarker")) {
         TMarker * b = (TMarker*)obj;
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
            if (!markonly) {
               b = (TMarker*)obj->Clone();
               b->SetX(b->GetX() - xoff);
               b->SetY(b->GetY() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }

      } else if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)obj;
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
            if (!markonly) {
               b = (TText*)obj->Clone();
               b->SetX(b->GetX() - xoff);
               b->SetY(b->GetY() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }

      } else if (obj->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)obj;
         Bool_t inside = kFALSE;
         if (cut->IsInside(b->GetX1(), b->GetY1()))inside = kTRUE;
        
         if (!inside && b->GetPhimin() != 0 || b->GetPhimax() != 360) {
            Double_t ar = b->GetPhimin()*TMath::Pi()/ 180.;
            Double_t x1 = b->GetR1() * TMath::Cos(ar) + b->GetX1();
            Double_t y1 = b->GetR1() * TMath::Sin(ar) + b->GetY1();
            cout << " Inside? : " << x1 << " " << y1 << endl;
            if (cut->IsInside(x1, y1)) {
               inside = kTRUE;
            } else {
               ar = b->GetPhimax()*TMath::Pi()/ 180.;
               x1 = b->GetR1() * TMath::Cos(ar) + b->GetX1();
               y1 = b->GetR1() * TMath::Sin(ar) + b->GetY1();
               if (cut->IsInside(x1, y1)) inside = kTRUE;
            }
         }      
         if (inside) {
            if (!markonly) {
               b = (TEllipse*)obj->Clone();
               b->SetX1(b->GetX1() - xoff);
               b->SetY1(b->GetY1() - yoff);
            }
            gg->AddMember(b,  lnk->GetOption());
         }

      } else if (obj->IsA() == ControlGraph::Class()) {
         cout << "Skip ControlGraph" << endl;

      } else if (obj->IsA() == TGraph::Class() && !strncmp(obj->GetName(), "ParallelG", 9)) {
         cout << "Skip ParallelG" << endl;

      } else if (obj->IsA() == TSplineX::Class()) {
         TSplineX* b = ( TSplineX*)obj;
         ControlGraph* gr = b->GetControlGraph();
         Double_t * x = gr->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = gr->GetY();
//         either first or last point
         if (cut->IsInside(x[0], y[0]) 
            |cut->IsInside(x[gr->GetN()-1], y[gr->GetN()-1])) {
            if (!markonly) {
               b = (TSplineX*)obj->Clone();
               gr = b->GetControlGraph();
               gr->SetParent(b);
               Double_t* xt = new Double_t[gr->GetN()];
               Double_t* yt = new Double_t[gr->GetN()];
         	   for (Int_t i = 0; i < gr->GetN(); i++) {
            	   xt[i] = x[i] - xoff;
               	yt[i] = y[i] - yoff;
               }
               b->SetAllControlPoints(gr->GetN(), xt, yt);
               delete [] xt;
               delete [] yt;
         	}
            gg->AddMember(b,  lnk->GetOption());
         } 
      } else if (obj->InheritsFrom("TGraph")) {
         TGraph * b = (TGraph *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         if (cut->IsInside(x[0], y[0]) 
            |cut->IsInside(x[b->GetN()-1], y[b->GetN()-1])) {
            if (!markonly) {
               b = (TGraph*)obj->Clone();
               x = b->GetX();
               y = b->GetY();
         	   for (Int_t i = 0; i < b->GetN(); i++) {
            	   x[i] -= xoff;
               	y[i] -= yoff;
               }
         	}
            gg->AddMember(b,  lnk->GetOption());
         } 
      } else {
//         cout << obj->ClassName() << " not yet implemented" << endl;
      }
      lnk = (TObjOptLink*)lnk->Next();
   }   
   if (cut) delete cut;  
   cout <<  gg->GetNMembers()<< " objects in list " << endl;
   if (gg->GetNMembers() > 0) {
      if (!markonly) {
         if (!fGObjectGroups) fGObjectGroups = new TList();
         TObjArray *colors = (TObjArray*)gROOT->GetListOfColors();
         TObjArray * col_clone = (TObjArray*)colors->Clone(); 
         gg->AddMember(col_clone,"");
         fGObjectGroups->Add(gg);
         ShowGallery();
      } else {
         gg->Draw();
         Modified();
         Update();
      }
   }
   return gg->GetNMembers();
}
//______________________________________________________________________________

void HTCanvas::InsertGObjects(const char * objname)
{
   if (!fGObjectGroups) {
      cout << "No Macro objects defined" << endl;
      return;
   }
   static TString name;
   static Double_t scaleNDC = 1;
   static Double_t scaleU = 1;
   static Double_t angle = 0;
   static Short_t  align = 11;
   static Double_t x0 = 0;
   static Double_t y0 = 0;
   static Int_t    draw_cut = 1;

   static void *valp[25];
   Int_t ind = 0;

   TList *row_lab = new TList(); 

   TMrbString temp;
   if (objname && strlen(objname) > 1) {
      name = objname;
   } else if (name.Length() < 1) {
      GroupOfGObjects * gg = (GroupOfGObjects *)fGObjectGroups->First();
      name = gg->GetName();
   }
   row_lab->Add(new TObjString("StringValue_Name of object"));
   valp[ind++] = &name;
   row_lab->Add(new TObjString("DoubleValue_Scale factor NDC"));
   valp[ind++] = &scaleNDC;
   row_lab->Add(new TObjString("DoubleValue_Scale factor User"));
   valp[ind++] = &scaleU;
   row_lab->Add(new TObjString("DoubleValue_Angle[deg]"));
   valp[ind++] = &angle;
   row_lab->Add(new TObjString("AlignSelect_Alignment"));
   valp[ind++] = &align;
   row_lab->Add(new TObjString("DoubleValue_X value"));
   valp[ind++] = &x0;
   row_lab->Add(new TObjString("DoubleValue_Y value"));
   valp[ind++] = &y0;
   row_lab->Add(new TObjString("CheckButton_Draw enclosing cut"));
   valp[ind++] = &draw_cut;


   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Insert Compound Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   GroupOfGObjects * gg = (GroupOfGObjects *)fGObjectGroups->FindObject(name); 
   if (!gg) {
      cout << "Object " << name << " not found" << endl;
      return;
   }
   if (x0 == 0 && y0 == 0) {
   	cout << "Mark position with left mouse" << endl;
   	fGetMouse = kTRUE;
	   while (fGetMouse == kTRUE) {
   	   gSystem->ProcessEvents();
   	   gSystem->Sleep(10);
	   }
      x0 = fMouseX;
      y0 = fMouseY;
   }
   this->cd();
   TObjArray *colors = (TObjArray*)gg->GetMemberList()
                       ->FindObject("ListOfColors");
   if (colors) {
      Int_t ncolors = colors->GetEntries();
      TIter next(colors);
      for (Int_t i=0;i<ncolors;i++) {
//            TColor *colold = (TColor*)colors->At(i);
//            TColor *colcur = gROOT->GetColor(i);
         TColor *colold = (TColor*)next();
         Int_t cn = 0;
         if (colold) cn = colold->GetNumber();
         TColor *colcur = gROOT->GetColor(cn);
         if (colold) {
            if (colcur) {
               colcur->SetRGB(colold->GetRed(),colold->GetGreen(),colold->GetBlue());
            } else {
               colcur = new TColor(cn,colold->GetRed(),
                                     colold->GetGreen(),
                                     colold->GetBlue(),
                                     colold->GetName());
               cout << " Adding new color: " << endl; 
               colcur->Print(); 
            }
         }
      }
   }
   gg->AddMembersToList(this, x0, y0, scaleNDC, scaleU, angle, align, draw_cut); 
   
   this->Modified(); 
   this->Update(); 
}
//______________________________________________________________________________

void HTCanvas::WriteGObjects()
{
   if (!fGObjectGroups) {
      cout << "No graph macro objects defined" << endl;
      return;
   }
   Bool_t ok;
   TString name = "Graph_macros.root";
   TEnv env(".rootrc");
   name = env.GetValue("HistPresent.GraphMacrosFileName", name.Data());
   name = GetString("File name", name.Data(), &ok,
        (TGWindow*)fRootCanvas);
   if (!ok) return;
   env.SetValue("HistPresent.GraphMacrosFileName", name.Data());
   TFile * outfile = new TFile(name, "UPDATE");
   TIter next(fGObjectGroups);
   GroupOfGObjects * obj;
   while ( (obj = (GroupOfGObjects *)next()) ){    
//      obj->Print();
      obj->Write(obj->GetName());
         
   }
   outfile->Close();
}
//______________________________________________________________________________

void HTCanvas::ReadGObjects()
{
   Bool_t ok;
   TString name = "Graph_macros.root";
   TEnv env(".rootrc");
   name = env.GetValue("HistPresent.GraphMacrosFileName", name.Data());
   name = GetString("File name", name.Data(), &ok,
        (TGWindow*)fRootCanvas);
   if (!ok) return;
   TFile * infile = new TFile(name);
   env.SetValue("HistPresent.GraphMacrosFileName", name.Data());
   env.SaveLevel(kEnvUser);
   if (!fGObjectGroups) fGObjectGroups = new TList();
   GroupOfGObjects * obj;
   TIter next(infile->GetListOfKeys());
   TKey *key;
   while ( (key = (TKey*)next()) ){ 
      if (!strcmp(key->GetClassName(),"GroupOfGObjects")){
         obj=(GroupOfGObjects *)key->ReadObj(); 
         if (!obj) break; 
//         obj->Print();
         fGObjectGroups->Add(obj);
      }
   }
   infile->Close();
   ShowGallery();
}
//______________________________________________________________________________

void HTCanvas::ShowGallery()
{
   if (!fGObjectGroups) {
      cout << "No graph macro objects defined" << endl;
      return;
   }
   Int_t n = fGObjectGroups->GetSize();
   TDialogCanvas *dialog = new TDialogCanvas("GObjects", "Graphics macro objects",
                            5,500, 500, 500);
   Double_t dx = 0.18, dy = 0.18, marg = 0.02;
   Double_t x = marg;
   Double_t y = marg;
   TButton * b;
   TString cmd;
   TString oname;

   GroupOfGObjects * go;
   if (n > 25) {
      cout << "Max 25 objects allowed, in Collection: " << n << endl;
      n = 25;
   }
   TText * tt;
   for (Int_t i = 0; i < n; i++) {
      go = (GroupOfGObjects*)fGObjectGroups->At(i);
      oname = go->GetName();
      cmd = "((HTCanvas*)(gROOT->GetListOfCanvases()->FindObject(\"";
      cmd += this->GetName();
      cmd += "\")))->InsertGObjects(\"";
      cmd += oname;
      cmd += "\");";
      b = new TButton("", cmd.Data(),x, y, x + dx, y + dy);
      Double_t xr = go->fXUpEdge - go->fXLowEdge;
      Double_t yr = go->fYUpEdge - go->fYLowEdge;
      Double_t xoff = 0.5;
      Double_t yoff = 0.5;
      if (go->fXorigin != 0 || go->fYorigin) {
         xoff = (go->fXorigin - go->fXLowEdge) / xr;
         yoff = (go->fYorigin - go->fYLowEdge) / yr;
      }
      b->Range(-(xoff+0.1)*xr, -(yoff+0.1)*yr, (xoff+0.1)*xr, (yoff +0.1)*yr);
      b->Draw();
      TList * lop = b->GetListOfPrimitives();
//      lop->Add(tt);
      go->AddMembersToList(b, 0, 0, 1, 1, 0, 22);
      tt = new TText();
      tt->SetText(-.1* xr, -.3 *yr, oname.Data());
      tt->SetTextSize(0.15);
      lop->Add(tt, "");
//      tt->Draw();
      b->Update();
      dialog->cd();
      y = y + dy + marg;
      if (y > 1 - dy - marg) {
         x = x + dx + marg;
         y = marg;
      }
   }
}
//______________________________________________________________________________

void HTCanvas::PutObjectsOnGrid(TList* list)
{
   TList *row_lab = new TList(); 
   static void *valp[25];
   Int_t ind = 0;

   static Int_t dox      = 1,
                doy      = 1,
                dopad    = 1,
                dopave    = 1,
                doarrow  = 1,
                doline  = 1,
                dograph  = 1, 
                dotext   = 1, 
                doarc    = 1, 
                domark    = 1, 
                docurlyl = 1, 
                docurlya = 1, 
                doxspline = 1;

   row_lab->Add(new TObjString("CheckButton_Align X"));
   row_lab->Add(new TObjString("CheckButton_Align Y"));
   row_lab->Add(new TObjString("CheckButton_Pads"));
   row_lab->Add(new TObjString("CheckButton_Paves"));
   row_lab->Add(new TObjString("CheckButton_Arrows"));
   row_lab->Add(new TObjString("CheckButton_Lines,Axis"));
   row_lab->Add(new TObjString("CheckButton_Graphs"));
   row_lab->Add(new TObjString("CheckButton_Text"));
   row_lab->Add(new TObjString("CheckButton_Arcs"));
   row_lab->Add(new TObjString("CheckButton_Markers"));
   row_lab->Add(new TObjString("CheckButton_CurlyLines"));
   row_lab->Add(new TObjString("CheckButton_CurlyArcs"));
   row_lab->Add(new TObjString("CheckButton_TSplineXs"));
   
 
   valp[ind++] = &dox     ; 
   valp[ind++] = &doy     ; 
   valp[ind++] = &dopad   ; 
   valp[ind++] = &dopave   ;
   valp[ind++] = &doarrow ; 
   valp[ind++] = &doline  ; 
   valp[ind++] = &dograph ; 
   valp[ind++] = &dotext  ; 
   valp[ind++] = &doarc   ; 
   valp[ind++] = &domark  ; 
   valp[ind++] = &docurlyl; 
   valp[ind++] = &docurlya; 
   valp[ind++] = &doxspline;
   
   Bool_t ok; 
   Int_t itemwidth = 320;

   ok = GetStringExt("Align Objects at Grid", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   Double_t x1;
   Double_t y1;
   Double_t x2;
   Double_t y2;
   TObject * obj;
   TList * lof;
   if (list) lof = list;
   else      lof = GetListOfPrimitives(); 
   TIter next(lof);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("EditMarker")) continue; 
//      obj->Print();
      if (obj->InheritsFrom("TPave") && dopave) {
         TPave * b = (TPave*)obj;
         x1 = PutOnGridX(b->GetX1());
         y1 = PutOnGridX(b->GetY1());
         x2 = PutOnGridX(b->GetX2());
         y2 = PutOnGridX(b->GetY2());

         x1 = (x1 - this->GetX1()) / (this->GetX2() - this->GetX1());
         y1 = (y1 - this->GetY1()) / (this->GetY2() - this->GetY1());
         x2 = (x2 - this->GetX1()) / (this->GetX2() - this->GetX1());
         y2 = (y2 - this->GetY1()) / (this->GetY2() - this->GetY1());
         if (dox) b->SetX1NDC(x1);
         if (doy) b->SetY1NDC(y1); 
         if (dox) b->SetX2NDC(x2); 
         if (doy) b->SetY2NDC(y2);
         
      } else if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj;
         if (dox) b->SetX1(PutOnGridX(b->GetX1()));
         if (dox) b->SetX2(PutOnGridX(b->GetX2()));
         if (doy) b->SetY1(PutOnGridY(b->GetY1()));
         if (doy) b->SetY2(PutOnGridY(b->GetY2()));
         
      } else if (obj->InheritsFrom("TPad") && dopad) {
         TPad * b = (TPad*)obj;
         x1 = b->GetAbsXlowNDC();
         y1 = b->GetAbsYlowNDC();
         x2 = x1 + b->GetAbsWNDC();
         y2 = y1 + b->GetAbsHNDC();
//         convert to user 
         x1 = (x1 - GetX1()) * (GetX2() - GetX1());
         y1 = (y1 - GetY1()) * (GetY2() - GetY1());
         x2 = (x2 - GetX1()) * (GetX2() - GetX1());
         y2 = (y2 - GetY1()) * (GetY2() - GetY1());
         if (dox) x1 = PutOnGridX(x1);
         if (doy) y1 = PutOnGridX(y1);
         if (dox) x2 = PutOnGridX(x2);
         if (doy) y2 = PutOnGridX(y2);
         x1 = GetX1()+ x1 / (GetX2() - GetX1());
         y1 = GetY1()+ y1 / (GetY2() - GetY1());
         x2 = GetX1()+ x2 / (GetX2() - GetX1());
         y2 = GetY1()+ y2 / (GetY2() - GetY1());
         b->SetPad(x1, y1, x2, y2);

      } else if (obj->InheritsFrom("TLine") && doline){
         TLine * b = (TLine*)obj;
         if (dox) b->SetX1(PutOnGridX(b->GetX1()));
         if (dox) b->SetX2(PutOnGridX(b->GetX2()));
         if (doy) b->SetY1(PutOnGridY(b->GetY1()));
         if (doy) b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("TArrow") && doarrow) {
         TArrow * b = (TArrow*)obj;
         if (dox) b->SetX1(PutOnGridX(b->GetX1()));
         if (dox) b->SetX2(PutOnGridX(b->GetX2()));
         if (doy) b->SetY1(PutOnGridY(b->GetY1()));
         if (doy) b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("TCurlyArc") && docurlya) {
         TCurlyArc * b = (TCurlyArc*)obj;
         x1 = b->GetStartX();
         y1 = b->GetStartY();
         if (dox) x1 = PutOnGridX(x1);
         if (doy) y1 = PutOnGridX(y1);
         b->SetStartPoint(x1, y1);
  
      } else if (obj->InheritsFrom("TCurlyLine") && docurlyl) {
         TCurlyLine * b = (TCurlyLine*)obj;
         x1 = b->GetStartX();
         y1 = b->GetStartY();
         if (dox) x1 = PutOnGridX(x1);
         if (doy) y1 = PutOnGridX(y1);
         b->SetStartPoint(x1, y1);

         x1 = b->GetEndX();
         y1 = b->GetEndY();
         if (dox) x1 = PutOnGridX(x1);
         if (doy) y1 = PutOnGridX(y1);
         b->SetEndPoint(x1, y1);

      } else if (obj->InheritsFrom("TMarker") && domark) {
         TMarker * b = (TMarker*)obj;
         if (dox) b->SetX(PutOnGridX(b->GetX()));
         if (doy) b->SetY(PutOnGridX(b->GetY()));

      } else if (obj->InheritsFrom("TText") && dotext) {
         TText * b = (TText*)obj;
         if (dox) b->SetX(PutOnGridX(b->GetX()));
         if (doy) b->SetY(PutOnGridX(b->GetY()));

      } else if (obj->InheritsFrom("TArc") && doarc) {
         TArc * b = (TArc*)obj;
         if (dox) b->SetX1(PutOnGridX(b->GetX1()));
         b->SetY1(PutOnGridY(b->GetY1()));
//         b->SetX2(PutOnGridX(b->GetX2()));
//         b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("ControlGraph") && doxspline) {
         ControlGraph * b = (ControlGraph *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            if (dox) x[i] = PutOnGridX(x[i]);
            if (doy) y[i] = PutOnGridY(y[i]);
         }
         b->GetParent()->NeedReCompute();
      } else if (obj->InheritsFrom("TGraph") 
                 && !(obj->InheritsFrom("TSplineX"))
                 && !(obj->InheritsFrom("ControlGraph"))
                 && dograph) {
         TGraph * b = (TGraph *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            if (dox) x[i] = PutOnGridX(x[i]);
            if (doy) y[i] = PutOnGridY(y[i]);
         }
      } else {
//         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }
   Modified();
   Update();   
}
//______________________________________________________________________________

void HTCanvas::DeleteObjects()
{
   TCutG * cut = (TCutG *)FindObject("CUTG");
   if (!cut) {
      WarnBox("Define a graphical cut first", fRootCanvas); 
      return;
   }
   if (QuestionBox("Really delete objects?", fRootCanvas) != kMBYes) return;
   TObject * obj;
   TIter next(GetListOfPrimitives());
   while ( (obj = next()) ) {
      if (obj == cut) continue;      // the enclosing TCutG itself
      if (obj->InheritsFrom("EditMarker")) continue; 

      if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX1(), b->GetY2())
            |cut->IsInside(b->GetX2(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
             delete obj;
         }
         
      } else if (obj->InheritsFrom("TPad")) {
         TPad * b = (TPad*)obj;
         Double_t x1 = b->GetAbsXlowNDC();
         Double_t y1 = b->GetAbsYlowNDC();
         Double_t x2 = x1 + b->GetAbsWNDC();
         Double_t y2 = y1 + b->GetAbsHNDC();
//         convert to user 
         x1 = x1 * (GetX2() - GetX1());
         y1 = y1 * (GetY2() - GetY1());
         x2 = x2 * (GetX2() - GetX1());
         y2 = y2 * (GetY2() - GetY1());

         if (cut->IsInside(x1, y1)
            |cut->IsInside(x1, y2)
            |cut->IsInside(x2, y1)
            |cut->IsInside(x2, y2) ) {
             delete obj;
         }

      } else if (obj->InheritsFrom("TLine")){
         TLine * b = (TLine*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
             delete obj;
         }

      } else if (obj->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)obj;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
             delete obj;
         }

      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj;
         if (cut->IsInside(b->GetStartX(), b->GetStartY())
            |cut->IsInside(b->GetEndX(), b->GetEndY()) ) {
             delete obj;
         }
      } else if (obj->InheritsFrom("TMarker")) {
         TMarker * b = (TMarker*)obj;
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
             delete obj;
         }
      } else if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)obj;
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
             delete obj;
         }

      } else if (obj->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)obj;
         Bool_t inside = kFALSE;
         if (cut->IsInside(b->GetX1(), b->GetY1()))inside = kTRUE;
        
         if (!inside && b->GetPhimin() != 0 || b->GetPhimax() != 360) {
            Double_t ar = b->GetPhimin()*TMath::Pi()/ 180.;
            Double_t x1 = b->GetR1() * TMath::Cos(ar) + b->GetX1();
            Double_t y1 = b->GetR1() * TMath::Sin(ar) + b->GetY1();
            cout << " Inside? : " << x1 << " " << y1 << endl;
            if (cut->IsInside(x1, y1)) {
               inside = kTRUE;
            } else {
               ar = b->GetPhimax()*TMath::Pi()/ 180.;
               x1 = b->GetR1() * TMath::Cos(ar) + b->GetX1();
               y1 = b->GetR1() * TMath::Sin(ar) + b->GetY1();
               if (cut->IsInside(x1, y1)) inside = kTRUE;
            }
         }      
         if (inside) {
             delete obj;
         }

      } else if (obj->InheritsFrom("TGraph")) {
         TGraph * b = (TGraph *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            delete obj;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         if (cut->IsInside(x[0], y[0]) 
            || cut->IsInside(x[b->GetN()-1], y[b->GetN()-1])) {
             delete obj;
         } 
      } else {
//         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
   if (cut) delete cut;  
}
//______________________________________________________________________________

Int_t getm(TString& cmd, Int_t sind) 
{
   Int_t ind = sind;
   Int_t nbopen = 0;
   while (ind < cmd.Length()) {
     if (cmd[ind] == '}') {
        nbopen -= 1;
        if (nbopen == 0) return ind;
     }
     if (cmd[ind] == '{') nbopen += 1;
     ind += 1;
   }
   return -1;
}
Int_t getmb(TString& cmd, Int_t sind) 
{
   Int_t ind = sind;
   Int_t nclose = 0;
   while (ind >= 0) {
     if (cmd[ind] == '{') {
        nclose -= 1;
        if (nclose == 0) return ind;
     }
     if (cmd[ind] == '}') nclose += 1;
     ind -= 1;
   }
   return -1;
}
//______________________________________________________________________________

TString lat2root(TString& cmd) 
{
// this tries to translate standard Latex into ROOTs
// latex like formular processor TLatex format

//    remove latex's $ (mathstyle), replace \ by #, ~ by space
   Int_t ind;
   Int_t sind = 0;
   TString ill("Illegal syntax");
   while (cmd.Index("$") >=0) cmd.Remove(cmd.Index("$"),1);
   while (cmd.Index("\\{") >=0) cmd.Remove(cmd.Index("\\{"),1);
   while (cmd.Index("\\}") >=0) cmd.Remove(cmd.Index("\\}"),1);
   while (cmd.Index("\\") >=0) cmd(cmd.Index("\\")) = '#';
   while (cmd.Index("~") >=0) cmd(cmd.Index("~")) = ' ';
//  caligraphics not yet supported
   while (cmd.Index("#cal") >=0) {
      Int_t ob = cmd.Index("#cal");
      Int_t cb = getm(cmd, ob + 4);
      if (cb < 0) {
         cout << "in #cal no closing }" << endl;
         return ill;
      }
      cmd.Remove(cb,1);
      cmd.Remove(ob,5);
   }
//    make sure super / sub scripts are enclosed in {}
   TRegexp supsub("[_^]");
   TString rep; 
   sind = 0;
   while (cmd.Index(supsub, sind) >=0) {
      ind = 1 + cmd.Index(supsub, sind);
      char c = cmd[ind];
      sind = ind + 1;
      if (c != '{') {
         rep = "{";
         rep += c;
         rep += "}";
         cmd.Replace(ind, 1, rep);
         sind += 2;
      }
   }

//   add space around operators 

   TRegexp oper("[-+*/=]");
   sind = 0;
   while (cmd.Index(oper, sind) >=0) {

      ind = 1 + cmd.Index(oper, sind);
      char c = cmd[ind];
      sind = ind + 1;
//   are we within sub / superscript?

      TString le = cmd(0, ind -1);
      Int_t ob = le.Last('{');
      if (ob > 0 && (cmd(ob-1) == '^' 
                 || (cmd(ob-1) == '_'))) {
         if (getm(le, ob) == - 1)  continue; // no match before
      }
      if (c != ' ' && c!= '{' && c!= '}') {
         if (ind > -1 && ind < cmd.Length()) { 
            cmd.Insert(ind," ");
            sind += 1;
         }
      }
      if (ind > 1) {
         c = cmd[ind - 2];
         if (c != ' ' && c!= '{' && c!= '}') { 
            cmd.Insert(ind-1," ");
            sind += 1;
         }
      }
   }
//   replace \over by \frac{}{}

 ind = cmd.Index("#over");
 Int_t ind1 = cmd.Index("#overline");
 if (ind > 0 && ind != ind1) {
     TString le = cmd(0, ind);
     Int_t cb = le.Last('}');
     if (cb < 0) {
        cout << "no closing } found" << endl;
        return ill;
     }
     Int_t ob = getmb(cmd, cb);
     if (ob < 0) {
        cout << "no matching { found" << endl;
        return ill;
     }
     cmd.Remove(ind, 5);
     cmd.Insert(ob, " #frac");
  }

//   remove not used \cos etc,
//   replace overline by bar

   TRegexp re_Ra("#Ra");
   while (cmd.Index(re_Ra) >= 0) cmd(re_Ra) = " #Rightarrow ";
   TRegexp re_La("#La");
   while (cmd.Index(re_La) >= 0) cmd(re_La) = " #Leftarrow ";
   TRegexp re_cdot("#cdot");
   while (cmd.Index(re_cdot) >= 0) cmd(re_cdot) = " #upoint ";
   TRegexp re_exp("#exp");
   while (cmd.Index(re_exp) >= 0) cmd(re_exp) = " e^";
   TRegexp re_ln("#ln");
   while (cmd.Index(re_ln) >= 0) cmd(re_ln) = "ln";
   TRegexp re_cos("#cos");
   while (cmd.Index(re_cos) >= 0)cmd(re_cos) = "cos";
   TRegexp re_sin("#sin");
   while (cmd.Index(re_sin) >= 0)cmd(re_sin) = "sin";
   TRegexp re_tan("#tan");
   while (cmd.Index(re_tan) >= 0)cmd(re_tan) = "tan";
   TRegexp re_ovl("#overline");
   while (cmd.Index(re_ovl) >= 0)cmd(re_ovl) = "#bar";
   return cmd;
}

//______________________________________________________________________________

void HTCanvas::FeynmanDiagMenu()
{
 //  Int_t win_width = 160
   static void *valp[25];
   Int_t ind = 0;
   TList * labels = new TList;
   static TString fsp("FeynmanSetPars()");
   static TString far("FeynmanArrow()");
   static TString fwl("FeynmanWavyLine()");
   static TString fcl("FeynmanCurlyLine()");
   static TString fwa("FeynmanWavyArc()");
   static TString fca("FeynmanCurlyArc()");
   static TString fsl("FeynmanSolidLine()");
   static TString fdl("FeynmanDashedLine()");
   static TString ftx("FeynmanText()");

   labels->Add(new TObjString("CommandButt_FeynmanSetPars()"));
   valp[ind++] = &fsp;
   labels->Add(new TObjString("CommandButt_FeynmanArrow()")); 	
   valp[ind++] = &far;
   labels->Add(new TObjString("CommandButt_FeynmanWavyLine()")); 
   valp[ind++] = &fwl;
   labels->Add(new TObjString("CommandButt_FeynmanCurlyLine()"));
   valp[ind++] = &fcl;
   labels->Add(new TObjString("PlainIntVal_Phi start"));
   valp[ind++] = &fFeynmanPhi1;
   labels->Add(new TObjString("PlainIntVal_Phi end"));
   valp[ind++] = &fFeynmanPhi2;
   labels->Add(new TObjString("CommandButt_FeynmanWavyArc()"));   
   valp[ind++] = &fwa;
   labels->Add(new TObjString("CommandButt_FeynmanCurlyArc()"));  
   valp[ind++] = &fca;
   labels->Add(new TObjString("CommandButt_FeynmanSolidLine()")); 
   valp[ind++] = &fsl;
   labels->Add(new TObjString("CommandButt_FeynmanDashedLine()"));
   valp[ind++] = &fdl;
   labels->Add(new TObjString("CommandButt_FeynmanText()"));  	 
   valp[ind++] = &ftx;
   Bool_t ok;
   Int_t itemwidth = 320;

   ok = GetStringExt("Feynman diagram", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, labels,valp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//______________________________________________________________________________

void HTCanvas::FeynmanSetPars()
{
	if(fHistPresent){
	   fHistPresent->SetCurlyAttributes(fRootCanvas, fFitHist);
	   if(fFitHist)fFitHist->UpdateCanvas();
	}
};
//______________________________________________________________________________

void HTCanvas::FeynmanArrow()
{   	
   TArrow * a = (TArrow*)this->WaitPrimitive("TArrow");
   if (a);
};
void HTCanvas::FeynmanWavyLine()
{
	TCurlyLine::SetDefaultIsCurly(kFALSE);
   TCurlyLine * a = (TCurlyLine*)this->WaitPrimitive("TCurlyLine");
   if (a);
};
void HTCanvas::FeynmanWavyArc()
{
	TCurlyArc::SetDefaultIsCurly(kFALSE);
   TCurlyArc * a = (TCurlyArc*)this->WaitPrimitive("TCurlyArc");
   a = (TCurlyArc *)gPad->GetListOfPrimitives()->Last();
   a->SetPhimin(fFeynmanPhi1);
   a->SetPhimax(fFeynmanPhi2);
   Modified();
   Update();
};
void HTCanvas::FeynmanCurlyLine()
{
	TCurlyLine::SetDefaultIsCurly(kTRUE);
   TCurlyLine * a = (TCurlyLine*)this->WaitPrimitive("TCurlyLine");
   if (a);
};
void HTCanvas::FeynmanCurlyArc()
{
	TCurlyArc::SetDefaultIsCurly(kTRUE);
   TCurlyArc * a = (TCurlyArc*)this->WaitPrimitive("TCurlyArc");
   a = (TCurlyArc *)gPad->GetListOfPrimitives()->Last();
//   cout << "TCurlyArc" <<endl;
   a->SetPhimin(fFeynmanPhi1);
   a->SetPhimax(fFeynmanPhi2);
   Modified();
   Update();
};
void HTCanvas::FeynmanSolidLine()
{
   Style_t save = gStyle->GetLineStyle();
   gStyle->SetLineStyle(kSolid);    
   TLine * a = (TLine*)this->WaitPrimitive("TLine");
   gStyle->SetLineStyle(save);    
   if (a);
};
void HTCanvas::FeynmanDashedLine()
{
   Style_t save = gStyle->GetLineStyle();
   gStyle->SetLineStyle(kDashed);    
   TLine * a = (TLine*)this->WaitPrimitive("TLine");
   gStyle->SetLineStyle(save);    
   if (a);
};
void HTCanvas::FeynmanText()
{
//   TText * a = (TText*)this->WaitPrimitive("TText");
//   if (a);
   InsertText(kFALSE);
};

//______________________________________________________________________________

void HTCanvas::InsertTextSaveDefaults()
{
}
//______________________________________________________________________________

void HTCanvas::InsertTextSetDefaults()
{
//   cout << "HTCanvas::InsertTextSetDefaults()" << endl;
   fEditTextFileName = "latex.txt";
   fEditTextFromFile = 0;
   fEditTextX0 = 0;
   fEditTextY0 = 0;
   fEditTextDy = 10;
   fEditTextAlign = 11; 
   fEditTextColor = 1; 
   fEditTextFont  = 6; 
   fEditTextPrec  = 2; 
   fEditTextSize = 0.02;
   fEditTextAngle = 0;
   fEditTextMarkCompound = 0;
   fEditTextLatexFilter = 1;
   fEditTextSeqNr = 0;
}
//______________________________________________________________________________

void HTCanvas::InsertText(Bool_t from_file)
{
   static void *valp[25];
   Int_t ind = 0;

   fEditTextFromFile = (Int_t)from_file;

   static TString excmd("InsertTextExecute()");
   TList *row_lab = new TList(); 
 
   if (fEditTextFromFile) {
      row_lab->Add(new TObjString("StringValue_File Name with text"));
      valp[ind++] = &fEditTextFileName;
      fEditTextMarkCompound = 1;
   } else {
      fEditTextMarkCompound = 0;
   }
   row_lab->Add(new TObjString("DoubleValue_X Position"));
   valp[ind++] = &fEditTextX0;
   row_lab->Add(new TObjString("DoubleValue+Y Position"));
   valp[ind++] = &fEditTextY0;
   row_lab->Add(new TObjString("DoubleValue_Line spacing"));
   valp[ind++] = &fEditTextDy;
   row_lab->Add(new TObjString("Float_Value+Size"));
   valp[ind++] = &fEditTextSize;
   row_lab->Add(new TObjString("CfontSelect_Font"));
   valp[ind++] = &fEditTextFont;
   row_lab->Add(new TObjString("PlainIntVal+Precission"));
   valp[ind++] = &fEditTextPrec;
   row_lab->Add(new TObjString("ColorSelect_Color"));
   valp[ind++] = &fEditTextColor;
   row_lab->Add(new TObjString("AlignSelect+Alignment"));
   valp[ind++] = &fEditTextAlign;
   row_lab->Add(new TObjString("Float_Value+Angle"));
   valp[ind++] = &fEditTextAngle;
   row_lab->Add(new TObjString("CheckButton_Mark as compound"));
    valp[ind++] = &fEditTextMarkCompound;
   row_lab->Add(new TObjString("CheckButton+Apply latex filter"));
   valp[ind++] = &fEditTextLatexFilter;
   row_lab->Add(new TObjString("CommandButt_InsertTextExecute"));
   valp[ind++] = &excmd;

//   if (!from_file) {
//       row_lab->Add(new TObjString("CheckButton_Keep Dialog"));
//       valp[ind++] = &keepdialog;
//   }
  
   static TString text;
//   TString * tpointer = 0; 
   fEditTextPointer = NULL;
   const char * history = 0;
   if (fEditTextFromFile == 0) {
      fEditTextPointer = &text;
      const char hist_file[] = {"text_hist.txt"};
      history = hist_file;
      if (gROOT->GetVersionInt() < 40000) history = NULL;
   }
   fEditTextX0 = 0;
   fEditTextY0 = 0;
   Bool_t ok; 
   Int_t itemwidth = 280;
//loop:
   ok = GetStringExt("Text (latex) string", fEditTextPointer, itemwidth, fRootCanvas,
                      history, NULL, row_lab, valp,
                      NULL, NULL, NULL, this, this->ClassName());
//   if (!ok) return;
}
//______________________________________________________________________________

void HTCanvas::InsertTextExecute()
{
   if (fEditTextX0 == 0 && fEditTextY0 == 0) {
   	cout << "Mark position with left mouse" << endl;
   	fGetMouse = kTRUE;
	   while (fGetMouse == kTRUE) {
   	   gSystem->ProcessEvents();
   	   gSystem->Sleep(10);
	   }
      fEditTextX0 = fMouseX;
      fEditTextY0 = fMouseY;
      fMousePad->cd();
   }

   ifstream infile;
   TString line;
   TString cmd; 
   TString converted_line;

   TLatex  * latex;
   Double_t xt = fEditTextX0;
   Double_t yt = fEditTextY0;
   Double_t longestline = 0, th_first = 0, th_last = 0;
   TList llist;
   Bool_t loop = kTRUE;
   if (fEditTextFromFile != 0) {
      infile.open(fEditTextFileName.Data());
      if (!infile.good()){
         cout << "Cant open: " << fEditTextFileName << endl;
         return;
      }
   }
   while(loop) {
// read lines, concatinate lines ending with 
      if (fEditTextFromFile != 0) {
      	line.ReadLine(infile);
      	if (infile.eof()) {
	      	infile.close();
         	if (cmd.Length() > 0) {
            	cout << "Warning: Files ends with \\" << endl;
            	cout << cmd << endl;
         	}
	      	break;
      	}
      	line = line.Strip(TString::kBoth);
      	cmd = cmd + line;
      	if (cmd.EndsWith("\\")) {
         	cmd(cmd.Length()-1) = ' ';
         	continue;
      	}
      } else {
         cmd = fEditTextPointer->Data();
         cout << fEditTextPointer << " " << cmd.Data() << endl;
         loop = kFALSE;
      }
      if (fEditTextLatexFilter > 0) converted_line = lat2root(cmd);
      else             converted_line = cmd;
      latex = new TLatex(xt, yt, converted_line.Data());
      latex->SetTextAlign(fEditTextAlign);
      latex->SetTextFont(fEditTextFont * 10 + fEditTextPrec);
      latex->SetTextSize(fEditTextSize);
      latex->SetTextColor(fEditTextColor);
      latex->Draw();
      latex->SetTextAngle(fEditTextAngle);
      llist.Add(latex);
      yt -= fEditTextDy;
//      latex->Dump();
//      outfile << cmd << endl;
      cmd.Resize(0);
      if (latex->GetXsize() > longestline) longestline = latex->GetXsize();
      if (th_first <= 0) th_first = latex->GetYsize();
      th_last = latex->GetYsize();
   }

   Int_t nlines = llist.GetSize();
   if (nlines > 1) {
      GroupOfGObjects * text_group = NULL;
      TString cname("text_obj_");

      if (fEditTextMarkCompound) {
         text_group = new GroupOfGObjects(cname.Data(), 0, 0, NULL);
         cname += fEditTextSeqNr;
      }
      Double_t yshift = 0;
      if (fEditTextAlign%10 == 1)yshift =  (nlines -1) * fEditTextDy;
      if (fEditTextAlign%10 == 2)yshift =  (nlines  -1)* (0.5 * fEditTextDy);
      TIter next(&llist);
      while ( (latex = (TLatex*)next()) ) {
          latex->SetY(latex->GetY() + yshift);
          if (text_group) text_group->AddMember(latex, "");
      }
      if (fEditTextMarkCompound) {
         yt += fEditTextDy;        // last displayed line
      	Double_t xenc[5];
      	Double_t yenc[5];
      	yenc[0] = yt + yshift;
//      	if (fEditTextAlign%10 == 1)yenc[0] -= 0.5 * th_last;
      	if (fEditTextAlign%10 == 2)yenc[0] -= 0.5 * th_last;
      	if (fEditTextAlign%10 == 3)yenc[0] -= th_last;
      	yenc[1] = yenc[0];

      	yenc[2] = fEditTextY0 + yshift;
      	if (fEditTextAlign%10 == 2)yenc[2] += 0.5 * th_first;
      	if (fEditTextAlign%10 == 1)yenc[2] += th_first;
      	yenc[3] = yenc[2];
      	yenc[4] = yenc[0];

      	Int_t halign = fEditTextAlign / 10;
      	if (halign == 1) {
         	xenc[0] = fEditTextX0;
         	xenc[1] = xenc[0] + longestline;
         	xenc[0] -= 0.001;
      	} else if (halign == 2) {
         	xenc[0] = fEditTextX0 + 0.5 * longestline;
         	xenc[1] = xenc[0] - longestline;
      	} else {
         	xenc[0] = fEditTextX0 - longestline;
         	xenc[1] = fEditTextX0 + 0.001;
      	}
      	xenc[2] = xenc[1];
      	xenc[3] = xenc[0];
      	xenc[4] = xenc[0];
      	TCutG cut(cname.Data(), 5, xenc, yenc);
	//      cut.Print();
      	text_group->SetEnclosingCut(&cut);
      	text_group->Draw();
   	}
   }
   Modified();
   Update();
   fEditTextX0 = 0;
   fEditTextY0 = 0;
//   if (!from_file && keepdialog == 1) goto loop;
}
//______________________________________________________________________________

void HTCanvas::InsertFunction()
{
   static void *valp[25];
   Int_t ind = 0;
   static Int_t Npar = 3;
   static Double_t par[10] = {1, -.2, 4, 0, 0, 0, 0, 0, 0, 0};
   static TString func_name("fun1");
//   static TString new_func_name("ff");
   static TString xtitle("x");
   static TString ytitle("y");
   static Double_t from = 0;
   static Double_t to   = 10;
   static Color_t fcol = kBlue;
   static Int_t pad_opacity = 30;
   static Int_t same_pad = 0;
   static Int_t new_pad = 1;
   static Int_t new_canvas = 0;

   Bool_t ok;
   Npar = GetInteger("Number of parameters", Npar, &ok, fRootCanvas);
   if (!ok) return;
   if (Npar < 1) {
      WarnBox("Number of parameters < 1", fRootCanvas);
      return;
   }
   if (Npar > 10) {
      WarnBox("Number of parameters > 10", fRootCanvas);
      return;
   }
   TList *row_lab = new TList(); 
//   TList *values  = new TList();
tryagain:
   ind = 0;
   row_lab->Clear();
   row_lab->Add(new TObjString("StringValue_Function Name"));
   valp[ind++] = &func_name;

   row_lab->Add(new TObjString("StringValue_X axis title"));
   valp[ind++] = &xtitle;
   row_lab->Add(new TObjString("StringValue_Y axis title"));
   valp[ind++] = &ytitle;
   row_lab->Add(new TObjString("DoubleValue_Lower limit (from)"));
   valp[ind++] = &from;
   row_lab->Add(new TObjString("DoubleValue_Upper limit (to)"));
   valp[ind++] = &to;

   TString text;
   TString * tpointer = 0; 
   const char * history = 0;
   tpointer = &text;
   const char hist_file[] = {"text_formulas.txt"};
   history = hist_file;
   if (gSystem->AccessPathName(history)) {
      ofstream hfile(history);
      hfile << "[0]*exp([1]*x)*cos([2]*x)" << endl;
      hfile.close();
   } 

   if (gROOT->GetVersionInt() < 40000) history = NULL;
   for (Int_t i =0; i < Npar; i++) {
      row_lab->Add(new TObjString(Form("DoubleValue_Value of Parameter %d",i)));
      valp[ind++] = &par[i];
   }
   row_lab->Add(new TObjString("ColorSelect_Drawing color"));
   valp[ind++] = &fcol;
   row_lab->Add(new TObjString("PlainIntVal_Opacity of pad (0-100)"));
   valp[ind++] = &pad_opacity;
   row_lab->Add(new TObjString("RadioButton_Use new(selected) pad"));
   valp[ind++] = &new_pad;
   row_lab->Add(new TObjString("RadioButton_Use same (selected) pad"));
   valp[ind++] = &same_pad;
   row_lab->Add(new TObjString("RadioButton_Create separate canvas"));
   valp[ind++] = &new_canvas;

   Int_t itemwidth = 440;
   ok = GetStringExt("Function formula", tpointer, itemwidth, fRootCanvas,
                      history, NULL, row_lab, valp);
   if (!ok) return;

   if (gROOT->GetListOfFunctions()->FindObject(func_name))
      IncrementIndex(&func_name);
   
   if (gROOT->GetListOfFunctions()->FindObject(func_name)) {
      cout << "Function with name: " << func_name << " already exists"
           << endl;
      goto tryagain;
   }
   if (new_pad != 0) {
   	TPad* pad = GetEmptyPad();
   	if (pad) {
   	  gROOT->SetSelectedPad(pad);
   	} else {
      	WarnBox("Please create a new Pad in this Canvas\n\
or select \"Use same (selected) pad\"", fRootCanvas); 
         goto tryagain;
   	} 
   }
   TF1 * f = new TF1(func_name.Data(), tpointer->Data(), from, to);
   if (gROOT->GetListOfFunctions()->FindObject(func_name) == 0) {
       WarnBox("Error in formula, watch printout", fRootCanvas);
      goto tryagain;
   }
   f->SetParameters(par);
   f->SetLineColor(fcol);
   if (pad_opacity > 0) {
      if (pad_opacity > 100) pad_opacity = 100;
      gPad->SetFillStyle(4000 + pad_opacity);
   }
   if (new_canvas != 0) new TCanvas("cc", "cc", 600,400);
   if (same_pad != 0) f->Draw("same");
   else                     f->Draw();
   gPad->Update();
}
//______________________________________________________________________________

void HTCanvas::InsertAxis()
{
   static void *valp[25];
   Int_t ind = 0;
   Double_t x0 = 0;
   Double_t y0 = 0;
   Double_t x1 = 0;
   Double_t y1 = 0;
   static Double_t wmin = 0;
   static Double_t wmax = 10;
   static Int_t    ndiv = 510;
   static Int_t    logscale = 0;
   static Int_t    usetimeformat = 0;
   static Int_t    timezero = 0;
   TString chopt; 
   static TString tformat("H.%H M.%M S.%S"); 

   TList *row_lab = new TList(); 
   row_lab->Add(new TObjString("DoubleValue_X Start"));
   valp[ind++] = &x0;
   row_lab->Add(new TObjString("DoubleValue_Y Start"));
   valp[ind++] = &y0;
   row_lab->Add(new TObjString("DoubleValue_X End"));
   valp[ind++] = &x1;
   row_lab->Add(new TObjString("DoubleValue_Y End"));
   valp[ind++] = &y1;

   row_lab->Add(new TObjString("DoubleValue_Axis Value at Start"));
   valp[ind++] = &wmin;
   row_lab->Add(new TObjString("DoubleValue_Axis Value at End"));
   valp[ind++] = &wmax;
   row_lab->Add(new TObjString("PlainIntVal_N divisions"));
   valp[ind++] = &ndiv;
   row_lab->Add(new TObjString("CheckButton_Logarithmic scale"));
   valp[ind++] = &logscale;
   row_lab->Add(new TObjString("CheckButton_Use Timeformat"));
   valp[ind++] = &usetimeformat;
   row_lab->Add(new TObjString("StringValue_Timeformat"));
   valp[ind++] = &tformat;
   row_lab->Add(new TObjString("PlainIntVal_Timeoffset"));
   valp[ind++] = &timezero;


   Bool_t ok; 
   Int_t itemwidth = 320;
tryagain:
   ok = GetStringExt("Axis Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   if (usetimeformat > 0) chopt += "t";
   if (logscale   > 0) chopt += "G";
   
   if (x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0) {
   	cout << "Input a TLine defining the axis" << endl;
      TLine * l = (TLine*)this->WaitPrimitive("TLine");
      if (l) {
         x0 = l->GetX1();
         y0 = l->GetY1();
         x1 = l->GetX2();
         y1 = l->GetY2();
         delete l;
      } else {
         cout << "No TLine found, try again" << endl;
         goto tryagain;
      }
   }
   TGaxis *ax = new TGaxis(x0, y0, x1, y1, wmin, wmax, ndiv, chopt.Data());
   TString whichA;
   if (TMath::Abs(y1-y0) < TMath::Abs(x1-x0)) whichA = "X";
   else                                       whichA = "Y";
   ax->SetLineColor(gStyle->GetAxisColor(whichA.Data()));
   ax->SetLabelColor(gStyle->GetLabelColor(whichA.Data()));
   ax->SetLabelOffset(gStyle->GetLabelOffset(whichA.Data()));
   ax->SetLabelFont(gStyle->GetLabelFont(whichA.Data()));
   ax->SetLabelSize(gStyle->GetLabelSize(whichA.Data()));
   ax->SetTickSize(gStyle->GetTickLength(whichA.Data()));
   ax->SetTitleSize(gStyle->GetTitleSize(whichA.Data()));
   ax->SetTitleOffset(gStyle->GetTitleOffset(whichA.Data()));

   if (usetimeformat) ax->SetTimeFormat(tformat.Data());
   ax->Draw();
   Modified();
   Update();
}
//______________________________________________________________________________

void HTCanvas::InsertTSplineX()
{
   new TSplineXDialog(fRootCanvas);
/*
   static void *valp[25];
   Int_t ind = 0;
   TList * row_lab = new TList(); 
   static Int_t    closed  = 0;
   static Int_t    approx  = 1;
   static Int_t    fixends = 1;
   static Double_t prec    = 0.2;
   static Int_t    showcp  = 1;
   static Color_t  color   = gStyle->GetLineColor();
   static Width_t  lwidth  = gStyle->GetLineWidth();
   static Style_t  lstyle  = gStyle->GetLineStyle();
   static Color_t  fcolor  = gStyle->GetFillColor();
   static Style_t  fstyle  = gStyle->GetFillStyle();

   static Double_t filled        = 0;
   static Double_t empty         = 5;
   static Double_t gage          = 4;
   static Int_t    arrow_at_start = 0;
   static Int_t    arrow_at_end  = 0;
   static Int_t    arrow_filled  = 0;
   static Double_t arrow_size    = 10;
   static Double_t arrow_angle   = 30;
   static Double_t arrow_indent_angle  = -30;

   row_lab->Add(new TObjString("CheckButton_Closed TSplineX"));
   valp[ind++] = &closed;
   row_lab->Add(new TObjString("CheckButton+Approximate"));
   valp[ind++] = &approx;
   row_lab->Add(new TObjString("CheckButton_Fix endpoints"));
   valp[ind++] = &fixends;
   row_lab->Add(new TObjString("CheckButton+Show Controlpoints"));
   valp[ind++] = &showcp;
   row_lab->Add(new TObjString("DoubleValue_Precision"));
   valp[ind++] = &prec;
   row_lab->Add(new TObjString("ColorSelect+Line Col"));
   valp[ind++] = &color;
   row_lab->Add(new TObjString("PlainShtVal_Line Width"));
   valp[ind++] = &lwidth;
   row_lab->Add(new TObjString("LineSSelect+Line Style"));
   valp[ind++] = &lstyle;
   row_lab->Add(new TObjString("CheckButton_Arrow@Start"));
   valp[ind++] = &arrow_at_start;
   row_lab->Add(new TObjString("CheckButton+Arrow@End"));
   valp[ind++] = &arrow_at_end;
   row_lab->Add(new TObjString("CheckButton_Fill Arrow"));
   valp[ind++] = &arrow_filled;
   row_lab->Add(new TObjString("DoubleValue+ArLength"));
   valp[ind++] = &arrow_size;
   row_lab->Add(new TObjString("DoubleValue_ArAngle"));
   valp[ind++] = &arrow_angle;
   row_lab->Add(new TObjString("DoubleValue+IndentAng"));
   valp[ind++] = &arrow_indent_angle;
//   row_lab->Add(new TObjString("CheckButton_Railway (double line)"));
//   valp[ind++] = &railway;
   row_lab->Add(new TObjString("DoubleValue_Railway Gage"));
   valp[ind++] = &gage;
   row_lab->Add(new TObjString("DoubleValue_SleeperL"));
   valp[ind++] = &filled;
   row_lab->Add(new TObjString("DoubleValue+SleeperDist"));
   valp[ind++] = &empty;
   row_lab->Add(new TObjString("ColorSelect_Fill Color"));
   valp[ind++] = &fcolor;
   row_lab->Add(new TObjString("Fill_Select+Fill Style"));
   valp[ind++] = &fstyle;

   Bool_t ok; 
   Int_t itemwidth = 320;
tryagain:
   ok = GetStringExt("TSplineX Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

  cout << "Input a  Polyline defining the controlpoints" << endl;
  TIter next(this->GetListOfPrimitives());
  TObject * obj;
  while ( (obj = next()) ) {
     if (obj->IsA() == TGraph::Class()) {
        TGraph * g = (TGraph*)obj;
        if(!(strncmp(g->GetName(), "Graph", 5))) { 
           cout << "Rename existing Graph" << endl;
           g->SetName("Hprgraph");
        }
     }
  }
  TGraph * gr = (TGraph*)this->WaitPrimitive("Graph", "PolyLine");
  if (!gr) {
      cout << "No PolyLine found, try again" << endl;
      goto tryagain;
   }
   gr->SetName("abc");
   Double_t* x = gr->GetX();
   Double_t* y = gr->GetY();
   Int_t npoints = gr->GetN();
//  add an extra point in between
   if (npoints < 2) {
      cout << "Need at least 2 points, try again" << endl;
      goto tryagain;
   }
   TArrayF shape_factors(npoints);
   if ( (fixends == 0 || closed == 1) && approx == 1) {
      shape_factors[0] = 1;
      shape_factors[npoints - 1] = 1;
   } else {
      shape_factors[0] = -1;
      shape_factors[npoints - 1] = -1;
   }
   for (Int_t i = 1; i < npoints - 1; i++) {
   	if (approx == 1) {
      	shape_factors[i] = 1;
   	} else {
      	shape_factors[i] = -1;
   	}
   }
   Bool_t closed_spline;
   if (closed != 0) closed_spline = kTRUE;
   else             closed_spline = kFALSE;
   TSplineX* xsp = 
     new TSplineX(npoints, x, y, shape_factors.GetArray(), prec, closed_spline);
   xsp->Draw("L");
   xsp->SetFillColor(fcolor);
   xsp->SetFillStyle(fstyle);
   xsp->SetLineColor(color);
   xsp->SetLineWidth(lwidth);
   xsp->SetLineStyle(lstyle);
   xsp->SetFilledLength(filled);
   xsp->SetEmptyLength(empty);
   if (gage > 0)xsp->SetRailwaylike(gage);
   if (showcp > 0) xsp->DrawControlPoints(0, 0);
   Bool_t afilled;
   if (arrow_filled == 0) afilled = kFALSE;
   else                  afilled = kTRUE;
   if (arrow_at_start) xsp->AddArrow(0, arrow_size, arrow_angle, arrow_indent_angle, afilled);
   if (arrow_at_end)   xsp->AddArrow(1, arrow_size, arrow_angle, arrow_indent_angle, afilled);
   delete gr;
   Modified();
   Update();
*/
}
//______________________________________________________________________________

void HTCanvas::InsertCurlyArrow()
{
   cout << "InsertCurlyArrow()" << endl;
   static void *valp[25];
   Int_t ind = 0;
   TList * row_lab = new TList(); 
   static Color_t  color   = gStyle->GetLineColor();
   static Width_t  lwidth  = gStyle->GetLineWidth();
   static Style_t  lstyle  = gStyle->GetLineStyle();

   static Int_t    arrow_at_start = 0;
   static Int_t    arrow_at_end  = 0;
   static Double_t arrow_size    = 0;
   static Double_t arrow_angle   = 30;
   static Double_t ampl          = 0.01;
   static Double_t wlen          = 0.02;

   row_lab->Add(new TObjString("DoubleValue_Ampl"));
   valp[ind++] = &ampl;
   row_lab->Add(new TObjString("DoubleValue+WaveL"));
   valp[ind++] = &wlen;
   row_lab->Add(new TObjString("ColorSelect_LColor"));
   valp[ind++] = &color;
   row_lab->Add(new TObjString("PlainShtVal+LWidth"));
   valp[ind++] = &lwidth;
   row_lab->Add(new TObjString("LineSSelect+LStyle"));
   valp[ind++] = &lstyle;
   row_lab->Add(new TObjString("CheckButton_Arrow@Start"));
   valp[ind++] = &arrow_at_start;
   row_lab->Add(new TObjString("CheckButton+Arrow@End"));
   valp[ind++] = &arrow_at_end;
   row_lab->Add(new TObjString("DoubleValue_ArLength"));
   valp[ind++] = &arrow_size;
   row_lab->Add(new TObjString("DoubleValue+ArAngle"));
   valp[ind++] = &arrow_angle;

   Bool_t ok; 
   Int_t itemwidth = 320;
tryagain:
   ok = GetStringExt("Curly Arrow Params", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

  cout << "Input a Line" << endl;
/*
  TIter next(this->GetListOfPrimitives());
  TObject * obj;
  while ( (obj = next()) ) {
     if (obj->IsA() == TGraph::Class()) {
        TGraph * g = (TGraph*)obj;
        if(!(strncmp(g->GetName(), "Graph", 5))) { 
           cout << "Rename existing Graph" << endl;
           g->SetName("Hprgraph");
        }
     }
  }
*/
   TLine * gr = (TLine*)this->WaitPrimitive("TLine", "Line");
   if (!gr) {
      cout << "No Line found, try again" << endl;
      goto tryagain;
   }
//   gr->SetName("abc");
//   Double_t* x = gr->GetX();
//   Double_t* y = gr->GetY();
 //  Int_t npoints = gr->GetN();
//  add an extra point in between
 //  if (npoints < 2) {
 //     cout << "Need at least 2 points, try again" << endl;
 //     goto tryagain;
 //  }
   Int_t where = 0;
   if (arrow_at_start) where += 1;
   if (arrow_at_end)   where += 2;
   TCurlyLineWithArrow* xsp = 
     new TCurlyLineWithArrow(gr->GetX1(), gr->GetY1(), gr->GetX2(), gr->GetY2(), 
                             wlen, ampl, where, arrow_size );
   xsp->Draw();
   xsp->SetLineColor(color);
   xsp->SetLineWidth(lwidth);
   xsp->SetLineStyle(lstyle);
   delete gr;
   Modified();
   Update();
}
