namespace std {} using namespace std;

#include "GroupOfGObjects.h"
#include "TBox.h"
#include "TLine.h"
#include "TCurlyLine.h"
#include "TArrow.h"
#include "TText.h"
#include "TEllipse.h"
#include "TPolyLine.h"
#include "iostream"


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

Int_t GroupOfGObjects::AddMembersToList(TList * list, 
                                  Double_t xoff, Double_t yoff,
                                  Double_t scale, Double_t angle, Bool_t all)
{
////////////////////////////////////////////////////////////////////
//
//  Add members to an external list, typical a ListOfPrimitives
//  applying a scale factor, a tranlation and rotation
//
////////////////////////////////////////////////////////////////////
   TIter next(&fMembers);
   TObject * obj;
   TObject * clone;
   while ( (obj = next()) ) {
      clone = obj->Clone();
      list->Add(clone);
      if (clone->InheritsFrom("TBox")) {
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
      } else if (clone->InheritsFrom("TArrow")) {
         TArrow * b = (TArrow*)clone;
         b->SetX1(b->GetX1() + xoff);
         b->SetX2(b->GetX2() + xoff);
         b->SetY1(b->GetY1() + yoff);
         b->SetY2(b->GetY2() + yoff);
      } else if (clone->InheritsFrom("TCurlyLine")) {
         TCurlyLine * b = (TCurlyLine*)clone;
         b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);
         b->SetEndPoint(b->GetEndX() + xoff, b->GetEndY() + yoff);
      } else if (clone->InheritsFrom("TText")) {
         TText * b = (TText*)clone;
         b->SetX(b->GetX() + xoff);
         b->SetY(b->GetY() + yoff);
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
      } else {
         cout << clone->ClassName() << " not yet implemented" << endl;
      }
   }     
   return 1;
};
