//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16
// Purpose:        MARaBOU configuration: Mesytec multiplexer MUX-16
// Description:    Implements class methods to handle a Mesytec Multiplexer
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbMesytec_Mux16.cxx,v 1.1 2007-10-16 14:24:04 Rudolf.Lutter Exp $       
// Date:           $Date: 2007-10-16 14:24:04 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"
#include "TEnv.h"

#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbMesytec_Mux16.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbMesytec_Mux16)

TMrbMesytec_Mux16::TMrbMesytec_Mux16(const Char_t * MuxName, const Char_t * ModuleName, Int_t FirstChannel, Int_t NofSubmodules) :
										TMrbModule(MuxName, "Mesytec_Mux16", NofSubmodules * 16, 1 << 12) {

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16
// Purpose:        Create a Mesytec multiplexer
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Int_t NofSubModules      -- number of modules stacked together
// Results:        --
// Exceptions:
// Description:    Creates a Mesytec MUX-16
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (gMrbConfig == NULL) {
			gMrbLog->Err() << "No config defined" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->FindModule(MuxName)) {
			gMrbLog->Err() << MuxName << ": Mux name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if ((module = (TMrbModule *) gMrbConfig->FindModule(ModuleName)) == NULL) {
			gMrbLog->Err() << MuxName << ": No such module - " << ModuleName << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (FirstChannel >= module->GetNofChannels()) {
			gMrbLog->Err()	<< MuxName << ": First channel out of range - " << FirstChannel
							<< " (should be in [0," << module->GetNofChannels() << "[)" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (NofSubmodules < 1 || NofSubmodules > 8) {
			gMrbLog->Err() << MuxName << ": Max 8 submodules" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle("Mesytec MUX-16 N * 16 channels 12 bit"); 	// store module type
	 		fModule = module;
			fFirstChannel = FirstChannel;
			fNofSubmodules = NofSubmodules;
			fHistoNames.Expand(NofSubmodules * 16);
			fHistoNames.Clear();
			gMrbConfig->AddMux(this);
			gDirectory->Append(this);
		}
	}
}

const Char_t * TMrbMesytec_Mux16::GetParamNames(TString & ParamNames) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16::GetParamNames
// Purpose:        Return param names
// Arguments:      TString & ParamNames -- where to store param names
// Results:        Char_t * ParamNames  -- pointer to param names
// Exceptions:
// Description:    Defines params with endings E1, E2, P1, and P2, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ParamNames = Form("%sE1- %sE2- %sP1- %sP2-", this->GetName(), this->GetName(), this->GetName(), this->GetName());
	return(ParamNames.Data());
}

Bool_t TMrbMesytec_Mux16::SetHistoName(Int_t Channel, const Char_t * HistoName, const Char_t * HistoTitle) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16::SetHistoName
// Purpose:        Define histo names and titles
// Arguments:      Int_t Channel        -- channel to be set
//                 Char_t * HistoName   -- name
//                 Char_t * HistoTitle  -- title
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines histo names and titles.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel < 0 || Channel >= this->GetNofChannels()) {
		gMrbLog->Err() << this->GetName() << ": Channel out of range - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "SetHistoName");
		return(kFALSE);
	}
	TString hstr = HistoName;
	if (HistoTitle != NULL && *HistoTitle != '\0') hstr += Form(":%s", HistoTitle);
	fHistoNames[Channel] = new TObjString(hstr);
	return(kTRUE);
}

Bool_t TMrbMesytec_Mux16::BookHistograms() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16::BookHistograms
// Purpose:        Book MUX histos
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Books histograms -
//                      fNofSubmodules * 16 a 4K  -- channel energies
//                      P1 & P2                   -- positions
//                      Cal1 & Cal2               -- calibration
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	for (Int_t chn = 0; chn < this->GetNofChannels(); chn++) {
		TString hName, hTitle;
		TObjString * o = (TObjString *) fHistoNames[chn];
		if (o) {
			hName = o->GetString();
			Int_t idx = hName.Index(":", 0);
			if (idx >= 0) {
				hTitle = hName(idx + 1, hName.Length());
				hName = hName(0, idx);
			}
		} else {
			hName = Form("hE%s%02d", this->GetName(), chn);
			hTitle = Form("Mux %s, chn %d", this->GetName(), chn);
		}
		gMrbConfig->BookHistogram(this->GetName(), "TH1F",	hName.Data(), hTitle.Data(),
															this->GetRange(), 0, this->GetRange());
	}
	for (Int_t pos = 0; pos < 2; pos++) {
		gMrbConfig->BookHistogram(this->GetName(), "TH1F",	Form("hP%s%d", this->GetName(), pos),
															Form("Mux %s, position %d", this->GetName(), pos),
															kPosRangePerMux, 0, kPosRangePerMux);
		gMrbConfig->BookHistogram(this->GetName(), "TH1F",	Form("hL%s%d", this->GetName(), pos),
															Form("Mux %s, lookup %d", this->GetName(), pos),
															kPosRangePerMux, 0, kPosRangePerMux);
		gMrbConfig->BookHistogram(this->GetName(), "TH1F",	Form("hU%s%d", this->GetName(), pos),
															Form("Mux %s, unidentified %d", this->GetName(), pos),
															this->GetRange(), 0, this->GetRange());
	}
	return(kTRUE);
}

Bool_t TMrbMesytec_Mux16::WriteLookup(const Char_t * LkpFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16::WriteLookup
// Purpose:        Write lookup data
// Arguments:      Char_t * LkpFile    -- filename
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (Over)writes lookup files using TEnv.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TEnv * mux = new TEnv(LkpFile);
	mux->SetValue("TUsrMux.Name", this->GetName());
	mux->SetValue("TUsrMux.Module", this->GetModule()->GetName());
	mux->SetValue("TUsrMux.Serial", this->GetModuleSerial());
	mux->SetValue("TUsrMux.NofSubmodules", this->GetNofSubmodules());
	mux->SetValue("TUsrMux.NofChannels", this->GetNofChannels());
	for (Int_t i = 0; i < 2; i++) {
		mux->SetValue(Form("TUsrMux.Position.%d", i), Form("hP%s%d", this->GetName(), i));
		mux->SetValue(Form("TUsrMux.Lookup.%d", i), Form("hL%s%d", this->GetName(), i));
		mux->SetValue(Form("TUsrMux.Unidentified.%d", i), Form("hU%s%d", this->GetName(), i));
	}
	for (Int_t i = 0; i < this->GetNofChannels(); i++) {
		for (Int_t k = 0; k < 2; k++) {
			Int_t n = mux->GetValue(Form("TUsrMux.%d.Chn%d.Xmin", k, i), 0);
			mux->SetValue(Form("TUsrMux.%d.Chn%d.Xmin", k, i), n);
			n = mux->GetValue(Form("TUsrMux.%d.Chn%d.Xmax", k, i), 0);
			mux->SetValue(Form("TUsrMux.%d.Chn%d.Xmax", k, i), n);
		}
		TObjString * o = (TObjString *) fHistoNames[i];
		TString hName = o ? o->GetString() : Form("hE%s%02d", this->GetName(), i);
		mux->SetValue(Form("TUsrMux.%d.Histogram", i), hName.Data());
	}
	mux->SaveLevel(kEnvLocal);
	return(kTRUE);
}
