//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvVMEModule.cxx
//! \brief			MARaBOU to Lynx: VME modules_param
//! \details		Implements class methods to handle VME modules
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2011-05-20 12:21:03 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrLogger.h"
#include "SrvUtils.h"
#include "SrvVMEModule.h"
#include "SetColor.h"

typedef int intptr_t;

#include "signal.h"
#include <stdint.h>
#include <unistd.h>
#include <smem.h>
#ifdef CPU_TYPE_RIO4
#include <mem.h>
#endif

#include <ces/vmelib.h>
extern "C" {
#include <ces/bmalib.h>
#ifndef CPU_TYPE_RIO2
#include <ces/buslib.h>
#endif
#include <ces/uiolib.h>
}

#ifndef CPU_TYPE_RIO2
#include <ces/xvme.h>
#endif

#include "vmecontrol.h"

#define PRINT_MAPPING	1

extern "C" u_int xvme_map(u_int vaddr, int wsiz, int am, int adp);

Bool_t bltInit = kFALSE;

TMrbLofNamedX * gLofVMEProtos;		// list of prototypes
TMrbLofNamedX * gLofVMEModules;		// list of actual modules
extern TMrbLogger * gMrbLog;		// message logger

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
		fAddrModVME = AddrMod;
		fSegSizeVME = SegSize;
		fNofChannels = NofChannels;
		fRange = Range;
		gLofVMEProtos->AddNamedX(new TMrbNamedX(gLofVMEProtos->GetEntries(), ModuleType, ModuleDescr, this));
	}
}

SrvVMEModule::SrvVMEModule(const Char_t * ModuleName, const Char_t * ModuleType,
		UInt_t BaseAddr, Int_t SegSize, Int_t NofChannels, UInt_t Mapping) : TNamed(ModuleName, ModuleType) {
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
				fSegSizeVME = SegSize;
				fAddrModVME = fProto->GetAddrModifier();
				fMappingModes = Mapping;
				this->MapVME(BaseAddr, fSegSizeVME, fAddrModVME, fMappingModes);
				fID = fProto->GetID();
				fNofChannels = nofChannels;
				fNofChannelsUsed = NofChannels;
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

volatile Char_t * SrvVMEModule::MapAddress(UInt_t Offset, Int_t SegSize) {

	return((volatile Char_t *) (fVmeBase + Offset));
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
					<< Module->GetPhysAddr();
	if (Offset != 0) gMrbLog->Err() 	<< " + 0x" << Offset;
	gMrbLog->Err()	<< ", log addr 0x" << (UInt_t) Module->GetBaseAddr();
	if (Offset != 0) gMrbLog->Err() 	<< " + 0x" << Offset;
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapVME
// Purpose:        Map VME address
// Arguments:      UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
//                 UInt_t AddrMod           -- address modifier
//                 UInt_t Mapping           -- available mapping modes
// Results:        s_mapDescr * Descr       -- pointer to database
// Description:    Performs VME mapping, either statically or dynamically
// Keywords:       
////////////////////////////////////////////////////////////////////////////'/

Bool_t SrvVMEModule::MapVME(UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, UInt_t Mapping) {

	struct pdparam_master s_param; 		/* vme segment params */
	UInt_t staticBase;
	UInt_t dynamicAddr;
	
	if (Size == 0) Size = 4096;

	fMappingModes = Mapping;
	fMappingVME = kVMEMappingUndef;
	fMappingBLT = kVMEMappingUndef;
	
#ifdef CPU_TYPE_RIO4
	if (Mapping & kVMEMappingDirect && AddrMod == kAM_A32) {	/* direct mapping for RIO4/A32 only */
		if (PhysAddr > 0x0FFFFFFF) {
			gMrbLog->Err() << "[" << this->GetName() << "] Direct mapping not possible - " << setbase(16) << PhysAddr << endl;
			gMrbLog->Flush(this->ClassName(), "MapVME");
			return kFALSE;
		}
		fVmeBase = (volatile Char_t *) (PhysAddr | kAddr_A32Direct);
		fMappingVME = kVMEMappingDirect;
		gMrbLog->Out() << "[" << this->GetName() << "] Direct mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fVmeBase << endl;
		gMrbLog->Flush(this->ClassName(), "MapVME");
	}
#endif
	if (fMappingVME == kVMEMappingUndef) {
		if (Mapping & kVMEMappingStatic) {
			if (PhysAddr > 0x0FFFFFFF) {
				gMrbLog->Err() << "[" << this->GetName() << "] Static mapping not possible - " << setbase(16) << PhysAddr << endl;
				gMrbLog->Flush(this->ClassName(), "MapVME");
				return kFALSE;
			}
			switch (AddrMod) {
				case kAM_A32:
					staticBase = kAddr_A32;
					break;
				case kAM_A24:
					staticBase = kAddr_A24;
					if (PhysAddr > 0x00FFFFFF) {
						gMrbLog->Err() << "[" << this->GetName() << "] Not a A24 addr - " << setbase(16) << PhysAddr << endl;
						gMrbLog->Flush(this->ClassName(), "MapVME");
						return kFALSE;
					}
				case kAM_A16:
					staticBase = kAddr_A16;
					if (PhysAddr > 0x0000FFFF) {
						gMrbLog->Err() << "[" << this->GetName() << "] Not a A16 addr - " << setbase(16) << PhysAddr << endl;
						gMrbLog->Flush(this->ClassName(), "MapVME");
						return kFALSE;
					}
				default:
					gMrbLog->Err() << "[" << this->GetName() << "] Illegal addr modifier - " << setbase(16) << AddrMod << endl;
					gMrbLog->Flush(this->ClassName(), "MapVME");
					return kFALSE;
			}

			fVmeBase = smem_create((Char_t *) this->GetName(), (Char_t *) (staticBase | PhysAddr), Size, SM_READ|SM_WRITE);
			if (fVmeBase == NULL) {
				gMrbLog->Err() << "[" << this->GetName() << "] Creating shared segment failed" << endl;
				gMrbLog->Flush(this->ClassName(), "MapVME");
				return kFALSE;
			}
			fMappingVME = kVMEMappingStatic;
			gMrbLog->Out() << "[" << this->GetName() << "] Static mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fVmeBase << endl;
			gMrbLog->Flush(this->ClassName(), "MapVME");
		} else if (Mapping & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO4
			fBusId = bus_open("xvme_mas");
			dynamicAddr = bus_map(fBusId, PhysAddr, 0, Size, 0xa0, 0);
#else
 			s_param.iack = 1;
 			s_param.rdpref = 0;
 			s_param.wrpost = 0;
 			s_param.swap = SINGLE_AUTO_SWAP;
 			s_param.dum[0] = 0;
 			dynamicAddr = find_controller(PhysAddr, Size, AddrMod, 0, 0, &s_param);
#endif
			if (dynamicAddr == 0xFFFFFFFF) {
				gMrbLog->Err() << "[" << this->GetName() << "] Dynamic mapping failed" << endl;
				gMrbLog->Flush(this->ClassName());
				return kFALSE;
			} else {
				fVmeBase = (volatile Char_t *) dynamicAddr;
			}
			fMappingVME = kVMEMappingDynamic;
			gMrbLog->Out() << "[" << this->GetName() << "] Dynamic mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fVmeBase << endl;
			gMrbLog->Flush(this->ClassName(), "MapVME");
		} else {
			gMrbLog->Err() << "[" << this->GetName() << "] Illegal mapping mode - " << setbase(16) << Mapping << endl;
			gMrbLog->Flush(this->ClassName(), "MapVME");
			return kFALSE;
		}
	}

	fAddrModVME = AddrMod;
	fPhysAddrVME = PhysAddr;
	fSegSizeVME = Size;
	fNofMappings++;
	return kTRUE;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapAdditionalVME
// Purpose:        Add another VME page
// Arguments:      UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
// Results:        Char_t * MappedAddr      -- mapped address
// Description:    Maps an additional address.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

volatile Char_t * SrvVMEModule::MapAdditionalVME(UInt_t PhysAddr, Int_t Size) {

	struct pdparam_master s_param; 		/* vme segment params */
	volatile Char_t * mappedAddr;
	UInt_t staticBase;
	UInt_t dynamicAddr;
	UInt_t cpuBaseAddr;

	TString segName;

	if (Size == 0) Size = fSegSizeVME;

	switch (fMappingVME) {
#ifdef CPU_TYPE_RIO4
		case kVMEMappingDirect:
			if (PhysAddr > 0x0FFFFFFF) {
				gMrbLog->Err() << "[" << this->GetName() << "] Direct mapping not possible - " << setbase(16) << PhysAddr << endl;
				gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
				return kFALSE;
			}
			fNofMappings++;
			gMrbLog->Out() << "[" << this->GetName() << "] Direct mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (PhysAddr | kAddr_A32Direct) << endl;
			gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
			return (volatile Char_t *) (PhysAddr | kAddr_A32Direct);
#endif
		case kVMEMappingStatic:
			if (PhysAddr > 0x0FFFFFFF) {
				gMrbLog->Err() << "[" << this->GetName() << "] Static mapping not possible - " << setbase(16) << PhysAddr << endl;
				gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
				return kFALSE;
			}
			switch (fAddrModVME) {
				case kAM_A32:
					staticBase = kAddr_A32;
					break;
				case kAM_A24:
					staticBase = kAddr_A24;
					if (PhysAddr > 0x00FFFFFF) {
						gMrbLog->Err() << "[" << this->GetName() << "] Not a A24 addr - " << setbase(16) << PhysAddr << endl;
						gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
						return NULL;
					}
				case kAM_A16:
					staticBase = kAddr_A16;
					if (PhysAddr > 0x0000FFFF) {
						gMrbLog->Err() << "[" << this->GetName() << "] Not a A16 addr - " << setbase(16) << PhysAddr << endl;
						gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
						return NULL;
					}
			}
			segName = Form("%s_%d", this->GetName(), fNofMappings + 1);
			mappedAddr = smem_create((Char_t *) segName.Data(), (Char_t *) (staticBase | PhysAddr), Size, SM_READ|SM_WRITE);
			if (mappedAddr == NULL) {
				gMrbLog->Err() << "[" << this->GetName() << "] Creating shared segment " << segName.Data() << " failed" << endl;
				gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
				return NULL;
			}
			fNofMappings++;
			gMrbLog->Out() << "[" << this->GetName() << "] Static mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) mappedAddr << endl;
			gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
			return mappedAddr;

		case kVMEMappingDynamic:
#ifdef CPU_TYPE_RIO4
			dynamicAddr = bus_map(fBusId, PhysAddr, 0, Size, 0xa0, 0);
#else
 			s_param.iack = 1;
 			s_param.rdpref = 0;
 			s_param.wrpost = 0;
 			s_param.swap = SINGLE_AUTO_SWAP;
 			s_param.dum[0] = 0;
 			dynamicAddr = find_controller(PhysAddr, Size, fAddrModVME, 0, 0, &s_param);
#endif
			if (dynamicAddr == 0xFFFFFFFF) {
				gMrbLog->Err() << "[" << this->GetName() << "] Dynamic mapping failed" << endl;
				gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
				return NULL;
			}
			fNofMappings++;
			gMrbLog->Out() << "[" << this->GetName() << "] Dynamic mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << dynamicAddr << endl;
			gMrbLog->Flush(this->ClassName(), "MapAdditionalVME");
			return (volatile Char_t *) dynamicAddr;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapBLT
// Purpose:        Map BLT address
// Arguments:      UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
//                 UInt_t AddrMod           -- address modifier
// Results:        TRUE/FALSE
// Description:    Performs mapping to be used with block xfer
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

Bool_t SrvVMEModule::MapBLT(UInt_t PhysAddr, Int_t Size, UInt_t AddrMod) {

	struct pdparam_master s_param; 		/* vme segment params */
	UInt_t staticBase;
	UInt_t dynamicAddr;

	TString bltName;

	fMappingBLT = kVMEMappingUndef;

#ifdef CPU_TYPE_RIO3
	if (fMappingModes & kVMEMappingStatic) {
		switch (AddrMod) {
			case kAM_BLT:
				staticBase = kAddr_BLT;
				break;
			case kAM_MBLT:
				staticBase = kAddr_MBLT;
			default:
				gMrbLog->Err() << "[" << this->GetName() << "] Illegal addr modifier - " << setbase(16) << AddrMod << endl;
				gMrbLog->Flush(this->ClassName(), "MapBLT");
				return kFALSE;
		}

		bltName = Form("%s_blt", this->GetName());
		fBltBase = smem_create(bltName, (Char_t *) (staticBase | PhysAddr), Size, SM_READ);
		if (fBltBase == NULL) {
			gMrbLog->Err() << "[" << this->GetName() << "] Creating shared segment " << bltName.Data() << " failed" << endl;
			gMrbLog->Flush(this->ClassName(), "MapBLT");
			return kFALSE;
		}
		gMrbLog->Out() << "[" << this->GetName() << "] Static BLT mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fBltBase << endl;
		gMrbLog->Flush(this->ClassName(), "MapBLT");
		fMappingBLT = kVMEMappingStatic;
	}
#endif
#ifdef CPU_TYPE_RIO2
	if (fMappingModes & kVMEMappingDynamic) {
 		s_param.iack = 1;
 		s_param.rdpref = 0;
 		s_param.wrpost = 0;
 		s_param.swap = SINGLE_AUTO_SWAP;
 		s_param.dum[0] = 0;
 		dynamicAddr = find_controller(PhysAddr, Size, AddrMod, 0, 0, &s_param);
		if (dynamicAddr == 0xFFFFFFFF) {
			gMrbLog->Err() << "[" << this->GetName() << "] Dynamic mapping failed" << endl;
			gMrbLog->Flush(this->ClassName(), "MapBLT");
			fBltBase = NULL;
		} else {
			fBltBase = (volatile Char_t *) dynamicAddr;
			fMappingBLT = kVMEMappingStatic;
			gMrbLog->Out() << "[" << this->GetName() << "] Dynamic BLT mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fBltBase << endl;
			gMrbLog->Flush(this->ClassName(), "MapBLT");
		}
	}
#else
	if (fMappingBLT == kVMEMappingUndef) {
		if (fMappingModes & kVMEMappingDynamic) {
			dynamicAddr = xvme_map(PhysAddr, Size, AddrMod, 0);
			if (dynamicAddr == -1) {
				gMrbLog->Err() << "[" << this->GetName() << "] Can't map XVME page" << endl;
				gMrbLog->Flush(this->ClassName(), "MapBLT");
				fBltBase = NULL;
			} else {
				fBltBase = (volatile Char_t *) dynamicAddr;
				gMrbLog->Out() << "[" << this->GetName() << "] Dynamic BLT mapping of phys addr 0x" << setbase(16) << PhysAddr << " to log addr 0x" << (UInt_t) fBltBase << endl;
				gMrbLog->Flush(this->ClassName(), "MapBLT");
			}
		}
	}
#endif

	if (bma_create_mode(&fBltModeId) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't create BLT mode struct" << endl;
		gMrbLog->Flush(this->ClassName(), "MapBLT");
	}

	fAddrModBLT = AddrMod;
	fPhysAddrBLT = PhysAddr;
	fSegSizeBLT = Size;

	if (fBltBase == NULL) {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't map BLT addr - phys addr " << setbase(16) << PhysAddr << ", addrMod=" << setbase(16) << AddrMod << endl;
		gMrbLog->Flush(this->ClassName(), "MapBLT");
		return kFALSE;
	}
	return kTRUE;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           setBLTMode
// Purpose:        Set BLT mode
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
//                 UInt_t VmeSize           -- VME size
//                 UInt_t WordSize          -- word size
//                 UInt_t AddrMod           -- address modifier
//                 Bool_t FifoMode          -- TRUE if AutoIncr
// Results:        ---
// Description:    Fills BLT mode struct.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

Bool_t SrvVMEModule::SetBLTMode(UInt_t VmeSize, UInt_t WordSize, Bool_t FifoMode) {

	Int_t sts;

	if ((sts = bma_set_mode(fBltModeId, BMA_M_VMESize, VmeSize)) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Error while setting BLT mode (VMESize = " << VmeSize << ") - error code " << sts << endl;
		gMrbLog->Flush(this->ClassName(), "SetBLTMode");
	}
	if ((sts = bma_set_mode(fBltModeId, BMA_M_WordSize, WordSize)) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Error while setting BLT mode (WordSize = " << WordSize << ") - error code " << sts << endl;
		gMrbLog->Flush(this->ClassName(), "SetBLTMode");
	}
	if ((sts = bma_set_mode(fBltModeId, BMA_M_AmCode, fAddrModBLT)) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Error while setting BLT mode (AmCode = " << setbase(16) << fAddrModBLT << ") - error code " << sts << endl;
		gMrbLog->Flush(this->ClassName(), "SetBLTMode");
	}
#ifndef CPU_TYPE_RIO2
	if ((sts = bma_set_mode(fBltModeId, BMA_M_VMEAdrInc, FifoMode ? BMA_M_VaiFifo : BMA_M_VaiNormal)) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Error while setting BLT mode (FifoMode = " << FifoMode << ") - error code " << sts << endl;
		gMrbLog->Flush(this->ClassName(), "SetBLTMode");
	}
#endif
	return kTRUE;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           initBLT
// Purpose:        Initialize block xfer
// Arguments:      ---
// Results:        TRUE/FALSE
// Description:    Calls bma_open() etc
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

bool_t SrvVMEModule::InitBLT() {

	Int_t bmaError;

	if (bltInit) return kTRUE;

	bmaError = bma_open();
	if (bmaError != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't initialize BLT - bma_open() failed" << endl;
		gMrbLog->Flush(this->ClassName(), "InitBLT");
		bma_close();
		return kFALSE;
	}
	gMrbLog->Out() << "[" << this->GetName() << "] Block xfer enabled" << endl;
	gMrbLog->Flush(this->ClassName(), "InitBLT");
	return kTRUE;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           unmapVME
// Purpose:        Unmap a mapped segment
// Arguments:      ---
// Results:        TRUE/FALSE
// Description:    Removes a mapped segment, frees memory.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

Bool_t SrvVMEModule::UnmapVME() {

	UInt_t addr;

	if (fMappingModes & kVMEMappingStatic) {
		smem_create((Char_t *) "", (Char_t *) fVmeBase, 0, SM_DETACH);
		smem_remove((Char_t *) this->GetName());
		return kTRUE;
	} else if (fMappingModes & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO2
		addr = return_controller(fVmeBase, fSegSizeVME);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO3
		addr = return_controller(fVmeBase, fSegSizeVME);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO4
		bus_unmap(fBusId, (UInt_t) fVmeBase);
#endif
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           unmapBLT
// Purpose:        Unmap a BLT segment
// Arguments:      ---
// Results:        TRUE/FALSE
// Description:    Removes a BMT segment, frees memory.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

Bool_t SrvVMEModule::UnmapBLT() {

	TString bltName;
	Bool_t sts;
	UInt_t addr;

	if (fBltBase == NULL) return kTRUE;

	if (fMappingModes & kVMEMappingStatic) {
		smem_create((Char_t *) "", (Char_t *) fBltBase, 0, SM_DETACH);
		bltName = Form("%s_blt", this->GetName());
		smem_remove((Char_t *) bltName.Data());
		return kTRUE;
	} else if (fMappingModes & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO2
		addr = return_controller(fBltBase, fSegSizeBLT);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO3
		addr = xvme_rel(fBltBase, fSegSizeBLT);
		return (addr != 0xFFFFFFFF);
#endif
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           getPhysAddr
// Purpose:        Get pysical address from virtual one
// Arguments:      UInt_t Addr              -- virtual address
//                 Int_t Size               -- size in bytes
// Results:        Int_t Offset             -- offset
// Description:    Converts virtual to physical address.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

Char_t * SrvVMEModule::GetPhysAddr(Char_t * Addr, Int_t Size) {

	struct dmachain chains[10];

	static Int_t addrOffset = 0;
	Int_t error;

	if (vmtopm(getpid(), chains, Addr, Size) == -1) {
		gMrbLog->Err() << "[" << this->GetName() << "] vmtopm call failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetPhysAddr");
		return NULL;
	}
	return (Char_t *) chains[0].address;
}

	
