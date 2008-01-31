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


   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();
   
   Int_t ind = 0;
   static Int_t dummy;
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

   fValp[ind++] = &fShowTitle;
   fValp[ind++] = &fShowStatBox;
   fValp[ind++] = &fShowDateBox;
   fValp[ind++] = &fUseTimeOfDisplay;
   fValp[ind++] = &dummy;

   Int_t mask = 1;;
   for (Int_t i = 0; i < kNopts; i++) {
      if (((fOptStat / mask) % 10) != 0) fOpts[i] = 1; 
      else                               fOpts[i] = 0; 
      fValp[ind++] = &fOpts[i];
      mask *= 10;
   }
   fValp[ind++] = &fShowFitBox;
      
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
   fOptStat = 0;
   if (fShowStatBox == 1) {
		for (Int_t i = 0; i < kNopts; i++) {
			if (fOpts[i] == 1)
				fOptStat += mask;
			mask *= 10;
		}
   }
   gStyle->SetOptStat(fOptStat);
   gStyle->SetOptFit(fShowFitBox);
   gStyle->SetOptTitle(fShowTitle);
   canvas->Modified();
   canvas->Update();
   SaveDefaults();
}
//_______________________________________________________________________

void WhatToShowDialog::SetDefaults()
{
   TEnv env(".hprrc");
   gStyle->SetOptStat (env.GetValue("WhatToShowDialog.fOptStat", 11111));
   gStyle->SetOptFit  (env.GetValue("WhatToShowDialog.fShowFitBox", 1));
   gStyle->SetOptTitle(env.GetValue("WhatToShowDialog.fShowTitle", 1));
}
//_______________________________________________________________________

void WhatToShowDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   gStyle->SetOptStat(fOptStat);
   gStyle->SetOptFit(fShowFitBox);
   gStyle->SetOptTitle(fShowTitle);
   env.SetValue("WhatToShowDialog.fOptStat", fOptStat);
   env.SetValue("WhatToShowDialog.fShowDateBox", fShowDateBox);
   env.SetValue("WhatToShowDialog.fShowStatBox", fShowStatBox);
   env.SetValue("WhatToShowDialog.fUseTimeOfDisplay", fUseTimeOfDisplay);
   env.SetValue("WhatToShowDialog.fShowTitle", fShowTitle);
   env.SetValue("WhatToShowDialog.fShowFitBox", fShowFitBox);
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void WhatToShowDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fOptStat          = env.GetValue("WhatToShowDialog.fOptStat", 11111);
   fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox", 1);
   fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle", 1);
   fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox", 1);
   fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox", 1);
   fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay", 1);
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
   cout << "CRButtonPressed(" << wid<< ", " <<bid;
   if (obj) cout  << ", " << canvas->GetName() << ")";
   cout << endl;
   SetWhatToShowNow(canvas);
}

