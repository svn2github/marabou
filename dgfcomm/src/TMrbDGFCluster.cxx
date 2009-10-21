//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGFCluster.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGFCluster.cxx,v 1.9 2009-10-21 07:46:51 Marabou Exp $       
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

TMrbDGFClusterMember::TMrbDGFClusterMember(Int_t ClusterIndex, Int_t ClusterNo, Char_t CapsuleId) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember
// Purpose:        Create a cluster member
// Arguments:      Int_t ClusterIndex     -- cluster index
//                 Int_t ClusterNo        -- cluster number
//                 Char_t CapsuleId       -- id of capsule
// Results:        --
// Exceptions:
// Description:    Creates a cluster member
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fClusterIndex = ClusterIndex;
	fClusterNo = ClusterNo;
	fCapsuleId = CapsuleId;
	fHex = 0;
	fVoltage = 0;
	fColor = "void";
	fAF = "void";
	fSide = "void";
	fIsRight = kFALSE;
	fHeight = "void";
	fIsUp = kFALSE;
	fAngle = "void";
	fIsForward = kFALSE;
	fCrate = 0;
	for (Int_t i = 0; i < kMrbDGFClusterNofModules; i++) fSlot[i] = 0;
	for (Int_t i = 0; i < kMrbDGFClusterNofModules; i++) fDgf[i] = "undef";
	fCmt = "";
}

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

void TMrbDGFClusterMember::Print(ostream & Out, Bool_t CmtFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
//                 Bool_t CmtFlag   -- output comment if kTRUE
// Results:        --
// Exceptions:
// Description:    Outputs data to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << Form("%3d %2d %c %4d %4d %-10s %s %-6s %-7s %-9s %1d ",
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
					fCrate);
	for (Int_t i = 0; i < fNofModules; i++) {
		Out << Form("%2d ", fSlot[i]);
	}
	for (Int_t i = 0; i < fNofModules; i++) {
		if (fSlot[i] > 0) Out << Form("%s ", fDgf[i].Data());
	}
	if (CmtFlag && fCmt.Length() > 0) Out << "\t" << fCmt;
	Out << endl;
}

TMrbDGFCluster::TMrbDGFCluster(Int_t ClusterIndex, const Char_t * ClusterName)  : TMrbNamedX(ClusterIndex, ClusterName) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster
// Purpose:        Create a dgf cluster 
// Arguments:      Int_t ClusterIndex     -- cluster index
//                  Char_t * ClusterName  -- name
// Results:        --
// Exceptions:
// Description:    Creates a dgf cluster
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fMembers.Delete();
	fClusterNo = 0;
	fCrate = -1;
	fLofChannelLayouts.Delete(); 	// channel layouts for 6 and 12 fold segmented crystals are predefined
	this->SetChannelLayout("6fold", 8, "c:1:2:x:3:4:5:6");
	this->SetChannelLayout("12fold", 16, "c:01:02:03:04:05:06:x1:x2:07:08:09:10:11:12:x3");
}

void TMrbDGFCluster::Print(ostream & Out, Bool_t CmtFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
//                 Bool_t CmtFlag   -- output comment if kTRUE
// Results:        --
// Exceptions:
// Description:    Outputs data to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFClusterMember * cluMem = (TMrbDGFClusterMember *) fMembers.First();
	while (cluMem) {
		cluMem->Print(Out, CmtFlag);
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

Bool_t TMrbDGFCluster::SetChannelLayout(const Char_t * LayoutName, Int_t NofChannels, const Char_t * ChannelNames) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::SetChannelLayout
// Purpose:        Define channel names to be used
// Arguments:      Char_t * LayoutName         -- name of channel layout
//                 Int_t NofChannels           -- max number of channels
//                 Char_t * ChannelNames       -- channel names, :-separated string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a set of channel names to be used.
//                 On startup 2 standard layouts will be defined:
//                 "6fold" ->  channels "c:1:2:x:3:4:5:6" (0-7)
//                 "12fold" -> channels "c:1:2:3:4:5:6:x1:x2:7:8:9:10:11:12:x3" (0-15)
//                 x, x1, x2, x3 spare channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fLofChannelLayouts.FindByName(LayoutName) != NULL) {
		gMrbLog->Err() << "Channel layout already defined - " << LayoutName << endl;
		gMrbLog->Flush(this->ClassName(), "SetChannelLayout");
		return(kFALSE);
	}

	TString cn = ChannelNames;
	TObjArray * ca = cn.Tokenize(":");
	Int_t nofChannels = ca->GetEntries();
	if (nofChannels != NofChannels) {
		gMrbLog->Err()	<< "Channel layout \"" << LayoutName << "\": number of channels ("
						<< NofChannels << ") != number of names given (" << nofChannels << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetChannelLayout");
		delete ca;
		return(kFALSE);
	}

	TString chnLayout = "<";
	Int_t modNo = 0;
	for (Int_t chn = 0; chn < nofChannels; chn++) {
		if (modNo && (modNo % kMrbXiaChansPerModule) == 0) {
			chnLayout += ">";
			ca->Add(new TObjString(chnLayout.Data()));
			chnLayout = "<";
		}
		TString c = ((TObjString *) ca->At(chn))->GetString();
		if (c.CompareTo("c") == 0)	chnLayout += "c";
		else if (c.BeginsWith("x")) chnLayout += "x";
		else						chnLayout += "s";
		modNo++;
	}
	if ((modNo % kMrbXiaChansPerModule) == 0) {
		chnLayout += ">";
		ca->Add(new TObjString(chnLayout.Data()));
	}

	TMrbNamedX * nx = new TMrbNamedX(nofChannels, LayoutName, ChannelNames);
	nx->AssignObject(ca);
	fLofChannelLayouts.AddNamedX(nx);

	return(kTRUE);
}

const Char_t * TMrbDGFCluster::GetChannelLayout(Int_t ModuleNumber, const Char_t * LayoutName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::GetChannelLayout
// Purpose:   	   Get channel layout per module
// Arguments:      Int_t ModuleNumber            -- module number
//                 Char_t * LayoutName           -- name of channel layout
// Results:        Char_t * ChannelLayout        -- layout for given module
// Exceptions:
// Description:    Returns layout definitions per module: <....>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * layout = (TMrbNamedX *) fLofChannelLayouts.FindByName(LayoutName);
	if (layout == NULL) {
		gMrbLog->Err() << "No such channel layout - " << LayoutName << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannelLayout");
		return(NULL);
	}

	Int_t chn1 = ModuleNumber * kMrbXiaChansPerModule;
	Int_t chn2 = chn1 + kMrbXiaChansPerModule - 1;
	if (chn1 < 0 || chn1 > layout->GetIndex() - 1 || chn2 < 0 || chn2 > layout->GetIndex() - 1) {
		gMrbLog->Err() << "Module number out of range - " << ModuleNumber << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannelLayout");
		return(NULL);
	}

	TObjArray * ca = (TObjArray *) layout->GetAssignedObject();
	if (ca) {
		TObjString * os = (TObjString *) ca->At(layout->GetIndex() + ModuleNumber);
		if (os) return(os->GetString());
	}
	return(NULL);
}

const Char_t * TMrbDGFCluster::GetChannelName(Int_t Channel, const Char_t * LayoutName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster::GetChannelName
// Purpose:        Get channel named for a given layout
// Arguments:      Int_t Channel               -- channel number
//                 Char_t * LayoutName         -- name of channel layout
// Results:        Char_t * ChannelName        -- channel name
// Exceptions:
// Description:    Returns channel name by its number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * layout = (TMrbNamedX *) fLofChannelLayouts.FindByName(LayoutName);
	if (layout == NULL) {
		gMrbLog->Err() << "No such channel layout - " << LayoutName << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannelName");
		return(NULL);
	}

	if (Channel < 0 || Channel >= layout->GetIndex()) {
		gMrbLog->Err()	<< "Wrong channel number (layout \"" << LayoutName << "\") - " << Channel
						<< " (should be in [0," << layout->GetIndex() - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "GetChannelName");
		return(NULL);
	}

	TObjArray * ca = (TObjArray *) layout->GetAssignedObject();
	return(((TObjString *) ca->At(Channel))->GetString());
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

	TString cluName;

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

	TString cLine;
	Int_t lineNo = 0;
	Int_t nofClusters = 0;
	Int_t cluIdx = 0;
	TMrbDGFCluster * clu = NULL;
	while (kTRUE) {
		cLine.ReadLine(cFile, kFALSE);
		TString cmt = "";
		if (cFile.eof()) {
			cFile.close();
			if (clu && clu->GetNofMembers() > 0) this->AddNamedX(cluIdx, cluName.Data(), NULL, clu);
			return(nofClusters);
		}
		cLine.ReplaceAll("\t", " ");
		cLine = cLine.Strip(TString::kBoth);
		if (cLine.Length() == 0 || cLine(0) == '#' || cLine.BeginsWith("//")) continue;
		Int_t x = cLine.Index("#", 0);
		if (x > 0) {
			cmt = cLine(x, 1000);
			cLine = cLine(0, x);
			cLine = cLine.Strip(TString::kBoth);
		}
		x = cLine.Index("//", 0);
		if (x > 0) {
			cmt = cLine(x, 1000);
			cLine = cLine(0, x);
			cLine = cLine.Strip(TString::kBoth);
		}

		TObjArray * sLine = cLine.Tokenize(" \t");
		Int_t nIdx = sLine->GetEntries();

		Int_t nofModules = nIdx - kMrbDgfClusterNofItems;
		Int_t nofModsPerMember;
		Bool_t hasNames;
		switch (nofModules) {
			case 1:
			case 2: nofModsPerMember = 2; hasNames = kFALSE; break;
			case 3: nofModsPerMember = 4; hasNames = kFALSE; break;
			case 4:
				{
					TString x = ((TObjString *) sLine->At(kMrbDgfClusterNofItems + 3))->GetString();
					if (atoi(x.Data()) > 0) {
						nofModsPerMember = 4;
						hasNames = kFALSE;
					} else {
						nofModules = 2;
						nofModsPerMember = 2;
						hasNames = kTRUE;
					}
				}
				break;
			case 5:
			case 6:
			case 7: nofModsPerMember = 4; hasNames = kFALSE; break;
			case 8: nofModsPerMember = 4; nofModules = 4; hasNames = kTRUE; break;
			default:
				gMrbLog->Err()	<< "Wrong number of items in line " << lineNo << " - " << nIdx << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFile");
				delete sLine;
				return(-1);
		}

		Int_t cIdx = atoi(((TObjString *) sLine->At(kMrbDGFClusterIdx))->GetString().Data());
		if (cIdx != cluIdx) {
			if (clu) this->AddNamedX(cluIdx, cluName.Data(), NULL, clu);
			cluName = "clu";
			cluName += cIdx;
			clu = new TMrbDGFCluster(cIdx, cluName.Data());
			cluIdx = cIdx;
			nofClusters++;
		}

		Int_t cluNo = atoi(((TObjString *) sLine->At(kMrbDGFClusterNo))->GetString());
		clu->SetClusterNo(cluNo);

		TString capsId = ((TObjString *) sLine->At(kMrbDGFClusterCapsId))->GetString();
		capsId.ToUpper();
		Int_t capsNo = (Int_t) (capsId(0) - 'A');
		if (capsNo < 0 || capsNo > 2) {
			gMrbLog->Err()	<< "Wrong capsule id in " << lineNo << " - " << capsId
							<< " (should be A, B, or C)" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFile");
			delete sLine;
			continue;
		}

		Bool_t ok = kTRUE;
		TMrbDGFClusterMember * cluMem = new TMrbDGFClusterMember(cluIdx, cluNo, capsId(0));

		cluMem->SetNofModules(nofModules);
		cluMem->SetHex(atoi(((TObjString *) sLine->At(kMrbDGFClusterHex))->GetString().Data()));
		cluMem->SetVoltage(atoi(((TObjString *) sLine->At(kMrbDGFClusterVoltage))->GetString().Data()));
		cluMem->SetColor(((TObjString *) sLine->At(kMrbDGFClusterColor))->GetString().Data());
		cluMem->SetAF(((TObjString *) sLine->At(kMrbDGFClusterAF))->GetString().Data());
		if (!cluMem->SetSide(((TObjString *) sLine->At(kMrbDGFClusterSide))->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetHeight(((TObjString *) sLine->At(kMrbDGFClusterHeight))->GetString().Data())) ok = kFALSE;
		if (!cluMem->SetAngle(((TObjString *) sLine->At(kMrbDGFClusterAngle))->GetString().Data())) ok = kFALSE;
		cluMem->SetCrate(atoi(((TObjString *) sLine->At(kMrbDGFClusterCrate))->GetString().Data()));
		Int_t idx = kMrbDgfClusterNofItems;
		for (Int_t i = 0; i < nofModules; i++, idx++) {
			cluMem->SetSlot(i, atoi(((TObjString *) sLine->At(idx))->GetString().Data()));
		}
		if (hasNames) {
			for (Int_t i = 0; i < nofModules; i++, idx++) {
				cluMem->SetDgf(i, ((TObjString *) sLine->At(idx))->GetString().Data());
			}
		} else {
			for (Int_t i = 0; i < nofModules; i++) {
				TString dgfName = "undef";
				if (cluMem->GetSlot(i) > 0) {
					dgfName = "dgf";
					dgfName += cluIdx;
					dgfName += capsNo * nofModsPerMember + i + 1;
				}
				cluMem->SetDgf(i, dgfName.Data());
			}
		}
		if (cmt.Length() > 0) cluMem->SetCmt(cmt.Data());

		if (ok) clu->AddMember(cluMem);
		delete sLine;
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
	Out << Form("%3s %-4s %4s %4s %-10s %2s %-6s %-7s %-9s %s %s",
					"#",
					"id",
					"hex",
					"V",
					"color",
					"AF",
					"side",
					"height",
					"angle",
					"C",
					"Nxx/Dgfxx ...") << endl;
	Out << "----------------------------------------------------------------------------------------------" << endl;

	TMrbNamedX * nx = (TMrbNamedX *) this->First();
	while (nx) {
		TMrbDGFCluster * clu = (TMrbDGFCluster *) nx->GetAssignedObject();
		if (clu) clu->Print(Out);
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

