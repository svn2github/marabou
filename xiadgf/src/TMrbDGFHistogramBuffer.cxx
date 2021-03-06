//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFHistogramBuffer.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGFHistogramBuffer.cxx,v 1.9 2006-07-19 09:08:58 Rudolf.Lutter Exp $       
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
#include "TMrbDGFHistogramBuffer.h"

#include "SetColor.h"

extern TMrbDGFData * gMrbDGFdata;				// common data base for all DGF modules
extern TMrbLogger * gMrbLog;

ClassImp(TMrbDGFHistogramBuffer)

TMrbDGFHistogramBuffer::TMrbDGFHistogramBuffer(const Char_t * BufferName, TObject * Module) :
																	TNamed(BufferName, "DGF-4C Histogram Buffer") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer
// Purpose:        DGF's histogram buffer
// Description:    Describes the histogram buffer.
// Keywords:       |
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fModule = Module;
	this->Reset();
}

void TMrbDGFHistogramBuffer::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::Reset
// Purpose:        Reset to initial values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets event buffer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;

	fNofChannels = 0;
	fSize = 0;
	fSizePerChannel = 0;

	for (i = 0; i < TMrbDGFData::kNofChannels; i++) {
		fIsActive[i] = kFALSE;
		fHistNo[i] = -1;
		fHistogram[i] = NULL;
	}
	this->Set(0);			// reset data
}

Bool_t TMrbDGFHistogramBuffer::SetActive(Int_t Channel, Int_t HistNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::SetActive
// Purpose:        Mark channel active
// Arguments:      Int_t Channel          -- channel number
//                 Int_t HistNo           -- histogram number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets active flag for given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetActive");
		return(kFALSE);
	}
	fIsActive[Channel] = kTRUE;
	fHistNo[Channel] = HistNo;
	return(kTRUE);
}

Bool_t TMrbDGFHistogramBuffer::IsActive(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::IsActive
// Purpose:        Test if channel active
// Arguments:      Int_t Channel         -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "IsActive");
		return(kFALSE);
	}
	return(fIsActive[Channel]);
}

void TMrbDGFHistogramBuffer::Print(ostream & OutStrm) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::Print
// Purpose:        Output buffer statistics
// Arguments:      ostream & Outstrm   -- output stream (defaults to cout)
// Results:        --
// Exceptions:
// Description:    Buffer printout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;

	if (fNofChannels == 0) {
		gMrbLog->Err() << "Buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
	} else {
		OutStrm	<< "===========================================================================================" << endl
				<< " DGF-4C Histogram buffer" << endl
				<< "..........................................................................................." << endl
				<< " Number of active channels   : " << fNofChannels << endl
				<< " Size of energy region       : " << fSize << endl
				<< "             ... per channel : " << fSizePerChannel << endl
				<< "..........................................................................................." << endl;
		for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			if (fIsActive[chn]) OutStrm	<< " Counts in channel # " << chn << "       : " << this->GetContents(chn) << endl;
		}
		OutStrm	<< "-------------------------------------------------------------------------------------------" << endl;
	}
}

Int_t TMrbDGFHistogramBuffer::GetContents(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::GetContents
// Purpose:        Get channel contents
// Arguments:      Int_t Channel   -- DGF channel
// Results:        Int_t Contents  -- channel contents
// Exceptions:
// Description:    Returns histogram contents for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sum;
	Int_t i, k;

	sum = 0;
	if (fIsActive[Channel]) {
		k = fHistNo[Channel] * fSizePerChannel;
		for (i = 0; i < fSizePerChannel; i++, k++) sum += fArray[k];
	}
	return(sum);
}

Bool_t TMrbDGFHistogramBuffer::FillHistogram(Int_t Channel, Bool_t DrawIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::FillHistogram
// Purpose:        Fill and display histogram
// Arguments:      Int_t Channel   -- DGF channel
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Fills and draws a MCA histogram for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "FillHistogram");
		return(kFALSE);
	}

	if (!IsActive(Channel)) {
		gMrbLog->Err() << "Channel not active - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "FillHistogram");
		return(kFALSE);
	}

	TMrbString hName = fModule->GetName();
	hName += "_mca_chn"; hName += Channel;
	TMrbString hTitle = fModule->GetName();
	hTitle += ": MCA histogram chn "; hTitle += Channel;
	TH1F * h = fHistogram[Channel];

	if (h) delete h;

	h = new TH1F(hName.Data(), hTitle.Data(), fSizePerChannel, 0., (Float_t) fSizePerChannel);

	Int_t k = fHistNo[Channel] * fSizePerChannel;
	for (Int_t i = 1; i <= fSizePerChannel; i++, k++) h->SetBinContent(i, (Stat_t) fArray[k]);
	h->SetEntries(h->Integral());
	if (DrawIt) h->Draw();
	fHistogram[Channel] = h;
	return(kTRUE);
}

Bool_t TMrbDGFHistogramBuffer::Save(const Char_t * McaFile, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::Save
// Purpose:        Save histograms to root file
// Arguments:      Char_t * McaFile   -- file name
//                 Int_t Channel      -- DGF channel
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Saves histograms to root file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "Draw");
		return(kFALSE);
	}

	TFile * mcaFile = NULL;
	TH1F * h = NULL;

	Int_t nofChannels = 0;
	Int_t nofHistos = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		Int_t chnNo = (Channel == -1) ? chn : Channel;
		if (this->IsActive(chn) && (chnNo == chn)) {
			nofChannels++;
			if (this->GetContents(chn) > 0) {
				if (mcaFile == NULL) {
					mcaFile = new TFile(McaFile, "RECREATE");
					if (!mcaFile->IsOpen()) {
						gMrbLog->Err() << "Can't open file " << McaFile << endl;
						gMrbLog->Flush(this->ClassName(), "Save");
						return(kFALSE);
					}
				}
				TMrbString hName = fModule->GetName();
				hName += "_mca_chn"; hName += chn;
				TMrbString hTitle = "MCA histogram ";
				hTitle += ((TMrbDGF *) fModule)->GetName();
				hTitle += ", chn ";
				hTitle += chn;
				if (h) delete h;
				h = new TH1F(hName.Data(), hTitle.Data(), fSizePerChannel, 0., (Float_t) fSizePerChannel);
				Int_t k = fHistNo[chn] * fSizePerChannel;
				for (Int_t i = 0; i < fSizePerChannel; i++, k++) h->Fill((Axis_t) i, (Float_t) fArray[k]);
				h->Write();
				nofHistos++;
			} else {
				gMrbLog->Err() << "Histogram for channel " << chn << " is empty" << endl;
				gMrbLog->Flush(this->ClassName(), "Save");
			}
			if (Channel != -1) break;
		}
	}
	if (mcaFile) mcaFile->Close();

	if (nofChannels == 0) {
		gMrbLog->Err() << "No active channels found" << endl;
		gMrbLog->Flush(this->ClassName(), "Save");
		return(kFALSE);
	} else if (nofHistos == 0) {
		gMrbLog->Err() << "No histograms saved" << endl;
		gMrbLog->Flush(this->ClassName(), "Save");
		return(kFALSE);
	} else {
		gMrbLog->Out() << nofChannels << " MCA channel(s) saved to " << McaFile	<< endl;
		gMrbLog->Flush(this->ClassName(), "Save()", setblue);
		return(kTRUE);
	}
}

TH1F * TMrbDGFHistogramBuffer::Histogram(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::Histogram
// Purpose:        Return histo addr for given channel
// Arguments:      Int_t Channel     -- DGF channel
// Results:        TH1F * HistAddr   -- histo addr
// Exceptions:
// Description:    Rerutns histo addr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "Histogram");
		return(NULL);
	}

	if (!IsActive(Channel)) {
		gMrbLog->Err() << "Channel not active - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Histogram");
		return(NULL);
	}
	return(fHistogram[Channel]);
}
