namespace std {} using namespace std;

#include "GroupOfGObjects.h"
#include "TBox.h"
#include "TPave.h"
#include "TLine.h"
#include "TCurlyLine.h"
#include "TCurlyArc.h"
#include "TArrow.h"
#include "TText.h"
#include "TEllipse.h"
#include "TPolyLine.h"
#include "iostream"
//________________________________________________________________

Double_t XtoNDC(TPad *pad, Double_t x)
{
   Double_t x1, x2, y1, y2;
   pad->GetRange(x1, y1, x2, y2);
   return (x - x1) / (x2 - x1);
};
Double_t XLtoNDC(TPad *pad, Double_t x)
{
   Double_t x1, x2, y1, y2;
   pad->GetRange(x1, y1, x2, y2);
   return x / (x2 - x1);
};

Double_t YtoNDC(TPad *pad,Double_t  y)
{
   Double_t x1, x2, y1, y2;
   pad->GetRange(x1, y1, x2, y2);
   return (y - y1) / (y2 - y1);
};
Double_t YLtoNDC(TPad *pad, Double_t y)
{
   Double_t x1, x2, y1, y2;
   pad->GetRange(x1, y1, x2, y2);
   return y / (y2 - y1);
};
//________________________________________________________________

ClassImp(GroupOfGObjects)
//________________________________________________________________

GroupOfGObjects::GroupOfGObjects(const char * name, Double_t x, Double_t y, TCutG* cut)
{
   fXorigin = x;
   fYorigin = y;
   fIsVisible = kTRUE;
   SetName(name);
   if (cut) SetEnclosingCut(cut);
   gROOT->GetListOfCleanups()->Add(this);
};
//________________________________________________________________

GroupOfGObjects::~GroupOfGObjects()
{ 
   fMembers.Clear();
   gPad->GetListOfPrimitives()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
};

//________________________________________________________________
void GroupOfGObjects::RecursiveRemove(TObject * obj)
{
//   cout << "GroupOfGObjects::RecursiveRemove " << obj <<  endl;
   fMembers.Remove(obj);
};
//________________________________________________________________
void GroupOfGObjects::Print()
{
   cout << ClassName() << " --- " << GetName() << " --- " <<  GetName() << endl;
   TIter next(&fMembers);
   TObject * obj;
   while ( (obj = next()) ) obj->Print();
   cout << "-------------------------------------------------------- "<< endl;
};
//________________________________________________________________

void GroupOfGObjects::AddMember(TObject * obj) 
{  
   fMembers.Add(obj); 
   obj->SetBit(kIsBound);
//   fEnclosingCut->GetLofGObjects()->Add(obj);
};
//________________________________________________________________

Bool_t GroupOfGObjects::BindMember(TObject * obj)
{
   TObject * o;
   o = fMembers.FindObject(obj);
   if (o) {o->SetBit(kIsBound); return kTRUE;}
   else   return kFALSE;
};
//________________________________________________________________

Bool_t GroupOfGObjects::FreeMember(TObject * obj)
{
   TObject * o;
   o = fMembers.FindObject(obj);
   if (o) {o->ResetBit(kIsBound); return kTRUE;}
   else   return kFALSE;
};
//________________________________________________________________

void GroupOfGObjects::BindReleaseObjects(Bool_t bind)
{
   TIter next(&fMembers);
   TObject * obj;
   while ( (obj = next()) ) { 
      if (bind) obj->SetBit(GroupOfGObjects::kIsBound);
      else      obj->ResetBit(GroupOfGObjects::kIsBound);
   }
};
//________________________________________________________________

void GroupOfGObjects::DeleteObjects()
{
   TIter next(&fMembers);
   TObject * obj;
   while ( (obj = next()) ) {
      
      delete obj;
   }
   delete this;
};
//________________________________________________________________

void GroupOfGObjects::SetEnclosingCut(TCutG *cut)
{
   if (!cut) return;
   Double_t * xc = cut->GetX();
   Double_t * yc = cut->GetY();
   Set(cut->GetN());
   Double_t * x = GetX();
   Double_t * y = GetY();
   for (Int_t i = 0; i < GetN(); i++) {
      x[i] = xc[i] - fXorigin;
      y[i] = yc[i] - fYorigin;
   }
   SetBit(kIsEnclosingCut);
   SetLineWidth(1);
   SetLineStyle(3);
}
//________________________________________________________________

Int_t GroupOfGObjects::AddMembersToList(TPad * pad, Double_t xoff, Double_t yoff,
                                        Double_t scaleNDC, Double_t scaleG, 
                                        Double_t angle, Int_t align, Int_t draw_cut)
{
////////////////////////////////////////////////////////////////////
//
//  Add members to an external list, typical a ListOfPrimitives
//  scaleNDCX, Y apply to TextSizes etc. 
//  applying a scale factor (scaleG), a tranlation and rotation
//
////////////////////////////////////////////////////////////////////
   TIter next(&fMembers);
   TObject * obj;
   TObject * clone;
   TList * list = pad->GetListOfPrimitives();
   Double_t xt, yt;
   TList * list_in_cut = 0;
//   cout << "AddMembersToList: " << xoff << " " << yoff << " " << angle  << " "<< align << endl;
//   pad->cd();
//   Inspect();
   if (draw_cut != 0 && this->GetN()){
      clone = this->Clone();
      GroupOfGObjects * cut = (GroupOfGObjects*)clone;
      list_in_cut = cut->GetMemberList();
      list_in_cut->Clear();
      Double_t * x = cut->GetX();
      Double_t * y = cut->GetY();
      for (Int_t i = 0; i < cut->GetN(); i++) {
         Transform(x[i], y[i], xoff,yoff, scaleG, angle, align, &xt, &yt);  
         x[i] = xt;
         y[i] = yt;
      }
      cut->SetLineWidth(1);
      cut->SetLineStyle(2);
      list->Add(cut);
//     cout << " AddMembersToList: cut " << cut << endl << flush;
//      cut->Draw();
   }
   while ( (obj = next()) ) {
      clone = obj->Clone();
      obj->SetBit(kIsBound);
//      cout << clone->ClassName() << endl; 
      list->Add(clone);
      if (list_in_cut)list_in_cut->Add(clone);
//      if (clone->InheritsFrom("TPave")) {
//         TPave * b = (TPave*)clone;
//         b->SetX1NDC(XtoNDC(pad, b->GetX1()) + XLtoNDC(pad, xoff));
//         b->SetX2NDC(XtoNDC(pad, b->GetX2()) + XLtoNDC(pad, xoff));
//         b->SetY1NDC(YtoNDC(pad, b->GetY1()) + YLtoNDC(pad, yoff));
//         b->SetY2NDC(YtoNDC(pad, b->GetY2()) + YLtoNDC(pad, yoff));

      if (clone->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)clone;
         Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX1(xt);
         b->SetY1(yt);
         Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX2(xt);
         b->SetY2(yt);
         b->SetArrowSize(b->GetArrowSize() * scaleNDC);

//         cout << "SetArrowSize: " << b->GetArrowSize() * scaleNDC << endl;

      } else if (clone->InheritsFrom("TCurlyArc")) {
         TCurlyArc* b = (TCurlyArc*)clone;
         Transform(b->GetStartX(), b->GetStartY(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetCenter(xt, yt);
         b->SetRadius(scaleG * b->GetRadius());
         Double_t phi = b->GetPhimin() + angle;
         if (phi > 360) phi -= 360; 
         b->SetPhimin(phi);
         phi = b->GetPhimax() + angle;
         if (phi > 360) phi -= 360; 
         b->SetPhimax(phi);
         b->Paint(b->GetDrawOption());
      } else if (clone->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)clone;
         Transform(b->GetStartX(), b->GetStartY(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetStartPoint(xt, yt);
         Transform(b->GetEndX(), b->GetEndY(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetEndPoint(xt, yt);
         b->SetWaveLength(b->GetWaveLength() * scaleNDC);
         b->SetAmplitude(b->GetAmplitude() * scaleNDC);
         b->Paint(b->GetDrawOption());
      } else if (clone->InheritsFrom("TText")) {
         TText * b = (TText*)clone;
         Transform(b->GetX(), b->GetY(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX(xt);
         b->SetY(yt);
         b->SetTextSize(b->GetTextSize() * scaleNDC);
        
      } else if (clone->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)clone;
         Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetR1(scaleG * b->GetR1());
         b->SetR2(scaleG * b->GetR2());
         if (b->GetPhimin() != 0 || b->GetPhimax() != 360) {
            Double_t phi = b->GetPhimin() + angle;
            if (phi > 360) phi -= 360; 
            b->SetPhimin(phi);
            phi = b->GetPhimax() + angle;
            if (phi > 360) phi -= 360; 
            b->SetPhimax(phi);
         }
         b->SetX1(xt);
         b->SetY1(yt);

      } else if (clone->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)clone;
         Double_t * x = b->GetX();
         Double_t * y = b->GetY();
         for (Int_t i = 0; i < b->GetN(); i++) {
            Transform(x[i], y[i], xoff,yoff, scaleG, angle, align, &xt, &yt);  
            x[i] = xt;
            y[i] = yt;
         }
      } else if (clone->InheritsFrom("TBox")) {
         TBox * b = (TBox*)clone;
         Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX1(xt);
         b->SetY1(yt);
         Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX2(xt);
         b->SetY2(yt);

      } else if (clone->InheritsFrom("TLine")){
         TLine * b = (TLine*)clone;
         Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX1(xt);
         b->SetY1(yt);
         Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, angle, align, &xt, &yt);  
         b->SetX2(xt);
         b->SetY2(yt);

      } else {
         cout << clone->ClassName() << " not yet implemented" << endl;
      }
   }     
   return 1;
};
//____________________________________________________________________________

void  GroupOfGObjects::Transform(Double_t x, Double_t y, Double_t xoff, Double_t yoff,
               Double_t scaleG, Double_t angle, Int_t  align, Double_t* xt, Double_t* yt)
{
   Double_t xx = scaleG * x;
   Double_t yy = scaleG * y;
   *xt = xx; 
   *yt = yy;
   if (angle != 0) {
      Double_t ar = angle * TMath::Pi() / 180.;
      *xt = xx * TMath::Cos(ar) - yy * TMath::Sin(ar);
      *yt = xx * TMath::Sin(ar) + yy * TMath::Cos(ar);
   }
//   xx += xoff;
//  yy += yoff;
   *xt += xoff;
   *yt += yoff;
}
