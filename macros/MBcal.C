//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         MBcal.C
// @(#)Purpose:      Calibrate miniball histograms
// Syntax:           .x MBcal.C(Int_t CalibSource,
//                               const Char_t * FirstHisto,
//                               const Char_t * LastHisto,
//                               const Char_t * CalFile,
//                               const Char_t * ResFile,
//                               const Char_t * PrecalFile,
//                               Bool_t VerboseFlag,
//                               Int_t SigmaPeakFind,
//                               Int_t PercentagePeakFind,
//                               Bool_t FitFlag,
//                               Int_t SigmaFit,
//                               Int_t FitRange,
//                               Int_t HistoLowLim,
//                               Int_t HistoUpperLim,
//                               Bool_t DebugFlag,
//                               Bool_t DebugFlagPeakFind)
// Arguments:        Int_t CalibSource         -- Calibration source
//                   Char_t * FirstHisto       -- Histo file / first histo
//                   Char_t * LastHisto        -- Histo file / last histo
//                   Char_t * CalFile          -- Calibration output file (*.cal)
//                   Char_t * ResFile          -- Results file (*.res)
//                   Char_t * PrecalFile       -- Precalibration file (needed if Eu152)
//                   Bool_t VerboseFlag        -- Verbose output
//                   Int_t SigmaPeakFind       -- Sigma for PeakFind() [bins]
//                   Int_t PercentagePeakFind  -- Relative percentage for PeakFind()
//                   Bool_t FitFlag            -- Peaks to be fitted
//                   Int_t SigmaFit            -- Sigma for FitSinglePeak()
//                   Int_t FitRange            -- Fit range (bins)
//                   Int_t HistoLowLim         -- Lower limit
//                   Int_t HistoUpperLim       -- Upper limit
//                   Bool_t DebugFlag          -- Debug
//                   Bool_t DebugFlagPeakFind  -- Debug for PeakFind()
// Description:      Calibrate miniball histograms
// @(#)Author:       miniball
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Dec 14 09:17:11 2004
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                MBcal.C
//                   Title:               Calibrate miniball histograms
//                   Width:               800
//                   NofArgs:             16
//                   Arg1.Name:           CalibSource
//                   Arg1.Title:          Calibration source
//                   Arg1.Type:           Int_t
//                   Arg1.EntryType:      Radio
//                   Arg1.Default:        Co60
//                   Arg1.Values:         Co60=0:Eu152=1:TripleAlpha=2
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           FirstHisto
//                   Arg2.Title:          Histo file / first histo
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      FObjCombo
//                   Arg2.Width:          400
//                   Arg2.Values:         ROOT files:*.root
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           LastHisto
//                   Arg3.Title:          Histo file / last histo
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      FObjCombo
//                   Arg3.Width:          400
//                   Arg3.Values:         ROOT files:*.root
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           CalFile
//                   Arg4.Title:          Calibration output file (*.cal)
//                   Arg4.Type:           Char_t *
//                   Arg4.EntryType:      File
//                   Arg4.Width:          400
//                   Arg4.Default:        MBcal.cal
//                   Arg4.Values:         Calib files:*.cal
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           ResFile
//                   Arg5.Title:          Results file (*.res)
//                   Arg5.Type:           Char_t *
//                   Arg5.EntryType:      File
//                   Arg5.Width:          400
//                   Arg5.Default:        MBcal.res
//                   Arg5.Values:         Results files:*.res
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           PrecalFile
//                   Arg6.Title:          Precalibration file (needed if Eu152)
//                   Arg6.Type:           Char_t *
//                   Arg6.EntryType:      File
//                   Arg6.Width:          400
//                   Arg6.Default:        MBcal.cal
//                   Arg6.Values:         Calib files:*.cal
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           VerboseFlag
//                   Arg7.Title:          Verbose output
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.Default:        No
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           SigmaPeakFind
//                   Arg8.Title:          Sigma for PeakFind() [bins]
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      UpDown
//                   Arg8.Default:        2
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           PercentagePeakFind
//                   Arg9.Title:          Relative percentage for PeakFind()
//                   Arg9.Type:           Int_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        3
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          FitFlag
//                   Arg10.Title:         Peaks to be fitted
//                   Arg10.Type:          Bool_t
//                   Arg10.EntryType:     YesNo
//                   Arg10.Default:       No
//                   Arg10.AddLofValues:  No
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          SigmaFit
//                   Arg11.Title:         Sigma for FitSinglePeak()
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     UpDown
//                   Arg11.Default:       0
//                   Arg11.AddLofValues:  No
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          FitRange
//                   Arg12.Title:         Fit range (bins)
//                   Arg12.Type:          Int_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       0
//                   Arg12.AddLofValues:  No
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          HistoLowLim
//                   Arg13.Title:         Lower limit
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     UpDown
//                   Arg13.Default:       100
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          HistoUpperLim
//                   Arg14.Title:         Upper limit
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     UpDown
//                   Arg14.Default:       0
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          DebugFlag
//                   Arg15.Title:         Debug
//                   Arg15.Type:          Bool_t
//                   Arg15.EntryType:     YesNo
//                   Arg15.Default:       No
//                   Arg15.AddLofValues:  No
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//                   Arg16.Name:          DebugFlagPeakFind
//                   Arg16.Title:         Debug for PeakFind()
//                   Arg16.Type:          Bool_t
//                   Arg16.EntryType:     YesNo
//                   Arg16.Default:       No
//                   Arg16.AddLofValues:  No
//                   Arg16.Base:          dec
//                   Arg16.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

void MBcal(Int_t CalibSource = 0,
           const Char_t * FirstHisto,
           const Char_t * LastHisto,
           const Char_t * CalFile = "MBcal.cal",
           const Char_t * ResFile = "MBcal.res",
           const Char_t * PrecalFile = "MBcal.cal",
           Bool_t VerboseFlag = kFALSE,
           Int_t SigmaPeakFind = 2,
           Int_t PercentagePeakFind = 3,
           Bool_t FitFlag = kFALSE,
           Int_t SigmaFit = 0,
           Int_t FitRange = 0,
           Int_t HistoLowLim = 0,
           Int_t HistoUpperLim = 0,
           Bool_t DebugFlag = kFALSE,
           Bool_t DebugFlagPeakFind = kFALSE)
//>>_________________________________________________(do not change this line)
//
{
	TString cmd;
	TString firstHisto, lastHisto;

	TString histoFile = FirstHisto;
	Int_t idx = histoFile.Index(":");
	if (idx > 0) {
		firstHisto = histoFile(idx + 1, 1000);
		firstHisto = firstHisto.Strip(TString::kBoth);
		histoFile.Resize(idx);
		histoFile = histoFile.Strip(TString::kBoth);
	} else {
		cout << "MBcal.C: Start histo missing" << endl;
		return;
	}
	if (histoFile.Length() == 0) {
		cout << "MBcal.C: No histogram file given" << endl;
		return;
	}

	TString hf = LastHisto;
	if (hf.Length() == 0) {
		hf = histoFile;
		lastHisto = firstHisto;
	} else {
		idx = hf.Index(":");
		if (idx > 0) {
			lastHisto = hf(idx + 1, 1000);
			lastHisto = lastHisto.Strip(TString::kBoth);
			hf.Resize(idx);
			hf = hf.Strip(TString::kBoth);
		}
	}

	if (histoFile.CompareTo(hf.Data()) != 0) {
		cout << "MBcal.C: Histogram files different" << endl;
		return;
	}

	if (*CalFile == '\0') {
		cout << "MBcal.C: No file for calibration output given" << endl;
		return;
	}
	if (*ResFile == '\0') {
		cout << "MBcal.C: No file for calibration results given" << endl;
		return;
	}
	cmd = "MBcal ";
	cmd += CalibSource;
	cmd += " ";
	cmd += histoFile;
	cmd += " \"\" ";
	cmd += PrecalFile;
	cmd += " ";
	cmd += CalFile;
	cmd += " ";
	cmd += ResFile;
	cmd += " ";
	cmd += firstHisto;
	cmd += " ";
	if (lastHisto.Length() == 0) {
		cmd += "\"\"";
	} else {
		cmd += lastHisto;
	}
	cmd += " ";
	cmd += VerboseFlag ? "1" : "0";
	cmd += " ";
	cmd += SigmaPeakFind;
	cmd += " ";
	cmd += PercentagePeakFind;
	cmd += " ";
	cmd += FitFlag ? "1" : "0";
	cmd += " ";
	cmd += SigmaFit;
	cmd += " ";
	cmd += FitRange;
	cmd += " ";
	cmd += HistoLowLim;
	if (HistoUpperLim > HistoLowLim) {
		cmd += ":";
		cmd += HistoUpperLim;
	}
	cmd += " ";
	cmd += DebugFlag ? "1" : "0";
	cmd += " ";
	cmd += DebugFlagPeakFind ? "1" : "0";
	if (VerboseFlag) {
		cout	<< "MBcal.C: Executing command -" << endl
				<< "      >> " << cmd << " <<" << endl;
	}
	system(cmd.Data());
}
