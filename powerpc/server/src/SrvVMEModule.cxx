//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvVMEModule.cxx
//! \brief			MARaBOU to Lynx: VME module
//! \details		Implements class methods to handle VME modules
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $
//! $Date: 2010-10-04 10:43:26 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrLogger.h"
#include "SrvUtils.h"
#include "SrvVMEModule.h"
#include "SetColor.h"

typedef int intptr_t;

#include "signal.h"
#include "vmelib.h"

#define PRINT_MAPPING	1

extern TMrbLofNamedX * gLofVMEProtos;		// list of prototypes
extern TMrbLofNamedX * gLofVMEModules;		// list of actual modules
extern TMrbLogger * gMrbLog;				// message logger

extern Bool_t gSignalTrap;

SrvVMEModule::SrvVMEModule(const Char_t * ModuleType,	const Char_t * ModuleDescr,
												UInt_t AddrMod,
												Int_t SegSize,
												Int_t NofChannels,
												Int_t Range) : TNamed(ModuleType, ModuleDescr) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			SrvVMEModule	SrvVMEModule.h
//! \brief			Create a prototype module
//! \details		Class def to define a module prototype
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "c2lynx.log");

	signal(SIGBUS, catchSignal);

	SrvVMEModule * m = (SrvVMEModule *) gLofVMEProtos->FindByName(ModuleType);
	if (m) {
		gMrbLog->Err() << "Prototype already defined - " << ModuleType << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		fProto = NULL;
		fAddrMod = AddrMod;
		fSegSize = SegSize;
		fNofChannels = NofChannels;
		fRange = Range;
		fBaseAddr = 0;
		fAddrLow = 0;
		fAddrHigh = 0;
		fMappedAddr = 0;
		gLofVMEProtos->AddNamedX(new TMrbNamedX(gLofVMEProtos->GetEntries(), ModuleType, ModuleDescr, this));
	}
}

SrvVMEModule::SrvVMEModule(const Char_t * ModuleName, const Char_t * ModuleType, UInt_t BaseAddr, Int_t NofChannels) :
																		TNamed(ModuleName, ModuleType) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			SrvVMEModule	SrvVMEModule.h
//! \brief			Create a real module
//! \details		Class def to define a real module
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "c2lynx.log");

	TMrbNamedX * m = gLofVMEProtos->FindByName(ModuleType);
	if (m == NULL) {
		gMrbLog->Err() << "[" << ModuleName << "] No such VME prototype - " << ModuleType << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		fProto = (SrvVMEModule *) m->GetAssignedObject();
		m = gLofVMEModules->FindByName(ModuleName);
		if (m) {
			gMrbLog->Err() << "Module already defined - " << ModuleName
					<< " (type=" << m->GetTitle()
					<< ", base=" << setbase(16) << ((SrvVMEModule *) m->GetAssignedObject())->GetBaseAddr() << ")"
					<< endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			Int_t nofChannels = fProto->GetNofChannels();
			if (NofChannels == 0) NofChannels = nofChannels;
			if (NofChannels <= 0 || NofChannels > nofChannels) {
				gMrbLog->Err() << "[" << ModuleName << "] Number of channels out of range - " << NofChannels
						<< " (should be in [1, " << nofChannels << "]" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else {
				fID = fProto->GetID();
				fAddrMod = fProto->GetAddrModifier();
				fSegSize = fProto->GetSegmentSize();
				fNofChannels = nofChannels;
				fNofChannelsUsed = NofChannels;
				fBaseAddr = BaseAddr;
				fAddrLow = 0;
				fAddrHigh = 0;
				fMappedAddr = 0;

				if (this->MapAddress() == NULL) this->MakeZombie();		// transform phys addr to vme addr
			}
		}
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Appends module to list
//////////////////////////////////////////////////////////////////////////////

void SrvVMEModule::Append() {

	gLofVMEModules->AddNamedX(new TMrbNamedX(gLofVMEModules->GetEntries(), this->GetName(), this->GetTitle(), this));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Checks if offset is within current window.<br>
//!                 If so returns immediately.<br>
//!                 If not unmaps module via 'return_controller' and maps it<br>
//!                 again via 'find_controller'<br>
//!                 Reason: Some modules use a huge addr space which fills<br>
//!                         the complete mapping table or even exceeds it.<br>
//!                         So there wouldn't be a chance to map several<br>
//!                         modules at the same time.
//! \param[in]		Offset			-- offset from base address
//! \retval 		MappedAddr		-- mapped address
//////////////////////////////////////////////////////////////////////////////

volatile Char_t * SrvVMEModule::MapAddress(UInt_t Offset) {

	struct pdparam_master s_param; 			// vme segment params

	s_param.iack = 1;
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					// forces static mapping!

	if (this->IsPrototype()) {
		gMrbLog->Err()	<< "[" << this->GetName() << "]: Can't map a PROTOTYPE module - ignored"<< endl;
		gMrbLog->Flush(this->ClassName(), "MapAddress");
		return(NULL);
	}

	Bool_t mapIt = kFALSE;
	if (fAddrHigh == 0) {
		mapIt = kTRUE;						// never done
	} else if (Offset < fAddrLow || Offset > fAddrHigh) {
#if (PRINT_MAPPING != 0)
		gMrbLog->Out()	<< "[Unmapping log addr=0x" << setbase(16) << fMappedAddr << ", size=0x" << fSegSize << "]" << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "MapAddress");
#endif
		UInt_t addr = return_controller(fMappedAddr, fSegSize);
		if (addr == 0xFFFFFFFF) {
			gMrbLog->Err()	<< "[" << this->GetName() << "]: Can't unmap log addr 0x"
							<< setbase(16) << fMappedAddr << " (seg size=0x" << fSegSize << ", addr mod=0x" << fAddrMod << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "MapAddress");
			return(NULL);
		}
		mapIt = kTRUE;
	}

	if (mapIt) {
		UInt_t low = (Offset / fSegSize) * fSegSize;
		UInt_t addr = find_controller(fBaseAddr + low, fSegSize, fAddrMod, 0, 0, &s_param);
		if (addr == 0xFFFFFFFF) {
#if (PRINT_MAPPING != 0)
			gMrbLog->Err()	<< "[Mapping offset=0x" << setbase(16) << Offset
							<< " (phys addr=" << (fBaseAddr + Offset) << ") low=0x" << low
							<< " high=0x" << (low + fSegSize - 1) << " to log addr=???" << "]"
							<< setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "MapAddress");
#endif
			gMrbLog->Err()	<< "[" << this->GetName() << "]: Can't map phys addr 0x"
							<< setbase(16) << (fBaseAddr + Offset) << " (size=0x" << fSegSize << ", addr mod=0x" << fAddrMod << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "MapAddress");
			return(NULL);
		}
		fMappedAddr = addr;
		fAddrLow = low;
		fAddrHigh = low + fSegSize - 1;
#if (PRINT_MAPPING != 0)
		gMrbLog->Out()	<< "[Mapping offset=0x" << setbase(16) << Offset
						<< " (phys addr=" << (fBaseAddr + Offset) << ") low=0x" << fAddrLow
						<< " high=0x" << fAddrHigh << " to log addr=0x" << addr << "]"
						<< setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "MapAddress");
#endif
	}
	return((volatile Char_t *) (fMappedAddr + (Offset - fAddrLow)));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Checks bus trap flag and gives error message
//! \param[in]		Module		-- module
//! \param[in]		Offset		-- offset
//! \param[in]		Method		-- calling method
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVMEModule::CheckBusTrap(SrvVMEModule * Module, UInt_t Offset, const Char_t * Method) {

	if (!gSignalTrap) return(kFALSE);

	gMrbLog->Err()	<< "[" << Module->GetName() << "]: Bus trap at phys addr 0x"
					<< setbase(16)
					<< Module->GetBaseAddr();
	if (Offset != 0) gMrbLog->Err() 	<< " + 0x" << Offset;
	gMrbLog->Err()	<< ", log addr 0x" << fMappedAddr;
	if (Offset != 0) gMrbLog->Err() 	<< " + 0x" << (Offset - fAddrLow);
	if (Method != NULL) gMrbLog->Err()	<< " (called by method \"" << Method << "()\")" << endl;
	gMrbLog->Flush(this->ClassName(), "CheckBusTrap");
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Prints definitions for prototypes and other modules
//////////////////////////////////////////////////////////////////////////////

void SrvVMEModule::Print() {

	if (this->IsPrototype()) {
		gMrbLog->Out()	<< "[Prototype] " << this->GetName() << ": " << this->GetTitle()
						<< ", addrMod=" << setbase(16) << this->GetAddrModifier() << setbase(10)
						<< ", segSize=" << setbase(16) << this->GetSegmentSize() << setbase(10)
						<< ", max chns=" << this->GetNofChannels()
						<< ", range=[0," << this->GetRange() << "]" << endl;
	} else {
		gMrbLog->Out()	<< "[New] " << this->GetName()
						<< ": type=" << this->GetTitle()
						<< setbase(16)
						<< ", base=" << this->GetBaseAddr()
						<< setbase(10) << ", chns used=" << this->GetNofChannelsUsed()
						<< " (out of " << this->GetNofChannels() << ")" << endl;
	}
	gMrbLog->Flush(this->ClassName(), "Print");
}

