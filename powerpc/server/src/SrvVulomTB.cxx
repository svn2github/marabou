//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvVulomTB.cxx
//! \brief			MARaBOU to Lynx: VME modules
//! \details		Implements class methods for a Vulom3 module (trigger box)
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $
//! $Date: 2010-10-04 11:24:38 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "SrvUtils.h"
#include "SrvVulomTB.h"
#include "SrvVulom_Layout.h"
#include "LwrLogger.h"
#include "SetColor.h"

extern TMrbLofNamedX * gLofVMEProtos;		//!< list of prototypes
extern TMrbLofNamedX * gLofVMEModules;		//!< list of actual modules

extern TMrbLogger * gMrbLog;				//!< message logger

extern Bool_t gSignalTrap;					//!< flag indicating bus error

SrvVulomTB::SrvVulomTB() : SrvVMEModule(	"VulomTB",						//!< prototype
											"VULOM (GSI) / trigger box",	//!< description
											0x09,							//!< address modifier: A32
											0x01000000,  					//!< segment size
											3 * kVulomTBNofChannels,		//!< 16 channels (3 sets)
											1 << 32) {						//!< range
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			SrvVulomTB	SrvVulomTB.h
//! \brief			Create a VulomTB prototype
//! \details		Prototype VulomTB
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "c2lynx.log");
	this->SetID(SrvVMEModule::kModuleVulomTB);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Access one of the registers to be sure the module is present
//! \param[in]		Module  -- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::TryAccess(SrvVMEModule * Module) {

	Int_t boardId, majorVersion, minorVersion;
	if (!this->GetModuleInfo(Module, boardId, majorVersion, minorVersion, kTRUE)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Execute function
//! \param[in]		Module		-- module address
//! \param[in]		Function	-- function
//! \param[in]		Data		-- data
//! \retval 		Hdr 		-- ptr to output message
//////////////////////////////////////////////////////////////////////////////

M2L_MsgHdr * SrvVulomTB::Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data) {

	Int_t scalerNo;
	Int_t channelNo;
	TArrayI data;
	Int_t d;

	this->SetupFunction(Data, data, scalerNo, channelNo);

	switch (Function->GetIndex()) {
		case kM2L_FCT_GET_MODULE_INFO:
			{
				Int_t BoardId;
				Int_t MajorVersion;
				Int_t MinorVersion;
				if (!this->GetModuleInfo(Module, BoardId, MajorVersion, MinorVersion, kFALSE)) return(NULL);
				M2L_VME_Return_Module_Info * m = (M2L_VME_Return_Module_Info *) allocMessage(sizeof(M2L_VME_Return_Module_Info), 1, kM2L_MESS_VME_EXEC_FUNCTION);
				m->fBoardId = swapIt(BoardId);
				m->fSerial = -1;
				m->fMajorVersion = swapIt(MajorVersion);
				m->fMinorVersion = swapIt(MinorVersion);
				return((M2L_MsgHdr *) m);
			}
		case kM2L_FCT_VULOM_TB_READ_SCALER:
			{
				if (!this->ReadScaler(Module, data, scalerNo)) return(NULL);
				break;
			}
		case kM2L_FCT_VULOM_TB_READ_CHANNEL:
			{
				if (!this->ReadChannel(Module, data, channelNo)) return(NULL);
				break;
			}
		case kM2L_FCT_VULOM_TB_ENABLE_CHANNEL:
			{
				if (!this->EnableChannel(Module, channelNo)) return(NULL);
				break;
			}
		case kM2L_FCT_VULOM_TB_DISABLE_CHANNEL:
			{
				if (!this->DisableChannel(Module, channelNo)) return(NULL);
				break;
			}
		case kM2L_FCT_VULOM_TB_SET_ENABLE_MASK:
			{
				d = data[0] & 0xFFFF;
				if (!this->SetEnableMask(Module, d)) return(NULL);
				data.Set(1); data[0] = d & 0xFFFF;
				break;
			}
		case kM2L_FCT_VULOM_TB_GET_ENABLE_MASK:
			{
				if (!this->GetEnableMask(Module, d)) return(NULL);
				data.Set(1); data[0] = d & 0xFFFF;
				break;
			}
		case kM2L_FCT_VULOM_TB_SET_SCALE_DOWN:
			{
				d = data[0];
				if (!this->SetScaleDown(Module, d, channelNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_VULOM_TB_GET_SCALE_DOWN:
			{
				if (channelNo == kVulomTBAllChannels) {
				  if (!this->ReadScaleDown(Module, data)) return(NULL);
				} else {
				  if (!this->GetScaleDown(Module, d, channelNo)) return(NULL);
				  data.Set(1); data[0] = d;
				}
				break;
			}
		case kM2L_FCT_VULOM_TB_CLEAR_SCALER:
			{
				if (!this->ClearScaler(Module, channelNo)) return(NULL);
				break;
			}
		default:
			{
				gMrbLog->Err()	<< "[" << Module->GetName() << "]: Function not implemented - "
								<< Function->GetName() << " (" << setbase(16) << Function->GetIndex() << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "Dispatch");
				return(NULL);
			}
	}
	return(this->FinishFunction(data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Prepares an 'exec function' call
//! \param[in]		Data		-- incoming datahold data
//! \param[out]		Array		-- array to hold data
//! \param[in]		ScalerNo	-- scaler
//! \param[in]		ChannelNo	-- channel
//////////////////////////////////////////////////////////////////////////////

void SrvVulomTB::SetupFunction(M2L_MsgData * Data, TArrayI & Array, Int_t & ScalerNo, Int_t & ChannelNo) {

	ScalerNo = swapIt(Data->fData[0]);
	ChannelNo = swapIt(Data->fData[0]);
	Int_t wc = swapIt(Data->fWc);
	Int_t * dp = &Data->fData[1];
	Array.Reset();
	Array.Set(wc);
	for (Int_t i = 0; i < wc; i++) Array[i] = swapIt(*dp++);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Finishes 'exec function' call
//! \param[out]		Array	-- array to hold data
//! \retval 		Msg 	-- ptr to message (header)
//////////////////////////////////////////////////////////////////////////////

M2L_MsgHdr * SrvVulomTB::FinishFunction(TArrayI & Array) {

	Int_t wc = Array.GetSize();
	M2L_VME_Return_Results * m = (M2L_VME_Return_Results *) allocMessage(sizeof(M2L_VME_Return_Results), wc, kM2L_MESS_VME_EXEC_FUNCTION);
	Int_t * dp = m->fData.fData;
	for (Int_t i = 0; i < wc; i++) *dp++ = swapIt(Array[i]);
	m->fData.fWc = swapIt(wc);
	return((M2L_MsgHdr *) m);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details 		Reads (and outputs) module info
//! \param[in]		Module				-- module address
//! \param[out]		BoardId 			-- board id (n/a)
//! \param[out]		MajorVersion		-- firmware (major) version (n/a)
//! \param[out]		MinorVersion		-- firmware (minor) version (n/a)
//! \param[in]		PrintFlag			-- ouput to gMrbLog if kTRUE
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion, Bool_t PrintFlag) {

	volatile ULong_t * enaAddr = (volatile ULong_t *) Module->MapAddress(VULOM_TBOX_ENA);
	if (enaAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	ULong_t enaPattern = *enaAddr;
	if (this->CheckBusTrap(Module, VULOM_TBOX_ENA, "GetModuleInfo")) return(kFALSE);

	BoardId = 0;			// vulom doesn't provide any version info
	MajorVersion = 0;
	MinorVersion = 0;
	if (PrintFlag) {
		gMrbLog->Out()	<< setbase(16)
						<< "TrigBox info: addr (phys) 0x" << Module->GetBaseAddr()
						<< " (log) 0x" << Module->GetMappedAddr()
						<< " mod 0x" << Module->GetAddrModifier()
						<< " type VULOM \"t5\"" << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleInfo");
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads scaler values: scaler #N (N=0..2) for ALL channels
//! \param[in]		Module			-- module address
//! \param[out]		ScalerValues	-- where to store scaler values
//! \param[in]		ScalerNo		-- scaler number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::ReadScaler(SrvVMEModule * Module, TArrayI & ScalerValues, Int_t ScalerNo) {

	if (ScalerNo < 0 || ScalerNo >= kVulomTBNofScalers) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Scaler number out of range - "
						<< ScalerNo << " (should be in [0," << (kVulomTBNofScalers - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadScaler");
		return(kFALSE);
	}

	UInt_t offs = VULOM_TBOX_SCALER1 + ScalerNo * kVulomTBNofChannels * sizeof(Int_t) ;
	volatile Int_t * scaData = (volatile Int_t *) Module->MapAddress(offs);

	gSignalTrap = kFALSE;
	ScalerValues.Set(kVulomTBNofChannels);
	for (Int_t chn = 0; chn < kVulomTBNofChannels; chn++) {
		ScalerValues[chn] = *scaData++;
		if (this->CheckBusTrap(Module, offs, "ReadScaler")) return(kFALSE);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads scaler values: scalers 0-2 for a given channel
//! \param[in]		Module			-- module address
//! \param[out]		ScalerValues 	-- scaler values
//! \param[in]		ChannelNo		-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::ReadChannel(SrvVMEModule * Module, TArrayI & ScalerValues, Int_t ChannelNo) {

	if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
						<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadChannel");
		return(kFALSE);
	}

	gSignalTrap = kFALSE;
	ScalerValues.Set(kVulomTBNofScalers);
	Int_t scalOffs = VULOM_TBOX_SCALER1 + ChannelNo * sizeof(Int_t);
	volatile Int_t * scaData = (volatile Int_t *) Module->MapAddress(scalOffs);
	for (Int_t sca = 0; sca < kVulomTBNofScalers; sca++) {
		ScalerValues[sca] = *scaData;
		if (this->CheckBusTrap(Module, scalOffs, "ReadChannel")) return(kFALSE);
		scaData += kVulomTBNofChannels * sizeof(Int_t);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets enable bit for a single channel
//! \param[in]		Module			-- module address
//! \param[out]		ChannelNo		-- channel number
//! \return 		TRUE or FALSE
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::EnableChannel(SrvVMEModule * Module, Int_t ChannelNo) {

	if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
						<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "EnableChannel");
		return(kFALSE);
	}

	volatile Int_t * enable = (volatile Int_t *) Module->MapAddress(VULOM_TBOX_ENA);

	gSignalTrap = kFALSE;
	UInt_t bits = *enable & 0xFFFF;
	if (this->CheckBusTrap(Module, VULOM_TBOX_ENA, "EnableChannel")) return(kFALSE);

	bits |= (0x1 << ChannelNo);

	gSignalTrap = kFALSE;
	*enable = bits;
	return(!this->CheckBusTrap(Module, VULOM_TBOX_ENA, "EnableChannel"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Clears enable bit for a single channel
//! \param[in]		Module			-- module address
//! \param[out] 	ChannelNo		-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::DisableChannel(SrvVMEModule * Module, Int_t ChannelNo) {

	if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
						<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "DisableChannel");
		return(kFALSE);
	}

	volatile Int_t * enable = (volatile Int_t *) Module->MapAddress(VULOM_TBOX_ENA);

	gSignalTrap = kFALSE;
	UInt_t bits = *enable & 0xFFFF;
	if (this->CheckBusTrap(Module, VULOM_TBOX_ENA, "DisableChannel")) return(kFALSE);

	bits &= ~(0x1 << ChannelNo);

	gSignalTrap = kFALSE;
	*enable = bits;
	return(!this->CheckBusTrap(Module, VULOM_TBOX_ENA, "DisableChannel"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets enable bits
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bit pattern
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::SetEnableMask(SrvVMEModule * Module, Int_t & Bits) {

	volatile Int_t * enable = (volatile Int_t *) Module->MapAddress(VULOM_TBOX_ENA);

	gSignalTrap = kFALSE;
	Bits &= 0xFFFF;
	*enable = Bits;
	return(!this->CheckBusTrap(Module, VULOM_TBOX_ENA, "SetEnableMask"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads enable bits
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bit pattern
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::GetEnableMask(SrvVMEModule * Module, Int_t & Bits) {

	volatile Int_t * enable = (volatile Int_t *) Module->MapAddress(VULOM_TBOX_ENA);

	gSignalTrap = kFALSE;
	Bits = *enable & 0xFFFF;
	return(!this->CheckBusTrap(Module, VULOM_TBOX_ENA, "GetEnableMask"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets scale down for given channel
//! \param[in]		Module		-- module address
//! \param[in]		ScaleDown	-- scale down value
//! \param[in]		ChannelNo	-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::SetScaleDown(SrvVMEModule * Module, Int_t & ScaleDown, Int_t ChannelNo) {

	if (ChannelNo == kVulomTBAllChannels) {
		for (Int_t ch = 0; ch < kVulomTBNofChannels; ch++) this->SetScaleDown(Module, ScaleDown, ch);
		return(kTRUE);
	}

	if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
						<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetScaleDown");
		return(kFALSE);
	}

	if (ScaleDown < 0 || ScaleDown > 0xF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Scale down out of range - "
						<< ScaleDown << " (should be in [0, 15])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetScaleDown");
		return(kFALSE);
	}

	UInt_t offs;
	Int_t chn;
	if (ChannelNo < 8) {
		offs = VULOM_TBOX_SCALL;
		chn = ChannelNo;
	} else {
		offs = VULOM_TBOX_SCALH;
		chn = ChannelNo - 8;
	}

	volatile Int_t * scdAddr = (volatile Int_t *) Module->MapAddress(offs);

	gSignalTrap = kFALSE;
	Int_t scdBits = *scdAddr;
	if (this->CheckBusTrap(Module, offs, "SetScaleDown")) return(kFALSE);

	scdBits &= ~(0xF << (chn * 4));
	scdBits |= ScaleDown << (chn * 4);

	gSignalTrap = kFALSE;
	*scdAddr = scdBits;
	return(!this->CheckBusTrap(Module, offs, "SetScaleDown"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads scale down values of all channels
//! \param[in]		Module		-- module address
//! \param[out]		ScaleDown	-- array to hold scale down values
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::ReadScaleDown(SrvVMEModule * Module, TArrayI & ScaleDown) {
	ScaleDown.Set(kVulomTBNofChannels);
	for (Int_t ch = 0; ch < kVulomTBNofChannels; ch++) {
		Int_t scd = 0;
		this->GetScaleDown(Module, scd, ch);
		ScaleDown[ch] = scd;
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads scale down of given channel
//! \param[in]		Module		-- module address
//! \param[out]		ScaleDown	-- scale down value
//! \param[in]		ChannelNo	-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::GetScaleDown(SrvVMEModule * Module, Int_t & ScaleDown, Int_t ChannelNo) {

	if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
						<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetScaleDown");
		return(kFALSE);
	}

	UInt_t offs;
	Int_t chn;
	if (ChannelNo < 8) {
		offs = VULOM_TBOX_SCALL;
		chn = ChannelNo;
	} else {
		offs = VULOM_TBOX_SCALH;
		chn = ChannelNo - 8;
	}

	volatile Int_t * scdAddr = (volatile Int_t *) Module->MapAddress(offs);

	gSignalTrap = kFALSE;
	Int_t scdBits = *scdAddr;
	ScaleDown = (scdBits >> (chn * 4)) & 0xF;
	if (this->CheckBusTrap(Module, offs, "GetScaleDown")) return(kFALSE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Clear scaler
//! \param[in]		Module		-- module address
//! \param[out]		ChannelNo	-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvVulomTB::ClearScaler(SrvVMEModule * Module, Int_t ChannelNo) {
	Int_t enable;

	if (!this->GetEnableMask(Module, enable)) return(kFALSE);

	if (ChannelNo == kVulomTBAllChannels) {
		Int_t clear = 0;
		this->SetEnableMask(Module, clear);
	} else {
		if (ChannelNo < 0 || ChannelNo >= kVulomTBNofChannels) {
			gMrbLog->Err()	<< "[" << Module->GetName() << "]: Channel number out of range - "
							<< ChannelNo << " (should be in [0," << (kVulomTBNofChannels - 1) << "])" << endl;
			gMrbLog->Flush(this->ClassName(), "ClearScaler");
			return(kFALSE);
		}
		if (enable & (BIT(ChannelNo))) this->DisableChannel(Module, ChannelNo);
	}
	return(this->SetEnableMask(Module, enable));
}

