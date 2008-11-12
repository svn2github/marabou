#include "TROOT.h"
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
#include <fstream>
//______________________________________________________________________________

ClassImp(EmptyHistDialog)

enum Ecmds {M_FitFormula = 1001, M_Save2File = 1002};

EmptyHistDialog::EmptyHistDialog(TGWindow * win, Int_t winx,  Int_t winy)
              : fWinx(winx), fWiny(winy) 
{
  
static const Char_t helpText[] = 
"This widget helps to create an empty histogram\n\
which can be used to hold a function or randomly\n\
filled according to a function. The canvas can\n\
divided into pads to allow for more than one histogram\n\
";

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fCommand = "Draw_The_Hist()";
   fCanvas = NULL;
//   fSave2FileDialog = NULL;
//   fDialog = NULL;
   gROOT->GetListOfCleanups()->Add(this);
   RestoreDefaults();
   TList *row_lab = new TList(); 

   row_lab->Add(new TObjString("StringValue_HistName"));
   row_lab->Add(new TObjString("StringValue_Title X  "));
   row_lab->Add(new TObjString("StringValue-Title Y  "));
   row_lab->Add(new TObjString("PlainIntVal_Nbins"));
   row_lab->Add(new TObjString("DoubleValue-Xmin"));
   row_lab->Add(new TObjString("DoubleValue-Xmax"));
   row_lab->Add(new TObjString("DoubleValue_Ymin"));
   row_lab->Add(new TObjString("DoubleValue-Ymax"));

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
   valp[ind++] = &fNbins;
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
   gROOT->GetListOfCleanups()->Remove(this);
};
//__________________________________________________________________________

void EmptyHistDialog::RecursiveRemove(TObject * obj)
{
//   cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
//        << obj << " fSelHist " <<  fSelHist <<  endl;
   if (obj == fCanvas) { 
 //      cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
 //       << obj << " fSelHist " <<  fSelHist <<  endl;
     fCanvas = NULL;
   }
}
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
	fHist = new TH1D(fHistName, fHistName, fNbins, xmin, xmax);
	fHist->Draw();
//	fHist->SetMinimum(ymin);
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
   TGPopupMenu * filemenu = menubar->GetPopup("File");
   if (filemenu) {
      const TList * el = filemenu->GetListOfEntries();
      TGMenuEntry *en = (TGMenuEntry*)el->First();
      filemenu->AddEntry("Save hist to rootfile", M_Save2File, NULL, NULL, en);
      filemenu->Connect("Activated(Int_t)", "EmptyHistDialog", this,
                      "HandleMenu(Int_t)");
   }    
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
   TEnv env(".hprrc");
   env.SetValue("EmptyHistDialog.HistName"		 , fHistName       );
   env.SetValue("EmptyHistDialog.HistSelPad" 	 , fHistSelPad     );
   env.SetValue("EmptyHistDialog.HistNewPad" 	 , fHistNewPad     );
   env.SetValue("EmptyHistDialog.HistXsize"  	 , fHistXsize      );
   env.SetValue("EmptyHistDialog.HistYsize"  	 , fHistYsize      );
   env.SetValue("EmptyHistDialog.HistXtitle" 	 , fHistXtitle     );
   env.SetValue("EmptyHistDialog.HistYtitle" 	 , fHistYtitle     );
   env.SetValue("EmptyHistDialog.Nbins"  		    , fNbins           );
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
   TEnv env(".hprrc");
   fHistName        = env.GetValue("EmptyHistDialog.HistName"  		, "hs");
   fHistSelPad      = env.GetValue("EmptyHistDialog.HistSelPad"		, 0);
   fHistNewPad      = env.GetValue("EmptyHistDialog.HistNewPad"		, 1);
   fHistXsize       = env.GetValue("EmptyHistDialog.HistXsize" 		, 800);
   fHistYsize       = env.GetValue("EmptyHistDialog.HistYsize" 		, 800);
   fHistXtitle      = env.GetValue("EmptyHistDialog.HistXtitle"	   , "Xvalues");
   fHistYtitle      = env.GetValue("EmptyHistDialog.HistYtitle"		, "Yvalues");
   fHistXdiv        = env.GetValue("EmptyHistDialog.HistXdiv"  		, 1);
   fNbins           = env.GetValue("EmptyHistDialog.Nbins"  		   , 100);
   fXaxisMin        = env.GetValue("EmptyHistDialog.XaxisMin"  		, 0);
   fYaxisMin        = env.GetValue("EmptyHistDialog.YaxisMin"  		, 0);
   fXaxisMax        = env.GetValue("EmptyHistDialog.XaxisMax"  		, 0);
   fYaxisMax        = env.GetValue("EmptyHistDialog.YaxisMax"  		, 0);
   fHistYdiv        = env.GetValue("EmptyHistDialog.HistYdiv"  		, 1);
}
//_________________________________________________________________________
            
void EmptyHistDialog::CloseDown(Int_t wid)
{
//   cout << "EmptyHistDialog::CloseDown() " << endl;
   if (fCanvas) delete fCanvas;
   delete this;
}
