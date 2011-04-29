//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbEvent.cxx
// Purpose:        MARaBOU configuration: event definition
// Description:    Implements class methods to define events/triggers
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbEvent.cxx,v 1.30 2011-04-29 07:19:03 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TDirectory.h"

#include "TMrbSystem.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbEvent.h"
#include "TMrbSubevent.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbCamacScaler.h"
#include "TMrbVMEScaler.h"
#include "TMrbNamedArray.h"

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (gMrbConfig == NULL) {
		gMrbLog->Err() << "No config defined" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	 } else if (gMrbConfig->NameNotLegal("event", EvtName)) {
		this->MakeZombie();
	}

	TString evtName = EvtName;
	TString evtTitle = EvtTitle;

	fLofSubevents.Delete();

	TMrbConfig::EMrbTriggerStatus trigStat = gMrbConfig->GetTriggerStatus(Trigger);

	if (trigStat == TMrbConfig::kTriggerOutOfRange) {
		gMrbLog->Err() << "Trigger out of range - " << Trigger << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (trigStat == TMrbConfig::kTriggerAssigned) {
		TMrbEvent * evt = (TMrbEvent *) gMrbConfig->FindEvent(Trigger);
		gMrbLog->Err() << "Trigger \"" << Trigger << "\" already assigned to event \"" << evt->GetName() << "\"" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	}

	if (!this->IsZombie()) {
		if (evtName.IsNull()) {
			if (Trigger == TMrbConfig::kTriggerStartAcq) evtName = "xstart";
			else if (Trigger == TMrbConfig::kTriggerStopAcq) evtName = "xstop";
			else evtName = Form("Trig%d", Trigger);
		}
		this->SetName(evtName.Data());

		if (evtTitle.IsNull()) {
			evtTitle = Form("Event associated with trigger %d", Trigger);
			this->SetTitle(evtTitle.Data());
		}

		this->SetBaseClass("TUsrEvent");
		TString evtNameUC = evtName;
		evtNameUC(0,1).ToUpper();
		this->SetPointerName(evtNameUC.Data());

		Bool_t isStart = evtName.CompareTo("start") == 0 || evtName.CompareTo("xstart") == 0;
		Bool_t isStop = evtName.CompareTo("stop") == 0 || evtName.CompareTo("xstop") == 0;

		if (trigStat == TMrbConfig::kTriggerReserved) {
			if (!isStart && !isStop) {
				gMrbLog->Err() << "Illegal trigger assignment - " << Trigger << " is reserved" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else if (isStart) {
				if (Trigger == TMrbConfig::kTriggerStartAcq) {
					evtName = "xstart";
					evtTitle = Form("start acquisition, trigger %d, user-defined", TMrbConfig::kTriggerStartAcq);
					this->SetTitle(evtTitle.Data());
					this->SetBaseClass("TUsrEvtStart");
					this->SetPointerName("StartEvent");
				} else {
					gMrbLog->Err()	<< "Illegal trigger assignment - "
									<< evtName << ":" << Trigger
									<< " (should be " << TMrbConfig::kTriggerStartAcq << ")" << endl;
					gMrbLog->Flush(this->ClassName());
					this->MakeZombie();
				}
			} else if (isStop) {
				if (Trigger == TMrbConfig::kTriggerStopAcq) {
					evtName = "xstop";
					this->SetName(evtName.Data());
					evtTitle = Form("stop acquisition, trigger %d, user-defined", TMrbConfig::kTriggerStopAcq);
					this->SetTitle(evtTitle.Data());
					this->SetBaseClass("TUsrEvtStop");
					this->SetPointerName("StopEvent");
				} else {
					gMrbLog->Err()	<< "Illegal trigger assignment - "
									<< evtName << ":" << Trigger
									<< " (should be " << TMrbConfig::kTriggerStopAcq << ")" << endl;
					gMrbLog->Flush(this->ClassName());
					this->MakeZombie();
				}
			}
		} else {
			if (isStart || isStop) {
				gMrbLog->Err() << "Illegal event name - " << evtName << " is reserved" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			}
		}
	}

	if (!this->IsZombie()) {
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

		fMbsBranch.Set(-1, "none");

		fCreateHistoArray = kFALSE;

		gMrbConfig->AddEvent(this);
		gDirectory->Append(this);
	}
}

TMrbSubevent * TMrbEvent::FindSubeventByCrate(Int_t Crate, TMrbSubevent * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::FindSubeventByCrate
// Purpose:        Find a subevent with given crate number
// Arguments:      Int_t Crate              -- crate number
//                 TMrbSubevent * After     -- start search after obj
// Results:        TMrbSubevent * Subevent  -- subevent
// Exceptions:
// Description:    Searches for next subevent in crate "Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent * sevt;

	sevt = (After == NULL) ? (TMrbSubevent *) fLofSubevents.First() : (TMrbSubevent *) fLofSubevents.After(After);
	while (sevt) {
		if (sevt->GetCrate() == Crate) return(sevt);
		sevt = (TMrbSubevent *) fLofSubevents.After(sevt);
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


	TString assign = Assignment;
	TString sevtName;
	Int_t from = 0;
	while (assign.Tokenize(sevtName, from, " ")) {
		sevtName = sevtName.Strip(TString::kBoth);
		if (sevtName.Length() <= 0) break;
		TMrbSubevent * sevt;
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
	TIterator * siter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) siter->Next()) {
		if (DestEvent->FindSubevent(sevt->GetName()) == NULL) DestEvent->HasSubevent(sevt->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbEvent::Assign2dimHisto(const Char_t * HistoName, const Char_t * SevtX, const Char_t * ParamX, const Char_t * SevtY, const Char_t * ParamY) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Assign2dimHisto
// Purpose:        Assign 2-dim histo to be accumulated
// Arguments:      Char_t * HistoName        -- hostgram name
//                 Char_t * SevtX            -- subevent containing param X
//                 Char_t * ParamX           -- name of param X
//                 Char_t * SevtY            -- subevent containing param Y
//                 Char_t * ParamY           -- name of param Y
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds histo to list of 2-dim histos.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray * lofHistos = gMrbConfig->GetLofUserHistograms();
	TMrbNamedX * histo = NULL;
	if (lofHistos) histo = (TMrbNamedX *) lofHistos->FindObject(HistoName);
	if (histo == NULL) {
		gMrbLog->Err() << "Histogram not booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	if ((histo->GetIndex() & TMrbConfig::kHistoTH2) == 0) {
		gMrbLog->Err() << "Not a 2-dim histogram - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	TMrbSubevent * sx = (TMrbSubevent *) fLofSubevents.FindObject(SevtX);
	if (sx == NULL) {
		gMrbLog->Err() << "[Event " << this->GetName() << "]: Subevent not found - " << SevtX << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	TObjArray * lofParams = sx->GetLofParams();
	TMrbModuleChannel * px = NULL;
	if (sx) px = (TMrbModuleChannel *) lofParams->FindObject(ParamX);
	if (px == NULL) {
		gMrbLog->Err() << "[Subevent " << SevtX << "]: Param not found - " << ParamX << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	TMrbSubevent * sy = (TMrbSubevent *) fLofSubevents.FindObject(SevtY);
	if (sy == NULL) {
		gMrbLog->Err() << "[Event " << this->GetName() << "]: Subevent not found - " << SevtY << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	lofParams = sy->GetLofParams();
	TMrbModuleChannel * py = NULL;
	if (sy) py = (TMrbModuleChannel *) lofParams->FindObject(ParamY);
	if (py == NULL) {
		gMrbLog->Err() << "[Subevent " << SevtY << "]: Param not found - " << ParamX << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	Bool_t sxNeedsHB = sx->NeedsHitBuffer();
	Bool_t syNeedsHB = sy->NeedsHitBuffer();
	if (syNeedsHB != sxNeedsHB) {
		gMrbLog->Err() << "Subevent " << SevtX << (sxNeedsHB ? "DOES" : "DOES NOT") << " use hitbuffer, " << SevtY << (syNeedsHB ? "DOES" : "DOES NOT") << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	TObjArray x(3);
	Int_t nx = (px->Parent()->GetSerial() << 16) | px->GetAddr();
	Int_t ny = (py->Parent()->GetSerial() << 16) | py->GetAddr();
	x[0] = histo;
	x[1] = new TMrbNamedX(nx, sx->GetName(), sx->GetTitle());
	x[2] = new TMrbNamedX(ny, sy->GetName(), sy->GetTitle());
	fLof2dimHistos.AddNamedX(histo->GetIndex(), histo->GetName(), histo->GetTitle(), &x);

	return(kTRUE);
}

Bool_t TMrbEvent::Assign2dimHisto(const Char_t * HistoName, const Char_t * SevtX, Int_t ParamX, const Char_t * SevtY, Int_t ParamY) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Assign2dimHisto
// Purpose:        Assign 2-dim histo to be accumulated
// Arguments:      Char_t * HistoName        -- hostgram name
//                 Char_t * SevtX            -- subevent containing param X
//                 Int_t ParamX              -- name of param X
//                 Char_t * SevtY            -- subevent containing param Y
//                 Int_t ParamY              -- name of param Y
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds histo to list of 2-dim histos.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray * lofHistos = gMrbConfig->GetLofUserHistograms();
	TMrbNamedX * histo = NULL;
	if (lofHistos) histo = (TMrbNamedX *) lofHistos->FindObject(HistoName);
	if (histo == NULL) {
		gMrbLog->Err() << "Histogram not booked - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	if ((histo->GetIndex() & TMrbConfig::kHistoTH2) == 0) {
		gMrbLog->Err() << "Not a 2-dim histogram - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	TMrbSubevent * sx = (TMrbSubevent *) fLofSubevents.FindObject(SevtX);
	if (sx == NULL) {
		gMrbLog->Err() << "[Event " << this->GetName() << "]: Subevent not found - " << SevtX << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	TObjArray * lofParams = sx->GetLofParams();
	if (ParamX >= lofParams->GetEntriesFast()) {
		gMrbLog->Err() << "[Subevent " << SevtX << "]: Param out of range - " << ParamX << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	TMrbModuleChannel * px = (TMrbModuleChannel *) lofParams->At(ParamX);

	TMrbSubevent * sy = (TMrbSubevent *) fLofSubevents.FindObject(SevtY);
	if (sy == NULL) {
		gMrbLog->Err() << "[Event " << this->GetName() << "]: Subevent not found - " << SevtY << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	lofParams = sy->GetLofParams();
	if (ParamY >= lofParams->GetEntriesFast()) {
		gMrbLog->Err() << "[Subevent " << SevtY << "]: Param out of range - " << ParamY << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}
	TMrbModuleChannel * py = (TMrbModuleChannel *) lofParams->At(ParamY);

	Bool_t sxNeedsHB = sx->NeedsHitBuffer();
	Bool_t syNeedsHB = sy->NeedsHitBuffer();
	if (syNeedsHB != sxNeedsHB) {
		gMrbLog->Err() << "Subevent " << SevtX << (sxNeedsHB ? "DOES" : "DOES NOT") << " use hitbuffer, " << SevtY << (syNeedsHB ? "DOES" : "DOES NOT") << endl;
		gMrbLog->Flush(this->ClassName(), "Assign2dimHisto");
		return(kFALSE);
	}

	TObjArray x(3);
	Int_t nx = (px->Parent()->GetSerial() << 16) | ParamX;
	Int_t ny = (py->Parent()->GetSerial() << 16) | ParamY;
	x[0] = histo;
	x[1] = new TMrbNamedX(nx, sx->GetName(), sx->GetTitle());
	x[2] = new TMrbNamedX(ny, sy->GetName(), sy->GetTitle());
	fLof2dimHistos.AddNamedX(histo->GetIndex(), histo->GetName(), histo->GetTitle(), &x);

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
					if (sevt->GetMbsBranchNo() == this->GetSelectedBranchNo()) sevt->MakeReadoutCode(RdoStrm, TagIndex, Template, "%S%");
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
			{
				TIterator * siter = fLofSubevents.MakeIterator();
				while (sevt = (TMrbSubevent *) siter->Next()) sevt->MakeReadoutCode(RdoStrm, TagIndex, Template, "%S%");
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
	TString evtAutoSave;

	TMrbNamedX * analyzeTag;
	TMrbConfig::EMrbAnalyzeTag tagIdx;

	TString anaTemplateFile;
	TString templatePath, srcPath;

	TString tf, tf1, tf2, tf3, tf4;
	const Char_t * pcf;

	TMrbSubevent * sevt;
	Bool_t foundSevt;

	TMrbTemplate anaTmpl;

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	srcPath = gEnv->GetValue("TMrbConfig.SrcPath", gSystem->WorkingDirectory());
	gSystem->ExpandPathName(srcPath);

	evtNameLC = this->GetName();
	evtNameUC = evtNameLC;
	evtNameUC(0,1).ToUpper();

	TString fileSpec = "";
	TMrbSystem ux;
	TObjArray err;
	err.Delete();
	if (this->HasPrivateCode()) {
		tf = "Event_";
		tf += evtNameUC;
		tf += Extension;
		tf += ".code";
		tf1 = tf;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
		if (fileSpec.IsNull()) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += Extension;
				tf += ".code";
				tf2 = tf;
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
				if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
			if (fileSpec.IsNull()) {
				tf = this->ClassName();
				tf += Extension;
				tf += ".code";
				tf.ReplaceAll("TMrb", "");
				tf3 = tf;
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
				if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
		}
	}
	if (fileSpec.IsNull()) {
		tf = "Event";
		tf += Extension;
		tf += ".code";
		tf3 = tf;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
	}

	if (fileSpec.IsNull()) {
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
		gMrbLog->Out()  << "[" << evtNameLC << "] Using template file " << fileSpec << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}

 	anaTemplateFile = fileSpec;

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
					evtAutoSave = "";
					evtAutoSave += this->GetAutoSave();
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
						TString trigName(this->GetTrigger());
						ana << anaTmpl.Encode(line, trigName) << endl;
					}
					break;
				case TMrbConfig::kAnaEventBookParams:
					if (this->GetTriggerStatus() != TMrbConfig::kTriggerPattern) {
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
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
						}
					}
					break;
				case TMrbConfig::kAnaEventBookHistograms:
					if ((this->GetAnalyzeOptions() & TMrbConfig::kAnaOptHistograms)
					&&  (this->GetTriggerStatus() != TMrbConfig::kTriggerPattern)) {
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							if (sevt->HistosToBeAllocated()) {
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
							}
						}
					}
					break;
				case TMrbConfig::kAnaEventSetupSevtList:
					{
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode();
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.Substitute("$sevtSerial", sevt->GetSerial());
							anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
							anaTmpl.WriteCode(ana);
						}
					}
					break;
				case TMrbConfig::kAnaEventAllocHitBuffer:
					{
						anaTmpl.InitializeCode("%H%");
						anaTmpl.Substitute("$evtNameLC", evtNameLC);
						anaTmpl.Substitute("$evtNameUC", evtNameUC);
						anaTmpl.Substitute("$nofEntries", this->GetSizeOfHitBuffer());
						anaTmpl.Substitute("$highWater", this->GetHBHighWaterLimit());
						anaTmpl.WriteCode(ana);
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							if (sevt->NeedsHitBuffer()) {
								sevtNameLC = sevt->GetName();
								sevtNameUC = sevtNameLC;
								sevtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode(sevt->HasXhit() ? "%SPH%" : "%S%");
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								anaTmpl.Substitute("$sevtXhit", sevt->GetNameOfXhit());
								anaTmpl.WriteCode(ana);
							}
						}
					}
					break;
				case TMrbConfig::kAnaEventUserMethods:
					if (gMrbConfig->UserCodeToBeIncluded()) {
						TMrbNamedX * icl;
						Bool_t first = kTRUE;
						TIterator * iter = gMrbConfig->GetLofUserIncludes()->MakeIterator();
						while (icl = (TMrbNamedX *) iter->Next()) {
							if ((icl->GetIndex() & TMrbConfig::kIclOptClassTUsrEvent) == TMrbConfig::kIclOptClassTUsrEvent) {
								TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
								TMrbNamedX * nx;
								TIterator * niter = lofMethods->MakeIterator();
								while (nx = (TMrbNamedX *) niter->Next()) {
									if ((nx->GetIndex() & TMrbConfig::kIclOptEventMethod) == TMrbConfig::kIclOptEventMethod) {
										TString method = nx->GetName();
										TString scope = "TUsrEvt";
										scope += evtNameUC.Data();
										scope += "::";
										if (method.Contains(scope.Data())) {
											if (first) ana << "// user-defined method(s)" << endl;
											first = kFALSE;
											method = nx->GetTitle();
											method.ReplaceAll(scope.Data(), "");
											method = method.Strip(TString::kBoth);
											ana << "\t\t" << method << endl;
										}
									}
								}
							}
						}
					}
					break;
				case TMrbConfig::kAnaEventMethodToProcessEvent:
				case TMrbConfig::kAnaEventReplayEvent:
					if (gMrbConfig->UserCodeToBeIncluded()) {
						Bool_t udc = kFALSE;
						TMrbNamedX * icl;
						TIterator * iter = gMrbConfig->GetLofUserIncludes()->MakeIterator();
						while (icl = (TMrbNamedX *) iter->Next()) {
							if ((icl->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
								TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
								TMrbNamedX * nx;
								TIterator * niter = lofMethods->MakeIterator();
								while (nx = (TMrbNamedX *) niter->Next()) {
									if ((nx->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
										anaTmpl.InitializeCode();
										anaTmpl.Substitute("$evtNameLC", evtNameLC);
										anaTmpl.Substitute("$evtNameUC", evtNameUC);
										TString method = nx->GetName();
										Int_t n = method.Index("::", 0);
										if (n >= 0) method = method(n + 2, method.Length() - n - 2);
										anaTmpl.Substitute("$processEvent", method.Data());
										anaTmpl.WriteCode(ana);
										udc = kTRUE;
										break;
									}
								}
							}
							if (udc) break;
						}
						if (udc) break;
					}
					anaTmpl.InitializeCode();
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$processEvent", "Analyze");
					anaTmpl.WriteCode(ana);
					break;
				case TMrbConfig::kAnaEventAnalyze:
					if (gMrbConfig->UserCodeToBeIncluded()) {
						Bool_t udc = kFALSE;
						TMrbNamedX * icl;
						TIterator * iter = gMrbConfig->GetLofUserIncludes()->MakeIterator();
						while (icl = (TMrbNamedX *) iter->Next()) {
							if ((icl->GetIndex() & TMrbConfig::kIclOptProcessEvent) == TMrbConfig::kIclOptProcessEvent) {
								udc = kTRUE;
								break;
							}
						}
						if (udc) break;
					}
					anaTmpl.InitializeCode();
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$evtTrigger", this->GetTrigger());
					anaTmpl.WriteCode(ana);
					break;
				case TMrbConfig::kAnaEventFillHistograms:
					anaTmpl.InitializeCode("%B%");
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$evtTrigger", this->GetTrigger());
					anaTmpl.WriteCode(ana);
					if ((this->GetAnalyzeOptions() & TMrbConfig::kAnaOptHistograms) != 0) {
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							if (sevt->HistosToBeAllocated()) {
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
							}
						}
					}
					anaTmpl.InitializeCode("%E%");
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.WriteCode(ana);
					break;
				case TMrbConfig::kAnaEventFillRateHistograms:
					anaTmpl.InitializeCode("%B%");
					anaTmpl.Substitute("$evtNameLC", evtNameLC);
					anaTmpl.Substitute("$evtNameUC", evtNameUC);
					anaTmpl.Substitute("$evtTrigger", this->GetTrigger());
					anaTmpl.WriteCode(ana);
					if ((this->GetAnalyzeOptions() & TMrbConfig::kAnaOptHistograms) != 0) {
						TObjArray * uHistos = gMrbConfig->GetLofUserHistograms();
						TMrbNamedX * h;
						TIterator * uiter = uHistos->MakeIterator();
						while (h = (TMrbNamedX *) uiter->Next()) {
							if (h->GetIndex() == TMrbConfig::kHistoRate) {
								TMrbNamedArrayD * a = (TMrbNamedArrayD *) h->GetAssignedObject();
								Int_t hRange = (Int_t) a->At(2);
								Int_t hScale = (Int_t) a->At(3);
								TString xScale;
								if (hScale == 1) xScale = "micro secs";
								else if (hScale == 1000) xScale = "milli secs";
								else if (hScale == 1000000) xScale = "secs";
								hScale *= 10;
								Bool_t hLoop = (a->At(4) > 0);
								anaTmpl.InitializeCode(hLoop ? "%RL%" : "%RN%");
								anaTmpl.Substitute("$hName", h->GetName());
								anaTmpl.Substitute("$hScale", hScale);
								anaTmpl.Substitute("$xScale", xScale.Data());
								anaTmpl.Substitute("$hRange", hRange);
								anaTmpl.WriteCode(ana);
							}
						}
					}
					anaTmpl.InitializeCode("%E%");
					anaTmpl.WriteCode(ana);
					break;
				case TMrbConfig::kAnaEventAddBranches:
					{
						if (this->IsStartEvent()) {
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$evt", "start");
							anaTmpl.WriteCode(ana);
						} else if (this->IsStopEvent()) {
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$evt", "stop");
							anaTmpl.WriteCode(ana);
						}
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%N%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.WriteCode(ana);
						}
					}
					break;
				case TMrbConfig::kAnaEventInitializeBranches:
					{
						if (this->IsStartEvent()) {
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$evt", "start");
							anaTmpl.WriteCode(ana);
						} else if (this->IsStopEvent()) {
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$evt", "stop");
							anaTmpl.WriteCode(ana);
						}
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%N%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.WriteCode(ana);
						}
					}
					break;
				case TMrbConfig::kAnaSevtClassInstance:
				case TMrbConfig::kAnaSevtGetAddr:
				case TMrbConfig::kAnaSevtSetName:
					{
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode();
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.WriteCode(ana);
						}
					}
					break;
				case TMrbConfig::kAnaEvtResetData:
					{
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							sevtNameLC = sevt->GetName();
							sevtNameUC = sevtNameLC;
							sevtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%S%");
							anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
							anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
							anaTmpl.WriteCode(ana);
						}
						anaTmpl.InitializeCode("%R%");
						anaTmpl.WriteCode(ana);
					}
					break;
				case TMrbConfig::kAnaEvtBaseClass:
					if (this->IsReservedEvent()) {
						if (this->GetTrigger() == TMrbConfig::kTriggerStartAcq) {
							ana << anaTmpl.Encode(line, "TUsrEvtStart") << endl;
						} else if (this->GetTrigger() == TMrbConfig::kTriggerStopAcq) {
							ana << anaTmpl.Encode(line, "TUsrEvtStop") << endl;
						} else {
							ana << anaTmpl.Encode(line, "TUsrEvent") << endl;
						}
					} else {
						ana << anaTmpl.Encode(line, "TUsrEvent") << endl;
					}
					break;
				case TMrbConfig::kAnaSevtDispatchOverType:
					{
						foundSevt = kFALSE;
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
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
						}
						if (foundSevt) {
							anaTmpl.InitializeCode("%E%");
							anaTmpl.WriteCode(ana);
						}
					}
					break;
				case TMrbConfig::kAnaSevtDispatchOverSerial:
					{
						TIterator * siter = fLofSubevents.MakeIterator();
						while (sevt = (TMrbSubevent *) siter->Next()) {
							if (sevt->SerialToBeCreated()) {
								sevtNameLC = sevt->GetName();
								sevtNameUC = sevtNameLC;
								sevtNameUC(0,1).ToUpper();
								anaTmpl.InitializeCode();
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$sevtSerial", sevt->GetSerial());
								anaTmpl.Substitute("$sevtTitle", sevt->GetTitle());
								anaTmpl.Substitute("$sevtType", (Int_t) sevt->GetType());
								anaTmpl.Substitute("$sevtSubtype", (Int_t) sevt->GetSubtype());
								anaTmpl.Substitute("$sevtWC", sevt->GetNofParams());
								anaTmpl.Substitute("$rawMode", sevt->IsRaw() ? "kTRUE" : "kFALSE");
								anaTmpl.WriteCode(ana);
							}
						}
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
			{
				Template.InitializeCode("%B%");
				Template.Substitute("$treeName", treeName);
				Template.WriteCode(ana);
				TIterator * siter = fLofSubevents.MakeIterator();
				while (sevt = (TMrbSubevent *) siter->Next()) sevt->MakeAnalyzeCode(ana, TagIndex, this, Template, "%S%");
				Template.InitializeCode("%E%");
				Template.Substitute("$treeName", treeName);
				Template.WriteCode(ana);
			}
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
		TIterator * siter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) siter->Next()) {
			Template.InitializeCode();
			Template.Substitute("$sevtNameLC", sevt->GetName());
			Template.Substitute("$evtNameLC", this->GetName());
			Template.Substitute("$evtTitle", this->GetTitle());
			Template.WriteCode(CfgStrm);
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

UInt_t TMrbEvent::GetReadoutOptions() const {
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

UInt_t TMrbEvent::GetAnalyzeOptions() const {
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

UInt_t TMrbEvent::GetConfigOptions() const {
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
void TMrbEvent::Print(ostream & OutStrm, const Char_t * Prefix) const {
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
		TIterator * siter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) siter->Next()) sevt->Print(OutStrm, prefix.Data());
	}
}

const Char_t * TMrbEvent::GetLofSubeventsAsString(TString & LofSubevents) const {
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
		TIterator * siter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) siter->Next()) {
			if (LofSubevents.Length() > 0) LofSubevents += ":";
			LofSubevents += sevt->GetName();
		}
	}
	return(LofSubevents.Data());
}


Bool_t TMrbEvent::SetMbsBranch(Int_t MbsBranchNo, const Char_t * MbsBranchName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::SetMbsBranch
// Purpose:        Assign event to a mbs branch
// Arguments:      Int_t MbsBranchNo         -- mbs branch number
//                 Char_t * BranchName       -- name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Assigns module to a mbs branch.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if(this->GetMbsBranchNo() != -1) {
		gMrbLog->Err()	<< "Event " << this->GetName() << " already assigned to mbs branch "
						<< fMbsBranch.GetName() << "(" << fMbsBranch.GetIndex() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMbsBranch");
		return(kFALSE);
	}
	if (!gMrbConfig->SetMbsBranch(fMbsBranch, MbsBranchNo, MbsBranchName)) {
		gMrbLog->Err()	<< "Event " << this->GetName() << "- can't set mbs branch" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMbsBranch");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbEvent::SelectMbsBranch(Int_t MbsBranchNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent::SelectMbsBranch
// Purpose:        Select a branch
// Arguments:      Int_t MbsBranchNo         -- mbs branch number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if given branch number is assigned to this event
//                 or to at least one subevent.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bNo = this->GetMbsBranchNo();
	if (bNo != -1) {
		if(bNo == MbsBranchNo) {
			fSelectedBranchNo = MbsBranchNo;
			return(kTRUE);
		}
	} else {
		TMrbSubevent * sevt;
		TIterator * sevtIter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) sevtIter->Next()) {
			if (sevt->GetMbsBranchNo() == MbsBranchNo) {
				fSelectedBranchNo = MbsBranchNo;
				return(kTRUE);
			}
		}
	}
	return(kFALSE);
}
