#include "TROOT.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TH1.h"
#include "TStyle.h"
#include "Set1DimOptDialog.h"
#include <iostream>

namespace std {} using namespace std;

//TString Set1DimOptDialog::fDrawOpt1Dim = "COLZ";
//_______________________________________________________________________

Set1DimOptDialog::Set1DimOptDialog(TGWindow * win)
{
static const Char_t helptext[] =
"\n\
A histogram can either been drawn as a line (\"Contour\")\n\
or with markers. To fill a histogram \"Contour\" must\n\
be selected\n\
\n\
Error Drawing Modes:\n\
E	Draw error bars.\n\
E0	Draw error bars. Markers are drawn for bins with 0 contents.\n\
E1	Draw error bars with perpendicular lines at the edges.\n\
	Length is controled by EndErrSize.\n\
E2	Draw error bars with rectangles.\n\
E3	Draw a fill area through the end points of the vertical error bars.\n\
E4	Draw a smoothed filled area through the end points of the error bars.\n\
E5	Like E3 but ignore the bins with 0 contents.\n\
E6	Like E4 but ignore the bins with 0 contents.\n\
\n\
Options E3-E6: Choose: Contour Off and FillHist On to get area\n\
               filled between the error lines.\n\
X ErrorS controls drawing of error bars in X.\n\
A value of 0.5 draws a line X +- 0.5*BinWidth\n\
\n\
If \"Live statbox\" a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values. Selecting \"Live Gauss fit\" fits a gaussian to the\n\
dragged region. Normally \"Linear background in fit\" should also\n\
be selected.\n\
";
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
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
 //  static Int_t dummy;
   static TString stycmd("SetHistAttPermLocal()");

   fRow_lab->Add(new TObjString("CheckButton_Contour/Histo"));
   fValp[ind++] = &fShowContour;
   fRow_lab->Add(new TObjString("CheckButton+  Labels Top X"));
   fRow_lab->Add(new TObjString("CheckButton+Labels Right Y"));
   fValp[ind++] = &fLabelsTopX;
   fValp[ind++] = &fLabelsRightY;
	fRow_lab->Add(new TObjString("ColorSelect_LineCol"));
	fRow_lab->Add(new TObjString("LineSSelect+Style"));
	fRow_lab->Add(new TObjString("PlainShtVal+Width"));
	fValp[ind++] = &fHistLineColor;
	fValp[ind++] = &fHistLineStyle;
	fValp[ind++] = &fHistLineWidth;
   fRow_lab->Add(new TObjString("ComboSelect_Error Mode;none;E;E1;E2;E3;E4;E5;E6"));
   fValp[ind++] = &fErrorMode;
	fRow_lab->Add(new TObjString("Float_Value+EndErrSize "));
	fValp[ind++] = &fEndErrorSize;
	fRow_lab->Add(new TObjString("Float_Value+X ErrorS"));
	fValp[ind++] = &fErrorX;
//   fRow_lab->Add(new TObjString("CheckButton+Xaxis at top"));
//   fValp[ind++] = &fDrawAxisAtTop;
   fRow_lab->Add(new TObjString("CheckButton_Fill hist"));
   fValp[ind++] = &fFill1Dim;
   fRow_lab->Add(new TObjString("ColorSelect+FillColor"));
   fValp[ind++] = &fHistFillColor;
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fValp[ind++] = &fHistFillStyle;

   fRow_lab->Add(new TObjString("ColorSelect_MarkerColor"));
   fValp[ind++] = &fMarkerColor;
   fRow_lab->Add(new TObjString("Mark_Select+MarkerStyle"));
   fValp[ind++] = &fMarkerStyle;
   fRow_lab->Add(new TObjString("Float_Value+MarkerSize"));
   fValp[ind++] = &fMarkerSize;

   fRow_lab->Add(new TObjString("CheckButton_Live statbox"));
   fValp[ind++] = &fLiveStat1Dim;
   fRow_lab->Add(new TObjString("CheckButton+Live Gauss fit"));
   fValp[ind++] = &fLiveGauss;
   fRow_lab->Add(new TObjString("CheckButton+Lin bg in fit "));
   fValp[ind++] = &fLiveBG;

   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;

   static Int_t ok;
   Int_t itemwidth = 420;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

void Set1DimOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "Set1DimOptDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void Set1DimOptDialog::CloseDialog()
{
//   cout << "Set1DimOptDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab) {
	   fRow_lab->Delete();
      delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void Set1DimOptDialog::SetHistAttNow(TCanvas *canvas)
{
   SetHistAtt(canvas);
}
//_______________________________________________________________________

void Set1DimOptDialog::SetHistAtt(TCanvas *canvas)
{
   if (!canvas) return;
	canvas->cd();
   if (fFill1Dim) {
      if (fHistFillColor == 0) fHistFillColor = 1;
      if (fHistFillStyle == 0) fHistFillStyle = 1001;
   }
   fDrawOpt = "";
   if (fErrorMode != "none") {
      fDrawOpt += fErrorMode;
   }
   if ( fMarkerSize > 0 && fShowContour == 0 ) {
      fDrawOpt += "P";
   }
   if (fDrawOpt.Length() == 0 || fShowContour != 0) fDrawOpt += "HIST";
//   if (fShowContour) fDrawOpt += "HIST";
   if (gPad->GetTickx() < 2 && fLabelsTopX)
      fDrawOpt += "X+";
   if (gPad->GetTicky() < 2 && fLabelsRightY)
       fDrawOpt += "Y+";
   gStyle->SetEndErrorSize (fEndErrorSize );
   gStyle->SetErrorX       (fErrorX       );
	if ( fLiveGauss )
	   fLiveStat1Dim = 1;
   TEnv env(".hprrc");
   env.SetValue("Set1DimOptDialog.fLiveStat1Dim" , fLiveStat1Dim);
   env.SetValue("Set1DimOptDialog.fLiveGauss"    , fLiveGauss);
   env.SetValue("Set1DimOptDialog.fLiveBG"       , fLiveBG);
   env.SaveLevel(kEnvLocal);

   TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) {
         SetAtt((TH1*)obj);
      } else if ((obj->InheritsFrom("TPad"))) {
         TPad *pad = (TPad*)obj;
         TIter next1(pad->GetListOfPrimitives());
         TObject *obj1;
         pad->cd();
			while ( (obj1 = next1()) ) {
				if (obj1->InheritsFrom("TH1")) {
					SetAtt((TH1*)obj1);
            }
         }
	      pad->Modified();
	      pad->Update();
      }
   }
/*
	if (fLiveStat1Dim) {
		if (!canvas->GetAutoExec())
			canvas->ToggleAutoExec();
   } else {
       if (canvas->GetAutoExec())
          canvas->ToggleAutoExec();
   }
*/
	canvas->Pop();
	canvas->cd();
	canvas->Modified();
	canvas->Update();
}
//____________________________________________________________

void Set1DimOptDialog::SetAtt(TH1* hist)
{
//	cout << "Set1DimOptDialog::SetAtt:fDrawOpt1Dim " << fDrawOpt << endl;
//			<< " hist: " << hist->GetName()
//			<< " canvas: " << fCanvas->GetName() << endl;
	if (fFill1Dim) {
		hist->SetFillColor(fHistFillColor);
		hist->SetFillStyle(fHistFillStyle);
	} else {
		hist->SetFillStyle(0);
	}
	hist->SetLineColor(fHistLineColor);
	hist->SetLineStyle(fHistLineStyle);
	hist->SetLineWidth(fHistLineWidth);
	hist->SetMarkerColor(fMarkerColor);
	hist->SetMarkerStyle(fMarkerStyle);
   if (fErrorMode == "E1" && fMarkerSize == 0) {
      hist->SetMarkerSize(0.01);
   } else {
	   hist->SetMarkerSize(fMarkerSize);
   }
	hist->SetDrawOption(fDrawOpt);
}
//______________________________________________________________________

void Set1DimOptDialog::SetHistAttPermLocal()
{
   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void Set1DimOptDialog::SetDefaults()
{
//   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
   TEnv env(".hprrc");
   gStyle->SetHistFillColor(env.GetValue("Set1DimOptDialog.fHistFillColor", 2));
   gStyle->SetHistLineColor(env.GetValue("Set1DimOptDialog.fHistLineColor", 1));
   gStyle->SetHistFillStyle(env.GetValue("Set1DimOptDialog.fHistFillStyle", 0));
   gStyle->SetHistLineStyle(env.GetValue("Set1DimOptDialog.fHistLineStyle", 1));
   gStyle->SetHistLineWidth(env.GetValue("Set1DimOptDialog.fHistLineWidth", 2));
   gStyle->SetEndErrorSize (env.GetValue("Set1DimOptDialog.fEndErrorSize", 0.01));
   gStyle->SetErrorX       (env.GetValue("Set1DimOptDialog.fErrorX", 0));
	TString temp =  env.GetValue("Set1DimOptDialog.fErrorMode", "");
   if (  temp != "none" )
      gStyle->SetDrawOption(temp);
   else
      gStyle->SetDrawOption("");
   gStyle->SetMarkerColor     (env.GetValue("Set1DimOptDialog.MarkerColor",       1));
   gStyle->SetMarkerStyle     (env.GetValue("Set1DimOptDialog.MarkerStyle",       7));
   gStyle->SetMarkerSize      (env.GetValue("Set1DimOptDialog.MarkerSize",      1.));
}
//______________________________________________________________________

void Set1DimOptDialog::SetHistAttPerm()
{
//   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
   SaveDefaults();
   gStyle->SetHistFillColor(fHistFillColor);
   gStyle->SetHistLineColor(fHistLineColor);
   gStyle->SetHistFillStyle(fHistFillStyle);
   gStyle->SetHistLineStyle(fHistLineStyle);
   gStyle->SetHistLineWidth(fHistLineWidth);
   gStyle->SetEndErrorSize (fEndErrorSize );
   gStyle->SetErrorX       (fErrorX       );
   if (fErrorMode != "none")
      gStyle->SetDrawOption(fErrorMode);
   else
      gStyle->SetDrawOption("");
   gStyle->SetMarkerColor  (fMarkerColor);
   gStyle->SetMarkerStyle  (fMarkerStyle);
   gStyle->SetMarkerSize   (fMarkerSize );
}
//______________________________________________________________________

void Set1DimOptDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("Set1DimOptDialog.fFill1Dim",      fFill1Dim     );
   env.SetValue("Set1DimOptDialog.fHistFillColor", fHistFillColor);
   env.SetValue("Set1DimOptDialog.fHistLineColor", fHistLineColor);
   env.SetValue("Set1DimOptDialog.fHistFillStyle", fHistFillStyle);
   env.SetValue("Set1DimOptDialog.fHistLineStyle", fHistLineStyle);
   env.SetValue("Set1DimOptDialog.fHistLineWidth", fHistLineWidth);
   env.SetValue("Set1DimOptDialog.fEndErrorSize" , fEndErrorSize);
   env.SetValue("Set1DimOptDialog.fErrorX",        fErrorX);
   env.SetValue("Set1DimOptDialog.fErrorMode",     fErrorMode);
	env.SetValue("Set1DimOptDialog.MarkerColor",    fMarkerColor     );
	env.SetValue("Set1DimOptDialog.MarkerStyle",    fMarkerStyle     );
	env.SetValue("Set1DimOptDialog.MarkerSize",     fMarkerSize      );

   env.SetValue("Set1DimOptDialog.fLiveStat1Dim"  , fLiveStat1Dim);
   env.SetValue("Set1DimOptDialog.fLiveGauss"     , fLiveGauss);
   env.SetValue("Set1DimOptDialog.fLiveBG"        , fLiveBG);
   env.SetValue("Set1DimOptDialog.fDrawAxisAtTop" , fDrawAxisAtTop);
   env.SetValue("Set1DimOptDialog.fShowContour"   , fShowContour);
   env.SetValue("Set1DimOptDialog.fLabelsTopX"    , fLabelsTopX);
   env.SetValue("Set1DimOptDialog.fLabelsRightY"  , fLabelsRightY);

   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void Set1DimOptDialog::RestoreDefaults()
{
   if ( !fHist ) return;
   TEnv env(".hprrc");
   fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
   fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
	if ( fLiveGauss )
	   fLiveStat1Dim = 1;
   fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
   fDrawAxisAtTop = env.GetValue("Set1DimOptDialog.fDrawAxisAtTop", 0);
	
	fEndErrorSize  = gStyle->GetEndErrorSize();
	fErrorX        = gStyle->GetErrorX();
	fFill1Dim      = fHist->GetFillStyle() != 0;
	fHistFillColor = fHist->GetFillColor();
	fHistFillStyle = fHist->GetFillStyle();
	fHistLineColor = fHist->GetLineColor  ();
	fHistLineStyle = fHist->GetLineStyle  ();
	fHistLineWidth = fHist->GetLineWidth  ();
	fMarkerColor   = fHist->GetMarkerColor();
	fMarkerStyle   = fHist->GetMarkerStyle();
	fMarkerSize    = fHist->GetMarkerSize();
	TString drawopt= fHist->GetDrawOption();
	
	fFill1Dim      = fHist->GetFillStyle() != 0;
	if (drawopt.Length() < 1) {
	   fErrorMode = "none";
	   fShowContour = 1;
		fLabelsTopX = 0;
		fLabelsRightY = 0;
   } else {
		if (drawopt.Contains("X+"))  fLabelsTopX = 1;
		else                         fLabelsTopX = 0;
		if (drawopt.Contains("Y+"))  fLabelsRightY = 1;
		else                         fLabelsRightY = 0;
		if (drawopt.Contains("HIST"))fShowContour = 1;
		else                         fShowContour = 0;
		TString em;
		fErrorMode = "none";
		if (drawopt.Contains("E")) {
		   fErrorMode = "E";
		   for (Int_t i=1; i<=6; i++) {
				em = "E";
				em += i;
				if ( drawopt.Contains(em.Data()) ) 
				  fErrorMode = em;
			}
		}
	}
}
//______________________________________________________________________

void Set1DimOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void Set1DimOptDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetHistAttNow(canvas);
}

