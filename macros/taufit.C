//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             taufit.C
// Purpose:          Taufit
// Syntax:           .x taufit.C(const Char_t * File,
//                                Int_t NofTaus,
//                                Double_t c,
//                                Double_t A0,
//                                Double_t Tau0,
//                                Double_t A1,
//                                Double_t Tau1)
// Arguments:        Char_t * File             -- File (.dat)
//                   Int_t NofTaus             -- Number of exp() functions
//                   Double_t c                -- Constant
//                   Double_t A0               -- a0
//                   Double_t Tau0             -- tau0
//                   Double_t A1               -- a1
//                   Double_t Tau1             -- tau1
// Description:      Taufit
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: taufit.C,v 1.3 2007-07-27 11:36:40 Rudolf.Lutter Exp $
// Date:             Fri Jul 27 13:25:50 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                taufit.C
//                   Title:               Taufit
//                   Width:               
//                   Aclic:               +g
//                   Modify:              
//                   RcFile:              
//                   NofArgs:             7
//                   Arg1.Name:           File
//                   Arg1.Title:          File (.dat)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         Data files:*.dat
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           NofTaus
//                   Arg2.Title:          Number of exp() functions
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        1
//                   Arg2.Values:         1:2
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           c
//                   Arg3.Title:          Constant
//                   Arg3.Type:           Double_t
//                   Arg3.EntryType:      Entry-C
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           A0
//                   Arg4.Title:          a0
//                   Arg4.Type:           Double_t
//                   Arg4.EntryType:      Entry-C
//                   Arg4.Default:        1.0
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Tau0
//                   Arg5.Title:          tau0
//                   Arg5.Type:           Double_t
//                   Arg5.EntryType:      Entry-C
//                   Arg5.Default:        1.0
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           A1
//                   Arg6.Title:          a1
//                   Arg6.Type:           Double_t
//                   Arg6.EntryType:      Entry-C
//                   Arg6.Default:        1.0
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           Tau1
//                   Arg7.Title:          tau1
//                   Arg7.Type:           Double_t
//                   Arg7.EntryType:      Entry-C
//                   Arg7.Default:        1.0
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "TSystem.h"
#include "TCanvas.h"
#include "TString.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"

Int_t fitNofTaus = 2;
Int_t fitBinWidth = 1;

Double_t multi_exp(Double_t * x, Double_t * par) {
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           multi_exp
// Purpose:        fit multi exponential
// Description:    par[0]   a0
//                 par[1]   tau0
//                 par[2]   a1
//                 par[3]   tau1
//                 par[4]   ...
//////////////////////////////////////////////////////////////////////////////

   Double_t fitval  = 0;
   Double_t c0 = par[0];
   fitval = c0;
   for (Int_t i = 0; i < fitNofTaus; i ++) {
      fitval += par[2 * i + 1] * exp(-x[0] * log(2) / par[2 * i + 2]);
   }
   return fitBinWidth * fitval;
}


void taufit(const Char_t * File,
         Int_t NofTaus = 1,
         Double_t C = 0.0, Bool_t FixC = kFALSE,
         Double_t A0 = 1.0, Bool_t FixA0 = kFALSE,
         Double_t Tau0 = 1.0, Bool_t FixTau0 = kFALSE,
         Double_t A1 = 1.0, Bool_t FixA1 = kFALSE,
         Double_t Tau1 = 1.0, Bool_t FixTau1 = kFALSE)
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           fit
// Purpose:        main fit function
// Description:    (1) reads data to a TGraphErrors object
//                 (2) applies a 1 or 2 fold exp function to it
//                 (3) displays the whole stuff and writes it to postscript
//////////////////////////////////////////////////////////////////////////////
//>>_________________________________________________(do not change this line)
//
{
	fitNofTaus = NofTaus;
	if (fitNofTaus > 2) {
		cout << "fit.C: max 2 exp functions allowed!" << endl;
		return;
	}

	TString fileName = gSystem->BaseName(File); 		// strip off path

	TCanvas * c = new TCanvas("fit", "Taufit"); 		// open a c anvas to draw data
	gStyle->SetOptFit(1);								// fit results will be displayed in upper right corner

	TGraph * g = new TGraphErrors(fileName.Data(), "%lg %lg %lg");		// read data: x, y, dy
	g->SetTitle(fileName.Data());
	g->Draw("a*");										// draw graph: asteriscs + error bars

	// set up fit function
	TF1 * f = new TF1("taufit", multi_exp, g->GetXaxis()->GetXmax(), g->GetXaxis()->GetXmin(), 2 * fitNofTaus + 1);

	f->SetParName(0, "c");					// set par name (in latex mode!)
	if (FixC) f->FixParameter(0, C);
	f->SetParName(1, "a_{0}");				// set par name (in latex mode!)
	f->SetParameter(1, A0); 				// start value
	if (FixA0) f->FixParameter(1, A0);
	f->SetParName(2, "#tau_{0}");			// ...
	f->SetParameter(2, Tau0);
	if (FixTau0) f->FixParameter(2, Tau0);
	if (fitNofTaus == 2) {
		f->SetParName(3, "a_{1}");
		f->SetParameter(3, A1);
		if (FixA1) f->FixParameter(3, A1);
		f->SetParName(4, "#tau_{1}");
		f->SetParameter(4, Tau1);
		if (FixTau1) f->FixParameter(4, Tau1);
	}
	f->SetLineColor(2);
	g->Fit(f);								// do the fit

	TLatex * tl = new TLatex(); 			// make an insert containing fit formula
	tl->SetTextSize(0.04);
	Double_t x0 = g->GetXaxis()->GetXmin()  + (g->GetXaxis()->GetXmax() - g->GetXaxis()->GetXmin()) * 0.6;
	Double_t y0 = g->GetYaxis()->GetXmin()  + (g->GetYaxis()->GetXmax() - g->GetYaxis()->GetXmin()) * 0.6;
	if (fitNofTaus > 1) {
		tl->DrawLatex(x0, y0, Form("f(x) = c + #sum_{i=0}^{%d}a_{i} #upoint e^{ -x #upoint 6.9315 / #tau_{i}}", fitNofTaus - 1));
	} else {
		tl->DrawLatex(x0, y0, "f(x) = c + a_{0} #upoint e^{ -x #upoint 6.9315 / #tau_{0}}");
	}

	fileName.ReplaceAll(".dat", ".ps"); 	// change file extension from .dat to .ps
	c->Print(fileName.Data());				// create a postscript plot
}
