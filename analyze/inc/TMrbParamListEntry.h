#ifndef __TMrbParamListEntry_h__
#define __TMrbParamListEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbParamListEntry.h
// Purpose:        Class to manage the list of parameters in an experiment
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbParamListEntry.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TH1.h"
#include "TF1.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbParamListEntry
// Purpose:        An entry in user's param list
// Description:    Bookkeeping: Connects params to modules and histos
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbParamListEntry : public TObject {

	public:
		TMrbParamListEntry( TMrbNamedX * Module = NULL,								// ctor
							TObject * Address = NULL,
							TH1 * HistoAddress = NULL,
							TH1 * SingleAddress = NULL) :	fModule(Module),
															fAddress(Address),
															fHistoAddress(HistoAddress),
															fSingleAddress(SingleAddress) {};
		virtual ~TMrbParamListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TObject * GetAddress() const { return(fAddress); };
		inline void SetAddress(TObject * Address) { fAddress = Address; };
		inline TH1 * GetHistoAddress(Bool_t SingleFlag = kFALSE) const {
			return(SingleFlag ? fSingleAddress : fHistoAddress);
		};
		inline void SetHistoAddress(TH1 * Address, Bool_t SingleFlag = kFALSE) {
			if (SingleFlag) fSingleAddress = Address;
			else			fHistoAddress = Address;
		};
		inline TF1 * GetCalibrationAddress() const { return(fCalibrationAddress); };
		inline void SetCalibrationAddress(TF1 * Address) { fCalibrationAddress = Address; };
		inline TF1 * GetDCorrAddress() const { return(fDCorrAddress); };
		inline void SetDCorrAddress(TF1 * Address) { fDCorrAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TObject * fAddress;
		TH1 * fHistoAddress;
		TH1 * fSingleAddress;
		TF1 * fCalibrationAddress;
		TF1 * fDCorrAddress;

	ClassDef(TMrbParamListEntry, 0) 	// [Analyze] List of params
};

#endif
