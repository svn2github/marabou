//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFHistogramBuffer.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

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
	fESize = 0;
	fESizePerChannel = 0;
	fBSize = 0;
	fBSizePerChannel = 0;

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

void TMrbDGFHistogramBuffer::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer::Print
// Purpose:        Output buffer statistics
// Arguments:      --
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
		cout	<< "===========================================================================================" << endl
				<< " DGF-4C Histogram buffer" << endl
				<< "..........................................................................................." << endl
				<< " Number of active channels   : " << fNofChannels << endl
				<< " Size of energy region       : " << fESize << endl
				<< "             ... per channel : " << fESizePerChannel << endl
				<< " Size of baseline region     : " << fBSize << endl
				<< "             ... per channel : " << fBSizePerChannel << endl
				<< "..........................................................................................." << endl;
		for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			if (fIsActive[chn]) cout	<< " Counts in channel # " << chn << "       : " << this->GetContents(chn) << endl;
		}
		cout	<< "-------------------------------------------------------------------------------------------" << endl;
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
		k = fHistNo[Channel] * fESizePerChannel;
		for (i = 0; i < fESizePerChannel; i++, k++) sum += fArray[k];
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

	Int_t i, k;
	TH1F * h;
	TMrbString hName, hTitle;

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

	hName = "mca"; hName += Channel;
	hTitle = "MCA histogram channel "; hTitle += Channel;
	h = fHistogram[Channel];

	if (h) delete h;

	h = new TH1F(hName.Data(), hTitle.Data(), fESizePerChannel, 0., (Float_t) fESizePerChannel);

	k = fHistNo[Channel] * fESizePerChannel;
	for (i = 1; i <= fESizePerChannel; i++, k++) h->SetBinContent(i, (Stat_t) fArray[k]);
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

	Int_t i, k;
	TH1F * h;
	TMrbString hName, hTitle;
	Int_t chn, chnNo;
	Int_t nofChannels;
	Int_t nofHistos;
	TFile * mcaFile;

	if (Channel >= TMrbDGFData::kNofChannels) {
		gMrbLog->Err()	<< "Channel number out of range - " << Channel
						<< " (should be in [0," << TMrbDGFData::kNofChannels - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "Draw");
		return(kFALSE);
	}

	mcaFile = NULL;
	h = NULL;

	nofChannels = 0;
	nofHistos = 0;
	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		chnNo = (Channel == -1) ? chn : Channel;
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
				hName = "mca"; hName += chn;
				hTitle = "MCA histogram ";
				hTitle += ((TMrbDGF *) fModule)->GetName();
				hTitle += ", chn ";
				hTitle += chn;
				if (h) delete h;
				h = new TH1F(hName.Data(), hTitle.Data(), fESizePerChannel, 0., (Float_t) fESizePerChannel);
				k = fHistNo[chn] * fESizePerChannel;
				for (i = 0; i < fESizePerChannel; i++, k++) h->Fill((Axis_t) i, (Float_t) fArray[k]);
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
