namespace std {} using namespace std;

#include "GroupOfGObjects.h"
#include "TBox.h"
#include "TPave.h"
#include "TLine.h"
#include "TCurlyLine.h"
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

GroupOfGObjects::GroupOfGObjects(const char * name, const char * title,
                                Double_t x, Double_t y) : TNamed(name, title)
{
   fXorig = x;
   fYorig = y;
};
//________________________________________________________________

GroupOfGObjects::~GroupOfGObjects(){};
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

Bool_t GroupOfGObjects::BindMember(TObject * obj)
{
   TObject * o;
   o = fMembers.FindObject(obj);
   if (o) {o->SetBit(kIsFree); return kTRUE;}
   else   return kFALSE;
};
//________________________________________________________________

Bool_t GroupOfGObjects::FreeMember(TObject * obj)
{
   TObject * o;
   o = fMembers.FindObject(obj);
   if (o) {o->ResetBit(kIsFree); return kTRUE;}
   else   return kFALSE;
};
//________________________________________________________________

Int_t GroupOfGObjects::AddMembersToList(TPad * pad, 
                                  Double_t xoff, Double_t yoff,
                                  Double_t scaleNDC,
                                  Double_t scaleG,
                                  Double_t angle, Bool_t all)
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
   while ( (obj = next()) ) {
      clone = obj->Clone();
//      cout << clone->ClassName() << endl;
      list->Add(clone);
      if (clone->InheritsFrom("TPave")) {
         TPave * b = (TPave*)clone;
         b->SetX1NDC(XtoNDC(pad, b->GetX1()) + XLtoNDC(pad, xoff));
         b->SetX2NDC(XtoNDC(pad, b->GetX2()) + XLtoNDC(pad, xoff));
         b->SetY1NDC(YtoNDC(pad, b->GetY1()) + YLtoNDC(pad, yoff));
         b->SetY2NDC(YtoNDC(pad, b->GetY2()) + YLtoNDC(pad, yoff));

      } else if (clone->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)clone;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);
         b->SetArrowSize(b->GetArrowSize() * scaleNDC);

//         cout << "SetArrowSize: " << b->GetArrowSize() * scaleNDC << endl;

      } else if (clone->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)clone;
         b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);
         b->SetEndPoint(b->GetEndX() + xoff, b->GetEndY() + yoff);
         b->SetWaveLength(b->GetWaveLength() * scaleNDC);
         b->SetAmplitude(b->GetAmplitude() * scaleNDC);

      } else if (clone->InheritsFrom("TText")) {
         TText * b = (TText*)clone;
         b->SetX(b->GetX() + xoff);
         b->SetY(b->GetY() + yoff);
         b->SetTextSize(b->GetTextSize() * scaleNDC);
        
      } else if (clone->InheritsFrom("TEllipse")) {
         TEllipse * b = (TEllipse*)clone;
         b->SetX1(b->GetX1() + xoff);
         b->SetY1(b->GetY1() + yoff);
      } else if (clone->InheritsFrom("TPolyLine")) {
         TPolyLine * b = (TPolyLine *)clone;
         Double_t * x = b->GetX();
         Double_t * y = b->GetY();
         for (Int_t i = 0; i < b->GetN(); i++) {
            x[i] += xoff;
            y[i] += yoff;
         }
      } else if (clone->InheritsFrom("TBox")) {
         TBox * b = (TBox*)clone;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);

      } else if (clone->InheritsFrom("TLine")){
         TLine * b = (TLine*)clone;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);

      } else {
         cout << clone->ClassName() << " not yet implemented" << endl;
      }
   }     
   return 1;
};
