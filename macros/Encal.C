//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Encal.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .x Encal.C(const Char_t * HistoFile,
//                               Int_t CalSource,
//                               const Char_t * CalFile,
//                               const Char_t * ResFile,
//                               const Char_t * PrecalFile,
//                               Int_t LowerLim,
//                               Int_t UpperLim,
//                               Int_t PeakFrac,
//                               Double_t Sigma,
//                               Int_t FitMode,
//                               Int_t FitGrouping,
//                               Double_t FitRange,
//                               Int_t DisplayFlag,
//                               Bool_t VerboseMode)
// Arguments:        Char_t * HistoFile        -- Histogram file (.root)
//                   Int_t CalSource           -- Calibration source
//                   Char_t * CalFile          -- Calibration output file (.cal)
//                   Char_t * ResFile          -- Results file (.res)
//                   Char_t * PrecalFile       -- Precalibration file (.cal, needed if Eu152)
//                   Int_t LowerLim            -- Lower limit [chn]
//                   Int_t UpperLim            -- Upper limit [chn]
//                   Int_t PeakFrac            -- Threshold for peak finder [% of max peak]
//                   Double_t Sigma            -- Sigma
//                   Int_t FitMode             -- Fit mode
//                   Int_t Grouping            -- FitGrouping
//                   Double_t FitRange         -- Range for single peak fit [sigma]
//                   Int_t DisplayFlag         -- Display results
//                   Bool_t VerboseMode        -- Verbose mode
// Description:      Energy calibration for 1-dim histograms
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: Encal.C,v 1.6 2007-02-01 13:47:07 Rudolf.Lutter Exp $
// Date:             Thu Feb  1 08:31:26 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               
//                   Aclic:               +g
//                   NofArgs:             14
//                   Arg1.Name:           HistoFile
//                   Arg1.Title:          Histogram file (.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      FObjListBox
//                   Arg1.Default:        none.root
//                   Arg1.Values:         ROOT files:*.root
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           CalSource
//                   Arg2.Title:          Calibration source
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        1
//                   Arg2.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source (Co60 precalibration needed)=2:TripleAlpha|calibrate using alpha source (Pu239/Am241/Cm244)=4
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           CalFile
//                   Arg3.Title:          Calibration output file (.cal)
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      File
//                   Arg3.Width:          150
//                   Arg3.Default:        Encal.cal
//                   Arg3.Values:         Calib files:*.cal
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           ResFile
//                   Arg4.Title:          Results file (.res)
//                   Arg4.Type:           Char_t *
//                   Arg4.EntryType:      File
//                   Arg4.Width:          150
//                   Arg4.Default:        Encal.res
//                   Arg4.Values:         Results file:*.res
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           PrecalFile
//                   Arg5.Title:          Precalibration file (.cal, needed if Eu152)
//                   Arg5.Type:           Char_t *
//                   Arg5.EntryType:      File
//                   Arg5.Width:          150
//                   Arg5.Default:        Co60.cal
//                   Arg5.Values:         Calib files:*.cal
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           LowerLim
//                   Arg6.Title:          Lower limit [chn]
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      UpDown-X
//                   Arg6.Default:        0
//                   Arg6.AddLofValues:   No
//                   Arg6.LowerLimit:     0
//                   Arg6.UpperLimit:     4096
//                   Arg6.Increment:      100
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           UpperLim
//                   Arg7.Title:          Upper limit [chn]
//                   Arg7.Type:           Int_t
//                   Arg7.EntryType:      UpDown-X
//                   Arg7.Default:        4096
//                   Arg7.AddLofValues:   No
//                   Arg7.LowerLimit:     0
//                   Arg7.UpperLimit:     4096
//                   Arg7.Increment:      100
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           PeakFrac
//                   Arg8.Title:          Threshold for peak finder [% of max peak]
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      UpDown-X
//                   Arg8.Default:        1
//                   Arg8.AddLofValues:   No
//                   Arg8.LowerLimit:     1
//                   Arg8.UpperLimit:     50
//                   Arg8.Increment:      1
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           Sigma
//                   Arg9.Title:          Sigma
//                   Arg9.Type:           Double_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        3
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     0
//                   Arg9.UpperLimit:     10
//                   Arg9.Increment:      .2
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          FitMode
//                   Arg10.Title:         Fit mode
//                   Arg10.Type:          Int_t
//                   Arg10.EntryType:     Radio
//                   Arg10.Default:       1
//                   Arg10.Values:        gaus|fit gaussian distribution=1:gaus+tail|fit gaussian + exp tail on left side=2
//                   Arg10.AddLofValues:  No
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          FitGrouping
//                   Arg11.Title:         Fit grouping
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     Radio
//                   Arg11.Default:       1
//                   Arg11.Values:        single peak|fit each peak separately=1:ensemble|group peaks before fitting=2
//                   Arg11.AddLofValues:  No
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          FitRange
//                   Arg12.Title:         Range for single peak fit [sigma]
//                   Arg12.Type:          Double_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       3
//                   Arg12.AddLofValues:  No
//                   Arg12.LowerLimit:    0
//                   Arg12.UpperLimit:    10
//                   Arg12.Increment:     .5
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          DisplayFlag
//                   Arg13.Title:         Display results
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     Check
//                   Arg13.Default:       1
//                   Arg13.Values:        Step|show each fit=1:2dim|show 2-dim histo after calibration=2
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          VerboseMode
//                   Arg14.Title:         Verbose mode
//                   Arg14.Type:          Bool_t
//                   Arg14.EntryType:     YesNo
//                   Arg14.Default:       1
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <fstream>
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TVirtualX.h"
#include "TButton.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TEnv.h"
#include "TGFrame.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TPolyMarker.h"
#include "TMrbLogger.h"
#include "SetColor.h"


enum	{ kCalSourceCo60 = 1 };
enum	{ kCalSourceEu152 = 2 };
enum	{ kCalSourceTripleAlpha = 4 };
enum	{ kFitModeGaus = 1 };
enum	{ kFitModeGausTail = 2 };
enum	{ kFitGroupingSinglePeak = 1 };
enum	{ kFitGroupingEnsemble = 2 };
enum	{ kDisplayFlagStep = 1 };
enum	{ kDisplayFlag2dim = 2 };

enum	{	kButtonOk = 1,
			kButtonDiscard,
			kButtonStop,
			kButtonQuit
		};

// global vars

TMrbLogger * msg = NULL;

// fitting
Double_t nofPeaks = 1;
Int_t binWidth = 1;
Double_t tailSide = 1;

// calibration
TString calSource;

Bool_t isTripleAlpha = kFALSE;
Bool_t isCo60 = kFALSE;
Bool_t isEu152 = kFALSE;
Bool_t doCal = kFALSE;
Int_t peaksNeeded = 0;

// canvas & display
Int_t buttonFlag = 0;
Bool_t stepIt = kFALSE;
Bool_t show2dim = kFALSE;
TCanvas * canv = NULL;
TButton * statusLine;

Double_t gaus_lbg(Double_t * x, Double_t * par)
{
/*
  par[0]   background constant
  par[1]   background slope
  par[2]   gauss width
  par[3]   gauss0 constant
  par[4]   gauss0 mean
  par[5]   gauss1 constant
  par[6]   gauss1 mean
  par[7]   gauss2 constant
  par[8]   gauss2 mean
  ...
*/
   static Float_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[0];
   Double_t bgslope = par[1];
   Double_t sigma   = par[2];
   if (sigma == 0) sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < nofPeaks; i ++) {
      arg = (x[0] - par[4 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return binWidth * fitval;
}

Double_t gaus_tail(Double_t * x, Double_t * par)
{
/*
  par[0]   tail fraction
  par[1]   tail width
  par[2]   background constant
  par[3]   background slope
  par[4]   gauss width
  par[5]   gauss0 constant
  par[6]   gauss0 mean
  par[7]   gauss1 constant
  par[8]   gauss1 mean
  par[9]   gauss2 constant
  par[10]  gauss2 mean
  ...
*/
   static Double_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t xij;
   Double_t arg;
   Double_t tail;
   Double_t g2b;
   Double_t err;
   Double_t norm;
   Double_t fitval = 0;
   Double_t tailfrac  = par[0];
   Double_t tailwidth = par[1];
   Double_t bgconst   = par[2];
   Double_t bgslope   = par[3];
   Double_t sigma = par[4];

//  force widths /= 0
   if (tailwidth == 0)
      tailwidth = 1;
   if (sigma == 0)
      sigma = 1;

   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < nofPeaks; i++) {
      xij = (x[0] - par[6 + 2 * i]);
      arg = xij / (sqrt2 * sigma);
      xij *= tailSide;
      tail = exp(xij / tailwidth) / tailwidth;
      g2b = 0.5 * sigma / tailwidth;
      err = 0.5 * tailfrac * par[5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
      norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
      fitval = fitval + norm * err * tail 
                      + par[5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return binWidth * fitval;
}

TCanvas * DrawCanvas() {
	Int_t nBtns = 4;
	Int_t canvw = 800;
	Int_t canvh = 600;
	Double_t xpx = 1./canvw;
	Double_t x0 = 100 * xpx;
	Double_t wdth = (canvw - 200) * xpx / nBtns;
	Double_t y0 = 0.03;
	Double_t y1 = 0.08;
	Double_t y2 = 0.081;
	Double_t y3 = 0.132;

	TCanvas * c = new TCanvas("EncalCanv", "Energy Calibration Tool", canvw, canvh);
	c->Divide(1,2);

	TButton * btnOk = new TButton("ok", "buttonFlag = kButtonOk;", x0, y0, x0 + wdth, y1);
	btnOk->SetFillColor(32);
	btnOk->SetToolTipText("pass data to calibration file and continue with next histo");
	btnOk->Draw();
	TButton * btnDiscard = new TButton("discard", "buttonFlag = kButtonDiscard;", x0 + wdth, y0, x0 + 2 * wdth, y1);
	btnDiscard->SetFillColor(32);
	btnDiscard->SetToolTipText("discard fit and continue with next histo");
	btnDiscard->Draw();
	TButton * btnStop = new TButton("stop", "buttonFlag = kButtonStop;", x0 + 2 * wdth, y0, x0 + 3 * wdth, y1);
	btnStop->SetFillColor(32);
	btnStop->SetToolTipText("leave calibration and return to MacroBrowser");
	btnStop->Draw();
	TButton * btnQuit = new TButton("quit", "gSystem->Exit(0);", x0 + 3 * wdth, y0, x0 + 4 * wdth, y1);
	btnQuit->SetFillColor(32);
	btnQuit->SetToolTipText("exit from ROOT");
	btnQuit->Draw();
	statusLine = new TButton(" ", " ", x0, y2, x0 + 4 * wdth, y3);
	statusLine->Draw();

	c->cd(2);
	gPad->SetBottomMargin(.3);

	c->cd();
	c->Update();

	return c;
}

Bool_t CheckCalSource(Int_t CalSource) {
	switch (CalSource) {
		case kCalSourceTripleAlpha:
			calSource = "TripleAlpha";
			isTripleAlpha = kTRUE;
			peaksNeeded = 3;
			return(kTRUE);
		case kCalSourceCo60:
			calSource = "Co60";
			isCo60 = kTRUE;
			peaksNeeded = 2;
			return(kTRUE);
		case kCalSourceEu152:
			calSource = "Eu152";
			isEu152 = kTRUE;
			peaksNeeded = 5;
			break;
	}
		
	peaksNeeded = 0;
	msg->Err()	<< "Calibration source not yet implemented - " << calSource
				<< ", performing peak fitting only" << endl;
	msg->Flush("Encal.C");
	return(kFALSE);
}

void WaitForButtonPressed() {
	buttonFlag = 0;
	if (!stepIt) return;
	while (buttonFlag == 0) {
		gSystem->Sleep(50);
		gSystem->ProcessEvents();
	}
}

void ClearCanvas(Int_t PadNo) {
	switch (PadNo) {
		case 0:
		case 1:
			canv->cd(1);
			gPad->Clear();
			if (PadNo == 1) break;
		case 2:
			canv->cd(2);
			gPad->Clear();
			break;
	}
	canv->cd(PadNo);
	canv->Update();
}

void SetStatusLine(const Char_t * Text = "", Int_t ColorIndex = 10) {
		statusLine->SetTitle(Text);
		statusLine->SetFillColor(ColorIndex);
		statusLine->Paint();
		statusLine->Modified();
		statusLine->Update();
}

void Encal(TObjArray * Histos = NULL,
           Int_t CalSource = kCalSourceCo60,
           const Char_t * CalFile = "Encal.cal",
           const Char_t * ResFile = "Encal.res",
           const Char_t * PrecalFile = "Co60.cal",
           Int_t LowerLim = 0,
           Int_t UpperLim = 4096,
           Int_t PeakFrac = 1,
           Double_t Sigma = 3,
           Int_t FitMode = kFitModeGaus,
           Int_t FitGrouping = kFitGroupingSinglePeak,
           Double_t FitRange = 3,
           Int_t DisplayFlag = kDisplayFlagStep,
           Bool_t VerboseMode = kFALSE)
//>>_________________________________________________(do not change this line)
//
{
	msg = new TMrbLogger("Encal.log");

	show2dim = ((DisplayFlag & kDisplayFlag2dim) != 0);
	stepIt = ((DisplayFlag & kDisplayFlagStep) != 0);

	Int_t nofHistos = Histos->GetEntriesFast();
	if (nofHistos == 0) {
		msg->Err() << "No histogram file given" << endl;
		msg->Flush("Encal.C");
		return;
	}

	TIterator * hIter = Histos->MakeIterator();
	TObjString * h;
	h = (TObjString *) hIter->Next();

	TString hFileName;
	hFileName.Resize(0);
	if (h) {
		hFileName = h->GetString();
		hFileName = hFileName.Strip(TString::kBoth);
	}
	if (hFileName.IsNull()) {
		msg->Err() << "No histogram file given" << endl;
		msg->Flush("Encal.C");
		return;
	}

	TFile * hFile = new TFile(hFileName.Data());
	if (!hFile->IsOpen()) {
		msg->Err() << "Can't open histogram file - " << hFileName << endl;
		msg->Flush("Encal.C");
		return;
	}

	if (nofHistos == 1) {
		msg->Err() << "No histogram(s) selected" << endl;
		msg->Flush("Encal.C");
		return;
	}

	doCal = CheckCalSource(CalSource);

	TEnv res(ResFile);
	if (VerboseMode) {
		msg->Out() << "Writing results to file " << ResFile << endl;
		msg->Flush("Encal.C");
	}

	res.SetValue("Calib.ROOTFile", hFileName.Data());
	res.SetValue("Calib.NofHistograms", nofHistos - 1);

	TEnv cal(CalFile);
	if (VerboseMode) {
		msg->Out() << "Writing calibration data to file " << CalFile << endl;
		msg->Flush("Encal.C");
	}

	cal.SetValue("Calib.ROOTFile", hFileName.Data());
	
	cal.SetValue("Calib.Source", calSource.Data());
	cal.SetValue("Calib.NofHistograms", nofHistos - 1);

	canv = DrawCanvas();

	while (h = (TObjString *) hIter->Next()) {

		TString hist = h->GetString();

		canv->cd(1);
		TH1F * h = (TH1F *) hFile->Get(hist.Data());
		if (h == NULL) {
			ClearCanvas(0);
			msg->Err() << "No such histogram - " << hFileName << ":" << hist << endl;
			msg->Flush("Encal.C");
			WaitForButtonPressed();
			if (buttonFlag == kButtonStop) return; else continue;
		}

		if (UpperLim == 0 || UpperLim < LowerLim) UpperLim = h->GetNbinsX();
		if (LowerLim != 0 || UpperLim != h->GetNbinsX()) h->GetXaxis()->SetRange(LowerLim, UpperLim);

		TSpectrum * s = new TSpectrum(100);

		Double_t pmax = (Double_t) PeakFrac / 100.;
		Int_t nPeaks = s->Search(h, 5, "", pmax);

		TArrayF px, py, fx, fy, fxe, fye, chi2;
		TF1 * pol1 = NULL;
		TString fitOk;

		if (nPeaks > 0) {
	
			px.Set(nPeaks, s->GetPositionX());
			py.Set(nPeaks, s->GetPositionY());

			TArrayF ps;
			TArrayI ind(nPeaks);
			ps.Set(nPeaks, px.GetArray());
			TMath::Sort(nPeaks, ps.GetArray(), ind.GetArray(), kFALSE);
			for (Int_t i = 0; i < nPeaks; i++) {
				Int_t k = ind[i];
				px[i] = ps[k];
			}
			ps.Set(nPeaks, py.GetArray());
			for (Int_t i = 0; i < nPeaks; i++) {
				Int_t k = ind[i];
				py[i] = ps[k];
			}

			cout << endl << "Peaks:" << endl;
			for (Int_t i = 0; i < nPeaks; i++) cout << i << " X=" << px[i] << " Y=" << py[i] << endl;

    		TList * lf = h->GetListOfFunctions();                        
    		TPolyMarker * pm = (TPolyMarker *) lf->FindObject("TPolyMarker");
			pm->SetMarkerColor(kBlue);

			fx.Set(nPeaks);
			fy.Set(nPeaks);
			fxe.Set(nPeaks);
			fye.Set(nPeaks);
			chi2.Set(nPeaks);

			TF1 * fit;
			if (FitMode == 1) {
				for (Int_t i = 0; i < nPeaks; i++) {
					Double_t p[7];
					// tail
					p[0] = 1;
					p[1] = Sigma;
					// linear background
					p[2] = h->GetBinContent(h->FindBin(px[i] - FitRange * Sigma));
					p[3] = 0.;
					// gauss
					p[4] = Sigma;
					p[5] = py[i];
					p[6] = px[i];

					fit = new TF1(Form("g%d", i), gaus_tail, px[i] - FitRange * Sigma, px[i] + FitRange * Sigma, 7);
					fit->SetParameters(p);
					cout << "Peak #" << i << ":" << endl;
					for (Int_t k = 0; k < 7; k++) cout << "    p" << k << "=" << p[k] << endl;
					fit->SetLineColor(2);
					h->Fit(fit, (i == 0) ? "R" : "R+");
					fit->GetParameters(p);
					fx[i] = p[6];
					fy[i] = p[5];
					fxe[i] = fit->GetParError(6);
					fye[i] = fit->GetParError(5);
					Int_t ndf = TMath::Max(1, fit->GetNDF());
					chi2[i] = fit->GetChisquare() / ndf;
				}
			} else {
				for (Int_t i = 0; i < nPeaks; i++) {
					Double_t p[5];
					// linear background
					p[0] = h->GetBinContent(h->FindBin(px[i] - FitRange * Sigma));
					p[1] = 0.;
					// gauss
					p[2] = Sigma;
					p[3] = py[i];
					p[4] = px[i];

					fit = new TF1(Form("g%d", i), gaus_lbg, px[i] - FitRange * Sigma, px[i] + FitRange * Sigma, 5);
					fit->SetParameters(p);
					cout << "Peak #" << i << ":" << endl;
					for (Int_t k = 0; k < 5; k++) cout << "    p" << k << "=" << p[k] << endl;
					fit->SetLineColor(2);
					h->Fit(fit, (i == 0) ? "R" : "R+");
					fit->GetParameters(p);
					fx[i] = p[4];
					fy[i] = p[3];
					fxe[i] = fit->GetParError(4);
					fye[i] = fit->GetParError(3);
					Int_t ndf = TMath::Max(1, fit->GetNDF());
					chi2[i] = fit->GetChisquare() / ndf;
				}
			}

			cout << endl << "=========================================================================" << endl;
			cout << "File : " << hFileName << endl;
			cout << "Histo: " << hist << endl;
			cout << "Peaks:  #    X         X(fit)       Chi2" << endl;
			cout << "------------------------------------------------------------------------" << endl;
			for (Int_t i = 0; i < nPeaks; i++) {
				cout << Form("%9d   %7.2f   %7.2f       %7.5f", i, px[i], fx[i], chi2[i]) << endl;
			}
			cout << "=========================================================================" << endl;

			ClearCanvas(2);

			if (doCal) {
				TArrayF calX;
				TArrayF calE;
				TArrayF calXerr;
				TArrayF calEerr;
				if (nPeaks >= peaksNeeded) { 
					if (isTripleAlpha) {
						calX.Set(3);
						calE.Set(3);
						calXerr.Set(3);
						calEerr.Set(3);

						Int_t k = nPeaks - 1;
						for (Int_t i = 2; i >= 0; i--, k--) {
							calX[i] = fx[k];
							calXerr[i] = fxe[k];
						}

 						calE[0] = 5157;
						calE[1] = 5486;
						calE[2] = 5865;
						calEerr[0] = 1;
						calEerr[1] = 1;
						calEerr[2] = 1;

					} else if (isCo60) {
						calX.Set(2);
						calE.Set(2);
						calXerr.Set(2);
						calEerr.Set(2);

						Int_t k = nPeaks - 1;
						for (Int_t i = 1; i >= 0; i--, k--) {
							calX[i] = fx[k];
							calXerr[i] = fxe[k];
						}

 						calE[0] = 1173.24;
						calE[1] = 1332.5;
						calEerr[0] = 1;
						calEerr[1] = 1;
					}
					TGraphErrors * calib = new TGraphErrors(3, calX.GetArray(), calE.GetArray(), calXerr.GetArray(), calEerr.GetArray());
					calib->SetName(Form("c_%s", hist.Data()));
					calib->SetTitle(Form("%s Calibration for histo %s", calSource.Data(), hist.Data()));
					calib->Draw("A*");
					calib->Fit("pol1");
					pol1 = calib->GetFunction("pol1");
					pol1->SetLineColor(2);
				} else {
					msg->Err()	<< "Wrong number of peaks - " << nPeaks
								<< " (" << calSource << " calibration needs at least " << peaksNeeded << " peaks)" << endl;
					msg->Flush("Encal.C");
					fitOk = "FALSE";
				}
			}

			canv->Update();
			gSystem->ProcessEvents();
		} else {
			canv->cd(1);
			h->Draw();
			ClearCanvas(2);
			TString sts = Form("No peaks found in histogram - %s", hist.Data());
			SetStatusLine(sts.Data(), 2);
			msg->Err()	<< sts << endl;
			msg->Flush("Encal.C");
			gSystem->ProcessEvents();
			fitOk = "FALSE";
		}

		res.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
		res.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);

		res.SetValue(Form("Calib.%s.NofPeaks", hist.Data()), nPeaks);
		for (Int_t i = 0; i < nPeaks; i++) {
			res.SetValue(Form("Calib.%s.Peak.%d.X", hist.Data(), i), px[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Xfit", hist.Data(), i), fx[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Xerr", hist.Data(), i), fxe[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Y", hist.Data(), i), py[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Yfit", hist.Data(), i), fy[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Yerr", hist.Data(), i), fye[i]);
			res.SetValue(Form("Calib.%s.Peak.%d.Chi2", hist.Data(), i), chi2[i]);
		}

		if (nPeaks) {
			TString sts;
			if (doCal) {
				sts = Form("File %s, histo %s: E(x) = %5.2f + %5.2f * x", hFileName.Data(), hist.Data(), pol1->GetParameter(0),pol1->GetParameter(1));
			} else {
				sts = Form("File %s, histo %s: %d peak(s)", hFileName.Data(), hist.Data(), nPeaks);
			}
			SetStatusLine(sts.Data(), 3);
		}

		WaitForButtonPressed();
		if (buttonFlag != 0) {
			if (buttonFlag == kButtonStop) return;
			if (buttonFlag != kButtonOk) continue;
			if (doCal && nPeaks) {
				fitOk = "TRUE";
				cal.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
				cal.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
				cal.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				cal.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				res.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				res.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
			} else {
				fitOk = "FALSE";
			}
		} else {
			if (doCal && nPeaks) {
				cal.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
				cal.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
				cal.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				cal.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				res.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				res.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				fitOk = "AUTO";
			} else {
				fitOk = "FALSE";
			}
		}
		res.SetValue(Form("Calib.%s.FitOk", hist.Data()), fitOk);
	}

	msg->Out() << "End of calibration - " << (nofHistos - 1) << " histogram(s)" << endl;
	msg->Flush("Encal.C", "", setblue);

	res.SaveLevel(kEnvLocal);
	cal.SaveLevel(kEnvLocal);
	return;
}
