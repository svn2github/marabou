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
//! $Revision: 1.1 $     
//! $Date: 2010-10-04 10:43:26 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrNamedX.h"
#include "LwrLofNamedX.h"

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

	public:

		//! Constructor: define a module prototype
		//! \param[in]			ModuleType	-- module type (i.e. "Sis3302")
		//! \param[in]			ModuleDescr	-- description
		//! \param[in]			AddrMod 	-- VME address modifier
		//! \param[in]			SegSize 	-- segment size in bytes
		//! \param[in]			NofChannels	-- number of channels
		//! \param[in]			Range		-- range in bits / resolution
		SrvVMEModule(const Char_t * ModuleType,	const Char_t * ModuleDescr,
												UInt_t AddrMod,
												Int_t SegSize,
												Int_t NofChannels,
												Int_t Range);

		//! Constructor: define a real module
		//! \param[in]			ModuleName	-- module name
		//! \param[in]			ModuleType	-- prototypee (i.e. "Sis3302")
		//! \param[in]			BaseAddr 	-- base (=physical) adress
		//! \param[in]			NofChannels	-- number of channels used
		SrvVMEModule(const Char_t * ModuleName,	const Char_t * ModuleType,
												UInt_t BaseAddr,
												Int_t NofChannels = 0);

		//! Destructor
		~SrvVMEModule() {};

		//! print module defs
		void Print();

		//! Get module's base (=physical) address
		//! \retval 	BaseAddr	-- base (=physical) address
		inline UInt_t GetBaseAddr() const { return(fBaseAddr); };

		//! Get	module's mapped (=vme) address
		//! \retval 	MappedAddr	-- mapped (=vme) address
		inline UInt_t GetMappedAddr() const { return(fMappedAddr); };

		//! Get lowest address mapped so far
		//! \retval 	AddrLow 	-- lower bound of mapping segment
		inline UInt_t GetAddrLow() const { return(fAddrLow); };

		//! Get highest address mapped so far
		//! \retval 	AddrHigh 	-- upper bound of mapping segment
		inline UInt_t GetAddrHigh() const { return(fAddrHigh); };

		//! Get segment size in bytes
		//! \retval 	SegSize 	-- segment size in bytes
		inline Int_t GetSegmentSize() const { return(fSegSize); };

		//! Get vme addr modifier
		//! \retval 	AddrMod 		-- vme address modifier
		inline Int_t GetAddrModifier() const { return(fAddrMod); };

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
		inline GetID() { return(fID); };

		//! Check if prototype or real module
		//! \return 	TRUE or FALSE
		inline Bool_t IsPrototype() { return(fProto == NULL); };

		//! Map addr bank
		volatile Char_t * MapAddress(UInt_t Offset = 0x0);

		//! Append module to list
		void Append();

		inline const Char_t * ClassName() const { return "SrvVMEModule"; };

	protected:

		//! Check if bustrap occurred during vme access
		Bool_t CheckBusTrap(SrvVMEModule * Module, UInt_t Offset = 0x0, const Char_t * Method = NULL);

		//! Set module id
		void SetID(Int_t ID) { fID = ID; };

	protected:

		SrvVMEModule * fProto;	 			//!< link to prototype module
		Int_t fID;							//!< module (=prototype) id
		UInt_t fBaseAddr;					//!< base address
		UInt_t fAddrLow; 					//!< current low limit
		UInt_t fAddrHigh; 					//!< current high limit
		UInt_t fMappedAddr; 				//!< mapped vme address
		UInt_t fAddrMod;					//!< address modifier
		Int_t fSegSize; 	 				//!< segment size
		Int_t fNofChannels; 				//!< number of channels
		Int_t fNofChannelsUsed; 			//!< number of channels used
		Int_t fRange;		 				//!< resolution
		TMrbLofNamedX * fLofFunctions;		//!< functions
};	

#endif
