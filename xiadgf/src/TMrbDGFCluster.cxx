//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFCluster.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGFCluster.cxx,v 1.3 2005-05-11 08:40:03 marabou Exp $       
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
	fSide = "void";
	if (side.CompareTo("void") == 0) return(kTRUE);

	if (side.CompareTo("right") == 0)		fIsRight = kTRUE;
	else if (side.CompareTo("left") == 0)	fIsRight = kFALSE;
	else {
		gMrbLog->Err() << "Wrong side data - " << Side << " (should be \"right\" or \"left\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSide");
		return(kFALSE);
	}
	fSide = side;
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
	fHeight = "void";
	if (height.CompareTo("void") == 0) return(kTRUE);

	if (height.CompareTo("up") == 0)			fIsUp = kTRUE;
	else if (height.CompareTo("down") == 0) 	fIsUp = kFALSE;
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
	fAngle = "void";
	if (angle.CompareTo("void") == 0) return(kTRUE);

	if (angle.CompareTo("forward") == 0)		fIsForward = kTRUE;
	else if (angle.CompareTo("backward") == 0)	fIsForward = kFALSE;
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

Bool_t TMrbDGFCluster::AddMember(TMrbDGFClusterMember * Member) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::AddMember
// Purpose:        Add a member to cluster
// Arguments:      TMrbDGFClusterMember * Member   -- cluster member
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a new member.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetNofMembers() > 0) {
		Bool_t ok = kTRUE;
		if (fCrate != Member->GetCrate()) {
			gMrbLog->Err() << "Wrong crate number - " << Member->GetCrate() << " (should be " << fCrate << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (fClusterNo != Member->GetClusterNo()) {
			gMrbLog->Err() << "Wrong cluster number - " << Member->GetClusterNo() << " (should be " << fClusterNo << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (fColor.CompareTo(Member->GetColor()) != 0) {
			gMrbLog->Err() << "Wrong cluster color - " << Member->GetColor() << " (should be " << fColor << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (fSide.CompareTo(Member->GetSide()) != 0) {
			gMrbLog->Err() << "Wrong cluster side - " << Member->GetSide() << " (should be " << fSide << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (fHeight.CompareTo(Member->GetHeight()) != 0) {
			gMrbLog->Err() << "Wrong cluster height - " << Member->GetHeight() << " (should be " << fHeight << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (fAngle.CompareTo(Member->GetAngle()) != 0) {
			gMrbLog->Err() << "Wrong cluster angle - " << Member->GetAngle() << " (should be " << fAngle << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMember");
			ok = kFALSE;
		}
		if (!ok) return(kFALSE);
	}
	fCrate = Member->GetCrate();
	fClusterNo = Member->GetClusterNo();
	fColor = Member->GetColor();
	fSide = Member->GetSide();
	fHeight = Member->GetHeight();
	fAngle = Member->GetAngle();
	fMembers.Add(Member);
	return(kTRUE);
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

	TString cluFile = ClusterFile;
	if (cluFile.EndsWith(".ps")) {
		TString cmd = Form("./nigel2cluster %s cluster.def", cluFile.Data());
		gSystem->Exec(cmd.Data());
		cluFile = "cluster.def";
	}

 	ifstream cFile(cluFile.Data(), ios::in);
	if (!cFile.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << cluFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFile");
		return(-1);
	}

	TMrbString cLine;
	Int_t lineNo = 0;
	Int_t nofClusters = 0;
	Int_t cluIdx = 0;
	Int_t clux = 0;
	TMrbDGFCluster * clu = NULL;
	while (kTRUE) {
		cLine.ReadLine(cFile, kFALSE);
		if (cFile.eof()) {
			cFile.close();
			if (clu && clu->GetNofMembers() > 0) this->AddNamedX(clux, cluName.Data(), NULL, clu);
			return(nofClusters);
		}
		cLine = cLine.Strip(TString::kBoth);
		if (cLine(0) == '#' || cLine.Length() == 0) continue;

		TObjArray sLine;
		sLine.Delete();
		Int_t nIdx = cLine.Split(sLine, " ", kTRUE);
		if (nIdx != kMrbDGFClusterIdxShort && nIdx != kMrbDGFClusterIdxLong) {
			gMrbLog->Err()	<< "Wrong number of items in line " << lineNo << " - " << nIdx
							<< " (should be " << kMrbDGFClusterIdxShort << " or " << kMrbDGFClusterIdxLong << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFile");
			continue;
		}

		Int_t cIdx = atoi(((TObjString *) sLine[kMrbDGFClusterIdx])->GetString().Data());
		if (cIdx != cluIdx) {
			if (clu) this->AddNamedX(clux, cluName.Data(), NULL, clu);
			cluName = "clu";
			cluName += cIdx;
			clu = new TMrbDGFCluster(cIdx, cluName.Data());
			cluIdx = cIdx;
			nofClusters++;
		}

		Int_t cluNo = atoi(((TObjString *) sLine[kMrbDGFClusterNo])->GetString());
		clu->SetClusterNo(cluNo);

		TString capsId = ((TObjString *) sLine[kMrbDGFClusterCapsId])->GetString();

		Bool_t ok = kTRUE;
		TMrbDGFClusterMember * cluMem = new TMrbDGFClusterMember(0, cluNo, capsId(0));

		cluMem->SetHex(atoi(((TObjString *) sLine[kMrbDGFClusterHex])->GetString().Data()));
		cluMem->SetVoltage(atoi(((TObjString *) sLine[kMrbDGFClusterVoltage])->GetString().Data()));
		cluMem->SetColor(((TObjString *) sLine[kMrbDGFClusterColor])->GetString().Data());
		cluMem->SetAF(((TObjString *) sLine[kMrbDGFClusterAF])->GetString().Data());
		if (!cluMem->SetSide(((TObjString *) sLine[kMrbDGFClusterSide])->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetHeight(((TObjString *) sLine[kMrbDGFClusterHeight])->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetAngle(((TObjString *) sLine[kMrbDGFClusterAngle])->GetString().Data())) ok = kFALSE;
		cluMem->SetCrate(atoi(((TObjString *) sLine[kMrbDGFClusterCrate])->GetString().Data()));
		cluMem->SetSlot(0, atoi(((TObjString *) sLine[kMrbDGFClusterSlot1])->GetString().Data()));
		cluMem->SetSlot(1, atoi(((TObjString *) sLine[kMrbDGFClusterSlot2])->GetString().Data()));
		if (nIdx == kMrbDGFClusterIdxShort) {
			TMrbString dgfName = "dgf";
			dgfName += cluMem->GetSlot(0);
			cluMem->SetDgf(0, dgfName.Data());
			dgfName = "dgf";
			dgfName += cluMem->GetSlot(1);
			cluMem->SetDgf(1, dgfName.Data());
			clux = cluIdx;
		} else {
			cluMem->SetDgf(0, ((TObjString *) sLine[kMrbDGFClusterDgf1])->GetString().Data());
			cluMem->SetDgf(1, ((TObjString *) sLine[kMrbDGFClusterDgf2])->GetString().Data());
			TMrbString d = cluMem->GetDgf(0);
			d = d(3,5);
			d.ToInteger(clux);
			clux /= 10;
		}

		if (ok) {
			cluMem->SetClusterIndex(clux);
			clu->AddMember(cluMem);
		}
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

	Out << endl << "DGF Cluster Data:" << endl;
	Out << Form("%3s %-4s %4s %4s %-10s %2s %-6s %-5s %-9s %s %2s %2s %s",
					"#",
					"id",
					"hex",
					"V",
					"color",
					"AF",
					"side",
					"hght",
					"angle",
					"C",
					"N1", "N2", "DGF") << endl;
	Out << "---------------------------------------------------------------------------------" << endl;

	TMrbNamedX * nx = (TMrbNamedX *) this->First();
	while (nx) {
		TMrbDGFCluster * clu = (TMrbDGFCluster *) nx->GetAssignedObject();
		if (clu) clu->Print(Out);
		Out << endl;
		nx = (TMrbDGFCluster *) this->After(nx);
	}
	Out << endl;
}

TMrbDGFCluster *  TMrbLofDGFClusters::GetCluster(Int_t CluNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFClusters::GetCluster
// Purpose:        Get cluster by its number
// Arguments:      Int_t CluNo              -- cluster number
// Results:        TMrbDGFCluster * Cluster -- cluster
// Exceptions:
// Description:    Returns cluster.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) this->FindByIndex(CluNo);
	if (nx) return((TMrbDGFCluster *) nx->GetAssignedObject());
	return(NULL);
}

