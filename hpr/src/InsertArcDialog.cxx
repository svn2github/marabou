#include "TArc.h"
#include "TArrow.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "TCanvas.h"
#include "TMarker.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "InsertArcDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertArcDialog)

InsertArcDialog::InsertArcDialog()
{
 //  Int_t win_width = 160InsertArcByPointsOnCF()
   static void *valp[25];
   Int_t ind = 0;
   RestoreDefaults();
   TList * labels = new TList;
   static TString bycenter("ArcByCenterAndRadius()");
   static TString byponcf("ArcByPointsOnCF()");
   fXcenter = fYcenter = fX1 = fX2 = fY1 = fY2 = 0;
   fPhi1 = 0;
   fPhi2 = 360;
   labels->Add(new TObjString("DoubleValue_Xc"));
   valp[ind++] = &fXcenter;
   labels->Add(new TObjString("DoubleValue+Yc"));
   valp[ind++] = &fYcenter;
   labels->Add(new TObjString("DoubleValue_R1"));
   valp[ind++] = &fR1;
   labels->Add(new TObjString("DoubleValue+Ell:R2/R1"));
   valp[ind++] = &fR2;
   labels->Add(new TObjString("DoubleValue_X1 on Cf"));
   valp[ind++] = &fX1;
   labels->Add(new TObjString("DoubleValue+Y1 on Cf"));
   valp[ind++] = &fY1;
   labels->Add(new TObjString("DoubleValue_X2 on Cf"));
   valp[ind++] = &fX2;
   labels->Add(new TObjString("DoubleValue+Y2 on Cf"));
   valp[ind++] = &fY2;
   labels->Add(new TObjString("DoubleValue_Phi1 on Cf"));
   valp[ind++] = &fPhi1;
   labels->Add(new TObjString("DoubleValue+Phi2 on Cf"));
   valp[ind++] = &fPhi2;
   labels->Add(new TObjString("CheckButton_Sense Clock"));
   valp[ind++] = &fSense;
   labels->Add(new TObjString("CheckButton+Show Edges"));
   valp[ind++] = &fShowEdges;
   labels->Add(new TObjString("ColorSelect_Color"));
   valp[ind++] = &fColor;
   labels->Add(new TObjString("LineSSelect+Style"));
   valp[ind++] = &fStyle;
   labels->Add(new TObjString("PlainShtVal+Width"));
   valp[ind++] = &fWidth;
   labels->Add(new TObjString("ColorSelect_FillColor"));
   valp[ind++] = &fFillColor;
   labels->Add(new TObjString("Fill_Select+FillStyle"));
   valp[ind++] = &fFillStyle;
   labels->Add(new TObjString("CommandButt_ArcByCenterAndRadius()")); 	
   valp[ind++] = &bycenter;
   labels->Add(new TObjString("CommandButt_ArcByPointsOnCircumference()")); 	
   valp[ind++] = &byponcf;
   Bool_t ok;
   Int_t itemwidth = 280;

   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   ok = GetStringExt("Arc / Ellipse", NULL, itemwidth, rc,
                      NULL, NULL, labels,valp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//_________________________________________________________________________
            
InsertArcDialog::~InsertArcDialog() 
{
   SaveDefaults();
};
//______________________________________________________________________________

void InsertArcDialog::ArcByCenterAndRadius()
{ 
   TMarker *m1 = NULL;
   TMarker *m2 = NULL;
   if (fXcenter == 0 && fYcenter == 0) {
   	cout << "Mark center with left mouse" << endl;
   	m1  = (TMarker*)gPad->WaitPrimitive("TMarker");
   	m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
      if (!m1) return;
   	fXcenter = m1->GetX();
   	fYcenter = m1->GetY();
   	m1->SetMarkerStyle(4);
   	m1->Draw();
      if (fR1 <= 0) {
   	   cout << "Mark radius with left mouse" << endl;
      	m2  = (TMarker*)gPad->WaitPrimitive("TMarker");
   	   m2 = (TMarker *)gPad->GetListOfPrimitives()->Last();
         if (!m2) {if (m1) delete m1; return;};
  		   fR1 = TMath::Sqrt( (fXcenter - m2->GetX()) * (fXcenter - m2->GetX())
                  	 + (fYcenter - m2->GetY()) * (fYcenter - m2->GetY())); 
      }
   }
   if (m1) delete m1;
   if (m2) delete m2;
   TEllipse *el;
   if (fR2 <= 0) {
      TArc *a = new TArc(fXcenter, fYcenter, fR1, fPhi1, fPhi2);
      el = (TEllipse *)a; 
   } else {
      el = new TEllipse(fXcenter, fYcenter, fR1, fR1 * fR2, fPhi1, fPhi2);
   }
   el->Draw();
   el->SetLineColor(fColor);
   el->SetLineStyle(fStyle);
   el->SetLineWidth(fWidth);
   el->SetFillColor(fFillColor);
   el->SetFillStyle(fFillStyle);
   el->SetNoEdges(!fShowEdges);
   fXcenter = fYcenter = 0;
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void InsertArcDialog::ArcByPointsOnCF()
{   	
   if (fR1 <= 0) {
      cout << "Radius must be defined and > 0" << endl;
      return;
   }
   if (fX1 == 0 && fY1 == 0 && fX2 == 0 && fY2 == 0) {
   	cout << "Mark first point with left mouse" << endl;
   	TMarker * m1  = (TMarker*)gPad->WaitPrimitive("TMarker");
   	m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
      if (!m1) return;
   	fX1 = m1->GetX();
   	fY1 = m1->GetY();
   	m1->SetMarkerStyle(4);
   	m1->Draw();
   	cout << "Mark second point with left mouse" << endl;
   	TMarker * m2  = (TMarker*)gPad->WaitPrimitive("TMarker");
      if (!m2) {delete m1; return;};
   	m2 = (TMarker *)gPad->GetListOfPrimitives()->Last();
   	fX2 = m2->GetX();
   	fY2 = m2->GetY();
   	delete m1;
   	delete m2;
   }
   Int_t sense;
   if (fSense > 0) sense = 1;
   else           sense = -1;
   Bool_t ok =  PoCftoCenterPhi(fX1, fY1, fX2, fY2, fR1, sense,
                       &fXcenter, &fYcenter, &fPhi1, &fPhi2);
   if (!ok) return;
   TArc * a = new TArc(fXcenter, fYcenter, fR1, fPhi1, fPhi2);
   a->Draw(); 
   a->SetLineColor(fColor);
   a->SetLineStyle(fStyle);
   a->SetLineWidth(fWidth);
   a->SetFillColor(fFillColor);
   a->SetFillStyle(fFillStyle);
   a->SetNoEdges(!fShowEdges);
   fXcenter = fYcenter = fX1 = fX2 = fY1 = fY2 = 0;
   fPhi1 = 0;
   fPhi2 = 360;
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void InsertArcDialog::SaveDefaults()
{
   cout << "InsertArcDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
   env.SetValue("InsertArcDialog.fColor" , fColor); 
   env.SetValue("InsertArcDialog.fStyle" , fStyle);
   env.SetValue("InsertArcDialog.fWidth" , fWidth);
   env.SetValue("InsertArcDialog.fFillColor" , fFillColor); 
   env.SetValue("InsertArcDialog.fFillStyle" , fFillStyle);
   env.SetValue("InsertArcDialog.fR1"         , fR1 );
   env.SetValue("InsertArcDialog.fR2"         , fR2 );
//   env.SetValue("InsertArcDialog.fPhi1"  , fPhi1 );
 //  env.SetValue("InsertArcDialog.fPhi2"  , fPhi2 ); 
   env.SetValue("InsertArcDialog.fSense" , fSense); 
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void InsertArcDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fColor = env.GetValue("InsertArcDialog.fColor" , 1); 
   fStyle = env.GetValue("InsertArcDialog.fStyle" , 1);
   fWidth = env.GetValue("InsertArcDialog.fWidth" , 2);
   fFillColor = env.GetValue("InsertArcDialog.fFillColor" , 1); 
   fFillStyle = env.GetValue("InsertArcDialog.fFillStyle" , 1);
   fR1  	 = env.GetValue("InsertArcDialog.fR1"  , 30.);
   fR2  	 = env.GetValue("InsertArcDialog.fR2"  , -1.);
//   fPhi1	 = env.GetValue("InsertArcDialog.fPhi1"  , 0.);
//   fPhi2	 = env.GetValue("InsertArcDialog.fPhi2"  , 360.); 
   fSense = env.GetValue("InsertArcDialog.fSense" , 1); 
} 
//_________________________________________________________________________
            
void InsertArcDialog::CloseDown()
{
   cout << "InsertArcDialog::CloseDown()" << endl;
   delete this;
}
//_________________________________________________________________________
            
Bool_t InsertArcDialog::PoCftoCenterPhi(Double_t x1, Double_t y1, 
                       Double_t x2, Double_t y2, Double_t r, Int_t sense,
                       Double_t *xcenter, Double_t *ycenter, 
                       Double_t *phimin, Double_t *phimax)
{
   // calculate center, given 2 points on circle and radius
/*
   Double_t x1, x2, y1, y2, xc, yc, r;
   
   x1 = 4;
   y1 = 4;
   x2 = -4;
   y2 = 4;

   r  = TMath::Sqrt(32.);
*/
   std::cout << "PoCftoCenterPhi: " << x1<<" " << y1<< " "<< x2 <<" " <<y2 << " " << r << std::endl; 
   Double_t xc, yc;
   Bool_t ok = kTRUE;
   if (x1 == x2) {
   	Double_t arg = r*r - (y2-y1)*(y2-y1)/4;
   	if (arg <= 0) {
      	std::cout << "No real solution" << std::endl;
      	xc = yc = 0;
         ok = kFALSE;
   	} else {
      	xc = x1 - sense * TMath::Sqrt(arg);
      	yc = 0.5*(y2+y1);
   	}
   } else {
   	Double_t a = 0.5 * ( x1*x1 - x2*x2 + y1*y1  - y2*y2) / (x1 - x2);
   	Double_t b =  (y1-y2) / (x2-x1);
   	Double_t A = 1 + b*b;
   	Double_t B = -2*y1 + 2*a*b - 2*b*x1;
   	Double_t C = a*a - 2*x1*a + x1*x1 + y1*y1 - r*r;
//      Double_t dist = TMath::Sqrt((x1 - x2)*(x1 - x2) + (y1-y2)*(y1-y2));
//   	std::cout << dist << " "  << A << " " << B << " " << C << std::endl;
      Double_t arg = B*B/4 - A*C;
      if (arg <=0 ) {
         std::cout << "No real solution" << std::endl;
         xc = yc = 0;
         ok = kFALSE;
      } else {
         yc = (-0.5*B  - sense * TMath::Sqrt(arg)) / A;
   	   xc = a + b * yc;
      }
   } 
   Double_t phi1 = 0;
   Double_t phi2 = 0;
   if (ok) {
      Double_t cosa = (x1 - xc)/r;
      Double_t sina = (y1 - yc)/r;
      phi1  = TMath::ATan2(sina,cosa);
      if(phi1 < 0) phi1 += 2 * TMath::Pi();
      phi1 = phi1 * 180 / TMath::Pi();
      cosa = (x2 - xc)/r;
      sina = (y2 - yc)/r;
      phi2  = TMath::ATan2(sina,cosa);
      if(phi2 < 0) phi2 += 2 * TMath::Pi();
      phi2 = phi2 * 180 / TMath::Pi();
      std::cout << xc << " " << yc<< " " << phi1<< " " << phi2 << std::endl;
   }
   *xcenter = xc;
   *ycenter = yc;
   *phimin = phi1;
   *phimax = phi2;
   return ok;
}

