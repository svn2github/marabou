//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFEventBuffer.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TROOT.h"
#include "TFile.h"

#include "TMrbLogger.h"

#include "TMrbDGFData.h"
#include "TMrbDGF.h"
#include "TMrbDGFCommon.h"
#include "TMrbDGFEvent.h"
#include "TMrbDGFEventBuffer.h"

#include "n2ll.h"

#include "SetColor.h"

extern TMrbDGFData * gMrbDGFdata;				// common data base for all DGF modules
extern TMrbLogger * gMrbLog;

ClassImp(TMrbDGFEventBuffer)

TMrbDGFEventBuffer::TMrbDGFEventBuffer(const Char_t * BufferName, TObject * Module, Int_t Index, Int_t Event0) :
																		TNamed(BufferName, "DGF-4C Event Buffer") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer
// Purpose:        DGF's list mode buffer
// Description:    Describes the list mode buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	Reset();
	fModule = Module;
	fGlobIndex = Index;
	fGlobEventNo = Event0;
	fHistogram = NULL;
	fTauFit = NULL;
}

void TMrbDGFEventBuffer::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::Reset
// Purpose:        Reset to initial values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets event buffer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fNofWords = 0;
	fNofEvents = 0;
	fEventIndex.Delete(); 	// reset event index
	fTraceError = 0;
	this->Set(0);			// reset data
}

void TMrbDGFEventBuffer::MakeIndex() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::MakeIndex
// Purpose:        Analyze buffer data
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Analyzes buffer data: number of events, indices etc.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t bufIdx;
	Int_t chnAddr;
	Int_t hit;
	Int_t nofChannels;
	TMrbDGFEvent * evt;

	bufIdx = 0;
	fNofWords = fArray[bufIdx + TMrbDGFEventBuffer::kBufNofWords];
	fNofEvents = 0;
	fEventIndex.Delete();

	bufIdx += TMrbDGFEventBuffer::kBufHeaderLength;
	while (bufIdx < fNofWords) {
		evt = new TMrbDGFEvent();
		fNofEvents++;
		nofChannels = 0;
		hit = fArray[bufIdx];				// hit pattern
		evt->SetHitPattern(hit);
		bufIdx += TMrbDGFEventBuffer::kEventHeaderLength;
		for (i = 0; i < TMrbDGFData::kNofChannels; i++) {
			if (hit & 1) {					// test channel by channel
				chnAddr = bufIdx;			// active channel: insert it in index
				bufIdx += fArray[bufIdx];
				nofChannels++;
			} else {
				chnAddr = 0; 				// channel inactive: skip it
			}
			evt->SetChannel(i, chnAddr);
			hit >>= 1;						// try next channel
		}
		evt->SetNofChannels(nofChannels);
		fEventIndex.AddLast((TObject *) evt);
	}
}

Int_t TMrbDGFEventBuffer::GetNofChannels(Int_t EventNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::GetNofChannels
// Purpose:        Return number of active channels
// Arguments:      Int_t EventNumber    -- event number
// Results:        Int_t NofChannels    -- number of active channels
// Exceptions:
// Description:    Returns the number of active channels for a given event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEvent * evt;

	if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
						<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofChannels");
		return(-1);
	}
	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	return(evt->GetNofChannels());
}

UInt_t TMrbDGFEventBuffer::GetHitPattern(Int_t EventNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::GetHitPattern
// Purpose:        Return pattern of active channels
// Arguments:      Int_t EventNumber    -- event number
// Results:        UInt_t HitPattern    -- pattern
// Exceptions:
// Description:    Returns the pattern of active channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEvent * evt;

	if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
						<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetHitPattern");
		return(0xffffffff);
	}
	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	return(evt->GetHitPattern());
}

Int_t TMrbDGFEventBuffer::GetChannel(Int_t EventNumber, Int_t Channel, TArrayI & Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::GetChannel
// Purpose:        Return data for a given channel
// Arguments:      Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
//                 TArrayI & Data       -- where to store channel data
// Results:        Int_t NofWords       -- number of words
// Exceptions:
// Description:    Returns data of a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEvent * evt;
	Int_t i, k, wc;
	Int_t chnAddr;

	if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
						<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannel");
		return(-1);
	}
	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannel");
		return(-1);
	}
	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	chnAddr = evt->GetChannel(Channel);
	if (chnAddr > 0) {
		wc = fArray[chnAddr + TMrbDGFEventBuffer::kChanNofWords] - TMrbDGFEventBuffer::kChanHeaderLength;
		Data.Set(wc);
		k = chnAddr + TMrbDGFEventBuffer::kChanHeaderLength;
		for (i = 0; i < wc; i++, k++) Data[i] = fArray[k];
		return(wc);
	}
	return(-1);
}	

Int_t TMrbDGFEventBuffer::GetNofWords(Int_t EventNumber, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::GetNofWords
// Purpose:        Return number of data words for a channel
// Arguments:      Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
// Results:        Int_t NofWords       -- number of words
// Exceptions:
// Description:    Returns number of words for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEvent * evt;
	Int_t chnAddr;
	Int_t wc;

	if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
						<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofWords");
		return(-1);
	}
	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofWords");
		return(-1);
	}
	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	chnAddr = evt->GetChannel(Channel);
	wc = (chnAddr == 0) ? 0 : fArray[chnAddr + TMrbDGFEventBuffer::kChanNofWords];
	return(wc);
}	

Double_t TMrbDGFEventBuffer::CalcTau(Int_t EventNumber, Int_t Channel, Int_t GlobalEventNo,
								Double_t A0, Double_t A1, Double_t A2, Int_t From, Int_t To,
								Bool_t Verbose, Bool_t DrawIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::CalcTau
// Purpose:        Calculate tau value
// Arguments:      Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
//                 Int_t GlobalEventNo  -- global event number
//                 Double_t A0          -- start value for a(0)
//                 Double_t A1          -- start value for a(1)
//                 Int_t From           -- fit region, lower limit
//                 Int_t To             -- fit region, upper limit
//                 Int_t Verbose        -- verbose mode?
//                 Bool_t DrawIt        -- draw histo + fit
// Results:        Double_t Tau         -- tau value
// Exceptions:
// Description:    Performs a fit on trace data
//                     f(x) = a(0) + a(1) * exp(a(2) * x)
//                 then calculates tau
//                     tau = 1 / (40 * a(2))
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i, wc;
	TArrayI data;
	Double_t tau;
	TMrbString hName, hTitle;
	TString fitOpt;
	Int_t evtNo;
	TMrbDGFEvent * evt;

	wc = this->GetChannel(EventNumber, Channel, data);
	if (wc < 0) {
		gMrbLog->Err() << "No trace data for event " << EventNumber << ", channel " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "CalcTau");
		return(-1);
	}
	
	if (To == 0) To = wc - 1;

	if (From >= To) {
		gMrbLog->Err() << "Illegal fit region - [" << From << "," << To << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "CalcTau");
		return(-1);
	}

	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	evtNo = (GlobalEventNo > 0) ? GlobalEventNo : EventNumber;

	hName = "ht"; hName += evtNo; hName += "c"; hName += Channel;
	hTitle = "Fitted trace data for evt "; hTitle += evtNo; hTitle += ", chn "; hTitle += Channel;

	if (fHistogram != NULL) {
		gROOT->GetList()->Remove(fHistogram);
		delete fHistogram;
	}

	fHistogram = new TH1F(hName.Data(), hTitle.Data(), wc, 0., (Float_t) wc);

	for (i = 1; i <= wc; i++) {
		fHistogram->SetBinContent(i, (Stat_t) data[i - 1]);
		if (fTraceError > 0) fHistogram->SetBinError(i, fTraceError);
	}

	if (fTauFit != NULL) {
		gROOT->GetList()->Remove(fTauFit);
		delete fTauFit;
	}

	hName = "ft"; hName += evtNo; hName += "c"; hName += Channel;
	fTauFit = new TF1(hName.Data(), "[0] + [1]*exp(-[2]*x)", (Double_t) From, (Double_t) To);

	fTauFit->SetParameters(A0, A1, A2);
	fTauFit->SetParName(0,"exp_const ");
	fTauFit->SetParName(1,"exp_ampl ");
	fTauFit->SetParName(2,"exp_tau ");
	fTauFit->SetLineColor(4);
	fTauFit->SetLineWidth(3);

	if (DrawIt) fHistogram->Draw();

	fitOpt = "R";
	if (!Verbose) fitOpt += "Q";
	if (!DrawIt) fitOpt += "0";
	fitOpt += "+";
	fHistogram->Fit(hName.Data(), fitOpt, "SAME");

	tau = 1. / (40. * fTauFit->GetParameter(2));
	if (Verbose) {
		gMrbLog->Out()	<< "Event " << EventNumber << ", channel " << Channel
						<< " --> fitted tau value = " << tau << endl;
		gMrbLog->Flush(this->ClassName(), "CalcTau()", setblue);
	}
	evt->SetTau(Channel, tau);				// store tau value
	return(tau);
}	

Double_t TMrbDGFEventBuffer::GetTau(Int_t EventNumber, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::GetTau
// Purpose:        Recall calculated tau value
// Arguments:      Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
// Results:        Double_t Tau         -- tau value
// Exceptions:
// Description:    Returns tau value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEvent * evt;

	if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
						<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTau");
		return(-1);
	}
	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTau");
		return(-1);
	}
	evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
	return(evt->GetTau(Channel));
}

TH1F * TMrbDGFEventBuffer::FillHistogram(Int_t EventNumber, Int_t Channel, Bool_t DrawIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::FillHistogram
// Purpose:        Fill a histogram with trace data
// Arguments:      Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
//                 Bool_t DrawIt        -- draw histogram
// Results:        TH1F * Histogram     -- address of internal histogram
// Exceptions:
// Description:    Fills a histogram with event (trace) data and draws it.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i, wc;
	TArrayI data;
	TMrbString hName, hTitle;

	wc = this->GetChannel(EventNumber, Channel, data);
	if (wc < 0) return(NULL);

	if (fHistogram != NULL) {
		gROOT->GetList()->Remove(fHistogram);
		delete fHistogram;
	}

	hName = "h"; hName += EventNumber; hName += "c"; hName += Channel;
	hTitle = "Trace data for evt "; hTitle += EventNumber; hTitle += ", chn "; hTitle += Channel;
	fHistogram = new TH1F(hName.Data(), hTitle.Data(), wc, 0., (Float_t) wc);

	for (i = 1; i <= wc; i++) {
		fHistogram->SetBinContent(i, (Stat_t) data[i - 1]);
		if (fTraceError > 0) fHistogram->SetBinError(i, fTraceError);
	}
	if (DrawIt) fHistogram->Draw();
	return(fHistogram);
}	

Bool_t TMrbDGFEventBuffer::SaveTraces(const Char_t * TraceFile, Int_t EventNumber, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::SaveTraces
// Purpose:        Save trace data to root file
// Arguments:      Char_t * TraceFile   -- file name
//                 Int_t EventNumber    -- event number
//                 Int_t Channel        -- channel number
// Results:        Int_t NofWords       -- number of words
// Exceptions:
// Description:    Saves trace data as histograms in root format.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i, j, k, wc;
	TArrayI data;
	TMrbString traceFile;
	TMrbString hName, hTitle;
	Int_t nofTraces, nofEvents;
	Int_t evtNo, chnNo;
	TFile * tFile;
	TH1F * h;
	ofstream hList;
	TString hListName;
	TMrbString hln;

	if (fNofEvents == 0) {
		gMrbLog->Err() << "Event buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveTraces");
		return(kFALSE);
	}

	tFile = NULL;
	h = NULL;

	if (TraceFile == NULL || *TraceFile == '\0') {		// default name is "dgfTrace.<module>.<channel>.root
		traceFile = "dgfTrace.";
		if (fModule != NULL) {
			traceFile += ((TMrbDGF *) fModule)->GetName();
			traceFile += ".";
		}
		if (Channel != -1) {
			traceFile += "chn";
			traceFile += Channel;
		}
		traceFile += ".root";
	} else {
		traceFile = TraceFile;
	}
	
	hListName = traceFile;
	Int_t idx = hListName.Index(".root", 0);
	if (idx > 0) hListName.Resize(idx);
	hln = "rm -f "; hln += hListName; hln += "-*.histlist";
	gSystem->Exec(hln.Data());

	nofTraces = 0;
	nofEvents = 0;
	for (i = 0; i < fNofEvents; i++) {
		evtNo = (EventNumber == -1) ? i : EventNumber;
		if (i == evtNo) {
			nofEvents++;
			for (j = 0; j < TMrbDGFData::kNofChannels; j++) {
				chnNo = (Channel == -1) ? j : Channel;
				if (j == chnNo) {
					wc = this->GetChannel(i, j, data);
					if (wc > 0) {
						nofTraces++;
						if ((nofTraces % 16) == 1) {
							if (nofTraces != 1 && hList.good()) hList.close();
							hln = hListName; hln += "-"; hln += nofTraces; hln += ".histlist";
							hList.open(hln.Data(), ios::out);
						}
						if (tFile == NULL) {
							tFile = new TFile(traceFile.Data(), "RECREATE");
							if (!tFile->IsOpen()) {
								gMrbLog->Err() << "Can't open file " << traceFile << endl;
								gMrbLog->Flush(this->ClassName(), "SaveTraces");
								return(kFALSE);
							}
						}
						hName = "ht"; hName += i; hName += "c"; hName += j;
						hTitle = "Trace data for evt "; hTitle += i; hTitle += ", chn "; hTitle += j;
						if (h) delete h;
						h = new TH1F(hName.Data(), hTitle.Data(), wc, 0., (Float_t) wc);
						for (k = 1; k <= wc; k++) {
							h->SetBinContent(k, (Stat_t) data[k - 1]);
							if (fTraceError > 0) h->SetBinError(k, fTraceError);
						}
						h->Write();
						if (hList.good()) hList << traceFile << " " << h->GetName() << endl;
					}
					if (Channel != -1) break;
				}
			}
			if (EventNumber != -1) break;
		}
	}
	if (tFile) tFile->Close();
	if (hList.good()) hList.close();

	if (nofTraces == 0) {
		gMrbLog->Err() << "No events/traces found" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveTraces");
		return(kFALSE);
	} else {
		gMrbLog->Out() << nofEvents << " event(s) (" << nofTraces << " trace(s)) saved to " << TraceFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveTraces()", setblue);
		return(kTRUE);
	}
}	

void TMrbDGFEventBuffer::Print(ostream & OutStrm, Int_t EventNumber, const Char_t * ModuleInfo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer::Print
// Purpose:        Print buffer headers
// Arguments:      ostream & OutStrm       -- output stream
//                 Int_t EventNumber       -- event number
//                 Char_t * ModuleInfo     -- module info
// Results:        --
// Exceptions:
// Description:    Outputs buffer / event / channel headers. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString timStr;
	TMrbDGFEvent * evt;
	Int_t chnAddr;
	Int_t i;
	UShort_t evtTime[3];

	if (EventNumber < 0) {
		if (fNofWords > 0) {
			evtTime[0] = fArray[TMrbDGFEventBuffer::kRunStartTime];
			evtTime[1] = fArray[TMrbDGFEventBuffer::kRunStartTime + 1];
			evtTime[2] = fArray[TMrbDGFEventBuffer::kRunStartTime + 2];
			OutStrm	<< "===========================================================================================" << endl;
			if (ModuleInfo) {
				OutStrm	<< " DGF-4C Event buffer: Module " << ModuleInfo << endl;
			} else {
				OutStrm	<< " DGF-4C Event buffer" << endl;
			}
			OutStrm	<< "..........................................................................................." << endl
						<< " Number of data words        : " << fNofWords << endl
						<< " Module number               : " << fArray[TMrbDGFEventBuffer::kModNumber] << endl
						<< " Format descriptor           : 0x"
						<< setbase(16) << fArray[TMrbDGFEventBuffer::kFormat] << setbase(10) << endl
						<< " Run start time              : " << ushort2ll48(evtTime)
						<< setbase(16) << " (0x" << fArray[TMrbDGFEventBuffer::kRunStartTime]
						<< " 0x" << fArray[TMrbDGFEventBuffer::kRunStartTime + 1]
						<< " 0x" << fArray[TMrbDGFEventBuffer::kRunStartTime + 2]
						<< ")" << endl
						<< " Number of events            : " << fNofEvents << endl
						<< " Common trace error          : " << fTraceError << endl;
			if (fGlobIndex != -1) OutStrm	<< " Global buffer index         : " << fGlobIndex << endl;
			OutStrm	<< "-------------------------------------------------------------------------------------------" << endl;
			for (Int_t i = 0; i < this->GetNofEvents(); i++) this->Print(OutStrm, i);
		} else {
			gMrbLog->Err() << "Event buffer is empty" << endl;
			gMrbLog->Flush(this->ClassName(), "Print");
		}
	} else if (EventNumber >= fNofEvents) {
		gMrbLog->Err()	<< "Event number out of range - " << EventNumber
							<< " (should be in [0," << fNofEvents - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
	} else {
		evt = (TMrbDGFEvent *) fEventIndex.At(EventNumber);
		evtTime[0] = fArray[TMrbDGFEventBuffer::kRunStartTime];
		evtTime[1] = fArray[TMrbDGFEventBuffer::kBufHeaderLength + TMrbDGFEventBuffer::kEventTime];
		evtTime[2] = fArray[TMrbDGFEventBuffer::kBufHeaderLength + TMrbDGFEventBuffer::kEventTime + 1];
		OutStrm	<< "===========================================================================================" << endl;
		if (fGlobIndex == -1)	OutStrm	<< " Event # " << EventNumber << endl;
		else					OutStrm	<< " Event # " << EventNumber << " (local), "
											<< (EventNumber - fGlobEventNo) << " (global)" << endl;
		OutStrm	<< "..........................................................................................." << endl
					<< " Event time                  : " << ushort2ll48(evtTime)
					<< setbase(16) << " ([0x" << fArray[TMrbDGFEventBuffer::kRunStartTime]
					<< "] 0x" << fArray[TMrbDGFEventBuffer::kBufHeaderLength + TMrbDGFEventBuffer::kEventTime]
					<< " 0x" << fArray[TMrbDGFEventBuffer::kBufHeaderLength + TMrbDGFEventBuffer::kEventTime + 1]
					<< ")" << endl
					<< " Number of active channels   : " << evt->GetNofChannels() << endl
					<< " Hit pattern                 : " << evt->GetHitPattern() << endl
					<< "-------------------------------------------------------------------------------------------" << endl;
		if (evt->GetNofChannels() > 0) {
			for (i = 0; i < TMrbDGFData::kNofChannels; i++) {
				chnAddr = evt->GetChannel(i);
				if (chnAddr > 0) {
					if (i > 0) OutStrm	<< "..........................................................................................." << endl;
					OutStrm	<< " Channel # " << i << endl
								<< "..........................................................................................." << endl
								<< " Number of data words        : " << fArray[chnAddr + TMrbDGFEventBuffer::kChanNofWords] << endl
								<< " Fast Trigger                : " << fArray[chnAddr + TMrbDGFEventBuffer::kFastTrigger] << endl
								<< " Energy                      : " << fArray[chnAddr + TMrbDGFEventBuffer::kEnergy] << endl
								<< " Const fraction trigger time : " << fArray[chnAddr + TMrbDGFEventBuffer::kConstFractTriggerTime] << endl;
				}
			}
			OutStrm	<< "-------------------------------------------------------------------------------------------" << endl;
		}
	}
}

Bool_t TMrbDGFEventBuffer::PrintToFile(const Char_t * FileName, const Char_t * ModuleInfo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintToFile
// Purpose:        Print event buffer to file
// Arguments:      Char_t * FileName             -- file name
//                 char _t * ModuleInfo          -- module info
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs event data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofWords;
	TString fileName;
	ofstream bufOut;
	
	this->MakeIndex();
	nofWords = this->GetNofWords();
	if (nofWords == 0) {
		gMrbLog->Err()	<< ModuleInfo << ": Event buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintToFile");
		return(kFALSE);
	}

	fileName = FileName;
	fileName = fileName.Strip(TString::kBoth);
	
	bufOut.open(fileName.Data(), ios::out);
	if (!bufOut.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintToFile");
		return(kFALSE);
	}

	this->Print(bufOut, -1, ModuleInfo);
	bufOut.close();
	gMrbLog->Out()	<< ModuleInfo << ": Event buffer written to file " << fileName
						<< " (" << nofWords << " word(s))" << endl;
	gMrbLog->Flush(this->ClassName(), "PrintToFile", setblue);
	return(kTRUE);
}
	
