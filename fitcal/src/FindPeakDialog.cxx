#include "TROOT.h"
#include "TStyle.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TList.h"
#include "TObjString.h"
#include "TPolyMarker.h"
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
   fFindPeakDone = 0;
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
   cout << "fSigma " <<fSigma << endl;
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
		row_lab->Add(new TObjString("CheckButton_Markow Alg"));
		valp[ind++] = &fMarkow;
		row_lab->Add(new TObjString("CheckButton+Remove BG"));
		valp[ind++] = &fRemoveBG;
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
   TPolyMarker poly; 
   TPolyMarker * pm;
   TAxis *xa = fSelHist->GetXaxis();
   Int_t fbin = 1;
   Int_t lbin = fSelHist->GetNbinsX();
   if (fFrom > 0) 
      fbin = fSelHist->FindBin(fFrom);
   if (fTo > 0) 
      lbin = fSelHist->FindBin(fTo);
   xa->SetRange(fbin, lbin);
   if (fClearList) {
      ClearList();
   } else {
      pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
      if (pm) {
          poly.SetPolyMarker(0);
          pm->Copy(poly);
      }
   }
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
   TString opt;
   if (!fMarkow) opt += "noMarkov";
   if (!fRemoveBG) opt += "nobackground";
   if (!fShowMarkers) opt += "goff";
   Int_t nfound = s->Search(fSelHist,fSigma,"",fThreshold);
   cout  << nfound << " peaks found" << endl;
   Float_t *xpeaks = s->GetPositionX();

   for (Int_t i=0;i<nfound;i++) {
      Float_t xp = xpeaks[i];
      if (xp < fFrom || xp > fTo) continue;
      Int_t bin = fSelHist->GetXaxis()->FindBin(xp);
      Float_t yp = fSelHist->GetBinContent(bin);
      printf("Pos: %8.1f Cont@Pos:  %8.1f\n", xp, yp);
		FhPeak *pe = new FhPeak(xp);
		pe->SetWidth(fSigma);
		pe->SetContent(yp);
		p->Add(pe);
   }
   if (!fClearList) {
      pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
      if (!pm) {
         pm = new TPolyMarker();
         fSelHist->GetListOfFunctions()->Add(pm);
      }
      Double_t * xp = poly.GetX();
      Double_t * yp = poly.GetY();
      Int_t np = pm->Size();
      for (Int_t i = 0; i < poly.GetN(); i++) pm->SetPoint(np + i, *xp++, *yp++);
   }
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
   fFindPeakDone++;
}
//_______________________________________________________________________

void FindPeakDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fFrom      = env.GetValue("FindPeakDialog.fFrom", 0.);
   fTo        = env.GetValue("FindPeakDialog.fTo", 2000.);
   fThreshold = env.GetValue("FindPeakDialog.fThreshold", 0.001);
   fSigma     = env.GetValue("FindPeakDialog.fSigma", 2.);
   fTwoPeakSeparation = env.GetValue("FindPeakDialog.fTwoPeakSeparation", 3.);
   fMarkow = env.GetValue("FindPeakDialog.fMarkow", 1);
   fRemoveBG = env.GetValue("FindPeakDialog.fRemoveBG", 1);
   fShowMarkers = env.GetValue("FindPeakDialog.fShowMarkers", 1);
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
   env.SetValue("FindPeakDialog.fMarkow", fMarkow);
   env.SetValue("FindPeakDialog.fRemoveBG", fRemoveBG);
   env.SetValue("FindPeakDialog.fShowMarkers", fShowMarkers);
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

