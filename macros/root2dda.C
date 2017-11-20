//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         root2dda.C
// @(#)Purpose:      Convert SNAKE data from root to dda
// Syntax:           .x root2dda.C(const Char_t * SnakeFile)
// Arguments:        Char_t * SnakeFile        -- SNAKE scan data file
// Description:      Convert SNAKE data from root to dda
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Feb 11 11:31:58 2004
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                root2dda.C
//                   Title:               Convert SNAKE data from root to dda
//                   Width:               
//                   NofArgs:             1
//                   Arg1.Name:           SnakeFile
//                   Arg1.Title:          SNAKE scan data file
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include "SetColor.h"

void root2dda(const Char_t * SnakeFile)
//>>_________________________________________________(do not change this line)
//
{
	gROOT->Macro("LoadUtilityLibs.C");
	gSystem->Load("$MARABOU/lib/libTSnkDDA0816.so");
	TFile * snakeFile = new TFile(SnakeFile);
	if (snakeFile->IsOpen()) {
		TSnkDDA0816 * snk = snakeFile->Get("DDA0816");
		if (snk == NULL) {
			cerr << setred << "root2dda.C: File \"" << SnakeFile << "\": No DDA0816 object found" << setblack << endl;
			return;
		} else {
			TString saveFile = SnakeFile;
			saveFile.ReplaceAll(".root", ".dda");
			TString cmd = "mv -f ";
			cmd += saveFile;
			cmd += " ";
			cmd += saveFile;
			cmd += ".bak 2>/dev/null";
			gSystem->Exec(cmd.Data());
			TMrbEnv * dda = new TMrbEnv(saveFile.Data());
			if (dda->IsZombie()) {
				cerr << setred << "root2dda.C: Can't open file - " << saveFile << setblack << endl;
				delete dda;
				return;
			}

			dda->SetPrefix("DDAControl");
// common settings
			dda->Set("Subdevice.Index", snk->GetSubdevice());
			dda->Set("Subdevice.Name", snk->GetSubdeviceName());
			dda->Set("PacerClock", snk->GetPacerClock());
			dda->Set("PreScale", snk->GetPreScaler(), 16);
			dda->Set("ClockSource", snk->GetClockSource(), 16);
			dda->Set("ScanMode", snk->GetLofScanModes()->FindByIndex(snk->GetScanMode()), 16);
			dda->Set("ScanPeriod", snk->GetScanInterval());
			dda->Set("Cycles", snk->GetCycleCount());
// calibration
			dda->Set("ChargeState", snk->GetChargeState());
			dda->Set("Energy", snk->GetEnergy());
// channel data
			TString chX = "XYHB";
			for (Int_t i = 0; i < 4; i++) {
				TSnkDDAChannel * chn = snk->GetDac(i);
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
				if (snk->IsXorY(i)) {
					dda->Set("SoftScale", chn->GetSoftScale());
					dda->Set("Offset", chn->GetOffset());
					dda->Set("Amplitude", chn->GetAmplitude());
					dda->Set("Increment", chn->GetIncrement());
					dda->Set("ScanProfile", snk->GetLofScanProfiles()->FindByIndex(chn->GetScanProfile()), 16);
					dda->Set("Calibration", chn->GetCalibration(), 6);
					dda->Set("Voltage", chn->GetVoltage());
					dda->Set("StepWidth", chn->GetStepWidth());
					dda->Set("Range", chn->GetRange());
					dda->Set("StopAt", chn->GetStopPos());
					dda->Set("Pos0", chn->GetPos0());
				}
			}
// save data
			dda->Save();
			delete dda;
		}
	} else {
		cerr << setred << "root2dda.C: File \"" << snakeFile << "\": Not a ROOT file" << setblack << endl;
		return;
	}
}
