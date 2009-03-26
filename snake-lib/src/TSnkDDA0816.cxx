//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816.cxx
// Purpose:        Implement class methods for dac module DDA0816
// Description:    Class methods to operate a dac type DDA0816
//                 in the SNAKE environment
//
// Header files:   TSnkDDA0816.h           -- class definitions
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TSnkDDA0816.cxx,v 1.1 2009-03-26 11:18:28 Rudolf.Lutter Exp $       
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
#include "TEnv.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TMrbEnv.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "TSnkDDA0816.h"
#include "TSnkDDACommon.h"
#include "TSnkDDAMessage.h"

#include "SetColor.h"

ClassImp(TSnkDDA0816)
ClassImp(TSnkDDAMessage)

extern TMrbLogger * gMrbLog;

void TSnkDDAMessage::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage::Reset
// Purpose:        Reset message data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes message data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fHost = gSystem->HostName();
	fPort = kSnkDefaultPort;
	fPid = gSystem->GetPid();
	fProgram = "";
	fAction = "undef";
	fExecMode = "normal";
	fInterval = 0.;
	fPath = gSystem->WorkingDirectory();
	fSaveScan = "";
	fText = "";
}

const Char_t * TSnkDDAMessage::ToString(TString & Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::ToString
// Purpose:        Convert message data to string
// Arguments:      TString & Message       -- where to put message data
// Results:        Char_t * Message        -- pointer to message data
// Exceptions:     
// Description:    Converts message data to string.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Msg.Resize(0);
	Msg = this->ClassName();	Msg += "|";
	Msg += fHost;				Msg += "|";
	Msg += fPort;				Msg += "|";
	Msg += fPid;				Msg += "|";
	Msg += fText;				Msg += "|";
	Msg += fProgram; 			Msg += "|";
	Msg += fAction;				Msg += "|";
	Msg += fExecMode;			Msg += "|";
	Msg += fInterval;			Msg += "|";
	Msg += fPath;				Msg += "|";
	Msg += fSaveScan;
	return(Msg.Data());
}

Bool_t TSnkDDAMessage::FromString(TString & Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage::FromString
// Purpose:        Decode string to message
// Arguments:      TString & Message    -- where to get message data from
// Results:        --
// Exceptions:     
// Description:    Decodes message string.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString intStr;
	TString dblStr;

	TObjArray * msgArr = Msg.Tokenize("|");
	Int_t n = msgArr->GetEntries();
	if (n <= 0) { delete msgArr; return(kFALSE); }
	TString type = ((TObjString *) msgArr->At(kSnkMsgIdxType))->GetString();
	if (type.CompareTo(this->ClassName()) != 0) return(kFALSE);

	fHost = ((TObjString *) msgArr->At(kSnkMsgIdxHost))->GetString();
	intStr = ((TObjString *) msgArr->At(kSnkMsgIdxPort))->GetString();		fPort = intStr.Atoi();
	intStr = ((TObjString *) msgArr->At(kSnkMsgIdxPid))->GetString();		fPid = intStr.Atoi();
	fText = ((TObjString *) msgArr->At(kSnkMsgIdxText))->GetString();
	fProgram = ((TObjString *) msgArr->At(kSnkMsgIdxProgram))->GetString();
	fAction = ((TObjString *) msgArr->At(kSnkMsgIdxAction))->GetString();
	fExecMode = ((TObjString *) msgArr->At(kSnkMsgIdxExecMode))->GetString();
	dblStr = ((TObjString *) msgArr->At(kSnkMsgIdxInterval))->GetString();	fInterval = dblStr.Atof();
	fPath = ((TObjString *) msgArr->At(kSnkMsgIdxPath))->GetString();
	fSaveScan = ((TObjString *) msgArr->At(kSnkMsgIdxScanFile))->GetString();
	delete msgArr;
	return(kTRUE);
}

void TSnkDDAMessage::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage::Print
// Purpose:        Print message
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:     
// Description:    Outputs message data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX lofActions;
	TMrbLofNamedX lofExecModes;
	TMrbNamedX * nx;
	TString str;
	UInt_t eMode;

	lofActions.AddNamedX(kSnkLofActions);
	lofExecModes.AddNamedX(kSnkLofExecModes);
	lofExecModes.SetPatternMode();

	Out << endl << this->ClassName() << "::Print():" << endl;
	Out << "Host (Port)      : " << fHost << " (" << fPort << ")" << endl;
	fProgram = fProgram.Strip(TString::kBoth);
	if (fProgram.Length() > 0)	Out << "Program (PID)    : " << fProgram << " (" << fPid << ")" << endl;
	fText = fText.Strip(TString::kBoth);
	if (fText.Length() > 0) 	Out << "Message text     : " << fText << endl;
	nx = lofActions.FindByName(fAction, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (nx) Out << "Action           : " << nx->GetTitle() << " (" << nx->GetIndex() << ")" << endl;
	else	Out << "Action           : " << fAction << " (unknown)" << endl;
	eMode = lofExecModes.FindPattern(fExecMode);
	Out << "Exec mode        : " << lofExecModes.Pattern2String(str, eMode, ":") << endl;
	if (fInterval == 0.) Out << "Scan interval    : infinite" << endl;
	else				Out << "Scan interval    : " << fInterval << " s" << endl;
	fPath = fPath.Strip(TString::kBoth);
	if (fPath.Length() > 0) 	Out << "Path             : " << fPath << endl;
	if (fSaveScan.Length() > 0) Out << "Path             : " << fSaveScan << endl << endl;
}

TSnkDDA0816::TSnkDDA0816(Int_t Subdevice) : TNamed("DDA0816", "Snake DAC") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816
// Purpose:        Create a DDA0816 object
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->SetSubdevice(Subdevice)) {
		this->MakeZombie();
	} else {
		fMaxPoints = gEnv->GetValue("TSnkDDA0816.MaxPoints", 0);	// number of points/pixels
		if (fMaxPoints == 0) fMaxPoints = gEnv->GetValue("DDAControl.MaxPoints", kSnkMaxPoints);

		fDacX.Reset(fMaxPoints);							// reset X
		fDacY.Reset(fMaxPoints);							// ... Y
		fDacH.Reset(fMaxPoints);							// ... hysteresis
		fDacB.Reset(fMaxPoints);							// ... beam on/off
		fAmplYScale = 1.0;									// scale factor Y if symmetric scan (obsolete)
		fPacerClock = 1;									// pacer clock off
		fClockSource = kSnkCSONB; 								// clock source on-board
		fPreScale = kSnkPSUndef;								// pixel frequ off
		fScanMode = kSnkSMUndef;								// scan mode undefined
		fExecMode = kSnkEMNormal; 								// exec mode normal
		fSocket = NULL; 									// not connected
		fLofPreScales.SetName("PreScaler Values");			// list of prescale values
		fLofPreScales.AddNamedX(kSnkLofPreScales);
		fLofClockSources.SetName("Clock Sources");			// list of clock sources
		fLofClockSources.AddNamedX(kSnkLofClockSources);
		fLofScanProfiles.SetName("Scan Profiles");			// list of scan profiles
		fLofScanProfiles.AddNamedX(kSnkLofScanProfiles);
		fLofScanModes.SetName("Scan Modes");				// list of scan modes
		fLofScanModes.AddNamedX(kSnkLofScanModes);
		fLofExecModes.SetName("Exec Modes");				// list of exec modes
		fLofExecModes.AddNamedX(kSnkLofExecModes);
		fLofExecModes.SetPatternMode();

		gROOT->Append(this);
	}
}

Bool_t TSnkDDA0816::SetSubdevice(Int_t Subdevice) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetSubdevice
// Purpose:        Define channel names according to subdevice
// Arguments:      Int_t Subdevice      -- subdevice number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Assigns channel names.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t offs;
	TString ddaName;
		
	if (Subdevice < kSnkNofSubdevs) {
		fSubdevice = Subdevice;
		offs = Subdevice * kSnkNofChnPerSubdev;
		ddaName = "/dev/dda08_";
		ddaName += offs;
		offs++;
		fDacX.SetChannel(kDacX, ddaName.Data(), "X", this);			// reset X
		ddaName = "/dev/dda08_";
		ddaName += offs;
		offs++;
		fDacY.SetChannel(kDacY, ddaName.Data(), "Y", this); 		// ... Y
		ddaName = "/dev/dda08_";
		ddaName += offs;
		offs++;
		fDacH.SetChannel(kDacH, ddaName.Data(), "H", this); 		// ... hysteresis
		ddaName = "/dev/dda08_";
		ddaName += offs;
		offs++;
		fDacB.SetChannel(kDacB, ddaName.Data(), "B", this); 		// ... beam on/off
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Illegal subdevice number - " << Subdevice
						<< " (should be in [0," << kSnkNofSubdevs - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSubdevice");
		return(kFALSE);
	}
}

TSnkDDAChannel * TSnkDDA0816::GetDac(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetDac
// Purpose:        Return DAC address
// Arguments:      Int_t Channel        -- channel number
// Results:        TSnkDDAChannl * Addr -- channel address
// Exceptions:     
// Description:    Returns dac address.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;
	chn = this->CheckChannel(Channel, this->ClassName(), "GetDac");
	if (chn == NULL) return(&fDacError); else return(chn);
};

Bool_t TSnkDDA0816::CheckParams(const Char_t * Class, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::CheckParams
// Purpose:        Check if channel number is ok
// Arguments:      Char_t * Class      -- class name
//                 Char_t * Method     -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks validity of params.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t nofErrors;
	TSnkDDAChannel * chn;

	nofErrors = 0;

	if (this->GetScanMode() != kSnkSMFromFile) {
		chn = &fDacX;
		for (i = 0; i <= 1; i++) {
			if (chn->GetOffset() == -1) {
				gMrbLog->Err() << "DAC channel " << i << ": No OFFSET defined" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}
			if (chn->GetAmplitude() == -1) {
				gMrbLog->Err() << "DAC channel " << i << ": No AMPLITUDE defined" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}
			if (chn->GetNofPixels() == -1) {
				gMrbLog->Err() << "DAC channel " << i << ": No SCAN SIZE defined" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}
			if (chn->GetScanProfile() == kSnkSPUndef) {
				gMrbLog->Err() << "DAC channel " << i << ": No SCAN PROFILE defined" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}
			if (this->GetScanMode() != kSnkSMLShape && chn->GetOffset() - chn->GetAmplitude() < 0) {
				gMrbLog->Err()	<< "DAC channel " << i << ": Out of range - "
								<< "Offset (" << chn->GetOffset() << ") - Amplitude (" << chn->GetAmplitude() << ") < 0"
								<< endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}

			if (chn->GetOffset() + chn->GetAmplitude() > kSnkMaxRange) {
				gMrbLog->Err()	<< "DAC channel " << i << ": Out of range - "
								<< "Offset (" << chn->GetOffset() << ") + Amplitude (" << chn->GetAmplitude() << ") > "
								<< kSnkMaxRange << endl;
				gMrbLog->Flush(this->ClassName(), "CheckParams");
				nofErrors++;
			}
			chn = &fDacY;
		}
	}

	Int_t preScale = this->GetPreScaler()->GetIndex();
	if (preScale == kSnkPSUndef) {
		gMrbLog->Err() << "No PIXEL FREQU defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckParams");
		nofErrors++;
	}

	if (preScale == kSnkPS100k && this->GetPacerClock() < 2) this->SetPacerClock(2);

	if (this->GetClockSource()->GetIndex() == kSnkCSUndef) {
		gMrbLog->Err() << "No CLOCK SOURCE defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckParams");
		nofErrors++;
	}

	if (this->GetSoftScale(0) > 0 && this->GetSoftScale(1) > 0) {
		gMrbLog->Err() << "At least one DAC channel should have SOFTSCALE = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckParams");
		nofErrors++;
	}

	if (nofErrors > 0) {
		gMrbLog->Err() << "Wrong param settings (" << nofErrors << " error(s)) - can't proceed" << endl;
		gMrbLog->Flush(Class, Method);
		return(kFALSE);
	}
	return(kTRUE);
}

TSnkDDAChannel * TSnkDDA0816::CheckChannel(Int_t Channel, const Char_t * Class, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::CheckChannel
// Purpose:        Check if channel number is ok
// Arguments:      Int_t Channel            -- channel number (0 or 1)
//                 Char_t * Class           -- class name
//                 Char_t * Method          -- calling method
// Results:        TSnkDDAChannel * Chaddr  -- channel addr
// Exceptions:     returns NULL on error
// Description:    Checks if channel number is 0, 1 or 2.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Channel) {
		case 0: 	return(&fDacX);
		case 1: 	return(&fDacY);
		case 2: 	return(&fDacH);
		case 3: 	return(&fDacB);
	}

	gMrbLog->Err() << "Illegal channel number - " << Channel << endl;
	gMrbLog->Flush(Class, Method);
	return(NULL);
}

TSocket * TSnkDDA0816::GetConnection(TString & Server, Int_t & Port) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetConnection
// Purpose:        Get connection data
// Arguments:      TString & Server    -- server name
//                 Int_t & Port        -- port number
// Results:        TSocket * Socket    -- TCP socket
// Exceptions:     
// Description:    Returns connection data to be used bu other DDA objects.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Server = fServer;
	Port = fPort;
	return(fSocket);
}

Bool_t TSnkDDA0816::SetConnection(TSocket * Socket, const Char_t * Server, Int_t Port) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetConnection
// Purpose:        Set connection data
// Arguments:      TSocket * Socket    -- TCP socket
//                 Char_t * Server     -- server name
//                 Int_t Port          -- port number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets connection to what we got from other DDA object.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fSocket = Socket;
	fServer = Server;
	fPort = Port;
	return(kTRUE);
}

Bool_t TSnkDDA0816::CheckConnect(const Char_t * Class, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::CheckConnect
// Purpose:        Check if server connection ok
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     returns NULL on error
// Description:    Checks if channel number is 0, 1 or 2.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsConnected()) {
		gMrbLog->Err() << "No connection to server" << endl;
		gMrbLog->Flush(Class, Method);
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TSnkDDA0816::AdjustSettings(Int_t Channel, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::AdjustSettings
// Purpose:        Calculate a legal set of params
// Arguments:      Int_t Channel       -- DAC channel
//                 Bool_t Verbose      -- error printout on/off
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates increment from amplitude and scan size,
//                 adjusts amplitude if necessary.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;
	Int_t nofPixels;
	Int_t amplitude;
	Int_t increment;
	Bool_t ok;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "AdjustSettings")) == NULL) return(kFALSE);

	cout << "@@@1 Addr " << Channel << " " << setbase(16) << chn << setbase(10) << endl;

	nofPixels = chn->GetNofPixels();
	cout << "@@@ " << Channel << " " << nofPixels << endl;

	ok = kTRUE;

	if (nofPixels == 0) {
		if (Verbose) {
			gMrbLog->Err() << "DAC channel " << Channel << ": Scan size undefined" << endl;
			gMrbLog->Flush(this->ClassName(), "AdjustSettings");
		}
		ok = kFALSE;
	}

	amplitude = chn->GetAmplitude();
	if (this->GetScanMode() != kSnkSMLShape) amplitude *= 2;
	
	if (amplitude == 0) {
		if (Verbose) {
			gMrbLog->Err() << "DAC channel " << Channel << ": Amplitude undefined" << endl;
			gMrbLog->Flush(this->ClassName(), "AdjustSettings");
		}
		ok = kFALSE;
	}

	if (ok) {
		increment = (amplitude + nofPixels - 1) / nofPixels;
		chn->SetIncrement(increment);
		amplitude = (increment * nofPixels);
		if (this->GetScanMode() != kSnkSMLShape) amplitude /= 2;
	}
	if (this->GetScanMode() != kSnkSMLShape && chn->GetOffset() - amplitude < 0) {
		if (Verbose) {
			gMrbLog->Err()	<< "DAC channel " << Channel << ": Out of range - "
							<< "Offset (" << chn->GetOffset() << ") - Amplitude (" << amplitude << ") < 0" << endl;
			gMrbLog->Flush(this->ClassName(), "AdjustSettings");
		}
		ok = kFALSE;
	}

	if (chn->GetOffset() + amplitude > kSnkMaxRange) {
		if (Verbose) {
			gMrbLog->Err()	<< "DAC channel " << Channel << ": Out of range - "
							<< "Offset (" << chn->GetOffset() << ") + Amplitude (" << amplitude << ") > " << kSnkMaxRange
							<< endl;
			gMrbLog->Flush(this->ClassName(), "AdjustSettings");
		}
		ok = kFALSE;
	}

	if (ok) chn->SetAmplitude(amplitude);
	
	return(ok);
}

Bool_t TSnkDDA0816::SetPreScaler(const Char_t * PreScale) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetPreScaler
// Purpose:        Set prescale
// Arguments:      Char_t * PreScale        -- prescale value
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets prescale value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	ESnkPreScale ps;

	if ((kp = fLofPreScales.FindByName(PreScale, TMrbLofNamedX::kFindIgnoreCase | TMrbLofNamedX::kFindUnique)) == NULL) {
		gMrbLog->Err() << "Illegal prescale value - " << PreScale << endl;
		gMrbLog->Flush(this->ClassName(), "SetPreScaler");
		return(kFALSE);
	}
	ps = (ESnkPreScale) kp->GetIndex();
	if (ps == kSnkPSExt) {
		this->SetClockSource(kSnkCSEXT);
	} else {
		this->SetClockSource(kSnkCSONB);
	}
	this->SetPreScaler(ps);
	return(kTRUE);
}

Bool_t TSnkDDA0816::SetPreScaler(ESnkPreScale PreScale) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetPreScaler
// Purpose:        Set prescale
// Arguments:      ESnkPreScale PreScale        -- prescale value
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets prescale value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (PreScale == kSnkPSExt) {
		this->SetClockSource(kSnkCSEXT);
	} else {
		this->SetClockSource(kSnkCSONB);
	}
	fPreScale = PreScale;
	return(kTRUE);
}

Bool_t TSnkDDA0816::SetClockSource(const Char_t * ClockSource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetClockSource
// Purpose:        Set clock source
// Arguments:      Int_t Channel                 -- channel number (0 or 1)
//                 Char_t * ClockSource          -- clock source
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets clock source.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;

	if ((kp = fLofClockSources.FindByName(ClockSource, TMrbLofNamedX::kFindIgnoreCase | TMrbLofNamedX::kFindUnique)) == NULL) {
		gMrbLog->Err() << "Illegal clock source - " << ClockSource << endl;
		gMrbLog->Flush(this->ClassName(), "SetClockSource");
		return(kFALSE);
	}
	return(this->SetClockSource((ESnkClockSource) kp->GetIndex()));
}

Bool_t TSnkDDA0816::SetSoftScale(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetSoftScale
// Purpose:        Set soft scale
// Arguments:      Int_t Channel           -- channel number (0 or 1)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets soft scale value.
//                 Soft scale down for one channel is derived from scan size
//                 of the other one.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t softScale;
	TSnkDDAChannel * chn;
	TSnkDDAChannel * chn2;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetSoftScale")) == NULL) return(kFALSE);

	softScale = -1;
	chn2 = NULL;

	switch (Channel) {
		case 0:
			softScale = fDacY.GetNofPixels() - 1;
			chn2 = &fDacY;
			break;
		case 1:
			softScale = fDacX.GetNofPixels() - 1;
			chn2 = &fDacX;
			break;
	}

	if (softScale < 0) {
		gMrbLog->Err() << "Illegal soft scale down for DAC channel " << Channel << " - " << softScale << endl;
		gMrbLog->Flush(this->ClassName(), "SetSoftScale");
		return(kFALSE);
	}

	chn->SetSoftScale(softScale);
	chn2->SetSoftScale(0);
	return(kTRUE);
}

Int_t  TSnkDDA0816::GetSoftScale(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetSoftScale
// Purpose:        Return soft scale
// Arguments:      Int_t Channel           -- channel number (0 or 1)
// Results:        Int_t SoftScale         -- soft scale value
// Exceptions:     
// Description:    Returns spft scale value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "GetSoftScale")) == NULL) return(kFALSE);

	return(chn->GetSoftScale());
}

Bool_t TSnkDDA0816::SetOffset(Int_t Channel, Int_t Offset) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetOffset
// Purpose:        Set offset value
// Arguments:      Int_t Channel           -- channel number (0 or 1)
//                 Int_t Offset            -- offset value
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets offset (=start) value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetOffset")) == NULL) return(kFALSE);

	chn->SetOffset(Offset);
	return(kTRUE);
}

Int_t TSnkDDA0816::GetOffset(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetOffset
// Purpose:        Return offset value
// Arguments:      Int_t Channel           -- channel number (0 or 1)
// Results:        Int_t Offset            -- offset
// Exceptions:     
// Description:    Returns offset (=start) value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "GetOffset")) == NULL) return(kFALSE);

	return(chn->GetOffset());
}

Bool_t TSnkDDA0816::SetAmplitude(Int_t Channel, Int_t Amplitude) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetAmplitude
// Purpose:        Set scan range
// Arguments:      Int_t Channel           -- channel number (0 or 1)
//                 Int_t Amplitude         -- scan amplitude
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets scan range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetAmplitude")) == NULL) return(kFALSE);

	chn->SetAmplitude(Amplitude);
	return(kTRUE);
}

Int_t TSnkDDA0816::GetAmplitude(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetAmplitude
// Purpose:        Return scan range
// Arguments:      Int_t Channel           -- channel number (0 or 1)
// Results:        Int_t Amplitude         -- amplitude value
// Exceptions:     
// Description:    Returns scan range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "GetAmplitude")) == NULL) return(kFALSE);

	return(chn->GetAmplitude());
}

Bool_t TSnkDDA0816::SetIncrement(Int_t Channel, Int_t Increment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetIncrement
// Purpose:        Set scan increment
// Arguments:      Int_t Channel           -- channel number (0 or 1)
//                 Int_t Increment         -- scan increment
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets step size.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetIncrement")) == NULL) return(kFALSE);

	chn->SetIncrement(Increment);
	return(kTRUE);
}

Int_t TSnkDDA0816::GetIncrement(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetIncrement
// Purpose:        Return scan increment
// Arguments:      Int_t Channel           -- channel number (0 or 1)
// Results:        Int_t Increment         -- increment
// Exceptions:     
// Description:    Returns step size.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "GetIncrement")) == NULL) return(kFALSE);

	return(chn->GetIncrement());
}

Bool_t TSnkDDA0816::SetScanProfile(Int_t Channel, const Char_t * ScanProfile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetScanProfile
// Purpose:        Set scan profile
// Arguments:      Int_t Channel            -- channel number (0 or 1)
//                 Char_t * ScanProfile     -- scan profile (triangle, sawtooth left/right)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets scan profile.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;

	if (this->CheckChannel(Channel, this->ClassName(), "SetScanProfile") == NULL) return(kFALSE);

	if ((kp = fLofScanProfiles.FindByName(ScanProfile, TMrbLofNamedX::kFindIgnoreCase | TMrbLofNamedX::kFindUnique)) == NULL) {
		gMrbLog->Err() << "Illegal scan profile - " << ScanProfile << endl;
		gMrbLog->Flush(this->ClassName(), "SetScanProfile");
		return(kFALSE);
	}
	return(this->SetScanProfile(Channel, (ESnkScanProfile) kp->GetIndex()));
}

Bool_t TSnkDDA0816::SetScanProfile(Int_t Channel, ESnkScanProfile ScanProfile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetScanProfile
// Purpose:        Set scan profile
// Arguments:      Int_t Channel                  -- channel number (0 or 1)
//                 ESnkScanProfile ScanProfile    -- scan profile (triangle, sawtooth left/right)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets scan profile.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetScanProfile")) == NULL) return(kFALSE);

	chn->SetScanProfile(ScanProfile);
	return(kTRUE);
}

TMrbNamedX * TSnkDDA0816::GetScanProfile(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetScanProfile
// Purpose:        Return scan profile
// Arguments:      Int_t Channel            -- channel number (0 or 1)
// Results:        TMrbNamedX * ScanProfile    -- scan profile
// Exceptions:     
// Description:    Returns scan profile.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "GetScanProfile")) == NULL) return(NULL);

	return(fLofScanProfiles.FindByIndex(chn->GetScanProfile()));
}

Bool_t TSnkDDA0816::SetCurve(Int_t Channel, TArrayI & CurveData, Int_t NofDataPoints) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurve
// Purpose:        Set curve data
// Arguments:      Int_t Channel            -- channel number (0 or 1)
//                 TArrayI & Data           -- curve data
//                 Int_t NofDataPoints      -- size of data array
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills the curve buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;

	if ((chn = this->CheckChannel(Channel, this->ClassName(), "SetCurve")) == NULL) return(kFALSE);

	chn->SetCurve(CurveData, NofDataPoints);
	return(kTRUE);
}

void TSnkDDA0816::Print(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::Print
// Purpose:        Output current settings
// Arguments:      Int_t Channel           -- channel number (0, 1 or -1)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Output settings to cout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;

	TSnkDDAChannel * chn;

	TMrbLofNamedX lofExecModes;
	TString str;

	lofExecModes.AddNamedX(kSnkLofExecModes);
	lofExecModes.SetPatternMode();

	if (Channel == -1) {
		fDacX.Print();
		fDacY.Print();

		cout	<< this->ClassName() << "::Print(): Common settings:" << endl
				<< "----------------------------------------------" << endl;

		cout	<< "Exec mode       : " << lofExecModes.Pattern2String(str, fExecMode, ":") << endl;

		cout	<< "PreScaler       : ";
		if (fPreScale == kSnkPSUndef) {
			cout	<< setred << "undef"
					<< setblack << endl;
		} else {
			kp = fLofPreScales.FindByIndex(fPreScale);
			cout	<< (kp ? kp->GetName() : "undef") << endl;
		}

		cout	<< "ClockSource     : ";
		if (fClockSource == kSnkCSUndef) {
			cout	<< setred << "undef"
					<< setblack << endl;
		} else {
			kp = fLofClockSources.FindByIndex(fClockSource);
			cout	<< (kp ? kp->GetName() : "undef") << endl;
		}

		cout	<< "Pacer Clock     : " << fPacerClock << endl;

		cout	<< "Scan mode       : ";
		if (fScanMode == kSnkSMUndef) {
			cout	<< setred << "undef"
					<< setblack << endl;
		} else {
			kp = fLofScanModes.FindByIndex(fScanMode);
			cout	<< (kp ? kp->GetName() : "undef") << endl;
		}

		if (fEnergy > 0.) {
			cout	<< endl
					<< this->ClassName() << "::Print(): Calibration:" << endl
					<< "----------------------------------------------" << endl;
			cout	<< "Charge state     : " << fChargeState << endl;
			cout	<< "Energy     [MeV] : " << fEnergy << endl;
			cout	<< "Calib const      : X: " << this->GetCalibration(0)
												<< "	Y: " << this->GetCalibration(1) << endl;
			cout	<< "Max voltage [kV] : X: " << this->GetVoltage(0)
												<< "	Y: " << this->GetVoltage(1) << endl;
			cout	<< "Step width  [um] : X: " << this->GetStepWidth(0)
												<< "	Y: " << this->GetStepWidth(1) << endl;
			cout	<< "Range       [um] : X: " << this->GetRange(0)
												<< "	Y: " << this->GetRange(1) << endl << endl;
		}
 	} else if ((chn = this->CheckChannel(Channel, this->ClassName(), "Print")) != NULL) chn->Print();
}

Bool_t TSnkDDA0816::SaveAs(const Char_t * FileName, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SaveAs
// Purpose:        Save setup
// Arguments:      Char_t * FileName  -- file name
//                 Char_t * Prefix    -- prefix to be used in .dda file
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Writes TSnkDDA0816 object to root or dda file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString saveFile;
	saveFile = FileName;
	return(this->Save(saveFile.Data(), Prefix));
}

Bool_t TSnkDDA0816::SaveDefault(Bool_t SaveAsDDA, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SaveDefault
// Purpose:        Save setup using default name
// Arguments:      Bool_t SaveAsDDA     -- save as .dda file if kTRUE
//                 Char_t * Prefix      -- prefix to be used in .dda file
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Writes TSnkDDA0816 object to root or dda file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fSaveScan = SaveAsDDA ? "*.dda" : "*.root";
	return(this->Save(fSaveScan.Data(), Prefix));
}

Bool_t TSnkDDA0816::Save(const Char_t * SaveFile, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::Save
// Purpose:        Save setup
// Arguments:      Char_t * SaveFile   -- file name
//                 Char_t * Prefix     -- prefix to be used in .dda file
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Writes TSnkDDA0816 object to root or dda file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString title;
	TString saveFile;
	TFile * save;
	
	title = "DDA0816";
	saveFile = SaveFile;

	if (saveFile.Index("*.", 0) == 0) {
		Bool_t saveAsDDA = (saveFile.Index(".dda", 0) == (saveFile.Length() - 4));
		switch (fScanMode) {
			case kSnkSMSymm:		saveFile = "DDA_Symm";
									title += ": Symmetric scan";
									break;
			case kSnkSMAsymm:		saveFile = "DDA_Asymm";
									title += ": Asymmetric scan";
									break;
			case kSnkSMLShape:		saveFile = "DDA_LShape";
									title += ": L-shaped scan";
									break;
			case kSnkSMXOnly:		saveFile = "DDA_XOnly";
									title += ": Scan along X, Y=const";
									break;
			case kSnkSMYOnly:		saveFile = "DDA_YOnly";
									title += ": Scan along Y, X=const";
									break;
			case kSnkSMXYConst:		saveFile = "DDA_XYConst";
									title += ": X=const, Y=const";
									break;
			case kSnkSMFromFile:	saveFile = "DDA_FromFile";
									title += ": Data from file";
									break;
			default:				saveFile = "DDA0816"; break;
		}
		if (fScanMode != kSnkSMLShape) {
			switch (fDacX.GetScanProfile()) {
				case kSnkSPTriangle:	saveFile += "_TR";
										title += ", 0: triangular";
										break;
				case kSnkSPSawToothLR:	saveFile += "_STL";
										title += ", 0: sawtooth left to right";
										break;
				case kSnkSPSawToothRL:	saveFile += "_STR";
										title += ", 0: sawtooth right to left";
										break;
			}
			switch (fDacY.GetScanProfile()) {
				case kSnkSPTriangle:	saveFile += "_TR";
										title += ", 1: triangular";
										break;
				case kSnkSPSawToothLR:	saveFile += "_STL";
										title += ", 1: sawtooth left to right";
										break;
				case kSnkSPSawToothRL:	saveFile += "_STR";
										title += ", 1: sawtooth right to left";
										break;
			}
		}
		saveFile += "_";
		saveFile += this->GetSubdeviceName();
		saveFile += saveAsDDA ? ".dda" : ".root";
	}

	this->SetTitle(title.Data());

	Bool_t saveAsDDA = (saveFile.Index(".dda", 0) == (saveFile.Length() - 4));
	Bool_t saveAsRoot = (saveFile.Index(".root", 0) == (saveFile.Length() - 5));

	if (saveAsDDA) {
		if (!this->SaveAsDDAFile(saveFile.Data(), Prefix)) return(kFALSE);
	} else if (saveAsRoot) {
		save = new TFile(saveFile.Data(), "RECREATE");
		if (!save->IsOpen()) {
			gMrbLog->Err() << "Can't open file - " << saveFile << endl;
			gMrbLog->Flush(this->ClassName(), "Save");
			return(kFALSE);
		}
		this->Print();
		this->Write();
		save->Close();
	} else {
		gMrbLog->Err()	<< "Illegal file name - " << saveFile
						<< " (ext has to be \".dda\" or \".root\""<< endl;
		gMrbLog->Flush(this->ClassName() , "Save");
		return(kFALSE);
	}
	cout	<< setblue
			<< this->ClassName() << "::Save(): DDA0816 settings saved to file " << saveFile
			<< setblack << endl;
	fSaveScan = saveFile;
	return(kTRUE);
}

Bool_t TSnkDDA0816::SaveAsDDAFile(const Char_t * SaveFile, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SaveAsDDAFile
// Purpose:        Save DDA data as environment
// Arguments:      Char_t * SaveFile   -- file name
//                 Char_t * Prefix     -- prefix
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Writes TSnkDDA0816 data using environment (TMrbEnv) format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	TString saveFile;
	saveFile = SaveFile;
//	ux.GetBaseName(saveFile, SaveFile);
	TString cmd = "mv -f ";
	cmd += saveFile;
	cmd += " ";
	cmd += saveFile;
	cmd += ".bak 2>/dev/null";
	gSystem->Exec(cmd.Data());
	TMrbEnv * dda = new TMrbEnv(saveFile.Data());
	if (dda->IsZombie()) {
		gMrbLog->Err() << "Can't open file - " << saveFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveAsDDA");
		delete dda;
		return(kFALSE);
	}

	if (Prefix == NULL || *Prefix == '\0') Prefix = this->ClassName();
	dda->SetPrefix(Prefix);

// common settings
	dda->Set("Subdevice.Index", fSubdevice);
	dda->Set("Subdevice.Name", fSubdeviceName);
	dda->Set("PacerClock", fPacerClock);
	TMrbNamedX * nx = fLofPreScales.FindByIndex(fPreScale);
	dda->Set("PreScale", nx, 16);
	nx = fLofClockSources.FindByIndex(fClockSource);
	dda->Set("ClockSource", nx, 16);
	nx = fLofScanModes.FindByIndex(fScanMode);
	dda->Set("ScanMode", nx, 16);
	dda->Set("ScanPeriod", fScanInterval);
	dda->Set("Cycles", fCycleCount);
// calibration
	dda->Set("ChargeState", fChargeState);
	dda->Set("Energy", fEnergy);
// channel data
	TString chX = "XYHB";
	for (Int_t i = 0; i < kSnkNofChnPerSubdev; i++) {
		TSnkDDAChannel * chn = this->GetDac(i);
		TString prefix = "DDAControl.";
		TString chnName = "Dac";
		chnName += chX(i);
		prefix += chnName;
		dda->SetPrefix(prefix.Data());
		dda->Set("Name", chnName.Data());
		dda->Set("Channel", chn->GetChannel());
		dda->Set("Device", chn->GetDeviceName());
		dda->Set("Axis", chn->GetAxisName());
		dda->Set("NofPixels", chn->GetNofPixels());
		dda->Set("NofDataPoints", chn->GetNofDataPoints());
		if (this->IsXorY(i)) {
			dda->Set("SoftScale", chn->GetSoftScale());
			dda->Set("Offset", chn->GetOffset());
			dda->Set("Amplitude", chn->GetAmplitude());
			dda->Set("Increment", chn->GetIncrement());
			dda->Set("ScanProfile", fLofScanProfiles.FindByIndex(chn->GetScanProfile()), 16);
			dda->Set("Calibration", chn->GetCalibration(), 6);
			dda->Set("Voltage", chn->GetVoltage());
			dda->Set("StepWidth", chn->GetStepWidth());
			dda->Set("Range", chn->GetRange());
			dda->Set("StopAt", chn->GetStopPos());
			dda->Set("Pos0", chn->GetPos0());
		}
	}
// save data
	dda->Save(kFALSE);
	delete dda;
	return(kTRUE);
}
	
Bool_t TSnkDDA0816::SetCurveSymm(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveSymm
// Purpose:        Setup: symmetric scan
// Arguments:      Bool_t SaveFlag  -- kTRUE if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data for a symmetric scan. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	Int_t nofPixels;
	Int_t nofPoints;
	
//	number of data points
	nofPixels = fDacX.GetNofPixels();
	if (nofPixels <= 0) nofPixels = 512;
	fDacX.SetNofPixels(nofPixels);

// same values for dac0 and dac1

//	fDacY.SetOffset(fDacX.GetOffset());
	fDacY.SetScanProfile(fDacX.GetScanProfile());
	fDacY.SetNofPixels(fDacX.GetNofPixels());

	fDacY.SetAmplitude(fDacX.GetAmplitude());
	
	fScanMode = kSnkSMSymm;

	if (!this->CheckParams(this->ClassName(), "SetCurveSymm")) return(kFALSE);
	if (!this->AdjustSettings(0)) return(kFALSE);
	if (!this->AdjustSettings(1)) return(kFALSE);

// calc curve data for channels 0 and 1

	switch (fDacX.GetScanProfile()) {

		case kSnkSPTriangle:				// triangular shape
			nofPoints = this->SetTriangle(curveData,
								fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), nofPixels);
			this->SetCurve(kDacX, curveData, nofPoints);
			nofPoints = this->SetTriangle(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), nofPixels);
			this->SetCurve(kDacY, curveData, nofPoints);
			break;
		case kSnkSPSawToothLR:			// saw-tooth from left to right
			nofPoints = this->SetSawToothLR(curveData,
								fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), nofPixels);
			this->SetCurve(kDacX, curveData, nofPoints);
			nofPoints = this->SetSawToothLR(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), nofPixels);
			this->SetCurve(kDacY, curveData, nofPoints);
			break;
		case kSnkSPSawToothRL:			// saw-tooth from right to left
			nofPoints = this->SetSawToothRL(curveData,
								fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), nofPixels);
			this->SetCurve(kDacX, curveData, nofPoints);
			nofPoints = this->SetSawToothRL(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), nofPixels);
			this->SetCurve(kDacY, curveData, nofPoints);
			break;
		default:
			gMrbLog->Err() << "Illegal SCAN PROFILE - "
					<< this->GetScanProfile(0)->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveSymm");
			return(kFALSE);
	}

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveSymm(): Symmetric scan - setup finished"
			<< setblack << endl;

	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveAsymm(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveAsymm
// Purpose:        Setup: asymmetric scan
// Arguments:      Bool_t SaveFlag   -- kTRUE if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data for an asymmetric scan. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	Int_t nofPoints;

	fScanMode = kSnkSMAsymm;

	if (!this->CheckParams(this->ClassName(), "SetCurveAsymm")) return(kFALSE);
	if (!this->AdjustSettings(0)) return(kFALSE);
	if (!this->AdjustSettings(1)) return(kFALSE);

// number of data points
	if (fDacX.GetNofPixels() <= 0) fDacX.SetNofPixels(512);
	if (fDacY.GetNofPixels() <= 0) fDacY.SetNofPixels(512);

// calc curve data for channel 0

	switch (fDacX.GetScanProfile()) {

		case kSnkSPTriangle:				// triangular shape
			nofPoints = this->SetTriangle(curveData,
						fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		case kSnkSPSawToothLR:			// saw-tooth from left to right
			nofPoints = this->SetSawToothLR(curveData,
						fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		case kSnkSPSawToothRL:			// saw-tooth from right to left
			nofPoints = this->SetSawToothRL(curveData,
						fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		default:
			gMrbLog->Err() << "Illegal SCAN PROFILE for DAC0 - " << this->GetScanProfile(0)->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveAsymm");
			return(kFALSE);
	}

	this->SetCurve(kDacX, curveData, nofPoints);

// calc curve data for channel 1

	switch (fDacY.GetScanProfile()) {

		case kSnkSPTriangle:				// triangular shape
			nofPoints = this->SetTriangle(curveData,
						fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		case kSnkSPSawToothLR:			// saw-tooth from left to right
			nofPoints = this->SetSawToothLR(curveData,
						fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		case kSnkSPSawToothRL:			// saw-tooth from right to left
			nofPoints = this->SetSawToothRL(curveData,
						fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		default:
			gMrbLog->Err() << "Illegal SCAN PROFILE for DAC1 - "
					<< this->GetScanProfile(1)->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveAsymm");
			return(kFALSE);
	}

	this->SetCurve(kDacY, curveData, nofPoints);

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveAsymm(): Asymmetric scan - setup finished"
			<< setblack << endl;


	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveLShape(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveLShape
// Purpose:        Setup: asymmetric scan
// Arguments:      Bool_t SaveFlag   -- kTRUE if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data for an asymmetric scan. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	Int_t nofPoints;

	fScanMode = kSnkSMLShape;

	if (!this->CheckParams(this->ClassName(), "SetCurveLShape")) return(kFALSE);
	if (!this->AdjustSettings(0)) return(kFALSE);
	if (!this->AdjustSettings(1)) return(kFALSE);

// number of data points
	if (fDacX.GetNofPixels() <= 0) fDacX.SetNofPixels(512);
	if (fDacY.GetNofPixels() <= 0) fDacY.SetNofPixels(512);

// scan profile: like triangle

	fDacX.SetScanProfile(kSnkSPTriangle);
	fDacY.SetScanProfile(kSnkSPTriangle);

// calc curve data for channel 0

	nofPoints = this->SetLShape(curveData,	fDacX.GetOffset(),	fDacX.GetIncrement(),	fDacX.GetNofPixels(),
																0,						fDacY.GetNofPixels());
	this->SetCurve(kDacX, curveData, nofPoints);

// calc curve data for channel 1

	nofPoints = this->SetLShape(curveData,	fDacY.GetOffset(),	0,						fDacX.GetNofPixels(),
																fDacY.GetIncrement(),	fDacY.GetNofPixels());
	this->SetCurve(kDacY, curveData, nofPoints);

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveLShape(): L-shaped scan - setup finished"
			<< setblack << endl;

	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveXOnly(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveXOnly
// Purpose:        Setup: scan x axis only
// Arguments:      Bool_t SaveFlag  -- kTRUE if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data to scan along x. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	Int_t nofPoints;

// number of data points
	if (fDacX.GetNofPixels() <= 0) fDacX.SetNofPixels(512);
	fDacY.SetNofPixels(1);

//	ignore Y settings beside of Y-offset
	fDacY.SetAmplitude(0);
	fDacY.SetIncrement(0);
	fDacY.SetScanProfile(kSnkSPConstant);

	fScanMode = kSnkSMXOnly;

	this->SetSoftScale(1);			// simulate soft scaling to trigger hysterisis
	
	if (!this->CheckParams(this->ClassName(), "SetCurveXOnly")) return(kFALSE);
	if (!this->AdjustSettings(0)) return(kFALSE);

// calc curve data for channel 0

	switch (fDacX.GetScanProfile()) {

		case kSnkSPTriangle:				// triangular shape
			nofPoints = this->SetTriangle(curveData,
							fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		case kSnkSPSawToothLR:			// saw-tooth from left to right
			nofPoints = this->SetSawToothLR(curveData,
							fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		case kSnkSPSawToothRL:			// saw-tooth from right to left
			nofPoints = this->SetSawToothRL(curveData,
							fDacX.GetOffset(), fDacX.GetAmplitude(), fDacX.GetIncrement(), fDacX.GetNofPixels());
			break;
		default:
			gMrbLog->Err() << "Illegal SCAN PROFILE for DAC0 - "
					<< this->GetScanProfile(0)->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveXOnly");
			return(kFALSE);
	}

	this->SetCurve(kDacX, curveData, nofPoints);

// calc curve data for channels 1

	curveData.Set(fMaxPoints);
	curveData.Reset();
	curveData.Set(fDacY.GetNofPixels());

	curveData[0] = fDacY.GetOffset();

	this->SetCurve(kDacY, curveData, fDacY.GetNofPixels());

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveXOnly(): Scan along X - setup finished"
			<< setblack << endl;

	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveYOnly(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveYOnly
// Purpose:        Setup: scan y axis only
// Arguments:      Bool_t SaveFlag  -- if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data to scan along y. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	Int_t nofPoints;

// number of data points
	fDacX.SetNofPixels(1);
	if (fDacY.GetNofPixels() <= 0) fDacY.SetNofPixels(512);

//	ignore X settings beside of X-offset
	fDacX.SetAmplitude(0);
	fDacX.SetIncrement(0);
	fDacX.SetScanProfile(kSnkSPConstant);

	fScanMode = kSnkSMYOnly;

	this->SetSoftScale(0);			// simulate soft scaling to trigger hysterisis
	
	if (!this->CheckParams(this->ClassName(), "SetCurveYOnly")) return(kFALSE);
	if (!this->AdjustSettings(1)) return(kFALSE);

// calc curve data for channel 0

	curveData.Set(fMaxPoints);
	curveData.Reset();
	curveData.Set(fDacX.GetNofPixels());

	curveData[0] = fDacX.GetOffset();

	this->SetCurve(kDacX, curveData, fDacX.GetNofPixels());

// calc curve data for channels 1

	switch (fDacY.GetScanProfile()) {

		case kSnkSPTriangle:				// triangular shape
			nofPoints = this->SetTriangle(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		case kSnkSPSawToothLR:			// saw-tooth from left to right
			nofPoints = this->SetSawToothLR(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		case kSnkSPSawToothRL:			// saw-tooth from right to left
			nofPoints = this->SetSawToothRL(curveData,
								fDacY.GetOffset(), fDacY.GetAmplitude(), fDacY.GetIncrement(), fDacY.GetNofPixels());
			break;
		default:
			gMrbLog->Err() << "Illegal SCAN PROFILE for DAC1 - "
					<< this->GetScanProfile(1)->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveXOnly");
			return(kFALSE);
	}

	this->SetCurve(kDacY, curveData, nofPoints);

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveYOnly(): Scan along Y - setup finished"
			<< setblack << endl;

	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveXYConst(Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveXYConst
// Purpose:        Setup: x = const, y = const
// Arguments:      Bool_t SaveFlag  -- if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates curve data for x = const and y = const
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;

	fDacX.SetScanProfile(kSnkSPConstant);
	fDacX.SetAmplitude(0);
	fDacX.SetIncrement(0);
	fDacX.SetNofPixels(1);

	fDacY.SetScanProfile(kSnkSPConstant);
	fDacY.SetAmplitude(0);
	fDacY.SetIncrement(0);
	fDacY.SetNofPixels(1);

	fScanMode = kSnkSMXYConst;

	if (!this->CheckParams(this->ClassName(), "SetCurveXYConst")) return(kFALSE);

// calc curve data for channel 0

	curveData.Set(fMaxPoints);
	curveData.Reset();
	curveData.Set(fDacX.GetNofPixels());
	curveData[0] = fDacX.GetOffset();

	this->SetCurve(kDacX, curveData, fDacX.GetNofPixels());

// calc curve data for channel 1

	curveData.Set(fMaxPoints);
	curveData.Reset();
	curveData.Set(fDacY.GetNofPixels());
	curveData[0] = fDacY.GetOffset();

	this->SetCurve(kDacY, curveData, fDacY.GetNofPixels());

	this->SetHysteresis();
	this->SetBeamOn();

	cout	<< setblue
			<< this->ClassName() << "::SetCurveXYConst(): Scan X=const, Y=const - setup finished"
			<< setblack << endl;

	if (SaveFlag) this->SaveDefault();

	return(kTRUE);
}

Bool_t TSnkDDA0816::SetCurveFromFile(const Char_t * FileName, Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetCurveFromFile
// Purpose:        Setup: data taken from file
// Arguments:      Char_t * FileName     -- file name
//                 Bool_t SaveFlag       -- kTRUE if data should be saved
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reads data from external file
//                 Data format is
//                        <xval> <yval> <intensity> <beam>
//                 Data fields separated by white space (blank and/or tab>
//                 <intensity>=1,2,... means "stay <intensity> time slices on same place" (default=1)
//                 <beam>=1 means "beam on", =0 "beam off" (default=1)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	ifstream f;
	TString line;
	TString intStr, dblStr;
	TObjArray * items;
	TArrayD xValue;
	TArrayD yValue;
	TArrayI bValue;
	TArrayI curveData;
	Double_t xVal, yVal, xPrev, yPrev;
	Int_t intens;
	Int_t beam;
	Int_t nofPoints;
	Int_t lineNo, steps;
	Int_t nofErrors;
	Int_t xOffset, yOffset;

	xOffset = fDacX.GetOffset();
	yOffset = fDacY.GetOffset();

	f.open(FileName, ios::in);
	if (!f.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << FileName << endl;
		gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
		return(kFALSE);
	}

	xValue.Set(fMaxPoints);
	xValue.Reset();
	yValue.Set(fMaxPoints);
	yValue.Reset();
	bValue.Set(fMaxPoints);
	bValue.Reset();

	nofPoints = 0;
	lineNo = 0;
	steps = 0;
	nofErrors = 0;
	while (!f.eof()) {
		line.ReadLine(f, kFALSE);
		lineNo++;
		line = line.Strip(TString::kBoth);
		if (line.Length() == 0 || line(0) == '#') continue;
		items = line.Tokenize(" \t");
		Int_t n = items->GetEntries();
		if (n > 4 || n < 2) {
			gMrbLog->Err()	<< FileName << " (line " << lineNo
							<< "): Illegal format - >> " << line << " <<" << endl;
			gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
			nofErrors++;
		} else {
			dblStr = ((TObjString *) items->At(0))->GetString();
			xVal = dblStr.Atof();
			Int_t iVal = this->Dist2Ampl(kDacX, xVal) + xOffset;
			if (iVal < 0 || iVal > kSnkMaxRange) {
				gMrbLog->Err()	<< FileName << " (line " << lineNo
								<< "): X value out of range - " << xVal
								<< " -> Ampl=" << iVal
								<< " (should be in [0," << kSnkMaxRange << "])" << endl;
				gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
				nofErrors++;
			}
			dblStr = ((TObjString *) items->At(1))->GetString();
			yVal = dblStr.Atof();
			iVal = this->Dist2Ampl(kDacY, yVal) + yOffset;
			if (iVal < 0 || iVal > kSnkMaxRange) {
				gMrbLog->Err()	<< FileName << " (line " << lineNo
								<< "): Y value out of range - " << xVal
								<< " -> Ampl=" << iVal
								<< " (should be in [0," << kSnkMaxRange << "])" << endl;
				gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
				nofErrors++;
			}
			if (n > 2) {
				intStr = ((TObjString *) items->At(2))->GetString();
				intens = intStr.Atoi();
				if (n == 4) {
					intStr = ((TObjString *) items->At(3))->GetString();
					beam = intStr.Atoi();
				} else {
					beam = 1;
				}
			} else {
				intens = 1;
				beam = 1;
			}
			n = intens; 								// stay <intens> times on same position
			if (beam == 0) n += 2;						// +2? 2 steps needed to change position with beam off
			if (n + nofPoints + 2 > fMaxPoints) {	// +2? 2 steps needed to return to [0,0] on stop
				gMrbLog->Err()	<< FileName << " (line " << lineNo
								<< "): Too many data points - max " << fMaxPoints << endl;
				gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
				break;
			}
			if (nofPoints == 0)	{					// move from [0,0] to 1st point
				xValue[0] = 0.0;
				yValue[0] = 0.0;
				bValue[0] = kSnkBeamOff;
				xValue[1] = xVal;
				yValue[1] = yVal;
				bValue[1] = kSnkBeamOff;
				nofPoints = 2;
			} else if (beam == 0) {
				xValue[nofPoints] = xPrev;			// don't move, switch beam off
				yValue[nofPoints] = yPrev;
				bValue[nofPoints] = kSnkBeamOff;
				nofPoints++;
				xValue[nofPoints] = xVal;			// then move to new position
				yValue[nofPoints] = yVal;
				bValue[nofPoints] = kSnkBeamOff;
				nofPoints++;
			}
			for (Int_t i = 0; i < intens; i++) {	// stay at this position for <intens> time slices
				xValue[nofPoints] = xVal;
				yValue[nofPoints] = yVal;
				bValue[nofPoints] = kSnkBeamOn;
				nofPoints++;
			}
			xPrev = xVal;
			yPrev = yVal;
			steps++;
		}
		delete items;
	}
	xValue[nofPoints] = xPrev;						// move to [0,0] at end of scan
	yValue[nofPoints] = yPrev;
	bValue[nofPoints] = kSnkBeamOff;
	nofPoints++;
	xValue[nofPoints] = 0.0;
	yValue[nofPoints] = 0.0;
	bValue[nofPoints] = kSnkBeamOff;
	nofPoints++;

	f.close();

	if (nofErrors > 0) {
		gMrbLog->Err()	<< FileName << ": " << nofErrors << " error(s), no curve data written" << endl;
		gMrbLog->Flush(this->ClassName(), "SetCurveFromFile");
		return(kFALSE);
	} else {
		curveData.Set(nofPoints);
		curveData.Reset();
		fDacX.SetNofPixels(nofPoints);
		for (Int_t i = 0; i < nofPoints; i++) curveData[i] = this->Dist2Ampl(kDacX, xValue[i]) + xOffset;
		this->SetCurve(kDacX, curveData, nofPoints);

		curveData.Reset();
		fDacY.SetNofPixels(nofPoints);
		for (Int_t i = 0; i < nofPoints; i++) curveData[i] = this->Dist2Ampl(kDacY, yValue[i]) + yOffset;
		this->SetCurve(kDacY, curveData, nofPoints);

		curveData.Reset();
		fDacB.SetNofPixels(nofPoints);
		fDacB.SetSoftScale(0);
		for (Int_t i = 0; i < nofPoints; i++) curveData[i] = bValue[i];
		this->SetCurve(kDacB, curveData, nofPoints);

		curveData.Reset();
		fDacH.SetNofPixels(1);
		fDacH.SetSoftScale(0);
		curveData[0] = kSnkHystLow;
		this->SetCurve(kDacH, curveData, 1);

		gMrbLog->Out()	<< FileName << ": " << steps << " step(s) -> " << nofPoints << " data point(s)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetCurveFromFile", setblue);
		return(kTRUE);
	}
}

Bool_t TSnkDDA0816::ConnectToServer(const Char_t * ServerName, Int_t Port, const Char_t * ServerProg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::ConnectToServer
// Purpose:        Connect to server
// Arguments:      Char_t * ServerName    -- server addr
//                 Int_t Port             -- port number
//                 Char_t * ServerProg    -- program to be started
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Establishes a server connection.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSocket * s;
	TMessage * msg;
	Int_t lmsg;
	TString cmd;
	TSnkDDAMessage * ddaMsg;
	ofstream cmdFile;
	
	if (this->IsConnected()) return(kTRUE);
	
	fServer = ServerName;
	fPort = Port;
	
	s = new TSocket(fServer.Data(), fPort);
	if ((lmsg = s->Recv(msg)) == -1 && ServerProg != NULL) {
		delete s;
		cmdFile.open("DDA.sh", ios::out);
		cmdFile << "export ROOTSYS=" << gSystem->Getenv("ROOTSYS") << endl;
		cmdFile << "export MARABOU=" << gSystem->Getenv("MARABOU") << endl;
		cmdFile << "export LD_LIBRARY_PATH=" << gSystem->Getenv("LD_LIBRARY_PATH") << endl;
		cmdFile << "cd " << gSystem->WorkingDirectory() << endl;
		TMrbSystem ux;
		TString srv;
		ux.Which(srv, gSystem->Getenv("PATH"), ServerProg);
		cmd += srv;
		cmd += " ";
		cmd += fPort;
		cmdFile << cmd << endl;
		cmdFile.close();
		gSystem->Exec("chmod +x DDA.sh");
		if (fServer.Length() != 0 && fServer.CompareTo("localhost") != 0) {
			cmd = "xterm -title ";
			cmd += ServerProg;
			cmd += "@";
			cmd += fServer;
			cmd += " -e ssh ";
			cmd += fServer;
			cmd += " ";
		} else {
			cmd = "xterm -title ";
			cmd += ServerProg;
			cmd += " -e ";
		}
		cmd += gSystem->WorkingDirectory();
		cmd += "/DDA.sh&";
		cout << "@@@ " << cmd << endl;

		gMrbLog->Out()	<< "Trying to connect to server " << fServer << ":" << fPort
						<< " (progr " << ServerProg << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToServer");
		gSystem->Exec(cmd.Data());
		lmsg = -1;
		cout	<< "Wait " << flush;
		for (Int_t i = 0; i < 5; i++) {
			sleep(1);
			cout << "." << flush;
		}
		for (Int_t i = 0; i < kSnkWaitForServer; i++) {
			sleep(1);
			cout << "." << flush;
			s = new TSocket(fServer.Data(), fPort);
			if ((lmsg = s->Recv(msg)) > 0) break;
			delete s;
		}
	}
	if (lmsg > 0) {
		cout << " done." << endl;
	} else {
		cout << endl;
		gMrbLog->Err()	<< "Can't connect to server " << fServer << ":" << fPort << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToServer");
		return(kFALSE);
	}
	
	if (msg->What() == kMESS_STRING) {
		Char_t str[256];
		msg->ReadString(str, 256);
		gMrbLog->Out()	<< "Got message: " << str << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToServer");
	} else if (msg->What() == kMESS_OBJECT) {
		ddaMsg = (TSnkDDAMessage *) msg->ReadObject(msg->GetClass());
		gMrbLog->Out()	<< "Got TSnkDDAMessage object:" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToServer");
		if (this->IsDebug()) ddaMsg->Print();
	}
	fSocket = s;
	return(kTRUE);
}
							
Bool_t TSnkDDA0816::StartScan(Double_t ScanInterval, Int_t NofCycles) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::StartScan
// Purpose:        Start actual scan
// Arguments:      Double_t ScanInterval    -- scan time in seconds
//                 Int_t NofCycles          -- number of cycles
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Starts actual scanning.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAMessage ddaMsg;
	TString str;

	if (!this->CheckParams(this->ClassName(), "StartScan")) return(kFALSE);

	if (fScanMode == kSnkSMUndef) {
		gMrbLog->Err() << "No scan defined" << endl;
		gMrbLog->Flush(this->ClassName(), "StartScan");
		return(kFALSE);
	}
	
	if (!this->CheckConnect(this->ClassName(), "StartScan")) return(kFALSE);
	
	if (NofCycles > 0) ScanInterval = 0.;

	ddaMsg.Reset();
	ddaMsg.fText = " ";
	ddaMsg.fProgram = this->ClassName();
	ddaMsg.fAction = "StartScan";
	ddaMsg.fExecMode = fLofExecModes.Pattern2String(str, fExecMode);
	ddaMsg.fInterval = ScanInterval;
	ddaMsg.fCycles = NofCycles;
	ddaMsg.fPath = gSystem->WorkingDirectory();
	ddaMsg.fSaveScan = "DDA_ScanData.root";
	fSocket->SendObject(&ddaMsg);
	if (this->IsDebug()) ddaMsg.Print();
	return(kTRUE);
}

Bool_t TSnkDDA0816::StopScan() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::StopScan
// Purpose:        Stop scan
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stops scan.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAMessage ddaMsg;
	TString str;

	if (!this->IsConnected()) return(kFALSE);

	ddaMsg.Reset();
	ddaMsg.fText = " ";
	ddaMsg.fProgram = this->ClassName();
	ddaMsg.fAction = "StopScan";
	ddaMsg.fExecMode = fLofExecModes.Pattern2String(str, fExecMode);
	ddaMsg.fInterval = 0.;
	ddaMsg.fPath = gSystem->WorkingDirectory();
	ddaMsg.fSaveScan = " ";
	if (this->IsVerbose()) ddaMsg.Print();
	fSocket->SendObject(&ddaMsg);
	return(kTRUE);
}

Bool_t TSnkDDA0816::KillServer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::KillServer
// Purpose:        Stop server program
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stops server program.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAMessage ddaMsg;
	TString str;

	if (!this->IsConnected()) return(kFALSE);

	ddaMsg.Reset();
	ddaMsg.fText = " ";
	ddaMsg.fProgram = this->ClassName();
	ddaMsg.fAction = "KillServer";
	ddaMsg.fExecMode = fLofExecModes.Pattern2String(str, fExecMode);
	ddaMsg.fInterval = 0;
	ddaMsg.fPath = gSystem->WorkingDirectory();
	ddaMsg.fSaveScan = " ";
	if (this->IsVerbose()) ddaMsg.Print();
	fSocket->SendObject(&ddaMsg);
	fSocket = NULL;
	return(kTRUE);
}

Bool_t TSnkDDA0816::ResetScan() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::ResetScan
// Purpose:        Reset to x = 0, y = 0
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Resets scan.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;

// calc curve data for channel 0

	this->SetScanProfile(kDacX, kSnkSPConstant);
	this->SetOffset(kDacX, kSnkOffset0);
	this->SetAmplitude(kDacX, 0);
	this->SetIncrement(kDacX, 0);

	this->SetScanProfile(kDacY, kSnkSPConstant);
	this->SetOffset(kDacY, kSnkOffset0);
	this->SetAmplitude(kDacY, 0);
	this->SetIncrement(kDacY, 0);

	curveData.Set(fMaxPoints);
	curveData.Reset();
	curveData.Set(1);

	fDacX.SetNofPixels(1);
	fDacY.SetNofPixels(1);
	curveData[0] = fDacX.GetOffset();

	this->SetCurve(kDacX, curveData, 1);
	this->SetCurve(kDacY, curveData, 1);
	this->SetBeamOn();

	fScanDataFile = "";

	fScanMode = kSnkSMXYConst;
	this->SaveDefault();

	cout	<< setblue
			<< this->ClassName() << "::ResetScan(): Resetting to X=0, Y=0"
			<< setblack << endl;

	this->StartScan(1);

	return(kTRUE);
}

Int_t TSnkDDA0816::SetTriangle(TArrayI & Data, Int_t Offset, Int_t Amplitude, Int_t Increment, Int_t NofPixels) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetTriangle
// Purpose:        Setup a triangular shape
// Arguments:      TArrayI & Data        -- array to store data
//                 Int_t Offset          -- offset
//                 Int_t Amplitude       -- amplitude
//                 Int_t Increment       -- increment
//                 Int_t NofPixels       -- number of pixels
// Results:        Int_t NofDataPoints   -- actual number of data points
// Exceptions:     
// Description:    Calculates a triangular shape.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t n;
	Int_t * pData;

	Data.Set(fMaxPoints);
	Data.Reset();
	Data.Set(2 * NofPixels);

	n = Offset - Amplitude;
	pData = Data.GetArray();
	for (i = 0; i < NofPixels; i++, n += Increment) *pData++ = n;
	n -= Increment;
	for (i = 0; i < NofPixels; i++, n -= Increment) *pData++ = n;
	return(2 * NofPixels);
}

Int_t TSnkDDA0816::SetSawToothLR(TArrayI & Data, Int_t Offset, Int_t Amplitude, Int_t Increment, Int_t NofPixels) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetSawToothLR
// Purpose:        Setup a saw-tooth from left to right
// Arguments:      TArrayI & Data        -- array to store data
//                 Int_t Offset          -- offset
//                 Int_t Amplitude       -- amplitude
//                 Int_t Increment       -- increment
//                 Int_t NofPixels       -- number of pixels
// Results:        Int_t NofDataPoints   -- actual number of data points
// Exceptions:     
// Description:    Calculates a saw-tooth shape.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t n;
	Int_t * pData;

	Data.Set(fMaxPoints);
	Data.Reset();
	Data.Set(NofPixels);

	n = Offset - Amplitude;
	pData = Data.GetArray();
	for (i = 0; i < NofPixels; i++, n += Increment) *pData++ = n;
	return(NofPixels);
}

Int_t TSnkDDA0816::SetSawToothRL(TArrayI & Data, Int_t Offset, Int_t Amplitude, Int_t Increment, Int_t NofPixels) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetSawToothRL
// Purpose:        Setup a saw-tooth from right to left
// Arguments:      TArrayI & Data        -- array to store data
//                 Int_t Offset          -- offset
//                 Int_t Amplitude       -- amplitude
//                 Int_t Increment       -- increment
//                 Int_t NofPixels       -- number of pixels
// Results:        Int_t NofDataPoints   -- actual number of data points
// Exceptions:     
// Description:    Calculates a saw-tooth shape.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t n;
	Int_t * pData;

	Data.Set(fMaxPoints);
	Data.Reset();
	Data.Set(NofPixels);

	n = Offset + Amplitude;
	pData = Data.GetArray();
	for (i = 0; i < NofPixels; i++, n -= Increment) *pData++ = n;
	return(NofPixels);
}

Int_t TSnkDDA0816::SetLShape(TArrayI & Data, Int_t Offset,	Int_t Increment1, Int_t NofPixels1,
															Int_t Increment2, Int_t NofPixels2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetTriangle
// Purpose:        Setup a triangular shape
// Arguments:      TArrayI & Data        -- array to store data
//                 Int_t Offset          -- offset
//                 Int_t Increment1      -- increment (axis 1)
//                 Int_t NofPixels1      -- number of pixels (axis 1)
//                 Int_t Increment2      -- increment (axis 2)
//                 Int_t NofPixels2      -- number of pixels (axis 2)
// Results:        Int_t NofDataPoints   -- actual number of data points
// Exceptions:     
// Description:    Calculates a L-shaped curve.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t n;
	Int_t * pData;
	Int_t nData;
	
	nData = 2 * (NofPixels1 + NofPixels2);
	Data.Set(fMaxPoints);
	Data.Reset();
	Data.Set(nData);

	n = Offset;
	pData = Data.GetArray();
	if (Increment1 != 0) {
		for (i = 0; i < NofPixels1; i++, n += Increment1) *pData++ = n;
		n -= Increment1;
		for (i = 0; i < NofPixels1; i++, n -= Increment1) *pData++ = n;
		for (i = 0; i < 2 * NofPixels2; i++) *pData++ = Offset;
	} else {
		for (i = 0; i < 2 * NofPixels1; i++) *pData++ = Offset;
		for (i = 0; i < NofPixels2; i++, n += Increment2) *pData++ = n;
		n -= Increment2;
		for (i = 0; i < NofPixels2; i++, n -= Increment2) *pData++ = n;
	}

	return(nData);
}

Bool_t TSnkDDA0816::SetHysteresis() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetHysteresis
// Purpose:        Calculate hysteresis output for channel 2
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets curve data according to slope in dac0 or dac1.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TSnkDDAChannel * chn;
	TArrayI curveData;
	Int_t i;
	Int_t n;
	Int_t nData;
	Int_t softScale;
	Int_t npx, npy;
		
	softScale = -1;

	curveData.Set(fMaxPoints);
	curveData.Reset();

	if (fScanMode == kSnkSMLShape) {
		npx = fDacX.GetNofPixels();
		npy = fDacY.GetNofPixels();
		if (npx == npy) {
			curveData[0] = kSnkHystLow;
			curveData[1] = kSnkHystHigh;
			curveData[2] = kSnkHystHigh;
			curveData[3] = kSnkHystLow;
			this->SetCurve(2, curveData, 4);
			fDacH.SetNofPixels(4);
			fDacH.SetSoftScale(npx - 1);
		} else {
			nData = 2 * (npx + npy);
			n = 0;
			for (i = 0; i < npx; i++, n++) curveData[n] = kSnkHystLow;
			for (i = 0; i < npx; i++, n++) curveData[n] = kSnkHystHigh;
			for (i = 0; i < npy; i++, n++) curveData[n] = kSnkHystHigh;
			for (i = 0; i < npy; i++, n++) curveData[n] = kSnkHystLow;
			this->SetCurve(2, curveData, nData);
			fDacH.SetNofPixels(nData);
			fDacH.SetSoftScale(0);
		}
	} else {
		chn = NULL;
		if (fDacX.GetSoftScale() != 0 || fDacY.GetSoftScale() != 0) {
			if (fDacX.GetSoftScale() == 0) {
				chn = &fDacX;
				softScale = fDacY.GetSoftScale();
			} else if (fDacY.GetSoftScale() == 0) {
				chn = &fDacY;
				softScale = fDacX.GetSoftScale();
			}
			if (chn != NULL) {
				if (chn->GetScanProfile() != kSnkSPTriangle) chn = NULL;
			}
		}
	
		if (chn == NULL) {
			curveData[0] = kSnkHystLow;
			this->SetCurve(2, curveData, 1);
			fDacH.SetNofPixels(1);
			fDacH.SetSoftScale(0);
			return(kTRUE);
		}

		curveData.Reset();
		curveData.Set(2);
		curveData[0] = kSnkHystLow;
		curveData[1] = kSnkHystHigh;
	
		this->SetCurve(kDacH, curveData, 2);
		fDacH.SetNofPixels(2);
		fDacH.SetSoftScale(softScale);
	}
	return(kTRUE);
}

Bool_t TSnkDDA0816::SetBeamOn() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetBeamOn
// Purpose:        Set beam permanently on
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets curve data for channel 3.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI curveData;
	curveData.Set(1);
	curveData.Reset();
	curveData[0] = kSnkBeamOn;
	this->SetCurve(kDacB, curveData, 1);
	fDacB.SetNofPixels(1);
	fDacB.SetSoftScale(0);
	return(kTRUE);
}

const Char_t * TSnkDDA0816::GetSubdeviceName() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::GetSubdeviceName
// Purpose:        Return subdevice
// Arguments:      --
// Results:        Char_t * Subdevice  -- subdevice name
// Exceptions:     
// Description:    Returns subdevice as ascii string.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fSubdeviceName = "ABCD"[fSubdevice];
	return(fSubdeviceName.Data());
}

Bool_t TSnkDDA0816::Calibrate(Int_t ChargeState, Double_t Energy,
						Double_t CalibX, Double_t VoltageX, Double_t CalibY, Double_t VoltageY) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::Calibrate
// Purpose:        Calibrate amplitude
// Arguments:      Int_t ChargeState      -- charge number
//                 Double_t Energy     -- beam energy [MeV]
//                 Double_t CalibX     -- calibration X
//                 Double_t VoltageX   -- max voltage power supply X [kV]
//                 Double_t CalibY     -- calibration Y
//                 Double_t VoltageY   -- max voltage power supply Y [kV]
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates calibration constants: step width, range.
//                  
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t stepWidth;

	if (Energy <= 0.) {
		this->ResetCalibration();
		return(kFALSE);
	}

	fChargeState = ChargeState;
	fEnergy = Energy;

	fDacX.SetCalibration(CalibX);
	fDacX.SetVoltage(VoltageX);
	stepWidth = CalibX * ChargeState * VoltageX / Energy;
	fDacX.SetStepWidth(stepWidth);
	fDacX.SetRange(stepWidth * kSnkMaxAmplitude);

	fDacY.SetCalibration(CalibY);
	fDacY.SetVoltage(VoltageY);
	stepWidth = CalibY * ChargeState * VoltageY / Energy;
	fDacY.SetStepWidth(stepWidth);
	fDacY.SetRange(stepWidth * kSnkMaxAmplitude);
	return(kTRUE);
}

Int_t TSnkDDA0816::Dist2Ampl(Int_t XorY, Double_t Distance) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::Dist2Ampl
// Purpose:        Distance to amplitude conversion
// Arguments:      Int_t XorY          -- X(0) or Y(1)
//                 Double_t Distance   -- distance [um]
// Results:        Int_t Amplitude     -- amplitude [steps]
// Exceptions:     
// Description:    Converts distance to amplitude.
//                  
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t a;

	if (this->IsXorY(XorY)) {
		a = Distance / this->GetStepWidth(XorY);
		if (a < 0.) a -= .5; else a += .5;
		return((Int_t) a);
	} else {
		gMrbLog->Err() << "Wrong index - " << XorY << " (should be 0(X) or 1(Y))" << endl;
		gMrbLog->Flush(this->ClassName(), "Dist2Ampl");
		return(0);
	}
}

Double_t TSnkDDA0816::Ampl2Dist(Int_t XorY, Int_t Amplitude) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::Ampl2Dist
// Purpose:        Amplitude to distance conversion
// Arguments:      Int_t XorY          -- X(0) or Y(1)
//                 Int_t Amplitude     -- amplitude [steps]
// Results:        Double_t Distance   -- distance [um]
// Exceptions:     
// Description:    Converts amplitude to distance.
//                  
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (this->IsXorY(XorY)) {
		return(Amplitude * this->GetStepWidth(XorY));
	} else {
		gMrbLog->Err() << "Wrong index - " << XorY << " (should be 0(X) or 1(Y))" << endl;
		gMrbLog->Flush(this->ClassName(), "Ampl2Dist");
		return(0.);
	}
}

void TSnkDDA0816::ResetCalibration() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::ResetCalibration
// Purpose:        Reset calibration
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Resets calibration to default values.
//                  
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fChargeState = 0;
	fEnergy = 0.;

	fDacX.SetCalibration(kSnkCalibX);
	fDacX.SetVoltage(0.);
	fDacX.SetStepWidth(1.);
	fDacX.SetRange((Double_t) kSnkMaxAmplitude);

	fDacY.SetCalibration(kSnkCalibY);
	fDacY.SetVoltage(0.);
	fDacY.SetStepWidth(1.);
	fDacY.SetRange((Double_t) kSnkMaxAmplitude);
}

Bool_t TSnkDDA0816::SetStopPos(Int_t XorY, Double_t StopPos, Int_t Pos0) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816::SetStopPos
// Purpose:        Define stop position
// Arguments:      Int_t XorY          -- X(0) or Y(1)
//                 Double_t StopPos    -- stop position [um]
//                 Int_t Pos0          -- offset (steps)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets the stop position for a channel.
//                  
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (this->IsXorY(XorY)) {
		this->GetDac(XorY)->SetStopPos(StopPos, Pos0);
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Wrong index - " << XorY << " (should be 0(X) or 1(Y))" << endl;
		gMrbLog->Flush(this->ClassName(), "SetStopPos");
		return(kFALSE);
	}
}

