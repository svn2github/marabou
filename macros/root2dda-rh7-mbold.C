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

#include <iostream.h>
#include <iomanip.h>

void root2dda(const Char_t * SnakeFile)
//>>_________________________________________________(do not change this line)
//
{
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadColors.C");
	gSystem->Load("$MARABOU/lib/libTSnkDDA0816.so");

	TString saveFile = SnakeFile;
	TFile * snakeFile = new TFile(saveFile.Data());
	if (snakeFile->IsOpen()) {
		TSnkDDA0816 * snk = snakeFile->Get("DDA0816");
		if (snk == NULL) {
			cerr << setred << "root2dda.C: File \"" << saveFile << "\": No DDA0816 object found" << setblack << endl;
			continue;
		} else {
			saveFile.ReplaceAll(".root", ".dda");
			TMrbEnv * dda = new TMrbEnv(saveFile.Data());
			if (dda->IsZombie()) {
				cerr << setred << "root2dda.C: Can't open file - " << saveFile << setblack << endl;
				delete dda;
				continue;
			}
			dda->SetPrefix("DDAControl");
// common settings
			dda->Set("Subdevice.Index", snk->GetSubdevice());
			dda->Set("Subdevice.Name", snk->GetSubdeviceName());
			dda->Set("PacerClock", snk->GetPacerClock());
			dda->Set("PreScale", (snk->GetPreScaler())->GetIndex(), 16);
			dda->Set("ClockSource", (snk->GetClockSource())->GetIndex(), 16);
			dda->Set("ScanMode", (Int_t) snk->GetScanMode(), 16);
			dda->Set("ScanPeriod", snk->GetScanInterval());
			dda->Set("Cycles", 0);
// calibration
			dda->Set("ChargeState", snk->GetChargeState());
			dda->Set("Energy", snk->GetEnergy());
// channel data
			TString chX = "XYH";
			for (Int_t i = 0; i < 3; i++) {
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
					dda->Set("ScanProfile", (Int_t) chn->GetScanProfile(), 16);
					dda->Set("Calibration", (i == 0) ? "0.484155" : "1.95067"); break;
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
