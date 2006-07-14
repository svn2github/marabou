#include "TCanvas.h"
#include "TGraph.h"
#include "TObjString.h"
#include "TRootCanvas.h"
#include "TSplineXDialog.h"
#include "TSplineX.h"
#include "TGMrbValuesAndText.h"

ClassImp(TSplineXDialog)

TSplineXDialog::TSplineXDialog()
{
   static void *valp[25];
   Int_t ind = 0;
   TList * row_lab = new TList(); 
   fClosed  = 0;
   fApprox  = 1;
   fFixends = 1;
   fPrec 	= 0.2;
   fShowcp  = 1;
   fColor	= gStyle->GetLineColor();
   fLwidth  = gStyle->GetLineWidth();
   fLstyle  = gStyle->GetLineStyle();
   fFcolor  = gStyle->GetFillColor();
   fFstyle  = gStyle->GetFillStyle();

   fFilled  		= 0;
   fEmpty			= 5;
   fGage 			= 0;
   fArrow_at_start = 0;
   fArrow_at_end  = 0;
   fArrow_filled  = 0;
   fArrow_size 	= 10;
   fArrow_angle	= 30;
   fArrow_indent_angle  = -30;
   fCommand = "Draw_The_TSplineX()";

   row_lab->Add(new TObjString("CheckButton_Closed curve"));
   valp[ind++] = &fClosed;
   row_lab->Add(new TObjString("CheckButton+Approximate"));
   valp[ind++] = &fApprox;
   row_lab->Add(new TObjString("CheckButton+Fix endpoints"));
   valp[ind++] = &fFixends;
   row_lab->Add(new TObjString("DoubleValue_Precision"));
   valp[ind++] = &fPrec;
   row_lab->Add(new TObjString("CheckButton+Show Controlpoints"));
   valp[ind++] = &fShowcp;
   row_lab->Add(new TObjString("ColorSelect_Line Col"));
   valp[ind++] = &fColor;
   row_lab->Add(new TObjString("PlainShtVal+L Width"));
   valp[ind++] = &fLwidth;
   row_lab->Add(new TObjString("LineSSelect+L Style"));
   valp[ind++] = &fLstyle;
   row_lab->Add(new TObjString("CheckButton_Arrow@Start"));
   valp[ind++] = &fArrow_at_start;
   row_lab->Add(new TObjString("CheckButton+Arrow@End"));
   valp[ind++] = &fArrow_at_end;
   row_lab->Add(new TObjString("CheckButton+Fill Arrow"));
   valp[ind++] = &fArrow_filled;
   row_lab->Add(new TObjString("DoubleValue_ArLen"));
   valp[ind++] = &fArrow_size;
   row_lab->Add(new TObjString("DoubleValue+ArAng"));
   valp[ind++] = &fArrow_angle;
   row_lab->Add(new TObjString("DoubleValue+InAng"));
   valp[ind++] = &fArrow_indent_angle;
//   row_lab->Add(new TObjString("CheckButton_Railway (double line)"));
//   valp[ind++] = &railway;
   row_lab->Add(new TObjString("DoubleValue_Rail Gage"));
   valp[ind++] = &fGage;
   row_lab->Add(new TObjString("DoubleValue+SleeperL"));
   valp[ind++] = &fFilled;
   row_lab->Add(new TObjString("DoubleValue+SleeperD"));
   valp[ind++] = &fEmpty;
   row_lab->Add(new TObjString("ColorSelect_Fill Color"));
   valp[ind++] = &fFcolor;
   row_lab->Add(new TObjString("Fill_Select+Fill Style"));
   valp[ind++] = &fFstyle;
   row_lab->Add(new TObjString("CommandButt_Draw_the_TSplineX"));
   valp[ind++] = &fCommand;

   Bool_t ok; 
   Int_t itemwidth = 320;
   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   ok = GetStringExt("TSplineX Params", NULL, itemwidth, rc,
                     NULL, NULL, row_lab, valp,
                     NULL, NULL, NULL, this, this->ClassName());
}
//______________________________________________________________________

void TSplineXDialog::Draw_The_TSplineX()
{

  cout << "Input a  Polyline defining the controlpoints" << endl;
  TIter next(gPad->GetListOfPrimitives());
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
  TGraph * gr = (TGraph*)gPad->WaitPrimitive("Graph", "PolyLine");
  if (!gr) {
      cout << "No PolyLine found, try again" << endl;
      return;
   }
   gr->SetName("abc");
   Double_t* x = gr->GetX();
   Double_t* y = gr->GetY();
   Int_t npoints = gr->GetN();
//  add an extra point in between
   if (npoints < 2) {
      cout << "Need at least 2 points, try again" << endl;
      return;
   }
   TArrayF shape_factors(npoints);
   if ( (fFixends == 0 || fClosed == 1) && fApprox == 1) {
      shape_factors[0] = 1;
      shape_factors[npoints - 1] = 1;
   } else {
      shape_factors[0] = -1;
      shape_factors[npoints - 1] = -1;
   }
   for (Int_t i = 1; i < npoints - 1; i++) {
   	if (fApprox == 1) {
      	shape_factors[i] = 1;
   	} else {
      	shape_factors[i] = -1;
   	}
   }
   Bool_t closed_spline;
   if (fClosed != 0) closed_spline = kTRUE;
   else              closed_spline = kFALSE;
   TSplineX* xsp = 
     new TSplineX(npoints, x, y, shape_factors.GetArray(), fPrec, closed_spline);
   xsp->Draw("L");
   xsp->SetFillColor(fFcolor);
   xsp->SetFillStyle(fFstyle);
   xsp->SetLineColor(fColor);
   xsp->SetLineWidth(fLwidth);
   xsp->SetLineStyle(fLstyle);
   xsp->SetFilledLength(fFilled);
   xsp->SetEmptyLength(fEmpty);
   if (fGage > 0)xsp->SetRailwaylike(fGage);
   if (fShowcp > 0) xsp->DrawControlPoints(0, 0);
   Bool_t afilled;
   if (fArrow_filled == 0) afilled = kFALSE;
   else                   afilled = kTRUE;
   if (fArrow_at_start) xsp->AddArrow(0, fArrow_size, fArrow_angle, fArrow_indent_angle, afilled);
   if (fArrow_at_end)   xsp->AddArrow(1, fArrow_size, fArrow_angle, fArrow_indent_angle, afilled);
   delete gr;
   gPad->Modified();
   gPad->Update();
}
