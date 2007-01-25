//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Encal.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .x Encal.C(Int_t CalSource,
//                               const Char_t * HistoFile,
//                               const Char_t * CalFile,
//                               const Char_t * ResFile,
//                               const Char_t * PrecalFile,
//                               Int_t BackEst,
//                               Int_t LowerLim,
//                               Int_t UpperLim,
//                               Int_t PeakFrac,
//                               Double_t Sigma,
//                               Int_t FitMode,
//                               Double_t FitRange,
//                               Int_t DisplayFlag,
//                               Int_t VerboseMode)
// Arguments:        Int_t CalSource           -- Calibration source
//                   Char_t * HistoFile        -- Histogram file (.root)
//                   Char_t * CalFile          -- Calibration output file (.cal)
//                   Char_t * ResFile          -- Results file (.res)
//                   Char_t * PrecalFile       -- Precalibration file (.cal, needed if Eu152)
//                   Int_t BackEst             -- Background estimate
//                   Int_t LowerLim            -- Lower limit [chn]
//                   Int_t UpperLim            -- Upper limit [chn]
//                   Int_t PeakFrac            -- Threshold for peak finder [% of max peak]
//                   Double_t Sigma            -- Sigma
//                   Int_t FitMode             -- Fit mode
//                   Double_t FitRange         -- Range for single peak fit [sigma]
//                   Int_t DisplayFlag         -- Display results
//                   Int_t VerboseMode         -- Verbose mode
// Description:      Energy calibration for 1-dim histograms
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: Encal.C,v 1.1 2007-01-25 09:25:44 Rudolf.Lutter Exp $
// Date:             Wed Nov 29 10:34:10 2006
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               
//                   Aclic:               ++g
//                   NofArgs:             14
//                   Arg1.Name:           CalSource
//                   Arg1.Title:          Calibration source
//                   Arg1.Type:           Int_t
//                   Arg1.EntryType:      Radio
//                   Arg1.Default:        1
//                   Arg1.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source (Co60 precalibration needed)=2:TripleAlpha|calibrate using 3x alpha source=4
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           HistoFile
//                   Arg2.Title:          Histogram file (.root)
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      FObjListBox
//                   Arg2.Default:        none.root
//                   Arg2.Values:         ROOT files:*.root
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
//                   Arg6.Name:           BackEst
//                   Arg6.Title:          Background estimate
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      Radio
//                   Arg6.Default:        1
//                   Arg6.Values:         off|don't calculate bg estimate=1:order2|calulate 2nd order polynomial=2:compton|assume compton background=4
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           LowerLim
//                   Arg7.Title:          Lower limit [chn]
//                   Arg7.Type:           Int_t
//                   Arg7.EntryType:      UpDown-X
//                   Arg7.Default:        0
//                   Arg7.AddLofValues:   No
//                   Arg7.LowerLimit:     0
//                   Arg7.UpperLimit:     4096
//                   Arg7.Increment:      100
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           UpperLim
//                   Arg8.Title:          Upper limit [chn]
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      UpDown-X
//                   Arg8.Default:        4096
//                   Arg8.AddLofValues:   No
//                   Arg8.LowerLimit:     0
//                   Arg8.UpperLimit:     4096
//                   Arg8.Increment:      100
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           PeakFrac
//                   Arg9.Title:          Threshold for peak finder [% of max peak]
//                   Arg9.Type:           Int_t
//                   Arg9.EntryType:      UpDown-X
//                   Arg9.Default:        1
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     1
//                   Arg9.UpperLimit:     50
//                   Arg9.Increment:      1
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          Sigma
//                   Arg10.Title:         Sigma
//                   Arg10.Type:          Double_t
//                   Arg10.EntryType:     UpDown
//                   Arg10.Default:       3
//                   Arg10.AddLofValues:  No
//                   Arg10.LowerLimit:    0
//                   Arg10.UpperLimit:    10
//                   Arg10.Increment:     .2
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          FitMode
//                   Arg11.Title:         Fit mode
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     Radio
//                   Arg11.Default:       1
//                   Arg11.Values:        gaus|fit gaussian distribution=1:gaus+tail|fit gaussian + exp tail on left side=2
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
//                   Arg13.EntryType:     Radio
//                   Arg13.Default:       2
//                   Arg13.Values:        No|don't display results=1:Step|show each fit=2:2dim|show 2-dim histo after calibration=4:Step+2dim|options 2+3=6
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          VerboseMode
//                   Arg14.Title:         Verbose mode
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     Radio
//                   Arg14.Default:       1
//                   Arg14.Values:        No=1:Yes=2:Debug=3
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
#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TEnv.h"
#include "TSpectrum.h"
#include "TPolyMarker.h"
#include "TMrbString.h"
#include "SetColor.h"

enum	{ kCalSourceCo60 = 1 };
enum	{ kCalSourceEu152 = 2 };
enum	{ kCalSourceTripleAlpha = 4 };
enum	{ kBackEstOff = 1 };
enum	{ kBackEstOrder2 = 2 };
enum	{ kBackEstCompton = 4 };
enum	{ kFitModeGaus = 1 };
enum	{ kFitModeGausTail = 2 };
enum	{ kDisplayFlagNo = 1 };
enum	{ kDisplayFlagStep = 2 };
enum	{ kDisplayFlag2dim = 4 };
enum	{ kDisplayFlagStep2dim = 6 };
enum	{ kVerboseModeNo = 1 };
enum	{ kVerboseModeYes = 2 };
enum	{ kVerboseModeDebug = 3 };

enum	{	kAskOk,
			kAskDiscard,
			kAskQuit
		};

Double_t nofPeaks = 1;
Int_t binWidth = 1;
Double_t tailSide = 1;

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
   Double_t sigma     = par[4];

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

void Encal(Int_t CalSource = 1,
           const Char_t * HistoFile = "none.root",
           const Char_t * CalFile = "Encal.cal",
           const Char_t * ResFile = "Encal.res",
           const Char_t * PrecalFile = "Co60.cal",
           Int_t BackEst = kBackEstOff,
           Int_t LowerLim = 0,
           Int_t UpperLim = 4096,
           Int_t PeakFrac = 1,
           Double_t Sigma = 3,
           Int_t FitMode = kFitModeGaus,
           Double_t FitRange = 3,
           Int_t DisplayFlag = kDisplayFlagStep,
           Int_t VerboseMode = kVerboseModeNo)
{
	TObjArray lofHistos;
	TMrbString hfStr = HistoFile;
	hfStr = hfStr.Strip(TString::kBoth);

	lofHistos.Delete();
	Int_t nofHistos = hfStr.Split(lofHistos, ":", kTRUE);
	if (nofHistos == 0) {
		cerr << setred << "Encal.C: No histogram file given" << setblack << endl;
		return;
	}

	TString hFileName = ((TObjString *) lofHistos[0])->GetString();
	hFileName = hFileName.Strip(TString::kBoth);
	if (hFileName.IsNull()) {
		cerr << setred << "Encal.C: No histogram file given" << setblack << endl;
		return;
	}

	TFile * hFile = new TFile(hFileName.Data());
	if (!hFile->IsOpen()) {
		cerr << setred << "Encal.C: Can't open histogram file - " << hFileName << setblack << endl;
		return;
	}

	if (nofHistos == 1) {
		cerr << setred << "Encal.C: No histogram(s) selected" << setblack << endl;
		return;
	}

	TEnv res(ResFile);

	res.SetValue("Calib.ROOTFile", hFileName.Data());
	res.SetValue("Calib.NofHistograms", nofHistos - 1);

	TEnv cal(CalFile);

	cal.SetValue("Calib.ROOTFile", hFileName.Data());
	
	TString s;
	switch (CalSource) {
		case kCalSourceCo60:	s = "Co60"; break;
		case kCalSourceEu152:	s = "Eu152"; break;
		case kCalSourceTripleAlpha:	s = "TripleAlpha"; break;
	}

	cal.SetValue("Calib.Source", s.Data());
	cal.SetValue("Calib.NofHistograms", nofHistos - 1);

	TCanvas * canv = new TCanvas("EncalCanv");
	canv->Divide(1,2);

	for (Int_t nHist = 1; nHist < nofHistos; nHist++) {
		TString hist = ((TObjString *) lofHistos[nHist])->GetString();

		canv->cd(1);
		TH1F * h = (TH1F *) hFile->Get(hist.Data());

		if (UpperLim == 0 || UpperLim < LowerLim) UpperLim = h->GetNbinsX();
		if (LowerLim != 0 || UpperLim != h->GetNbinsX()) h->GetXaxis()->SetRange(LowerLim, UpperLim);

		TSpectrum * s = new TSpectrum(100);

		Double_t pmax = (Double_t) PeakFrac / 100.;
		Int_t nPeaks = s->Search(h, 5, "", pmax);

		TArrayF px, py, fx, fy, fxe, fye, chi2;
		TF1 * pol1 = NULL;

		if (nPeaks > 0) {
	
			if (BackEst > 0) {
				TH1F * bg = (TH1F *) s->Background(h, 20, "Compton");
				bg->Draw("same");
			}

			px.Set(nPeaks, s->GetPositionX());
			py.Set(nPeaks, s->GetPositionY());
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
				delete fit;
			}

			canv->cd(2);
			gPad->Clear();

			if ((CalSource == kCalSourceTripleAlpha) && (nPeaks == 3)) {
				TArrayF calX(3);
				TArrayF calE(3);
				TArrayF calXerr(3);
				TArrayF calEerr(3);

				for (Int_t i = 0; i < 3; i++) calX[i] = fx[i];
				TArrayI ind(3);
				TMath::Sort(3, calX.GetArray(), ind.GetArray(), kFALSE);
				for (Int_t i = 0; i < 3; i++) {
					Int_t k = ind[i];
					calX[i] = fx[k];
					calXerr[i] = fxe[k];
				}

 				calE[0] = 5157;
				calE[1] = 5486;
				calE[2] = 5865;
				calEerr[0] = 1;
				calEerr[1] = 1;
				calEerr[2] = 1;

				TGraphErrors * calib = new TGraphErrors(3, calX.GetArray(), calE.GetArray(), calXerr.GetArray(), calEerr.GetArray());
				calib->SetName(Form("c_%s", hist.Data()));
				calib->SetTitle(Form("Calibration for histo %s", hist.Data()));
				calib->Draw("A*");
				calib->Fit("pol1");
				pol1 = calib->GetFunction("pol1");
				pol1->SetLineColor(2);
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
			if (nPeaks != 3) {
				cerr << setred << "Encal.C: Need 3 peaks exactly - " << hFileName << ":" << hist << setblack << endl;
			}

			canv->Update();
			gSystem->ProcessEvents();
		} else {
			cerr << setred << "Encal.C: No peaks found in histogram " << hFileName << ":" << hist << setblack << endl;
			continue;
		}

		res.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
		res.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);

		TString fitOk;
		if (nPeaks == 3) {
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
			if (DisplayFlag & kDisplayFlagStep) {
				cout << "y=ok, n=discard, q=quit ..." << ends;
				Char_t c = getchar();
				cout << endl;
				if (c == 'q') break;
				if (c == 'y') {
					fitOk = "TRUE";
					if (pol1) {
						cal.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
						cal.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
						cal.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
						cal.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
						res.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
						res.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
					}
				} else {
					fitOk = "FALSE";
				}
			} else {
				fitOk = "AUTO";
				if (pol1) {
					cal.SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
					cal.SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
					cal.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
					cal.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
					res.SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
					res.SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				}
			}
			res.SetValue(Form("Calib.%s.FitOk", hist.Data()), fitOk);
		} else {
			res.SetValue(Form("Calib.%s.NofPeaks", hist.Data()), nPeaks);
			res.SetValue(Form("Calib.%s.FitOk", hist.Data()), "FALSE");
			if (DisplayFlag & kDisplayFlagStep) {
				cout << "<any>=discard, q=quit ..." << ends;
				Char_t c = getchar();
				cout << endl;
				if (c == 'q') break;
			}
		}
	}
	cout << setblue << "End of calibration" << setblack << endl;

	res.SaveLevel(kEnvLocal);
	cal.SaveLevel(kEnvLocal);
	return;
}
