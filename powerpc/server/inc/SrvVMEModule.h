#ifndef __SrvVMEModule_h__
#define __SrvVMEModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvVMEModule.h
//! \brief			MARaBOU to Lynx: VME modules
//! \details		Class definitions for M2L server:<br>
//! 				Module Vulom3 (GSI), mode "trigger box"
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2011-05-20 13:51:17 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrNamedX.h"
#include "LwrLofNamedX.h"

#include "mapping_database.h"

class SrvVMEModule : public TNamed {

	public:
		enum ESrvManufacturers	{	kManufactOther			=	0,
									kManufactCaen			=	BIT(1),
									kManufactSis			=	BIT(2),
									kManufactGsi			=	BIT(3)
								};

		enum ESrvModuleID		{	kModuleOther			=	0,
									kModuleCaen785 			=	kManufactCaen + 0x1,
									kModuleSis3302  		=	kManufactSis + 0x2,
									kModuleVulomTB  		=	kManufactGsi + 0x3
								};

		enum ESrvMapping	 	{	kVMEMappingUndef	=	0,			/* vme mapping */
									kVMEMappingDirect	=	BIT(0),
									kVMEMappingStatic	=	BIT(1),
									kVMEMappingDynamic	=	BIT(2),
									kVMEMappingHasBLT	=	BIT(10),
									kVMEMappingHasMBLT	=	BIT(11)
						};

		enum ESrvAddrMod		{	kAM_A32		=	0x09,
									kAM_A24		=	0x39,
									kAM_A16		=	0x29,
									kAM_BLT		=	0x0b,
									kAM_MBLT	=	0x08
						};

#ifdef CPU_TYPE_RIO2
		enum	ESrvMapAddr		{	kAddr_A32	=	0xE0000000,
									kAddr_A24	=	0xEE000000,
									kAddr_A16	=	0xEFFE0000
						};
		enum	ESrvMapSize		{	kSize_A32	=	0x0E000000,
									kSize_A24	=	0x01000000,
									kSize_A16	=	0x00010000
						};
#elif CPU_TYPE_RIO3
		enum	ESrvMapAddr		{	kAddr_A32	=	0x50000000,
									kAddr_A24	=	0x4F000000,
									kAddr_A16	=	0x4E000000,
									kAddr_BLT	=	0x50000000,
									kAddr_MBLT	=	0x60000000
						};
		enum	ESrvMapSize		{	kSize_A32	=	0x10000000,
									kSize_A24	=	0x01000000,
									kSize_A16	=	0x00010000,
									kSize_BLT	=	0x10000000,
									kSize_MBLT	=	0x10000000
						};
#else
		enum	ESrvMapAddr		{	kAddr_A32	=	0x50000000,
									kAddr_A24	=	0x4F000000,
									kAddr_A16	=	0x4E000000,
									kAddr_A32Direct	=	0xF0000000
						};
		enum	ESrvMapSize		{	kSize_A32	=	0x06000000,
									kSize_A24	=	0x01000000,
									kSize_A16	=	0x00010000
						};
#endif

	public:

		//! Constructor: define a module prototype
		//! \param[in]			ModuleType	-- module type (i.e. "Sis3302")
		//! \param[in]			ModuleDescr	-- description
		//! \param[in]			AddrMod 	-- VME address modifier
		//! \param[in]			SegSize 	-- segment size in bytes
		//! \param[in]			NofChannels	-- number of channels
		//! \param[in]			Range		-- range in bits / resolution
		SrvVMEModule(const Char_t * ModuleType,	const Char_t * ModuleDescr, UInt_t AddrMod, Int_t SegSize, Int_t NofChannels, Int_t Range);

		//! Constructor: define a real module
		//! \param[in]			ModuleName	-- module name
		//! \param[in]			ModuleType	-- prototypee (i.e. "Sis3302")
		//! \param[in]			PhysAddr 	-- physical adress
		//! \param[in]			NofChannels	-- number of channels used
		SrvVMEModule(const Char_t * ModuleName,	const Char_t * ModuleType, UInt_t PhysAddr, Int_t NofChannels = 0);

		//! Destructor
		~SrvVMEModule() {};

		//! print module defs
		void Print();

		//! Get module's base (=physical) address
		//! \retval 	PhysAddr	-- base (=physical) address
		inline UInt_t GetPhysAddr() const { return(fPhysAddrVME); };

		//! Get	module's mapped (=vme) address
		//! \retval 	BaseAddr	-- mapped (=vme) address
		inline UInt_t GetBaseAddr() const { return((UInt_t) fVmeBase); };

		//! Set segment size in bytes
		inline void SetSegmentSize(Int_t SegSize) { fSegSizeVME = SegSize; };

		//! Get segment size in bytes
		//! \retval 	SegSize 	-- segment size in bytes
		inline Int_t GetSegmentSize() const { return(fSegSizeVME); };

		//! Get vme addr modifier
		//! \retval 	AddrMod 		-- vme address modifier
		inline Int_t GetAddrModifier() const { return(fAddrModVME); };

		//! Get number of channels housed in this module
		//! \retval 	NofChannels 	-- number of channels
		inline Int_t GetNofChannels() const { return(fNofChannels); };

		//! Get number of channels used by program
		//! \retval 	NofChannels 	-- number of channels
		inline Int_t GetNofChannelsUsed() const { return(fNofChannelsUsed); };

		//! Get range (module's resolution)
		//! \retval 	Range 	-- resolution
		inline Int_t GetRange() const { return(fRange); };

		//! Get module's prototype
		//! \retval 	Proto	 	-- ptr to prototype definition
		inline SrvVMEModule * GetPrototype() const { return(fProto); };

		//! Get a list of functions available
		//! \retval 	LofFunctions	 	-- list of functions available
		inline TMrbLofNamedX * GetLofFunctions() { return(fLofFunctions); };

		//! Get module id
		//! \retval 	Id	 	-- module id
		inline Int_t GetID() { return(fID); };

		//! Check if prototype or real module
		//! \return 	TRUE or FALSE
		inline Bool_t IsPrototype() { return(fProto == NULL); };

		//! Map addr bank
		volatile Char_t * MapAddress(UInt_t Offset = 0x0, Int_t SegSize = 0);

		//! Append module to list
		void Append();

		inline const Char_t * ClassName() const { return "SrvVMEModule"; };

		//! Map modules addr space
		//! \param[in]			PhysAddr	-- phys addr
		//! \param[in]			Size		-- segment size
		//! \param[in]			AddrMod		-- addr modifier
		//! \param[in]			Mapping		-- mapping modes
		//! \return 	TRUE or FALSE
		Bool_t MapVME(UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, UInt_t Mapping);
		volatile Char_t * MapAdditionalVME(UInt_t PhysAddr, Int_t Size);

		//! Map modules BLT space
		//! \return 	TRUE or FALSE
		//! \param[in]			PhysAddr	-- phys addr
		//! \param[in]			Size		-- segment size
		//! \param[in]			AddrMod		-- addr modifier
		Bool_t MapBLT(UInt_t PhysAddr, Int_t Size, UInt_t AddrMod);

		//! Unmap VME and BLT segments
		//! \return 	TRUE or FALSE
		Bool_t UnmapVME();
		Bool_t UnmapBLT();

		//! Initialize BLT
		//! \return 	TRUE or FALSE
		Bool_t InitBLT();

		//! Define BLT attributes
		//! \return 	TRUE or FALSE
		Bool_t SetBLTMode(UInt_t VmeSize, UInt_t WordSize, Bool_t FifoMode);

		//! Calc phys addr from logical one
		//! \return 	PhysAddr
		Char_t * GetPhysAddr(Char_t * Addr, Int_t Size);

	protected:

		//! Check if bustrap occurred during vme access
		Bool_t CheckBusTrap(SrvVMEModule * Module, UInt_t Offset = 0x0, const Char_t * Method = NULL);

		//! Set module id
		void SetID(Int_t ID) { fID = ID; };

	protected:

		SrvVMEModule * fProto;	 			//!< link to prototype module
		Int_t fID;					//!< module (=prototype) id
		Int_t fNofChannels; 				//!< number of channels
		Int_t fNofChannelsUsed; 			//!< number of channels used
		Int_t fRange;		 			//!< resolution
		TMrbLofNamedX * fLofFunctions;			//!< functions

		UInt_t fMappingModes;				//!< mapping modes available
		UInt_t fMappingVME;				//!< used VME mapping
		UInt_t fAddrModVME;				//!< address modifier
		UInt_t fPhysAddrVME;				//!< phys addr given by module switches
		volatile Char_t * fVmeBase;			//!< mapped address, static or dynamic mapping
		Int_t fSegSizeVME;				//!< segment size
		Int_t fBusId;					//!< identifier from bus_open() call

		Bool_t fBltEnabled;				//!< TRUE if BLT is enabled
		UInt_t fMappingBLT;				//!< used BLT mapping
		UInt_t fAddrModBLT;				//!< address modifier
		UInt_t fPhysAddrBLT;				//!< phys addr given by module switches
		volatile Char_t * fBltBase;			//!< mapped address
		Int_t fSegSizeBLT;				//!< segment size
		Int_t fBltModeId;				//!< id from bma_create_mode()

		Int_t fNofMappings;				//!< number of mappings for this device

};	

#endif
