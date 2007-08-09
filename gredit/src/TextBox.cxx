#include "TROOT.h"
#include "TPad.h"
#include "TGraph.h"
#include "TMath.h"
#include "TText.h"
#include "TSystem.h"
#include "TSplineX.h"
#include "TextBox.h"
#include "HTCanvas.h"

ClassImp(TextBoxMember)
ClassImp(TextBox)
ClassImp(TbTimer)

//____________________________________________________________________________
TbTimer::TbTimer(Long_t ms, Bool_t synch,  TextBox *textbox):TTimer(ms,synch)
{
   fTextBox = textbox;
    cout << "init TbTimer " << ms << endl;
   gSystem->AddTimer(this);
};
//_____________________________________________________________________________
//TbTimer::~TbTimer() {}
//_____________________________________________________________________________

Bool_t TbTimer::Notify() {
 //  cout << " Notify reached" << endl;
   fTextBox->Wakeup();
   Reset();
   return kTRUE;
};

Double_t distance(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   return TMath::Sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}
//_________________________________________________________________________

TextBox::TextBox(Double_t x1, Double_t y1, Double_t x2, Double_t y2,const char *name)
   : TPave(x1 ,y1, x2, y2) 
{
   if (name) SetName(name);
   fTextMargin = 0.25;
   fSmall = 0.001 *(x2-x1);
   fPrimitives = 0;
   SetBit(kMustCleanup);
   gROOT->GetListOfCleanups()->Add(this);
   if (gPad) {
       HTCanvas* hc = (HTCanvas*)gPad;
       hc->Add2ConnectedClasses(this);
   }
   fTimer = new TbTimer(100, kTRUE, this);
};  
//_________________________________________________________________________
            
TextBox::~TextBox() 
{
   cout << "dtor TextBox" << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   delete fTimer;
};
//_________________________________________________________________________

void TextBox::RecursiveRemove(TObject * obj)
{
   TIter next(&fMembers);
   TextBoxMember *tbm;
   while ( (tbm = (TextBoxMember*)next()) ) {
      if (tbm->GetObject() == obj) {
         cout << "TextBox::RecursiveRemove " << obj << " " << obj->ClassName() << endl;
         fMembers.Remove(tbm);
         delete tbm;
         fPrimitives--;
         break;  
      }
   }
};
//_________________________________________________________________________

void TextBox::ObjCreated(Int_t px, Int_t py, TObject *obj)
{
   cout << "TextBox::ObjCreated() obj, px, py " << obj->ClassName() << " " << px << " " << py << endl;
//   AdoptMember();
}
//_________________________________________________________________________

void TextBox::ObjMoved(Int_t px, Int_t py, TObject *obj)
{
   if (!FindMember(obj)) return; 
   cout << "TextBox::Moved() obj, px, py " << obj->ClassName() << " " << px << " " << py << endl;
   AdoptMember(obj);
}

//_________________________________________________________________________

void TextBox::AdoptAll()
{
   cout << "TextBox::AdoptAll()" << endl;
   TIter next(gPad->GetListOfPrimitives());
   TObject *obj;
   while( (obj = next()) ) {
      if (obj == this)                       continue;
      if (obj->InheritsFrom("EditMarker"))   continue;
      if (obj->InheritsFrom("ControlGraph")) continue;
      AdoptMember(obj);
   }
}
//_________________________________________________________________________

void TextBox::AdoptMember(TObject * obj)
{
   cout << "TextBox::AdoptMember()" << endl;
	TText  *t = NULL;
	TGraph *g = NULL;
	TPave  *p = NULL;
   Bool_t mod      = kFALSE;
	cout << "try AdoptMember() " << obj << " " <<  obj->ClassName() << endl;
	if (obj->InheritsFrom("TSplineX")) {
		g = ((TSplineX*)obj)->GetControlGraph();
	} else if (obj->InheritsFrom("TText")) {
		t =(TText*)obj;
	} else if (obj->InheritsFrom("TGraph")) {
		g =(TGraph*)obj;
	} else if (obj->InheritsFrom("TPave")) {  
		p =(TPave*)obj;
	}
	if (t && IsInside(t->GetX(), t->GetY())) {
		AddMember(t, t->GetX(), t->GetY());
		mod = kTRUE;
	} else if (g) {
		Double_t *xp = g->GetX();
		Double_t *yp = g->GetY();
		if (IsInside(xp[0], yp[0])) {
			AddMember(obj, xp[0], yp[0]);
			mod = kTRUE;
	   } else if (IsInside(xp[g->GetN()-1], yp[g->GetN()-1])) {
			AddMember(obj, xp[g->GetN()-1], yp[g->GetN()-1]);
			mod = kTRUE;
      }
	} else if (p && IsInside(p->GetX1(), p->GetY1())) {
		mod = kTRUE;
		AddMember(p, p->GetX1(), p->GetY1());
	}
   if (mod) gPad->Modified();
}
//_________________________________________________________________________
 
void TextBox::AddMember(TObject* obj, Double_t xi, Double_t yi)
{
//   if (xi < GetX1() || xi > GetX2() || yi < GetY1() || yi > GetY2()) return;
   Double_t x = xi;
   Double_t y = yi;
   TText  *t = NULL;
   TGraph *g = NULL;
   TSplineX *s = NULL;
   TPave  *p = NULL;
   Bool_t its_line = kFALSE;
   Short_t align = 0;
   if (obj->InheritsFrom("TText")) {
      t =(TText*)obj;
      cout << "TText: ";
   } else if (obj->InheritsFrom("TGraph")) {
      g =(TGraph*)obj;
      its_line = kTRUE;
      cout << "TGraph: ";
   } else if (obj->InheritsFrom("TPave")) {
      p =(TPave*)obj;
       cout << "TPave: ";
   } else if (obj->InheritsFrom("TSplineX")){
      s = (TSplineX*)obj;
      g = ((TSplineX*)obj)->GetControlGraph();
      its_line = kTRUE;
      cout << "TSplineX: ";
   }
   align = GetAlignbyXY(xi, yi, its_line);
   cout << " enter align: " << align << " ";
   TextBoxMember * tb = FindMember(obj);
   if (tb == NULL) {
      tb = new TextBoxMember(obj, align, fAlignType);
      fMembers.Add(tb);
      obj->SetBit(kMustCleanup);
      cout << " new ";
   }
   if (align > 0 && align <= 33) {
      Double_t ts2 = 0;     // allow for some margin if text
      if (t)  ts2 = fTextMargin * t->GetTextSize() * (gPad->GetY2() - gPad->GetY1());
 //        cout << "ts2 " << ts2 << endl;
      tb->SetAlign(align);
		x = GetXbyAlign(align, ts2);
		y = GetYbyAlign(align, ts2);
		if (fAlignType == 1) {
         Int_t halign = align / 10;
         Int_t valign = align%10;
			Double_t ymin = GetY2() - yi;
			Double_t xmin = GetX2() - xi;
			if (yi - GetY1() < ymin) ymin =  yi - GetY1(); 
			if (xi - GetX1() < xmin) xmin =  xi - GetX1(); 
			if (xmin  < ymin) {
            y = yi;
            valign = 0;
			} else {
            x = xi;
            halign = 0;
         }
         tb->SetAlign(halign*10 + valign);
		}
		if (fAlignType == 0) {
			x = xi;
			y = yi;
      }
      cout << " SetAlign " << tb->GetAlign() << endl;
   }
   if (t) {
      t->SetX(x);
      t->SetY(y);
      t->SetTextAlign(align);
//      t->Draw();
   }
   if (g) {
      Int_t np = g->GetN();
      Double_t * xp = g->GetX();
      Double_t * yp = g->GetY();
      Double_t xc = x;
      Double_t yc = y;
      Int_t ip;
      Int_t ip1;
      if (distance( xp[0],yp[0], xi, yi) < distance( xp[np-1],yp[np-1], xi, yi)) {
         tb->SetFirst(kTRUE);
         ip  = 0;
         ip1 = 1;
      } else {
         tb->SetFirst(kFALSE);
         ip  = np - 1;
         ip1 = np - 2;
      }
      if (fCornerRadius > 0 && (align==11 || align==31 || align==33 || align==13)) {
         Double_t rad = fCornerRadius* TMath::Min(GetX2()-GetX1(), GetY2()-GetY1());
         if (align/10 == 1) xc += rad;
         else               xc -= rad;
         if (align%10 == 1) yc += rad;
         else               yc -= rad;
		   Double_t phi = PhiOfLine(xc, yc, xp[ip1], yp[ip1], align);
		   x = xc + rad * TMath::Cos(phi);
		   y = yc + rad * TMath::Sin(phi);
      }
      g->SetPoint(ip, x, y);
   }
   if (p && fAlignType != 0) {
      if (align > 0 && align <= 33) {
         TransformPave(p, align);
      }
   } 
   if (s) {
      s->NeedReCompute();
      s->Paint();
   }
}
//_____________________________________________________________________________

Double_t TextBox::PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t align)
{
   Double_t len = TMath::Sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
   Double_t phi = 0;
   if (len > 0) {
      Double_t sina = (y2 - y1) / len;
      Double_t cosa = (x2 - x1) / len;
      phi = TMath::ATan2(sina, cosa);
      if (phi < 0) phi += 2. * TMath::Pi();
   }
	if (align == 13) {
		if (phi < 0.5 * TMath::Pi()) phi = 0.5 * TMath::Pi();
		if (phi > TMath::Pi()) phi =  TMath::Pi();
	}
	if (align == 11) {
		if (phi < TMath::Pi()) phi = TMath::Pi();
		if (phi > 1.5 * TMath::Pi()) phi = 1.5 * TMath::Pi();
	}
	if (align == 31) {
		if (phi > 2 * TMath::Pi())   phi = 2 * TMath::Pi();
		if (phi < 1.5 * TMath::Pi()) phi = 1.5 * TMath::Pi();
	}
	if (align == 33) {
		if (phi < 0) phi = 0;
		if (phi > .5 * TMath::Pi()) phi = .5 * TMath::Pi();
	}
   return phi;
}
//_________________________________________________________________________

void TextBox::TransformPave(TPave *p, Short_t align)
{
   Double_t x   = 0;
   Double_t y   = 0;
   Double_t dx  = p->GetX2() - p->GetX1();
   Double_t dy  = p->GetY2() - p->GetY1();
   Int_t halign = align / 10;
   Int_t valign = align%10;

   if      (halign == 1)     x =  GetX1();
   else if (halign == 2)     x =  0.5 * (GetX2() + GetX1() - dx);
   else if (halign == 3)     x =  GetX2() - dx;

   if      (valign == 1)     y =  GetY1();
   else if (valign == 2)     y =  0.5 * (GetY2() + GetY1() - dy);
   else if (valign == 3)     y =  GetY2() - dy;
   cout << "TransformPave: nm: " << fMembers.GetEntries() << endl;
//  is there already one?
   if (fMembers.GetEntries() > 0) {
      TIter next(&fMembers);
      TextBoxMember *mobj;
      TObject *obj;
      while ( (mobj = (TextBoxMember*)next()) ) {
         TPave *p = NULL;
         obj = mobj->GetObject();
     	   if (obj->InheritsFrom("TPave")) {
            p = (TPave*)obj;
            if (distance(p->GetX1(), p->GetY1(), x, y) < fSmall) {
               if (halign == 3) 
                  x -= dx;
               else 
                  x+= dx;
               break;
            }
         }
      }
   }
   p->SetX1(x);
   p->SetX2(x+dx);
   p->SetY1(y);
   p->SetY2(y+dy);
   SetPaveNDC(p);
}
//_________________________________________________________________________

Short_t TextBox::GetAlignbyXY(Double_t x, Double_t y, Bool_t its_line) 
{
   Int_t halign = 0;
   Int_t valign = 0;
   Double_t dx = GetX2() - GetX1();
   Double_t dy = GetY2() - GetY1();
   Double_t frac = 1./3.;
   if (its_line) frac = TMath::Max(frac, fCornerRadius);
   if      (x < GetX1() + frac * dx) halign = 1; 
   else if (x > GetX2() - frac * dx) halign = 3;
   else                              halign = 2;
   if      (y < GetY1() + frac * dy) valign = 1; 
   else if (y > GetY2() - frac * dy) valign = 3;
   else                              valign = 2;
   return halign *10 + valign;
}           
//_________________________________________________________________________

Double_t TextBox::GetXbyAlign(Short_t align, Double_t ts2) 
{
   Double_t x = 0;
   Int_t halign = align / 10;
   if      (halign == 1)     x =  GetX1() + ts2;
   else if (halign == 2)     x =  0.5 * (GetX2() + GetX1());
   else if (halign == 3)     x =  GetX2() - ts2;
   return x;
}           
//_________________________________________________________________________

Double_t TextBox::GetYbyAlign(Short_t align, Double_t ts2) 
{
   Double_t y = 0;
   Int_t valign = align%10;
   if      (valign == 1)     y =  GetY1() + ts2;
   else if (valign == 2)     y =  0.5 * (GetY2() + GetY1());
   else if (valign == 3)     y =  GetY2() - ts2;
   return y;
}
//_________________________________________________________________________

TextBoxMember* TextBox::FindMember(TObject *obj)
{
   TIter next(&fMembers);
   TextBoxMember *tbm;
   while ( (tbm = (TextBoxMember*)next()) ) {
      if (tbm->GetObject() == obj) return tbm;
   }
   return NULL;
}
//_________________________________________________________________________

void TextBox::PrintMembers()
{
   TIter next(&fMembers);
   TextBoxMember *tbm;
   cout << endl << "TextBox::PrintMembers()" << endl;
   while ( (tbm = (TextBoxMember*)next()) ) {
      cout << tbm->GetObject() << " " << tbm->GetObject()->ClassName() << " Align: " <<
              tbm->GetAlign() << " AlignType: "  << tbm->GetAlignType()<< endl;
   }
   cout << "------------------------------------------------" << endl;
}
//_________________________________________________________________________

void TextBox::Wakeup()
{
   if (gROOT->GetEditorMode() != 0) return;
   Int_t nfp = gPad->GetListOfPrimitives()->GetEntries();
   if (nfp <= fPrimitives) {
      fPrimitives = nfp;
      return;
   }
   fPrimitives = nfp;
   AdoptAll();
}
//_________________________________________________________________________

void TextBox::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//   if ((event == kButton1Down || event == kButton1Up)) {
//       && gROOT->GetEditorMode() != 0) {
//     cout << "EditorMode " << gROOT->GetEditorMode() << endl;
//  }
   fX1Save = GetX1();
   fY1Save = GetY1();
   fX2Save = GetX2();
   fY2Save = GetY2();

   TPave::ExecuteEvent(event, px, py);

   Double_t dX1 = GetX1() - fX1Save;
   Double_t dY1 = GetY1() - fY1Save;
   Double_t dX2 = GetX2() - fX2Save;
   Double_t dY2 = GetY2() - fY2Save;
   if (dX1 != 0 || dX2 != 0 || dY1 != 0 || dY2 != 0)
      AlignEntries(dX1, dY1, dX2, dY2);
};
//_________________________________________________________________________

void TextBox::AlignEntries(Double_t dX1, Double_t dY1, Double_t dX2, Double_t dY2)
{
   if (fMembers.GetEntries() <= 0) return;
	TIter next(&fMembers);
	TextBoxMember *mobj;
	TObject *obj;
//	Double_t newx = 0;
//	Double_t newy = 0;
	while ( (mobj = (TextBoxMember*)next()) ) {
		Short_t align = mobj->GetAlign();
      if (align == 0) continue;
		TText  *t = NULL;
		TGraph *g = NULL;
		TPave  *p = NULL;
//		TSplineX  *ts = NULL;
		Double_t ts2 = 0;
		obj = mobj->GetObject();
		if (obj->InheritsFrom("TText")) {
			t =(TText*)obj;
			ts2 = fTextMargin * t->GetTextSize() * (gPad->GetY2() - gPad->GetY1());
		} else if (obj->InheritsFrom("TSplineX")) {
			g =(ControlGraph*)((TSplineX*)obj)->GetControlGraph();
		}
		else if (obj->InheritsFrom("TGraph")) {
			g =(TGraph*)obj;
		}
		else if (obj->InheritsFrom("TPave")) {
			p =(TPave*)obj;
		}
		if (   TMath::Abs(dX1) < fSmall && TMath::Abs(dY1) < fSmall
         &&  TMath::Abs(dX2) < fSmall && TMath::Abs(dY2) < fSmall) continue;
      Double_t dX = 0;
      Double_t dY = 0;
      cout << dX1 << " " <<dY1 << " " <<dX2 << " " <<dY2 << " " << align << endl;
      Int_t halign = align / 10;
      Int_t valign = align%10;
      Bool_t shiftonly = kFALSE;
      cout << halign << " va " << valign << endl;
      if (   TMath::Abs(dX1 - dX2) < fSmall 
          && TMath::Abs(dY1 - dY2) < fSmall) {
         // pure shift
         shiftonly = kTRUE;
         dX = dX1;
         dY = dY1;
      } else {
         //size changed
         if      (halign == 1) dX = dX1;
         else if (halign == 2) dX = 0.5 * (dX1 + dX2);
         else if (halign == 3) dX = dX2;
           
         if      (valign == 1) dY = dY1;
         else if (valign == 2) dY = 0.5 * (dY1 + dY2);
         else if (valign == 3) dY = dY2;
           
      }
      cout << halign << " " << valign << " " << dX << " " <<dY << endl;
		if (t) {
			t->SetX(t->GetX() + dX);
			t->SetY(t->GetY() + dY);
		}
		if (g) {
         Int_t np = g->GetN();
			Double_t *xp = g->GetX();
			Double_t *yp = g->GetY();
         Int_t ip;
         Int_t ip1;
			if (mobj->GetFirst()) {
            ip = 0;
            ip1 = 1;
         } else {
            ip = np-1;
            ip1 = np - 2;
         }
         Double_t x = xp[ip] + dX;
         Double_t y = yp[ip] + dY;

         Int_t align = mobj->GetAlign();
			if (fCornerRadius > 0 && (align==11 || align==31 || align==33 || align==13)) {
				x = GetXbyAlign(align);
				y = GetYbyAlign(align);
				Double_t xc = x;
				Double_t yc = y;
				Double_t phi = 0;
				Double_t rad = fCornerRadius* TMath::Min(GetX2()-GetX1(), GetY2()-GetY1());
				if (align/10 == 1) xc += rad;
				else               xc -= rad;
				if (align%10 == 1) yc += rad;
				else               yc -= rad;
			   phi = PhiOfLine(xc, yc, xp[ip1], yp[ip1], align);
				x = xc + rad * TMath::Cos(phi);
				y = yc + rad * TMath::Sin(phi);
			}
         g->SetPoint(ip, x, y);
         if (obj->InheritsFrom("TSplineX")) {
				((TSplineX*)obj)->NeedReCompute();
				((TSplineX*)obj)->Paint();
				cout << "((TSplineX*)obj)->Paint()" << endl;
			}
		}
		if (p && fAlignType != 0) {
			p->SetX1(p->GetX1() + dX);
			p->SetY1(p->GetY1() + dY);
			p->SetX2(p->GetX2() + dX);
			p->SetY2(p->GetY2() + dY);
         SetPaveNDC(p);
		}
	}
   gPad->Update();
}
//________________________________________________________________________________

void TextBox::Paint(Option_t * opt)
{
 //  AlignEntries(0., 0., 0., 0.);
   if (opt);    // keep compiler quiet
   TPave::Paint(GetDrawOption());
}
//________________________________________________________________________________

void TextBox::SetPaveNDC(TPave *p)
 {
   Double_t dpx  = gPad->GetX2() - gPad->GetX1();
   Double_t dpy  = gPad->GetY2() - gPad->GetY1();
   Double_t xp1  = gPad->GetX1();
   Double_t yp1  = gPad->GetY1();
   p->SetX1NDC((p->GetX1()-xp1) / dpx);
   p->SetX2NDC((p->GetX2()-xp1) / dpx);
   p->SetY1NDC((p->GetY1()-yp1) / dpy);
   p->SetY2NDC((p->GetY2()-yp1) / dpy);
}
