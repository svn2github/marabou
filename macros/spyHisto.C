#include <iostream.h>
#include <iomanip.h>
#include "TSocket.h"
#include "TMessage.h"
#include "TCanvas.h"
#include "TTimer.h"
#include "TString.h"
#include "TH1F.h"
#include "TSystem.h"
#include "TMrbString.h"
#include "SetColor.h"

class SpyTimer : public TTimer {

	public:
    	SpyTimer(Long_t ms, Bool_t synch);
    	Bool_t Notify();
		inline void SetHist(const Char_t * Hist) {
			fHistName = Hist;
		};
		inline void SetSpyWindow(Int_t LowerBin, Int_t UpperBin) {
			fLowerSpy = LowerBin;
			fUpperSpy = UpperBin;
			TMrbString title = fHistName.Data();
			title += ": Count/s in window [";
			title += fLowerSpy;
			title += ",";
			title += fUpperSpy;
			title += "]";
			if (fSpyHist) fSpyHist->SetTitle(title.Data());
		};
		inline void SetSock(TSocket * Sock) { fSock = Sock; };
		inline void ResetHist() {
			if (fSpyHist) {
				fSpyHist->Reset();
				fSpyContents = 0;
			}
		};
		inline void SetAlarm(Double_t Thresh) { fThresh = Thresh; };

		inline void Close() {
			if (fSock) {
				fSock->Send("M_client exit");
				fSock->Close("force");
			}
		};
		void Alarm(Int_t Duration = 500);

	public:
		TSocket * fSock;
		TCanvas * fCanvas;
		TString fHistName;
		TH1F * fHist; 
		TH1F * fSpyHist; 
		Int_t fLowerBin;
		Int_t fUpperBin;
		Int_t fLowerSpy;
		Int_t fUpperSpy;
		Double_t fSpyContents;
		Int_t fSpyCount;
		Double_t fThresh;
		Int_t fAlarm;

	ClassDef(SpyTimer, 0)
};

SpyTimer::SpyTimer(Long_t MilliSecs, Bool_t SynchFlag): TTimer(MilliSecs, SynchFlag){
	gSystem->AddTimer(this);
	fSock = NULL;
	fCanvas = NULL;
	fHist = NULL;
	fSpyHist = NULL;
	fLowerBin = -1;
	fUpperBin = -1;
	fLowerSpy = -1;
	fUpperSpy = -1;
	fSpyContents = 0.;
	fSpyCount = 0;
	fThresh = 0.;
	fAlarm = 0;
}

Bool_t SpyTimer::Notify() {
	TMessage * message;

	if (fHistName.Length() == 0) return(kFALSE);

	TString mess("M_client gethist ");
	mess += fHistName;
	Int_t lmsg = fSock->Send(mess);
	if (lmsg == -1) {          // send message
		cerr	<< setred << "spyHisto.C: Connection lost." << setblack << endl;
		for (Int_t i = 0; i < 5; i++) this->Alarm(500);
		this->Stop();
		return(kFALSE);
	} else if (lmsg == 0) return(kTRUE);

	while ((fSock->Recv(*&message))) // receive next message
	{
		if ( message->What() == kMESS_STRING ) {
		} else if ( message->What() == kMESS_OBJECT ) {
			if (fCanvas == NULL) {
				fCanvas = new TCanvas();
				fCanvas->Divide(1, 2);
			}
			if (fHist) {
				fLowerBin = fHist->GetXaxis()->GetFirst();
				fUpperBin = fHist->GetXaxis()->GetLast();
				delete fHist;
			}
			fCanvas->cd(1);
			fHist = (TH1F *) message->ReadObject(message->GetClass());
			if (fHist == NULL) return(kFALSE);
			if (fLowerBin != -1) fHist->GetXaxis()->SetRange(fLowerBin, fUpperBin);
			fHist->Draw();
			fCanvas->cd(2);
			if (fLowerBin == -1) {
				fLowerBin = fHist->GetXaxis()->GetFirst();
				fUpperBin = fHist->GetXaxis()->GetLast();
			}
			if (fLowerSpy == -1) fLowerSpy = fLowerBin;
			if (fUpperSpy == -1) fUpperSpy = fUpperBin;
			TMrbString title = fHistName.Data();
			title += ": Count/s in window [";
			title += fLowerSpy;
			title += ",";
			title += fUpperSpy;
			title += "]";
			if (fSpyHist == NULL) fSpyHist = new TH1F("Spy", title.Data(), 300, 0, 300);
			fSpyHist->SetFillStyle(1001);
			fSpyHist->SetFillColor(3);
			for (Int_t i = 1; i < 298; i++) fSpyHist->SetBinContent(i, fSpyHist->GetBinContent(i+1));
			Double_t contents = fHist->Integral(fLowerSpy, fUpperSpy);
			if (fSpyContents > 0) {
				Double_t diff = contents - fSpyContents;
				fSpyHist->SetBinContent(298, diff);
				if (fThresh >= 0 && diff < fThresh) {
					fSpyHist->SetFillColor(2);
					this->Alarm();
				} else {
					fSpyHist->SetFillColor(3);
				}
			}
			fSpyContents = contents;
			fSpyHist->Draw();
			fCanvas->Update();
			this->Start(-1, kTRUE);
			return(kTRUE); 
		}
	}
	return(kTRUE);
}

void SpyTimer::Alarm(Int_t Duration) {
	const char bell[] = "\007";
	Int_t a;
	if (fAlarm == 0) {
		a = 400;
		fAlarm = 1;
	} else {
		a = (Int_t) (400 * pow(pow(2,1./12.), 1));
		fAlarm = 0;
	}
	Int_t loadness = 100;
	Int_t duration = Duration;
	TMrbString cmd;
	cmd = "xset b ";
	cmd += loadness;
	cmd += " ";
	cmd += a;
	cmd += " ";
	cmd += duration;
	gSystem->Exec(cmd.Data());
	cout << bell << flush;
}

void help() {
	cout	<< "spyHist.C: ROOT script to spy on histgrams and to monitor counting rate" << endl << endl;
	cout	<< "Usage:" << endl;
	cout	<< "        gROOT->Macro(\"LoadUtilityLibs.C\")" << endl;
	cout	<< "        .L spyHisto.C+" << endl;
	cout 	<< "        spyHisto(HistoName [, Refresh] [, Host] [, Socket])" << endl << endl;
	cout	<< "        HistoName       Name of histogram to be spyed on" << endl;
	cout	<< "        Refresh         Spy interval in secs (default: 5)" << endl;
	cout	<< "        Socket          Socket to listen to (default: 9090)" << endl;
	cout	<< "        Host            Host where M_analyze is running (default: localhost)" << endl << endl;
	cout	<< "Commands:" << endl;
	cout	<< "        spy(ll, ul)     Set spy window to [ll,ul] (default: entire histogram)" << endl;
	cout	<< "        alarm(n)        Set alarm to n count/s (default: 0, -1 -> turned off)" << endl;
	cout	<< "        stop()          Stop timer" << endl;
	cout	<< "        start(n)        Start timer, set spy interval to n (default: previous value)" << endl;
	cout	<< "        bye()           Close connection and exit (don't use \".q\"!)" << endl << endl;
}

static SpyTimer * spyTimer = NULL;

void spyHisto(	const Char_t * HistoName,
				Int_t Refresh = 5,
				Int_t TcpPort = 9090,
				const Char_t * HostName = "localhost")
//>>_________________________________________________(do not change this line)
//
{
	TMessage * message;

	enum	{	kTimeout	= 10	};

	cout << "[spyHist.C: Spy on a histogram and monitor counting rate. Type help() for help]" << endl;

// Open connection to server
	TSocket * sock = new TSocket(HostName, TcpPort);

// Wait till we get the start message
	Bool_t connect = kFALSE;
	if (sock->Recv(*&message) <= 0) {
		cout << "spyHisto.C: Trying to connect to M_analyze @ " << HostName << ":" << TcpPort << " - wait " << flush;
		for (Int_t i = 0; i < kTimeout; i++) {
			sleep(1);
			if (sock->Recv(*&message) > 0) {
				connect = kTRUE;
				break;
			} else {
				cout << "." << flush;
			}
		}
		if (connect) {
			cout << " done." << endl;
		} else {
			cout << setred << " timed out. Try again." << setblack << endl;
			return;
		}
	} else {
		cout << setblue << "[spyHisto.C: Connected to M_analyze @ " << HostName << ":" << TcpPort << "]" << setblack << endl;
	}

	if (spyTimer == NULL) {
		spyTimer = new SpyTimer(Refresh * 1000, kTRUE);
	} else {
		spyTimer->ResetHist();
	}
	spyTimer->SetSock(sock);
	spyTimer->SetHist(HistoName);
	spyTimer->Start(-1, kTRUE);
}

void spy(Int_t LowerSpy, Int_t UpperSpy) {
	if (spyTimer) {
		spyTimer->Stop();
		spyTimer->ResetHist();
		spyTimer->SetSpyWindow(LowerSpy, UpperSpy);
		spyTimer->Start(-1, kTRUE);
	}
};

void stop() {
	if (spyTimer) {
		spyTimer->Stop();
		cout << "[Spy timer STOPPED]" << endl;
	}
}

void start() {
	if (spyTimer) {
		spyTimer->ResetHist();
		spyTimer->Start(-1, kTRUE);
		cout << "[Spy timer STARTED]" << endl;
	}
}

void alarm(Double_t Thresh = 0) {
	if (spyTimer) {
		spyTimer->SetAlarm(Thresh);
		if (Thresh < 0) {
			cout << "[Alarm TURNED OFF]" << endl;
		} else {
			cout << "[Alarm threshold set to " << Thresh << "]" << endl;
		}
	}
};

void bye() {
	if (spyTimer) {
		spyTimer->Close();
		cout << "[spyHisto.C: Exit]" << endl;
		gSystem->Exit(0);
	}
};
