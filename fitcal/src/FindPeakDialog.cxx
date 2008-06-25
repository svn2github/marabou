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
#include "PeakFinder.h"
#include "FindPeakDialog.h"

using std::cout;
using std::endl;
//____________________________________________________________________________
FindPeakDialog::FindPeakDialog(TH1 * hist, Int_t interactive)
{

static const Char_t helptext[] =
"Find peaks either using TSpectrum implemented in root (default)\n\
or a simple method based on a shifting a square wave across\n\
the histogram and calculating the convolution integral.\n\
Peaks are added to list associated with the histogram for later\n\
use in fitting procedures.\n\
If peak heights vary strongly use several different ranges\n\
Parameters:\n\
  - fInteractive: = 0 (unchecked): dont present dialog\n\
  - From, To  : range for peaks to be stored for later fit\n\
Paramemeter for TSpectrum method:\n\
  - Sigma     : Assumumd Sigma of the peaks\n\
  - Threshold : Relative height of peak compared to maximum\n\
                in displayed range taken into account\n\
  - 2 Peak Resolution: Minimum separation of peaks in units\n\
                       of sigma, default 3, this the maximum\n\
                       for bigger values sigma must be increased\n\
     !!!!!!!!!  not yet implemented  !!!!!!!!!!!!!!!!!\n\
Parameters for square wave convolution:\n\
  - ThresholdSigma: Corresponds rougly to the significance of the\n\
                    peak (Sigma) above background\n\
  - PeakMWidth:     Should be about the full width of the peak\n\
                    e.g. 2 Sigma correspond to 68 % of the content.\n\
 \n\
";
   fInteractive = interactive;
   fFindPeakDone = 0;
   fSelHist = hist;
   fName = hist->GetName();
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
//         cout << "fSelPad " << fSelPad << " name: " << fName<< endl;
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
//   cout << "fSigma " <<fSigma << endl;
   if ( interactive > 0 ) {
		TList *row_lab = new TList(); 
		static void *valp[50];
		Int_t ind = 0;
		static Int_t dummy = 0;
		static TString exgcmd("ExecuteFindPeak()");
		static TString clfcmd("ClearList()");
	
		row_lab->Add(new TObjString("DoubleValue_From"));
		valp[ind++] = &fFrom;
		row_lab->Add(new TObjString("DoubleValue+To"));
		valp[ind++] = &fTo;
		row_lab->Add(new TObjString("RadioButton_TSpectrum"));
      fUseTSpectrumEntry = ind;
		valp[ind++] = &fUseTSpectrum;
		row_lab->Add(new TObjString("RadioButton+SQWaveConv"));
      fUseSQWaveFoldEntry = ind;
		valp[ind++] = &fUseSQWaveFold;
		row_lab->Add(new TObjString("CommentOnly_Parameters for TSpectrum"));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("DoubleValue_Sigma;0.;"));
      fSigmaEntry = ind;
		valp[ind++] = &fSigma;
//		row_lab->Add(new TObjString("DoubleValue+2 Peak Sep[sig]"));
//		valp[ind++] = &fTwoPeakSeparation;
		row_lab->Add(new TObjString("DoubleValue+Thresh;0.0;0.5"));
      fThresholdEntry = ind;
		valp[ind++] = &fThreshold;
		row_lab->Add(new TObjString("CheckButton_Markow Alg"));
      fMarkowEntry = ind;
		valp[ind++] = &fMarkow;
		row_lab->Add(new TObjString("CheckButton+Remove BG"));
      fRemoveBGEntry = ind;
		valp[ind++] = &fRemoveBG;
 		row_lab->Add(new TObjString("CommentOnly_Parameters for Square Wave Convolution"));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("DoubleValue_ThresholdSigma"));
      fThresholdSigmaEntry = ind;
		valp[ind++] = &fThresholdSigma;
//		row_lab->Add(new TObjString("DoubleValue+2 Peak Sep[sig]"));
//		valp[ind++] = &fTwoPeakSeparation;
		row_lab->Add(new TObjString("DoubleValue+PeakMWidth"));
      fPeakMwidthEntry = ind;
		valp[ind++] = &fPeakMwidth;
		row_lab->Add(new TObjString("CommandButt_Execute Find"));
		valp[ind++] = &exgcmd;
		row_lab->Add(new TObjString("CommandButt+Clear List"));
		valp[ind++] = &clfcmd;
		Int_t itemwidth = 320;
	//   TRootCanvas* fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
	//   cout << "fParentWindow " << fParentWindow << endl;
		Int_t ok = 0;
		fDialog =
		new TGMrbValuesAndText ("FindPeaks", NULL, &ok, itemwidth,
								fParentWindow, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
      CRButtonPressed(0, fUseTSpectrumEntry, NULL);
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
//   cout << "FindPeakDialog: RecursiveRemove " << obj << endl;
   if (obj == fSelPad) { 
//      cout << "FindPeakDialog: CloseDialog "  << endl;
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
   TPolyMarker *pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
      if (pm) {
         fSelHist->GetListOfFunctions()->Remove(pm);
         pm->Delete();
      }
   if (gPad) {
      gPad->Modified();
      gPad->Update();
   }
}
//__________________________________________________________________________

void FindPeakDialog::ExecuteFindPeak()
{
   PeakFinder * pf = NULL;
   TSpectrum *s    = NULL;
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
//   if (fClearList) {
//      ClearList();
//   } else {
      pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
      if (pm) {
          poly.SetPolyMarker(0);
          pm->Copy(poly);
      }
//   }
   TList *lof = fSelHist->GetListOfFunctions();
   TList *p = NULL;
   p = (TList*)lof->FindObject("spectrum_peaklist");
   if (!p) {
      p = new TList();
      p->SetName("spectrum_peaklist");
      fSelHist->GetListOfFunctions()->Add(p);
   }
   Float_t *xpeaks;
   Int_t nfound = 0;
   if (fUseTSpectrum) {
		Int_t npeaks = 100;
		s = new TSpectrum(2*npeaks, 1);
	//   if (fTwoPeakSeparation <= 0) fTwoPeakSeparation = 3.;
	//   s->SetResolution(3. / fTwoPeakSeparation);
	//   cout << " SetResolution " << 3. / fTwoPeakSeparation << endl;
		TString opt("");
		if (!fMarkow) opt += "noMarkov";
		if (!fRemoveBG) opt += "nobackground";
		if (!fShowMarkers) opt += "goff";
		nfound = s->Search(fSelHist,fSigma, opt,fThreshold);
		xpeaks = s->GetPositionX();
   } else {
      pf = new PeakFinder(fSelHist, fPeakMwidth, fThresholdSigma);
      xpeaks = pf->GetPositionX();
      nfound = pf->GetNpeaks();
   }
   if (nfound > 100) nfound = 100;
   for (Int_t i=0;i<nfound;i++) {
      Float_t xp = xpeaks[i];
      if (xp < fFrom || xp > fTo) continue;
      Int_t bin = fSelHist->GetXaxis()->FindBin(xp);
      Float_t yp = fSelHist->GetBinContent(bin);
      if (fInteractive) 
         printf("Pos: %8.1f Cont@Pos:  %8.1f\n", xp, yp);
		FhPeak *pe = new FhPeak(xp);
		pe->SetWidth(fSigma);
		pe->SetContent(yp);
		p->Add(pe);
   }
//   if (!fClearList) {
      pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
      if (!pm) {
         pm = new TPolyMarker();
         fSelHist->GetListOfFunctions()->Add(pm);
      }
      Double_t * xp = poly.GetX();
      Double_t * yp = poly.GetY();
      Int_t np = pm->Size();
      for (Int_t i = 0; i < poly.GetN(); i++) pm->SetPoint(np + i, *xp++, *yp++);
//   }
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
   fFindPeakDone++;
}
//_______________________________________________________________________

void FindPeakDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fFrom      = env.GetValue("FindPeakDialog.fFrom", 0.);
   fTo        = env.GetValue("FindPeakDialog.fTo", 2000.);
   fThreshold = env.GetValue("FindPeakDialog.fThreshold", 0.001);
   fSigma     = env.GetValue("FindPeakDialog.fSigma", 2.);
   fTwoPeakSeparation = env.GetValue("FindPeakDialog.fTwoPeakSeparation", 3.);
   fMarkow    = env.GetValue("FindPeakDialog.fMarkow", 1);
   fRemoveBG  = env.GetValue("FindPeakDialog.fRemoveBG", 1);
   fShowMarkers = env.GetValue("FindPeakDialog.fShowMarkers", 1);
   fUseTSpectrum   = env.GetValue("FindPeakDialog.fUseTSpectrum", 1);
   fUseSQWaveFold  = env.GetValue("FindPeakDialog.fUseSQWaveFold", 0);
   fThresholdSigma = env.GetValue("FindPeakDialog.fThresholdSigma", 2.5);
   fPeakMwidth     = env.GetValue("FindPeakDialog.fPeakMwidth", 5);
}
//_______________________________________________________________________

void FindPeakDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("FindPeakDialog.fFrom",      fFrom     );
   env.SetValue("FindPeakDialog.fTo",        fTo       );
   env.SetValue("FindPeakDialog.fThreshold", fThreshold);
   env.SetValue("FindPeakDialog.fSigma",     fSigma    );
   env.SetValue("FindPeakDialog.fTwoPeakSeparation",fTwoPeakSeparation);
   env.SetValue("FindPeakDialog.fMarkow", fMarkow);
   env.SetValue("FindPeakDialog.fRemoveBG", fRemoveBG);
   env.SetValue("FindPeakDialog.fShowMarkers", fShowMarkers);
   env.SetValue("FindPeakDialog.fUseTSpectrum", fUseTSpectrum);
   env.SetValue("FindPeakDialog.fUseSQWaveFold", fUseSQWaveFold);
   env.SetValue("FindPeakDialog.fThresholdSigma", fThresholdSigma);
   env.SetValue("FindPeakDialog.fPeakMwidth",fPeakMwidth) ;
   env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FindPeakDialog::CloseDialog()
{
//   cout << "FindPeakDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fInteractive >  0) fDialog->CloseWindowExt();
   delete this;
}
//_______________________________________________________________________

void FindPeakDialog::CloseDown(Int_t wid)
{
//   cout << "FindPeakDialog::CloseDown() " << wid<< endl;
   SaveDefaults();
   delete this;
}
//_______________________________________________________________________

void FindPeakDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) 
{
   if (bid == fUseTSpectrumEntry || bid == fUseSQWaveFoldEntry) {
		if (fUseTSpectrum) {
			fDialog->DisableButton(fThresholdSigmaEntry);
			fDialog->DisableButton(fPeakMwidthEntry    );
			fDialog->EnableButton(fThresholdEntry);
			fDialog->EnableButton(fSigmaEntry    );
			fDialog->EnableButton(fMarkowEntry   );
			fDialog->EnableButton(fRemoveBGEntry );
		} else {
			fDialog->DisableButton(fThresholdEntry);
			fDialog->DisableButton(fSigmaEntry    );
			fDialog->DisableButton(fMarkowEntry   );
			fDialog->DisableButton(fRemoveBGEntry );
			fDialog->EnableButton(fThresholdSigmaEntry);
			fDialog->EnableButton(fPeakMwidthEntry    );
		}
   }
//   cout << "FindPeakDialog::Btp " << wid << " " << bid << endl;
}

