//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_DGF_1.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,21] - DGF data
// Description:    Implements class methods to handle [10,21] subevents
//                 reflecting data structure of XIA DGF-4C modules
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

#include "TDirectory.h"
#include "TEnv.h"

#include "TMrbTemplate.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent_DGF_1.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_DGF_1)

TMrbSubevent_DGF_1::TMrbSubevent_DGF_1(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_DGF_1
// Purpose:        Create a subevent type [10,21]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,21]
//                 used to store DGF-4C list-mode data
//
//                 Data format as given by the producer (MBS):
//                 -  data preceeded by a buffer header
//                 -  events preceeded by a event header
//                 -  variable channel data controlled by
//                    a format descriptor and a channel header
//                 -  up to 4 channels per event
//                 -  several modules per buffer
//                 -  only 1 event per module (MAXEVENTS=1)
//
//                 15               8|7                  0
//                 |=================|===================| buffer header
//                 |     number of words in buffer       |
//                 |-------------------------------------|
//                 |           module number             |
//                 |-------------------------------------|
//                 |         format descriptor           |
//                 |-------------------------------------|
//                 |       run start time (3 words)      |
//                 |=====================================|
//                 |             hit pattern             | event header
//                 |-------------------------------------|
//                 |        event time (2 words)         |
//                 |=====================================|
//                 |   number of words for this channel  | channel header
//                 |-------------------------------------|
//                 |           fast trigger time         |
//                 |-------------------------------------|
//                 |               energy                |
//                 |-------------------------------------|
//                 |            reserved (XIA)           |
//                 |-------------------------------------|
//                 |            reserved (XIA)           |
//                 |-------------------------------------|
//                 |            reserved (XIA)           |
//                 |-------------------------------------|
//                 |            reserved (XIA)           |
//                 15====================================0
//
//                 Data storage by the consumer (ROOT):
//                 -  stored in a fixed-length vector, indexed by channel number
//                 -  empty channels padded with a zero value
//                 -  therefore works with MAXEVENTS=1 only
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		if (*SevtTitle == '\0') SetTitle("CAMAC subevent [10,21]: XIA DGF-4C data, multi-module, single-event (MAXEVENTS=1)");
		fSevtType = 10; 	 						// set subevent type & subtype
		fSevtSubtype = 21;
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		gMrbConfig->AddUserClass("TMrbSubevent_DGF_1");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_DGF_1::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_DGF_1::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,21]
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code needed for subevent handling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * param;
	TMrbModule * parentModule, * module;
	TString sevtName;
	Int_t parNo;
	Int_t nextChannel = 0;
	Int_t thisChannel;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
			Template.InitializeCode("%SB%");
			Template.Substitute("$sevtNameLC", this->GetName());
			sevtName = this->GetName();
			sevtName(0,1).ToUpper();
			Template.Substitute("$sevtNameUC",sevtName.Data());
			Template.Substitute("$sevtType", (Int_t) fSevtType);
			Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
			Template.Substitute("$sevtSerial", fSerial);
			Template.Substitute("$crateNo", this->GetCrate());
			Template.WriteCode(RdoStrm);

			parentModule = NULL;
			param = (TMrbModuleChannel *) fLofParams.First();
			parNo = 0;
			while (param) {
				thisChannel = param->GetAddr();

				if (param->Parent() != parentModule) {
					parentModule = (TMrbModule *) param->Parent();
					parentModule->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
					nextChannel = thisChannel;
				}

				((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadModule);
				parNo += parentModule->GetNofChannelsUsed();
				param = (parNo <= fLofParams.GetLast()) ? (TMrbModuleChannel *) fLofParams.At(parNo) : NULL;				
			}
			module = (TMrbModule *) fLofModules.First();
			while (module) {
				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleFinishReadout);
				module = (TMrbModule *) fLofModules.After(module);
			}

			Template.InitializeCode("%SE%");
			Template.Substitute("$sevtName", this->GetName());
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			module = (TMrbModule *) fLofModules.First();
			while (module) {
				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleClearModule);
				module = (TMrbModule *) fLofModules.After(module);
			}
			break;
	}
	return(kTRUE);
}

Bool_t TMrbSubevent_DGF_1::MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_DGF_1::MakeRcFile
// Purpose:        Generate subevent-specific entries to resource file
// Arguments:      ofstream & RcStrm           -- where to output code
//                 EMrbRcFileTag TagIndex      -- tag word index
//                 Char_t * ResourceName       -- resource prefix
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates subevent-specific entries to resource file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;

	TMrbNamedX * rcFileTag;

	Char_t * fp;
	TString rcTemplateFile;
	TString templatePath;
	TString sevtNameUC;
	TString iniTag;
		
	TString tf;
	
	TMrbTemplate rcTmpl;
	
	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetRcFileOptions() & TMrbConfig::kRcOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	fp = NULL;
	tf = this->ClassName();
	tf.ReplaceAll("TMrb", "");
	tf += ".rc.code";
	fp = gSystem->Which(templatePath.Data(), tf.Data());
	if (fp == NULL) return(kTRUE);

	if (verboseMode) {
		gMrbLog->Out()  << "[" << this->GetName() << "] Using template file " << fp << endl;
		gMrbLog->Flush(this->ClassName(), "MakeRcFile");
	}
	
 	rcTemplateFile = fp;

	if (!rcTmpl.Open(rcTemplateFile, &gMrbConfig->fLofRcFileTags)) return(kFALSE);

	sevtNameUC = this->GetName();
	sevtNameUC(0,1).ToUpper();

	iniTag = (gMrbConfig->GetRcFileOptions() & TMrbConfig::kRcOptByName) ? "%NAM%" : "%NUM%";
	
	for (;;) {
		rcFileTag = rcTmpl.Next(line);
		if (rcTmpl.IsEof()) break;
		if (rcTmpl.IsError()) continue;
		if (rcTmpl.Status() & TMrbTemplate::kNoTag) continue;
		if (TagIndex == rcFileTag->GetIndex()) {
			if (!gMrbConfig->ExecUserMacro(&RcStrm, this, rcFileTag->GetName())) {
				rcTmpl.InitializeCode(iniTag.Data());
				rcTmpl.Substitute("$resource", ResourceName);
				rcTmpl.Substitute("$sevtNameLC", this->GetName());
				rcTmpl.Substitute("$sevtNameUC", sevtNameUC.Data());
				rcTmpl.Substitute("$clusterNumber", this->GetClusterNumber());
				rcTmpl.Substitute("$clusterSerial", this->GetClusterSerial());
				rcTmpl.Substitute("$clusterColor", this->GetClusterColor());
				rcTmpl.WriteCode(RcStrm);
				return(kTRUE);
			}
		}
	}
	return(kTRUE);
}
