#ifndef __TMrbCalibrationListEntry_h__
#define __TMrbCalibrationListEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCalibrationListEntry.h
// Purpose:        Class to manage a list of calibration functions
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbCalibrationListEntry.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCalibrationListEntry
// Purpose:        An entry in user's list of calibration functions
// Description:    Bookkeeping: Connects calibrations to modules and params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbCalibrationListEntry : public TObject {

	public:
		TMrbCalibrationListEntry(	TMrbNamedX * Module = NULL,								// ctor
									TMrbNamedX * Param = NULL,
									TF1 * Address = NULL) : fModule(Module),
															fParam(Param),
															fAddress(Address) {};
		virtual ~TMrbCalibrationListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TMrbNamedX * GetParam() const { return(fParam); };
		inline void SetParam(TMrbNamedX * Param) { fParam = Param; };
		inline TF1 * GetAddress() const { return(fAddress); };
		inline void SetAddress(TF1 * Address) { fAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TMrbNamedX * fParam;
		TF1 * fAddress;

	ClassDef(TMrbCalibrationListEntry, 0) 	// [Analyze] List of calibration functions
};

#endif
