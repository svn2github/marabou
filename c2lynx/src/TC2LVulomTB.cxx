//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods to connect to VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TC2LVulomTB.cxx,v 1.1 2009-05-29 14:14:31 Rudolf.Lutter Exp $     
// Date:           $Date: 2009-05-29 14:14:31 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TEnv.h"
#include "TDatime.h"
#include "TROOT.h"

#include "TMrbC2Lynx.h"
#include "TC2LVulomTB.h"

#include "TMrbTemplate.h"
#include "TMrbNamedX.h"
#include "TMrbLogger.h"
#include "TMrbResource.h"
#include "SetColor.h"

#include "M2L_CommonDefs.h"

extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

ClassImp(TC2LVulomTB)

Bool_t TC2LVulomTB::ExecFunction(Int_t Fcode, TArrayI & DataSend, TArrayI & DataRecv, Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB::ExecFunction()
// Purpose:        Execute function
// Arguments:      Int_t Fcode            -- function code
//                 TArrayI & DataSend     -- data (send)
//                 TArrayI & DataRecv     -- data (receive)
//                 Int_t Index            -- channel or scaler number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes function.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsVerbose() || this->IsOffline()) {
		TMrbNamedX * f = this->FindFunction(Fcode);
		TString fn = f ? f->GetName() : "???";
		TString an = (Index == kVulomTBAllChannels) ? "all" : Form("chn%d", Index);
		cout << "[" << this->GetName() << ", " << an << "] Exec function - \""
					<< fn << "\" (0x" << setbase(16) << Fcode << setbase(10) << ")";
		for (Int_t i = 0; i < DataSend.GetSize(); i++) {
			if (i == 0) cout	<< ", data: "; else cout << ", ";
			cout	<< DataSend[i]
					<< "|0x" << setbase(16) << DataSend[i] << setbase(10);
		}
		cout << endl;
		if (this->IsOffline()) {
			DataRecv.Set(100);
			return(kTRUE);
		}
	}

	Int_t wc = DataSend.GetSize() + 1;
	M2L_VME_Exec_Function * x = (M2L_VME_Exec_Function *) gMrbC2Lynx->AllocMessage(sizeof(M2L_VME_Exec_Function), wc, kM2L_MESS_VME_EXEC_FUNCTION);
	x->fData.fData[0] = Index;
	Int_t * dp = &x->fData.fData[1];
	for (Int_t i = 0; i < DataSend.GetSize(); i++, dp++) *dp = DataSend[i];
	x->fXhdr.fHandle = this->GetHandle();
	x->fXhdr.fCode = Fcode;
	x->fData.fWc = wc;
	if (gMrbC2Lynx->Send((M2L_MsgHdr *) x)) {
		wc = DataRecv.GetSize();
		M2L_VME_Return_Results * r = (M2L_VME_Return_Results *) gMrbC2Lynx->AllocMessage(sizeof(M2L_VME_Return_Results), wc, kM2L_MESS_VME_EXEC_FUNCTION);
		if (gMrbC2Lynx->Recv((M2L_MsgHdr *) r)) {
			wc = r->fData.fWc;
			DataRecv.Set(wc);
			Int_t * dp = r->fData.fData;
			for (Int_t i = 0; i < wc; i++, dp++) DataRecv[i] = *dp;
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	} else {
		return(kFALSE);
	}
}

Bool_t TC2LVulomTB::GetModuleInfo(Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion) {
	if (this->IsOffline()) return(kTRUE);
	M2L_VME_Exec_Function x;
	gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &x, sizeof(M2L_VME_Exec_Function), kM2L_MESS_VME_EXEC_FUNCTION);
	x.fData.fData[0] = kVulomTBAllChannels;
	x.fXhdr.fHandle = this->GetHandle();
	x.fXhdr.fCode = kM2L_FCT_GET_MODULE_INFO;
	x.fData.fWc = 1;
	if (gMrbC2Lynx->Send((M2L_MsgHdr *) &x)) {
		M2L_VME_Return_Module_Info r;
		if (gMrbC2Lynx->Recv((M2L_MsgHdr *) &r)) {
			BoardId = r.fBoardId;
			MajorVersion = r.fMajorVersion;
			MinorVersion = r.fMinorVersion;
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	} else {
		return(kFALSE);
	}
}
	
Bool_t TC2LVulomTB::ReadScaler(TArrayI & ScalerValues, Int_t ScalerNo) {
	TArrayI dataSend(0);
	return(this->ExecFunction(kM2L_FCT_VULOM_TB_READ_SCALER, dataSend, ScalerValues, ScalerNo));
}
	
Bool_t TC2LVulomTB::ReadChannel(TArrayI & ScalerValues, Int_t ChannelNo) {
	TArrayI dataSend(0);
	return(this->ExecFunction(kM2L_FCT_VULOM_TB_READ_CHANNEL, dataSend, ScalerValues, ChannelNo));
}
	
Bool_t TC2LVulomTB::EnableChannel(Int_t ChannelNo) {
	TArrayI bits(1); bits[0] = 1;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_ENABLE_CHANNEL, bits, bits, ChannelNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t TC2LVulomTB::DisableChannel(Int_t ChannelNo) {
	TArrayI bits(1); bits[0] = 0;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_DISABLE_CHANNEL, bits, bits, ChannelNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t TC2LVulomTB::SetEnableMask(UInt_t & Bits) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_SET_ENABLE_MASK, bits, bits, kVulomTBAllChannels)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LVulomTB::GetEnableMask(UInt_t & Bits) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_GET_ENABLE_MASK, dataSend, bits, kVulomTBAllChannels)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LVulomTB::SetScaleDown(Int_t & ScaleDown, Int_t ChannelNo) {
	TArrayI scaleDown(1); scaleDown[0] = ScaleDown;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_SET_SCALE_DOWN, scaleDown, scaleDown, ChannelNo)) return(kFALSE);
	ScaleDown = scaleDown[0];
	return(kTRUE);
}

Bool_t TC2LVulomTB::GetScaleDown(Int_t & ScaleDown, Int_t ChannelNo) {
	TArrayI dataSend(0);
	TArrayI scaleDown;
	if (!this->ExecFunction(kM2L_FCT_VULOM_TB_GET_SCALE_DOWN, dataSend, scaleDown, ChannelNo)) return(kFALSE);
	ScaleDown = scaleDown[0];
	return(kTRUE);
}

Bool_t TC2LVulomTB::RestoreSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB::RestoreSettings
// Purpose:        Read settings from file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads env data from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();

	TString settingsFile = SettingsFile;
	gSystem->ExpandPathName(settingsFile);
	if (gSystem->AccessPathName(settingsFile.Data())) {
		gMrbLog->Err()	<< "Settings file not found - " << settingsFile << endl;
		gMrbLog->Flush("RestoreSettings");
		return(kFALSE);
	}

	TMrbResource * settings = new TMrbResource("VulomTB", settingsFile.Data());
	TString moduleName = settings->Get(".ModuleName", "");
	if (moduleName.IsNull()) {
		gMrbLog->Err()	<< "[" << settingsFile << "] Wrong format - module name missing" << endl;
		gMrbLog->Flush("RestoreSettings");
		return(kFALSE);
	}

	TString dotMod = Form(".%s", moduleName.Data());

	for (Int_t chn = 0; chn < kVulomTBNofChannels; chn++) {
		Bool_t enable = settings->Get(dotMod.Data(), "Enable", Form("%d", chn), kFALSE);
		if (enable) this->EnableChannel(chn); else this->DisableChannel(chn);
	}

	for (Int_t chn = 0; chn < kVulomTBNofChannels; chn++) {
		Int_t scd = settings->Get(dotMod.Data(), "ScaleDown", Form("%d", chn), 0);
		this->SetScaleDown(scd, chn);
	}

	return(kTRUE);
}

Bool_t TC2LVulomTB::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB::SaveSettings
// Purpose:        Write settings to file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Write env data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();

	TString settingsFile = SettingsFile;

	ofstream settings(settingsFile.Data(), ios::out);
	if (!settings.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << settingsFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings");
		return(kFALSE);
	}

	TString tmplPath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	TString tf = "Module_VulomTB.rc.code";
	gSystem->ExpandPathName(tmplPath);

	const Char_t * fp = gSystem->Which(tmplPath.Data(), tf.Data());
	if (fp == NULL) {
		gMrbLog->Err()	<< "Template file not found -" << endl
						<<           "                 Searching on path " << tmplPath << endl
						<<           "                 for file          " << tf << endl;
		gMrbLog->Flush("SaveSettings");
		return(kFALSE);
	}
	
	tf = fp;
	
	TMrbLofNamedX tags;
	tags.AddNamedX(TC2LVulomTB::kRcModuleSettings, "MODULE_SETTINGS");

	TMrbTemplate tmpl;
	TString line;
	if (tmpl.Open(tf.Data(), &tags)) {
		for (;;) {
			TMrbNamedX * tag = tmpl.Next(line);
			if (tmpl.IsEof()) break;
			if (tmpl.IsError()) continue;
			if (tmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) settings << line << endl;
			} else {
				switch (tag->GetIndex()) {
					case TC2LVulomTB::kRcModuleSettings:
					{
						TDatime d;
						tmpl.InitializeCode("%Preamble%");
						tmpl.Substitute("$author", gSystem->Getenv("USER"));
						tmpl.Substitute("$date", d.AsString());
						tmpl.Substitute("$moduleName", this->GetName());
						tmpl.WriteCode(settings);

						UInt_t enaMask;
						this->GetEnableMask(enaMask);
						UInt_t bit = 0x1;
						for (Int_t chn = 0; chn < kVulomTBNofChannels; chn++) {
							tmpl.InitializeCode("%Channels%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chn", chn);
							tmpl.Substitute("$enaFlag", (enaMask & bit) ? "TRUE" : "FALSE");
							bit <<= 1;
							Int_t scd;
							this->GetScaleDown(scd, chn);
							tmpl.Substitute("$scaleDown", scd);
							tmpl.WriteCode(settings);
						}
					}
				}
			}
		}
	}
	settings.close();
	fSettingsFile = settingsFile;
	gMrbLog->Out()  << "[" << this->GetName() << "] Settings saved to file " << settingsFile << endl;
	gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
	return(kTRUE);
}
