#ifndef __TPolControl_h__
#define __TPolControl_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           polar/inc/TPolControl.h
// Purpose:        Class defs to control the polarized ion source
// Description:    Class definitions to control Ralf Hertenberger's polarized ion source
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"

#include "TH1.h"

#include "TMrbLofNamedX.h"
#include "TMrbSerialComm.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl
// Purpose:        Control the polaraized ion source
// Description:    Controls the polarized ion source via a RS232 interface
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TPolControl : public TMrbSerialComm {

	public:
		enum	{	kPolNofAdcs 	= 32		};		// number of adcs per subdevice
		enum	{	kPolNofDacs 	= 20		};		// number of dacs per subdevice
		enum	{	kPolAdcRange 	= 32768 	};		// adc range
		enum	{	kPolDacRange 	= 4095		};		// dac range
		enum	{	kPolMaxVoltage 	= 10		};		// max voltage
		enum	{	kPolAdcBit	 	= BIT(17)	};		// mark table entry as adc
		enum	{	kPolDacBit	 	= BIT(16)	};		// mark table entry as dac
		enum	{	kPolChannelMask = 0xFFFF 	};		// channel mask

		enum	{	kPolIdxName, kPolIdxSubdev, kPolIdxChannel, kPolIdxType, kPolIdxComment };
		enum	{	kPolNofFields	= kPolIdxComment - kPolIdxName + 1	};

 	public:
		TPolControl(const Char_t * Device = "/dev/ttyS0");
		virtual ~TPolControl() {};

		Int_t ReadAdc(Int_t Subdev, Int_t Channel, Bool_t LowResolution = kFALSE);	// read adc, relative addr
		Int_t ReadAdc(Int_t AbsChannel, Bool_t LowResolution = kFALSE); 			// read adc, absolute addr
		Int_t ReadAdc(const Char_t * ChnName, Bool_t LowResolution = kFALSE);		// read adc, addressed by name

		Int_t SetDac(Int_t Subdev, Int_t Channel, Double_t DacValue,				// set dac, relative addr
								Int_t ReadBack = -1, Bool_t LowResolution = kFALSE);
		Int_t SetDac(Int_t AbsChannel, Double_t DacValue,							// set dac, absolute addr
								Int_t ReadBack = -1, Bool_t LowResolution = kFALSE);
		Int_t SetDac(const Char_t * ChnName, Double_t DacValue,						// set dac, addressed by name
								const Char_t * ReadBack = NULL, Bool_t LowResolution = kFALSE);	// set dac
		void SetWait(Int_t WaitAV, Int_t WaitAR = 0, Int_t WaitSD = 0); 			// set wait states
		void SetSubdevs(Int_t S1, Int_t S2 = -1, Int_t S3 = -1, Int_t S4 = -1, Int_t S5 = -1);	// define subdevices

		Int_t ReadNameTable(const Char_t * FileName = "polNames.dat");		// read name table
		void ClearNameTable() { fNameTable.Delete(); fNofTableEntries = 0; };
		TMrbNamedX * AddName(const Char_t * NameString);	// add an item to name table
		TMrbNamedX * AddName(const Char_t * Name, Int_t Subdevice, Int_t Channel,
								const Char_t * Type = "A", const Char_t * Comment = NULL);

		TH1F * Plot(	const Char_t * HistoName,					// plot, relative addr
						Int_t Subdev, Int_t X, Int_t Y,
						Double_t Xmin, Double_t Xmax, Int_t Steps,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		TH1F * Plot(	const Char_t * HistoName,					// plot, absolute addr
						Int_t X, Int_t Y,
						Double_t Xmin, Double_t Xmax, Int_t Steps,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		TH1F * Plot(	const Char_t * HistoName,					// plot, addr by name
						const Char_t * X, const Char_t * Y,
						Double_t Xmin, Double_t Xmax, Int_t Steps,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		inline void Draw() { if (fHisto) fHisto->Draw(); };

		virtual void Print();										// print settings

		void PrintNameTable();										// print adc/dac names
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TPolControl.html&"); };

	protected:
		Bool_t CheckAdcChannel(Int_t Subdev, Int_t Channel, const Char_t * Method = NULL);
		Bool_t CheckAdcChannel(Int_t AbsChannel, const Char_t * Method = NULL);
		Bool_t CheckDacChannel(Int_t Subdev, Int_t Channel, const Char_t * Method = NULL);
		Bool_t CheckDacChannel(Int_t AbsChannel, const Char_t * Method = NULL);
		Bool_t CheckDacValue(Double_t DacValue, const Char_t * Method = NULL);
		Bool_t CheckSubdev(Int_t Subdev, const Char_t * Method = NULL);

	protected:
		TH1F * fHisto;				//! histogram to store XY plot

		TString fNameTableFile; 	//! file where to get names from
		Int_t fNofTableEntries; 	// number of entries in name table
		TMrbLofNamedX fNameTable;	// name table

		UInt_t fSubdevPattern;		// bit pattern for subdevs

		Int_t fWaitAV;				// wait states
		Int_t fWaitAR;
		Int_t fWaitSD;

	ClassDef(TPolControl, 1) 		// [Polar] Control polarized ion source
};

#endif
