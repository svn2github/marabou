//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    How to store multiplexer data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrMux.cxx,v 1.1 2007-10-27 16:26:30 Marabou Exp $       
// Date:           $Date: 2007-10-27 16:26:30 $
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TSystem.h"

#include "TMrbAnalyze.h"
#include "TMrbLogger.h"
#include "TUsrMux.h"
#include "TMrbMesytec_Mux16.h"

#include "SetColor.h"

extern TMrbAnalyze * gMrbAnalyze;
extern TMrbLogger * gMrbLog;					// message logger

TUsrMuxEntry theEmptyMux(NULL, NULL, NULL);	// place holder in channel table
TH1F theEmptyHisto("empty", "empty", 1, 0.0, 1.0);			// place holder in lookup table

ClassImp(TUsrMux)
ClassImp(TUsrMuxEntry)

TUsrMux::TUsrMux(const Char_t * ConfigFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux
// Purpose:        Class to store data from Mesytec Mux16 multiplexer
// Arguments:      Char_t * ConfigFile    -- configuration
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fDebug = gEnv->GetValue("TUsrMux.DebugMode", kFALSE);
	fDebugLock = "";
	if (fDebug) {
		fDebugLock = gEnv->GetValue("TUsrMux.DebugLock", "lock");
		fVerbose = kTRUE;
		gMrbLog->Out()	<< "DebugMode is ON! Lock file = " << fDebugLock << endl;
		gMrbLog->Flush(this->ClassName(), "", setblue);
		gSystem->Exec(Form("touch %s", fDebugLock.Data()));
	}
	if (!fDebug) {
		fVerbose = gEnv->GetValue("TUsrMux.VerboseMode", kFALSE);
		if (fVerbose) {
			gMrbLog->Out()	<< "VerboseMode is ON!" << endl;
			gMrbLog->Flush(this->ClassName(), "", setblue);
		}
	}
	Init(ConfigFile);
	ClearTables();
}

Bool_t TUsrMux::Init(const Char_t * ConfigFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::Init
// Purpose:        Initialize tables
// Arguments:      Char_t * ConfigFile    -- configuration
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Setup tables.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gSystem->AccessPathName(ConfigFile, kFileExists)) {
		gMrbLog->Err()	<< "No such file - " << ConfigFile << endl;
		gMrbLog->Flush(this->ClassName(), "Init");
		return(kFALSE);
	}

	fConfig = new TEnv(ConfigFile);

	fIndexOfFirstHisto = -1;
	fNofHistograms = 0;

	fChanTable.Delete();
	fLofMuxEntries.Delete();
	fLofMuxHits.Delete();
	fLofLookups.Delete();

	Int_t nofMux = fConfig->GetValue("TMrbConfig.Mux.NofMultiplexers", 0);
	if (nofMux == 0) {
		gMrbLog->Err()	<< "No mux in config file " << ConfigFile << endl;
		gMrbLog->Flush(this->ClassName(), "Init");
		return(kFALSE);
	}

	fChansPerModule = fConfig->GetValue("TMrbConfig.Mux.NofParams", 0);
	if (fChansPerModule == 0) {
		gMrbLog->Err()	<< "Wrong number of chans per module - 0" << endl;
		gMrbLog->Flush(this->ClassName(), "Init");
		return(kFALSE);
	}
	fModuleRange = fConfig->GetValue("TMrbConfig.Mux.ModuleRange", 0);
	if (fModuleRange == 0) {
		gMrbLog->Err()	<< "Wrong module range - 0" << endl;
		gMrbLog->Flush(this->ClassName(), "Init");
		return(kFALSE);
	}

	fSmin = fConfig->GetValue("TMrbConfig.Mux.Smin", 0);
	fSmax = fConfig->GetValue("TMrbConfig.Mux.Smax", 0);

	Int_t size = (fSmax - fSmin + 1) * fChansPerModule;
	fChanTable.Expand(size);
	for (Int_t i = 0; i < size; i++) fChanTable[i] = &theEmptyMux;

	for (Int_t i = 0; i < nofMux; i++) {
		Int_t adcSerial = fConfig->GetValue(Form("TMrbConfig.Mux.%d.AdcSerial", i), 0);
		Int_t firstChan = fConfig->GetValue(Form("TMrbConfig.Mux.%d.FirstChannel", i), 0);
		TString lkpFile = fConfig->GetValue(Form("TMrbConfig.Mux.%d.LookupFile", i), "");
		TEnv * lkp = new TEnv(lkpFile.Data());
		TString muxName = lkp->GetValue("TUsrMux.Name", "mux??");
		TUsrMuxLookup * lookup = this->MakeLookupTable(lkp);
		if (lookup == NULL) {
			gMrbLog->Err()	<< muxName << ": Couldn't make lookup table" << endl;
			gMrbLog->Flush(this->ClassName(), "Init");
		} else {
			fLofLookups.Add(lookup);
			Int_t start = (adcSerial - fSmin) * fChansPerModule + firstChan;
			if (start < 0) {
				gMrbLog->Err()	<< muxName << ": Serial number out of range - " << adcSerial
								<< " < lower bound (" << fSmin << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "Init");
			}
			for (Int_t i = start; i < start + 4; i++)  fChanTable[i] = new TUsrMuxEntry(lkp, lookup, (TUsrHit *) NULL);
		}
	}
	return(kTRUE);
}

TUsrMuxLookup * TUsrMux::MakeLookupTable(TEnv * LkpEnv) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::MakeLookupTable
// Purpose:        Make lookup table
// Arguments:      TEnv * LkpEnv             -- lookup data
// Results:        TUsrMuxLookup * LkpTable  -- lookup table
// Exceptions:
// Description:    Fill lookup table
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString muxName = LkpEnv->GetValue("TUsrMux.Name", "mux??");
	Int_t muxSerial = LkpEnv->GetValue("TUsrMux.Serial", 0);
	Int_t nofChans = LkpEnv->GetValue("TUsrMux.NofParams", 0);
	Int_t nofSub = LkpEnv->GetValue("TUsrMux.NofSubmodules", 0);
	Int_t moduleRange = LkpEnv->GetValue("TUsrMux.ModuleRange", 0);
	TUsrMuxLookup * lookup = new TUsrMuxLookup(muxName.Data(), moduleRange);
	TArrayI * paramData = new TArrayI(nofChans);
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< muxName << " muxSerial=" << muxSerial << " chans=" << nofChans << " subs=" << nofSub
						<< " range=" << moduleRange << endl;
		gMrbLog->Flush(this->ClassName(), "MakeLookupTable");
	}
	for (Int_t chn = 0; chn < nofChans; chn++) {
		Int_t xmin = LkpEnv->GetValue(Form("TUsrMux.%d.Xmin", chn), 0);
		Int_t xmax = LkpEnv->GetValue(Form("TUsrMux.%d.Xmax", chn), 0);
		TString param = LkpEnv->GetValue(Form("TUsrMux.%d.Param", chn), "??");
		TString histo = LkpEnv->GetValue(Form("TUsrMux.%d.Histogram", chn), "??");
		TH1F * h = (TH1F *) gROOT->FindObject(histo.Data());
		if (h == NULL) {
			gMrbLog->Err()	<< muxName << ": No such histogram - " << histo << " (chn" << chn << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "MakeLookupTable");
			h = &theEmptyHisto;
		}
		gMrbAnalyze->AddParamToList(param.Data(), (TObject *) (paramData->GetArray() + chn), muxSerial, chn);
		gMrbAnalyze->AddHistoToList(h, muxSerial, chn);
		fNofHistograms++;

		Int_t hIndex = gMrbAnalyze->GetHistoIndex(muxSerial, chn);
		if (fIndexOfFirstHisto == -1) fIndexOfFirstHisto = hIndex;

		if ((xmax - xmin) > 0) {
			Int_t x4 = (xmax - xmin + 1) / 4;
			Int_t x2 = (xmax - xmin + 1) - 2 * x4;
			Int_t x = xmin;
			for (Int_t k = 0; k < x4; k++, x++) lookup->AddAt(-hIndex, x);
			for (Int_t k = 0; k < x2; k++, x++) lookup->AddAt(hIndex, x);
			for (Int_t k = 0; k < x4; k++, x++) lookup->AddAt(-hIndex, x);
		}

		if (this->IsVerbose()) {
			gMrbLog->Out()	<< muxName << " chn=" << chn << " xmin=" << xmin << " xmax=" << xmax
							<< " h=" << h->GetName() << " idx=" << hIndex << endl;
			gMrbLog->Flush(this->ClassName(), "MakeLookupTable");
		}
	}
	return(lookup);
}

void TUsrMux::ClearTables() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::ClearTables
// Purpose:        Reset tables
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clear tables
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofMuxEntries.Clear();
	fLofMuxHits.Clear();
}

TUsrMuxEntry * TUsrMux::IsMuxChannel(TUsrHit * Hit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::IsMuxChannel
// Purpose:        Check if mux channel
// Arguments:      TUsrHit * Hit            -- hit
// Results:        TUsrMuxEntry * MuxEntry  -- pointer to entry in chan table
// Exceptions:
// Description:    Check if mux channel and store it in table if so.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t modNo = Hit->GetModuleNumber();
	Int_t chn = Hit->GetChannel();
	if (modNo < fSmin || modNo > fSmax) return(NULL);
	modNo -= fSmin;
	TUsrMuxEntry * e = (TUsrMuxEntry *) fChanTable[modNo * fChansPerModule + chn];
	return((e == &theEmptyMux) ? NULL : e);
}

Bool_t TUsrMux::CollectHits(TUsrHBX * Hbx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::CollectHits
// Purpose:        Collect hits from a hit buffer
// Arguments:      TUsrHBX * HbufIndex    -- hit buffer index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Fill the hit list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofMuxEntries.Clear();

	Hbx->ResetIndex();
	TUsrHit * hit;
	while (hit = Hbx->NextHit()) {
		TUsrMuxEntry * muxEntry = IsMuxChannel(hit);
		if (muxEntry) {
			if (this->IsVerbose()) {
				TString output;
				gMrbLog->Out()	<< "mod=" << hit->GetModuleNumber() << " chn=" << hit->GetChannel()
								<< " lkp: " << muxEntry->PrintEntry(output) << endl;
				gMrbLog->Flush(this->ClassName(), "CollectHits");
			}
			muxEntry->fHit = hit;
			fLofMuxEntries.Add(muxEntry);
		}
	}

	Int_t nofHits = fLofMuxEntries.GetEntriesFast();
	if (nofHits == 0) return(kTRUE);

	if ((nofHits % 4) != 0) {
		gMrbLog->Err()	<< "Wrong number of hits in mux table - " << nofHits << " (should be % 4)" << endl;
		gMrbLog->Flush(this->ClassName(), "GetMuxHits");
		this->ClearTables();
		return(kFALSE);
	}

	Int_t nofEntries = nofHits / 4;
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "hits=" << nofHits << " entries=" << nofEntries << endl;
		gMrbLog->Flush(this->ClassName(), "CollectHits");
	}
	Int_t k = 0;
	for (Int_t i = 0; i < nofEntries; i++) {
		for (Int_t n = 0; n < 2; n++, k += 2) {
			TUsrMuxEntry * e = (TUsrMuxEntry *) fLofMuxEntries[k];
			TUsrMuxEntry * p = (TUsrMuxEntry *) fLofMuxEntries[k + 1];
			Int_t pos = p->fHit->GetData();
			Int_t muxChan = p->fLookup->At(pos);
			if (this->IsVerbose()) {
				TString hName;	
				if (muxChan != 0) {
					TH1F * h = (TH1F *) gMrbAnalyze->GetHistoAddr(abs(muxChan));
					hName = h ? h->GetName() : "??";
				} else {
					hName = "none";
				}
				gMrbLog->Out()	<< i << " " << n << " pos=" << pos
								<< " hidx=" << muxChan << " h=" << hName << endl;
				gMrbLog->Flush(this->ClassName(), "CollectHits");
			}
			if (muxChan != 0) {
				e->fHit->SetMuxChannel(abs(muxChan));
				fLofMuxHits.Add(e->fHit);
				p->fLookup->CountHit(muxChan);
				p->fLookup->CalculateRatio();
				if (fDebug) gMrbAnalyze->WaitForLock(fDebugLock.Data(), "CollectHits");
			}
		}
	}
	fLofMuxEntries.Clear();
	if (fDebug && (fLofMuxHits.GetEntriesFast() > 0)) {
		for (Int_t i = 0; i < fLofMuxHits.GetEntriesFast(); i++) {
			TUsrHit * hit = (TUsrHit *) fLofMuxHits[i];
			cout << hit->GetMuxChannel() << " | "; hit->Print();
		}
		gMrbAnalyze->WaitForLock(fDebugLock.Data(), "CollectHits");
	}
	return(kTRUE);
}	

void TUsrMux::AccuSingles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::AccuSingles
// Purpose:        Accumulate single mux channels to histos
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Accumulate event data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetNofHits() == 0) return;
	
	this->ResetIndex();
	TUsrHit * hit;
	while (hit = this->NextHit()) {
		TH1F * h = (TH1F *) gMrbAnalyze->GetHistoAddr(hit->GetMuxChannel());
		if (h) h->Fill((Axis_t) hit->GetEnergy());
	}
	if (fDebug) gMrbAnalyze->WaitForLock(fDebugLock.Data(), "AccuSingles");
}	

void TUsrMux::PrintChanTable() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::PrintChannelTable
// Purpose:        Output channel table to cout
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Diagnostics
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString output;
	cout << endl << "Channel table ------------------------------------------------------------" << endl;
	for (Int_t i = 0; i < fChanTable.GetEntries(); i++) {
		TUsrMuxEntry * m = (TUsrMuxEntry *) fChanTable[i];
		if (m != &theEmptyMux) cout << m->PrintEntry(output) << endl;
	}
	cout << "--------------------------------------------------------------------------" << endl << endl;
}

const Char_t * TUsrMuxEntry::PrintEntry(TString & Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMuxEntry::PrintEntry
// Purpose:        Output mux entry data to cout
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Diagnostics
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString muxName = fLkpEnv->GetValue("TUsrMux.Name", "mux??");
	TString moduleName = fLkpEnv->GetValue("TUsrMux.Module", "module??");
	Int_t serial = fLkpEnv->GetValue("TUsrMux.AdcSerial", 0);
	Output = Form("%-8s %-8s %2d  %4d", muxName.Data(), moduleName.Data(), serial, fLookup->GetEntries());
	return(Output.Data());
}

void TUsrMux::PrintLookup(TUsrMuxLookup * Lookup) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::PrintLookup
// Purpose:        Output lookup table to cout
// Arguments:      TObjArray * Lookup  -- lookup table
// Results:        --
// Exceptions:
// Description:    Diagnostics
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Lookup->GetEntries() == 0) {
		gMrbLog->Err()	<< "Lookup table for " << Lookup->GetName() << " is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintLookup");
		return;
	}

	cout << endl << "Lookup table for " << Lookup->GetName() << " ---------------------------------------------------" << endl;
	Int_t lastChan = 0;
	Int_t xmin = 0;
	for (Int_t i = 0; i < Lookup->GetSize(); i++) {
		Int_t muxChan = Lookup->At(i);
		if (abs(muxChan) == lastChan) continue;
		if (lastChan != 0) {
			TH1F * h = (TH1F *) gMrbAnalyze->GetHistoAddr(lastChan);
			TString hName = h ? h->GetName() : "undef";
			cout << Form("%2d %-8s %4d ... %4d", lastChan, hName.Data(), xmin, i - 1) << endl;
		}
		xmin = i;
		lastChan = abs(muxChan);
	}
	cout << "--------------------------------------------------------------------------" << endl << endl;
}

void TUsrMux::PrintLofMuxHits() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux::PrintLofMuxHits
// Purpose:        Output list of hits
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Diagnostics
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fLofMuxHits.GetEntriesFast() == 0) {
		gMrbLog->Err()	<< "Hit list is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintLofMuxHits");
		return;
	}

	cout << endl << "Hit list --------------------------------------------------------------" << endl;
	TIterator * mIter = fLofMuxHits.MakeIterator();
	TUsrHit * hit;
	while (hit == (TUsrHit *) mIter->Next()) {
		TH1F * h = (TH1F *) gMrbAnalyze->GetHistoAddr(hit->GetMuxChannel());
		cout << (h ? h->GetName() : "undef") << " ";
		hit->Print();
	}
	cout << "--------------------------------------------------------------------------" << endl << endl;
}
