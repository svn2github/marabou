#ifndef __DGFModule_h__
#define __DGFModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFModule.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFModule
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"

#include "TMrbDGF.h"
#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFModule
// Purpose:        A wrapper class for TMrbDGF
/// Description:   Stores internal DGF data;
//                 acts as interface to real DGF objects connected to CAMAC
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFModule : public TNamed {

	public:
		DGFModule() {};
		DGFModule(const Char_t * Name, const Char_t * Host, Int_t Crate, Int_t Station);
		~DGFModule() {};

		DGFModule(const DGFModule & f) {};	// default copy ctor

		inline void SetHost(const Char_t * HostName) { fHostName = HostName; };
		inline const Char_t * GetHost() { return(fHostName.Data()); };
		Bool_t SetCrate(Int_t Crate);
		inline Int_t GetCrate() { return(fCrate); };
		Bool_t SetStation(Int_t Station);
		inline Int_t GetStation() { return(fStation); };
		inline void SetAddr(TMrbDGF * Addr) { fAddr = Addr; };
		inline TMrbDGF * GetAddr() { return(fAddr); };
		inline void SetActive(Bool_t Flag = kTRUE) { fIsActive = Flag; };
		inline Bool_t IsActive() { return(fIsActive); };
				
		inline void SetClusterID(Int_t ClusterSerial = 0, const Char_t * ClusterColor = "", const Char_t * SegmentID = "", Int_t HexNum = 0) {
			fClusterID.Set((Int_t) (ClusterSerial << 12) + HexNum, ClusterColor, SegmentID);
		};
		inline TMrbNamedX * GetClusterID() { return(&fClusterID); };
		inline Int_t GetClusterSerial() const { return((Int_t) (fClusterID.GetIndex() >> 12)); }; 		// use TMrbNamedX object:
		inline Int_t GetClusterHexNum() const { return(fClusterID.GetIndex() & 0xFFF); }; 		// use TMrbNamedX object:
		inline const Char_t * GetClusterColor() const { return(fClusterID.GetName()); };	// index <- serial, name <- color
		inline const Char_t * GetClusterSegments() const { return(fClusterID.GetTitle()); };	// title <- segment info
		
	protected:
		Bool_t fIsActive;
		TString fHostName;
		Int_t fCrate;
		Int_t fStation;
		TMrbDGF * fAddr;
		TMrbNamedX fClusterID;

	ClassDef(DGFModule, 1)		// [DGFControl] Module definitions
};

#endif
