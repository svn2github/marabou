//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         dgfStart.C
// @(#)Purpose:      Perform (cold) start for DGF-4C / ESONE
// Syntax:           .x dgfStart.C(const Char_t * HostName,
//                               const Char_t * Module,
//                               const Char_t * StartMode)
// Arguments:        Char_t * HostName         -- Host name
//                   Char_t * Module           -- Module name (dgfN, N=Camac Station)
//                   Char_t * StartMode        -- Start mode
// Description:      Perform (cold) start for DGF-4C / ESONE
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Feb 20 15:35:27 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                dgfStart.C
//                   Title:               Perform (cold) start for DGF-4C / ESONE
//                   Width:               
//                   NofArgs:             3
//                   Arg1.Name:           HostName
//                   Arg1.Title:          Host name
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Combo
//                   Arg1.Width:          100
//                   Arg1.Default:        <DGFControl.DefaultHost
//                   Arg1.Values:         ppc-0:ppc-1:ppc-2:ppc-3:ppc-4:ppc-5:ppc-6:ppc-7:ppc-8
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Module
//                   Arg2.Title:          Module name (dgfN, N=Camac Station)
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Combo
//                   Arg2.Width:          100
//                   Arg2.Default:        <TMrbDGF.LastSelected
//                   Arg2.Values:         *ALL*:dgf1:dgf3:dgf5:dgf7:dgf9:dgf10:dgf11:dgf13:dgf15:dgf17:dgf19:dgf21:dgf23
//                   Arg2.AddLofValues:   Yes
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           StartMode
//                   Arg3.Title:          Start mode
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      Radio
//                   Arg3.Default:        connect
//                   Arg3.Values:         connect:warm:cold
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>

void dgfStart(const Char_t * HostName = "<DGFControl.DefaultHost",
           const Char_t * Module = "<TMrbDGF.LastSelected",
           const Char_t * ModuleValues = "*ALL*:dgf1:dgf3:dgf5:dgf7:dgf9:dgf11:dgf13:dgf15:dgf17:dgf19:dgf21:dgf23",
           const Char_t * StartMode = "connect")
//>>__________________________________________________(do not change this line)
//
{
	TString dgfHost, dgfName, dgfAddr;
	TMrbLofNamedX dgfNameArray;
	TMrbNamedX * nx;
	Int_t n;
	Bool_t startAll;

	TString sMode = StartMode;
	gROOT->Macro("LoadColors.C");
	gROOT->Macro("LoadDGFLibs.C");

	dgfHost = HostName;
	if (dgfHost.Length() == 0) dgfHost = gEnv->GetValue("TMrbDGF.HostName", "");
	if (dgfHost.Length() == 0) {
		cout << "Enter host name          : " << flush;
		dgfHost.ReadLine(cin);
	}
	dgfName = Module;
	if (dgfName.Length() == 0) dgfName = gEnv->GetValue("TMrbDGF.LastSelected", "");
	if (dgfName.Length() == 0) {
		cout << "Enter module name (dgfN): " << flush;
		dgfAddr.ReadLine(cin);
	}

	if (sMode.CompareTo("cold") == 0) {
		TMrbEsone * cold = new TMrbEsone();
		if (cold->IsZombie()) {
			cerr	<< setred
					<< "dgfStart.C: Can't connect to ESONE server @ " << dgfHost << ". Sorry."
					<< setblack << endl;
			return;
		}
		cout << "[dgfStart.C: Cold start for ESONE server / MBS @ " << dgfHost << "]" << endl;
		if (!cold->StartServer(dgfHost.Data())) return;
	}

	startAll = (dgfName.CompareTo("*ALL*") == 0);

	dgfNameArray.AddNamedX(ModuleValues);
	nx = (TMrbNamedX *) dgfNameArray[1];
	while (nx) {
		dgfName = nx->GetName();
		if (startAll || dgfName.CompareTo(Module) == 0) {
			n = dgfName.Index("dgf", 0);
			dgfAddr = dgfName(n + 3, dgfName.Length());
			dgfAddr.Prepend("c1.n");

			cout << "[dgfStart.C: Connecting to DGF module in " << dgfAddr << "]" << endl;
			gEnv->SetValue("TMrbDGF.LastSelected", dgfName.Data(), kEnvChange);
			dgf = new TMrbDGF(dgfName.Data(), dgfHost.Data(), dgfAddr.Data());
			if (!dgf.IsConnected()) {
				cerr	<< setred
						<< "dgfStart.C: Can't connect to ESONE server @ " << dgfHost << ". Sorry."
						<< setblack << endl;
				return;
			}
			dgf->Data()->ReadNameTable();
			dgf->Data()->ReadFPGACode("SystemFPGA");
			dgf->Data()->ReadFPGACode("FippiFPGA");
			dgf->Data()->ReadDSPCode();
			if (sMode.CompareTo("connect") != 0) {
				dgf->DownloadFPGACode("SystemFPGA");
				dgf->DownloadFPGACode("FippiFPGA");
				dgf->SetSwitchBus("Single", "Single");
				dgf->DownloadDSPCode();
				dgf->Wait();
			}
			dgf->ReadParamMemory();
			dgf->ClearChannelMask();
			dgf->ClearTriggerMask();
			dgf->PrintParams(-1);
			dgf->Print();
			gROOT->Append(dgf);
			gEnv->SetValue("TMrbDGF.LastSelected", dgfName.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) dgfNameArray.After(nx);
	}
	return;
}
