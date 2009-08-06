#include "TROOT.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TRootCanvas.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "Set2DimOptDialog.h"
#include <iostream>

namespace std {} using namespace std;

//TString Set2DimOptDialog::fDrawOpt2Dim = "COLZ";
//_______________________________________________________________________

Set2DimOptDialog::Set2DimOptDialog(TGWindow * win)
{
static const Char_t helptext[] =
"Most of the value in this widget are self explaining\n\
If \"Live statbox\" a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values.\n\
\n\
SCAT  : Draw a scatter-plot (default)\n\
BOX   : a box is drawn for each cell with surface proportional to the\n\
          content's absolute value. A negative content is marked with a X.\n\
BOX1  : a button is drawn for each cell with surface proportional to\n\
          content's absolute value. A sunken button is drawn for negative values\n\
          a raised one for positive.\n\
COL   : a box is drawn for each cell with a color scale varying with contents\n\
COLZ  : same as COL. In addition the color palette is also drawn\n\
CONT  : Draw a contour plot (same as CONT0)\n\
CONT0 : Draw a contour plot using surface colors to distinguish contours\n\
CONT1 : Draw a contour plot using line styles to distinguish contours\n\
CONT2 : Draw a contour plot using the same line style for all contours\n\
CONT3 : Draw a contour plot using fill area colors\n\
CONT4 : Draw a contour plot using surface colors (SURF option at theta = 0)\n\
TRI2  : (TGraph2D only) Draw a contour plot using Delaunay triangles\n\
LEGO  : Draw a lego plot with hidden line removal\n\
LEGO1 : Draw a lego plot with hidden surface removal\n\
LEGO2 : Draw a lego plot using colors to show the cell contents\n\
SURF  : Draw a surface plot with hidden line removal\n\
SURF1 : Draw a surface plot with hidden surface removal\n\
SURF2 : Draw a surface plot using colors to show the cell contents\n\
SURF3 : same as SURF with in addition a contour view drawn on the top\n\
SURF4 : Draw a surface using Gouraud shading\n\
SURF5 : Same as SURF3 but only the colored contour is drawn. Used with\n\
          option CYL, SPH or PSR it allows to draw colored contours on a\n\
          sphere, a cylinder or a in pseudo rapidy space. In cartesian\n\
          or polar coordinates, option SURF3 is used.\n\
ARR   : arrow mode. Shows gradient between adjacent cells\n\
TEXT  : Draw bin contents as text (format set via gStyle->SetPaintTextFormat)\n\
";

   const char *fDrawOpt2[kNdrawopt] =
   {" SCAT", " BOX0", " BOX1", "  COL", "  ARR",
	 "CONT0", "CONT1", "CONT2", "CONT3", "CONT4",
    "SURF0", "SURF1", "SURF2", "SURF3", "SURF4",
    " LEGO", "LEGO1", "LEGO2", " TRI2", " TEXT"};
	 
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   fHist = NULL;
   Int_t nh1 = 0, nh2 = 0;
   TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) {
         fHist = (TH1*)obj;
         if (obj->InheritsFrom("TGraph")) {
            fHist = ((TGraph*)obj)->GetHistogram();
            if (!fHist) continue;
		   }
         if (fHist->GetDimension() == 1) nh1++;
         if (fHist->GetDimension() > 1)  nh2++;
      }
   }
	if ( fHist == NULL ) {
	   cout << "No Histogram in Canvas" << endl;
	}
   RestoreDefaults();
	
   Int_t selected = -1;
   for (Int_t i = 0; i < kNdrawopt; i++) {
      fDrawOpt2DimArray[i] = fDrawOpt2[i];
      if (fDrawOpt2Dim.Contains(fDrawOpt2DimArray[i]) ) {
         if (selected < 0) {
            fOptRadio[i] = 1;
            selected = i;
         }
      } else {
         fOptRadio[i] = 0;
      }
   }
	if (fDrawOpt2Dim.Contains("Z")) fShowZScale = 1;
	else                            fShowZScale = 0;
	if (fDrawOpt2Dim.Contains("BB")) fHideBackBox = 1;
	else                             fHideBackBox = 0;
	if (fDrawOpt2Dim.Contains("FB")) fHideFrontBox = 1;
	else                             fHideFrontBox = 0;
	
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
   Int_t indopt = 0;
   static Int_t dummy;
   static TString stycmd("SetHistAttPermLocal()");

   fRow_lab->Add(new TObjString("CommentOnly_Scatter, Boxes, Colorplots"));
   fValp[ind++] = &dummy;
	fBidSCAT = 1;
	fBidBOX  = 2;
	fBidBOX1 = 3;
	fBidARR =  5;
   for (Int_t i = 0; i < 5; i++) {
//   cout << " indopt  " <<  indopt << endl;
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
//   cout << " indopt  " <<  indopt << endl;
   fRow_lab->Add(new TObjString("CommentOnly_Contour options"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
   fRow_lab->Add(new TObjString("CommentOnly_Surface options"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
   fRow_lab->Add(new TObjString("CommentOnly_LEGO, Arrows, Text"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
	fBidTEXT = ind - 1;
   fRow_lab->Add(new TObjString("ColorSelect_BgColor"));
   fValp[ind++] = &f2DimBackgroundColor;
   fRow_lab->Add(new TObjString("ColorSelect+FillC"));
   fBidFillColor = ind; fValp[ind++] = &fHistFillColor2Dim;
   fRow_lab->Add(new TObjString("ColorSelect+LineC"));
   fBidLineColor = ind; fValp[ind++] = &fHistLineColor2Dim;

   fRow_lab->Add(new TObjString("ColorSelect_MColor"));
   fBidMarkerColor = ind; fValp[ind++] = &fMarkerColor2Dim;
   fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
   fBidMarkerStyle = ind; fValp[ind++] = &fMarkerStyle2Dim;
   fRow_lab->Add(new TObjString("Float_Value+MSize"));
   fBidMarkerSize = ind; fValp[ind++] = &fMarkerSize2Dim;
	
   fRow_lab->Add(new TObjString("CheckButton_Z Scale"));
   fRow_lab->Add(new TObjString("CheckButton+No Fbox"));
   fRow_lab->Add(new TObjString("CheckButton+No Bbox"));
   fRow_lab->Add(new TObjString("CheckButton+Live stat"));
   fValp[ind++] = &fShowZScale;
   fValp[ind++] = &fHideFrontBox;
   fValp[ind++] = &fHideBackBox;
   fValp[ind++] = &fLiveStat2Dim;
//   fRow_lab->Add(new TObjString("DoubleValue_LogScaleMin"));
//   fRow_lab->Add(new TObjString("DoubleValue+LogScaleMax"));
//   fValp[ind++] = &fLogScaleMin;
//   fValp[ind++] = &fLogScaleMax;


   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;

   static Int_t ok;
   Int_t itemwidth = 360;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
/*
	if ( fDrawOpt2Dim.Contains("SCAT") ||   fDrawOpt2Dim.Contains("TEXT")) {
	   fDialog->EnableButton(fBidMarkerColor);
		fDialog->EnableButton(fBidMarkerStyle);
		fDialog->EnableButton(fBidMarkerSize);
   } else {
	   fDialog->DisableButton(fBidMarkerColor);
		fDialog->DisableButton(fBidMarkerStyle);
		fDialog->DisableButton(fBidMarkerSize);
	}
	if ( fDrawOpt2Dim.Contains("BOX") || fDrawOpt2Dim.Contains("ARR") ) {
	   fDialog->EnableButton(fBidFillColor);
		fDialog->EnableButton(fBidLineColor);
   } else {
	   fDialog->DisableButton(fBidFillColor);
		fDialog->DisableButton(fBidLineColor);
	}
*/
}
//_______________________________________________________________________

void Set2DimOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "Set2DimOptDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void Set2DimOptDialog::CloseDialog()
{
//   cout << "Set2DimOptDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void Set2DimOptDialog::SetHistAttNow(TCanvas *canvas)
{
   for (Int_t i = 0; i < kNdrawopt; i++) {
      if (fOptRadio[i] == 1) {
//         cout << "fDrawOpt2DimArray[" << i << "]" << endl;
         fDrawOpt2Dim = fDrawOpt2DimArray[i];
//         if (fShowZScale)   fDrawOpt2Dim += "Z";

      }
   }
   if (!canvas) return;
   Set2DimOptDialog::SetHistAtt(canvas);
}
//_______________________________________________________________________

void Set2DimOptDialog::SetHistAtt(TCanvas *canvas)
{
   if (!canvas) return;
	canvas->cd();
   TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   fDrawOpt = fDrawOpt2Dim;
   if ( fShowZScale && (
        fDrawOpt.Contains("COL")   || fDrawOpt.Contains("CONT0")
      ||fDrawOpt.Contains("CONT1") || fDrawOpt.Contains("CONT4")
      ||fDrawOpt.Contains("TRI2")  || fDrawOpt.Contains("SURF1")
      ||fDrawOpt.Contains("SURF2") || fDrawOpt.Contains("SURF3")
      ||fDrawOpt.Contains("LEGO2") ) )
      fDrawOpt += "Z";
   if (fHideFrontBox) fDrawOpt += "FB";
   if (fHideBackBox)  fDrawOpt += "BB";

   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TGraph2D")) {
         ((TGraph2D*)obj)->SetDrawOption(fDrawOpt);
      } else if (obj->InheritsFrom("TH2")) {
         ((TH2*)obj)->SetDrawOption(fDrawOpt);
         ((TH2*)obj)->SetOption(fDrawOpt);
			((TH2*)obj)->SetFillColor(fHistFillColor2Dim);
			((TH2*)obj)->SetLineColor(fHistLineColor2Dim);
			((TH2*)obj)->SetLineWidth(1);
			((TH2*)obj)->SetLineStyle(1);
			((TH2*)obj)->SetFillStyle(0);
			((TH2*)obj)->SetMarkerColor(fMarkerColor2Dim);  
			((TH2*)obj)->SetMarkerStyle(fMarkerStyle2Dim);  
			((TH2*)obj)->SetMarkerSize (fMarkerSize2Dim);   			
      } else if ((obj->InheritsFrom("TPad"))) {
         TPad *pad = (TPad*)obj;
         TIter next1(pad->GetListOfPrimitives());
         TObject *obj1;
         pad->cd();
			while ( (obj1 = next1()) ) {
				if (obj1->InheritsFrom("TH1")) {
               ((TH2*)obj1)->SetDrawOption(fDrawOpt);
               ((TH2*)obj1)->SetOption(fDrawOpt);
            }
         }
	      pad->Modified();
	      pad->Update();
      }
   }
   if (gPad == gPad->GetMother()) {
		if (fLiveStat2Dim) {
//			cout << "SetAutoExec() true" << endl;
			if (!canvas->GetAutoExec())
				canvas->ToggleAutoExec();
		} else {
//			cout << "SetAutoExec() false" << endl;
			if (canvas->GetAutoExec())
				canvas->ToggleAutoExec();
		}
   }
   if (f2DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         canvas->GetFrame()->SetFillStyle(0);
      } else {
      	canvas->GetFrame()->SetFillStyle(1001);
      	canvas->GetFrame()->SetFillColor(10);
   	}
   } else {
      canvas->GetFrame()->SetFillStyle(1001);
      canvas->GetFrame()->SetFillColor(f2DimBackgroundColor);
   }
	canvas->Pop();
	canvas->Modified();
	canvas->Update();
}
//______________________________________________________________________

void Set2DimOptDialog::SetHistAttPermLocal()
{
//   cout << "Set2DimOptDialog:: SetHistAttPerm()" << endl;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void Set2DimOptDialog::SetHistAttPerm()
{
//   cout << "Set2DimOptDialog:: SetHistAttPerm()" << endl;
   TEnv env(".hprrc");
   env.SetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fDrawOpt2Dim", fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fLiveStat2Dim", fLiveStat2Dim);
   env.SetValue("Set2DimOptDialog.fShowZScale", fShowZScale);
	env.SetValue("Set2DimOptDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimOptDialog.fHistFillColor2Dim",fHistFillColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineColor2Dim",fHistLineColor2Dim);
	env.SetValue("Set2DimOptDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
   env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set2DimOptDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("Set2DimOptDialog.fDrawOpt2Dim",      fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fLiveStat2Dim",     fLiveStat2Dim);
   env.SetValue("Set2DimOptDialog.fShowZScale",       fShowZScale);
	env.SetValue("Set2DimOptDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimOptDialog.fHistFillColor2Dim",fHistFillColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineColor2Dim",fHistLineColor2Dim);
	env.SetValue("Set2DimOptDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void Set2DimOptDialog::RestoreDefaults()
{
   if ( !fHist ) return;
	fDrawOpt2Dim = fHist->GetOption();
   TEnv env(".hprrc");
//   fDrawOpt2Dim  = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
   fShowZScale        = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
   fLiveStat2Dim      = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
	f2DimBackgroundColor = env.GetValue("Set2DimOptDialog.f2DimBackgroundColor", 0);
	fHistFillColor2Dim = env.GetValue("Set2DimOptDialog.fHistFillColor2Dim", 1);
	fHistLineColor2Dim = env.GetValue("Set2DimOptDialog.fHistLineColor2Dim", 1);
	fMarkerColor2Dim   = env.GetValue("Set2DimOptDialog.fMarkerColor2Dim",   1);
	fMarkerStyle2Dim   = env.GetValue("Set2DimOptDialog.fMarkerStyle2Dim",   1);
	fMarkerSize2Dim    = env.GetValue("Set2DimOptDialog.fMarkerSize2Dim",    1);
}
//______________________________________________________________________

void Set2DimOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void Set2DimOptDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
/*
//	if ( fDrawOpt2Dim.Contains("SCAT") ) {
	if ( bid == fBidSCAT  || bid == fBidTEXT) {
	   fDialog->EnableButton(fBidMarkerColor);
		fDialog->EnableButton(fBidMarkerStyle);
		fDialog->EnableButton(fBidMarkerSize);
   } else if  (bid <= fBidTEXT ) {
	   fDialog->DisableButton(fBidMarkerColor);
		fDialog->DisableButton(fBidMarkerStyle);
		fDialog->DisableButton(fBidMarkerSize);
	}
//	if ( fDrawOpt2Dim.Contains("BOX") ) {
	if ( bid == fBidBOX || bid == fBidBOX1  || bid == fBidARR) {
	   fDialog->EnableButton(fBidFillColor);
		fDialog->EnableButton(fBidLineColor);
   } else  if  (bid <= fBidTEXT ) {
	   fDialog->DisableButton(fBidFillColor);
		fDialog->DisableButton(fBidLineColor);
	}
*/
   SetHistAttNow(canvas);
}

