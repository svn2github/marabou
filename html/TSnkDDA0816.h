#ifndef __TSnkDDA0816_h__
#define __TSnkDDA0816_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816.h
// Purpose:        Class defs to describe a dac DDA0816
// Class:          TSnkDDA0816: main class
// Description:    Class definitions to handle a dac type DDA0816
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TSnkDDAChannel.h"
#include "TSnkDDAMessage.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816
// Purpose:        Class defs to handle a dac type DDA0816
// Description:    Describes a dac module DDA0816.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TSnkDDA0816 : public TNamed {

	public:
		TSnkDDA0816(Int_t Subdevice = 0);				// ctor

		~TSnkDDA0816() {};								// default dtor

		inline Bool_t Reset() {
			fDacX.Reset();
			fDacY.Reset();
			fDacH.Reset();
			fScanMode = kSnkSMUndef;
			fPacerClock = 1;
			fVerbose = kFALSE;
			fScanInterval = kSnkMaxInterval;
			this->ResetCalibration();
			fDacX.SetStopPos(this->Ampl2Dist(0, kSnkOffset0), kSnkOffset0);
			fDacY.SetStopPos(this->Ampl2Dist(1, kSnkOffset0), kSnkOffset0);
			return(kTRUE);
		};

		inline Bool_t SetPreScaler(ESnkPreScale PreScale) { fPreScale = PreScale; return(kTRUE); };
		Bool_t SetPreScaler(const Char_t * PreScale);
		inline TMrbNamedX * GetPreScaler() { return(fLofPreScales.FindByIndex(fPreScale)); };
		inline Bool_t SetClockSource(ESnkClockSource ClockSource) { fClockSource = ClockSource; return(kTRUE); };
		Bool_t SetClockSource(const Char_t * ClockSource);
		inline TMrbNamedX * GetClockSource() { return(fLofClockSources.FindByIndex(fClockSource)); };
		Bool_t SetSoftScale(Int_t Channel);
		inline void ClearSoftScale() { fDacX.SetSoftScale(0); fDacY.SetSoftScale(0); };
		Int_t GetSoftScale(Int_t Channel);
		Bool_t SetCurve(Int_t Channel, TArrayI & Data, Int_t Size = kSnkMaxPoints);
		Bool_t AdjustSettings(Int_t Channel, Bool_t Verbose = kTRUE);
		Bool_t SetOffset(Int_t Channel, Int_t Offset);
		Int_t GetOffset(Int_t Channel);
		Bool_t SetAmplitude(Int_t Channel, Int_t Offset);
		Int_t GetAmplitude(Int_t Channel);
		Bool_t SetIncrement(Int_t Channel, Int_t Increment);
		Int_t GetIncrement(Int_t Channel);
		Bool_t SetScanProfile(Int_t Channel, ESnkScanProfile ScanProfile);
		Bool_t SetScanProfile(Int_t Channel, const Char_t * ScanProfile);
		TMrbNamedX * GetScanProfile(Int_t Channel);

		inline Double_t GetScanInterval() { return(fScanInterval); };

		inline void SetPacerClock(Int_t ClockValue) { fPacerClock = ClockValue; };
		inline Int_t GetPacerClock() { return(fPacerClock); };

		inline void SetScanMode(ESnkScanMode Mode) { fScanMode = Mode; };
		inline ESnkScanMode GetScanMode() { return(fScanMode); };

		Bool_t SetCurveSymm(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveAsymm(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveXOnly(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveYOnly(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveXYConst(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveLShape(Bool_t SaveFlag = kTRUE);
		Bool_t SetCurveFromFile(const Char_t * FileName, Bool_t SaveFlag = kTRUE);

		Bool_t ConnectToServer(const Char_t * ServerName = "localhost", Int_t Port = kSnkDefaultPort,
																			const Char_t * ServerProg = NULL);
		inline Bool_t IsConnected() { return(fSocket != NULL); };
				
		Bool_t StartScan(Double_t Interval = kSnkMaxInterval);
		Bool_t StopScan();
		Bool_t KillServer();
		Bool_t ResetScan();

		Bool_t SaveAs(const Char_t *);
		Bool_t SaveDefault();
		Bool_t Save(const Char_t * SaveFile);

		TSnkDDAChannel * GetDac(Int_t);

		Bool_t SetSubdevice(Int_t Subdevice);				// define subdevice
		inline Int_t GetSubdevice() { return(fSubdevice); };
		const Char_t * GetSubdeviceName();
		
		const Char_t * GetSaveScan() { return(fSaveScan); };

		inline void SetAmplYScale(Double_t Scale) { fAmplYScale = Scale; }; 	// scale factor Y
		inline Double_t GetAmplYScale() { return(fAmplYScale); };
																				// calibration				
		inline Bool_t IsXorY(Int_t XorY) { return(XorY == 0 || XorY == 1); };
		inline Int_t GetChargeState() { return(fChargeState); };
		inline Double_t GetEnergy() { return(fEnergy); };

		inline Double_t GetVoltage(Int_t XorY) { return(IsXorY(XorY) ? this->GetDac(XorY)->GetVoltage() : 1.); };
		inline Double_t GetStepWidth(Int_t XorY) { return(IsXorY(XorY) ? this->GetDac(XorY)->GetStepWidth() : 1.); };
		inline Double_t GetRange(Int_t XorY) { return(IsXorY(XorY) ? this->GetDac(XorY)->GetRange() : 0.); };

		Bool_t Calibrate(Int_t ChargeState, Double_t Energy, Double_t VoltageX, Double_t VoltageY);
		Int_t Dist2Ampl(Int_t XorY, Double_t Distance);
		Double_t Ampl2Dist(Int_t XorY, Int_t Amplitude);
		void ResetCalibration();

		Bool_t SetStopPos(Int_t XorY, Double_t Pos, Int_t Pos0);
		inline Double_t GetStopPos(Int_t XorY) { return(IsXorY(XorY) ? this->GetDac(XorY)->GetStopPos() : 0.); };

		inline UInt_t GetExecMode() { return((UInt_t) fExecMode); };		// exec mode
		inline void SetExecMode(UInt_t ExecMode) { fExecMode = ExecMode; };

		inline void SetNormalMode() { fExecMode = kSnkEMNormal; };

		inline void SetVerbose() { fExecMode = (ESnkExecMode) (fExecMode | kSnkEMVerbose); };
		inline Bool_t IsVerbose() { return((fExecMode & kSnkEMVerbose) != 0); };

		inline void SetDebug() { fExecMode = (ESnkExecMode) (fExecMode | kSnkEMDebug); };
		inline Bool_t IsDebug() { return((fExecMode & kSnkEMDebug) != 0); };

		inline void SetOffline() { fExecMode = (ESnkExecMode) (fExecMode | kSnkEMOffline); };
		inline Bool_t IsOffline() { return((fExecMode & kSnkEMOffline) != 0); };

		inline void SetOnline() { fExecMode = (ESnkExecMode) (fExecMode & ~kSnkEMOffline); };

		void SetScanDataFile(const Char_t * FileName) { fScanDataFile = FileName; };
		inline const Char_t * GetScanDataFile() { return(fScanDataFile.Data()); };

		TSocket * GetConnection(TString & Server, Int_t & Port);
		Bool_t SetConnection(TSocket * Socket, const Char_t * Server, Int_t Port);

		void Print(Int_t Channel = -1);					// print settings

		inline TMrbLofNamedX * GetLofPreScales() { return(&fLofPreScales); };
		inline TMrbLofNamedX * GetLofClockSources() { return(&fLofClockSources); };
		inline TMrbLofNamedX * GetLofScanProfiles() { return(&fLofScanProfiles); };
		inline TMrbLofNamedX * GetLofScanModes() { return(&fLofScanModes); };
		inline TMrbLofNamedX * GetLofExecModes() { return(&fLofExecModes); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TSnkDDA0816.html&"); };

	protected:
		TSnkDDAChannel * CheckChannel(Int_t, const Char_t *, const Char_t *);
		Bool_t CheckParams(const Char_t *, const Char_t *);
		Bool_t CheckConnect(const Char_t *, const Char_t *);
		Int_t SetTriangle(TArrayI &, Int_t, Int_t, Int_t, Int_t);
		Int_t SetSawToothLR(TArrayI &, Int_t, Int_t, Int_t, Int_t);
		Int_t SetSawToothRL(TArrayI &, Int_t, Int_t, Int_t, Int_t);
		Int_t SetLShape(TArrayI &, Int_t, Int_t, Int_t, Int_t, Int_t);
		Bool_t SetHysteresis();
		Bool_t SetBeamOn();


	protected:
		Int_t fSubdevice;				// subdevice: 0 (chn 0-2), 1 (chn 4-6), 2 (chn 8-10), 3 (chn 12-14)
		TString fSubdeviceName; 		// ...
		
		TSnkDDAChannel fDacX;			// channel X
		TSnkDDAChannel fDacY;			// channel Y
		TSnkDDAChannel fDacH;			// channel H (hysteresis)
		TSnkDDAChannel fDacB;			// channel B (beam on/off)
		TSnkDDAChannel fDacError;		// dummy in case of error

		Double_t fAmplYScale;			// scale factor Y in case of symmetric scan

										// calibration steps <-> distance
		Int_t fChargeState;				// charge number
		Int_t fMassNo;					// mass number
		Double_t fEnergy;				// beam energy [MeV]

		Int_t fPacerClock; 				// pacer clock
		ESnkPreScale fPreScale;			// prescale frequency
		ESnkClockSource fClockSource;	// clock source

		ESnkScanMode fScanMode; 		// scan mode
		TString fSaveScan;				// save of current scan data

		TString fScanDataFile;			// file name if scan data taken from file

		Bool_t fVerbose;				// kTRUE if verbose mode

		Double_t fScanInterval;			// interval in seconds

		UInt_t fExecMode; 				// exec mode

		TSocket * fSocket;				//! connection to server
		TString fServer;				// server name
		Int_t fPort;					// port number
		
		TMrbLofNamedX fLofPreScales; 		// prescaler values
		TMrbLofNamedX fLofClockSources; 	// clock sources
		TMrbLofNamedX fLofScanProfiles;	 	// scan profiles
		TMrbLofNamedX fLofScanModes;	 	// scan modes
		TMrbLofNamedX fLofExecModes;	 	// exec modes

	ClassDef(TSnkDDA0816, 1)		// [SNAKE] Base class for DAC module DDA0816
};

#endif
