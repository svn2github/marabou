#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "WhatToShowDialog.h"
#include <iostream>

namespace std {} using namespace std;

//TString WhatToShowDialog::fDrawOpt2Dim = "COLZ";
//_______________________________________________________________________

WhatToShowDialog::WhatToShowDialog(TGWindow * win)
{
static const Char_t helptext[] =
"This determines what to show for a histogram,\n\
especially which values to display in the statistics box";


   gROOT->GetListOfCleanups()->Add(this);
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
// is it 1 or 2dim
   fDim = 0;
   TIter next(fCanvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) 
        fDim = ((TH1*)obj)->GetDimension();
   }
   if ( fDim == 0 )
      cout << "WARNING: No hist found in pad" << endl;
   fRow_lab = new TList();
   
   Int_t ind = 0;
   static Int_t dummy;
   static TString stycmd("SaveDefaults()");
//   static TString stycmd("SetWhatToShowPermLocal()");

   RestoreDefaults();
   fRow_lab->Add(new TObjString("CheckButton_Hist Title"));
   fRow_lab->Add(new TObjString("CheckButton+Stat Box"));
   fRow_lab->Add(new TObjString("CheckButton_Date Box"));
   fRow_lab->Add(new TObjString("CheckButton+Use time of display"));
   fRow_lab->Add(new TObjString("CommentOnly_Content in Statistics Box"));
   fRow_lab->Add(new TObjString("CheckButton_Name of Hist"));
   fRow_lab->Add(new TObjString("CheckButton+Number of entries"));
   fRow_lab->Add(new TObjString("CheckButton_Mean"));
   fRow_lab->Add(new TObjString("CheckButton+RMS"));
   fRow_lab->Add(new TObjString("CheckButton+Underflow"));
   fRow_lab->Add(new TObjString("CheckButton+Overflow"));
   fRow_lab->Add(new TObjString("CheckButton_Integral"));
   fRow_lab->Add(new TObjString("CheckButton+Fit Pars"));
   fValp[ind++] = &fShowTitle ;
   fValp[ind++] = &fShowStatBox ;
	fValp[ind++] = &fShowDateBox ;
	fValp[ind++] = &fUseTimeOfDisplay ;
   fValp[ind++] = &dummy;

   Int_t mask = 1;;
   for (Int_t i = 0; i < kNopts; i++) {
      if (((fOptStat / mask) % 10) != 0) fOpts[i] = 1; 
      else                               fOpts[i] = 0; 
      fValp[ind++] = &fOpts[i];
      mask *= 10;
   }
   fValp[ind++] = &fShowFitBox;
   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;
     
 //  fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
//   fValp[ind++] = &stycmd;

   static Int_t ok; 
   Int_t itemwidth = 360;
   fDialog = 
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

void WhatToShowDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) { 
 //     cout << "WhatToShowDialog: CloseDialog "  << endl;
      CloseDialog(); 
   }
}
//_______________________________________________________________________

void WhatToShowDialog::CloseDialog()
{
//   cout << "WhatToShowDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void WhatToShowDialog::SetWhatToShowNow(TCanvas *canvas)
{
   if (!canvas) return;
   Int_t mask = 1;
//   Int_t save_OptStat  = gStyle->GetOptStat();
//   Int_t save_OptFit   = gStyle->GetOptFit();
//   Int_t save_OptTitle = gStyle->GetOptTitle();

   Int_t save_opt = 0, active_opt = 0;
   save_opt = fOptStat;
	for (Int_t i = 0; i < kNopts; i++) {
	   if (fOpts[i] == 1)
	      active_opt += mask;
	    mask *= 10;
	}
   gStyle->SetOptStat(active_opt);
   gStyle->SetOptFit(fShowFitBox);
	gStyle->SetOptTitle(fShowTitle);
   TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) {
         TH1* hh = (TH1*)obj;
		   if ( active_opt && fShowStatBox ) {
            if ( active_opt != save_opt ) {
				   hh->SetStats(0);
//				   cout << "hh->SetStats(0); " << endl;
               canvas->Modified();
               canvas->Update();
            }
				hh->SetStats(1);
//				cout << "hh->SetStats(1); " << endl;
		   } else {
				hh->SetStats(0);
//				cout << "hh->SetStats(0); "<< hh << endl;
		   } 
      }
   }
   canvas->Modified();
   canvas->Update();
   fOptStat = active_opt;
//   gStyle->SetOptStat(save_OptStat);
//   gStyle->SetOptFit(save_OptFit);
//   gStyle->SetOptTitle(save_OptTitle);
//   SaveDefaults();
}
//_______________________________________________________________________

void WhatToShowDialog::SetDefaults()
{
   TEnv env(".hprrc");
   gStyle->SetOptStat (env.GetValue("WhatToShowDialog.fOptStat1Dim", 11111));
   gStyle->SetOptFit  (env.GetValue("WhatToShowDialog.fShowFitBox1Dim", 1));
   gStyle->SetOptTitle(env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1));
}
//_______________________________________________________________________

void WhatToShowDialog::SaveDefaults()
{
   TEnv env(".hprrc");
//   cout << " fOptStat "<< fOptStat<< " dim "  << fDim << endl;
   if (fDim == 1) {
//		gStyle->SetOptStat(fOptStat1Dim);
//		gStyle->SetOptFit(fShowFitBox1Dim);
//		gStyle->SetOptTitle(fShowTitle1Dim);
		env.SetValue("WhatToShowDialog.fOptStat1Dim",          fOptStat);
		env.SetValue("WhatToShowDialog.fShowDateBox1Dim",      fShowDateBox);
		env.SetValue("WhatToShowDialog.fShowStatBox1Dim",      fShowStatBox);
		env.SetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", fUseTimeOfDisplay);
		env.SetValue("WhatToShowDialog.fShowTitle1Dim",        fShowTitle);
		env.SetValue("WhatToShowDialog.fShowFitBox1Dim",       fShowFitBox);
   } else {
//		gStyle->SetOptStat(fOptStat2Dim);
//		gStyle->SetOptFit(fShowFitBox2Dim);
//		gStyle->SetOptTitle(fShowTitle2Dim);
		env.SetValue("WhatToShowDialog.fOptStat2Dim",          fOptStat);
		env.SetValue("WhatToShowDialog.fShowDateBox2Dim",      fShowDateBox);
		env.SetValue("WhatToShowDialog.fShowStatBox2Dim",      fShowStatBox);
		env.SetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", fUseTimeOfDisplay);
		env.SetValue("WhatToShowDialog.fShowTitle2Dim",        fShowTitle);
		env.SetValue("WhatToShowDialog.fShowFitBox2Dim",       fShowFitBox);
   }
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void WhatToShowDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   if (fDim == 1) {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat1Dim", 11111);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox1Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox1Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox1Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", 1);
   } else {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat2Dim", 11111);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox2Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle2Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox2Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox2Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", 1);
   }
   gStyle->SetOptFit(fShowFitBox);
}
//______________________________________________________________________

void WhatToShowDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void WhatToShowDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetWhatToShowNow(canvas);
}

