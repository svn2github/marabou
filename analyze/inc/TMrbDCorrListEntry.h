#ifndef __TMrbDCorrListEntry_h__
#define __TMrbDCorrListEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDCorrListEntry.h
// Purpose:        Class to manage a list of functions for doppler correction
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbDCorrListEntry.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDCorrListEntry
// Purpose:        An entry in user's list of doppler correction functions
// Description:    Bookkeeping: Connects dcorr functions to modules and params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbDCorrListEntry : public TObject {

	public:

		enum EMrbDCorrType	{	kDCorrNone			=	0,
								kDCorrConstFactor 	=	1,
								kDCorrFixedAngle 	=	2,
								kDCorrVariableAngle	=	3
							};

	public:
		TMrbDCorrListEntry(	TMrbNamedX * Module = NULL,								// ctor
							TMrbNamedX * Param = NULL,
							TF1 * Address = NULL) : fModule(Module),
													fParam(Param),
													fAddress(Address) {};
		virtual ~TMrbDCorrListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TMrbNamedX * GetParam() const { return(fParam); };
		inline void SetParam(TMrbNamedX * Param) { fParam = Param; };
		inline TF1 * GetAddress() const { return(fAddress); };
		inline void SetAddress(TF1 * Address) { fAddress = Address; };
		inline EMrbDCorrType GetType() { return(fType); };
		inline void SetBeta(Double_t Beta) { fBeta = Beta; };
		inline Double_t GetBeta() { return(fBeta); };
		inline Bool_t AngleInDegrees() { return(fAngleInDegrees); };

	protected:
		EMrbDCorrType fType;
		Bool_t fAngleInDegrees;
		Double_t fBeta;
		TMrbNamedX * fModule;
		TMrbNamedX * fParam;
		TF1 * fAddress;

	ClassDef(TMrbDCorrListEntry, 0) 	// [Analyze] List of dcorr functions
};

#endif
