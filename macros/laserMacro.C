//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             laserMacro.C
// Purpose:          Show laser on/off data, to be used in a MINIBALL experiment
// Syntax:           root> gROOT->Macro("LoadUtilityLibs.C");
//                   root> gROOT->ProcessLine(".L laserMacro.C+");
//                   root> laser()
// Author:           R. Lutter
// Revision:         
// Date:             Aug-2004
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
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

enum	{	kDefaultRefresh		=	1		};
enum	{	kNofTicks			=	300 	};
enum	{	kDefaultBinning		=	4		};

// timer to display laser data every N seconds
class LaserTimer : public TTimer {

	public:
    	LaserTimer(Long_t ms, Bool_t synch);
    	Bool_t Notify();
		inline void ResetHist() {
			if (fHist) fHist->Reset();
		};
		inline void RemoveHist() {
			if (fHist) delete fHist;
			fHist = NULL;
		};
		inline void SetBinning(Int_t Binning) { fBinning = Binning; };

	public:
		TCanvas * fCanvas;
		TH1F * fHist; 
		Int_t fCounter;
		Int_t fBinning;
		Int_t fMaxLaser;
		Int_t fMaxLaserOn;
		Int_t fMaxLaserOff;
		Int_t fTotal;
		Double_t fAverage;

	ClassDef(LaserTimer, 0)
};

LaserTimer::LaserTimer(Long_t MilliSecs, Bool_t SynchFlag): TTimer(MilliSecs, SynchFlag){
	gSystem->AddTimer(this);
	fCanvas = NULL;
	fHist = NULL;
	fBinning = kDefaultBinning;
	fCounter = 0;
	fMaxLaser = 0;
	fMaxLaserOn = 0;
	fMaxLaserOff = 0;
	fTotal = 0;
	fAverage = 0;
}

static LaserTimer * laserTimer = NULL;

// ppac main: has to be called once on start
void laser(Int_t Refresh = kDefaultRefresh, Int_t Binning = kDefaultBinning)
{
	if (laserTimer == NULL) {
		laserTimer = new LaserTimer(Refresh * 1000, kTRUE);	// instantiate a new timer
	} else {
		laserTimer->ResetHist();				// use existing one, reset histograms
	}
	laserTimer->SetBinning(Binning);
	laserTimer->Start(-1, kTRUE);				// start timer, use previously defined interval
}

// come here on timer interrupts
Bool_t LaserTimer::Notify() {
	if (fCanvas == NULL) {						// create a canvas if not already done
		fCanvas = new TCanvas("laser", "Laser on/off monitor", 200, 10, 700, 500);
		fCanvas->SetGridx(1);					// show grid lines in X and Y
		fCanvas->SetGridy(1);
	}
	if (fHist == NULL) {						// histogram to show laser on/off values
		fHist = new TH1F("laser", "laser on/off", kNofTicks, -kNofTicks * fBinning / 60., 0);	// for about 2 hours
		fHist->SetLineColor(2);					// data will be shown red
		fHist->SetLineWidth(2);
		fHist->GetXaxis()->SetTitle("Time [min]");
		fHist->GetXaxis()->CenterTitle();
		fHist->GetYaxis()->SetTitle("Laser on/off scaler contents");
		fHist->GetYaxis()->CenterTitle();
	}
	ifstream f;
	Int_t data;
	f.open("VMEScalers.dat", ios::in);			// read data from file "VMEScalers.dat"
	if (!f.good()) return(kFALSE);
	for (Int_t i = 0; i < 64; i++) f >> data;	// skip first 64 items (ppac data)
	f >> data;									// item 65: laser on/off
	f.close();

	if (data > fMaxLaser) fMaxLaser = data;
	fCounter++;

	fTotal += data;
	fAverage = (Double_t) fTotal / (Double_t) fCounter;
	if (data > fAverage) {
		if (data > fMaxLaserOn) fMaxLaserOn = data;
	} else {
		if (data > fMaxLaserOff) fMaxLaserOff = data;
	}

	if ((fCounter % fBinning) == 0) {
		for (Int_t i = 1; i < kNofTicks - 1; i++) fHist->SetBinContent(i, fHist->GetBinContent(i + 1));		
		fHist->SetBinContent(kNofTicks - 1, fMaxLaser);
		fMaxLaser = 0;
	}
	fHist->Draw(); 					// draw X values
	fCanvas->Update();					// update canvas
	this->Start(-1, kTRUE); 			// restart timer
	return(kTRUE);
}

// stop timer
void stop() {
	if (laserTimer) {
		laserTimer->Stop();
		laserTimer->RemoveHist();		// remove histograms
		cout << "[Laser on/off timer STOPPED]" << endl;
	}
}

// start timer with new interval
void start(Int_t Refresh = 0, Int_t Binning = kDefaultBinning) {
	if (laserTimer) {
		laserTimer->SetBinning(Binning);
		laserTimer->ResetHist();		// clear histograms
		if (Refresh <= 0) {
			laserTimer->Start(-1, kTRUE);				// restart timer, same interval
		} else {
			laserTimer->Start(Refresh * 1000, kTRUE);	// restart timer
		}
		ULong_t r = (ULong_t) laserTimer->GetTime() / 1000;
		cout << "[Laser on/off timer RESTARTED: refreshing every " << r << "s, binning is " << Binning << " timer ticks]" << endl;
	}
}

// exit from root
void bye() {
	cout << "[laser.C: Exit]" << endl;
	gSystem->Exit(0);
};

// help message
void help() {
	cout	<< endl << "laser.C: Laser on/off monitor" << endl;
	cout	<< "Usage:  laser(n,b)   start execution" << endl;
	cout	<< "                     n being the timer interval in seconds (default: 1)" << endl;
	cout	<< "                     b being the number if timer ticks binned together (default: 4)" << endl;
	cout	<< "        stop()       stop timer" << endl;
	cout	<< "        start(n,b)   restart timer, set interval to n (default: previous value)" << endl;
	cout	<< "                     b binning" << endl;
	cout	<< "        bye()     exit" << endl << endl;
};

