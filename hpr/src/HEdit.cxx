#include "TArc.h"
#include "TArrow.h"
#include "TDialogCanvas.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "TEllipse.h"
#include "TEnv.h"
#include "TRegexp.h"
#include "TImage.h"
#include "TAttImage.h"
#include "TContextMenu.h"

#include "HistPresent.h"
#include "support.h"
#include "HTCanvas.h"
#include "HandleMenus.h"

#include "TGMrbInputDialog.h"
#include "TMrbString.h"
#include "TGMrbTableFrame.h"
#include "EditMarker.h"
#include "GroupOfGObjects.h"
#include "HprImage.h"
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
//    cout << ":DrawEditGrid " << dx << " " << dy << endl;
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

void HTCanvas::SetVisibilityOfEnclosingCuts(Bool_t visible)
{
   TIter next(GetListOfPrimitives());
   TObject * obj;
   while ( (obj = next()) ) {
      if (obj->TestBit(GroupOfGObjects::kIsEnclosingCut))
        ((GroupOfGObjects*)obj)->SetVisible(visible);  
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
   cout << "this, pad: " << this << " " << pad << endl;
   if (pad && GetListOfPrimitives()->Contains(pad)) {
      Bool_t ok;
      TString name = "picture.jpg";
      name =
         GetString("Picture name", name.Data(), &ok,
                 (TGWindow*)fRootCanvas);
      if (!ok) return;

//      TImage *img = TImage::Open(name.Data());
      HprImage * hprimg = new HprImage(name.Data());
      TImage *img = hprimg->GetImage();
      if (!img) {
         cout << "Could not create an image... exit" << endl;
         return;
      }

      cout << "InsertImage(): " <<  pad->GetXlowNDC() << " " << pad->GetYlowNDC() << " "
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

      pad->SetTopMargin(.02);
      pad->SetBottomMargin(0.02);
      pad->SetLeftMargin(0.02);
      pad->SetRightMargin(0.02);

//      img->SetConstRatio(kTRUE);
//      img->SetEditable(kTRUE);
//      img->SetImageQuality(TAttImage::kImgBest);

      hprimg->Draw("xxx");
//      hprimg->Paint();
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

Int_t HTCanvas::ExtractGObjects(Bool_t markonly)
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
//   TString title = "tt";
//   title =
//   GetString("Title (used as comment)", title.Data(), &ok,
//        (TGWindow*)fRootCanvas);

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
   TList * lenc = gg->GetMemberList();
   TObject * obj;
   TIter next(GetListOfPrimitives());
   while ( (obj = next()) ) {
      if (obj == cut) continue;      // the enclosing TCutG itself
      if (obj->InheritsFrom("EditMarker")) continue; 
      TObject * clone;
      if (markonly) clone = obj;
      else          clone = obj->Clone();
//      obj->Print();
      if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)clone;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX1(), b->GetY2())
            |cut->IsInside(b->GetX2(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }
         
      } else if (obj->InheritsFrom("TLine")){
         TLine * b = (TLine*)clone;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }

      } else if (obj->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)clone;
         if (cut->IsInside(b->GetX1(), b->GetY1())
            |cut->IsInside(b->GetX2(), b->GetY2()) ) {
            b->SetX1(b->GetX1() - xoff);
            b->SetX2(b->GetX2() - xoff);
            b->SetY1(b->GetY1() - yoff);
            b->SetY2(b->GetY2() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }

      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)clone;
         if (cut->IsInside(b->GetStartX(), b->GetStartY())
            |cut->IsInside(b->GetEndX(), b->GetEndY()) ) {
            b->SetStartPoint(b->GetStartX() - xoff, b->GetStartY() - yoff);
            b->SetEndPoint(b->GetEndX() - xoff, b->GetEndY() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }
      } else if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)clone;
//         if (cut->IsInside(b->GetX(), b->GetY()) ) {
         if (SloppyInside(cut, b->GetX(), b->GetY()) ) {
            b->SetX(b->GetX() - xoff);
            b->SetY(b->GetY() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }

      } else if (obj->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)clone;
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
            b->SetX1(b->GetX1() - xoff);
            b->SetY1(b->GetY1() - yoff);
            gg->AddMember(b);
            if (markonly)lenc->Add(b);
         }

      } else if (obj->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)clone;
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
         gg->AddMember(b);
         if (markonly)lenc->Add(b);
      } else {
         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
   if (cut) delete cut;  
   cout <<  gg->GetNMembers()<< " objects in list " << endl;
   if (gg->GetNMembers() > 0) {
      if (!markonly) {
         if (!fGObjectGroups) fGObjectGroups = new TList();
         fGObjectGroups->Add(gg);
         ShowGallery();
      } else {
         gg->Draw();
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
   static TString defname = "p1";
   TOrdCollection *row_lab = new TOrdCollection(); 
   TOrdCollection *values  = new TOrdCollection();
   row_lab->Add(new TObjString("Name of object"));
   row_lab->Add(new TObjString("Scale factor NDC"));
   row_lab->Add(new TObjString("Scale factor User"));
   row_lab->Add(new TObjString("Angle[deg]"));
   row_lab->Add(new TObjString("Align (22 cent)"));
   row_lab->Add(new TObjString("X value"));
   row_lab->Add(new TObjString("Y value"));
   row_lab->Add(new TObjString("Draw enclosing cut"));

   static Double_t scaleNDC = 1;
   static Double_t scaleU = 1;
   static Double_t angle = 0;
   static Int_t align = 22;
   Double_t x0 = 0;
   Double_t y0 = 0;
   static Int_t draw_cut = 1;

   TMrbString temp;
   TString name;
   if (objname && strlen(objname) > 1) {
      name = objname;
   } else {
      name = defname;
   }

   values->Add(new TObjString(name.Data()));
   values->Add(new TObjString(Form("%lf", scaleNDC)));
   values->Add(new TObjString(Form("%lf", scaleU)));
   values->Add(new TObjString(Form("%lf", angle)));
   values->Add(new TObjString(Form("%d", align)));
   values->Add(new TObjString(Form("%lf", x0)));
   values->Add(new TObjString(Form("%lf", y0)));
   if (draw_cut != 0) 
      values->Add(new TObjString("CheckButton_Down"));
   else
      values->Add(new TObjString("CheckButton_Up"));

   Int_t ret,  itemwidth=120, nrows = values->GetSize(); 
tryagain:
   new TGMrbTableFrame((TGWindow*)fRootCanvas, &ret, "", 
                        itemwidth, 1, nrows, values,
                        0, row_lab);
   if (ret < 0) {
      return;
   }
   name = ((TObjString*)values->At(0))->GetString();
   temp = ((TObjString*)values->At(1))->GetString();
   if (!temp.ToDouble(scaleNDC)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(2))->GetString();
   if (!temp.ToDouble(scaleU)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(3))->GetString();
   if (!temp.ToDouble(angle)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(4))->GetString();
   if (!temp.ToInteger(align)) {
      cout << "Illegal integer: " << temp << endl;
      goto tryagain;      
   }
   if (align < 11 || align > 33) {
      cout << "Illegal alignment " << temp << " should be: 11-33" << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(5))->GetString();
   if (!temp.ToDouble(x0)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(6))->GetString();
   if (!temp.ToDouble(y0)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(7))->GetString();
   if (temp.EndsWith("Down")) draw_cut = 1;
   else                       draw_cut = 0;

   GroupOfGObjects * gg = (GroupOfGObjects *)fGObjectGroups->FindObject(name); 
   if (!gg) {
      cout << "Object " << name << " not found" << endl;
      goto tryagain;
   }
   if (x0 == 0 && y0 == 0) {
   	cout << "Mark position with left mouse" << endl;
   	fGetMouse = kTRUE;
	   while (fGetMouse == kTRUE) {
   	   gSystem->ProcessEvents();
   	   gSystem->Sleep(10);
	   }
	   cout << fMouseX << " " << fMouseY << endl;
      x0 = fMouseX;
      y0 = fMouseY;
   }
/*
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
*/
   this->cd();
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
      Double_t xoff = 0.5;
      Double_t yoff = 0.5;
      if (go->fXorigin != 0 || go->fYorigin) {
         xoff = (go->fXorigin - go->fXLowEdge) / xr;
         yoff = (go->fYorigin - go->fYLowEdge) / yr;
      }
      b->Range(-(xoff+0.1)*xr, -(yoff+0.1)*yr, (xoff+0.1)*xr, (yoff +0.1)*yr);
      tt = new TText(0,0, "");
      TList * lop = b->GetListOfPrimitives();
      lop->Add(tt);
      go->AddMembersToList(b, 0, 0, 5);
//      SetAllCurlySizes(lop, 5, 5, kFALSE);
//      SetAllArrowSizes(lop, 5, kFALSE);
//      SetAllTextSizes(lop, 5, kFALSE);
      tt = new TText();
      tt->SetText(-.4 * xr, -.5 *yr, oname.Data());
      tt->SetTextSize(0.15);
      lop->Add(tt);
      b->Draw();
      y = y + dy + marg;
      if (y > 1 - dy - marg) {
         x = x + dx + marg;
         y = marg;
      }
   }
}
//______________________________________________________________________________

void HTCanvas::ShiftObjects(TList* list, Double_t xoff, Double_t yoff)
{
//   cout << "ShiftObjects " << list << " " << xoff << " " << yoff << endl;
//  if (list) list->Print();
   TObject * obj;
   TIter next(list);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("EditMarker")) continue; 
//      obj->Print();
      if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);
         
      } else if (obj->InheritsFrom("TLine")){
      TLine * b = (TLine*)obj;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);

      } else if (obj->InheritsFrom("TArrow")) {
      TArrow * b = (TArrow*)obj;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);

      } else if (obj->InheritsFrom("TCurlyArc")) {
         TCurlyArc * b = (TCurlyArc*)obj;
         b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);

      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj;
         b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);
         b->SetEndPoint(b->GetEndX() + xoff, b->GetEndY() + yoff);

      } else if (obj->InheritsFrom("TText")) {
      TText * b = (TText*)obj;
         b->SetX(b->GetX() + xoff);
         b->SetY(b->GetY() + yoff);
      } else if (obj->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)obj;
         b->SetX1(b->GetX1() + xoff);
         b->SetY1(b->GetY1() + yoff);

      } else if (obj->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TPolyLine with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            x[i] += xoff;
            y[i] += yoff;
         }
      } else if (obj->InheritsFrom("TGraph")) {
         TGraph* b = (TGraph*)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            x[i] += xoff;
            y[i] += yoff;
         }
      } else {
         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
}
//______________________________________________________________________________

void HTCanvas::PutObjectsOnGrid(TList* list)
{
   TObject * obj;
   TList * lof;
   if (list) lof = list;
   else      lof = GetListOfPrimitives(); 
   TIter next(lof);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("EditMarker")) continue; 
//      obj->Print();
      if (obj->InheritsFrom("TBox")) {
         TBox * b = (TBox*)obj;
         b->SetX1(PutOnGridX(PutOnGridX(b->GetX1())));
         b->SetX2(PutOnGridX(b->GetX2()));
         b->SetY1(PutOnGridY(b->GetY1()));
         b->SetY2(PutOnGridY(b->GetY2()));
         
      } else if (obj->InheritsFrom("TLine")){
      TLine * b = (TLine*)obj;
         b->SetX1(PutOnGridX(b->GetX1()));
         b->SetX2(PutOnGridX(b->GetX2()));
         b->SetY1(PutOnGridY(b->GetY1()));
         b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("TArrow")) {
      TArrow * b = (TArrow*)obj;
         b->SetX1(PutOnGridX(b->GetX1()));
         b->SetX2(PutOnGridX(b->GetX2()));
         b->SetY1(PutOnGridY(b->GetY1()));
         b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("TCurlyArc")) {
         TCurlyArc * b = (TCurlyArc*)obj;
         b->SetStartPoint(PutOnGridX(b->GetStartX()), PutOnGridY(b->GetStartY()));
  
      } else if (obj->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)obj;
         b->SetStartPoint(PutOnGridX(b->GetStartX()), PutOnGridY(b->GetStartY()));
         b->SetEndPoint(PutOnGridX(b->GetEndX()), PutOnGridY(b->GetEndY()));

      } else if (obj->InheritsFrom("TText")) {
      TText * b = (TText*)obj;
         b->SetX(b->GetX());
         b->SetY(b->GetY());
      } else if (obj->InheritsFrom("TArc")) {
         TArc * b = (TArc*)obj;
         b->SetX1(PutOnGridX(b->GetX1()));
         b->SetY1(PutOnGridY(b->GetY1()));
//         b->SetX2(PutOnGridX(b->GetX2()));
//         b->SetY2(PutOnGridY(b->GetY2()));

      } else if (obj->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TPolyLine with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            x[i] = PutOnGridX(x[i]);
            y[i] = PutOnGridY(y[i]);
         }
      } else if (obj->InheritsFrom("TGraph")) {
         TGraph* b = (TGraph*)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TGraph with 0 points" << endl;
            continue;
         }
         Double_t * y = b->GetY();
//         either first or last point
         for (Int_t i = 0; i < b->GetN(); i++) {
            x[i] = PutOnGridX(x[i]);
            y[i] = PutOnGridY(y[i]);
         }
      } else {
         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
}
//______________________________________________________________________________

void HTCanvas::DeleteObjects()
{
   TCutG * cut = (TCutG *)FindObject("CUTG");
   if (!cut) {
      cout << "Define a graphical cut first" << endl;
      return;
   }
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
      } else if (obj->InheritsFrom("TText")) {
         TText * b = (TText*)obj;
//         if (cut->IsInside(b->GetX(), b->GetY()) ) {
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

      } else if (obj->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)obj;
         Double_t * x = b->GetX();
         if (!x) {
            cout << "TPolyLine with 0 points" << endl;
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
         cout << obj->ClassName() << " not yet implemented" << endl;
      }
   }   
   if (cut) delete cut;  
}
//______________________________________________________________________________

void HTCanvas::Latex2Root()
{
const Char_t helpText[] =
"Text alignment convention: \n\
13     23    33 \n\
\n\
12     22    32 \n\
\n\
11     21    31";

  
 
// this tries to translate standard Latex into ROOTs
// latex like formular processor TLatex format

   TOrdCollection *row_lab = new TOrdCollection(); 
   TOrdCollection *values  = new TOrdCollection();
   row_lab->Add(new TObjString("File Name with Latex"));
   row_lab->Add(new TObjString("X Position"));
   row_lab->Add(new TObjString("Y Position"));
   row_lab->Add(new TObjString("Line spacing"));
   row_lab->Add(new TObjString("Text alignment (see Help)"));
   static Double_t x0 = 20;
   static Double_t y0 = 250;
   static Double_t dy = 10;
   static Int_t align = 11; // lower left
   TMrbString temp;
   static TString fname = "latex.txt";
   values->Add(new TObjString(fname.Data()));
   values->Add(new TObjString(Form("%lf", x0)));
   values->Add(new TObjString(Form("%lf", y0)));
   values->Add(new TObjString(Form("%lf", dy)));
   values->Add(new TObjString(Form("%d",  align)));

   Int_t ret,  itemwidth=120, nrows = values->GetSize(); 
tryagain:
   new TGMrbTableFrame((TGWindow*)fRootCanvas, &ret, "", 
                        itemwidth, 1, nrows, values,
                        0, row_lab, 0, 0, helpText);
   if (ret < 0) {
      return;
   }
   fname = ((TObjString*)values->At(0))->GetString();

   temp = ((TObjString*)values->At(1))->GetString();
   if (!temp.ToDouble(x0)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(2))->GetString();
   if (!temp.ToDouble(y0)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(3))->GetString();
   if (!temp.ToDouble(dy)) {
      cout << "Illegal double: " << temp << endl;
      goto tryagain;      
   }
   temp = ((TObjString*)values->At(4))->GetString();
   if (!temp.ToInteger(align)) {
      cout << "Illegal integer: " << temp << endl;
      goto tryagain;      
   }
   ifstream infile(fname);
 
   TString line;
   TString cmd; 
   Int_t ind;
   TString rep; 
   TLatex  * latex;
   Double_t xt = x0;
   Double_t yt = y0;

   while(1) {
// read lines, concatinate lines ending with 
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
//      cout << cmd << endl;
//    remove latex's $ (mathstyle), replace \ by #
      Int_t sind = 0;
      while (cmd.Index("$") >=0) cmd.Remove(cmd.Index("$"));
      while (cmd.Index("\\") >=0) cmd(cmd.Index("\\")) = '#';
      TRegexp oper("[-+*/=]");
//    make sure super / sub scripts are enclosed in {}
      TRegexp supsub("[_^]");
 //     cout << "Before supsub: " << cmd << endl;
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
//         cout << cmd << endl;
      }
//      cout << "Before oper: " << cmd << endl;
//   add space around operators 
      sind = 0;
      while (cmd.Index(oper, sind) >=0) {
         ind = 1 + cmd.Index(oper, sind);
         char c = cmd[ind];
//         cout << "ind " << ind << endl;
         sind = ind + 2;
         if (c != ' ' && c!= '{' && c!= '}') {
            
            cmd.Insert(ind," ");
            sind += 1;
         }
         c = cmd[ind - 2];
         if (c != ' ' && c!= '{' && c!= '}') { 
            cmd.Insert(ind-1," ");
            sind += 1;
         }
  //        cout << cmd << endl;
      }
//   remove not used \cos etc,
//   replace overline by bar

      TRegexp re_cos("#cos");
      TRegexp re_sin("#sin");
      TRegexp re_tan("#tan");
      TRegexp re_ovl("#overline");
      while (cmd.Index(re_cos) >= 0)cmd(re_cos) = "cos";
      while (cmd.Index(re_sin) >= 0)cmd(re_sin) = "sin";
      while (cmd.Index(re_tan) >= 0)cmd(re_tan) = "tan";
      while (cmd.Index(re_ovl) >= 0)cmd(re_ovl) = "#bar";
//      cout << "Final:     " << cmd << endl;
      latex = new TLatex(xt, yt, cmd.Data());
      latex->SetTextAlign(align);
      latex->Draw();
      yt -= dy;
//      outfile << cmd << endl;
      cmd.Resize(0);
   }
   Modified();
   Update();
}
