#include "TArrow.h"
#include "TDialogCanvas.h"
#include "TCurlyLine.h"
#include "TEllipse.h"
#include "TEnv.h"
#include "TImage.h"
#include "TAttImage.h"

#include "HistPresent.h"
#include "support.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "TContextMenu.h"
#include "TGMrbInputDialog.h"
#include "EditMarker.h"
#include "GroupOfGObjects.h"
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
         cout << "SetArrowSize(s) " << s << endl;
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
    cout << ":DrawEditGrid " << dx << " " << dy << endl;
   Double_t xl, yl, xh, yh;
   GetRange(xl, yl, xh, yh);

   Double_t x = xl;
   Double_t y;
   EditMarker * em;
   while (x <= xh) {
      y = yl;   
      while (y <= yh) {
         em = new EditMarker(x, y, 0.5);
         em->Draw();
         y += dy;
      }
      x += dx;
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::RemoveEditGrid()
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->GetUniqueID() == 0xaffec0c0) delete obj;
   }
   Update();
}
//______________________________________________________________________________

void HTCanvas::DrawHist()
{
   if (!fHistPresent) return;
   TPad * selected = (TPad *)gROOT->GetSelectedPad();
   if (selected) {
//      cout << selected << endl;
//      GetListOfPrimitives()->ls();
 //     cout << GetListOfPrimitives()->FindObject("newpad") << endl;
      if (GetListOfPrimitives()->Contains(selected)) {
         if (fHistPresent->fSelectHist->GetSize() != 1) {
            cout << "select exactly 1 histogram" << endl;
            return;
         } else {
            TH1* hist = fHistPresent->GetSelHistAt(0, NULL, kTRUE);
            if (hist) {
//               hist->Print();
               selected->cd();
               hist->Draw();
               TString drawopt;
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
               selected->Modified();
            }
         }
      } else {
         cout << "Please select a Pad (middle mouse) in this Canvas" << endl;
      } 
   } else {
         cout << "No Pad selected" << endl;
   } 
   Update();
}

//______________________________________________________________________________

void HTCanvas::InsertImage()
{
   TPad * pad = (TPad *)gROOT->GetSelectedPad();
   if (pad && GetListOfPrimitives()->Contains(pad)) {
      Bool_t ok;
      TString name = "picture.jpg";
      name =
         GetString("Picture name", name.Data(), &ok,
                 (TGWindow*)fRootCanvas);
      if (!ok) return;
      TImage *img = TImage::Open(name.Data());
      if (!img) {
         cout << "Could not create an image... exit" << endl;
         return;
      }
      cout <<  pad->GetXlowNDC() << " " << pad->GetYlowNDC() << " "
           <<  pad->GetWNDC()    << " " << pad->GetHNDC()    << endl;
      Double_t img_width = (Double_t )img->GetWidth();
      Double_t img_height = (Double_t )img->GetHeight();
      cout << "Image size, X,Y: " << img_width
                           << " " << img_height << endl;
      Double_t aspect_ratio = img_height * this->GetXsizeReal() 
                            / (img_width* this->GetYsizeReal());
      pad->SetPad(pad->GetXlowNDC(),pad->GetYlowNDC(),
                  pad->GetXlowNDC() + pad->GetWNDC(),
                  pad->GetYlowNDC() + pad->GetWNDC() * aspect_ratio);
      pad->SetTopMargin(0);
      pad->SetBottomMargin(0);
      pad->SetLeftMargin(0);
      pad->SetRightMargin(0);
      img->SetConstRatio(kTRUE);
//      img->SetConstRatio(kFALSE);
      img->SetImageQuality(TAttImage::kImgBest);
      img->Draw();
      Update();
   } else {
      cout << "Please select a Pad (middle mouse) in this Canvas" << endl;
   }
}

//______________________________________________________________________________

void HTCanvas::WritePrimitives()
{
   Bool_t ok;
//   TString name = "drawing";
   TString name = GetName();
   name += "_pict";
   name =
       GetString("Save picture with name", name.Data(), &ok,
                 (TGWindow*)fRootCanvas);
   if (!ok)
      return;
   if (OpenWorkFile(fRootCanvas)) {
      RemoveEditGrid();
      Write(name.Data());
 //     GetListOfPrimitives()->Write(name.Data(), 1);
      CloseWorkFile();
   }
}
//______________________________________________________________________________

void HTCanvas::GetPrimitives()
{
}
//______________________________________________________________________________

Int_t HTCanvas::ExtractGObjects()
{
   TCutG * cut = (TCutG *)FindObject("CUTG");
   if (!cut) {
      cout << "Define a graphical cut first" << endl;
      return -1;
   }
   Bool_t ok;
   TString name = "p1";
   name =
   GetString("Name of object (must be unique)", name.Data(), &ok,
        (TGWindow*)fRootCanvas);
   TString title = "tt";
   title =
   GetString("Title (used as comment)", title.Data(), &ok,
        (TGWindow*)fRootCanvas);

   GroupOfGObjects * gg = new GroupOfGObjects(name.Data(), title.Data(), 0. ,0.);
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
   Float_t xoff =  gg->fXLowEdge;
   Float_t yoff =  gg->fYLowEdge;
   TObject * obj;
   TIter next(GetListOfPrimitives());
   while ( (obj = next()) ) {
      if (obj == cut) continue;      // the enclosing TCutG itself
      if (obj->InheritsFrom("EditMarker")) continue; 

      if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj->Clone();
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX1(), b->GetY2())
            |cut->IsInside(b->GetX2(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
         }
         
      } else if (obj->InheritsFrom("TLine")){
         TLine * b = (TLine*)obj->Clone();
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
         }

      } else if (obj->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)obj->Clone();
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
         }

      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj->Clone();
         if (cut->IsInside(b->GetStartX(), b->GetStartY())
            |cut->IsInside(b->GetEndX(), b->GetEndY()) ) {
            b->SetStartPoint(b->GetStartX() - xoff, b->GetStartY() - yoff);
            b->SetEndPoint(b->GetEndX() - xoff, b->GetEndY() - yoff);
            gg->AddMember(b);
         }
      } else if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)obj->Clone();
//         if (cut->IsInside(b->GetX(), b->GetY()) ) {
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
            b->SetX(b->GetX() - xoff);
            b->SetY(b->GetY() - yoff);
            gg->AddMember(b);
         }

      } else if (obj->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)obj->Clone();
         if (cut->IsInside(b->GetX1(), b->GetY1()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetY1(b->GetY1() - yoff);
            gg->AddMember(b);
         }

      } else if (obj->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)obj->Clone();
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TPolyLine with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         if (cut->IsInside(x[0], y[0]) 
            |cut->IsInside(x[b->GetN()-1], y[b->GetN()-1])) {
         	for (Int_t i = 0; i < b->GetN(); i++) {
            	x[i] -= xoff;
            	y[i] -= yoff;
         	}
         } 
      } else {
         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
   if (cut) delete cut;  
   if (gg->GetNMembers() > 0) {
      if (!fGObjectGroups) fGObjectGroups = new TList();
      fGObjectGroups->Add(gg);
 
//      gg->Print();
      cout <<  gg->GetNMembers()<< " objects in list " << endl;
      return gg->GetNMembers();
   } else {
      cout << "No objects inside selection" << endl;
      return 0;
   }
     
}
//______________________________________________________________________________

void HTCanvas::InsertGObjects(const char * objname)
{
   if (!fGObjectGroups) {
      cout << "No Macro objects defined" << endl;
      return;
   }
   static TString defname = "p1";
   TString name;
   if (objname && strlen(objname) > 1) {
      name = objname;
   } else {
      Bool_t ok;
      name = defname;
      name =
      GetString("Name of object (must be unique)", name.Data(), &ok,
               (TGWindow*)fRootCanvas);
      if (!ok) return;
   }
   GroupOfGObjects * gg = (GroupOfGObjects *)fGObjectGroups->FindObject(name); 
   if (!gg) {
      cout << "Object " << name << " not found" << endl;
      return;
   }
	cout << "Mark position with left mouse" << endl;
	fGetMouse = kTRUE;
	while (fGetMouse == kTRUE) {
   	gSystem->ProcessEvents();
   	gSystem->Sleep(10);
	}
	cout << fMouseX << " " << fMouseY << endl;

   if (fInsertMacrosAsGroup) {
      TString pad_name(gg->GetName());
      pad_name += "_pad";
      Double_t dx = gg->fXUpEdge - gg->fXLowEdge;
      Double_t dy = gg->fYUpEdge - gg->fYLowEdge;
      this->cd();
      Double_t x1, x2, y1, y2;
      GetRange(x1, y1,x2, y2);
      Double_t dxc = x2 - x1;
      Double_t dyc = y2 - y1;

      cout <<"dx, dy "  << dx << " " << dy << endl;
      cout <<"dxc, dyc "  << dxc << " " << dyc << endl;
      TPad * pad = new TPad(pad_name, gg->GetTitle(), 
                            fMouseX / dxc, fMouseY / dyc, 
                           (fMouseX + dx) / dxc, (fMouseY + dy) / dyc);
      pad->Range(0, 0, dx, dy);
      pad->Draw();
      pad->cd();
   //   gg->AddMembersToList(this->GetListOfPrimitives(),fMouseX, fMouseY); 
//      SetAllCurlySizes(gg->GetMemberList(), dyc/dy, dyc/dy, kFALSE);
//      SetAllArrowSizes(gg->GetMemberList(), dyc/dy, kFALSE);
 //     SetAllTextSizes(gg->GetMemberList(), dyc/dy, kFALSE);
      gg->AddMembersToList(pad, 0., 0., dyc/dy); 
   } else {
      gg->AddMembersToList(this, fMouseX, fMouseY); 
   }
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
//         obj->Print();
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
         obj->Print();
         fGObjectGroups->Add(obj);
      }
   }
   infile->Close();
}
//______________________________________________________________________________

void HTCanvas::ShowGallery()
{
   if (!fGObjectGroups) {
      cout << "No graph macro objects defined" << endl;
      return;
   }
   Int_t n = fGObjectGroups->GetSize();
   new TDialogCanvas("GObjects", "Graphics macro objects",
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
//      cout << cmd << endl;
      b = new TButton("", cmd.Data(),x, y, x + dx, y + dy);
      Double_t xr = go->fXUpEdge - go->fXLowEdge;
      Double_t yr = go->fYUpEdge - go->fYLowEdge;
      b->Range(-0.1 * xr, -0.1 * yr , 1.2 * xr, 1.2 * yr);
      tt = new TText(0,0, "");
      TList * lop = b->GetListOfPrimitives();
      lop->Add(tt);
      go->AddMembersToList(b, 0, 0, 5);
//      SetAllCurlySizes(lop, 5, 5, kFALSE);
//      SetAllArrowSizes(lop, 5, kFALSE);
//      SetAllTextSizes(lop, 5, kFALSE);
      tt = new TText();
      tt->SetText(0.1 * xr, 0.5 *yr, oname.Data());
      tt->SetTextSize(0.25);
      lop->Add(tt);
      b->Draw();
      y = y + dy + marg;
      if (y > 1 - dy - marg) {
         x = x + dx + marg;
         y = marg;
      }
   }
}
