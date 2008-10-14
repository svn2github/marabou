//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFControlData
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFControlData.cxx,v 1.19 2008-10-14 17:27:05 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TSystem.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbResource.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "DGFControlData.h"
#include "DGFControlCommon.h"

#include "SetColor.h"

ClassImp(DGFControlData)

extern TMrbLogger * gMrbLog;

DGFControlData::DGFControlData() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData
// Purpose:        Common data base
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fCAMACHost = "";
	fDefaultCrate = -1;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
// open ROOT's resource data base
	fRootrc = new TMrbResource("DGFControl:TMrbDGF", ".rootrc");

// initialize colors
	gClient->GetColorByName("black", fColorBlack);
	gClient->GetColorByName("blue", fColorDarkBlue);
	gClient->GetColorByName("lightblue2", fColorBlue);
	gClient->GetColorByName("gray75", fColorGray);
	gClient->GetColorByName("lightgoldenrod1", fColorGold);
	gClient->GetColorByName("antiquewhite1", fColorWhite);
	gClient->GetColorByName("darkseagreen", fColorGreen);
	gClient->GetColorByName("red", fColorRed);
	gClient->GetColorByName("yellow", fColorYellow);

// initialize fonts
	fNormalFont = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fBoldFont = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fBoldFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fBoldFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("DGFControlData");
		fBoldFont = fNormalFont;
	}		
	fSlantedFont = gEnv->GetValue("Gui.SlantedFont", "-adobe-helvetica-medium-o-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fSlantedFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fSlantedFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("DGFControlData");
		fSlantedFont = fNormalFont;
	}		

// global switches
	if (fRootrc->Get(".DebugMode", kFALSE))			fStatus |= DGFControlData::kDGFDebugMode;
	else											fStatus &= ~DGFControlData::kDGFDebugMode;
	if (fRootrc->Get(".VerboseMode", kFALSE))		fStatus |= DGFControlData::kDGFVerboseMode;
	else											fStatus &= ~DGFControlData::kDGFVerboseMode;
	if (fRootrc->Get(".ModNumGlobal", kTRUE))		fStatus |= DGFControlData::kDGFModNumGlobal;
	else											fStatus &= ~DGFControlData::kDGFModNumGlobal;
	if (fRootrc->Get(".OfflineMode", kFALSE))		fStatus |= DGFControlData::kDGFOfflineMode;
	else											fStatus &= ~DGFControlData::kDGFOfflineMode;
	if (fRootrc->Get(".SingleStepMode", kFALSE))	fStatus |= DGFControlData::kDGFEsoneSingleStepMode;
	else											fStatus &= ~DGFControlData::kDGFEsoneSingleStepMode;

// paths and filenames
	TString errMsg;

// open DGF specific resource data base
	TString rcFile = fRootrc->Get(".RcFile", ".DGFControl.rc");
	gSystem->ExpandPathName(rcFile);
	Bool_t ok = this->CheckAccess(rcFile.Data(), kDGFAccessRead, errMsg, kFALSE);
	if (ok) {
		fDgfrc = new TMrbResource("DGFControl", rcFile.Data());

		fSimulStartStop = fRootrc->Get(".StartStopSimultaneously", kFALSE);
		fSyncClocks = fRootrc->Get(".SynchronizeClocks", kFALSE);
		fIndivSwitchBusTerm = fRootrc->Get(".TerminateSwitchBusIndividually", kFALSE);
		fUserPSA = fRootrc->Get(".ActivateUserPSACode", kFALSE);

		fDataPath = fRootrc->Get(".DataPath", gSystem->WorkingDirectory());
		gSystem->ExpandPathName(fDataPath);
		this->CheckAccess(fDataPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

		fRunDataFile = fDataPath;
		fRunDataFile += "/";
		fRunDataFile += fRootrc->Get(".RunDataFile", "DGFRunData.%T-%R.root");
		gSystem->ExpandPathName(fRunDataFile);

		fLoadPath = fRootrc->Get(".LoadPath", "$MARABOU/data/xiadgf/v2.80");
		gSystem->ExpandPathName(fLoadPath);
		this->CheckAccess(fLoadPath.Data(), kDGFAccessDirectory, errMsg, kTRUE);

		fDSPCodeFile = fLoadPath;
		fDSPCodeFile += "/";

		TString path = fRootrc->Get(".DSPCode", "dsp/DGFcodeE.bin");
		fDSPCodeFile += path;
		gSystem->ExpandPathName(fDSPCodeFile);
		this->CheckAccess(fDSPCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

		fDSPParamsFile = fLoadPath;
		fDSPParamsFile += "/";

		path = fRootrc->Get(".ParamNames", "dsp/dfgcodeE.var");
		fDSPParamsFile += path;
		gSystem->ExpandPathName(fDSPParamsFile);
		this->CheckAccess(fDSPParamsFile.Data(), kDGFAccessRead, errMsg, kTRUE);

		fUPSAParamsFile = fLoadPath;
		fUPSAParamsFile += "/";

		path = fRootrc->Get(".UPSAParams", "dsp/upsaParams.var");
		fUPSAParamsFile += path;
		gSystem->ExpandPathName(fUPSAParamsFile);
		this->CheckAccess(fUPSAParamsFile.Data(), kDGFAccessRead, errMsg, kTRUE);

		fSystemFPGAConfigFile = fLoadPath;
		fSystemFPGAConfigFile += "/";

		path = fRootrc->Get(".SystemFPGACode", "Firmware/dgf4c.bin");
		fSystemFPGAConfigFile += path;
		gSystem->ExpandPathName(fSystemFPGAConfigFile);
		this->CheckAccess(fSystemFPGAConfigFile.Data(), kDGFAccessRead, errMsg, kTRUE);

		fFippiFPGAConfigFile[TMrbDGFData::kRevD] = fLoadPath;
		fFippiFPGAConfigFile[TMrbDGFData::kRevD] += "/";

		path = fRootrc->Get(".FippiFPGACode.RevD", "");
		if (path.Length() == 0) path = fRootrc->Get(".FippiFPGACode", "Firmware/fdgf4c4D.bin");
		fFippiFPGAConfigFile[TMrbDGFData::kRevD] += path;
		gSystem->ExpandPathName(fFippiFPGAConfigFile[TMrbDGFData::kRevD]);
		this->CheckAccess(fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data(), kDGFAccessRead, errMsg, kTRUE);

		fFippiFPGAConfigFile[TMrbDGFData::kRevE] = fLoadPath;
		fFippiFPGAConfigFile[TMrbDGFData::kRevE] += "/";

		path = fRootrc->Get(".FippiFPGACode.RevE", "");
		if (path.Length() == 0) path = fRootrc->Get(".FippiFPGACode", "Firmware/fdgf4c4E.bin");
		fFippiFPGAConfigFile[TMrbDGFData::kRevE] += path;
		gSystem->ExpandPathName(fFippiFPGAConfigFile[TMrbDGFData::kRevE]);
		this->CheckAccess(fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data(), kDGFAccessRead, errMsg, kTRUE);

		fDgfSettingsPath = fRootrc->Get(".SettingsPath", "../dgfSettings");
		gSystem->ExpandPathName(fDgfSettingsPath);
		this->CheckAccess(fDgfSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

		path = fRootrc->Get(".CptmCodeFile", "");
		if (path.Length() == 0) path = fRootrc->Get("TMrbCPTM.CodeFile", "cptm.rbf");
		fCptmCodeFile = path;
		gSystem->ExpandPathName(fCptmCodeFile);
		this->CheckAccess(fCptmCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

		path = fRootrc->Get(".CptmSettingsPath", "");
		if (path.Length() == 0) path = fRootrc->Get("TMrbCPTM.SettingsPath", "../cptmSettings");
		fCptmSettingsPath = path;
		gSystem->ExpandPathName(fCptmSettingsPath);
		this->CheckAccess(fCptmSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

		fLofChannels.SetName("DGF channels");
		fLofChannels.AddNamedX(kDGFChannelNumbers);
		fLofChannels.SetPatternMode();

		fHistPresent = new HistPresent();

		this->SetupModuleList();

//	clear list of modules to be updated
		fLofModulesToBeUpdated.Clear();

// come here after errors
	} else {
		this->MakeZombie();
	}
}

Bool_t DGFControlData::CheckIfStarted() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CheckIfStarted
// Purpose:        Check if dgf modules allocated & connected
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Loops thru table of dgfs to see if modules allocated
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * module;
	Bool_t ok;
	UInt_t modIdx;
	Int_t cl, modNo;

	if (fNofModules <= 0) {
		gMrbLog->Err() << "No DGF module defined" << endl;
		gMrbLog->Flush("DGFControlData", "CheckIfStarted");
		return(kFALSE);
	}

	ok = kTRUE;
	module = (DGFModule *) fLofModules.First();

	while (module) {
		modIdx = this->GetIndex(module->GetName(), cl, modNo);
		modIdx &= 0xFFFF;
		if (module->IsActive() && ((fPatInUse[cl] & modIdx) != 0) && module->GetAddr() == NULL) {
			gMrbLog->Err()	<< "DGF module not allocated - "
							<< module->GetName() << " in C" << module->GetCrate()
							<< ".N" << module->GetStation() << endl;
			gMrbLog->Flush("DGFControlData", "CheckIfStarted");
			ok = kFALSE;
		}
		module = (DGFModule *) fLofModules.After(module);
	}
	return(ok);
}

Int_t DGFControlData::SetupModuleList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::SetupModuleList()
// Purpose:        Create module list and key list
// Arguments:      --
// Results:        Int_t NofModules   -- number of dgf modules
// Exceptions:     
// Description:    Decodes env file and fills module list and key list    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofModules.Delete();
	for (Int_t i = 0; i < kNofClusters; i++) fLofModuleKeys[i].Delete();
	fLofClusters.Delete();
	fNofModules = 0;
	fNofClusters = 0;
	Int_t cl = 0;

	Int_t nofSevts = fDgfrc->Get(".NofSubevents", 0);
	for (Int_t i = 0; i < nofSevts; i++) {
		TString sevtName = fDgfrc->Get(".Subevent", Form("%d", i), "Name", "");
		if (!sevtName.IsNull()) {
			TString sn = sevtName;
			sn(0,1).ToUpper();
			TString sevtNuna = Form("%d:%s", i, sn.Data());
			TString sevtClass = fDgfrc->Get(".Subevent", sevtNuna.Data(), "ClassName", "");
			if (sevtClass.Index("TMrbSubevent_DGF", 0) == 0) {
				fNofClusters++;
				TString sevtTitle = "CLU";
				sevtTitle += fDgfrc->Get(".Subevent", sevtNuna.Data(), "ClusterSerial", 0);
				sevtTitle += " ";
				sevtTitle += fDgfrc->Get(".Subevent", sevtNuna.Data(), "ClusterColor", "");
				fLofModuleKeys[cl].SetName(sevtTitle.Data());
				fLofModuleKeys[cl].SetPatternMode();
				fLofClusters.AddNamedX(cl + 1, sevtName.Data(), sevtTitle.Data());
				fPatEnabled[cl] = 0;
				fPatInUse[cl] = 0;
				Int_t nofModules = fDgfrc->Get(".Subevent", sevtNuna.Data(), "NofModules", 0);
				if (nofModules > 0) {
					TString sevtLofModules = fDgfrc->Get(".Subevent", sevtNuna.Data(), "LofModules", "");
					Int_t modNo = 0;
					TString dgfName;
					Int_t crate;
					Int_t from = 0;
					while (sevtLofModules.Tokenize(dgfName, from, ":")) {
						TString dn = dgfName;
						dn(0,1).ToUpper();
						TString dgfNuna = Form("%d:%s", i, dn.Data());
						crate = fDgfrc->Get(".Module", dgfNuna.Data(), "Crate", 0);
						Int_t station = fDgfrc->Get(".Module", dgfNuna.Data(), "Station", 0);
						DGFModule * dgfModule = new DGFModule(dgfName.Data(), this->fCAMACHost.Data(), crate, station);
						if (!dgfModule->IsZombie())	{
							dgfModule->SetActive();
							fNofModules++;
							Bool_t isActive = fDgfrc->Get(".Module", dgfNuna.Data(), "IsActive", kFALSE);
							if (isActive) {
								fPatEnabled[cl] |= 0x1 << modNo;
								fPatInUse[cl] |= 0x1 << modNo;
							}
							Int_t serial = fDgfrc->Get(".Module", dgfNuna.Data(), "ClusterSerial", 0);
							Int_t dgfHex = fDgfrc->Get(".Module", dgfNuna.Data(), "ClusterHexNum", 0);
							TString dgfColor = fDgfrc->Get(".Module", dgfNuna.Data(), "ClusterColor", "");
							TString dgfSegment = fDgfrc->Get(".Module", dgfNuna.Data(), "ClusterSegments", "");

							TString dgfTitle = Form("CLU%d %d (%#x) %s %s", serial, dgfHex, dgfHex, dgfColor.Data(), dgfSegment.Data());
							dgfName += Form(" (%s)", fDgfrc->Get(".Module", dgfNuna.Data(), "Address", ""));
							dgfModule->SetClusterID(serial, dgfColor, dgfSegment, dgfHex);
							fLofModules.Add(dgfModule);
							fLofModuleKeys[cl].AddNamedX(this->ModuleIndex(cl, modNo), dgfName.Data(), dgfTitle.Data(), dgfModule);
						}
						modNo++;
					}
					Int_t n = modNo;
					for (Int_t j = n; j < kNofModulesPerCluster; j++, modNo++) {
						dgfName = sevtName;
						dgfName += "-void";
						dgfName += j + 1;
						DGFModule * dgfModule = new DGFModule(dgfName.Data(), "", crate, 1);
						dgfModule->SetActive(kFALSE);
						fLofModules.Add(dgfModule);
						fLofModuleKeys[cl].AddNamedX(this->ModuleIndex(cl, modNo), dgfName.Data(), "not used");
					}
				}
				cl++;
			}
		}
	}
	return(fNofModules);
}

DGFModule * DGFControlData::GetModule(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetModule
// Purpose:        Get module by name
// Arguments:      Char_t * ModuleName    -- module name
// Results:        DGFModule * Module     -- module defs
// Exceptions:     
// Description:    Finds module by its name.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * mp;
	TMrbNamedX * np;

	mp = (DGFModule *) fLofModules.FindObject(ModuleName);
	if (mp) return(mp);

	for (Int_t cl = 0; cl < kNofClusters; cl++) {
		np = fLofModuleKeys[cl].FindByName(ModuleName, TMrbLofNamedX::kFindUnique);
		if (np) return((DGFModule *) np->GetAssignedObject());
	}
	return(NULL);
};

DGFModule * DGFControlData::GetModule(Int_t GlobIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetModule
// Purpose:        Get module by its global index
// Arguments:      Int_t GlobIndex        -- global module index
// Results:        DGFModule * Module     -- module defs
// Exceptions:     
// Description:    Finds module by its index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	for (Int_t cl = 0; cl < kNofClusters; cl++) {
		np = fLofModuleKeys[cl].FindByIndex(GlobIndex);
		if (np) return((DGFModule *) np->GetAssignedObject());
	}
	return(NULL);
};

DGFModule * DGFControlData::GetModule(Int_t Cluster, Int_t ModNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetModule
// Purpose:        Get module by index
// Arguments:      Int_t Cluster          -- cluster number
//                 Int_t ModNo            -- module number within cluster
// Results:        DGFModule * Module     -- module defs
// Exceptions:     
// Description:    Finds module by its index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	np = fLofModuleKeys[Cluster].FindByIndex(0x1 << ModNo, 0xFFFF);
	if (np) return((DGFModule *) np->GetAssignedObject());
	return(NULL);
};

Int_t DGFControlData::GetChannel(Int_t ChannelIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetChannel
// Purpose:        Get channel number by index
// Arguments:      Int_t ChannelIndex     -- channel index
// Results:        Int_t Channel          -- channel number
// Exceptions:     
// Description:    Finds channel by its index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	if ((np = fLofChannels.FindByIndex(ChannelIndex)) == NULL) return(-1);
	else												return(fLofChannels.IndexOf(np));
};

Int_t DGFControlData::ChannelIndex2Number(Int_t ChannelIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::ChannelIndex2Number
// Purpose:        Convert channel index to channel number
// Arguments:      Int_t ChannelIndex     -- channel index
// Results:        Int_t Channel          -- channel number
// Exceptions:     
// Description:    Returns channel number fopr a given index (bits 12-15).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = 0; i < TMrbDGFData::kNofChannels; i++) {
		if (ChannelIndex & 1) return(i);
		ChannelIndex >>= 1;
	};
	return(-1);
}

Int_t DGFControlData::GetChannelIndex(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetChannelIndex
// Purpose:        Convert channel number to channel index
// Arguments:      Int_t Channel          -- channel number
// Results:        Int_t ChannelIndex     -- channel index
// Exceptions:     
// Description:    Finds module by its index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	np = (TMrbNamedX *) fLofChannels.At(Channel);
	return(np ? np->GetIndex() : -1);
};

TMrbNamedX * DGFControlData::GetKey(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetKey
// Purpose:        Get module key
// Arguments:      Char_t * ModuleName    -- module name
// Results:        TMrbNamedX * ModuleKey -- key
// Exceptions:     
// Description:    Returns the module key (index, name, title, object)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	for (Int_t cl = 0; cl < kNofClusters; cl++) {
		np = fLofModuleKeys[cl].FindByName(ModuleName, TMrbLofNamedX::kFindUnique);
		if (np) return(np);
	}
	return(NULL);
};

UInt_t DGFControlData::GetIndex(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetIndex
// Purpose:        Get module index
// Arguments:      Char_t * ModuleName    -- module name
// Results:        UInt_t ModuleIndex     -- index
// Exceptions:     
// Description:    Returns the module index
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;

	np = this->GetKey(ModuleName);
	return((np == NULL) ? 0xFFFFFFFF : (UInt_t) np->GetIndex());
}

TMrbNamedX * DGFControlData::GetKey(const Char_t * ModuleName, Int_t & Cluster, Int_t & ModuleNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetKey
// Purpose:        Get module index
// Arguments:      Char_t * ModuleName    -- module name
// Results:        TMrbNamedX * ModuleKey -- key
//                 Int_t & Cluster        -- cluster number
//                 Int_t & ModuleNumber   -- module number
// Exceptions:     
// Description:    Returns module key, cluster number, and module number
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t modIdx, pat;
	TMrbNamedX * np;

	for (Int_t cl = 0; cl < kNofClusters; cl++) {
		np = fLofModuleKeys[cl].FindByName(ModuleName, TMrbLofNamedX::kFindUnique);
		if (np) {
			modIdx = (UInt_t) np->GetIndex();
			pat = modIdx >> 16; Cluster = 0; while (pat >>= 1) Cluster++;
			pat = modIdx & 0xFFFF; ModuleNumber = 0; while (pat >>= 1) ModuleNumber++;
			return(np);
		}
	}
	Cluster = -1;
	ModuleNumber = -1;
	return(NULL);
};

UInt_t DGFControlData::GetIndex(const Char_t * ModuleName, Int_t & Cluster, Int_t & ModuleNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetIndex
// Purpose:        Get module index
// Arguments:      Char_t * ModuleName    -- module name
// Results:        UInt_t Index           -- module index
//                 Int_t & Cluster        -- cluster number
//                 Int_t & ModuleNumber   -- module number
// Exceptions:     
// Description:    Returns the module index
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;

	np = this->GetKey(ModuleName, Cluster, ModuleNumber);
	return((np == NULL) ? 0xFFFFFFFF : (UInt_t) np->GetIndex());
}
	
Bool_t DGFControlData::ModuleInUse(DGFModule * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::ModuleInUse
// Purpose:        Check if module in use
// Arguments:      DGFModule * Module     -- module
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Tests if module is active & selected.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t modIdx;
	Int_t cl, modNo;

	if (!Module->IsActive()) return(kFALSE);
	modIdx = this->GetIndex(Module->GetName(), cl, modNo);
	modIdx &= 0xFFFF;
	return((modIdx & this->GetPatInUse(cl)) != 0);
}

Bool_t DGFControlData::ModuleInUse(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::ModuleInUse
// Purpose:        Check if module in use
// Arguments:      Char_t * ModuleName     -- module name
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Tests if module is active & selected.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * module;
	module = this->GetModule(ModuleName);
	return((module == NULL) ? kFALSE : this->ModuleInUse(module));
}

void DGFControlData::SetSelectedModule(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::SetSelectedModule
// Purpose:        Set module selection
// Arguments:      Char_t * ModuleName     -- module name
// Results:        --
// Exceptions:     
// Description:    Defines given module to be 'selected'.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * np;
	np = this->GetKey(ModuleName);
	if (np != NULL) {
		this->SetSelectedModuleIndex(np->GetIndex());
	} else {
		gMrbLog->Err()	<< "DGF module not found - " << ModuleName << endl;
		gMrbLog->Flush("DGFControlData", "SetSelectedModule");
	}
}

TMrbLofNamedX *  DGFControlData::CopyKeyList(TMrbLofNamedX * KeyList, Int_t Cluster, Int_t NofClusters, Bool_t ArrayMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::CopyKeyList
// Purpose:        Copy part of key list
// Arguments:      TMrbLofNamedX * KeyList    -- where to put the copy to
//                 Int_t Cluster              -- cluster index
//                 Int_t NofClusters          -- number of clusters to be copied
//                 Bool_t ArrayMode           -- kTRUE if KeyList points to an array
//                                               of TMrbLofNamedX objects
// Results:        TMrbLofNamedX * KeyList    
// Exceptions:     
// Description:    Copies key list for a given cluster.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX * kpi;
	TMrbLofNamedX * kpo;

	if (NofClusters == -1) {
		NofClusters = fNofClusters;
		kpi = fLofModuleKeys;
	} else {
		kpi = &fLofModuleKeys[Cluster];
	}
	if (NofClusters == 1) ArrayMode = kTRUE;
	kpo = KeyList;
	if (ArrayMode) {
		for (Int_t cl = 0; cl < NofClusters; cl++, kpi++, kpo++) {
			kpo->Delete();
			kpo->SetName(kpi->GetName());
			kpo->SetPatternMode();
			kpo->AddNamedX(kpi);
		}
	} else {
		KeyList->Delete();
		KeyList->SetName("DGF cluster");
		KeyList->SetPatternMode();
		for (Int_t cl = 0; cl < NofClusters; cl++, kpi++) KeyList->AddNamedX(kpi);
	}
	return(KeyList);
};

TMrbLofNamedX * DGFControlData::CopyKeyList(TMrbLofNamedX * KeyList, Int_t Cluster, UInt_t Pattern, Bool_t ClearFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::CopyKeyList
// Purpose:        Copy part of key list
// Arguments:      TMrbLofNamedX * KeyList    -- where to put the copy to
//                 Int_t Cluster              -- cluster index
//                 UInt_t Pattern             -- pattern to be matched
//                 Bool_t ClearFlag           -- clear list
// Results:        TMrbLofNamedX * KeyList    
// Exceptions:     
// Description:    Copies key list for a given cluster.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX * kpi;
	TMrbNamedX * nx;
	UInt_t idx;

	kpi = &fLofModuleKeys[Cluster];
	if (ClearFlag) KeyList->Delete();
	KeyList->SetName("DGF cluster");
	KeyList->SetPatternMode();
	nx = (TMrbNamedX *) kpi->First();
	while (nx) {
		idx = (UInt_t) nx->GetIndex() & 0xFFFF;
		if (idx & Pattern) KeyList->AddNamedX(nx);
		nx = (TMrbNamedX *) kpi->After(nx);
	}
	return(KeyList);
};

Bool_t DGFControlData::CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::CheckAccess
// Purpose:        Check access to file or path
// Arguments:      Char_t * FileOrPath    -- file or path spec
//                 Int_t AccessMode       -- access mode: kDGFAccessXXX
//                 TString & ErrMsg       -- resulting error message
//                 Bool_t WarningOnly     -- kTRUE -> warning
// Results:        kTRUE/kFALSE  
// Exceptions:     
// Description:    Checks if file or path exists and is readable/writable
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	ErrMsg = "";
	Bool_t ok = kTRUE;

	if (AccessMode & kDGFAccessRegular && !ux.IsRegular(FileOrPath)) {
		ErrMsg = "Not a regular file - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	} else if (AccessMode & kDGFAccessDirectory && !ux.IsDirectory(FileOrPath)) {
		ErrMsg = "Not a directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	if (AccessMode & kDGFAccessRead && !ux.HasReadPermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No such file or directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}
	if (AccessMode & kDGFAccessWrite && !ux.HasWritePermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No write access to file/directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	return(ok);
}

void DGFControlData::AddToUpdateList(DGFModule * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::AddToUpdateList
// Purpose:        Add a module to update list
// Arguments:      DGFModule * Module   -- module
// Results:        --
// Exceptions:     
// Description:    Adds module to update list. Modules in this list will
//                 call thbeir method WriteParamMemory() later on to update their
//                 param settings as well as fpgas.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Module == NULL || Module->GetAddr() == NULL) return;
	for (Int_t i = 0; i < fLofModulesToBeUpdated.GetEntriesFast(); i++) {
		if (Module == fLofModulesToBeUpdated[i]) return;
	}
	fLofModulesToBeUpdated.Add(Module);
}

void DGFControlData::UpdateParamsAndFPGAs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::UpdateParamsAndFPGAs
// Purpose:        Update params and fpgas
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Calls method WriteParamMemory() for each module in list
//                 thus writing params to memory and updating fpga settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t nofUpdates = fLofModulesToBeUpdated.GetEntriesFast();
	if (nofUpdates > 0) {
		cout << setmagenta << "Updating params & FPGAs: ";
		for (Int_t i = 0; i < nofUpdates; i++) {
			DGFModule * module = (DGFModule *) fLofModulesToBeUpdated[i];
			if (module) {
				TMrbDGF * dgf = module->GetAddr();
				if (dgf) {
					cout << setmagenta << dgf->GetName() << " " << ends;
					dgf->WriteParamMemory(kTRUE);
				}
			}
		}
		cout << setblack << endl;
	}
	fLofModulesToBeUpdated.Clear();
}
