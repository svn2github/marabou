//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFControlData
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFControlData.cxx,v 1.6 2005-05-04 13:36:57 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TSystem.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbEnv.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "DGFControlData.h"
#include "DGFControlCommon.h"

#include "SetColor.h"

ClassImp(DGFControlData)

extern TMrbLogger * gMrbLog;

DGFControlData::DGFControlData() : TNamed("DGFControlData", "DGFControlData") {
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

	TMrbEnv env;

	fCAMACHost = "";
	fDefaultCrate = -1;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
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
	if (gEnv->GetValue("DGFControl.DebugMode", kFALSE))		fStatus |= DGFControlData::kDGFDebugMode;
	else													fStatus &= ~DGFControlData::kDGFDebugMode;
	if (gEnv->GetValue("DGFControl.VerboseMode", kFALSE))	fStatus |= DGFControlData::kDGFVerboseMode;
	else													fStatus &= ~DGFControlData::kDGFVerboseMode;
	if (gEnv->GetValue("DGFControl.ModNumGlobal", kTRUE))	fStatus |= DGFControlData::kDGFModNumGlobal;
	else													fStatus &= ~DGFControlData::kDGFModNumGlobal;
	if (gEnv->GetValue("DGFControl.OfflineMode", kFALSE))	fStatus |= DGFControlData::kDGFOfflineMode;
	else													fStatus &= ~DGFControlData::kDGFOfflineMode;

// paths and filenames
	TString errMsg;
	Bool_t panic = kFALSE;

	env.Find(fDataPath, "DGFControl:TMrbDGF", "DataPath", gSystem->WorkingDirectory());
	gSystem->ExpandPathName(fDataPath);
	this->CheckAccess(fDataPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

	fRunDataFile = fDataPath;
	fRunDataFile += "/";
	fRunDataFile += gEnv->GetValue("DGFControl.RunDataFile", "DGFRunData.%T-%R.root");
	gSystem->ExpandPathName(fRunDataFile);

	env.Find(fLoadPath, "DGFControl:TMrbDGF", "LoadPath", "$MARABOU/data/xiadgf/v2.80");
	gSystem->ExpandPathName(fLoadPath);
	this->CheckAccess(fLoadPath.Data(), kDGFAccessDirectory, errMsg, kTRUE);

	fDSPCodeFile = fLoadPath;
	fDSPCodeFile += "/";
	TString path;
	env.Find(path, "DGFControl:TMrbDGF", "DSPCode", "dsp/DGFcodeE.bin");
	fDSPCodeFile += path;
	gSystem->ExpandPathName(fDSPCodeFile);
	this->CheckAccess(fDSPCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	fDSPParamsFile = fLoadPath;
	fDSPParamsFile += "/";
	env.Find(path, "DGFControl:TMrbDGF", "ParamNames", "dsp/dfgcodeE.var");
	fDSPParamsFile += path;
	gSystem->ExpandPathName(fDSPParamsFile);
	this->CheckAccess(fDSPParamsFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	fSystemFPGAConfigFile = fLoadPath;
	fSystemFPGAConfigFile += "/";
	env.Find(path, "DGFControl:TMrbDGF", "SystemFPGACode", "Firmware/dgf4c.bin");
	fSystemFPGAConfigFile += path;
	gSystem->ExpandPathName(fSystemFPGAConfigFile);
	this->CheckAccess(fSystemFPGAConfigFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	fFippiFPGAConfigFile[TMrbDGFData::kRevD] = fLoadPath;
	fFippiFPGAConfigFile[TMrbDGFData::kRevD] += "/";
	env.Find(path, "DGFControl:TMrbDGF", "FippiFPGACode.RevD", "");
	if (path.Length() == 0) env.Find(path, "DGFControl:TMrbDGF", "FippiFPGACode", "Firmware/fdgf4c4D.bin");
	fFippiFPGAConfigFile[TMrbDGFData::kRevD] += path;
	gSystem->ExpandPathName(fFippiFPGAConfigFile[TMrbDGFData::kRevD]);
	this->CheckAccess(fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data(), kDGFAccessRead, errMsg, kTRUE);

	fFippiFPGAConfigFile[TMrbDGFData::kRevE] = fLoadPath;
	fFippiFPGAConfigFile[TMrbDGFData::kRevE] += "/";
	env.Find(path, "DGFControl:TMrbDGF", "FippiFPGACode.RevE", "");
	if (path.Length() == 0) env.Find(path, "DGFControl:TMrbDGF", "FippiFPGACode", "Firmware/fdgf4c4E.bin");
	fFippiFPGAConfigFile[TMrbDGFData::kRevE] += path;
	gSystem->ExpandPathName(fFippiFPGAConfigFile[TMrbDGFData::kRevE]);
	this->CheckAccess(fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data(), kDGFAccessRead, errMsg, kTRUE);

	env.Find(fDgfSettingsPath, "DGFControl:TMrbDGF", "SettingsPath", "../dgfSettings");
	gSystem->ExpandPathName(fDgfSettingsPath);
	this->CheckAccess(fDgfSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

	env.Find(path, "DGFControl", "CptmCodeFile", "");
	if (path.Length() == 0) env.Find(path, "TMrbCPTM", "CodeFile", "cptm.rbf");
	fCptmCodeFile = path;
	gSystem->ExpandPathName(fCptmCodeFile);
	this->CheckAccess(fCptmCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	env.Find(path, "DGFControl", "CptmSettingsPath", "");
	if (path.Length() == 0) env.Find(path, "TMrbCPTM", "SettingsPath", "../cptmSettings");
	fCptmSettingsPath = path;
	gSystem->ExpandPathName(fCptmSettingsPath);
	this->CheckAccess(fCptmSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fHistPresent = new HistPresent();

	if (panic) {
		gMrbLog->Err() << "Can't continue. Correct errors, then start over. Sorry" << endl;
		gMrbLog->Flush(this->ClassName());
		gSystem->Exit(1);
	}

	this->SetupModuleList();
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

	Int_t nofModules, nofSevts;
	DGFModule * dgfModule;
	TMrbString intStr;
	TMrbString sevtName, sevtTitle, sevtClass, sevtLofModules;
	TMrbString dgfName, dgfTitle, dgfColor, dgfSegment, dgfClass, dgfAddr;
	TObjArray lofModules;
	Int_t crate, station, serial, dgfHex;
	Bool_t isActive;
	Int_t modNo, cl;
	Int_t nerr;
	
	fLofModules.Delete();
	for (Int_t i = 0; i < kNofClusters; i++) fLofModuleKeys[i].Delete();
	fLofClusters.Delete();
	fNofModules = 0;
	fNofClusters = 0;
	cl = 0;

	nofSevts = gEnv->GetValue("DGFControl.NofSubevents", 0);
	for (Int_t i = 0; i < nofSevts; i++) {
		this->GetResource(sevtName, "DGFControl.Subevent", i, NULL, "Name");
		if (sevtName.Length() > 0) {
			this->GetResource(sevtClass, "DGFControl.Subevent", i, sevtName.Data(), "ClassName");
			if (sevtClass.Index("TMrbSubevent_DGF", 0) == 0) {
				fNofClusters++;
				sevtTitle = "CLU";
				sevtTitle += this->GetResource(serial, "DGFControl.Subevent", i, sevtName.Data(), "ClusterSerial");
				sevtTitle += " ";
				sevtTitle += this->GetResource(dgfColor, "DGFControl.Subevent", i, sevtName.Data(), "ClusterColor");
				fLofModuleKeys[cl].SetName(sevtTitle.Data());
				fLofModuleKeys[cl].SetPatternMode();
				fLofClusters.AddNamedX(cl + 1, sevtName.Data(), sevtTitle.Data());
				fPatEnabled[cl] = 0;
				fPatInUse[cl] = 0;
				this->GetResource(nofModules, "DGFControl.Subevent", i, sevtName.Data(), "NofModules");
				if (nofModules > 0) {
					this->GetResource(sevtLofModules, "DGFControl.Subevent", i, sevtName.Data(), "LofModules");
					lofModules.Delete();
					Int_t n = sevtLofModules.Split(lofModules);
					modNo = 0;
					for (Int_t j = 0; j < n; j++, modNo++) {
						dgfName = ((TObjString *) lofModules.At(j))->GetString();
						this->GetResource(crate, "DGFControl.Module", i, dgfName.Data(), "Crate");
						this->GetResource(station, "DGFControl.Module", i, dgfName.Data(), "Station");
						this->GetResource(dgfAddr, "DGFControl.Module", i, dgfName.Data(), "Address");
						dgfModule = new DGFModule(	dgfName.Data(), this->fCAMACHost.Data(), crate, station);
						if (!dgfModule->IsZombie())	{
							dgfModule->SetActive();
							fNofModules++;
							if (this->GetResource(isActive, "DGFControl.Module", i, dgfName.Data(), "IsActive")) {
								fPatEnabled[cl] |= 0x1 << modNo;
								fPatInUse[cl] |= 0x1 << modNo;
							}
							dgfTitle = "CLU";
							dgfTitle += this->GetResource(serial, "DGFControl.Module", i, dgfName.Data(), "ClusterSerial");
							dgfTitle += " ";
							this->GetResource(dgfHex, "DGFControl.Module", i, dgfName.Data(), "ClusterHexNum", 16);
							TMrbString h;
							h.FromInteger(dgfHex, 0, ' ', 16);
							dgfTitle += h;
							dgfTitle += " ";
							dgfTitle += this->GetResource(dgfColor, "DGFControl.Module", i, dgfName.Data(), "ClusterColor");
							dgfTitle += " ";
							dgfTitle += this->GetResource(dgfSegment, "DGFControl.Module", i, dgfName.Data(), "ClusterSegments");
							dgfName += " (";
							dgfName += dgfAddr;
							dgfName += ")";
							dgfModule->SetClusterID(serial, dgfColor, dgfSegment, dgfHex);
							fLofModules.Add(dgfModule);
							fLofModuleKeys[cl].AddNamedX(this->ModuleIndex(cl, modNo), dgfName.Data(), dgfTitle.Data(), dgfModule);
						}
					}
					for (Int_t j = n; j < kNofModulesPerCluster; j++, modNo++) {
						dgfName = sevtName;
						dgfName += "-void";
						dgfName += j + 1;
						dgfModule = new DGFModule(dgfName.Data(), "", crate, 1);
						if (dgfModule->IsZombie())	nerr++;
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

const Char_t * DGFControlData::GetResource(TString & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      TString & Result   -- where to put resulting value
//                 Char_t * Prefix    -- prefix "DGFControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
// Results:        Char_t * Result    -- resource value
// Exceptions:     
// Description:    Reads an ascii resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "DGFControl.Module.1.Address"
//                          and for      "DGFControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr;
		
	Result = "";
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		Result = gEnv->GetValue(resStr.Data(), "");
	}
	if (Result.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		Result = gEnv->GetValue(resStr.Data(), "");
	}
	return(Result.Data());
}

Int_t DGFControlData::GetResource(Int_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      Int_t & Result     -- where to put resulting value
//                 Char_t * Prefix    -- prefix "DGFControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
//                 Int_t Base         -- numerical base
// Results:        Int_t Result       -- resource value
// Exceptions:     
// Description:    Reads an integer resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "DGFControl.Module.1.Address"
//                          and for      "DGFControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr, resVal;
		
	resVal = "";
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	resVal.ToInteger(Result, Base);
	return(Result);
}

Bool_t DGFControlData::GetResource(Bool_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      Bool_t & Result    -- where to put resulting value
//                 Char_t * Prefix    -- prefix "DGFControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
// Results:        Int_t Result       -- resource value
// Exceptions:     
// Description:    Reads an boolean resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "DGFControl.Module.1.Address"
//                          and for      "DGFControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr, resVal;
		
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.CompareTo("TRUE") == 0 || resVal.CompareTo("true") == 0) Result = kTRUE; else Result = kFALSE;
	return(Result);
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

