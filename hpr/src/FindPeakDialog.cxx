#include "TROOT.h"
#include "TStyle.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TList.h"
#include "TObjString.h"
#include "SetColor.h"
#include <iostream>
#include "FhPeak.h"
#include "FindPeakDialog.h"

using std::cout;
using std::endl;
//____________________________________________________________________________
FindPeakDialog::FindPeakDialog(TH1 * hist, Int_t interactive)
{

static const Char_t helptext[] =
"Find peaks using TSpectrum and add list of peaks\n\
to the histogram. If peak heights vary strongly\n\
use serveral different ranges, uncheck: \"ClearList\"\n\
Parameters:\n\
  - fInteractive: = 0 (unchecked): dont present dialog\n\
  - From, To  : range for peaks to be stored for later fit\n\
  - Sigma     : Assumumd Sigma of the peaks\n\
  - Threshold : Relative height of peak compared to maximum\n\
                in displayed range taken into account\n\
  - 2 Peak Resolution: Minimum separation of peaks in units\n\
                       of sigma (default 3)\n\
  - ClearList: Clear list of found peaks before search\n\
";
   fInteractive = interactive;
   fFindPeakDone = kFALSE;
   fSelHist = hist;
   fParentWindow = NULL; 
   SetBit(kMustCleanup);
   fSelPad = gPad;
   gROOT->GetListOfCleanups()->Add(this);
   TIter next(gROOT->GetListOfCanvases());
   TCanvas *c;
   while ( (c = (TCanvas*)next()) ) {
      if (c->GetListOfPrimitives()->FindObject(fName)) {
         fSelPad = c;
         fSelPad->cd();
//         cout << "fSelPad " << fSelPad << endl;
         break;
      }
   }
   if (fSelPad == NULL) {
     cout << "fSelPad = 0!!" <<  endl;
   } else {
      fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
   }
   TAxis *ax = fSelHist->GetXaxis();
   fFrom = fSelHist->GetBinCenter(ax->GetFirst());
   fTo = fSelHist->GetBinCenter(ax->GetLast());
   RestoreDefaults();
   if ( interactive > 0 ) {
		TList *row_lab = new TList(); 
		static void *valp[50];
		Int_t ind = 0;
		static TString exgcmd("ExecuteFindPeak()");
	
		row_lab->Add(new TObjString("DoubleValue_From"));
		valp[ind++] = &fFrom;
		row_lab->Add(new TObjString("DoubleValue+To"));
		valp[ind++] = &fTo;
		row_lab->Add(new TObjString("DoubleValue_Sigma"));
		valp[ind++] = &fSigma;
		row_lab->Add(new TObjString("DoubleValue+2 Peak Sep[sig]"));
		valp[ind++] = &fTwoPeakSeparation;
		row_lab->Add(new TObjString("DoubleValue_Thresh"));
		valp[ind++] = &fThreshold;
		row_lab->Add(new TObjString("CheckButton+Clear List"));
		valp[ind++] = &fClearList;
		row_lab->Add(new TObjString("CommandButt_Execute Find"));
		valp[ind++] = &exgcmd;
		Int_t itemwidth = 320;
	//   TRootCanvas* fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
	//   cout << "fParentWindow " << fParentWindow << endl;
		Int_t ok = 0;
		fDialog =
		new TGMrbValuesAndText ("FindPeaks", NULL, &ok, itemwidth,
								fParentWindow, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
   }
}
//__________________________________________________________________________

FindPeakDialog::~FindPeakDialog()
{ 
// cout << "dtor FindPeakDialog: " << this << endl;
   gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void FindPeakDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fSelHist) { 
      CloseDialog(); 
   }
}
//__________________________________________________________________________

void FindPeakDialog::ClearList()
{
   cout << " FindPeakDialog::ClearList()" << endl;
   TList * p
   = (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
   if (p) {
      fSelHist->GetListOfFunctions()->Remove(p);
      p->Delete();
      delete p;
   }
}
//__________________________________________________________________________

void FindPeakDialog::ExecuteFindPeak()
{
   TAxis *xa = fSelHist->GetXaxis();
   Int_t fbin = 1;
   Int_t lbin = fSelHist->GetNbinsX();
   if (fFrom > 0) 
      fbin = fSelHist->FindBin(fFrom);
   if (fTo > 0) 
      lbin = fSelHist->FindBin(fTo);
   xa->SetRange(fbin, lbin);
   if (fClearList)
      ClearList();
   TList *lof = fSelHist->GetListOfFunctions();
   TList *p = NULL;
   p = (TList*)lof->FindObject("spectrum_peaklist");
   if (!p) {
      p = new TList();
      p->SetName("spectrum_peaklist");
      fSelHist->GetListOfFunctions()->Add(p);
   }

   Int_t npeaks = 100;
   TSpectrum *s = new TSpectrum(2*npeaks, 1);
   s->SetResolution(fTwoPeakSeparation / 3.);
   Int_t nfound = s->Search(fSelHist,fSigma,"",fThreshold);
   cout  << nfound << " peaks found" << endl;
   Float_t *xpeaks = s->GetPositionX();

   for (Int_t i=0;i<nfound;i++) {
      Float_t xp = xpeaks[i];
      if (xp < fFrom || xp > fTo) continue;
      Int_t bin = fSelHist->GetXaxis()->FindBin(xp);
      Float_t yp = fSelHist->GetBinContent(bin);
      cout << "Pos, cont@Pos " << xp << " " << yp << endl;
		FhPeak *pe = new FhPeak(xp);
		pe->SetWidth(fSigma);
		pe->SetContent(yp);
		p->Add(pe);
   }
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
   fFindPeakDone = kTRUE;
}
//_______________________________________________________________________

void FindPeakDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fFrom      = env.GetValue("FindPeakDialog.fFrom", 0);
   fTo        = env.GetValue("FindPeakDialog.fTo", 2000);
   fThreshold = env.GetValue("FindPeakDialog.fThreshold", 0.001);
   fSigma     = env.GetValue("FindPeakDialog.fSigma", 2);
   fTwoPeakSeparation = env.GetValue("FindPeakDialog.fTwoPeakSeparation", 3);
}
//_______________________________________________________________________

void FindPeakDialog::SaveDefaults()
{
   TEnv env(".rootrc");
   env.SetValue("FindPeakDialog.fFrom",      fFrom     );
   env.SetValue("FindPeakDialog.fTo",        fTo       );
   env.SetValue("FindPeakDialog.fThreshold", fThreshold);
   env.SetValue("FindPeakDialog.fSigma",     fSigma    );
   env.SetValue("FindPeakDialog.fTwoPeakSeparation",fTwoPeakSeparation);
   env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FindPeakDialog::CloseDialog()
{
 //  cout << "FindPeakDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fInteractive >  0) fDialog->CloseWindow();
   delete this;
}
//_______________________________________________________________________

void FindPeakDialog::CloseDown()
{
//   cout << "FindPeakDialog::CloseDown() " << endl;
   SaveDefaults();
   delete this;
}

