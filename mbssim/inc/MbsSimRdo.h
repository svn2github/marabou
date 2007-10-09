#ifndef __MbsSimRdo_h__
#define __MbsSimRdo_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo.h
// Purpose:        Simulate MBS i/o offline
// Class:          MbsSimRdo            -- base class
// Description:    Class definitions to simulate MBS i/o
// Author:         R. Lutter
// Revision:       $Id: MbsSimRdo.h,v 1.3 2007-10-09 12:05:24 Rudolf.Lutter Exp $       
// Date:           $Date: 2007-10-09 12:05:24 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TObject.h"
#include "TString.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbEnv.h"

#define ERR__MSG_INFO	0
#define ERR__MSG_ERROR	0
#define MASK__PRTT	0

struct pdparam_master {};

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           C, N, A, F
// Purpose:        Encode CNAF components
// Syntax:         B(x)
//                 C(x)
//                 N(x)
//                 A(x)
//                 F(x)
// Arguments:      x  -- value of cnaf component
//////////////////////////////////////////////////////////////////////////////

#define B(branch)		branch	
#define C(crate)		crate
#define N(nstation) 	nstation
#define A(addr) 		addr
#define F(function) 	function

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           [B]CNAF_ADDR
// Purpose:        Calculate [B]CNAF address
// Syntax:         BCNAF_ADDR(branch, crate, nstation, addr, function)
// Arguments:      branch    -- branch number (default 0)
//                 crate     -- crate number (default 0)
//                 nstation  -- camac station Nxx (xx = 1 .. 31)
//                 addr      -- camac subaddress Axx (xx = 0 .. 15)
//                 function  -- camac function code Fxx (xx = 0 .. 31)
//////////////////////////////////////////////////////////////////////////////

#define BCNAF_ADDR(branch,crate,nstation,addr,function) gMbsSimRdo->BcnafAddr(branch,crate,nstation,addr,function)

#define CNAF_ADDR(crate,nstation,addr,function) gMbsSimRdo->BcnafAddr(0,crate,nstation,addr,function)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_SET_BASE
// Purpose:        Return base address
// Syntax:         CIO_SET_BASE(branch, crate, nstation)
// Arguments:      branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
//////////////////////////////////////////////////////////////////////////////

#define CIO_SET_BASE(branch,crate,nstation)	gMbsSimRdo->CioSetBase(branch,crate,nstation)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_CTRL
//                 CIO_CTRL_R2B
// Purpose:        Execute a CONTROL cnaf
// Syntax:         CIO_CTRL(branch, crate, station, function, addr)
//                 CIO_CTRL_R2B(base, function, addr)
//                 CIO_CTRL2(branch, crate, station, function, addr)
//                 CIO_CTRL2_R2B(base, function, addr)
// Arguments:      base      -- cnaf base given by CIO_SET_BASE(B, C, N)
//                 branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
//                 function  -- function code
//                 addr      -- subaddress
//                 data      -- data word
//////////////////////////////////////////////////////////////////////////////

#define CIO_CTRL(branch,crate,station,function,addr) gMbsSimRdo->CioCtrl(branch,crate,station,function,addr)

#define CIO_CTRL2(branch,crate,station,function,addr) gMbsSimRdo->CioCtrl(branch,crate,station,function,addr)

#define CIO_CTRL_R2B(base,function,addr) gMbsSimRdo->CioCtrlR2b((long unsigned int *) base,function,addr)

#define CIO_CTRL2_R2B(base,function,addr) gMbsSimRdo->CioCtrlR2b((long unsigned int *) base,function,addr)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_READ
//                 CIO_READ_16
//                 CIO_READ_M
//                 CIO_READ_R2B
//                 CIO_READ_R2B_16
//                 CIO_READ_R2B_M
// Purpose:        Execute a READ cnaf
// Syntax:         CIO_READ(branch, crate, station, function, addr)
//                 CIO_READ_16(branch, crate, station, function, addr)
//                 CIO_READ_M(branch, crate, station, function, addr, mask)
//                 CIO_READ_R2B(base, function, addr)
//                 CIO_READ_R2B_16(base, function, addr)
//                 CIO_READ_R2B_M(base, function, addr, mask)
// Arguments:      base      -- cnaf base given by CIO_SET_BASE(B, C, N)
//                 branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
//                 function  -- function code
//                 addr      -- subaddress
//                 data      -- data word
//                 mask      -- bit mask
// Results:        Returns a data item of type "unsigned long"
// Description:    Executes a read cnaf for a given module
//                 *_R2B calculates cnaf address relative to base,
//                 *_16 cuts data from 32 bits down to 16 bits
//                 *_M applies given mask bits.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define CIO_READ(branch,crate,station,function,addr) gMbsSimRdo->CioRead(branch,crate,station,function,addr,0xFFFFFF)

#define CIO_READ_16(branch,crate,station,function,addr) gMbsSimRdo->CioRead(branch,crate,station,function,addr,0xFFFF)

#define CIO_READ_M(branch,crate,station,function,addr,mask) gMbsSimRdo->CioRead(branch,crate,station,function,addr,mask)

#define CIO_READ_R2B(base,function,addr) gMbsSimRdo->CioReadR2b((long unsigned int *) base,function,addr,0xFFFFFF)

#define CIO_READ_R2B_16(base,function,addr) gMbsSimRdo->CioReadR2b((long unsigned int *) base,function,addr,0xFFFF)

#define CIO_READ_R2B_M(base,function,addr,mask) gMbsSimRdo->CioReadR2b((long unsigned int *) base,function,addr,mask)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_WRITE
//                 CIO_WRITE_R2B
// Purpose:        Execute a WRITE cnaf
// Syntax:         CIO_WRITE(branch,crate,station,function,addr,data)
//                 CIO_WRITE_R2B(base, function, addr, data)
// Arguments:      base      -- cnaf base given by CIO_SET_BASE(B, C, N)
//                 branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
//                 function  -- function code
//                 addr      -- subaddress
//                 data      -- data word
// Results:        
// Description:    Executes a write cnaf for a given module
//                 R2B macro calculates cnaf address relative to base
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define CIO_WRITE(branch,crate,station,function,addr,data) gMbsSimRdo->CioWrite(branch,crate,station,function,addr,data)

#define CIO_WRITE_R2B(base,function,addr,data) gMbsSimRdo->CioWriteR2b((long unsigned int *) base,function,addr,data)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_SETUP
//                 RDO_SETUP_VME
// Purpose:        Set up base addr for subsequent readout operations (CAMAC)
// Syntax:         RDO_SETUP(base, function, addr)
// Arguments:      base      -- cnaf base given by CIO_SET_BASE(B, C, N)
//                 function  -- function code
//                 addr      -- subaddress
// Results:        unsigned long * rdoinp  -- ptr where to get data from
// Description:    Calculates a pointer to F.A of given module.
//                 %%%% Any addr calculation if done on a (char*) base %%%%
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_SETUP(base,function,addr) gMbsSimRdo->RdoSetup((long unsigned int *) base,function,addr)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_SETUP_VME
// Purpose:        Set up base addr for subsequent readout operations (VME)
// Syntax:         RDO_SETUP_VME(base, offset)
// Arguments:      base      -- module base addr
//                 offset    -- channel offset
// Results:        unsigned long * rdoinp  -- ptr where to get data from
// Description:    Calculates a pointer to specified channel of given module
//                 %%%% Any addr calculation if done on a (long*) base %%%%
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_SETUP_VME(base,offset) gMbsSimRdo->RdoSetupVme((long unsigned int *) base,offset)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_XXXX
//                 RDO_XXXX_C
// Purpose:        Xfer a data item from camac to memory
// Syntax:         RDO_C2MI_16()
//                 RDO_C2MI_24()
//                 RDO_C2MI_32()
//                 RDO_C2MI_16_M(mask)
//                 RDO_C2MI_32_M(mask)
//                 RDO_BLT_16(wc)
//                 RDO_BLT_24(wc)
//                 RDO_BLT_32(wc)
//                 RDO_BLT_32_SW(wc)
//                 RDO_BLT_16_M(wc,mask)
//                 RDO_BLT_32_M(wc,mask)
//                 RDO_BLTI_16(wc)
//                 RDO_BLTI_24(wc)
//                 RDO_BLTI_32(wc)
//                 RDO_BLTI_16_M(wc,mask)
//                 RDO_BLTI_32_M(wc,mask)
//                 RDO_READ_16()
//                 RDO_READ_24()
//                 RDO_READ_32()
//                 RDO_READ_16_M(mask)
//                 RDO_READ_24_M(mask)
//                 RDO_READ_32_M(mask)
//                 RDO_READI_16()
//                 RDO_READI_24()
//                 RDO_READI_32()
//                 RDO_READI_16_M(mask)
//                 RDO_READI_24_M(mask)
//                 RDO_READI_32_M(mask)
// Arguments:      --
// Results:        unsigned short * evt  -- ptr to event buffer (16 bit)
//                 unsigned long * evt   -- ptr to event buffer (24/32 bit)
//                 int wc                -- word count
//                 unsigned long mask    -- bit mask
// Description:    Executes subsequent readout oerations defined by RDO_SETUP / RDO_SETUP_VME.
//                 Increments input and output pointers.
//                 RDO_C2MI_nn           -- store data, increment input & output ptrs
//                 RDO_C2MI_nn_M         -- apply a data mask before storing data.
//                 RDO_BLT_nn            -- block xfer, input ptr stays const
//                 RDO_BLT_nn_SW         -- block xfer, swapped, input ptr stays const
//                 RDO_BLT_nn_M          -- block xfer with mask, input ptr const
//                 RDO_BLTI_xxxx         -- block xfer, input ptr incremented
//                 RDO_BLT_nn_M          -- block xfer with mask, input ptr incremented
//                 RDO_READ_nn           -- read data nn bits
//                 RDO_READ_nn_M         -- read masked
//                 RDO_READI_nn          -- read and increment
//                 RDO_READI_nn_M        -- read masked and increment
//
//                 Suffix "_C"           -- special macros to be used for CAMAC (*)
//
//                 (*) As different CAMAC controllers have different "increment A" operations
//                     auto-increment mode can't be applied in general:
//                           CBV:    "A" coding starts at bit 0 (good design!) --> auto incr rdoinp++
//                           CC32:   "A" coding starts at bit 6 (why that?)    --> rdoinp += ALONG(1) needed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_C2M_16()	gMbsSimRdo->RdoC2M(0xFFFF, kTRUE)
#define RDO_C2M_24()	gMbsSimRdo->RdoC2M(0xFFFFFF)
#define RDO_C2M_32()	gMbsSimRdo->RdoC2M(0xFFFFFFFF)

#define RDO_C2MI_16()	gMbsSimRdo->RdoC2M(0xFFFF, kTRUE)
#define RDO_C2MI_24()	gMbsSimRdo->RdoC2M(0xFFFFFF)
#define RDO_C2MI_32()	gMbsSimRdo->RdoC2M(0xFFFFFFFF)

#define RDO_C2MI_16_M(mask)	gMbsSimRdo->RdoC2M(mask, kTRUE)
#define RDO_C2MI_32_M(mask)	gMbsSimRdo->RdoC2M(mask)

#define RDO_BLT_16(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFF, kTRUE)
#define RDO_BLT_24(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFFFF)
#define RDO_BLT_32(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFFFFFF)

#define RDO_BLT_16_M(wc,mask)	gMbsSimRdo->RdoBlt(wc, mask, kTRUE)
#define RDO_BLT_32_M(wc,mask)	gMbsSimRdo->RdoBlt(wc, mask)

#define RDO_BLT_32_SW(wc)	gMbsSimRdo->RdoBltSw(wc, 0xFFFFFFFF)
#define RDO_BLT_32_M_SW(wc,mask)	gMbsSimRdo->RdoBltSw(wc, mask)

#define RDO_BLTI_16(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFF, kTRUE)
#define RDO_BLTI_24(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFFFF)
#define RDO_BLTI_32(wc)	gMbsSimRdo->RdoBlt(wc, 0xFFFFFFFF)

#define RDO_BLTI_16_M(wc,mask)	gMbsSimRdo->RdoBlt(wc, mask, kTRUE)
#define RDO_BLTI_32_M(wc,mask)	gMbsSimRdo->RdoBlt(wc, mask)

#define RDO_BLTI_32_SW(wc)	gMbsSimRdo->RdoBltSw(wc, 0xFFFFFFFF)
#define RDO_BLTI_32_M_SW(wc,mask)	gMbsSimRdo->RdoBltSw(wc, mask)

#define RDO_READ_16()	gMbsSimRdo->RdoRead(0xFFFF, kTRUE)
#define RDO_READ_24()	gMbsSimRdo->RdoRead(0xFFFFFF)
#define RDO_READ_32()	gMbsSimRdo->RdoRead(0xFFFFFFFF)

#define RDO_READ_16_M(mask)	gMbsSimRdo->RdoRead(mask, kTRUE)
#define RDO_READ_32_M(mask)	gMbsSimRdo->RdoRead(mask)

#define RDO_READI_16()	gMbsSimRdo->RdoRead(0xFFFF, kTRUE)
#define RDO_READI_24()	gMbsSimRdo->RdoRead(0xFFFFFF)
#define RDO_READI_32()	gMbsSimRdo->RdoRead(0xFFFFFFFF)

#define RDO_READI_16_M(mask)	gMbsSimRdo->RdoRead(mask, kTRUE)
#define RDO_READI_32_M(mask)	gMbsSimRdo->RdoRead(mask)

#define RDO_C2MI_16_C()	gMbsSimRdo->RdoC2M(0xFFFF, kTRUE)
#define RDO_C2MI_24_C()	gMbsSimRdo->RdoC2M(0xFFFFFF)
#define RDO_C2MI_32_C()	gMbsSimRdo->RdoC2M(0xFFFFFFFF)

#define RDO_C2MI_16_C_M(mask)	gMbsSimRdo->RdoC2M(mask, kTRUE)

#define RDO_BLTI_16_C(wc)	gMbsSimRdo->RdoC2M(0xFFFF, kTRUE)
#define RDO_BLTI_24_C(wc)	gMbsSimRdo->RdoC2M(0xFFFFFF)
#define RDO_BLTI_32_C(wc)	gMbsSimRdo->RdoC2M(0xFFFFFFFF)

#define RDO_BLTI_16_C_M(wc,mask)	gMbsSimRdo->RdoC2M(mask, kTRUE)
#define RDO_BLTI_32_C_M(wc,mask)	gMbsSimRdo->RdoC2M(mask)

#define RDO_READI_16_C()	gMbsSimRdo->RdoRead(0xFFFF, kTRUE)
#define RDO_READI_24_C()	gMbsSimRdo->RdoRead(0xFFFFFF)
#define RDO_READI_32_C()	gMbsSimRdo->RdoRead(0xFFFFFFFF)

#define RDO_READI_16_C_M(mask)	gMbsSimRdo->RdoRead(mask, kTRUE)
#define RDO_READI_32_C_M(mask)	gMbsSimRdo->RdoRead(mask)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_SKIP_ADDR
//                 RDO_INCR_ADDR<empty clipboard>
// Purpose:        Skip next subaddress(es)
// Syntax:         RDO_SKIP_ADDR(naddr)
//                 RDO_INCR_ADDR()
// Arguments:      naddr  -- number of subaddresses to be skipped
// Results:        unsigned long * rdoinp  -- ptr where to read data from
// Description:    Increments subaddress A(x) by some value
//                 Suffix "_C" for CAMAC operations (see explanation above)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_INCR_ADDR() 	 	gMbsSimRdo->RdoIncrAddr(1)
#define RDO_SKIP_ADDR(naddr)	gMbsSimRdo->RdoIncrAddr(naddr)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_ALIGN_16
//                 RDO_ALIGN_32
// Purpose:        Align output pointer
// Syntax:         RDO_ALIGN_16()
//                 RDO_ALIGN_32()
// Arguments:      --
// Results:        Memory pointer aligned 16/32 bit boundaries
// Description:    Aligns output pointer from 8 to 16 and from 8/16 to 32 bits
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_ALIGN_16() gMbsSimRdo->RdoAlign(kTRUE)

#define RDO_ALIGN_32() gMbsSimRdo->RdoAlign()

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           RDO_2MEM_16
//                 RDO_2MEM_24
//                 RDO_2MEM_32
// Purpose:        Output data to memory
// Syntax:         RDO_2MEM_16(data)
//                 RDO_2MEM_24(data)
//                 RDO_2MEM_32(data)
// Arguments:      data  -- data to be output
// Results:        
// Description:    Outputs data to memory and increments output pointer
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define RDO_2MEM_16(data)	gMbsSimRdo->Rdo2Mem(data, 0xFFFF, kTRUE)
#define RDO_2MEM_24(data)	gMbsSimRdo->Rdo2Mem(data, 0xFFFFFF)
#define RDO_2MEM_32(data)	gMbsSimRdo->Rdo2Mem(data, 0xFFFFFFFF)

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           BEGIN_SUBEVENT
//                 END_SUBEVENT
// Purpose:        Create a MBS subevent structure
// Syntax:         BEGIN_SUBEVENT(type, subtype, control, crate, sevtid)
//                 END_SUBEVENT()
// Arguments:      type     -- subevent type
//                 subtype  -- subevent subtype
//                 control  -- control word
//                 crate    -- (sub)crate number
//                 sevtid   -- unique subevent id
// Results:        
// Description:    Begin_subvent creates a subevent header of given type.
//                 (Note: member i_procid is used to store a unique subevent id)
//                 End_subevent updates header data (i.e. subevent length).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define BEGIN_SUBEVENT(type,subtype,control,crate,sevtid) \
					ps_veshe->l_dlen = (long) 0L; \
					ps_veshe->i_type = (short) type; \
					ps_veshe->i_subtype = (short) subtype; \
					ps_veshe->h_control = (char) control; \
					ps_veshe->h_subcrate = (char) crate; \
					ps_veshe->i_procid = (short) sevtid; \
					evt.out8 = (unsigned char *) ps_veshe + sizeof(s_veshe)

#define END_SUBEVENT() \
					RDO_ALIGN_32(); \
					ps_veshe->l_dlen = evt.out16 - ((unsigned short *) ps_veshe) \
										- sizeof(s_evhe) / sizeof(short); \
					ps_veshe = (s_veshe *) evt.out8

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           WRITE_TIME_STAMP
// Purpose:        Output time stamp (MBS subevent type [9000,1], ROOT Id = 999)
// Syntax:         WRITE_TIME_STAMP()
// Arguments:      --
// Results:        
// Description:    Writes time in secs/nsecs taken from the real-time clock
//                 via clock_gettime system-call.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define WRITE_TIME_STAMP() \
					BEGIN_SUBEVENT(9000, 1, 0, 0, 999); \
					{ \
						struct timespec ts; \
						clock_gettime(CLOCK_REALTIME, &ts); \
						*evt.out32++ = clock_resolution; \
						*evt.out32++ = ts.tv_sec; \
						*evt.out32++ = ts.tv_nsec; \
					} \
					END_SUBEVENT()

//_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           WRITE_DEAD_TIME
// Purpose:        Output dead time (MBS subevent type [9000,2], ROOT Id = 998)
// Syntax:         WRITE_DEAD_TIME()
// Arguments:      --
// Results:        
// Description:    Writes time in secs/nsecs taken from the real-time clock
//                 as well as dead time counts.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#define WRITE_DEAD_TIME() \
					BEGIN_SUBEVENT(9000, 2, 0, 0, 998); \
					{ \
						struct timespec ts; \
						clock_gettime(CLOCK_REALTIME, &ts); \
						*evt.out32++ = clock_resolution; \
						*evt.out32++ = ts.tv_sec; \
						*evt.out32++ = ts.tv_nsec; \
						*evt.out32++ = total_event_count; \
						*evt.out32++ = dt_event_count; \
						*evt.out32++ = dt_scaler_contents; \
					} \
					END_SUBEVENT()


//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimModule
// Purpose:        Define a module in the mbs simulation environment
// Methods:
// Description:    Defines a camac/vme module
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class MbsSimModule : public TNamed {

	public:

		MbsSimModule() {};

		MbsSimModule(const Char_t * Module, TMrbNamedX * Type, Int_t Serial, Int_t Crate, Int_t Station) : TNamed(Module, Type->GetTitle()) {
			fModuleType = Type;
			fSerial = Serial;
			fCrate = Crate;
			fStation = Station;
			fIsCamac = kTRUE;
		};

		MbsSimModule(const Char_t * Module, TMrbNamedX * Type, Int_t Serial, Int_t Addr) : TNamed(Module, Type->GetTitle()) {
			fModuleType = Type;
			fSerial = Serial;
			fVmeAddr = Addr;
			fIsCamac = kFALSE;
		};


		~MbsSimModule() {}; 

		inline void SetInterface(Bool_t CamacFlag) { fIsCamac = CamacFlag; };
		inline Bool_t IsCamac() { return(fIsCamac); };
		inline Bool_t IsVME() { return(!fIsCamac); };

		inline void SetCamacAddr(Int_t Crate, Int_t Station) { fCrate = Crate; fStation = Station; };
		inline Int_t Crate() { return(fIsCamac ? fCrate : -1); };
		inline Int_t Station() { return(fIsCamac ? fStation : -1); };

		inline void SetSerial(Int_t Serial) { fSerial = Serial; };
		inline Int_t Serial() { return(fSerial); };

		inline void SetVmeAddr(Int_t Addr) { fVmeAddr = Addr; };
		inline Int_t VmeAddr() { return(fIsCamac ? -1 : fVmeAddr); };

		inline void SetType(TMrbNamedX * Type) { fModuleType = Type; };
		inline TMrbNamedX * GetType() { return(fModuleType); };

	protected:
		Int_t fIsCamac;
		Int_t fSerial;
		TMrbNamedX * fModuleType;
		Int_t fCrate;
		Int_t fStation;
		Int_t fVmeAddr;

	ClassDef(MbsSimModule, 1)		// [MBS Simulate Readout] Define module
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo
// Purpose:        Simulate MBS I/O offline
// Methods:
// Description:    Provides a tool to simulate MBS data i/o offline
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class MbsSimRdo : public TObject {

	public:
		enum EMbsSimModuleType	{	kMbsSimMtypeCamac			= 0x1000000,
									kMbsSimMtypeVme 			= 0x2000000,
									kMbsSimMtypeCaen 			= 0x0010000,
									kMbsSimMtypeSis 			= 0x0020000,
									kMbsSimMtypeSilena  		= 0x0030000,
									kMbsSimMtypeXia 			= 0x0040000,
									kMbsSimMtypeSilena4418V  	= kMbsSimMtypeCamac | kMbsSimMtypeSilena | 0x1,
									kMbsSimMtypeSilena4418T  	= kMbsSimMtypeCamac | kMbsSimMtypeSilena | 0x2,
									kMbsSimMtypeCaenV785 		= kMbsSimMtypeVme | kMbsSimMtypeCaen | 0x1,
									kMbsSimMtypeCaenV775 		= kMbsSimMtypeVme | kMbsSimMtypeCaen | 0x2,
									kMbsSimMtypeSis3300 		= kMbsSimMtypeVme | kMbsSimMtypeSis | 0x1,
									kMbsSimMtypeSis3600 		= kMbsSimMtypeVme | kMbsSimMtypeSis | 0x2,
									kMbsSimMtypeSis3801 		= kMbsSimMtypeVme | kMbsSimMtypeSis | 0x3,
									kMbsSimMtypeSis3820 		= kMbsSimMtypeVme | kMbsSimMtypeSis | 0x4,
									kMbsSimMtypeXiaDgf4C		= kMbsSimMtypeCamac | kMbsSimMtypeXia | 0x1,
								};

	public:

		MbsSimRdo() {};	
		MbsSimRdo(const Char_t * ConfigFile);	
		~MbsSimRdo() {}; 

		Bool_t SetConfigFile(const Char_t * ConfigFile);

		MbsSimModule * FindModule(const Char_t * Module, Bool_t CreateIt = kFALSE);
		MbsSimModule * FindModule(Int_t Crate, Int_t Station, Bool_t CreateIt = kFALSE);
		MbsSimModule * FindModule(Int_t Addr, Bool_t CreateIt = kFALSE);
		
		void Init();

		inline void SetVerbose(Bool_t Flag) { fVerbose = Flag; };
		inline Bool_t IsVerbose() { return(fVerbose); };

		unsigned long * BcnafAddr(Int_t branch, Int_t crate, Int_t nstation, Int_t addr, Int_t function);
		volatile unsigned long * CioSetBase(Int_t branch, Int_t crate, Int_t nstation);
		void CioCtrl(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr);
		void CioCtrlR2b(unsigned long * base, Int_t function, Int_t addr);
		Int_t CioRead(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t mask);
		Int_t CioReadR2b(unsigned long * base, Int_t function, Int_t addr, Int_t mask);
		void CioWrite(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t data);
		void CioWriteR2b(unsigned long * base, Int_t function, Int_t addr, Int_t data);
		void RdoSetup(unsigned long *  base, Int_t function, Int_t addr);
		void RdoSetupVme(unsigned long * base, Int_t offset);
		void RdoC2M(Int_t mask, Bool_t shortflag = kFALSE);
		void RdoBlt(Int_t wc, Int_t mask, Bool_t shortflag = kFALSE);
		void RdoBltSw(Int_t wc, Int_t mask);
		Int_t RdoRead(Int_t mask, Bool_t shortflag = kFALSE);
		void RdoIncrAddr(Int_t naddr);
		void RdoAlign(Bool_t shortflag = kFALSE);
		void Rdo2Mem(Int_t data, Int_t mask, Bool_t shortflag = kFALSE);

	protected:
		MbsSimModule * CreateModule(const Char_t * ModuleName);
		Int_t GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, Int_t Default);
		Bool_t GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, Bool_t Default);
		const Char_t * GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, const Char_t * Default);

	protected:
		Bool_t fVerbose;
		TString fConfigFile;
		TEnv * fConfig;
		TMrbLofNamedX fLofModuleTypes;
		TMrbLofNamedX fLofModules;

	ClassDef(MbsSimRdo, 1)		// [MBS Simulate Readout] Base class
};

void f_ut_send_msg(const Char_t * hdr, const Char_t * msg, Int_t dmy1, Int_t dmy2);
inline void sleep(Int_t secs) {};

#endif
