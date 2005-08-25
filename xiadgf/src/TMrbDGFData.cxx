//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFData.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGFData.cxx,v 1.9 2005-08-25 14:32:17 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TEnv.h"

#include "TMrbLogger.h"

#include "TMrbDGFData.h"
#include "TMrbDGF.h"
#include "TMrbDGFCommon.h"

#include "TMrbSystem.h"

#include "SetColor.h"

ClassImp(TMrbDGFData)

extern TMrbLogger * gMrbLog;

TMrbDGFData::TMrbDGFData() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData
// Purpose:        Data base to hold DSP / FPGA data
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	Setup();
	this->CheckXiaRelease();
	this->PrintXiaRelease();
}

void TMrbDGFData::Setup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::Setup
// Purpose:        Setup data base
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Initializes DGF's data base.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofDGFFileTypes.SetName("DGF File Types"); 		 			// initialize public lists
	fLofDGFFileTypes.AddNamedX(kMrbDGFFileTypes);

	fLofFPGATypes.SetName("FPGA Types");
	fLofFPGATypes.AddNamedX(kMrbFPGATypes);
	fLofFPGATypes.SetPatternMode();

	fLofDGFStatusDBits.SetName("Soft Status (Data)");
	fLofDGFStatusDBits.AddNamedX(kMrbDGFStatusDBits);
	fLofDGFStatusDBits.SetPatternMode();

	fLofCamacICSRBits.SetName("Camac Control & Status (System)");
	fLofCamacICSRBits.AddNamedX(kMrbCamacICSRBits);
	fLofCamacICSRBits.SetPatternMode();

	fLofCamacCSRBits.SetName("Camac Control & Status");
	fLofCamacCSRBits.AddNamedX(kMrbCamacCSRBits);
	fLofCamacCSRBits.SetPatternMode();

	fLofModCSRABits.SetName("Module Control & Status Reg A");
	fLofModCSRABits.AddNamedX(kMrbModCSRABits);
	fLofModCSRABits.SetPatternMode();

	fLofModCSRBBits.SetName("Module Control Reg B");
	fLofModCSRBBits.AddNamedX(kMrbModCSRBBits);
	fLofModCSRBBits.SetPatternMode();

	fLofChanCSRABits.SetName("Channel Control & Status Reg A");
	fLofChanCSRABits.AddNamedX(kMrbChanCSRABits);
	fLofChanCSRABits.SetPatternMode();

	fLofUserPsaCSRBits.SetName("UserPSA CSR Bits");
	fLofUserPsaCSRBits.AddNamedX(kMrbUserPsaCSRBits);

	fLofRunTasks.SetName("Run Tasks");
	fLofRunTasks.AddNamedX(kMrbRunTasks);

	fLofControlTasks.SetName("Control Tasks");
	fLofControlTasks.AddNamedX(kMrbControlTasks);

	fDSPType = kFileUndef; 				// no code loaded so far
	fDSPSize = -1;
	fDSPCode.Set(kDSPMaxSize);

	fSystemFPGAType = kFileUndef;
	fSystemFPGASize = -1;
	fSystemFPGACode.Set(kFPGAMaxSize);

	fFippiFPGAType[kRevD] = kFileUndef;
	fFippiFPGASize[kRevD] = -1;
	fFippiFPGACode[kRevD].Set(kFPGAMaxSize);

	fFippiFPGAType[kRevE] = kFileUndef;
	fFippiFPGASize[kRevE] = -1;
	fFippiFPGACode[kRevE].Set(kFPGAMaxSize);

	this->Reset();
}

void TMrbDGFData::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::Reset
// Purpose:        Reset to initial values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Initializes DGF's data base.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fNofParams = 0; 				// clear param names
	fParamNames.Delete();
	fStatusD = 0;					// clear status
}

Int_t TMrbDGFData::ReadFPGACode(EMrbFPGAType FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACode
// Purpose:        Read FPGA code
// Arguments:      EMrbFPGAType FPGAType -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code. Data format depends on file extension:
//                       .fip  -- ascii
//                       .bin  -- binary
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString sysfip;
	TString resource;

	TString ext;
	TMrbSystem ux;

	if (this->FPGACodeRead(FPGAType, Rev) && !Forced) return(fSystemFPGASize);

	if (FPGAType == kSystemFPGA) {
		sysfip = "System";
		resource = "TMrbDGF.SystemFPGACode";
	} else {
		sysfip = "Fippi";
		resource = "TMrbDGF.FippiFPGACode";
	}

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue(resource.Data(), "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "[" << sysfip << "FPGA] File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACode");
		return(-1);
	}

	ux.GetExtension(ext, CodeFile);
	if (ext.CompareTo(".fip") == 0) 		return(this->ReadFPGACodeAscii(FPGAType, CodeFile, Rev, Forced));
	else if (ext.CompareTo(".bin") == 0)	return(this->ReadFPGACodeBinary(FPGAType, CodeFile, Rev, Forced));
	else {
		gMrbLog->Err() << "[" << sysfip << "FPGA] Unknown file extension - " << CodeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACode");
		return(-1);
	}
}

Int_t TMrbDGFData::ReadFPGACode(const Char_t * FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACode
// Purpose:        Read FPGA code
// Arguments:      Char_t * FPGAType     -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code. Data format depends on file extension
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACode");
		return(-1);
	}
	return(this->ReadFPGACode((EMrbFPGAType) sysfip->GetIndex(), CodeFile, Rev, Forced));
}

Int_t TMrbDGFData::ReadFPGACodeBinary(EMrbFPGAType FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACodeBinary
// Purpose:        Read FPGA code from binary file
// Arguments:      EMrbFPGAType FPGAType -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code.
//                   Input:    byte-oriented binary
//                   Output:   UShort_t, MSB = 0
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t byte;
	ifstream fpga;
	Int_t size;
	TString dataPath;
	TString codeFile;

	TString sysfip;
	TString resource;

	UShort_t * dp;

	if (this->FPGACodeRead(FPGAType, Rev) && !Forced) return(fSystemFPGASize);

	if (FPGAType == kSystemFPGA) {
		sysfip = "System";
		resource = "TMrbDGF.SystemFPGACodeBinary";
	} else {
		sysfip = "Fippi";
		resource = "TMrbDGF.FippiFPGACodeBinary";
	}

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf/v2.70");
	gSystem->ExpandPathName(dataPath);

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue(resource.Data(), "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "[" << sysfip << "FPGA] File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeBinary");
		return(-1);
	}

	codeFile = CodeFile;
	if (!codeFile.BeginsWith("../") && !codeFile.BeginsWith("/")) {
		TString fileSpec = gSystem->Which(dataPath.Data(), CodeFile);
		if (fileSpec.IsNull()) {
			gMrbLog->Err() << "[" << sysfip << "FPGA] No such file - " << CodeFile << endl;
			gMrbLog->Err() << "(Searched on path " << dataPath << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFPGACodeBinary");
			return(-1);
		}
		codeFile = fileSpec;
	}
 
	fpga.open(codeFile, ios::binary | ios::in);
	if (!fpga.good()) {
		gMrbLog->Err() << "[" << sysfip << "FPGA] " << gSystem->GetError() << " - " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeBinary");
		return(-1);
	}
	if (FPGAType == kSystemFPGA) {
		fSystemFPGAFile = codeFile;
		fSystemFPGAType = kFileBinary;
		size = 0;
		dp = (UShort_t *) fSystemFPGACode.GetArray();
		for (;;) {
			fpga.get(byte);
			if (fpga.eof()) break;
			*dp = (UShort_t) byte;
			dp++;
			size++;
		}
		fSystemFPGASize = size;
		fStatusD |= kSystemFPGACodeRead;
	} else {
		fFippiFPGAFile[Rev] = codeFile;
		fFippiFPGAType[Rev] = kFileBinary;
		size = 0;
		dp = (UShort_t *) fFippiFPGACode[Rev].GetArray();
		for (;;) {
			fpga.get(byte);
			if (fpga.eof()) break;
			*dp = (UShort_t) byte;
			dp++;
			size++;
		}
		fFippiFPGASize[Rev] = size;
		if (Rev == kRevE)	fStatusD |= kFippiFPGARevECodeRead;
		else							fStatusD |= kFippiFPGARevDCodeRead;
	}
	fpga.close();

	if (fVerboseMode) {
		gMrbLog->Out() << "[" << sysfip << "FPGA] " << size << " bytes read from " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeBinary", setblue);
	}

	return(size);
}

Int_t TMrbDGFData::ReadFPGACodeBinary(const Char_t * FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACodeBinary
// Purpose:        Read FPGA code from binary file
// Arguments:      Char_t * FPGAType     -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeBinary");
		return(-1);
	}
	return(this->ReadFPGACodeBinary((EMrbFPGAType) sysfip->GetIndex(), CodeFile, Rev, Forced));
}

Int_t TMrbDGFData::ReadFPGACodeAscii(EMrbFPGAType FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACodeAscii
// Purpose:        Read FPGA code from ascii file
// Arguments:      EMrbFPGAType FPGAType -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code.
//                   Input:    hex code ascii
//                   Output:   UShort_t, MSB = 0
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString lineOfCode;
	ifstream fpga;
	Int_t size;
	Int_t lng;
	Int_t line;
	UInt_t data;
	UShort_t *dp;
	TString dataPath;
	TString codeFile;
	TString hexDigits;
	Bool_t hStart, hEnd;

	TString sysfip;
	TString resource;

	if (this->FPGACodeRead(FPGAType, Rev) && !Forced) return(fSystemFPGASize);

	if (FPGAType == kSystemFPGA) {
		sysfip = "System";
		resource = "TMrbDGF.SystemFPGACodeAscii";
	} else {
		sysfip = "Fippi";
		resource = "TMrbDGF.FippiFPGACodeAscii";
	}

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf/v1.0");
	gSystem->ExpandPathName(dataPath);

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue(resource.Data(), "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "[" << sysfip << "FPGA] File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
		return(-1);
	}

	codeFile = CodeFile;
	if (!codeFile.BeginsWith("../") && !codeFile.BeginsWith("/")) {
		TString fileSpec = gSystem->Which(dataPath.Data(), CodeFile);
		if (fileSpec.IsNull()) {
			gMrbLog->Err() << "[" << sysfip << "FPGA] No such file - " << CodeFile << endl;
			gMrbLog->Err() << "(Searched on path " << dataPath << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
			return(-1);
		}
		codeFile = fileSpec;
	}
  
	fpga.open(codeFile, ios::binary | ios::in);
	if (!fpga.good()) {
		gMrbLog->Err() << "[" << sysfip << "FPGA] " << gSystem->GetError() << " - " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
		return(-1);
	}

	if (FPGAType == kSystemFPGA) {
		fSystemFPGAFile = codeFile;
		fSystemFPGAType = kFileAscii;
		dp = (UShort_t *) fSystemFPGACode.GetArray();
	} else {
		fFippiFPGAFile[Rev] = codeFile;
		fFippiFPGAType[Rev] = kFileAscii;
		dp = (UShort_t *) fFippiFPGACode[Rev].GetArray();
	}

	size = 0;
	line = 0;
	hStart = kFALSE;
	hEnd = kFALSE;
	for (;;) {
		lineOfCode.ReadLine(fpga, kFALSE);
		if (fpga.eof()) break;
		line++;
		lineOfCode = lineOfCode.Strip(TString::kTrailing, '\r');
		lineOfCode = lineOfCode.Strip(TString::kBoth);
		lng = lineOfCode.Length();
		if (lng == 0) continue;
		if (!hEnd) {
			if (lineOfCode(0) == '*') {
				if (fVerboseMode && !hStart) {
					cout	<< "===========================================================================================" << endl
							<< " Header of " << sysfip << "FPGA code file " << codeFile << endl
							<< "..........................................................................................." << endl;
				}
				hStart = kTRUE;
				if (fVerboseMode) cout << " " << lineOfCode << endl;
				continue;
			} else if (hStart) {
				if (fVerboseMode) cout	<< "-------------------------------------------------------------------------------------------" << endl;
				hEnd = kTRUE;
			}
		}
		if (lng % 2) {
			gMrbLog->Err() << "[" << sysfip << "FPGA] " << codeFile << " (line " << line << "): Odd byte count" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
			fpga.close();
			return(-1);
		}

		hexDigits = "0123456789ABCDEFabcdef";
		for (i = 0; i < lng; i++) {
			data = hexDigits.Index(lineOfCode(i));
			if (data == 0xffffffff) {
				gMrbLog->Err()	<< codeFile << " (line " << line << ", byte " << i
								<< "): Not a hex digit - " << lineOfCode(i) << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
				fpga.close();
				return(-1);
			}
			if (i % 2) {
				*dp |= data;
				dp++;
				size++;
			} else {
				*dp = data << 4;
			}
		}
 	}
	fpga.close();

	if (FPGAType == kSystemFPGA) {
		fSystemFPGASize = size;
		fStatusD |= kSystemFPGACodeRead;
	} else {
		fFippiFPGASize[Rev] = size;
		if (Rev == kRevE)	fStatusD |= kFippiFPGARevECodeRead;
		else							fStatusD |= kFippiFPGARevDCodeRead;
	}

	if (fVerboseMode) {
		gMrbLog->Out() << "[" << sysfip << "FPGA] " << size << " bytes read from " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii", setblue);
	}
	return(size);
}

Int_t TMrbDGFData::ReadFPGACodeAscii(const Char_t * FPGAType, const Char_t * CodeFile, Int_t Rev, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadFPGACodeAscii
// Purpose:        Read FPGA code from ascii file
// Arguments:      Char_t * FPGAType     -- fpga type (system or fippi)
//                 Char_t * CodeFile     -- file name
//                 Int_t Rev             -- module revision
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads FPGA code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFPGACodeAscii");
		return(-1);
	}
	return(this->ReadFPGACodeAscii((EMrbFPGAType) sysfip->GetIndex(), CodeFile, Rev, Forced));
}

Int_t TMrbDGFData::ReadDSPCode(const Char_t * CodeFile, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadDSPCode
// Purpose:        Read DSP code
// Arguments:      Char_t * CodeFile     -- file name
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads DSP code. Data format depends on file extension:
//                      .exe  -- ascii
//                      .bin  -- binary
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString ext;
	TMrbSystem ux;

	if (this->DSPCodeRead() && !Forced) return(fDSPSize);

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue("TMrbDGF.DSPCode", "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCode");
		return(-1);
	}

	ux.GetExtension(ext, CodeFile);
	if (ext.CompareTo(".exe") == 0) 		return(this->ReadDSPCodeAscii(CodeFile, Forced));
	else if (ext.CompareTo(".bin") == 0)	return(this->ReadDSPCodeBinary(CodeFile, Forced));
	else {
		gMrbLog->Err() << "Unknown file extension - " << CodeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCode");
		return(-1);
	}
}

Int_t TMrbDGFData::ReadDSPCodeBinary(const Char_t * CodeFile, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadDSPCodeBinary
// Purpose:        Read DSP code from binary file
// Arguments:      Char_t * CodeFile     -- file name
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads DSP code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	ifstream dsp;
	Int_t size;
	TString dataPath;
	TString codeFile;
	Int_t byte0, byte1, byte2, byte3;
	
	if (this->DSPCodeRead() && !Forced) return(fDSPSize);

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf/v1.0");
	gSystem->ExpandPathName(dataPath);

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue("TMrbDGF.DSPCodeBinary", "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary");
		return(-1);
	}

	codeFile = CodeFile;
	if (!codeFile.BeginsWith("../") && !codeFile.BeginsWith("/")) {
		TString fileSpec = gSystem->Which(dataPath.Data(), CodeFile);
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "No such file - " << CodeFile << endl
							<< "(Searched on path " << dataPath << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary");
			return(-1);
		}
		codeFile = fileSpec;
	}
 
	dsp.open(codeFile, ios::binary | ios::in);
	if (!dsp.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary");
		return(-1);
	}
	fDSPFile = codeFile;
	fDSPType = kFileBinary;

	dsp.read((Char_t *) fDSPCode.GetArray(), kDSPMaxSize); 	// read the whole bunch of code
	size = dsp.gcount();													// get real byte count
	dsp.close();
	
	if (size % 4) {
		gMrbLog->Err() << codeFile << ": Odd file size - " << size << " bytes" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary");
		return(-1);
	}
	size /= 2;													// count in 16 bit words
	
	for (i = 0; i < size; i += 2) {
		byte0 = (fDSPCode[i] >> 8) & 0x00ff;
		byte1 = fDSPCode[i] & 0x00ff;
		byte2 = (fDSPCode[i + 1] >> 8) & 0x00ff;
		byte3 = fDSPCode[i + 1] & 0x00ff;
		if (byte2 != 0) {
			gMrbLog->Err() << "Data inconsistent - byte #2 != 0" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary");
			return(-1);
		}
		fDSPCode[i] = (byte3 << 8) + byte0;
		fDSPCode[i + 1] = byte1;
	}

	fDSPSize = size;
	fStatusD |= kDSPCodeRead;

	if (fVerboseMode) {
		gMrbLog->Out() << size << " words read from " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeBinary", setblue);
	}

	return(size);
}

Int_t TMrbDGFData::ReadDSPCodeAscii(const Char_t * CodeFile, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadDSPCodeAscii
// Purpose:        Read DSP code from WaveMetrics ascii file
// Arguments:      Char_t * CodeFile     -- file name
//                 Bool_t Forced         -- read even if code already loaded
// Results:        Int_t NofWords        -- number of words read
// Exceptions:
// Description:    Reads DSP code.
//                 Format: WaveMetrics (ASCII)
//                 Code: 24 bits per line, 0xHHHHLL -> 0xHHHH 0x00LL
//                 Lines starting with '@' indicate address specifier in next line
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t data;
	ifstream dsp;
	TString dataPath;
	TString codeFile;
	TMrbString line;
	TString c;
	Bool_t addrFlag;
	Int_t addr;
	Int_t lCnt;
				
	TString hexDigits = "0123456789abcdefABCDEF";
	
	if (this->DSPCodeRead() && !Forced) return(fDSPSize);

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf/v1.0");
	gSystem->ExpandPathName(dataPath);

	if (*CodeFile == '\0') CodeFile = gEnv->GetValue("TMrbDGF.DSPCodeAscii", "");
	if (*CodeFile == '\0') {
		gMrbLog->Err() << "File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeAscii");
		return(-1);
	}

	codeFile = CodeFile;
	if (!codeFile.BeginsWith("../") && !codeFile.BeginsWith("/")) {
		TString fileSpec = gSystem->Which(dataPath.Data(), CodeFile);
		if (fileSpec.IsNull()) {
			gMrbLog->Err()	<< "No such file - " << CodeFile << endl
							<< "(Searched on path " << dataPath << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadDSPCodeAscii");
			return(-1);
		}
		codeFile = fileSpec;
	}
 
	dsp.open(codeFile, ios::in);
	if (!dsp.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeAscii");
		return(-1);
	}
	fDSPFile = codeFile;
	fDSPType = kFileAscii;

	fDSPCode.Set(kDSPMaxSize);
	fDSPCode.Reset();
		
	addrFlag = kFALSE;
	addr = 0;
	lCnt = 0;
	while(kTRUE) {
		line.ReadLine(dsp, kFALSE);
		if (dsp.eof()) {
			dsp.close();
			break;
		}
		lCnt++;
		line = line.Strip(TString::kBoth);
		c = line(line.Length() - 1);
		if (hexDigits.Index(c, 0) < 0) line = line(0, line.Length() - 1);
		if (line(0) == '@') {
			addrFlag = kTRUE;
		} else {
			c = line(0);
			if (hexDigits.Index(c, 0) >= 0) {
				if (!line.ToInteger(data, 16)) {
					gMrbLog->Err() << fDSPFile << " (line " << lCnt << ": Not a HEX number - " << line << endl;
					gMrbLog->Flush(this->ClassName(), "ReadDSPCodeAscii");
				}
				if (addrFlag) {
					addr = 2 * data;
				} else {
					fDSPCode[addr++] = (data >> 8) & 0xFFFF;		// 0xHHHHLL -> word #1: high order bits 0xHHHH
					fDSPCode[addr++] = data & 0xFF; 				// 0xHHHHLL -> word #2: low order bits 0x00LL
				}
			}
			addrFlag = kFALSE;
		}
	}

	fDSPSize = addr;
	fStatusD |= kDSPCodeRead;

	ofstream xxx;
	xxx.open("dsp.dat", ios::out);
	for (i = 0; i < addr; i++) xxx << setbase(16) << fDSPCode[i] << setbase(10) << endl;
	xxx.close();
	
	if (fVerboseMode) {
		gMrbLog->Out() << addr << " words read from " << codeFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDSPCodeAscii", setblue);
	}

	return(addr);
}
	

Int_t TMrbDGFData::ReadNameTable(const Char_t * ParamFile, Bool_t Forced) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadNameTable
// Purpose:        Read param names from file
// Arguments:      Char_t * ParamFile    -- file name
//                 Bool_t Forced         -- read even if names already read
// Results:        Int_t NofParams       -- number of params read
// Exceptions:
// Description:    Reads param names and offsets from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	TString pName;
	TMrbNamedX * pKey;
	ifstream param;
	Int_t nofParams;
	TString dataPath;
	TString paramFile;
	TString pLine;

	TString charU;

	if (this->ParamNamesRead() && !Forced) return(fNofParams);

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf/v2.70");
	gSystem->ExpandPathName(dataPath);

	if (*ParamFile == '\0') ParamFile = gEnv->GetValue("TMrbDGF.ParamNames", "");
	if (*ParamFile == '\0') {
		gMrbLog->Err() << "File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(-1);
	}

	paramFile = ParamFile;
	if (!paramFile.BeginsWith("../") && !paramFile.BeginsWith("/")) {
		TString fileSpec = gSystem->Which(dataPath.Data(), ParamFile);
		if (fileSpec.IsNull()) {
			gMrbLog->Err() << "No such file - " << ParamFile << ", searched on \"" << dataPath << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadNameTable");
			return(-1);
		}
		paramFile = fileSpec;
	}

	param.open(paramFile, ios::in);
	if (!param.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(-1);
	}
	fParamFile = paramFile;

	nofParams = 0;
	fParamNames.Delete();
	for (;;) {
		pLine.ReadLine(param, kFALSE);
		if (param.eof()) break;
		pLine = pLine.Strip(TString::kBoth);
		if (pLine.Length() == 0 || pLine(0) == '#') continue;
		istringstream str(pLine.Data());
		str >> pOffset >> pName;
		pName = pName.Strip(TString::kBoth);
		if (pName.Length() > 0) {
			pKey = new TMrbNamedX(pOffset, pName.Data());
			fParamNames.AddNamedX(pKey);
			nofParams++;
		}
	}
	param.close();
	fNofParams = nofParams;
	fStatusD |= kParamNamesRead;

	if (fVerboseMode) {
		gMrbLog->Out() << nofParams << " params read from " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable", setblue);
	}

	return(nofParams);
}

TMrbNamedX * TMrbDGFData::FindParam(Int_t Channel, const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::FindParam
// Purpose:        Search a param for a given channel
// Arguments:      Int_t Channel         -- channel number
//                 Char_t * ParamName    -- param name
// Results:        TMrbNamedX * Param       -- param name & index
// Exceptions:
// Description:    Searches a param for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString paramName;

	paramName = ParamName;
	paramName += Channel;
	return(this->FindParam(paramName.Data()));
}

Bool_t TMrbDGFData::FPGACodeRead(EMrbFPGAType FPGAType, Int_t Rev) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FGPACodeRead
// Purpose:        Test if FPGA code is read
// Arguments:      EMrbFPGAType FPGAType    -- fpga type (system or fippi)
//                 EMrbDGFRevision Rev      -- module revision (if fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been read.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t bit;
	if (FPGAType == kSystemFPGA) {
		bit = kSystemFPGACodeRead;
	} else if (Rev == kRevE) {
		bit = kFippiFPGARevECodeRead;
	} else {
		bit = kFippiFPGARevDCodeRead;
	}
	return((fStatusD & bit) != 0);
}

Bool_t TMrbDGFData::FPGACodeRead(const Char_t * FPGAType, Int_t Rev) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FGPACodeRead
// Purpose:        Test if FPGA code is read
// Arguments:      Char_t * FPGAType    -- fpga type (system or fippi)
//                 Int_t Rev            -- module revision (if fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been read.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "FPGACodeRead");
		return(kFALSE);
	}
	return(this->FPGACodeRead((EMrbFPGAType) sysfip->GetIndex(), Rev));
}

void TMrbDGFData::Print() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::Print
// Purpose:        Print status
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs status info.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	cout	<< "===========================================================================================" << endl;
	cout	<< " DGF-4C";
	if (fLocalData) cout << " *LOCAL* "; else cout << " Common ";
	cout	<< "Data Section" << endl
			<< "..........................................................................................." << endl;
	if (this->FPGACodeRead(kSystemFPGA)) {
		cout	<< " Code for SystemFPGA read from file        : " << fSystemFPGAFile << endl;
		cout	<< " Size of FPGA code                         : " << fSystemFPGASize << " bytes" << endl;
	} else {
		cout	<< " Code for SystemFPGA                       : none" << endl;
	}
	if (this->FPGACodeRead(kFippiFPGA, kRevD)) {
		cout	<< " Code for FippiFPGA (RevD) read from file  : " << fFippiFPGAFile[kRevD] << endl;
		cout	<< " Size of FPGA code                         : " << fFippiFPGASize[kRevD] << " bytes" << endl;
	} else {
		cout	<< " Code for FippiFPGA (RevD)                 : none" << endl;
	}
	if (this->FPGACodeRead(kFippiFPGA, kRevE)) {
		cout	<< " Code for FippiFPGA (RevE) read from file  : " << fFippiFPGAFile[kRevE] << endl;
		cout	<< " Size of FPGA code                         : " << fFippiFPGASize[kRevE] << " bytes" << endl;
	} else {
		cout	<< " Code for FippiFPGA (RevE)                 : none" << endl;
	}
	if (this->DSPCodeRead()) {
		cout	<< " DSP code read from file                   : " << fDSPFile << endl;
		cout	<< " Size of DSP code                          : " << fDSPSize << " words" << endl;
	} else {
		cout	<< " DSP code                                  : none" << endl;
	}
	if (this->ParamNamesRead()) {
		cout	<< " Param names read from file                : " << fParamFile << endl;
		cout	<< " Number of params                          : " << fNofParams << endl;
	} else {
		cout	<< " Param names                               : none" << endl;
	}
	cout	<< "-------------------------------------------------------------------------------------------" << endl;
}

Bool_t TMrbDGFData::CheckXiaRelease() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::CheckXiaRelease
// Purpose:        Determine XIA's release number
// Arguments:      kTRUE/kFALSE
// Results:        --
// Exceptions:
// Description:    Looks for env var TMrbDGF.XiaRelease.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString release, relDate;
	
	fXiaRelease = 0;
	
	release = gEnv->GetValue("TMrbDGF.XiaRelease", "");
	relDate = gEnv->GetValue("TMrbDGF.XiaDate", "");

	if (release.Length() == 0) {
		gMrbLog->Err() << "Release number missing (environment var \"TMrbDGF.XiaRelease\")" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckXiaRelease");
		return(kFALSE);
	} else if (release.Index("v1.0", 0) == 0) {
		fXiaRelease = 100;
		gMrbLog->Err() << "Release no longer supported - " << release << "(" << relDate << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckXiaRelease");
		return(kFALSE);
	} else if (release.Index("v2.0", 0) == 0) {
		fXiaRelease = 200;
		gMrbLog->Err() << "Release no longer supported - " << release << "(" << relDate << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckXiaRelease");
		return(kFALSE);
	} else if (release.Index("v2.70", 0) == 0) {
		fXiaRelease = 270;
		return(kTRUE);
	} else if (release.Index("v2.80", 0) == 0) {
		fXiaRelease = 280;
		return(kTRUE);
	} else if (release.Index("v2.7mb", 0) == 0) {
		fXiaRelease = 271;
		return(kTRUE);
	} else if (release.Index("v3.00", 0) == 0) {
		fXiaRelease = 300;
		return(kTRUE);
	} else if (release.Index("v3.04", 0) == 0) {
		fXiaRelease = 304;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Malformed release - TMrbDGF.XiaRelease = " << release << endl;
		gMrbLog->Flush(this->ClassName(), "CheckXiaRelease");
		return(kFALSE);
	}
}

void TMrbDGFData::PrintXiaRelease() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::PrintXiaRelease
// Purpose:        Output XIA's release number
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs current XIA release to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fXiaRelease == 0) {
		gMrbLog->Err()	<< "XIA Release is undefined" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintXiaRelease");
	} else {
		gMrbLog->Out()	<< "XIA Release "
						<< gEnv->GetValue("TMrbDGF.XiaRelease", "") << " ("
						<< gEnv->GetValue("TMrbDGF.XiaDate", "") << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintXiaRelease", setblue);
	}
}

