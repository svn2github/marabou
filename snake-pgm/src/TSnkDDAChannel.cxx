//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAChannel.cxx
// Purpose:        Implement class methods for dac module DDA0816
// Description:    Class methods to operate a dac type DDA0816
//                 in the SNAKE environment
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TSnkDDAChannel.cxx,v 1.1 2009-03-26 11:13:42 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TMrbNamedX.h"
#include "TMrbLogger.h"

#include "TSnkDDA0816.h"
#include "TSnkDDAChannel.h"
#include "TSnkDDACommon.h"

#include "SetColor.h"

ClassImp(TSnkDDAChannel)

extern TMrbLogger * gMrbLog;

Bool_t TSnkDDAChannel::Reset(Int_t MaxPoints) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAChannel::Reset
// Purpose:        Reset data base
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Resets channel data 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fOffset = -1;								// must be defined by caller
	fAmplitude = -1;
	fIncrement = -1;
	fSoftScale = -1;
	fNofPixels = -1;
	fScanProfile = kSnkSPUndef;
	fVoltage = 0;
	fStepWidth = 1;
	fRange = (Double_t) kSnkMaxAmplitude;
	fStopPos = 0;
	fPos0 = kSnkOffset0;

	fMaxPoints = MaxPoints;

	fCurveData.Reset();

	return(kTRUE);
}

Bool_t TSnkDDAChannel::SetCurve(TArrayI & CurveData, Int_t NofDataPoints) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAChannel::SetCurve
// Purpose:        Fill curve buffer
// Arguments:      TArrayI & CurveData      -- data to be filled in
//                 Int_t NofDataPoints      -- number of data points
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Passes curve data to internal struct. 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (NofDataPoints > fMaxPoints) {
		gMrbLog->Err() << "::SetCurve: Too many data points - " << NofDataPoints << " (max " << fMaxPoints << endl;
		gMrbLog->Flush(this->ClassName(), "SetCurve");
		return(kFALSE);
	}
	this->SetNofDataPoints(NofDataPoints);

	fCurveData.Set(fMaxPoints);
	fCurveData.Reset();
	fCurveData.Set(NofDataPoints, CurveData.GetArray());
	return(kTRUE);
}

void TSnkDDAChannel::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAChannel::Print
// Purpose:        Output current settings
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Output channel settings.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;

	cout	<< this->ClassName() << "::Print(): Current DAC settings:" << endl
			<< "----------------------------------------------" << endl;
	cout	<< "Device          : " << fDevice << endl;
	cout	<< "Channel         : " << fChannel << endl;

	cout	<< "SoftScale       : ";	if (fSoftScale == -1)	cout << setred << "undef" << setblack;
										else					cout << fSoftScale; cout << endl; 
	cout	<< "Offset          : ";	if (fOffset == -1)		cout << setred << "undef" << setblack;
										else					cout << fOffset; cout << endl; 
	cout	<< "Amplitude       : ";	if (fAmplitude == -1)	cout << setred << "undef" << setblack;
										else					cout << fAmplitude; cout << endl; 
	cout	<< "Increment       : ";	if (fIncrement == -1)	cout << setred << "undef" << setblack;
										else					cout << fIncrement; cout << endl; 
	cout	<< "ScanProfile     : ";
	if (fScanProfile == kSnkSPUndef) {
		cout	<< setred << "undef"
				<< setblack << endl;
	} else {
		kp = ((TSnkDDA0816 *) fParent)->GetLofScanProfiles()->FindByIndex(fScanProfile);
		cout	<< kp->GetName() << endl;
	}

	cout	<< "Stop at         : " << fStopPos << " (" << fPos0 << ")" << endl;

	cout	<< "Data points     : ";	if (fNofPixels == -1)	cout << setred << "undef" << setblack;
										else					cout << fNofPixels; cout << endl; 

	cout	<< endl;
}
