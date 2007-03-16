//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Encal.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .x Encal.C(TObjArray * LofHistos,
//                               Int_t CalSource,
//                               const Char_t * Energies,
//                               const Char_t * CalFile,
//                               const Char_t * ResFile,
//                               const Char_t * FitFile,
//                               Bool_t ClearFlag,
//                               const Char_t * PrecalFile,
//                               Int_t LowerLim,
//                               Int_t UpperLim,
//                               Int_t PeakFrac,
//                               Double_t Sigma,
//                               Int_t FitMode,
//                               Int_t FitGrouping,
//                               Int_t FitBackground,
//                               Double_t FitRange,
//                               Int_t DisplayMode,
//                               Bool_t VerboseMode)
// Arguments:        TObjArray * LofHistos     -- list of histos (#0=name of histo file)
//                   Int_t CalSource           -- Calibration source
//                   Char_t * Energies         -- Calibration energies
//                   Char_t * CalFile          -- Calibration data (.cal)
//                   Char_t * ResFile          -- Calibration results (.res)
//                   Char_t * FitFile          -- Fit results (.root)
//                   Bool_t ClearFlag          -- Clear output files (.cal, .res)
//                   Char_t * PrecalFile       -- Precalibration file (.cal, needed if Eu152)
//                   Int_t LowerLim            -- Lower limit [chn]
//                   Int_t UpperLim            -- Upper limit [chn]
//                   Int_t PeakFrac            -- Threshold for peak finder [% of max peak]
//                   Double_t Sigma            -- Sigma
//                   Int_t FitMode             -- Fit mode
//                   Int_t FitGrouping         -- Fit grouping
//                   Int_t FitBackground       -- Background
//                   Double_t FitRange         -- Range for peak fit [sigma]
//                   Int_t DisplayMode         -- Display results
//                   Bool_t VerboseMode        -- Verbose mode
// Description:      Energy calibration for 1-dim histograms
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: Encal.C,v 1.17 2007-03-16 09:14:11 Rudolf.Lutter Exp $
// Date:             Thu Feb  8 13:23:50 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               
//                   Aclic:               +g
//                   Modify:              
//                   RcFile:              .EncalLoadLibs.C              
//                   NofArgs:             18
//                   Arg1.Name:           HistoFile
//                   Arg1.Title:          Histogram file (.root)
//                   Arg1.Type:           TObjArray *
//                   Arg1.EntryType:      FObjListBox
//                   Arg1.Default:        none.root
//                   Arg1.Values:         ROOT files:*.root
//                   Arg1.Width:          300
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           CalSource
//                   Arg2.Title:          Calibration source
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        1
//                   Arg2.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source (Co60 precalibration needed)=2:TripleAlpha|calibrate using alpha source (Pu239/Am241/Cm244)=4
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Energies
//                   Arg3.Title:          Calibration energies
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      File
//                   Arg3.Default:        $MARABOU/data/encal/energies.dat
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           CalFile
//                   Arg4.Title:          Calibration data (.cal)
//                   Arg4.Type:           Char_t *
//                   Arg4.EntryType:      File
//                   Arg4.Width:          150
//                   Arg4.Default:        Encal.cal
//                   Arg4.Values:         Calib files:*.cal
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           ResFile
//                   Arg5.Title:          Calibration results (.res)
//                   Arg5.Type:           Char_t *
//                   Arg5.EntryType:      File
//                   Arg5.Width:          150
//                   Arg5.Default:        Encal.res
//                   Arg5.Values:         Result files:*.res
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           FitFile
//                   Arg6.Title:          Fit results (.root)
//                   Arg6.Type:           Char_t *
//                   Arg6.EntryType:      File
//                   Arg6.Width:          150
//                   Arg6.Default:        Encal.root
//                   Arg6.Values:         Fit data files:*.root
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           ClearFlag
//                   Arg7.Title:          Clear output files (.cal, .res)
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           PrecalFile
//                   Arg8.Title:          Precalibration file (.cal, needed if Eu152)
//                   Arg8.Type:           Char_t *
//                   Arg8.EntryType:      File
//                   Arg8.Width:          150
//                   Arg8.Default:        Co60.cal
//                   Arg8.Values:         Calib results:*.cal
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           LowerLim
//                   Arg9.Title:          Lower limit [chn]
//                   Arg9.Type:           Int_t
//                   Arg9.EntryType:      UpDown-X
//                   Arg9.Default:        0
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     0
//                   Arg9.UpperLimit:     4096
//                   Arg9.Increment:      100
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          UpperLim
//                   Arg10.Title:         Upper limit [chn]
//                   Arg10.Type:          Int_t
//                   Arg10.EntryType:     UpDown-X
//                   Arg10.Default:       4096
//                   Arg10.AddLofValues:  No
//                   Arg10.LowerLimit:    0
//                   Arg10.UpperLimit:    4096
//                   Arg10.Increment:     100
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          PeakFrac
//                   Arg11.Title:         Threshold for peak finder [% of max peak]
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     UpDown-X
//                   Arg11.Default:       1
//                   Arg11.AddLofValues:  No
//                   Arg11.LowerLimit:    1
//                   Arg11.UpperLimit:    50
//                   Arg11.Increment:     1
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          Sigma
//                   Arg12.Title:         Sigma
//                   Arg12.Type:          Double_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       3
//                   Arg12.AddLofValues:  No
//                   Arg12.LowerLimit:    0
//                   Arg12.UpperLimit:    10
//                   Arg12.Increment:     .2
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          FitMode
//                   Arg13.Title:         Fit mode
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     Radio
//                   Arg13.Default:       1
//                   Arg13.Values:        gauss|fit gaussian distribution=1:gauss+tail|fit gaussian + exp tail on left side=2
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          FitGrouping
//                   Arg14.Title:         Fit grouping
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     Radio
//                   Arg14.Default:       1
//                   Arg14.Values:        single peak|fit each peak separately=1:ensemble|group peaks before fitting=2
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          FitBackground
//                   Arg15.Title:         Background
//                   Arg15.Type:          Int_t
//                   Arg15.EntryType:     Radio
//                   Arg15.Default:       1
//                   Arg15.Values:        linear|fit linear Background=1:const|fit constant background=2
//                   Arg15.AddLofValues:  No
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//                   Arg16.Name:          FitRange
//                   Arg16.Title:         Range for peak fit [sigma]
//                   Arg16.Type:          Double_t
//                   Arg16.EntryType:     UpDown
//                   Arg16.Default:       3
//                   Arg16.AddLofValues:  No
//                   Arg16.LowerLimit:    0
//                   Arg16.UpperLimit:    10
//                   Arg16.Increment:     .5
//                   Arg16.Base:          dec
//                   Arg16.Orientation:   horizontal
//                   Arg17.Name:          DisplayMode
//                   Arg17.Title:         Display results
//                   Arg17.Type:          Int_t
//                   Arg17.EntryType:     Check
//                   Arg17.Default:       1
//                   Arg17.Values:        step|show each fit=1:2dim|show 2-dim histo after calibration=2
//                   Arg17.AddLofValues:  No
//                   Arg17.Base:          dec
//                   Arg17.Orientation:   horizontal
//                   Arg18.Name:          VerboseMode
//                   Arg18.Title:         Verbose mode
//                   Arg18.Type:          Bool_t
//                   Arg18.EntryType:     YesNo
//                   Arg18.Default:       1
//                   Arg18.AddLofValues:  No
//                   Arg18.Base:          dec
//                   Arg18.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <fstream>
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TVirtualX.h"
#include "TButton.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TEnv.h"
#include "TGFrame.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TPolyMarker.h"
#include "TMrbLogger.h"
#include "SetColor.h"

// global vars

TMrbLogger * msg = NULL;

// global fitting vars used by fit functions
Int_t fitNofPeaks = 1;
Int_t fitBinWidth = 1;
Int_t fitTailSide = 1;

// global button flag
enum EEncalButtons		{	kButtonDiscard = 1,
							kButtonOk,
							kButtonStop,
							kButtonQuit
						};

Int_t buttonFlag = 0;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           Encal
// Description:    Class to describe an energy calibration
//////////////////////////////////////////////////////////////////////////////

class TMrbEncal {

	public:
		enum EEncalBrowserBits {	kCalSourceCo60 = 1,
									kCalSourceEu152 = 2,
									kCalSourceTripleAlpha = 4,
									kFitModeGaus = 1,
									kFitModeGausTail = 2,
									kFitModeBackgroundLinear = 1,
									kFitModeBackgroundConst = 2,
									kFitGroupingSinglePeak = 1,
									kFitGroupingEnsemble = 2,
									kDisplayFlagStep = 1,
									kDisplayFlag2dim = 2
								};

		enum EEncalColors		{	kColorBlack = 1,
									kColorWhite = 10,
									kColorRed = 2,
									kColorGreen = 3,
									kColorBlue = 4,
									kColorGray = 32
								};

		enum					{	kNofButtons = 4 };
		enum					{	kCanvasWidth = 800 };
		enum					{	kCanvasHeight = 600 };

		enum EEncalFitStatus	{	kFitDiscard = 0,
									kFitOk,
									kFitAuto
								};

	public:
		TMrbEncal() {}; 							// default ctor
		TMrbEncal(	TObjArray * LofHistos,			// ctor
						Int_t CalSource,
						const Char_t * Energies,
						const Char_t * CalFile,
						const Char_t * ResFile,
						const Char_t * FitFile,
						Bool_t ClearFlag,
						const Char_t * PrecalFile,
						Int_t LowerLim,
						Int_t UpperLim,
						Int_t PeakFrac,
						Double_t Sigma,
						Int_t FitMode,
						Int_t FitGrouping,
						Int_t FitBackground,
						Double_t FitRange,
						Int_t DisplayMode,
						Bool_t VerboseMode);

		virtual ~TMrbEncal() {};	// default dtor
		
		Bool_t OpenHistoFile();
		inline Int_t GetNofHistos() { return(fNofHistos); };
		Bool_t OpenCalFiles();

		inline const Char_t * SourceName() { return(fSourceName.Data()); };
		Bool_t SetCalSource();
		inline Bool_t IsTripleAlpha() { return(fCalSource == kCalSourceTripleAlpha); };
		inline Bool_t IsCo60() { return(fCalSource == kCalSourceCo60); };
		inline Bool_t IsEu152() { return(fCalSource == kCalSourceEu152); };

		inline TObjArray * GetLofHistos() { return(fLofHistos); };
		inline void ResetLofHistos() { fHIter = fLofHistos->MakeIterator(); };
		TH1F * GetNextHisto();

		Bool_t FindPeaks(Int_t MaxNofPeaks = 100, Float_t Resolution = 1.0);

		inline Int_t GetNofPeaks() { return(fNofPeaks); };
		inline Int_t GetNofPeaksNeeded() { return(fNofPeaksNeeded); };

		Int_t GetPeakFinderResults(TArrayF & PeakX, TArrayF & PeakY);

		void FitGaussTailSingle();
		void FitGaussTailEnsemble();
		void FitGaussSingle();
		void FitGaussEnsemble();
		Int_t GetFitResults(TArrayF & Xfit, TArrayF & Xerr, TArrayF & Yfit, TArrayF & Yerr, TArrayF & Chi2);
		void PrintFitResults();

		inline Int_t GetFitMode() { return(fFitMode); };
		inline Int_t GetFitGrouping() { return(fFitGrouping); };

		void Calibrate();

		Int_t GetNofHistosCalibrated();

		inline Bool_t StepMode() { return((fDisplayMode & kDisplayFlagStep) != 0); };
		inline Bool_t TwoDimMode() { return((fDisplayMode & kDisplayFlag2dim) != 0); };

		void WaitForButtonPressed(Bool_t StepFlag = kFALSE);
		inline Bool_t ButtonOk() { return(buttonFlag == kButtonOk); };
		inline Bool_t ButtonDiscard() { return(buttonFlag == kButtonDiscard); };
		inline Bool_t ButtonStop() { return(buttonFlag == kButtonStop); };
		inline Bool_t ButtonQuit() { return(buttonFlag == kButtonQuit); };

		void ClearCanvas(Int_t PadNo);
		void SetStatusLine(const Char_t * Text = NULL, Int_t FillColor = kColorWhite, Int_t LineColor = kColorBlack);
		void OutputMessage(const Char_t * Method, const Char_t * Text, Bool_t ErrFlag = kFALSE, Int_t ColorIndex = -1);

		TCanvas * OpenCanvas();
		void CloseCanvas() { if (fMainCanvas) fMainCanvas->Close(); if (f2DimCanvas) f2DimCanvas->Close(); }

		void WriteResults();
		void WriteCalibration();
		void UpdateStatusLine();
		void WriteMinMax();
		void WriteFitStatus();

		void SetFitStatus(Int_t FitStatus, const Char_t * Reason = NULL);

		void ShowResults2dim();
		void CloseEnvFiles();
		void CloseRootFile();

		inline Bool_t Verbose() { return(fVerboseMode); };
		inline void SetVerboseMode(Bool_t Flag) { fVerboseMode = Flag; };

		void SetCurHisto(TH1F * Histo);
		inline Int_t GetLowerLim() { return(fLowerLim); };
		inline Int_t GetUpperLim() { return(fUpperLim); };

		inline TFile * HistoFile() { return(fHistoFile); };

		void Exit();

	protected:
		TObjArray * fLofHistos;	// argument storage
		Int_t fCalSource;
		TString fEnergies;
		TString fCalFile;
		TString fResFile;
		TString fFitFile;
		Bool_t fClearFlag;
		TString fPrecalFile;
		Int_t fLowerLim;
		Int_t fUpperLim;
		Int_t fPeakFrac;
		Double_t fSigma;
		Int_t fFitMode;
		Int_t fFitGrouping;
		Int_t fFitBackground;
		Double_t fFitRange;
		Int_t fDisplayMode;
		Bool_t fVerboseMode;

	protected:
		TString fSourceName; 	// calibration source
		TEnv * fEnvEnergies;	// calibration energies
		TEnv * fEnvCalib;		// calibration data
		TEnv * fEnvResults;		// detailed calibration results
		TFile * fFitResults;	// histograms + fits

		Int_t fNofPeaksNeeded; 	// peaks needed for given calibration
		Bool_t fEnableCalib;	// kTRUE if we have to calibrate

		TCanvas * fMainCanvas;	// canvas to display histo + fit
		TCanvas * f2DimCanvas;	// 2nd canvas: 2-dim histo
		TButton * fStatusLine;	// status

		TFile * fHistoFile; 	// root file containing histograms
		TIterator * fHIter;
		TH1F * fCurHisto;
		Int_t fNofHistos;		// number of histograms (selected from root file)
		Int_t fNofHistosCalibrated;	// number of histograms calibrated

		TH2S * f2DimFitResults;	// 2-dim histo to show re-calibrated histograms

		Int_t fNofPeaks;		// peak finder results
		TArrayF fPeakX;
		TArrayF fPeakY;

		TArrayF fXfit;			// fit results
		TArrayF fXerr;
		TArrayF fYfit;
		TArrayF fYerr;
		TArrayF fChi2;

		Int_t fXmin;			// energy calibration
		Double_t fEmin;
		Int_t fXmax;
		Double_t fEmax;
		TF1 * fPoly;

		TArrayF fCalX;
		TArrayF fCalE;
		TArrayF fCalXerr;
		TArrayF fCalEerr;

		Int_t fFitStatus;		// fit quality
		TString fReason;

		TObjArray * fParamNames;	// param names for gauss/tail/background fit

		TButton * fBtnOk;		//buttons
		TButton * fBtnDiscard;
		TButton * fBtnStop;
		TButton * fBtnQuit;
};

void buttonPressed(Int_t Flag = 0) { buttonFlag = Flag; }

Double_t gaus_lbg(Double_t * x, Double_t * par) {
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gaus_lbg
// Purpose:        fit linear/const background + N gaussians
// Description:    par[0]   background constant
//                 par[1]   background slope
//                 par[2]   gauss width
//                 par[3]   gauss0 constant
//                 par[4]   gauss0 mean
//                 par[5]   gauss1 constant
//                 par[6]   gauss1 mean
//                 par[7]   gauss2 constant
//                 par[8]   gauss2 mean
//                 ...
//////////////////////////////////////////////////////////////////////////////

   static Float_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[0];
   Double_t bgslope = par[1];
   Double_t sigma   = par[2];
   if (sigma == 0) sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i ++) {
      arg = (x[0] - par[4 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

Double_t gaus_tail(Double_t * x, Double_t * par) {
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gaus_tail
// Purpose:        fit linear/const background + tail on left/right side + N gaussians
// Description:    par[0]   tail fraction
//                 par[1]   tail width
//                 par[2]   background constant
//                 par[3]   background slope
//                 par[4]   gauss width
//                 par[5]   gauss0 constant
//                 par[6]   gauss0 mean
//                 par[7]   gauss1 constant
//                 par[8]   gauss1 mean
//                 par[9]   gauss2 constant
//                 par[10]  gauss2 mean
//                 ...
//////////////////////////////////////////////////////////////////////////////

   static Double_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t xij;
   Double_t arg;
   Double_t tail;
   Double_t g2b;
   Double_t err;
   Double_t norm;
   Double_t fitval = 0;
   Double_t tailfrac  = par[0];
   Double_t tailwidth = par[1];
   Double_t bgconst   = par[2];
   Double_t bgslope   = par[3];
   Double_t sigma = par[4];

//  force widths /= 0
   if (tailwidth == 0)
      tailwidth = 1;
   if (sigma == 0)
      sigma = 1;

   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i++) {
      xij = (x[0] - par[6 + 2 * i]);
      arg = xij / (sqrt2 * sigma);
      xij *= fitTailSide;
      tail = exp(xij / tailwidth) / tailwidth;
      g2b = 0.5 * sigma / tailwidth;
      err = 0.5 * tailfrac * par[5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
      norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
      fitval = fitval + norm * err * tail 
                      + par[5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

TMrbEncal::TMrbEncal(	TObjArray * LofHistos,
						Int_t CalSource,
						const Char_t * Energies,
						const Char_t * CalFile,
						const Char_t * ResFile,
						const Char_t * FitFile,
						Bool_t ClearFlag,
						const Char_t * PrecalFile,
						Int_t LowerLim,
						Int_t UpperLim,
						Int_t PeakFrac,
						Double_t Sigma,
						Int_t FitMode,
						Int_t FitGrouping,
						Int_t FitBackground,
						Double_t FitRange,
						Int_t DisplayMode,
						Bool_t VerboseMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal
// Purpose:        ROOT object to hold energy calibration data
//////////////////////////////////////////////////////////////////////////////

	fLofHistos = LofHistos; 								// xfer arguments
	fCalSource = CalSource;
	fEnergies = (Energies == NULL || *Energies == '\0') ? "$MARABOU/data/encal/energies.dat" : Energies;
	fCalFile = (CalFile == NULL || *CalFile == '\0') ? "Encal.cal" : CalFile;
	fResFile = (ResFile == NULL || *ResFile == '\0') ? "" : CalFile;
	if (fResFile.IsNull()) { fResFile = fCalFile; fResFile.ReplaceAll(".cal", 4, ".res", 4); }
	fFitFile = (FitFile == NULL || *FitFile == '\0') ? "" : FitFile;
	if (fFitFile.IsNull()) { fFitFile = fCalFile; fFitFile.ReplaceAll(".cal", 4, ".root", 5); }
	fClearFlag = ClearFlag;
	fPrecalFile = (PrecalFile == NULL || *PrecalFile == '\0') ? "" : PrecalFile;
	fLowerLim = LowerLim;
	fUpperLim = UpperLim;
	fPeakFrac = PeakFrac;
	fSigma = Sigma;
	fFitMode = FitMode;
	fFitGrouping = FitGrouping;
	fFitBackground = FitBackground;
	fFitRange = FitRange;
	fDisplayMode = DisplayMode;
	fVerboseMode = VerboseMode;

	fNofPeaksNeeded = 0;
	fEnableCalib = kFALSE;
	fEnvEnergies = NULL;
	fEnvCalib = NULL;
	fFitResults = NULL;

	TString pNames = "Ta_Frac:Ta_Width:Bg_Slope:Bg_Const:Ga_Sigma:Ga_Const:Ga_Mean";
	fParamNames = pNames.Tokenize(":");
};

Bool_t TMrbEncal::OpenHistoFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::OpenHistoFile
// Purpose:        Open ROOT file and extract histo list
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Extracts file name and histo names
//////////////////////////////////////////////////////////////////////////////

	Int_t fNofHistos = fLofHistos->GetEntriesFast();
	if (fNofHistos == 0) {
		this->OutputMessage("OpenHistoFile", "No histogram file given", kTRUE);
		return(kFALSE);
	}

	TObjString * hStr = (TObjString *) fLofHistos->At(0);
	TString hFileName = hStr->GetString();
	hFileName = hFileName.Strip(TString::kBoth);

	if (hFileName.IsNull()) {
		this->OutputMessage("OpenHistoFile", "No histogram file given", kTRUE);
		return(kFALSE);
	}

	fHistoFile = new TFile(hFileName.Data());
	if (!fHistoFile->IsOpen()) {
		this->OutputMessage("OpenHistoFile", Form("Can't open histogram file - %s", hFileName.Data()), kTRUE);
		return(kFALSE);
	}

	if (fNofHistos == 1) {
		this->OutputMessage("OpenHistoFile", Form("File %s - no histogram(s) selected", hFileName.Data()), kTRUE);
		return(kFALSE);
	}

	fLofHistos->Remove(hStr);
	fNofHistos--;
	fNofHistosCalibrated = 0;

	this->ResetLofHistos();

	return(kTRUE);
}

TH1F * TMrbEncal::GetNextHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::GetNextHisto
// Purpose:        Fetch next histogram from list
// Arguments:      --
// Results:        TH1F * Histo    -- ptr to histo
// Description:    Reads next histo name from list
//////////////////////////////////////////////////////////////////////////////

	TObjString * hStr = (TObjString *) fHIter->Next();
	if (hStr) {
		TString hName = hStr->GetString();
		TH1F * h = (TH1F *) fHistoFile->Get(hName.Data());
		if (h == NULL) this->OutputMessage("GetNextHisto", Form("No such histogram - %s:%s", fHistoFile->GetName(), hName.Data()), kTRUE);
		return(h);
	}
	return(NULL);
}

Bool_t TMrbEncal::OpenCalFiles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::OpenCalFiles
// Purpose:        Open calibration files
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Opens calibration files
//////////////////////////////////////////////////////////////////////////////

	if (fClearFlag) {
		TString rmCmd = Form("rm -f %s %s", fCalFile.Data(), fResFile.Data());
		gSystem->Exec(rmCmd.Data());
		if (this->Verbose()) this->OutputMessage("OpenCalFiles", Form("Removing existing files - %s %s", fCalFile.Data(), fResFile.Data()));
	}

	fEnvCalib = new TEnv(fCalFile.Data());
	if (this->Verbose()) this->OutputMessage("OpenCalFiles", Form("Writing calibration data to file %s", fCalFile.Data()));

	fEnvCalib->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	
	fEnvCalib->SetValue("Calib.Source", fSourceName.Data());
	fEnvCalib->SetValue("Calib.Energies", fEnergies.Data());

	fEnvResults = new TEnv(fResFile.Data());
	if (this->Verbose()) this->OutputMessage("OpenCalFiles", Form("Writing results to file %s", fResFile.Data()));

	fEnvResults->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	fEnvResults->SetValue("Calib.Energies", fEnergies.Data());

	fFitResults = new TFile(fFitFile.Data(), "RECREATE");
	if (!fFitResults->IsOpen()) {
		this->OutputMessage("OpenCalFiles", Form("Can't open file - %s", fFitFile.Data()), kTRUE);
		fFitResults = NULL;
	} else if (this->Verbose()) {
		this->OutputMessage("OpenCalFiles", Form("Writing fit data to file %s", fFitFile.Data()));
	}

	fEmin = fEnvResults->GetValue("Calib.Emin", 1e+20);
	fEmax = fEnvResults->GetValue("Calib.Emax", 0);
	fXmin = fEnvResults->GetValue("Calib.Xmin", 1000000);
	fXmax = fEnvResults->GetValue("Calib.Xmax", 0);

	return(kTRUE);
}

void TMrbEncal::SetCurHisto(TH1F * Histo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::SetCurHisto
// Purpose:        Define current histo
// Arguments:      TH1F * Histo   -- histogram
// Results:        --
// Description:    Defines current histo and adjusts histo limits
//////////////////////////////////////////////////////////////////////////////

	fCurHisto = Histo;
	if (fUpperLim == 0 || fUpperLim < fLowerLim) fUpperLim = fCurHisto->GetNbinsX();
	if (fLowerLim != 0 || fUpperLim < Histo->GetNbinsX()) fCurHisto->GetXaxis()->SetRange(fLowerLim, fUpperLim);
}

void TMrbEncal::WriteMinMax() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::WriteMinMax
// Purpose:        Set minimum and maximum
// Arguments:      --
// Results:        --
// Description:    Writes minimum/maximum values to .res file
//////////////////////////////////////////////////////////////////////////////

	fEnvResults->SetValue("Calib.Emin", fEmin);
	fEnvResults->SetValue("Calib.Emax", fEmax);
	fEnvResults->SetValue("Calib.Xmin", fXmin);
	fEnvResults->SetValue("Calib.Xmax", fXmax);

}

void TMrbEncal::WaitForButtonPressed(Bool_t StepFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::WaitForButtonPressed
// Purpose:        Control buttons
// Arguments:      Bool_t StepFlag    -- wait if kTRUE (overwrites StepMode bit)
// Results:        --
// Description:    Waits for button flag to change
//////////////////////////////////////////////////////////////////////////////

	if (StepFlag || this->StepMode()) {
		buttonPressed(0);
		while (buttonFlag == 0) {
			gSystem->Sleep(50);
			gSystem->ProcessEvents();
		}
	} else {
		buttonPressed(kButtonOk);
	}
}

void TMrbEncal::Exit() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::Exit
// Purpose:        Close files and exit
// Arguments:      --
// Results:        --
// Description:    Exits from MacroBrowser
//////////////////////////////////////////////////////////////////////////////

	if (fHistoFile && fHistoFile->IsOpen()) fHistoFile->Close();
	this->CloseRootFile();
	gSystem->Exit(0);
}

void TMrbEncal::SetStatusLine(const Char_t * Text, Int_t FillColor, Int_t LineColor) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::SetStatusLine
// Purpose:        Write text to status line
// Arguments:      Char_t * Text     -- text to be written
//                 Int_t FillColor   -- background color
//                 Int_t LineColor   -- foreground color
// Results:        --
// Description:    Update status information
//////////////////////////////////////////////////////////////////////////////

	fStatusLine->SetTitle(Text);
	fStatusLine->SetFillColor(FillColor);
	fStatusLine->SetTextColor(LineColor);
	fStatusLine->Paint();
	fStatusLine->Modified();
	fStatusLine->Update();
}

void TMrbEncal::OutputMessage(const Char_t * Method, const Char_t * Text, Bool_t ErrFlag, Int_t ColorIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::OutputMessage
// Purpose:        Write message to cout/cerr
// Arguments:      Char_t * Method   -- method
//                 Char_t * Text     -- message text
//                 Bool_t ErrFlag    -- kTRUE if error message
//                 Int_t ColorIndex  -- foreground color
// Results:        --
// Description:    Outputs a message
//////////////////////////////////////////////////////////////////////////////

	const Char_t * tCol = setblack;
	Int_t fgCol = 1;
	TString method;
	TString cName;
	if (Method == NULL || *Method == '\0') {
		cName = "Encal.C";
		method = "";
	} else {
		cName = "TMrbEncal";
		method = Method;
	}

	switch (ColorIndex) {
		case kColorRed: tCol = setred; fgCol = 1; break;
		case kColorGreen: tCol = setgreen; fgCol = 1; break;
		case kColorBlue: tCol = setblue; fgCol = 10; break;
	}
	if (ColorIndex > 0) SetStatusLine(Text, ColorIndex, fgCol);
	if (ErrFlag) {
		msg->Err() << Text << endl;
		msg->Flush(cName.Data(), method.Data());
	} else {
		msg->Out() << Text << endl;
		msg->Flush(cName.Data(), method.Data(), tCol);
	}
}

TCanvas * TMrbEncal::OpenCanvas() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::OpenCanvas
// Purpose:        Create canvas and display buttons
// Arguments:      --
// Results:        TCanvas * Canvas   -- canvas
// Description:    Opens the canvas
//////////////////////////////////////////////////////////////////////////////

	fMainCanvas = new TCanvas("EncalCanv", "Energy Calibration Tool", kCanvasWidth, kCanvasHeight);
	fMainCanvas->Divide(1,2);

	Double_t xpx = 1./kCanvasWidth;
	Double_t x0 = 100 * xpx;
	Double_t wdth = (kCanvasWidth - 200) * xpx / kNofButtons;
	Double_t y0 = 0.03;
	Double_t y1 = 0.08;
	Double_t y2 = 0.081;
	Double_t y3 = 0.132;

	Int_t btnNo = 0;
	fBtnOk = new TButton("next", "buttonPressed(kButtonOk);", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	fBtnOk->SetFillColor(32);
	fBtnOk->SetToolTipText("pass data to calibration file and continue with next histo");
	fBtnOk->Draw();
	btnNo++;
	fBtnDiscard = new TButton("discard", "buttonPressed(kButtonDiscard);", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	fBtnDiscard->SetFillColor(32);
	fBtnDiscard->SetToolTipText("discard fit and continue with next histo");
	fBtnDiscard->Draw();
	btnNo++;
	fBtnStop = new TButton("stop", "buttonPressed(kButtonStop);", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	fBtnStop->SetFillColor(32);
	fBtnStop->SetToolTipText("leave calibration and return to MacroBrowser");
	fBtnStop->Draw();
	btnNo++;
	fBtnQuit = new TButton("quit", "buttonPressed(kButtonQuit);", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	fBtnQuit->SetFillColor(32);
	fBtnQuit->SetToolTipText("exit from ROOT");
	fBtnQuit->Draw();
	fStatusLine = new TButton(" ", " ", .03, y2, .97, y3);
	fStatusLine->Draw();

	fMainCanvas->cd(2);
	gPad->SetBottomMargin(.3);

	fMainCanvas->cd();
	fMainCanvas->Update();
	return(fMainCanvas);
}

void TMrbEncal::ClearCanvas(Int_t PadNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::ClearCanvas
// Purpose:        Clear canvas
// Arguments:      Int_t PadNo     -- pad number
// Results:        --
// Description:    Clears whole canvas or specified pads
//////////////////////////////////////////////////////////////////////////////

	switch (PadNo) {
		case 0:
		case 1:
			fMainCanvas->cd(1);
			gPad->Clear();
			if (PadNo == 1) break;
		case 2:
			fMainCanvas->cd(2);
			gPad->Clear();
			break;
	}
	fMainCanvas->cd(PadNo);
	fMainCanvas->Update();
}

Bool_t TMrbEncal::SetCalSource() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::SetCalSource
// Purpose:        Set calibration source
// Arguments:      --
//                 --
// Results:        kTRUE/kFALSE
// Description:    Define calibration source
//////////////////////////////////////////////////////////////////////////////

	fNofPeaksNeeded = 0;
	fEnableCalib = kFALSE;

	switch (fCalSource) {
		case kCalSourceTripleAlpha:
			fSourceName = "TripleAlpha";
			break;
		case kCalSourceCo60:
			fSourceName = "Co60";
			break;
		case kCalSourceEu152:
			fSourceName = "Eu152";
			break;
		default:
			this->OutputMessage("SetCalSource", "Wrong calibration source - performing peak fitting only", kTRUE);
			return(kFALSE);
	}

	gSystem->ExpandPathName(fEnergies);
	if (gSystem->AccessPathName(fEnergies.Data())) {
		this->OutputMessage("SetCalSource", Form("No such file - %s, can't read calibration energies", fEnergies.Data()), kTRUE);
		return(kFALSE);
	}

	fEnvEnergies = new TEnv(fEnergies.Data());

	TString calNames = fEnvEnergies->GetValue("Calib.SourceNames", "");
	if (calNames.IsNull()) {
		this->OutputMessage("SetCalSource", Form("File %s doesn't contain calibration energies for source \"%s\"", fEnergies.Data(), fSourceName.Data()), kTRUE);
		delete fEnvEnergies;
		fEnvEnergies = NULL;
		return(kFALSE);
	}	
		
	fNofPeaksNeeded = fEnvEnergies->GetValue(Form("Calib.%s.NofLines", fSourceName.Data()), 0);
	if (fNofPeaksNeeded == 0) {
		this->OutputMessage(	"SetCalSource",
								Form("Error reading \"Calib.%s.NofLines\" from file %s - can't calibrate using %s source",
								fSourceName.Data(), fEnergies.Data(), fSourceName.Data()), kTRUE);
		delete fEnvEnergies;
		fEnvEnergies = NULL;
		return(kFALSE);
	}	
	fEnableCalib = kTRUE;
	return(kTRUE);
}

void TMrbEncal::CloseEnvFiles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::CloseEnvFiles
// Purpose:        Close calibration files
// Arguments:      --
// Results:        --
// Description:    Close TEnv files
//////////////////////////////////////////////////////////////////////////////

	TList * lofEntries = (TList *) fEnvResults->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	fEnvResults->SetValue("Calib.NofHistograms", nh);
	fEnvResults->SaveLevel(kEnvLocal);

	fEnvCalib->SetValue("Calib.NofHistograms", this->GetNofHistosCalibrated());
	fEnvCalib->SaveLevel(kEnvLocal);
}

void TMrbEncal::CloseRootFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::CloseRootFile
// Purpose:        Write histo results and close file
// Arguments:      --
// Results:        --
// Description:    Closes root file containing histo + fit data
//////////////////////////////////////////////////////////////////////////////

	if (fFitResults && fFitResults->IsOpen()) {
		fFitResults->Write();
		fFitResults->Close();
	}
}

Int_t TMrbEncal::GetNofHistosCalibrated() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::GetNofHistosCalibrated
// Purpose:        Count histos in .cal file
// Arguments:      --
// Results:        Int_t NofHistos   -- number of calibrated histos
// Description:    Counts histos in calibration file
//////////////////////////////////////////////////////////////////////////////

	TList * lofEntries = (TList *) fEnvCalib->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	fNofHistosCalibrated = nh;
	return(nh);
}

Bool_t TMrbEncal::FindPeaks(Int_t MaxNofPeaks, Float_t Resolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::FindPeaks
// Purpose:        Start the peak finder
// Arguments:      TH1F * Histo         -- histogram
//                 Int_t MaxNofPeaks    -- max number of peaks expected
//                 Float_t Resolution   -- resolution
// Results:        kTRUE/kFALSE
// Description:    Peak finding
//////////////////////////////////////////////////////////////////////////////

	TSpectrum * s = new TSpectrum(MaxNofPeaks, Resolution);

	Double_t pmax = (Double_t) fPeakFrac / 100.;
	fNofPeaks = s->Search(fCurHisto, 5, "", pmax);

	if (fNofPeaks == 0) {
		this->OutputMessage("FindPeaks", Form("No peaks found in histogram - %s", fCurHisto->GetName()), kTRUE, TMrbEncal::kColorRed);
		this->SetFitStatus(TMrbEncal::kFitDiscard, "No peaks found");
		return(kFALSE);
	}

	fPeakX.Set(fNofPeaks, s->GetPositionX());
	fPeakY.Set(fNofPeaks, s->GetPositionY());

	TArrayF ps;
	TArrayI ind(fNofPeaks);
	ps.Set(fNofPeaks, fPeakX.GetArray());
	TMath::Sort(fNofPeaks, ps.GetArray(), ind.GetArray(), kFALSE);
	for (Int_t i = 0; i < fNofPeaks; i++) {
		Int_t k = ind[i];
		fPeakX[i] = ps[k];
	}
	ps.Set(fNofPeaks, fPeakY.GetArray());
	for (Int_t i = 0; i < fNofPeaks; i++) {
		Int_t k = ind[i];
		fPeakY[i] = ps[k];
	}

	if (this->Verbose()) {
		cout << endl << "Peaks in histogram " << fCurHisto->GetName() << ":" << endl;
		for (Int_t i = 0; i < fNofPeaks; i++) cout << i << " X=" << fPeakX[i] << " Y=" << fPeakY[i] << endl;
	}

    TList * lf = fCurHisto->GetListOfFunctions();                        
    TPolyMarker * pm = (TPolyMarker *) lf->FindObject("TPolyMarker");
	pm->SetMarkerColor(kBlue);

	return(kTRUE);
}

void TMrbEncal::FitGaussTailSingle() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::FitGaussTailSingle
// Purpose:        Fit: gauss + tail, single peaks
// Arguments:      --
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	fXfit.Set(fNofPeaks);
	fXerr.Set(fNofPeaks);
	fYfit.Set(fNofPeaks);
	fYerr.Set(fNofPeaks);
	fChi2.Set(fNofPeaks);

	Int_t fitNofPeaks = 1;
	Int_t nPar = 5 + 2 * fitNofPeaks;

	for (Int_t i = 0; i < fNofPeaks; i++) {
		TArrayD p(nPar);
		// tail
		p[0] = 1;
		p[1] = fSigma;
		// linear background
		p[2] = fCurHisto->GetBinContent(fCurHisto->FindBin(fPeakX[i] - fFitRange * fSigma));
		p[3] = 0.;
		// gauss
		p[4] = fSigma;
		p[5] = fPeakY[i];
		p[6] = fPeakX[i];

		TF1 * fit = new TF1(Form("g%d", i), gaus_tail, fPeakX[i] - fFitRange * fSigma, fPeakX[i] + fFitRange * fSigma, nPar);
		fit->SetParameters(p.GetArray());
		if (fFitBackground == TMrbEncal::kFitModeBackgroundConst) fit->FixParameter(3, 0.0);
		if (this->Verbose()) {
			cout	<< endl << "---------------------------------------------------------------------" << endl
					<< "Gaussian/tail start params for peak #" << i << ":" << endl;
		}
		for (Int_t k = 0; k < nPar; k++) {
			const Char_t * pn = ((TObjString *) fParamNames->At(k))->GetString().Data();
			fit->SetParName(k, pn);
			if (this->Verbose()) cout << Form("   %d: %-10s = %10.4f", k, pn, p[k]) << endl;
		}
		fit->SetLineColor(2);
		fCurHisto->Fit(fit, (i == 0) ? "R" : "R+");
		fit->GetParameters(p.GetArray());
		fXfit[i] = p[6];
		fYfit[i] = p[5];
		fXerr[i] = fit->GetParError(6);
		fYerr[i] = fit->GetParError(5);
		Int_t ndf = TMath::Max(1, fit->GetNDF());
		fChi2[i] = fit->GetChisquare() / ndf;
	}
	if (fFitResults) fFitResults->Append(fCurHisto);
}

void TMrbEncal::FitGaussTailEnsemble() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::FitGaussTailEnsemble
// Purpose:        Fit: gauss + tail, group of peaks
// Arguments:      --
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	fXfit.Set(fNofPeaks);
	fXerr.Set(fNofPeaks);
	fYfit.Set(fNofPeaks);
	fYerr.Set(fNofPeaks);
	fChi2.Set(fNofPeaks);

	Int_t fitNofPeaks = (fNofPeaks > fNofPeaksNeeded) ? fNofPeaksNeeded : fNofPeaks;
	Int_t idxPeak1 = (fNofPeaks <= fitNofPeaks) ? 0 : (fNofPeaks - fitNofPeaks);
	Int_t nPar = 5 + 2 * fitNofPeaks;
	TArrayD p(nPar);
	// tail
	p[0] = 1;
	p[1] = fSigma;
	// linear background
	p[2] = fCurHisto->GetBinContent(fCurHisto->FindBin(fPeakX[idxPeak1] - fFitRange * fSigma));
	p[3] = 0.;
	// gauss
	p[4] = fSigma;
	Int_t k = 5;
	for (Int_t i = idxPeak1; i < fNofPeaks; i++, k+= 2) {
		p[k] = fPeakY[i];
		p[k + 1] = fPeakX[i];
	}

	TF1 * fit = new TF1("g_group", gaus_tail, fPeakX[idxPeak1] - fFitRange * fSigma, fPeakX[fNofPeaks - 1] + fFitRange * fSigma, nPar);
	fit->SetParameters(p.GetArray());
	if (fFitBackground == TMrbEncal::kFitModeBackgroundConst) fit->FixParameter(3, 0.0);
	if (this->Verbose()) {
		cout	<< endl << "---------------------------------------------------------------------" << endl
				<< "Gaussian/tail start params for a group of " << fitNofPeaks << " peak(s)" << endl;
	}
	for (Int_t k = 0; k < nPar; k++) {
		const Char_t * pn;
		Int_t kk;
		if (k <= 4) {
			kk = k;
		} else {
			kk = (k & 1) ? 5 : 6;
		}
		pn = ((TObjString *) fParamNames->At(kk))->GetString().Data();
		if (k > 4) pn = Form("%s_%d", pn, (k - 3) / 2);
		fit->SetParName(k, pn);
		if (this->Verbose()) cout << Form("   %2d: %-10s = %10.4f", k + 1, pn, p[k]) << endl;
	}
	fit->SetLineColor(2);
	fCurHisto->Fit(fit, "R");
	if (fFitResults) fFitResults->Append(fCurHisto);
	fit->GetParameters(p.GetArray());
	Int_t ndf = TMath::Max(1, fit->GetNDF());
	k = 5;
	for (Int_t i = 0; i < fitNofPeaks; i++, k+= 2) {
		fYfit[i] = p[k];
		fYerr[i] = fit->GetParError(k);
		fXfit[i] = p[k + 1];
		fXerr[i] = fit->GetParError(k + 1);
		fChi2[i] = fit->GetChisquare() / ndf;
	}
}

void TMrbEncal::FitGaussSingle() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::FitGaussSingle
// Purpose:        Fit: gauss, single peaks
// Arguments:      --
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	fXfit.Set(fNofPeaks);
	fXerr.Set(fNofPeaks);
	fYfit.Set(fNofPeaks);
	fYerr.Set(fNofPeaks);
	fChi2.Set(fNofPeaks);

	Int_t fitNofPeaks = 1;
	Int_t nPar = 3 + 2 * fitNofPeaks;
	for (Int_t i = 0; i < fNofPeaks; i++) {
		TArrayD p(nPar);
		// linear background
		p[0] = fCurHisto->GetBinContent(fCurHisto->FindBin(fPeakX[i] - fFitRange * fSigma));
		p[1] = 0.;
		// gauss
		p[2] = fSigma;
		p[3] = fPeakY[i];
		p[4] = fPeakX[i];

		TF1 * fit = new TF1(Form("g%d", i), gaus_lbg, fPeakX[i] - fFitRange * fSigma, fPeakX[i] + fFitRange * fSigma, nPar);
		fit->SetParameters(p.GetArray());
		if (fFitBackground == kFitModeBackgroundConst) fit->FixParameter(1, 0.0);
		if (this->Verbose()) {
			cout	<< endl << "---------------------------------------------------------------------" << endl
					<< "Gaussian start params for peak #" << i << ":" << endl;
		}
		Int_t np = 2;
		for (Int_t k = 0; k < nPar; k++, np++) {
			const Char_t * pn = ((TObjString *) fParamNames->At(np))->GetString().Data();
			fit->SetParName(k, pn);
			if (this->Verbose()) cout << Form("   %d: %-10s = %10f", k, pn, p[k]) << endl;
		}
		fit->SetLineColor(2);
		fCurHisto->Fit(fit, (i == 0) ? "R" : "R+");
		fit->GetParameters(p.GetArray());
		fYfit[i] = p[3];
		fYerr[i] = fit->GetParError(3);
		fXfit[i] = p[4];
		fXerr[i] = fit->GetParError(4);
		Int_t ndf = TMath::Max(1, fit->GetNDF());
		fChi2[i] = fit->GetChisquare() / ndf;
	}
	if (fFitResults) fFitResults->Append(fCurHisto);
}


void TMrbEncal::FitGaussEnsemble() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::FitGaussTailEnsemble
// Purpose:        Fit: gauss, group of peaks
// Arguments:      --
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	fXfit.Set(fNofPeaks);
	fXerr.Set(fNofPeaks);
	fYfit.Set(fNofPeaks);
	fYerr.Set(fNofPeaks);
	fChi2.Set(fNofPeaks);

	fitNofPeaks = (fNofPeaks > fNofPeaksNeeded) ? fNofPeaksNeeded : fNofPeaks;
	Int_t idxPeak1 = (fNofPeaks <= fitNofPeaks) ? 0 : (fNofPeaks - fitNofPeaks);
	Int_t nPar = 3 + 2 * fitNofPeaks;
	TArrayD p(nPar);
	// linear background
	p[0] = fCurHisto->GetBinContent(fCurHisto->FindBin(fPeakX[idxPeak1] - fFitRange * fSigma));
	p[1] = 0.;
	// gauss
	p[2] = fSigma;
	Int_t k = 3;
	for (Int_t i = idxPeak1; i < fNofPeaks; i++, k += 2) {
		p[k] = fPeakY[i];
		p[k + 1] = fPeakX[i];
	}

	TF1 * fit = new TF1("g_group", gaus_lbg, fPeakX[idxPeak1] - fFitRange * fSigma, fPeakX[fNofPeaks - 1] + fFitRange * fSigma, nPar);
	fit->SetParameters(p.GetArray());
	if (fFitBackground == TMrbEncal::kFitModeBackgroundConst) fit->FixParameter(1, 0.0);
	if (this->Verbose()) {
		cout	<< endl << "---------------------------------------------------------------------" << endl
				<< "Gaussian start params for a group of " << fitNofPeaks << " peak(s)" << endl;
	}
	for (Int_t k = 0; k < nPar; k++) {
		const Char_t * pn;
		Int_t kk;
		if (k <= 2) {
			kk = k + 2;
		} else {
			kk = (k & 1) ? 5 : 6;
		}
		pn = ((TObjString *) fParamNames->At(kk))->GetString().Data();
		if (k > 2) pn = Form("%s_%d", pn, (k - 3) / 2);
		fit->SetParName(k, pn);
		if (this->Verbose()) cout << Form("   %2d: %-10s = %10.4f", k + 1, pn, p[k]) << endl;
	}
	fit->SetLineColor(2);
	fCurHisto->Fit(fit, "R");
	if (fFitResults) fFitResults->Append(fCurHisto);
	fit->GetParameters(p.GetArray());
	Int_t ndf = TMath::Max(1, fit->GetNDF());
	k = 3;
	for (Int_t i = 0; i < fitNofPeaks; i++, k += 2) {
		fYfit[i] = p[k];
		fYerr[i] = fit->GetParError(k);
		fXfit[i] = p[k + 1];
		fXerr[i] = fit->GetParError(k + 1);
		fChi2[i] = fit->GetChisquare() / ndf;
	}
}

Int_t TMrbEncal::GetPeakFinderResults(TArrayF & PeakX, TArrayF & PeakY) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::GetPeakFinderResults
// Purpose:        Pass peak finder results to caller
// Arguments:      TArrayF & PeakX     -- centroids
//                 TArrayF & PeakY     -- amplitudes
// Results:        Int_t NofPeaks      -- number of peaks
// Description:    Returns peak finder data
//////////////////////////////////////////////////////////////////////////////

	PeakX.Set(fNofPeaks, fPeakX.GetArray());
	PeakY.Set(fNofPeaks, fPeakY.GetArray());
	return(fNofPeaks);
}

Int_t TMrbEncal::GetFitResults(TArrayF & Xfit, TArrayF & Xerr, TArrayF & Yfit, TArrayF & Yerr, TArrayF & Chi2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::GetFitResults
// Purpose:        Pass peak finder results to caller
// Arguments:      TArrayF & Xfit     -- centroids
//                 TArrayF & Xfit     -- errors
//                 TArrayF & Yfit     -- amplitudes
//                 TArrayF & Yerr     -- errors
//                 TArrayF & Chi2     -- chi sqaure
// Results:        Int_t NofPeaks      -- number of peaks
// Description:    Returns peak finder data
//////////////////////////////////////////////////////////////////////////////

	Xfit.Set(fNofPeaks, fXfit.GetArray());
	Xerr.Set(fNofPeaks, fXerr.GetArray());
	Yfit.Set(fNofPeaks, fYfit.GetArray());
	Yerr.Set(fNofPeaks, fYerr.GetArray());
	Chi2.Set(fNofPeaks, fChi2.GetArray());
	return(fNofPeaks);
}

void TMrbEncal::PrintFitResults() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::PrintFitResults
// Purpose:        Print results
// Arguments:      --
// Results:        --
// Description:    Print peak finder & fit results
//////////////////////////////////////////////////////////////////////////////

	cout << endl << "=========================================================================" << endl;
	cout << "File : " << fHistoFile->GetName() << endl;
	cout << "Histo: " << fCurHisto->GetName() << endl;
	cout << "Peaks:  #    X         X(fit)       Chi2" << endl;
	cout << "------------------------------------------------------------------------" << endl;
	for (Int_t i = 0; i < fNofPeaks; i++) {
		cout << Form("%9d   %7.2f   %7.2f       %7.5f", i, fPeakX[i], fXfit[i], fChi2[i]) << endl;
	}
	cout << "=========================================================================" << endl;
}

void TMrbEncal::Calibrate() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::Calibrate
// Purpose:        Perform energy calibration
// Arguments:      --
// Results:        --
// Description:    Starts energy calibration
//////////////////////////////////////////////////////////////////////////////

	if (!fEnableCalib) return;

	fCalX.Set(fNofPeaksNeeded);
	fCalE.Set(fNofPeaksNeeded);
	fCalXerr.Set(fNofPeaksNeeded);
	fCalEerr.Set(fNofPeaksNeeded);

	if (fNofPeaks >= fNofPeaksNeeded) { 
		Int_t k = fNofPeaks - 1;
		for (Int_t i = fNofPeaksNeeded - 1; i >= 0; i--, k--) {
			fCalX[i] = fXfit[k];
			fCalXerr[i] = fXerr[k];
		}

 		for (Int_t i = 0; i < fNofPeaksNeeded; i++) {
			fCalE[i] = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.E", fSourceName.Data(), i), 0.0);
			fCalEerr[i] = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Eerr", fSourceName.Data(), i), 0.0);
		}

		TGraphErrors * calib = new TGraphErrors(fNofPeaksNeeded, fCalX.GetArray(), fCalE.GetArray(), fCalXerr.GetArray(), fCalEerr.GetArray());
		calib->SetName(Form("calib_%s", fCurHisto->GetName()));
		calib->SetTitle(Form("%s Calibration for histo %s", fSourceName.Data(), fCurHisto->GetName()));
		calib->Draw("A*");
		calib->Fit("pol1");
		fPoly = calib->GetFunction("pol1");
		fPoly->SetLineColor(2);

		Int_t low = -1;
		Int_t up = -1;
		for (Int_t i = fLowerLim; i < fUpperLim; i++) {
			if (fCurHisto->GetBinContent(i) != 0) {
				if (low == -1) low = i;
				up = i;
			}
		}

		Double_t e = fPoly->GetParameter(0) + fPoly->GetParameter(1) * low;
		if (e < fEmin) {
			fXmin = low;
			fEmin = e;
		}
		e = fPoly->GetParameter(0) + fPoly->GetParameter(1) * up;
		if (e > fEmax) {
			fXmax = up;
			fEmax = e;
		}
		this->SetFitStatus(kFitOk);
	} else {
		this->OutputMessage("Calibrate", Form("Too few peaks - %d (%s calibration needs at least %d peaks)", fNofPeaks, fSourceName.Data(), fNofPeaksNeeded), kTRUE);
		this->SetFitStatus(kFitDiscard, "Too few peaks");
	}
}

void TMrbEncal::WriteResults() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::WriteResults
// Purpose:        Write results to file
// Arguments:      --
// Results:        --
// Description:    Writes results to .res file
//////////////////////////////////////////////////////////////////////////////

	fEnvResults->SetValue(Form("Calib.%s.Xmin", fCurHisto->GetName()), fLowerLim);
	fEnvResults->SetValue(Form("Calib.%s.Xmax", fCurHisto->GetName()), fUpperLim);

	fEnvResults->SetValue(Form("Calib.%s.NofPeaks", fCurHisto->GetName()), fNofPeaks);
	for (Int_t i = 0; i < fNofPeaks; i++) {
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.X", fCurHisto->GetName(), i), fPeakX[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Xfit", fCurHisto->GetName(), i), fXfit[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Xerr", fCurHisto->GetName(), i), fXerr[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Y", fCurHisto->GetName(), i), fPeakY[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Yfit", fCurHisto->GetName(), i), fYfit[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Yerr", fCurHisto->GetName(), i), fYerr[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Chi2", fCurHisto->GetName(), i), fChi2[i]);
	}
}

void TMrbEncal::WriteCalibration() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::WriteCalibration
// Purpose:        Write calibration data to file
// Arguments:      --
// Results:        --
// Description:    Writes calibration to .cal file
//////////////////////////////////////////////////////////////////////////////

	if (fEnableCalib && (fNofPeaks >= fNofPeaksNeeded)) {
		fEnvCalib->SetValue(Form("Calib.%s.Xmin", fCurHisto->GetName()), fLowerLim);
		fEnvCalib->SetValue(Form("Calib.%s.Xmax", fCurHisto->GetName()), fUpperLim);
		fEnvCalib->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fPoly->GetParameter(1));
		fEnvCalib->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fPoly->GetParameter(0));
		fEnvResults->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fPoly->GetParameter(1));
		fEnvResults->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fPoly->GetParameter(0));
		this->SetFitStatus(this->StepMode() ? kFitOk : kFitAuto);
	} else {
		this->SetFitStatus(kFitDiscard, "Too few peaks");
	}
}

void TMrbEncal::WriteFitStatus() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::WriteFitStatus
// Purpose:        Write fit status to file
// Arguments:      --
// Results:        --
// Description:    Writes fit status to .res file
//////////////////////////////////////////////////////////////////////////////

	TString fitStr;
	switch (fFitStatus) {
		case kFitOk: fitStr = "TRUE"; break;
		case kFitDiscard:	fitStr = "FALSE"; break;
		case kFitAuto:		fitStr = "AUTO"; break;
	}
	fEnvResults->SetValue(Form("Calib.%s.FitOk", fCurHisto->GetName()), fitStr.Data());
	fEnvResults->SetValue(Form("Calib.%s.Reason", fCurHisto->GetName()), fReason.Data());
}

void TMrbEncal::UpdateStatusLine() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::UpdateStatusLine
// Purpose:        Update status line
// Arguments:      --
// Results:        --
// Description:    Writes results status line
//////////////////////////////////////////////////////////////////////////////

	if (fNofPeaks >= fNofPeaksNeeded) {
		TString sts;
		if (fEnableCalib) {
			sts = Form("File %s, histo %s: E(x) = %5.2f + %5.2f * x", fHistoFile->GetName(), fCurHisto->GetName(), fPoly->GetParameter(0), fPoly->GetParameter(1));
		} else {
			sts = Form("File %s, histo %s: %d peak(s)", fHistoFile->GetName(), fCurHisto->GetName(), fNofPeaks);
		}
		this->OutputMessage("UpdateStatusLine", sts.Data(), kFALSE, 3);
	}
}

void TMrbEncal::SetFitStatus(Int_t FitStatus, const Char_t * Reason) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::SetFitStatus
// Purpose:        Set fit status
// Arguments:      Int_t FitStatus   -- status
//                 Char_t * Reason   -- reason
// Results:        --
// Description:    stores fit status
//////////////////////////////////////////////////////////////////////////////

	fFitStatus = FitStatus;
	fReason = (Reason == NULL || *Reason == '\0') ? "" : Reason;
}

void TMrbEncal::ShowResults2dim() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEncal::ShowResults2dim
// Purpose:        Fill 2-dim histogram
// Arguments:      --
// Results:        --
// Description:    Shows fit results in 2-dim histogram
//////////////////////////////////////////////////////////////////////////////

	if (!this->TwoDimMode()) return;

	TList * lofEntries = (TList *) fEnvCalib->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	TObjArray lofCalibs;
	while (r = (TEnvRec *) iter->Next()) {
		TString str = r->GetName();
		TObjArray * a = str.Tokenize(".");
		TString hn = ((TObjString *) a->At(1))->GetString();
		if (lofCalibs.FindObject(hn.Data()) == NULL) {
			TH1F * h = (TH1F *) fHistoFile->Get(hn.Data());
			if (h) {
				TString testIt = fEnvCalib->GetValue(Form("Calib.%s.Gain", hn.Data()), "");
				if (!testIt.IsNull()) {
					Double_t a1 = fEnvCalib->GetValue(Form("Calib.%s.Gain", hn.Data()), 1.);
					Double_t a0 = fEnvCalib->GetValue(Form("Calib.%s.Offset", hn.Data()), 0.);
					const Char_t * formula = Form("%f + %f * x[0]", a0, a1);
					TFormula * poly = new TFormula(hn.Data(), formula);
					lofCalibs.Add(poly);
					nh++;
				}
			}
		}
	}

	if (nh == 0) {
		this->OutputMessage("ShowResults2dim", Form("File %s - No histograms found", fHistoFile->GetName()), kTRUE);
	} else {
 		f2DimCanvas = new TCanvas();
		TString hTitle = Form("Calibration restults: file %s, %d histogram(s)", fHistoFile->GetName(), nh);
		f2DimFitResults = new TH2S("hCalResults", hTitle.Data(), (fXmax - fXmin), fEmin, fEmax, nh, 0, nh);
		if (fFitResults) fFitResults->Append(f2DimFitResults);
		TAxis * yAxis = f2DimFitResults->GetYaxis();

		for (Int_t n = 0; n < nh; n++) {
			TFormula * poly = (TFormula *) lofCalibs[n];
			TString hn = poly->GetName();
			TH1F * h = (TH1F *) fHistoFile->Get(hn.Data());
			if (h != NULL) {
				for (Int_t i = fXmin; i < fXmax; i++) {
					Int_t bCont = (Int_t) h->GetBinContent(i);
					Double_t bCent = h->GetBinCenter(i);
					for (Int_t j = 0; j < bCont; j++) {
						Double_t x = bCent + gRandom->Rndm() - 0.5;
						f2DimFitResults->Fill((Axis_t) poly->Eval(x), (Axis_t) n);
					}
				}
				yAxis->SetBinLabel(n + 1, hn.Data());
			}
		}
		f2DimFitResults->Draw();
		f2DimCanvas->Update();
	}
}

void Encal(TObjArray * LofHistos,
           Int_t CalSource,
           const Char_t * Energies,
           const Char_t * CalFile,
           const Char_t * ResFile,
           const Char_t * FitFile,
           Bool_t ClearFlag,
           const Char_t * PrecalFile,
           Int_t LowerLim,
           Int_t UpperLim,
           Int_t PeakFrac,
           Double_t Sigma,
           Int_t FitMode,
           Int_t FitGrouping,
           Int_t FitBackground,
           Double_t FitRange,
           Int_t DisplayMode,
           Bool_t VerboseMode)
//_____________________________________________________________[MAIN FUNCTION]
//
{
	msg = new TMrbLogger("Encal.log");

	TMrbEncal encal(	LofHistos,
						CalSource,
						Energies,
						CalFile,
						ResFile,
						FitFile,
						ClearFlag,
						PrecalFile,
						LowerLim,
						UpperLim,
						PeakFrac,
						Sigma,
						FitMode,
						FitGrouping,
						FitBackground,
						FitRange,
						DisplayMode,
						VerboseMode);

	if (!encal.OpenHistoFile()) return;

	if (!encal.SetCalSource()) return;

	encal.OpenCalFiles();

	TCanvas * mainCanvas = encal.OpenCanvas();

	encal.ResetLofHistos();
	TH1F * h;
	while (h = encal.GetNextHisto()) {

		mainCanvas->cd(1);
		if (h == NULL) {
			encal.ClearCanvas(0);
			encal.WaitForButtonPressed();
			if (encal.ButtonQuit()) encal.Exit();
			if (encal.ButtonStop()) break;
			continue;
		}

		encal.SetCurHisto(h);

		if (encal.FindPeaks(100)) {
			if (encal.GetFitMode() == TMrbEncal::kFitModeGausTail) {
				if (encal.GetFitGrouping() == TMrbEncal::kFitGroupingSinglePeak) {
					encal.FitGaussTailSingle();
				} else if (encal.GetFitGrouping() == TMrbEncal::kFitGroupingEnsemble) {
					encal.FitGaussTailEnsemble();
				}
			} else if (encal.GetFitMode() == TMrbEncal::kFitModeGaus) {
				if (encal.GetFitGrouping() == TMrbEncal::kFitGroupingSinglePeak) {
					encal.FitGaussSingle();
				} else if (encal.GetFitGrouping() == TMrbEncal::kFitGroupingEnsemble) {
					encal.FitGaussEnsemble();
				}
			}

			encal.PrintFitResults();

			encal.ClearCanvas(2);

			encal.Calibrate();

			mainCanvas->Update();
			gSystem->ProcessEvents();
		} else {
			mainCanvas->cd(1);
			h->Draw();
			encal.ClearCanvas(2);
		}

		encal.WriteResults();
		encal.UpdateStatusLine();

		encal.WaitForButtonPressed();
		if (encal.ButtonQuit()) encal.Exit();
		if (encal.ButtonStop()) break;
		if (!encal.ButtonOk()) continue;

		encal.WriteCalibration();
		encal.WriteFitStatus();
	}

	encal.WriteMinMax();

	encal.ShowResults2dim();

	encal.CloseEnvFiles();

	encal.OutputMessage(NULL, Form("%s - %d histogram(s), press \"execute\" to restart or \"quit\" to exit", encal.HistoFile()->GetName(), encal.GetNofHistosCalibrated()), kFALSE, TMrbEncal::kColorBlue);

	encal.WaitForButtonPressed(kTRUE);

	encal.CloseRootFile();

	return;
}
