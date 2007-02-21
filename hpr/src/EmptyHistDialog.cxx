#include "TEnv.h"
#include "TH1D.h"
#include "TPad.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TString.h"
#ifdef MARABOUVERS
#include "HTCanvas.h"
#else 
#include "TCanvas.h"
#endif
#include "TGMrbValuesAndText.h"
#include "EmptyHistDialog.h"
#include "FitOneDimDialog.h"
#include "Save2FileDialog.h"
#include <fstream>
//______________________________________________________________________________

ClassImp(EmptyHistDialog)

enum Ecmds {M_FitFormula = 1001, M_Save2File = 1002};

EmptyHistDialog::EmptyHistDialog(TGWindow * win, Int_t winx,  Int_t winy)
              : fWinx(winx), fWiny(winy) 
{
  
static const Char_t helpText[] = 
"The graph can be drawn / overlayed in a selected pad.\n\
Default is to construct a new canvas\n\
";

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fCommand = "Draw_The_Hist()";
   fCanvas = NULL;
   RestoreDefaults();
   TList *row_lab = new TList(); 

   row_lab->Add(new TObjString("StringValue_HistName"));
   row_lab->Add(new TObjString("StringValue_Title X  "));
   row_lab->Add(new TObjString("StringValue-Title Y  "));
   row_lab->Add(new TObjString("DoubleValue_Xaxis min"));
   row_lab->Add(new TObjString("DoubleValue-Xaxis max"));
   row_lab->Add(new TObjString("DoubleValue_Yaxis min"));
   row_lab->Add(new TObjString("DoubleValue-Yaxis max"));

   row_lab->Add(new TObjString("CheckButton_Draw/Overlay in a sel pad"));
//   row_lab->Add(new TObjString("CheckButton_Draw in a new canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Xsize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal-Ysize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div X of canvas"));
   row_lab->Add(new TObjString("PlainIntVal-Div Y of canvas"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Hist"));
//   row_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = row_lab->GetSize();

   valp[ind++] = &fHistName;
   valp[ind++] = &fHistXtitle;
   valp[ind++] = &fHistYtitle;
   valp[ind++] = &fXaxisMin;
   valp[ind++] = &fXaxisMax;
   valp[ind++] = &fYaxisMin;
   valp[ind++] = &fYaxisMax;
   valp[ind++] = &fHistSelPad;
//   valp[ind++] = &fHistNewPad;
   valp[ind++] = &fHistXsize;
   valp[ind++] = &fHistYsize;
   valp[ind++] = &fHistXdiv;
   valp[ind++] = &fHistYdiv;
   valp[ind++] = &fCommand;
   Int_t itemwidth = 320;
   ok = GetStringExt("Hists parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};  
//_________________________________________________________________________
            
EmptyHistDialog::~EmptyHistDialog() 
{
   SaveDefaults();
};
//_________________________________________________________________________
            
void EmptyHistDialog::Draw_The_Hist()
{
	cout << "Empty pad only" << endl;
//	TGraph *graph = new TGraph();
   if (fHistSelPad |= 0) {
      if (gPad == NULL) {
         cout << "Please select a pad first" << endl;
         return;
      }
   } else {
#ifdef MARABOUVERS
	   fCanvas = new HTCanvas("Empty", "Empty", fWinx, fWiny,
						fHistXsize, fHistYsize);
#else
	   fCanvas = new TCanvas("Empty", "Empty", fWinx, fWiny,
						fHistXsize, fHistYsize);
#endif
   }
   BuildMenu();
	if (fHistXdiv > 1 || fHistYdiv > 1) {
		fCanvas->Divide(fHistXdiv, fHistYdiv);
		fCanvas->cd(1);
	}
	Double_t xmin = 0, xmax = 100;
	Double_t ymin = 0, ymax = 100;
	if (fXaxisMin != 0 || fXaxisMax != 0) {
		xmin = fXaxisMin;
		xmax = fXaxisMax;
	}
	if (fYaxisMin != 0 || fYaxisMax != 0) {
		ymin = fYaxisMin;
		ymax = fYaxisMax;
	}
	gStyle->SetOptStat(0);
	fHist = new TH1D(fHistName, fHistName, 100, xmin, xmax);
	fHist->Draw();
	fHist->SetMinimum(ymin);
	fHist->SetMaximum(ymax);
	if (fHistXtitle.Length() > 0)
		fHist->GetXaxis()->SetTitle(fHistXtitle.Data());
	if (fHistYtitle.Length() > 0)
		fHist->GetYaxis()->SetTitle(fHistYtitle.Data());
//	graph->SetHistogram(gh);
	gPad->Update();
};
//________________________________________________________________________

void EmptyHistDialog::BuildMenu()
{
//   cout << "EmptyHistDialog::BuildMenu() " <<this << endl;
   fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
   TGMenuBar * menubar = fRootCanvas->GetMenuBar();
   TGLayoutHints * layoh_right = new TGLayoutHints(kLHintsTop | kLHintsLeft);
   fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Draw_Fill", fMenu, layoh_right, menubar->GetPopup("Inspect"));
   fMenu->AddEntry("Draw / fill with user defined function", M_FitFormula);
   fMenu->AddEntry("Save hist to rootfile", M_Save2File);

   fMenu->Connect("Activated(Int_t)", "EmptyHistDialog", this,
                      "HandleMenu(Int_t)");
   
   menubar->MapSubwindows();
   menubar->Layout(); 
}
//_________________________________________________________________________
            
void EmptyHistDialog::SaveDefaults()
{
   cout << "EmptyHistDialog::SaveDefaults() " << endl;
   TEnv env(".rootrc");
   env.SetValue("EmptyHistDialog.HistName"		 , fHistName       );
   env.SetValue("EmptyHistDialog.HistSelPad" 	 , fHistSelPad     );
   env.SetValue("EmptyHistDialog.HistNewPad" 	 , fHistNewPad     );
   env.SetValue("EmptyHistDialog.HistXsize"  	 , fHistXsize      );
   env.SetValue("EmptyHistDialog.HistYsize"  	 , fHistYsize      );
   env.SetValue("EmptyHistDialog.HistXtitle" 	 , fHistXtitle     );
   env.SetValue("EmptyHistDialog.HistYtitle" 	 , fHistYtitle     );
   env.SetValue("EmptyHistDialog.XaxisMin"  		 , fXaxisMin        );
   env.SetValue("EmptyHistDialog.YaxisMin"  		 , fYaxisMin        );
   env.SetValue("EmptyHistDialog.XaxisMax"  		 , fXaxisMax        );
   env.SetValue("EmptyHistDialog.YaxisMax"  		 , fYaxisMax        );
   env.SetValue("EmptyHistDialog.HistXdiv"		 , fHistXdiv       );
   env.SetValue("EmptyHistDialog.HistYdiv"		 , fHistYdiv       );
   env.SaveLevel(kEnvLocal);
}
//________________________________________________________________________

void EmptyHistDialog::HandleMenu(Int_t id)
{
   switch (id) {

      case M_FitFormula:
         new FitOneDimDialog(fHist, 4);
         break;
      case M_Save2File:
         new Save2FileDialog(fHist);
         break;
   }
}
//_________________________________________________________________________
            
void EmptyHistDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fHistName        = env.GetValue("EmptyHistDialog.HistName"  		, "hs");
   fHistSelPad      = env.GetValue("EmptyHistDialog.HistSelPad"		, 0);
   fHistNewPad      = env.GetValue("EmptyHistDialog.HistNewPad"		, 1);
   fHistXsize       = env.GetValue("EmptyHistDialog.HistXsize" 		, 800);
   fHistYsize       = env.GetValue("EmptyHistDialog.HistYsize" 		, 800);
   fHistXtitle      = env.GetValue("EmptyHistDialog.HistXtitle"	   , "Xvalues");
   fHistYtitle      = env.GetValue("EmptyHistDialog.HistYtitle"		, "Yvalues");
   fHistXdiv        = env.GetValue("EmptyHistDialog.HistXdiv"  		, 1);
   fXaxisMin         = env.GetValue("EmptyHistDialog.XaxisMin"  		, 0);
   fYaxisMin         = env.GetValue("EmptyHistDialog.YaxisMin"  		, 0);
   fXaxisMax         = env.GetValue("EmptyHistDialog.XaxisMax"  		, 0);
   fYaxisMax         = env.GetValue("EmptyHistDialog.YaxisMax"  		, 0);
   fHistYdiv        = env.GetValue("EmptyHistDialog.HistYdiv"  		, 1);
}
//_________________________________________________________________________
            
void EmptyHistDialog::CloseDown()
{
//   cout << "EmptyHistDialog::CloseDown() " << endl;
   if (fCanvas) delete fCanvas;
   delete this;
}
