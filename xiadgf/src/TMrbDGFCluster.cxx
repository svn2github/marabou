//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFCluster.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGFCluster.cxx,v 1.1 2005-05-10 15:08:12 marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TROOT.h"
#include "TObjString.h"

#include "TMrbLogger.h"

#include "TMrbDGFCluster.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

ClassImp(TMrbDGFClusterMember)
ClassImp(TMrbDGFCluster)
ClassImp(TMrbLofDGFClusters)

Bool_t TMrbDGFClusterMember::SetSide(const Char_t * Side) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember::SetSide
// Purpose:        Set side (right, left)
// Arguments:      const Char_t * Side   -- side
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines detector side.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString side = Side;
	side.ToLower();
	fSide = "";
	if (side.CompareTo("right") == 0)		fIsRight = kTRUE;
	else if ((side.CompareTo("left") == 0)) fIsRight = kFALSE;
	else {
		gMrbLog->Err() << "Wrong side data - " << Side << " (should be \"right\" or \"left\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSide");
		return(kFALSE);
	}
	fSide = Side;
	return(kTRUE);
}

Bool_t TMrbDGFClusterMember::SetHeight(const Char_t * Height) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember::SetHeight
// Purpose:        Set height (up, down)
// Arguments:      const Char_t * Height   -- height
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines detector height.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString height = Height;
	height.ToLower();
	fHeight = "";
	if (height.CompareTo("up") == 0)			fIsUp = kTRUE;
	else if ((height.CompareTo("down") == 0))	fIsUp = kFALSE;
	else {
		gMrbLog->Err() << "Wrong height data - " << Height << " (should be \"up\" or \"down\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetHeight");
		return(kFALSE);
	}
	fHeight = height;
	return(kTRUE);
}

Bool_t TMrbDGFClusterMember::SetAngle(const Char_t * Angle) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember::SetAngle
// Purpose:        Set angle (forward, backward)
// Arguments:      const Char_t * Angle   -- angle
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines detector angle.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString angle = Angle;
	angle.ToLower();
	fAngle = "";
	if (angle.CompareTo("forward") == 0)			fIsForward = kTRUE;
	else if ((angle.CompareTo("backward") == 0))	fIsForward = kFALSE;
	else {
		gMrbLog->Err() << "Wrong angle data - " << Angle << " (should be \"forward\" or \"backward\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetAngle");
		return(kFALSE);
	}
	fAngle = angle;
	return(kTRUE);
}

void TMrbDGFClusterMember::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs data to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << Form("%3d %2d %c %4d %4d %-10s %s %-6s %-5s %-9s %1d %2d %2d %s %s",
					fClusterIndex,
					fClusterNo,
					fCapsuleId,
					fHex,
					fVoltage,
					fColor.Data(),
					fAF.Data(),
					fSide.Data(),
					fHeight.Data(),
					fAngle.Data(),
					fCrate,
					fSlot[0], fSlot[1],
					fDgf[0].Data(), fDgf[1].Data()) << endl;
}

void TMrbDGFCluster::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs data to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFClusterMember * cluMem = (TMrbDGFClusterMember *) fMembers.First();
	while (cluMem) {
		cluMem->Print(Out);
		cluMem = (TMrbDGFClusterMember *) fMembers.After(cluMem);
	}
}

Int_t TMrbLofDGFClusters::ReadFile(const Char_t * ClusterFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFClusters::ReadFile
// Purpose:        Read cluster data
// Arguments:      Char_t * ClusterFile   -- cluster file
// Results:        Int_t NofClusters      -- number of clusters
// Exceptions:
// Description:    Reads cluster data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString cluName;

 	ifstream cFile(ClusterFile, ios::in);
	if (!cFile.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << ClusterFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFile");
		return(-1);
	}

	TMrbString cLine;
	Int_t lineNo = 0;
	Int_t nofClusters = 0;
	Int_t cluIdx = 0;
	TMrbDGFCluster * clu = NULL;
	while (kTRUE) {
		cLine.ReadLine(cFile, kFALSE);
		if (cFile.eof()) {
			cFile.close();
			if (clu && clu->GetNofMembers() > 0) this->AddNamedX(cluIdx, cluName.Data(), NULL, clu);
			return(nofClusters);
		}
		cLine = cLine.Strip(TString::kBoth);
		if (cLine(0) == '#' || cLine.Length() == 0) continue;

		TObjArray sLine;
		sLine.Delete();
		Int_t n = cLine.Split(sLine, " ", kTRUE);
		if (n != 15) {
			gMrbLog->Err() << "Wrong number of items in line " << lineNo << " - " << n << " (should be 15)" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFile");
			continue;
		}

		Int_t cIdx = atoi(((TObjString *) sLine[0])->GetString().Data());
		if (cIdx != cluIdx) {
			if (clu) this->AddNamedX(cIdx, cluName.Data(), NULL, clu);
			cluName = "clu";
			cluName += cIdx;
			clu = new TMrbDGFCluster(cIdx, cluName.Data());
			cluIdx = cIdx;
			nofClusters++;
		}

		Int_t cluNo = atoi(((TObjString *) sLine[1])->GetString());
		clu->SetClusterNo(cluNo);

		TString capsId = ((TObjString *) sLine[2])->GetString();

		Bool_t ok = kTRUE;
		TMrbDGFClusterMember * cluMem = new TMrbDGFClusterMember(cIdx, cluNo, capsId(0));

		cluMem->SetHex(atoi(((TObjString *) sLine[3])->GetString().Data()));
		cluMem->SetVoltage(atoi(((TObjString *) sLine[4])->GetString().Data()));
		cluMem->SetColor(((TObjString *) sLine[5])->GetString().Data());
		cluMem->SetAF(((TObjString *) sLine[6])->GetString().Data());
		if (!cluMem->SetSide(((TObjString *) sLine[7])->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetHeight(((TObjString *) sLine[8])->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetAngle(((TObjString *) sLine[9])->GetString().Data())) ok = kFALSE;
		cluMem->SetCrate(atoi(((TObjString *) sLine[10])->GetString().Data()));
		cluMem->SetSlot(0, atoi(((TObjString *) sLine[11])->GetString().Data()));
		cluMem->SetSlot(1, atoi(((TObjString *) sLine[12])->GetString().Data()));
		cluMem->SetDgf(0, ((TObjString *) sLine[13])->GetString().Data());
		cluMem->SetDgf(1, ((TObjString *) sLine[14])->GetString().Data());

		if (ok) clu->AddMember(cluMem);
	}
}	

void TMrbLofDGFClusters::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFClusters::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs data to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) this->First();
	while (nx) {
		TMrbDGFCluster * clu = (TMrbDGFCluster *) nx->GetAssignedObject();
		if (clu) clu->Print(Out);
		Out << endl;
		nx = (TMrbDGFCluster *) this->After(nx);
	}
}
