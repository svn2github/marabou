//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbEvent.cxx
// Purpose:        MARaBOU configuration: event definition
// Description:    Implements class methods to define events/triggers
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

#include "TEnv.h"
#include "TDirectory.h"

#include "TMrbSystem.h"
#include "TMrbString.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbEvent.h"
#include "TMrbSubevent.h"
#include "TMrbModule.h"
#include "TMrbCamacScaler.h"
#include "TMrbVMEScaler.h"

#include "SetColor.h"

const SMrbNamedXShort kMrbEvtReadoutOptions[] =
							{
								{0, NULL}
							};

const SMrbNamedXShort kMrbEvtAnalyzeOptions[] =
							{
								{TMrbConfig::kAnaOptSubevents,		"SUBEVENTS" 		},
								{~TMrbConfig::kAnaOptSubevents, 	"NOSUBEVENTS"		},
								{TMrbConfig::kAnaOptHistograms, 	"HISTOGRAMS"		},
								{~TMrbConfig::kAnaOptHistograms,	"NOHISTOGRAMS"		},
								{TMrbConfig::kAnaOptEvtDefault, 	"DEFAULT"			},
								{0, 								NULL				}
							};

const SMrbNamedXShort kMrbEvtConfigOptions[] =
							{
								{0, NULL}
							};

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbEvent)

TMrbEvent::TMrbEvent(Int_t Trigger, const Char_t * EvtName, const Char_t * EvtTitle) : TNamed(EvtName, EvtTitle) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent
// Purpose:        Create a event
// Arguments:      Int_t Trigger           -- associated trigger number
//                 Char_t * EvtName        -- event name
//                 Char_t * EvtTitle       -- ... and title
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;
	TMrbConfig::EMrbTriggerStatus trigStat;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (gMrbConfig == NULL) {
		gMrbLog->Err() << "No config defined" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	 } else if (gMrbConfig->NameNotLegal("event", EvtName)) {
		this->MakeZombie();
	}

	fLofSubevents.Delete();

	trigStat = gMrbConfig->GetTriggerStatus(Trigger);

	if (trigStat == TMrbConfig::kTriggerOutOfRange) {
		gMrbLog->Err() << "Trigger out of range - " << Trigger << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	}

	if (trigStat == TMrbConfig::kTriggerReserved) {
		gMrbLog->Err() << "Illegal trigger assignment - " << Trigger << " is reserved" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	}

	if (trigStat == TMrbConfig::kTriggerAssigned) {
		evt = (TMrbEvent *) gMrbConfig->FindEvent(Trigger);
		gMrbLog->Err() << "Trigger \"" << Trigger << "\" already assigned to event \"" << evt->GetName() << "\"" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	}

	if (!this->IsZombie()) {
		if (*EvtName == '\0') {
			TMrbString evtName("Trig", Trigger);
			this->SetName(evtName.Data());
		}

		if (*EvtTitle == '\0') {
			TMrbString evtTitle("Event associated with trigger ", Trigger);
			this->SetTitle(evtTitle.Data());
		}

		fLofSubevents.SetObject("Subevents", "List of subevents");
		fNofSubevents = 0;
		
		gMrbConfig->UpdateTriggerTable(Trigger);

		fTrigger = Trigger;
		fTriggerStatus = gMrbConfig->GetTriggerStatus(Trigger);

		fReadoutOptions = TMrbConfig::kNoOptionSpecified;
		fAnalyzeOptions = TMrbConfig::kNoOptionSpecified;
		fConfigOptions = TMrbConfig::kNoOptionSpecified;

		fAutoSave = TMrbConfig::kAutoSave;
		
		fSizeOfHitBuffer = 1000;
		fHBHighWaterLimit = 0;
		fHBOffset = 0;

		gMrbConfig->AddEvent(this);
		gDirectory->Append(this);
	}
}

TObject * TMrbEvent::FindSubeventByCrate(Int_t Crate, TObject * After) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::FindSubeventByCrate
// Purpose:        Find a subevent with given crate number
// Arguments:      Int_t Crate           -- crate number
//                 TObject * After       -- start search after obj
// Results:        TObject * Subevent    -- subevent
// Exceptions:
// Description:    Searches for next subevent in crate "Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * sevt;

	sevt = (After == NULL) ? fLofSubevents.First() : fLofSubevents.After(After);
	while (sevt) {
		if (((TMrbSubevent *) sevt)->GetCrate() == Crate) return(sevt);
		sevt = fLofSubevents.After(sevt);
	}
	return(NULL);
}

Bool_t TMrbEvent::HasSubevent(const Char_t * Assignment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::HasSubevent
// Purpose:        Assign subevent(s)
// Arguments:      Char_t * Assignment  -- subevent name(s)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a subevent to current event. More than one subevent
//                 may be assigned separated by space.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;
	TString sevtName;

	istrstream assign(Assignment);

	for (;;) {
		assign >> sevtName;
		if (sevtName.Length() <= 0) break;
		if ((sevt = (TMrbSubevent *) gMrbConfig->FindSubevent(sevtName)) == NULL) {
			gMrbLog->Err() << "Event " << this->GetName() << ": No such subevent - " << sevtName << endl;
			gMrbLog->Flush(this->ClassName(), "HasSubevent");
			continue;
		}
		if (fLofSubevents.FindObject(sevtName) != NULL) {
			gMrbLog->Err() << "Event " << this->GetName() << ": Subevent already assigned - " << sevtName << endl;
			gMrbLog->Flush(this->ClassName(), "HasSubevent");
			continue;
		}
		fLofSubevents.Add(sevt);		// insert objects vice versa
		fNofSubevents++;
		sevt->AddEvent(this);
	}
	return(kTRUE);
}

Bool_t TMrbEvent::ShareSubevents(TMrbEvent * DestEvent) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::ShareSubevents
// Purpose:        Share subevent(s) with another event
// Arguments:      TMrbEvent * DestEvent   -- destination
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Assigns subevents to DestEvent. Used to generate
//                 multi-trigger events from their single-bit ancestors.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;

	sevt = (TMrbSubevent *) fLofSubevents.First();
	while (sevt) {
		if (DestEvent->FindSubevent(sevt->GetName()) == NULL) DestEvent->HasSubevent(sevt->GetName());
		sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
	}
	return(kTRUE);
}

Bool_t TMrbEvent::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::MakeReadoutCode
// Purpose:        Generate user-defined code to read MBS data
// Arguments:      ofstream & RdoStrm        -- where to output code
//                 EMrbReadoutTag TagIndex   -- tag word index
//                 TMrbTemplate Template     -- template
//                 Char_t * Prefix           -- prefix to select template code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates code for specified event (=trigger).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;
	TString trigNameUC;
	Bool_t wtstmpFlag;
	Int_t crate;
	TMrbNamedX * cType;

	TMrbModule * dtSca;
	TString dtFct;

	trigNameUC = this->GetName();
	trigNameUC(0,1).ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kRdoDefineTriggers:
			Template.InitializeCode();
			Template.Substitute("$trigName", trigNameUC);
			Template.Substitute("$trigNo", this->GetTrigger());
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoOnTriggerXX:
			Template.InitializeCode("%B%");
			Template.Substitute("$trigName", trigNameUC);
			Template.Substitute("$trigNo", this->GetTrigger());
			Template.WriteCode(RdoStrm);
			wtstmpFlag = kFALSE;
			crate = gMrbConfig->FindCrate();
			while (crate >= 0) {
				Template.InitializeCode("%CB%");
				Template.Substitute("$crateNo", crate);
				cType = gMrbConfig->fLofCrateTypes.FindByIndex(gMrbConfig->GetCrateType(crate));
				Template.Substitute("$crateType", cType->GetName());
				Template.WriteCode(RdoStrm);
				if (!wtstmpFlag && gMrbConfig->DeadTimeToBeWritten()) {
					Template.InitializeCode("%CD%");
					dtSca = (TMrbModule *) gMrbConfig->GetDeadTimeScaler();
					Template.Substitute("$dtScaler", dtSca->GetName());
					Template.Substitute("$dtInterval", gMrbConfig->GetDeadTimeInterval());
					dtFct = (dtSca->GetType()->GetIndex() & TMrbConfig::kModuleCamac) ?
										((TMrbCamacScaler *) dtSca)->GetFunctionName(TMrbConfig::kScalerFctDeadTime) :
										((TMrbVMEScaler *) dtSca)->GetFunctionName(TMrbConfig::kScalerFctDeadTime);
					Template.Substitute("$scaRead", dtFct.Data());
					Template.WriteCode(RdoStrm);
					if (gMrbConfig->TimeStampToBeWritten()) {
						Template.InitializeCode("%CDT%");
						Template.WriteCode(RdoStrm);
					}
					wtstmpFlag = kTRUE;
				}
				if (!wtstmpFlag && gMrbConfig->TimeStampToBeWritten()) {
					Template.InitializeCode("%CT%");
					Template.WriteCode(RdoStrm);
				}
				wtstmpFlag = kTRUE;
				sevt = (TMrbSubevent *) this->FindSubeventByCrate(crate);
				while (sevt) {
					sevt->MakeReadoutCode(RdoStrm, TagIndex, Template, "%S%");
					sevt = (TMrbSubevent *) this->FindSubeventByCrate(crate, sevt);
				}
				Template.InitializeCode("%CE%");
				Template.WriteCode(RdoStrm);
				crate = gMrbConfig->FindCrate(crate);
			}
			Template.InitializeCode("%E%");
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			sevt = (TMrbSubevent *) fLofSubevents.First();
			while (sevt) {
				sevt->MakeReadoutCode(RdoStrm, TagIndex, Template, "%S%");
				sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
			}
			break;
	}
	return(kTRUE);
}

Bool_t TMrbEvent::MakeAnalyzeCode(ofstream & ana, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::MakeAnalyzeCode
// Purpose:        Generate user-defined code to analyze MBS data
// Arguments:      ofstream & ana              -- where to output code
//                 EMrbAnalyzeTag TagIndex     -- tag word index
//                 Char_t * Extension          -- file extension (.h or .cxx)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates code for specified event (=trigger).
//                 Searches for following files:
//                      (1)  if event has private code
//                      (1a)    Event_<Name>.<Extension>.code
//                      (1b)    <PrivateCodeFile>.<Extension>.code
//                      (1c)    Event_<Class>.<Extension>.code
//                      (2)  standard:
//                              Event.<Extension>.code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;
	TString evtNameUC;
	TString evtNameLC;
	TString sevtNameLC;
	TString sevtNameUC;
	TMrbString evtAutoSave;
	
	TMrbNamedX * analyzeTag;
	TMrbConfig::EMrbAnalyzeTag tagIdx;

	const Char_t * fp;
	TString anaTemplateFile;
	TString templatePath, srcPath;

	TString tf, tf1, tf2, tf3, tf4;
	const Char_t * pcf;

	TMrbSubevent * sevt;
	Bool_t foundSevt;
	Bool_t writeCmt;
	
	TMrbNamedX * icl;
	TString iclFile, iclPath;

	TMrbTemplate anaTmpl;
	
	Bool_t foundEventTimeBuffer;

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	srcPath = gEnv->GetValue("TMrbConfig.SrcPath", gSystem->WorkingDirectory());
	gSystem->ExpandPathName(srcPath);
	
	evtNameLC = this->GetName();
	evtNameUC = evtNameLC;
	evtNameUC(0,1).ToUpper();

	fp = NULL;
	if (this->HasPrivateCode()) {
		tf = "Event_";
		tf += evtNameUC;
		tf += Extension;
		tf += ".code";
		tf1 = tf;
		fp = gSystem->Which(templatePath.Data(), tf.Data());
		if (fp == NULL) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += Extension;
				tf += ".code";
				tf2 = tf;
				fp = gSystem->Which(templatePath.Data(), tf.Data());
			}
			if (fp == NULL) {
				tf = this->ClassName();
				tf += Extension;
				tf += ".code";
				tf.ReplaceAll("TMrb", "");
				tf3 = tf;
				fp = gSystem->Which(templatePath.Data(), tf.Data());
			}
		}
	}
	if (fp == NULL) {
		tf = "Event";
		tf += Extension;
		tf += ".code";
		tf3 = tf;
		fp = gSystem->Which(templatePath.Data(), tf.Data());
	}
	if (fp == NULL) {
		gMrbLog->Err()	<< "Template file not found -" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		gMrbLog->Err()	<< "          Searching on path " << templatePath << endl;
		gMrbLog->Flush();
		if (this->HasPrivateCode()) {
			gMrbLog->Err()	<< "          for file " << tf1 << endl;
			gMrbLog->Flush();
			if (tf2) {
				gMrbLog->Err()	<< "          or       " << tf2 << endl;
				gMrbLog->Flush();
			}
			gMrbLog->Err()	<< "          or       " << tf3 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "          or       " << tf4 << endl;
			gMrbLog->Flush();
		} else {
			gMrbLog->Err()	<< "          for file " << tf4 << endl;
			gMrbLog->Flush();
		}
		return(kFALSE);
	}

	if (verboseMode) {
		gMrbLog->Out()  << "[" << evtNameLC << "] Using template file " << fp << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}
	
 	anaTemplateFile = fp;

	if (!anaTmpl.Open(anaTemplateFile, &gMrbConfig->fLofAnalyzeTags)) return(kFALSE);

	for (;;) {
		analyzeTag = anaTmpl.Next(line);
		if (anaTmpl.IsEof()) break;
		if (anaTmpl.IsError()) continue;
		if (anaTmpl.Status() & TMrbTemplate::kNoTag) {
			if (line.Index("//-") != 0) ana << line.Data() << endl;
		} else if (gMrbConfig->ExecUserMacro(&ana, this, analyzeTag->GetName())) {
			continue;
		} else {
			switch (tagIdx = (TMrbConfig::EMrbAnalyzeTag) analyzeTag->GetIndex()) {
				case TMrbConfig::kAnaEventNameLC:
					ana << anaTmpl.Encode(line, evtNameLC) << endl;
					break;
				case TMrbConfig::kAnaEventNameUC:
					ana << anaTmpl.Encode(line, evtNameUC) << endl;
					break;
				case TMrbConfig::kAnaEventTitle:
					ana << anaTmpl.Encode(line, this->GetTitle()) << endl;
					break;
				case TMrbConfig::kAnaEventAutoSave:
					evtAutoSave = this->GetAutoSave();
					ana << anaTmpl.Encode(line, evtAutoSave.Data()) << endl;
					break;
				case TMrbConfig::kAnaEventFirstSubevent:
					sevt = (TMrbSubevent *) fLofSubevents.First();
					if (sevt) {
						ana << anaTmpl.Encode(line, sevt->GetName()) << endl;
					} else {
						gMrbLog->Err() << "Event has no subevents - " << this->GetName() << endl;
						gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
						ana << "??" << line << "  // event \"" << this->GetName() << "\" has no subevents" << endl;
					}
					break;
				case TMrbConfig::kAnaEventTrigger:
					{
						TMrbString trigName(this->GetTrigger());
						ana << anaTmpl.Encode(line, trigName) << endl;
					}
					break;
				case TMrbConfig::kAnaEventBookParams:
					if (this->GetTriggerStatus() != TMrbConfig::kTriggerPattern) {
						sevt = (TMrbSubevent *) fLofSubevents.First();
						while (sevt) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode();
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
							sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
						}
					}
					break;
				case TMrbConfig::kAnaEventBookHistograms:
					if ((this->GetAnalyzeOptions() & TMrbConfig::kAnaOptHistograms)
					&&  (this->GetTriggerStatus() != TMrbConfig::kTriggerPattern)) {
						sevt = (TMrbSubevent *) fLofSubevents.First();
						while (sevt) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							if (this->HasPrivateHistograms())	anaTmpl.InitializeCode("%P%");
							else								anaTmpl.InitializeCode("%N%");
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
							sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
						}
					}
					break;
				case TMrbConfig::kAnaEventAllocHitBuffer:
					anaTmpl.InitializeCode("%H%");
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$nofEntries", this->GetSizeOfHitBuffer());
					anaTmpl.Substitute("$highWater", this->GetHBHighWaterLimit());
					anaTmpl.Substitute("$offset", (Int_t) this->GetHBOffset());
					anaTmpl.WriteCode(ana);
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						if (sevt->NeedsHitBuffer()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
						}
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					foundEventTimeBuffer = kFALSE;
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						if (sevt->ProvidesEventTime()) {
							if (foundEventTimeBuffer) {
								gMrbLog->Err() << "More than 1 subevent provides event times - " << sevt->GetName() << endl;
								gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
							}
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%PEV%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
							foundEventTimeBuffer = kTRUE;
						}
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						if (sevt->NeedsEventTime()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%NEV%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
							if (!foundEventTimeBuffer) {
								gMrbLog->Err() << "There is no module/subevent to provide event times - needed by subevent " << sevt->GetName() << endl;
								gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
							}
						}
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					break;
				case TMrbConfig::kAnaEventAnalyze:
					anaTmpl.InitializeCode("%B%");
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$evtTrigger", this->GetTrigger());
					anaTmpl.WriteCode(ana);
					if (this->GetAnalyzeOptions() & TMrbConfig::kAnaOptHistograms) {
						sevt = (TMrbSubevent *) fLofSubevents.First();
						while (sevt) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							if (this->HasPrivateHistograms())	anaTmpl.InitializeCode("%P%");
							else								anaTmpl.InitializeCode("%N%");
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
							sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
						}
					}
					anaTmpl.InitializeCode("%C%");
					anaTmpl.WriteCode(ana);
					if (gMrbConfig->UserCodeToBeIncluded()) {
						icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->First();
						while (icl) {
							iclFile = icl->GetName();
							if (iclFile.Length() == 0) iclFile = gMrbConfig->GetName();
							iclFile(0,1).ToUpper();
							if (icl->GetIndex() & TMrbConfig::kIclOptByEventName) iclFile += evtNameUC.Data();
							if (icl->GetIndex() & TMrbConfig::kIclOptAnalyze) {
								iclFile += "Analyze.udc.cxx";
								anaTmpl.InitializeCode("%IUC%");
								iclPath = icl->GetTitle();
								if (iclPath.Length() == 0) iclPath = srcPath;
								fp = gSystem->Which(iclPath.Data(), iclFile.Data());
								if (fp) {
									TMrbSystem ux;
									iclPath = fp;
									fp = ux.GetRelPath(iclPath, gSystem->WorkingDirectory());
									anaTmpl.Substitute("$iclFile", fp);
								} else {
									anaTmpl.Substitute("$iclFile", iclFile);
								}
								anaTmpl.WriteCode(ana);
							}
							icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->After(icl);
						}
					}
					anaTmpl.InitializeCode("%E%");
					anaTmpl.WriteCode(ana);
					break;
				case TMrbConfig::kAnaEventAddBranches:
				case TMrbConfig::kAnaEventInitializeBranches:
				case TMrbConfig::kAnaEvtResetData:
				case TMrbConfig::kAnaSevtClassInstance:
				case TMrbConfig::kAnaSevtSetName:
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						sevtNameLC = sevt->GetName();
						sevtNameUC = sevtNameLC;
						sevtNameUC(0,1).ToUpper();
						anaTmpl.InitializeCode();
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.WriteCode(ana);
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					break;
				case TMrbConfig::kAnaSevtDispatchOverType:
					foundSevt = kFALSE;
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						if (!sevt->SerialToBeCreated()) {
							if (!foundSevt) {
								anaTmpl.InitializeCode("%B%");
								anaTmpl.WriteCode(ana);
								foundSevt = kTRUE;
							}
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
							anaTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
							anaTmpl.Substitute("$sevtWC", sevt->GetNofParams());
							anaTmpl.Substitute("$rawMode", sevt->IsRaw() ? "kTRUE" : "kFALSE");
							anaTmpl.WriteCode(ana);
						}
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					if (foundSevt) {
						anaTmpl.InitializeCode("%E%");
						anaTmpl.WriteCode(ana);
					}
					break;
				case TMrbConfig::kAnaSevtDispatchOverSerial:
					sevt = (TMrbSubevent *) fLofSubevents.First();
					while (sevt) {
						if (sevt->SerialToBeCreated()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode();
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
							anaTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
							anaTmpl.Substitute("$sevtWC", sevt->GetNofParams());
							anaTmpl.Substitute("$rawMode", sevt->IsRaw() ? "kTRUE" : "kFALSE");
							anaTmpl.WriteCode(ana);
						}
						sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
					}
					break;
				case TMrbConfig::kAnaUserBookHistograms:
					if (!gMrbConfig->UserCodeToBeIncluded()) break;
					icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->First();
					writeCmt = kTRUE;
					while (icl) {
						iclFile = icl->GetName();
						if (iclFile.Length() == 0) iclFile = gMrbConfig->GetName();
						iclFile(0,1).ToUpper();
						if (icl->GetIndex() & TMrbConfig::kIclOptByEventName) iclFile += evtNameUC.Data();
						if (icl->GetIndex() & TMrbConfig::kIclOptBookHistograms) {
							iclFile += "BookHistograms.udc.cxx";
							if (writeCmt) {
								anaTmpl.InitializeCode("%B%");
								anaTmpl.WriteCode(ana);
							}
							writeCmt = kFALSE;
							anaTmpl.InitializeCode("%I%");
							iclPath = icl->GetTitle();
							if (iclPath.Length() > 0) iclPath += "/";
							anaTmpl.Substitute("$iclPath", iclPath);
							anaTmpl.Substitute("$iclFile", iclFile);
							anaTmpl.WriteCode(ana);
						}
						icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->After(icl);
					}
					break;
				case TMrbConfig::kAnaUserBookParams:
					if (!gMrbConfig->UserCodeToBeIncluded()) break;
					icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->First();
					writeCmt = kTRUE;
					while (icl) {
						iclFile = icl->GetName();
						if (iclFile.Length() == 0) iclFile = gMrbConfig->GetName();
						iclFile(0,1).ToUpper();
						if (icl->GetIndex() & TMrbConfig::kIclOptByEventName) iclFile += evtNameUC.Data();
						if (icl->GetIndex() & TMrbConfig::kIclOptBookParams) {
							iclFile += "BookParams.udc.cxx";
							if (writeCmt) {
								anaTmpl.InitializeCode("%B%");
								anaTmpl.WriteCode(ana);
							}
							writeCmt = kFALSE;
							anaTmpl.InitializeCode("%I%");
							iclPath = icl->GetTitle();
							if (iclPath.Length() > 0) iclPath += "/";
							anaTmpl.Substitute("$iclPath", iclPath);
							anaTmpl.Substitute("$iclFile", iclFile);
							anaTmpl.WriteCode(ana);
						}
						icl = (TMrbNamedX *) gMrbConfig->GetLofUserIncludes()->After(icl);
					}
					break;
			}
		}
	}
	return(kTRUE);
}

Bool_t TMrbEvent::MakeAnalyzeCode(ofstream & ana, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::MakeAnalyzeCode
// Purpose:        Generate user-defined code to analyze MBS data
// Arguments:      ofstream & ana            -- where to output code
//                 EMrbAnalyzeTag TagIndex   -- tag word index
//                 TMrbTemplate Template     -- template
//                 Char_t * Prefix           -- prefix to select template code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates code for specified event (=trigger).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString treeName;
	
	TMrbSubevent * sevt;

	treeName = this->GetName();

	switch (TagIndex) {
		case TMrbConfig::kAnaEventSetBranchStatus:
			Template.InitializeCode("%B%");
			Template.Substitute("$treeName", treeName);
			Template.WriteCode(ana);
			sevt = (TMrbSubevent *) fLofSubevents.First();
			while (sevt) {
				sevt->MakeAnalyzeCode(ana, TagIndex, this, Template, "%S%");
				sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
			}
			Template.InitializeCode("%E%");
			Template.Substitute("$treeName", treeName);
			Template.WriteCode(ana);
			break;
	}
	return(kTRUE);
}

Bool_t TMrbEvent::MakeConfigCode(ofstream & CfgStrm, TMrbConfig::EMrbConfigTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::MakeConfigCode
// Purpose:        Generate config code
// Arguments:      ofstream & CfgStrm        -- where to output code
//                 EMrbConfigTag TagIndex    -- tag word index
//                 TMrbTemplate Template     -- template
//                 Char_t * Prefix           -- prefix to select template code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs config code for specified event (=trigger).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;

	if (TagIndex == TMrbConfig::kCfgConnectToEvent) {
		sevt = (TMrbSubevent *) fLofSubevents.First();
		while (sevt) {
			Template.InitializeCode();
			Template.Substitute("$sevtNameLC", sevt->GetName());
			Template.Substitute("$evtNameLC", this->GetName());
			Template.Substitute("$evtTitle", this->GetTitle());
			Template.WriteCode(CfgStrm);
			sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
		}
	}
	return(kTRUE);
}

Bool_t TMrbEvent::SetReadoutOptions(const Char_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::SetReadoutOptions
// Purpose:        Define some options to be used in MakeReadoutCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets readout options for this event individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX readoutOptions;

	opt = readoutOptions.CheckPatternShort(this->ClassName(), "SetReadoutOptions", Options, kMrbEvtReadoutOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fReadoutOptions = opt;
	return(kTRUE);
}

Bool_t TMrbEvent::SetAnalyzeOptions(const Char_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::SetAnalyzeOptions
// Purpose:        Define some options to be used in MakeAnalyzeCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets analyze options for this event individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX analyzeOptions;

	opt = analyzeOptions.CheckPatternShort(this->ClassName(), "SetAnalyzeOptions", Options, kMrbEvtAnalyzeOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fAnalyzeOptions = opt;
	return(kTRUE);
}

Bool_t TMrbEvent::SetConfigOptions(const Char_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::SetConfigOptions
// Purpose:        Define some options to be used in MakeConfigCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets config options for this event individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX configOptions;

	opt = configOptions.CheckPatternShort(this->ClassName(), "SetConfigOptions", Options, kMrbEvtConfigOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fConfigOptions = opt;
	return(kTRUE);
}

UInt_t TMrbEvent::GetReadoutOptions() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::GetReadoutOptions
// Purpose:        Return current options for method MakeReadoutCode()
// Arguments:      --
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns readout options:
//                     local options if fReadoutOptions is set
//                     global options from TMrbConfig otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fReadoutOptions == TMrbConfig::kNoOptionSpecified) return(gMrbConfig->GetReadoutOptions());
	return(fReadoutOptions);
}

UInt_t TMrbEvent::GetAnalyzeOptions() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::GetAnalyzeOptions
// Purpose:        Return current options for method MakeAnalyzeCode()
// Arguments:      --
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns analyze options:
//                     local options if fAnalyzeOptions is set
//                     global options from TMrbConfig otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fAnalyzeOptions == TMrbConfig::kNoOptionSpecified) return(gMrbConfig->GetAnalyzeOptions());
	return(fAnalyzeOptions);
}

UInt_t TMrbEvent::GetConfigOptions() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::GetConfigOptions
// Purpose:        Return current options for method MakeConfigCode()
// Arguments:      --
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns config options:
//                     local options if fConfigOptions is set
//                     global options from TMrbConfig otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fConfigOptions == TMrbConfig::kNoOptionSpecified) return(gMrbConfig->GetConfigOptions());
	return(fConfigOptions);
}
void TMrbEvent::Print(ostream & OutStrm, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::Print
// Purpose:        Output current event definitions
// Arguments:      ofstream & Out   -- output stream
//                 Char_t * Prefix  -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current event definition to OutStrm.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;
	TString prefix;
		
	OutStrm << Prefix << "|\n";
	OutStrm << Prefix << "+-> Event Definition:" << endl;
	OutStrm << Prefix << "       Name          : " << this->GetName() << endl;
	OutStrm << Prefix << "       Title         : " << this->GetTitle() << endl;
	OutStrm << Prefix << "       Type/Subtype  : [" << fEventType << "," << fEventSubtype << "]" << endl;
	OutStrm << Prefix << "       Trigger       : " << fTrigger << endl;
	OutStrm << Prefix << "       Subevents     : " << fNofSubevents << endl;
	if (fNofSubevents > 0) {
		prefix = Prefix;
		prefix += "       ";
		sevt = (TMrbSubevent *) fLofSubevents.First();
		while (sevt) {
			sevt->Print(OutStrm, prefix.Data());
			sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
		}
	}
}

const Char_t * TMrbEvent::GetLofSubeventsAsString(TString & LofSubevents) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::GetLofSubeventsAsString
// Purpose:        Return a colon-separated string with subevent names
// Arguments:      TString & LofSubevents    -- where to store subevent names
// Results:        Char_t * LofSubevents     -- string containing subevent names
// Exceptions:
// Description:    Returns a colon-separated string with subevent names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;

	LofSubevents = "";
	if (fNofSubevents > 0) {
		sevt = (TMrbSubevent *) fLofSubevents.First();
		while (sevt) {
			if (LofSubevents.Length() > 0) LofSubevents += ":";
			LofSubevents += sevt->GetName();
			sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
		}
	}
	return(LofSubevents.Data());
}

