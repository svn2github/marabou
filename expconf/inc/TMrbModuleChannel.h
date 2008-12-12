#ifndef __TMrbModuleChannel_h__
#define __TMrbModuleChannel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModuleChannel.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleChannel    -- base class to describe a single channel
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbModuleChannel.h,v 1.8 2008-12-12 13:09:57 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TNamed.h"

#include "TMrbModule.h"
#include "TMrbSubevent.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel
// Purpose:        Define methods for module channels
// Description:    Describes a single module channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbModuleChannel : public TNamed {

	public:
		enum EMrbHistoMode	{	kMrbHasHistogramDefault,
								kMrbHasHistogramFalse,
								kMrbHasHistogramTrue
							};

	public:

		TMrbModuleChannel() {};									// default ctor

		TMrbModuleChannel(TMrbModule * Module, Int_t Addr); 	// ctor: link channel to parent

		~TMrbModuleChannel() {									// default dtor
			fRegSpec.Delete();
		};

		inline Bool_t MarkUsed(TMrbSubevent * Sevt) {			// mark as used
			fIsUsed = kTRUE;
			fUsedBy = Sevt;
			return(kTRUE);
		};

		inline Bool_t MarkIndexed(TMrbModuleChannel * Param0) {	// mark as indexed with respect to param0
			fArrayHead = Param0;
			fStatus = TMrbConfig::kChannelArrElem;
			return(kTRUE);
		};

		Bool_t Set(const Char_t * RegName, Int_t Value);				// set register by name
		Bool_t Set(Int_t RegIndex, Int_t Value);						// ... by index
		Int_t Get(const Char_t * RegName) const;						// get register by name
		Int_t Get(Int_t RegIndex) const;								// ... by index

		Bool_t Set(const Char_t * RegName, const Char_t * Value);		// set register mnemonically
		Bool_t Set(Int_t RegIndex, const Char_t * Value);				// ... by index

		inline Bool_t HasRegisters() const { return(fRegSpec.GetEntriesFast() > 0); };	// check if regs present

		inline Int_t GetAddr() const { return(fAddr); };						// return channel address

		inline TMrbSubevent * UsedBy() const { return(fUsedBy); }; 			// return address of subevent
		inline TMrbModule * Parent() const { return(fParent); };  			// return address of parent module

		inline TMrbModuleChannel * ArrayHead() const { return(fArrayHead); };	// return array addr
		inline Int_t GetIndexRange() const { return(fIndexRange); };  		// get number of subsequent channels
		inline void SetIndexRange(Int_t Range) { fIndexRange = Range; };			// define number of subsequent channels
		inline TMrbConfig::EMrbChannelStatus GetStatus() const { return(fStatus); }; 		// get channel status
		inline void SetStatus(TMrbConfig::EMrbChannelStatus Status) { fStatus = Status; };	// set status bits
		Int_t GetIndex() const;												// get index with respect to array head

		Bool_t SetHeadName(const Char_t * HeadName); 				// store name of param head in case it is indexed
		const Char_t * GetHeadName() const;							// return name w/o index

		inline void SetOffset(Int_t Offset) { fOffset = Offset; };			// set channel offset within module
		inline Int_t GetOffset() const { return(fOffset); }; 				// get offset

		inline Bool_t IsUsed() const { return(fIsUsed); };					// param in use?

		inline EMrbHistoMode GetHistoMode() const { return(fHistoMode); };	// histogram to be allocated?
		inline void SetHistoMode(EMrbHistoMode HistoMode) { fHistoMode = HistoMode; };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		virtual void Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix = "") {};	 // show registers
		inline void Print() { Print(cout, kFALSE, kFALSE, ""); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TMrbModule * fParent;					// parent module
		TMrbSubevent * fUsedBy;					// address of subevent

		TMrbModuleChannel * fArrayHead;			// address of channel with index 0
		Int_t fIndexRange;	 					// number of subsequent channels indexed from this one
		TMrbConfig::EMrbChannelStatus fStatus; 	// status bits

		TString fHeadName; 						// name of param array if any

		Int_t fAddr;							// channel address
		Int_t fOffset;							// channel offset
		Bool_t fIsUsed;							// kTRUE if used
		EMrbHistoMode fHistoMode;				// histogram to be booked for this param?

		TObjArray fRegSpec; 					// internal registers

	ClassDef(TMrbModuleChannel, 1)		// [Config] Base class describing a single module channel
};	

#endif
