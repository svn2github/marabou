#ifndef __TSnkDDAChannel_h__
#define __TSnkDDAChannel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDA0816.h
// Purpose:        Class defs to describe a dac DDA0816
// Class:          TSnkDDAChannel: a single DAC channel
// Description:    Class definitions to handle a dac type DDA0816
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TArrayI.h"
#include "TString.h"

#include "TSnkDDACommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAChannel
// Purpose:        Class defs to decsribe a single dac channel
// Description:    Describes a single dac channel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TSnkDDAChannel : public TObject {

	public:
		TSnkDDAChannel() { Reset(); };					// default ctor
		~TSnkDDAChannel() {};							// default dtor

		void SetChannel(Int_t ChannelNo, const Char_t * DeviceName, const Char_t * AxisName, TObject * Parent) {
			fChannel = ChannelNo;
			fDevice = DeviceName;
			fAxis = AxisName;
			fParent = Parent;
		};

		inline Int_t GetChannel	() { return(fChannel); };
		inline const Char_t * GetDeviceName() { return(fDevice.Data()); };
		inline const Char_t * GetAxisName() { return(fAxis.Data()); };

		Bool_t Reset();									// reset data base

		Bool_t SetCurve(TArrayI &, Int_t NofPixels = kSnkMaxPoints);	// fill in curve data
		inline Int_t * GetCurveData() { return(fCurveData.GetArray()); };

		inline void SetOffset(Int_t Offset) 					{ fOffset = Offset; };
		inline void SetAmplitude(Int_t Amplitude)				{ fAmplitude = Amplitude; };
		inline void SetIncrement(Int_t Increment)				{ fIncrement = Increment; };
		inline void SetSoftScale(Int_t Scale)					{ fSoftScale = Scale; };
		inline void SetScanProfile(ESnkScanProfile ScanProfile) { fScanProfile = ScanProfile; };
		inline void SetNofPixels(Int_t NofPixels)  				{ fNofPixels = NofPixels; };
		inline void SetNofDataPoints(Int_t NofDataPoints)  	{ fNofDataPoints = NofDataPoints; };

		inline Int_t GetOffset()								{ return(fOffset); };
		inline Int_t GetAmplitude() 							{ return(fAmplitude); };
		inline Int_t GetIncrement()							{ return(fIncrement); };
		inline Int_t GetSoftScale()							{ return(fSoftScale); };
		inline ESnkScanProfile GetScanProfile()					{ return(fScanProfile); };
		inline Int_t GetNofPixels() 							{ return(fNofPixels); };
		inline Int_t GetNofDataPoints() 					 	{ return(fNofDataPoints); };

		void Print();					// print settings

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TSnkDDAChannel.html&"); };

	protected:
		TObject * fParent;				// link to parent DDA0816

		Int_t fChannel;				// dac channel
		TString fDevice;				// device name (/dev/dda08_N)
		TString fAxis;					// axis name
		Int_t fSoftScale;				// soft scale
		Int_t fOffset; 	 			// offset
		Int_t fAmplitude;				// amplitude
		Int_t fIncrement;				// increment
		ESnkScanProfile fScanProfile; 	// scan profile
		Int_t fNofPixels;				// scan size
		Int_t fNofDataPoints;			// number of curve data points
		TArrayI fCurveData; 			// curve buffer

	ClassDef(TSnkDDAChannel, 1) 	// [SNAKE] Single channel of DAC module DDA0816
};

#endif
