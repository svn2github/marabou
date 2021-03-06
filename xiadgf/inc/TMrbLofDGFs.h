#ifndef __TMrbLofDGFs_h__
#define __TMrbLofDGFs_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbLofDGFs.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbLofDGFs         -- a list of DGF modules
// Description:    Class definitions to operate XIA DGF-4C modules
// Author:         R. Lutter
// Revision:       $Id: TMrbLofDGFs.h,v 1.5 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TMrbDGF.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs
// Purpose:        Class defs describing a list of XIA DGF-4C modules
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofDGFs : public TObjArray {

	public:

		TMrbLofDGFs() { 			// ctor
			fCamac = NULL;
			fCrate = 0;
			fStationMask = 0;
			fDGFData = NULL;
			fMultiCrate = kFALSE;
			fAbortFlag = kFALSE;
		};
		
		~TMrbLofDGFs() {};		 	// default dtor

		Bool_t AddModule(TMrbDGF * Dgf, Bool_t MultiCrate = kFALSE);
		inline TMrbDGF * FindModule(const Char_t * DgfName) { return((TMrbDGF *) this->FindObject(DgfName)); };
		inline TMrbDGF * FindModule(TMrbDGF * Dgf) { return((TMrbDGF *) this->FindObject(Dgf->GetName())); };
				
		// FPGA section
		Bool_t DownloadFPGACode(TMrbDGFData::EMrbFPGAType FPGAType);	// download code to system or fippi FPGA
		Bool_t DownloadFPGACode(const Char_t * FPGAType);
		Bool_t FPGACodeLoaded(TMrbDGFData::EMrbFPGAType FPGAType);		// test if download ok
		Bool_t FPGACodeLoaded(const Char_t * FPGAType);

		// DSP section
		Bool_t DownloadDSPCode(Int_t Retry = 3);					// download code to DSP
		Bool_t DSPCodeLoaded(); 										// test if download ok

		inline void Wait(Int_t Msecs = 100) { gSystem->Sleep(Msecs); };	// wait for DGF to settle down
		TMrbDGF::EMrbWaitStatus WaitActive(Int_t Timeout = 10);					// wait for active bit to drop

		inline TMrbDGFData * Data() { return(fDGFData); };				// data handle
		inline TMrbEsone * Camac() { return(fCamac); }; 				// camac handle
		
		inline UInt_t GetStationMask() { return(fStationMask); };

		inline void ResetAbort() { fAbortFlag = kFALSE; };
		inline void Abort() { fAbortFlag = kTRUE; };
		inline Bool_t IsAborted() { return(fAbortFlag); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t CheckConnect(const Char_t * Method);						// check if connected to CAMAC
		Bool_t CheckModules(const Char_t * Method);						// check if there are modules in the list
		
																		// camac access:
		Bool_t WriteICSR(UInt_t Bits);									//		write ICSR (Control Status Reg)
		Bool_t WriteCSR(UInt_t Bits);									//		write CSR (Control Status Reg)
		Bool_t WriteTSAR(UInt_t Data);									//		write TSAR (Transfer Start Address Reg)
		Bool_t WriteWCR(Int_t WordCount); 								//		write WCR (Word Count Reg)

		inline void CopyData(TArrayI & Dest, Short_t * Src, Int_t NofWords) {	// copy back and forth
			for (Int_t i = 0; i < NofWords; i++) Dest[i] = (Int_t) *Src++;
		};
		inline void CopyData(TArrayS & Dest, Int_t * Src, Int_t NofWords) {
			for (Int_t i = 0; i < NofWords; i++) Dest[i] = (Short_t) *Src++;
		};
		inline void CopyData(TArrayI & Dest, UShort_t * Src, Int_t NofWords) {
			for (Int_t i = 0; i < NofWords; i++) Dest[i] = (Int_t) *Src++;
		};

	protected:
		TMrbEsone * fCamac;												//! camac handle via esone rpc
		TString fCamacHost; 											// host name
		Int_t fCrate;													// crate number

		Bool_t fBroadCast; 												// kTRUE if broadcast available
		Int_t fNsetMask;												// station N to set broadcast mask reg
		Int_t fNexecCmd;												// station N to exec broadcast command

		UInt_t fStationMask;											// camac station mask: 1 bit per module

		Bool_t fAbortFlag;												// used to abort wait loop
		Bool_t fMultiCrate; 											// kTRUE if modules from different crates

		TMrbDGFData * fDGFData; 										//! pointer to DGF data base

	ClassDef(TMrbLofDGFs, 1)		// [XIA DGF-4C] A list of DGF modules
};

#endif
