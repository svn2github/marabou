//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             ppacMacro.C
// Purpose:          Show PPAC data, to be used in a MINIBALL experiment
// Syntax:           root> gROOT->Macro("LoadUtilityLibs.C");
//                   root> gROOT->ProcessLine(".L ppacMacro.C+");
//                   root> ppac()
// Author:           R. Lutter
// Revision:         
// Date:             Aug-2004
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include "TROOT.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TCanvas.h"
#include "TTimer.h"
#include "TString.h"
#include "TH1F.h"
#include "TSystem.h"
#include "TLine.h"
#include "TMrbString.h"
#include "SetColor.h"

// ppac strips are NOT attached to subsequent scaler channels,
// so strip numbers have to be transformed
static Int_t chnMap[]	=	{	8,	7,	6,	5,	4,	3,	2,	16,
								15, 14, 13, 12, 11, 10, 24, 23,
								22, 21, 20, 19, 18, 32, 31, 30,
								29
							};

// use individual current offsets for all strips
static Int_t offsetX[]	=	{	0,		0,		500,	0,		0,		0,		2000,	0,
								0,		300,	0,		0,		0,		0,		0,	 	0,
								0,		0,		0,		0,		1000,	0,		0,		1500,
								0
							};
static Int_t offsetY[]	=	{	0,		0,		0,		0,		0,		0,		1000,	0,
								0,		500,	0,		0,		0,		0,		0,		0,
								1000,	0,		0,		0,		2000,	0,		0,		600,
								0
							};

// timer to display ppac data every N seconds
class PpacTimer : public TTimer {

	public:
    	PpacTimer(Long_t ms, Bool_t synch);
    	Bool_t Notify();
    	inline void SetRange(Double_t Range) { fRange = Range; };
		inline void ResetHists() {
			if (fHistX) fHistX->Reset();
			if (fHistY) fHistY->Reset();
		};
		inline void RemoveHists() {
			if (fHistX) delete fHistX;
			fHistX = NULL;
			if (fHistY) delete fHistY;
			fHistY = NULL;
			if (fCanvas) delete fCanvas;
			fCanvas = NULL;
			if (fLeftMarker) delete fLeftMarker;
			fLeftMarker = NULL;
			if (fRightMarker) delete fRightMarker;
			fRightMarker = NULL;
		};

	public:
		TCanvas * fCanvas;
		TH1F * fHistX; 
		TH1F * fHistY; 
		TLine * fLeftMarker;
		TLine * fRightMarker;
		Double_t fRange;

	ClassDef(PpacTimer, 0)
};

PpacTimer::PpacTimer(Long_t MilliSecs, Bool_t SynchFlag): TTimer(MilliSecs, SynchFlag){
	gSystem->AddTimer(this);
	fCanvas = NULL;
	fHistX = NULL;
	fHistY = NULL;
	fRange = 20000;
}


static PpacTimer * ppacTimer = NULL;

// ppac main: has to be called once on start
void ppac(Int_t Refresh = 1)
{
	if (ppacTimer == NULL) {
		ppacTimer = new PpacTimer(Refresh * 1000, kTRUE);	// instantiate a new timer
	} else {
		ppacTimer->ResetHists();				// use existing one, reset histograms
	}
	ppacTimer->Start(-1, kTRUE);				// start timer, use previously defined interval
}

// come here on timer interrupts
Bool_t PpacTimer::Notify() {
	if (fCanvas == NULL) {						// create a canvas if not already done
		fCanvas = new TCanvas("ppac", "PPAC beam monitor", 200, 10, 700, 500);
		fCanvas->SetGridx(1);					// show grid lines in X and Y
		fCanvas->SetGridy(1);
	}
	if (fHistX == NULL) {						// histogram to show X strips
		fHistX = new TH1F("PPAC", "PPAC", 25, -20, 20);
		fHistX->SetLineColor(2);				// data will be shown red
		fHistX->SetLineWidth(2);
		fHistX->GetXaxis()->SetTitle("position of beam in the PPAC [mm]");
		fHistX->GetXaxis()->CenterTitle();
		fHistX->GetYaxis()->SetTitle("PPAC strip current [pA]");
		fHistX->GetYaxis()->CenterTitle();
		fHistX->GetYaxis()->SetTitleOffset(1.3);
		fHistX->SetAxisRange(-fRange, fRange, "Y");
		fLeftMarker = new TLine(-5, -fRange, -5, fRange); 	// show marker lines at X=-/+5
		fLeftMarker->SetLineColor(kRed);
		fRightMarker = new TLine(5, -fRange, 5, fRange);
		fRightMarker->SetLineColor(kRed);
	}
	if (fHistY == NULL) {						// histogram to show Y strips
		fHistY = new TH1F("PPACY", "PPAC", 25, -20, 20);
		fHistY->SetLineColor(4);				// data will be shown blue
		fHistY->SetLineWidth(2);
		fHistY->SetAxisRange(-fRange, fRange, "Y");
	}
	ifstream f;
	Int_t dataX[32], dataY[32];
	f.open("VMEScalers.dat", ios::in);			// read data from file "VMEScalers.dat"
	if (!f.good()) return(kFALSE);
	for (Int_t i = 0; i < 32; i++) {
		f >> dataX[i];	// first 32 channels belong to X strips
	}
	for (Int_t i = 0; i < 32; i++) {
		f >> dataY[i];	// next 32 channels belong to Y strips
	}
	f.close();
	for (Int_t i = 0; i < 25; i++) {				// now do the channel mapping for X
		Stat_t d = (Stat_t) (dataX[chnMap[i] - 1] / 20. - offsetX[i]);	// apply offsets
		fHistX->SetBinContent(i + 1, d);			// set histogram channel
	}
	for (Int_t i = 0; i < 25; i++) {				// same for Y
		Stat_t d = (Stat_t) (-1)*(dataY[chnMap[i] - 1] / 20. - offsetY[i]);
		fHistY->SetBinContent(i + 1, d);
	}
	fHistX->Draw(); 					// draw X values
	fLeftMarker->Draw();				// show marker lines
	fRightMarker->Draw();
	fHistY->Draw("same");				// draw Y values in same canvas
	fCanvas->Update();					// update canvas
	this->Start(-1, kTRUE); 			// restart timer
	return(kTRUE);
}

// stop timer
void stop() {
	if (ppacTimer) {
		ppacTimer->Stop();
		ppacTimer->RemoveHists();		// remove histograms
		cout << "[PPAC timer STOPPED]" << endl;
	}
}

// start timer with new interval
void start(Int_t Refresh = 0) {
	if (ppacTimer) {
		ppacTimer->ResetHists();		// clear histograms
		if (Refresh <= 0) {
			ppacTimer->Start(-1, kTRUE);	// restart timer
		} else {
			ppacTimer->Start(Refresh * 1000, kTRUE);	// restart timer
		}
		ULong_t r = (ULong_t) ppacTimer->GetTime() / 1000;
		cout << "[PPAC timer RESTARTED: refreshing every " << r << "s]" << endl;
	}
}

// set range in Y
void range(Double_t Range = 20000) {
	if (ppacTimer) {
		ppacTimer->Stop();
		ppacTimer->RemoveHists();
		ppacTimer->SetRange(Range);		// remove histograms
		ppacTimer->Notify();
		cout << "[PPAC Y-Range set to " << Range << "]" << endl;
	}
}

// exit from root
void bye() {
	cout << "[ppac.C: Exit]" << endl;
	gSystem->Exit(0);
};

// help message
void help() {
	cout	<< endl << "ppac.C: PPAC beam monitor" << endl;
	cout	<< endl << "Usage:  ppac(n)   start execution, n being the timer interval in seconds (default: 1)" << endl;
	cout	<< "        stop()    stop timer" << endl;
	cout	<< "        start(n)  restart timer, set interval to n (default: previous value)" << endl;
	cout	<< "        range(r)  set Y range to r (default: 20000)" << endl;
	cout	<< "        bye()     exit" << endl << endl;
};

