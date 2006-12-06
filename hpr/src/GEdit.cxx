
#include "TArc.h"
#include "TArrow.h"
#include "TArrayI.h"
#include "TPave.h"
#include "TColor.h"
#include "TButton.h"
#include "TDialogCanvas.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "TEllipse.h"
#include "TEnv.h"
#include "TF1.h"
#include "TFile.h"
#include "TKey.h"
#include "TLatex.h"
#include "TGaxis.h"
#include "TRegexp.h"
#include "TImage.h"
#include "TAttImage.h"
#include "TContextMenu.h"
#include "TCanvasImp.h"
#include "TRootCanvas.h"
#include "TObjString.h"
#include "TGMsgBox.h"
#include <TGClient.h>

#include "TGMrbTableFrame.h"
#include "EditMarker.h"
#include "GroupOfGObjects.h"
#include "TGMrbInputDialog.h"
#include "InsertArcDialog.h"
#include "InsertTextDialog.h"
#include "TGMrbValuesAndText.h"

#include "HprImage.h"
#include "HprEditBits.h"
#include "HprEditCommands.h"
#include "TSplineX.h"
#include "TSplineXDialog.h"
#include "InsertFunctionDialog.h"
#include "FeynmanDiagramDialog.h"
#include "TCurlyLineWithArrow.h"
#include "CurlyLineWithArrowDialog.h"
#include "SetColor.h"
#include "GEdit.h"

#include <fstream>
//extern Bool_t SloppyInside(TCutG * cut, Double_t x, Double_t y);
extern Double_t MinElement(Int_t n, Double_t * x);
extern Double_t MaxElement(Int_t n, Double_t * x);
extern void SetAllArrowSizes(TList *list, Double_t size,  Bool_t abs); 
extern void SetAllCurlySizes(TList *list, Double_t wl, Double_t amp, Bool_t abs);
extern void SetAllTextSizes(TList *list, Double_t size, Bool_t abs);

enum EGeditCommandIds {
   M_WritePrim,
   M_SetGrid,
   M_UseGrid,
   M_DrawGrid,  
   M_DrawGridVis,  
   M_RemoveGrid,
   M_RemoveCGraph,
   M_DrawCGraph,
   M_SetVisEnclosingCut,
   M_PutObjectsOnGrid,
   M_ExtractGObjects,
   M_MarkGObjects,
   M_InsertGObjectsG,
   M_InsertGObjects,
   M_WriteGObjects,
   M_DeleteObjects,
   M_ReadGObjects,
   M_ShowGallery
}; 

ClassImp(GEdit)

//____________________________________________________________________________
#ifdef MARABOUVERS
GEdit::GEdit(HTCanvas * parent)
#else
GEdit::GEdit(TCanvas * parent)
#endif
{
   cout << "ctor GEdit, parent " << parent << endl;
   fParent = parent;
   fRootCanvas = (TRootCanvas*)parent->GetCanvas()->GetCanvasImp();
   fOrigWw = fParent->GetWw();
   fOrigWh = fParent->GetWh();
   RestoreDefaults();
   BuildMenu();
   InitEditCommands();

   if (fUseEditGrid) {
      SetUseEditGrid(kTRUE);
      DrawEditGrid(kTRUE);
      fParent->Modified();
      fParent->Update();
   }
   SetBit(kMustCleanup);
   gROOT->GetListOfCleanups()->Add(this);
}
//______________________________________________________________________________

GEdit::~GEdit() 
{
   cout << "~GEdit " << this << endl;
   SaveDefaults();
   if (fEditCommands) delete fEditCommands;
}

//______________________________________________________________________________

void GEdit::RecursiveRemove(TObject * obj)
{
//   cout << "GEdit::RecursiveRemove " << obj << endl;
   if (obj == fParent) {
      gROOT->GetListOfCleanups()->Remove(this);
      delete this;
   }
}
//______________________________________________________________________________

void GEdit::BuildMenu()
{
   TGMenuBar * menubar = ((TRootCanvas*)fParent->GetCanvasImp())->GetMenuBar();
//   TGLayoutHints * layoh = ((TRootCanvas*)fParent->GetCanvasImp())->GetMenuBarItemLayout();
//   TGLayoutHints * layoh_right = new TGLayoutHints(kLHintsTop | kLHintsRight);
   TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
   fEditMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   fEditMenu->AddEntry("Write this picture to root file",  M_WritePrim);
   fEditMenu->AddEntry("Write macro objects to file",      M_WriteGObjects);
   fEditMenu->AddEntry("Read macro objects from file",     M_ReadGObjects);
   fEditMenu->AddEntry("Display list of macro objects",    M_ShowGallery);
   fEditMenu->AddSeparator();
   fEditMenu->AddEntry("Mark selected objects as compound", M_MarkGObjects);
   fEditMenu->AddEntry("Extract selected objects as compound",  M_ExtractGObjects);
   fEditMenu->AddEntry("Delete selected objects",          M_DeleteObjects);

   fEditMenu->AddSeparator();
   fEditMenu->AddEntry("Set Edit Grid",             M_SetGrid);
   fEditMenu->AddEntry("Use Edit Grid",             M_UseGrid);
   fEditMenu->AddEntry("Align objects at grid",     M_PutObjectsOnGrid);
   if (fUseEditGrid) fEditMenu->CheckEntry(M_UseGrid);
   else              fEditMenu->UnCheckEntry(M_UseGrid);
   fEditMenu->AddEntry("Draw visible grid",         M_DrawGridVis);
   fEditMenu->AddEntry("Draw real grid",            M_DrawGrid  );
   fEditMenu->AddEntry("Remove Edit Grid",          M_RemoveGrid);
   fEditMenu->AddEntry("Remove Control Graphs",     M_RemoveCGraph);
   fEditMenu->AddEntry("Draw Control Graphs",       M_DrawCGraph);
   fEditMenu->AddEntry("Make EnclosingCut visible", M_SetVisEnclosingCut);
   fEditMenu->CheckEntry(M_SetVisEnclosingCut);
//   TGPopupMenu * help = menubar->GetPopup("Help");
//   menubar->RemovePopup("Help");
//   fRootCanvas->MapSubwindows();
//   fRootCanvas->MapWindow(); 
   menubar->AddPopup("Hpr-Edit", fEditMenu, layoh_left, menubar->GetPopup("Help"));
   fEditMenu->Connect("Activated(Int_t)", "GEdit", this,
                      "HandleMenu(Int_t)");
   menubar->MapSubwindows();
   menubar->Layout(); 
}
//______________________________________________________________________________

void GEdit::HandleMenu(Int_t id)
{
   switch (id) {

      case M_WritePrim:
         WritePrimitives();
         break;

      case M_WriteGObjects:
         WriteGObjects();
         break;

      case M_ReadGObjects:
         ReadGObjects();
         break;

      case M_ShowGallery:
         ShowGallery();
         break;

      case M_MarkGObjects:
         MarkGObjects();
         break;

      case M_ExtractGObjects:
         ExtractGObjects();
         break;

      case M_DeleteObjects:
         DeleteObjects();
         break;

      case M_SetGrid:
         {
         SetEditGrid(0, 0, 0, 0);
         }
         break;

      case M_UseGrid:
         {
     		if (fUseEditGrid) {
               fEditMenu->UnCheckEntry(M_UseGrid);
               SetUseEditGrid(kFALSE);
      		} else {                    
               fEditMenu->CheckEntry(M_UseGrid);
               SetUseEditGrid(kTRUE);
            }
         }
         break;

      case M_PutObjectsOnGrid:
         PutObjectsOnGrid();
         break;

      case M_DrawGridVis:
         DrawEditGrid(kTRUE);
         break;

      case M_DrawGrid:
         DrawEditGrid(kFALSE);
         break;

      case M_RemoveGrid:
         RemoveEditGrid();
         break;

      case M_RemoveCGraph:
         RemoveControlGraphs();
         break;

      case M_DrawCGraph:
         DrawControlGraphs();
         break;

      case M_SetVisEnclosingCut:
         if (fEditMenu->IsEntryChecked(M_SetVisEnclosingCut)) {
            fEditMenu->UnCheckEntry(M_SetVisEnclosingCut);
            SetVisibilityOfEnclosingCuts(kFALSE);
         } else {
            fEditMenu->CheckEntry(M_SetVisEnclosingCut);
            SetVisibilityOfEnclosingCuts(kTRUE);
         }
         break;
   }
}

//______________________________________________________________________________
Bool_t GEdit::SloppyInside(TCutG * cut, Double_t x, Double_t y)
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

Double_t GEdit::MinElement(Int_t n, Double_t * x){
   Double_t min = x[0];
   for (Int_t i = 1; i < n; i++) {
      if (x[i] < min) min = x[i];
   }
   return min;
}
//______________________________________________________________________________

Double_t GEdit::MaxElement(Int_t n, Double_t * x){
   Double_t max = x[0];
   for (Int_t i = 1; i < n; i++) {
      if (x[i] > max) max = x[i];
   }
   return max;
}
//_______________________________________________________________________________________

void GEdit::SetAllArrowSizes(TList *list, Double_t size,  Bool_t abs) 
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

void GEdit::SetAllCurlySizes(TList *list, Double_t wl, Double_t amp, Bool_t abs) 
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

void GEdit::SetAllTextSizes(TList *list, Double_t size, Bool_t abs) 
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

void GEdit::InitEditCommands()
{
   Int_t win_width = 160;
   TList * labels = new TList;
   TList * methods = new TList;
   Bool_t has_xgrabsc = kFALSE;
   char * xgrabsc = gSystem->Which(gSystem->Getenv("PATH"), "xgrabsc");
   if (xgrabsc != NULL) {
      has_xgrabsc = kTRUE;
      delete [] xgrabsc;
   }
   if (has_xgrabsc) 
      labels->Add(new TObjString("Grab image from screen"));
   labels->Add(new TObjString("Insert image (gif, jpg"));
#ifdef MARABOUVERS
   labels->Add(new TObjString("Insert histogram "));
#endif
   labels->Add(new TObjString("Insert graph"));
   labels->Add(new TObjString("Insert Function Dialog"));
   labels->Add(new TObjString("Feynman Dialog"));
   labels->Add(new TObjString("Text (Latex) from file"));
   labels->Add(new TObjString("Text (Latex) from keyboard"));
   labels->Add(new TObjString("Insert compound object"));
   labels->Add(new TObjString("Draw an axis"));
   labels->Add(new TObjString("TSplineX Dialog"));
   labels->Add(new TObjString("Draw CurlyLine with Arrow"));
   labels->Add(new TObjString("Insert Arc Dialog"));

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
#ifdef MARABOUVERS
   methods->Add(new TObjString("InsertHist()"));
#endif
   methods->Add(new TObjString("InsertGraph()"));
   methods->Add(new TObjString("InsertFunction()"));
   methods->Add(new TObjString("FeynmanDiagMenu()"));
   methods->Add(new TObjString("InsertTextF()"));
   methods->Add(new TObjString("InsertTextK()"));
   methods->Add(new TObjString("InsertGObjects()"));
   methods->Add(new TObjString("InsertAxis()"));
   methods->Add(new TObjString("InsertTSplineX()"));
   methods->Add(new TObjString("InsertCurlyArrow()"));
   methods->Add(new TObjString("InsertArc()"));
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

void GEdit::ModifyObjects()
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

void GEdit::ListAllObjects()
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

void GEdit::ModifyTexts()
{
   fParent->cd();
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
//   Int_t prec = 3;
   TIter next_1(gPad->GetListOfPrimitives());
   while ( (obj = next_1()) ) {
      if (obj->InheritsFrom("TLatex")) {
         text = (TLatex*)obj;

         os = new TObjString(text->GetTitle());
         vt[ind + 0 * nrows] = (TObject*)os;

//         TMrbString sx(text->GetX(), prec);
         os = new TObjString(Form("%g", text->GetX()));
         vt[ind + 1 * nrows] = (TObject*)os;

//         TMrbString sy(text->GetY(), prec);
         os = new TObjString(Form("%g", text->GetY()));
         vt[ind + 2 * nrows] = (TObject*)os;

//         TMrbString ss(text->GetTextSize(), prec);
         os = new TObjString(Form("%g", text->GetTextSize()));
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
//   Double_t val;
   TIter next_2(gPad->GetListOfPrimitives());
   while ( (obj = next_2()) ) {
      if (obj->InheritsFrom("TLatex")) {
         if (flags[ind] >0) {
            text = (TLatex*)obj;
            os = (TObjString *) values.At(ind + 0 * nrows);
            text->SetTitle(os->GetString().Data());

            os = (TObjString *) values.At(ind + 1 * nrows);
//            TMrbString sx(os->GetString().Data());
//            sx.ToDouble(val);
            text->SetX(atof(os->GetString().Data()));

            os = (TObjString *) values.At(ind + 2 * nrows);
//            TMrbString sy(os->GetString().Data());
//            sy.ToDouble(atof(os->GetString().Data());
            text->SetY(atof(os->GetString().Data()));

            os = (TObjString *) values.At(ind +3 * nrows);
//            TMrbString ss(os->GetString().Data());
//            ss.ToDouble(val);
            text->SetTextSize(atof(os->GetString().Data()));
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

void GEdit::ModifyCurlyLines()
{
   fParent->cd();
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

void GEdit::ModifyCurlyArcs()
{
   fParent->cd();
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

void GEdit::ModifyArrows()
{
   fParent->cd();
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

void GEdit::ModifyArcs()
{
   fParent->cd();
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

void GEdit::ModifyEllipses()
{
   fParent->cd();
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

void GEdit::ModifyMarkers()
{
   fParent->cd();
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

void GEdit::ModifyPads()
{
//   if (gPad != this) {
//      WarnBox("Cant modify subpads
   fParent->cd();
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

void GEdit::ModifyPaves()
{
   fParent->cd();
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

void GEdit::ModifyGraphs()
{
   fParent->cd();
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

void GEdit::DefineBox()
{
   fParent->cd();
   TObject * obj = gPad->WaitPrimitive("TPave");
   obj = gPad->GetListOfPrimitives()->Last();
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
      fParent->Update();
   } else {
       cout << "Error getting TPave" << endl;
   }
}
//______________________________________________________________________________

void GEdit::DefinePolygone()
{
   fParent->cd();
   TObject * obj = gPad->GetListOfPrimitives()->FindObject("CUTG");
   if (obj) delete obj;
   obj = gPad->WaitPrimitive("CUTG", "CutG");
//   obj = gPad->GetListOfPrimitives()->Last();
   if (obj->IsA() == TCutG::Class()) {
       ((TCutG*)obj)->SetLineWidth(1);
       ((TCutG*)obj)->SetLineStyle(2);
       ((TCutG*)obj)->SetLineColor(1);
        fParent->Update();
   } else {
       cout << "Error getting TCutG" << endl;
   }
}     
//______________________________________________________________________________

void GEdit::RemoveTSplineXsPolyLines()
{
   TIter next(fParent->GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) ((TSplineX*)obj)->RemovePolyLines();
   }
//   fParent->Update();
}
//______________________________________________________________________________

void GEdit::DrawTSplineXsParallelGraphs()
{
   fParent->cd();
   TIter next(fParent->GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) {
         ((TSplineX*)obj)->NeedReCompute();
         ((TSplineX*)obj)->DrawParallelGraphs();
      }
   }
   fParent->Modified();
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::DrawControlGraphs()
{
   fParent->cd();
   TIter next( fParent->GetListOfPrimitives());
   TObject * obj;
   fParent->cd();
   while ( (obj = next()) ){
      if (obj->IsA() == TSplineX::Class()) ((TSplineX*)obj)->DrawControlPoints();
   }
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::RemoveControlGraphs()
{
   fParent->cd();
   TList * lop = fParent->GetListOfPrimitives();
   TObject * obj;
   while ( (obj = lop->FindObject("ControlGraph")) ) {
      cout << obj->ClassName() << endl;
      lop->Remove(obj);
   }
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::RemoveParallelGraphs()
{
   TList * lop =  fParent->GetListOfPrimitives();
   TObject * obj;
   while ( (obj = lop->FindObject("ParallelG")) ){
      lop->Remove(obj);
   }
//   fParent->Modified();
//   fParent->Update();
}
//______________________________________________________________________________

void GEdit::RemoveEditGrid()
{
   fParent->cd();
   TIter next( fParent->GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->GetUniqueID() == 0xaffec0c0) delete obj;
   }
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::WritePrimitives()
{
   Bool_t ok;
//   TString name = "drawing";
   static TString name(fPictureName);
   name = GetString("Save picture with name", name.Data(), &ok,
                 (TGWindow*)fRootCanvas);
   if (!ok)
      return;
   TString fn(fRootFileName);
   fn = GetString("File name", fn.Data(), &ok,
                 (TGWindow*)fRootCanvas);
   if (!ok)
      return;

   TFile *f = new TFile(fn, "UPDATE"); 
   RemoveEditGrid();
   RemoveControlGraphs();
   fParent->Write(name.Data());
   f->Close();
   fPictureName =  name;
   fRootFileName = fn;
   SaveDefaults();
}
///______________________________________________________________________________

void GEdit::SetVisibilityOfEnclosingCuts(Bool_t visible)
{
   fParent->cd();
   TIter next( fParent->GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->TestBit(kIsEnclosingCut))
        ((GroupOfGObjects*)obj)->SetVisible(visible);  
   }
   fParent->Update();
}
#ifdef MARABOUVERS
//______________________________________________________________________________

void GEdit::InsertHist()
{
   fParent->cd();
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
//      if ( fParent->GetListOfPrimitives()->Contains(selected)) {
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
            hist->SetFillColor(fHistPresent->fHistFillColor);
         } else
            hist->SetFillStyle(0);
      } else if (hist->GetDimension() == 2) {
         drawopt = fHistPresent->fDrawOpt2Dim->Data();
      }
      hist->SetOption(drawopt.Data());
      hist->SetDrawOption(drawopt.Data());
      pad->Modified();
   }
   fParent->Update();
}
#endif
//______________________________________________________________________________

TPad*  GEdit::GetEmptyPad()
{
   TIter next(fParent->GetListOfPrimitives());
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

void GEdit::InsertGraph()
{
   fParent->cd();
   TPad* pad = GetEmptyPad();
   if (pad) {
     gROOT->SetSelectedPad(pad);
   } else {
      WarnBox("Please create a new Pad in this Canvas", fRootCanvas); 
      return;
   }   
//      if ( fParent->GetListOfPrimitives()->Contains(selected)) {
   static void *valp[25];
   Int_t ind = 0;
   TList *row_lab = new TList(); 
   static TString fname;
   static TString gname;
   static TString goption;
   static Int_t select_from_list = 1;
   static Double_t scale = 1;
//   if (fHistPresent) goption = fHistPresent->fDrawOptGraph;
//   else 
   goption = "AL";

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
#ifdef MARABOUVERS
   
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
#endif
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
#ifdef MARABOUVERS
   }
#endif
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
   fParent->Update();
}

//______________________________________________________________________________

void GEdit::GrabImage()
{
   fParent->cd();
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
   valp[ind++] = &pname;
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
   if (xwg == 0 && fix_size == 0) {
      cout << setred 
      << "Variable width requested, please mark area with Button 1 pressed"
      << setblack  << endl;
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
   sernr++;
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
      x2 = x1 + fParent->AbsPixeltoX(xwg);
      y2 = y1 + fParent->AbsPixeltoY(ywg);

      gPad->GetRange(xr1, yr1, xr2, yr2);
      x1 = (x1 - xr1) / (xr2 - xr1);
      y1 = (y1 - yr1) / (yr2 - yr1);
      x2 = x1 + (Double_t) xwg / (Double_t)gPad->GetWw();
      y2 = y1 + (Double_t) ywg / (Double_t)gPad->GetWh();

      TPad * pad = new TPad(pname.Data(), "For HprImage", 
                            x1, y1, x2, y2); 
      pad->Draw(); 
      TImage *hprimg = TImage::Open(pname.Data());
//      HprImage * hprimg = new HprImage(pname.Data(), pad);
      gROOT->SetSelectedPad(pad);
      pad->cd();
      hprimg->Draw("xxx");
      fParent->cd();
      fParent->Update();
   }
}
//______________________________________________________________________________

void GEdit::InsertImage()
{
   fParent->cd();
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

   TImage *img = TImage::Open(name.Data());
//   HprImage * hprimg = new HprImage(name.Data(), pad);
//   TImage *img = hprimg->GetImage();
   if (!img) {
      cout << "Could not create an image... exit" << endl;
      return;
   }
   Double_t img_width = (Double_t )img->GetWidth();
   Double_t img_height = (Double_t )img->GetHeight();
  
   if (new_pad) {
      pad = GetEmptyPad();
      pad = (TPad*)gPad;
      if (pad) {
         gROOT->SetSelectedPad(pad);
   		Double_t aspect_ratio = img_height * fParent->GetXsizeReal() 
                        		/ (img_width* fParent->GetYsizeReal());

   		if (fix_w) {  
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetWNDC(),
                  		pad->GetYlowNDC() + pad->GetWNDC() * aspect_ratio);
   		} else if (fix_h) {  
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetHNDC() / aspect_ratio,
                  		pad->GetYlowNDC() + pad->GetHNDC());
   		}

//   		pad->SetTopMargin(.0);
//   		pad->SetBottomMargin(0.0);
//   		pad->SetLeftMargin(0.0);
//   		pad->SetRightMargin(0.0);
//  
//         pad->Range(0,0, (GetUxmax() - GetUxmin())* pad->GetWNDC()
//                       , (GetUymax() - GetUymin())* pad->GetHNDC());
         
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
//   cout << drawopt << endl;
//   hprimg->Draw(drawopt);
   img->Draw(drawopt);
//   hprimg->Paint();
//   if(pad) pad->Range(0,0, (GetUxmax() - GetUxmin())* pad->GetWNDC()
//                       , (GetUymax() - GetUymin())* pad->GetHNDC());
//   img->Pop();
   fParent->Update();
}

//______________________________________________________________________________

Int_t GEdit::MarkGObjects()
{
   return ExtractGObjects(kTRUE);
}
//______________________________________________________________________________

Int_t GEdit::ExtractGObjectsE()
{
    return ExtractGObjects(kFALSE);
}
//______________________________________________________________________________

Int_t GEdit::ExtractGObjects(Bool_t markonly)
{
   fParent->cd();
   cout << "fParent " << fParent<< " gPad " << gPad << endl;
   TCutG * cut = (TCutG *)gPad->FindObject("CUTG");
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
   TObjOptLink *lnk = (TObjOptLink*) fParent->GetListOfPrimitives()->FirstLink();
//   TIter next( fParent->GetListOfPrimitives());
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
         x1 = x1 * ( fParent->GetX2() -  fParent->GetX1());
         y1 = y1 * ( fParent->GetY2() -  fParent->GetY1());
         x2 = x2 * ( fParent->GetX2() -  fParent->GetX1());
         y2 = y2 * ( fParent->GetY2() -  fParent->GetY1());

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
         fParent->Modified();
         fParent->Update();
      }
   }
   return gg->GetNMembers();
}
//______________________________________________________________________________

void GEdit::InsertGObjects(const char * objname)
{
   fParent->cd();
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

//   TMrbString temp;
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
      TMarker * mark  = (TMarker*)gPad->WaitPrimitive("TMarker");
      mark  = (TMarker*)gPad->GetListOfPrimitives()->Last();
      x0 = mark->GetX();
      y0 = mark->GetY();
      delete mark;
   }
   cout << "x0 " << x0 << endl;
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
   gg->AddMembersToList(fParent, x0, y0, scaleNDC, scaleU, angle, align, draw_cut); 
   x0 = y0 = 0;
   fParent->Modified(); 
   fParent->Update(); 
}
//______________________________________________________________________________

void GEdit::WriteGObjects()
{
   fParent->cd();
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

void GEdit::ReadGObjects()
{
   fParent->cd();
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

void GEdit::ShowGallery()
{
   fParent->cd();
   if (!fGObjectGroups) {
      cout << "No graph macro objects defined" << endl;
      return;
   }
   TVirtualPad *padsave = gPad;
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
      cmd += fParent->GetName();
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
   if (padsave)padsave->cd();
}
//______________________________________________________________________________

void GEdit::PutObjectsOnGrid(TList* list)
{
   fParent->cd();
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
   else      lof =  fParent->GetListOfPrimitives(); 
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

         x1 = (x1 - fParent->GetX1()) / (fParent->GetX2() - fParent->GetX1());
         y1 = (y1 - fParent->GetY1()) / (fParent->GetY2() - fParent->GetY1());
         x2 = (x2 - fParent->GetX1()) / (fParent->GetX2() - fParent->GetX1());
         y2 = (y2 - fParent->GetY1()) / (fParent->GetY2() - fParent->GetY1());
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
         x1 = (x1 -  fParent->GetX1()) * ( fParent->GetX2() -  fParent->GetX1());
         y1 = (y1 -  fParent->GetY1()) * ( fParent->GetY2() -  fParent->GetY1());
         x2 = (x2 -  fParent->GetX1()) * ( fParent->GetX2() -  fParent->GetX1());
         y2 = (y2 -  fParent->GetY1()) * ( fParent->GetY2() -  fParent->GetY1());
         if (dox) x1 = PutOnGridX(x1);
         if (doy) y1 = PutOnGridX(y1);
         if (dox) x2 = PutOnGridX(x2);
         if (doy) y2 = PutOnGridX(y2);
         x1 =  fParent->GetX1()+ x1 / ( fParent->GetX2() -  fParent->GetX1());
         y1 =  fParent->GetY1()+ y1 / ( fParent->GetY2() -  fParent->GetY1());
         x2 =  fParent->GetX1()+ x2 / ( fParent->GetX2() -  fParent->GetX1());
         y2 =  fParent->GetY1()+ y2 / ( fParent->GetY2() -  fParent->GetY1());
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
   fParent->Modified();
   fParent->Update();   
}
//______________________________________________________________________________

void GEdit::DeleteObjects()
{
   fParent->cd();
   TCutG * cut = (TCutG *)FindObject("CUTG");
   if (!cut) {
      WarnBox("Define a graphical cut first", fRootCanvas); 
      return;
   }
   if (QuestionBox("Really delete objects?", fRootCanvas) != kMBYes) return;
   TObject * obj;
   TIter next( fParent->GetListOfPrimitives());
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
         x1 = x1 * ( fParent->GetX2() -  fParent->GetX1());
         y1 = y1 * ( fParent->GetY2() -  fParent->GetY1());
         x2 = x2 * ( fParent->GetX2() -  fParent->GetX1());
         y2 = y2 * ( fParent->GetY2() -  fParent->GetY1());

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

void GEdit::FeynmanDiagMenu()
{
   fParent->cd();
   new FeynmanDiagramDialog();
}
//______________________________________________________________________________

void GEdit::InsertText(Bool_t from_file)
{
   fParent->cd();
   new InsertTextDialog(from_file);
}
//______________________________________________________________________________

void GEdit::InsertFunction()
{
   fParent->cd();
   new InsertFunctionDialog();
}
//______________________________________________________________________________

void GEdit::InsertAxis()
{
static const Char_t helpText[] = 
"InsertAxis()\n\
";
   fParent->cd();
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
                      NULL, NULL, row_lab, valp,
                      NULL, NULL, helpText);
   if (!ok) return;
   if (usetimeformat > 0) chopt += "t";
   if (logscale   > 0) chopt += "G";
   
   if (x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0) {
   	cout << "Input a TLine defining the axis" << endl;
      TLine * l = (TLine*)fParent->WaitPrimitive("TLine");
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
   fParent->Modified();
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::InsertTSplineX()
{
   fParent->cd();
   new TSplineXDialog();
}
//______________________________________________________________________________

void GEdit::InsertCurlyArrow()
{
   fParent->cd();
   new CurlyLineWithArrowDialog();
}
//______________________________________________________________________________

void GEdit::InsertArc()
{
   fParent->cd();
   new InsertArcDialog();
}
//______________________________________________________________________________

void GEdit::WarnBox(const Char_t *text, TRootCanvas *win )
{
   fParent->cd();
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), win, "Warning", text,
                kMBIconExclamation, kMBDismiss, &retval);
}
//______________________________________________________________________________

Bool_t GEdit::QuestionBox(const Char_t *text, TRootCanvas *win)
{
   fParent->cd();
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), win, "Question", text,
                kMBIconQuestion, kMBYes | kMBNo, &retval);
   if (retval == kMBNo)
      return kFALSE;
   else
      return kTRUE;
}
//______________________________________________________________________________

void GEdit::ZoomIn()
{
   fParent->cd();
   if (fParent->GetWw() > 15000 || fParent->GetWh() > 15000) {
      cout << "No more zoom possible" << endl;
      return;
   }
   fParent->SetCanvasSize(fParent->GetWw() << 1, fParent->GetWh() << 1);
   fParent->Update();
}
//______________________________________________________________________________

void GEdit::ZoomOut()
{
   fParent->cd();
   if (fParent->GetWw() > fOrigWw) {
      fParent->SetCanvasSize(fParent->GetWw() >> 1, fParent->GetWh() >> 1);
      fParent->Update();
   }
}
//______________________________________________________________________________

void GEdit::UnZoom()
{
   fParent->cd();
   fParent->SetCanvasSize(fOrigWw, fOrigWh);
   fParent->Update();
}
//______________________________________________________________________________

Double_t GEdit::PutOnGridX(Double_t x)
{
   if (fEditGridX ==  0) return x;
   Int_t n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
   return (Double_t)n * fEditGridX;
}   
//______________________________________________________________________________

Double_t GEdit::PutOnGridY(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Int_t n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
   return (Double_t)n * fEditGridY;
}   
//______________________________________________________________________________
void  GEdit::SetUseEditGrid(Int_t use) 
{
   if (use) { 
      if (fEditGridX <= 0 ||  fEditGridY <= 0) {
         cout << "fEditGridX <= 0 ||  fEditGridY <= 0 " << endl;
         return;
      }
   }
#ifdef MARABOUVERS
   fParent->SetEditGrid(fEditGridX, fEditGridY);
   fParent->SetUseEditGrid(use);
#endif
   fUseEditGrid = use;
   Int_t temp = 0;
   if (use) temp = 1;
};
//______________________________________________________________________________

void GEdit::SetEditGrid(Double_t x, Double_t y, Double_t xvis, Double_t yvis)
{
   fParent->cd();
   if (x <= 0 || x <= 0) {
      static void *valp[4];
      Int_t ind = 0;
      TList *row_lab = new TList();
      row_lab->Add(new TObjString("DoubleValue_Real EditGrid X"));
      row_lab->Add(new TObjString("DoubleValue_Real EditGrid Y"));
      row_lab->Add(new TObjString("DoubleValue_Visible Grid X"));
      row_lab->Add(new TObjString("DoubleValue_Visible Grid Y"));
      valp[ind++] = &fEditGridX;
      valp[ind++] = &fEditGridY;
      valp[ind++] = &fVisibleGridX;
      valp[ind++] = &fVisibleGridY;
      Bool_t ok;
      Int_t itemwidth = 280;

      ok = GetStringExt("Define edit grid", NULL, itemwidth, fRootCanvas,
                      NULL, NULL, row_lab, valp);
      row_lab->Delete();
      delete row_lab;
      if(!ok) return;

   } else { 
      fEditGridX = x; 
      fEditGridY = y;
      fVisibleGridX = xvis; 
      fVisibleGridY = yvis;
   }
   SaveDefaults();
#ifdef MARABOUVERS
   fParent->SetEditGrid(fEditGridX, fEditGridY);
   SetUseEditGrid(kTRUE);
#endif
   SetUseEditGrid(kTRUE);
   DrawEditGrid(kTRUE);
}
//______________________________________________________________________________

void GEdit::DrawEditGrid(Bool_t visible)
{
   fParent->cd();
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
   fParent->GetRange(xl, yl, xh, yh);
   cout << "DrawEditGrid, fParent, dx, xh  " << fParent << " " << dx << " " << xh  << endl;

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
   fParent->Modified();
   fParent->Update();
}
//_____________________________________________________________________

void GEdit::SaveDefaults()
{
   TEnv env(".rootrc");
   env.SetValue("GEdit.RootFileName",   fRootFileName);
   env.SetValue("GEdit.PictureName",    fPictureName);
   env.SetValue("GEdit.EditGridX",      fEditGridX);  
   env.SetValue("GEdit.EditGridY",      fEditGridY);  
   env.SetValue("GEdit.VisibleGridX",   fVisibleGridX);
   env.SetValue("GEdit.VisibleGridY",   fVisibleGridY);
   env.SetValue("GEdit.UseEditGrid",    fUseEditGrid);
   env.SaveLevel(kEnvUser);
}
//_____________________________________________________________________
void GEdit::RestoreDefaults()
{
   TEnv env(".rootrc");
   fRootFileName    = env.GetValue("GEdit.RootFileName", "pictures.root"); 
   fPictureName     = env.GetValue("GEdit.PictureName",  "pict"); 
   fEditGridX       = env.GetValue("GEdit.EditGridX",      5.);
   fEditGridY       = env.GetValue("GEdit.EditGridY",      5.);
   fVisibleGridX    = env.GetValue("GEdit.VisibleGridX",  10.);
   fVisibleGridY    = env.GetValue("GEdit.VisibleGridY",  10.);
   fUseEditGrid     = env.GetValue("GEdit.UseEditGrid",    1);
}
