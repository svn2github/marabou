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
#include "TF1.h"
#include "TH1.h"
#include "TH3.h"
#include "TStyle.h"
#include "TSystem.h"
#include "Set3DimOptDialog.h"
#include <iostream>

Double_t gTranspThresh = 1;
Double_t gTranspAbove = 0.4;
Double_t gTranspBelow = 0.005;

Double_t my_transfer_function(const Double_t *x, const Double_t * /*param*/)
{
   // Bin values in our example range from -2 to 1.
   // Let's make values from -2. to -1.5 more transparent:
   if (*x < gTranspThresh) {
      return gTranspBelow;
	} else{
		return gTranspAbove;
	}
}


namespace std {} using namespace std;

//TString Set3DimOptDialog::fDrawOpt3Dim = "COLZ";
//_______________________________________________________________________

Set3DimOptDialog::Set3DimOptDialog(Int_t batch)
{
	if (batch);
	cout << "ctor Set2DimOptDialog, non interactive" <<endl;
	fDialog = NULL;
}
//_______________________________________________________________________

Set3DimOptDialog::Set3DimOptDialog(TGWindow * win)
{
static const Char_t helptext[] =
"Note: Changeing options only influence the current histogram\n\
       To make them active for subsequently displayed histograms\n\
		 press: \"Set as global default\"\n\
\n\
Most of the value in this widget are self explaining\n\
If \"Live statbox\" a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values.\n\
\n\
\" \"  : Draw a scatter-plot, swarm of points\n\
BOX   : a box is drawn for each cell with surface proportional to the\n\
          content's absolute value. \n\
BOX1  : With GL option: Draw a sphere instead of box.\n\
ISO   :	Draw a Gouraud shaded 3d iso surface through a 3d histogram.\n\
         It paints one surface at the value computed as follow: \n\
         SumOfWeights/(NbinsX*NbinsY*NbinsZ). \n\
\n\
Option GL:\n\
Use OpenGL to display histogram together with BOX, BOX1 or ISO options\n\
With BOX options axis are displayed\n\
To enhance part of the plot the transparency of the bins may be adjusted\n\
according to their bin content. Typically the treshold is set to one\n\
the transparency \"below\" to a small value (tranparent) and a \"above\"\n\
to e.g. 0.5. In this way non filled bin are invisible.\n\
\n\
Caveat: When switching between GL and non GL the histogram should be\n\
redisplayed. I.e. Set the option, \"Set as global default\" and then\n\
display the histogram again.\n\
\n\
For further details contact ROOTs documentation.\n\
";

   const char *fDrawOpt3[4] =
   {"SCAT", "BOX0", "BOX1", " ISO"};
	
//	gTranspThresh = 1;
//	gTranspAbove = 0.4;
//	gTranspBelow = 0.005;
	fApplyTranspCut = 0;
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
	GetValuesFromHist();
   Int_t selected = -1;
   for (Int_t i = 0; i < 4; i++) {
      fDrawOpt3DimArray[i] = fDrawOpt3[i];
		TString temp(fDrawOpt3[i]);
		temp = temp.Strip(TString::kBoth);
      if (fDrawOpt3Dim.Contains(temp) ) {
         if (selected < 0) {
				fOptRadio[i] = 1;
            selected = i;
         }
      } else {
         fOptRadio[i] = 0;
      }
   }
	
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
   Int_t indopt = 0;
//    static Int_t dummy;
   static TString stycmd("SetHistAttPermLocal()");
   static TString sadcmd("SetAllToDefault()");

	fBidSCAT = 1;
	fBidBOX  = 2;
	fBidBOX1 = 3;
   for (Int_t i = 0; i < 4; i++) {
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt3DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
//   cout << " indopt  " <<  indopt << endl;
   fRow_lab->Add(new TObjString("ColorSelect_BgColor"));
   fValp[ind++] = &f3DimBackgroundColor;
   fRow_lab->Add(new TObjString("ColorSelect+FillC"));
   fBidFillColor = ind; fValp[ind++] = &fHistFillColor3Dim;
   fRow_lab->Add(new TObjString("ColorSelect+LineC"));
   fBidLineColor = ind; fValp[ind++] = &fHistLineColor3Dim;

   fRow_lab->Add(new TObjString("ColorSelect_MColor"));
   fBidMarkerColor = ind; fValp[ind++] = &fMarkerColor3Dim;
   fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
   fBidMarkerStyle = ind; fValp[ind++] = &fMarkerStyle3Dim;
   fRow_lab->Add(new TObjString("Float_Value+MSize"));
   fBidMarkerSize = ind; fValp[ind++] = &fMarkerSize3Dim;
	
	fRow_lab->Add(new TObjString("CheckButton_Use GL"));
	fValp[ind++] = &fUseGL;
	fRow_lab->Add(new TObjString("CheckButton+Apply Transp"));
	fValp[ind++] = &fApplyTranspCut;
   fRow_lab->Add(new TObjString("DoubleValue+Threshold"));
   fValp[ind++] = &gTranspThresh;
   fRow_lab->Add(new TObjString("DoubleValue_Transp below"));
   fValp[ind++] = &gTranspBelow;
   fRow_lab->Add(new TObjString("DoubleValue+Transp above"));
   fValp[ind++] = &gTranspAbove;


   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;
   fRow_lab->Add(new TObjString("CommandButt+Reset all to default"));
   fValp[ind++] = &sadcmd;

   static Int_t ok;
   Int_t itemwidth = 360;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

void Set3DimOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
      CloseDialog();
   }
}
//_______________________________________________________________________

void Set3DimOptDialog::CloseDialog()
{
//   cout << "Set3DimOptDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void Set3DimOptDialog::SetHistAttNow(TCanvas *canvas)
{
   for (Int_t i = 0; i < 4; i++) {
      if (fOptRadio[i] == 1) {
//			cout << "fDrawOpt3DimArray[" << i << "]" << endl;
         fDrawOpt3Dim = fDrawOpt3DimArray[i];
        if (fUseGL && !fDrawOpt3Dim.Contains("GL"))
			  fDrawOpt3Dim.Prepend("GLCOL");
      }
   }
   if (!canvas) return;
   Set3DimOptDialog::SetHistAtt(canvas);
}
//_______________________________________________________________________

void Set3DimOptDialog::SetHistAtt(TCanvas *canvas)
{
   if (!canvas) return;
	canvas->cd();
	TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   fDrawOpt = fDrawOpt3Dim;
	if (fUseGL && ( fDrawOpt.Contains("BOX") ||fDrawOpt.Contains("ISO"))) {
		fDrawOpt.Prepend("GLCOL");
		gStyle->SetCanvasPreferGL(kTRUE);

	} else {
		gStyle->SetCanvasPreferGL(kFALSE);
	}
		
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TGraph2D")) {
         ((TGraph2D*)obj)->SetDrawOption(fDrawOpt);
      } else if (obj->InheritsFrom("TH3")) {
			TList * lof = ((TH3*)obj)->GetListOfFunctions();
			TObject * trf= lof->FindObject("TransferFunction");
			if ( fApplyTranspCut ) {
				if ( !trf ) {
					TF1 * tf = new TF1("TransferFunction", my_transfer_function);
					lof->Add(tf);
				}
			} else {
				if (trf) {
					lof->Remove(trf);
					delete trf;
				}
			}
         ((TH3*)obj)->SetDrawOption(fDrawOpt);
         ((TH3*)obj)->SetOption(fDrawOpt);
			((TH3*)obj)->SetFillColor(fHistFillColor3Dim);
			((TH3*)obj)->SetLineColor(fHistLineColor3Dim);
			((TH3*)obj)->SetLineWidth(1);
			((TH3*)obj)->SetLineStyle(1);
			((TH3*)obj)->SetFillStyle(0);
			((TH3*)obj)->SetMarkerColor(fMarkerColor3Dim);  
			((TH3*)obj)->SetMarkerStyle(fMarkerStyle3Dim);  
			((TH3*)obj)->SetMarkerSize (fMarkerSize3Dim);   			
      } else if ((obj->InheritsFrom("TPad"))) {
         TPad *pad = (TPad*)obj;
         TIter next1(pad->GetListOfPrimitives());
         TObject *obj1;
         pad->cd();
			while ( (obj1 = next1()) ) {
				if (obj1->InheritsFrom("TH3")) {
               ((TH3*)obj1)->SetDrawOption(fDrawOpt);
               ((TH3*)obj1)->SetOption(fDrawOpt);
            }
         }
	      pad->Modified();
	      pad->Update();
      }
   }
   if (f3DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         canvas->GetFrame()->SetFillStyle(0);
      } else {
      	canvas->GetFrame()->SetFillStyle(1001);
      	canvas->GetFrame()->SetFillColor(10);
   	}
   } else {
      canvas->GetFrame()->SetFillStyle(1001);
      canvas->GetFrame()->SetFillColor(f3DimBackgroundColor);
   }
	canvas->Pop();
	canvas->Modified();
	canvas->Update();
}
//______________________________________________________________________

void Set3DimOptDialog::SetHistAttPermLocal()
{
//   cout << "Set3DimOptDialog:: SetHistAttPerm()" << endl;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void Set3DimOptDialog::SetHistAttPerm()
{
   cout << "Set3DimOptDialog:: SetHistAttPerm()" << endl;
   TEnv env(".hprrc");
   env.SetValue("Set3DimOptDialog.fDrawOpt3Dim", fDrawOpt3Dim);
	env.SetValue("Set3DimOptDialog.f3DimBackgroundColor",f3DimBackgroundColor);
	env.SetValue("Set3DimOptDialog.fHistFillColor3Dim",fHistFillColor3Dim);
	env.SetValue("Set3DimOptDialog.fHistLineColor3Dim",fHistLineColor3Dim);
	env.SetValue("Set3DimOptDialog.fMarkerColor3Dim",  fMarkerColor3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerStyle3Dim",  fMarkerStyle3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerSize3Dim",   fMarkerSize3Dim   );
	env.SetValue("Set3DimOptDialog.fUseGL",            fUseGL            );
	env.SetValue("Set3DimOptDialog.fApplyTranspCut",   fApplyTranspCut   );
	
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set3DimOptDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("Set3DimOptDialog.fDrawOpt3Dim",      fDrawOpt3Dim);
	env.SetValue("Set3DimOptDialog.f3DimBackgroundColor",f3DimBackgroundColor);
	env.SetValue("Set3DimOptDialog.fHistFillColor3Dim",fHistFillColor3Dim);
	env.SetValue("Set3DimOptDialog.fHistLineColor3Dim",fHistLineColor3Dim);
	env.SetValue("Set3DimOptDialog.fMarkerColor3Dim",  fMarkerColor3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerStyle3Dim",  fMarkerStyle3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerSize3Dim",   fMarkerSize3Dim   );
	env.SetValue("Set3DimOptDialog.fUseGL",            fUseGL            );
	env.SetValue("Set3DimOptDialog.fApplyTranspCut",   fApplyTranspCut   );
	env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void Set3DimOptDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void Set3DimOptDialog::GetValuesFromHist()
{
   if ( !fHist ) return;

	fDrawOpt3Dim = fHist->GetOption();
}
//______________________________________________________________________

void Set3DimOptDialog::RestoreDefaults(Int_t resetall)
{
   cout << "Set3DimOptDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
   TEnv env(envname);
	f3DimBackgroundColor = env.GetValue("Set3DimOptDialog.f3DimBackgroundColor", 0);
	fHistFillColor3Dim = env.GetValue("Set3DimOptDialog.fHistFillColor3Dim", 1);
	fHistLineColor3Dim = env.GetValue("Set3DimOptDialog.fHistLineColor3Dim", 1);
	fMarkerColor3Dim   = env.GetValue("Set3DimOptDialog.fMarkerColor3Dim",   1);
	fMarkerStyle3Dim   = env.GetValue("Set3DimOptDialog.fMarkerStyle3Dim",   1);
	fMarkerSize3Dim    = env.GetValue("Set3DimOptDialog.fMarkerSize3Dim",    1);
	fUseGL             = env.GetValue("Set3DimOptDialog.fUseGL",             0);
	fApplyTranspCut    = env.GetValue("Set3DimOptDialog.fApplyTranspCut",    0);
}
//______________________________________________________________________

void Set3DimOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void Set3DimOptDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetHistAttNow(canvas);
}

