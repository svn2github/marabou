#ifndef CC32
#	ifndef CBV
#		define CBV
#	endif
#endif

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       %M%
// @(#)Purpose:    Camac I/O
// Typedefs:       |
// Description:    Defines macros to handle MBS readout.
//
//                 WARNING!! These macros are legal only in the context of
//                           function f_user_readout!
//
// @(#)Author:     R. Lutter
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_SETUP(base,function,addr) \
		rdoinp = (unsigned long *) ((char *) base + function + addr)

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_SETUP_VME(base, offset) \
		rdoinp = (unsigned long *) ((char *) base + offset)

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_C2M_16()	*evt.out16++ = (unsigned short) *rdoinp
#define RDO_C2M_24()	*evt.out32++ = (unsigned long) (*rdoinp & 0xffffff)
#define RDO_C2M_32()	*evt.out32++ = (unsigned long) *rdoinp

#define RDO_C2MI_16()	*evt.out16++ = (unsigned short) *rdoinp++
#define RDO_C2MI_24()	*evt.out32++ = (unsigned long) (*rdoinp++ & 0xffffff)
#define RDO_C2MI_32()	*evt.out32++ = (unsigned long) *rdoinp++

#define RDO_C2MI_16_M(mask)	*evt.out16++ = (unsigned short) (*rdoinp++ & mask)
#define RDO_C2MI_32_M(mask)	*evt.out32++ = (unsigned long) (*rdoinp++ & mask)

#define RDO_BLT_16(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out16++ = (unsigned short) *rdoinp;	}
#define RDO_BLT_24(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) (*rdoinp & 0xffffff);	}
#define RDO_BLT_32(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) *rdoinp;	}

#define RDO_BLT_16_M(wc,mask)	{ int i; for (i = 0; i < wc; i++) *evt.out16++ = (unsigned short) (*rdoinp & mask);	}
#define RDO_BLT_32_M(wc,mask)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) (*rdoinp & mask);	}

#define RDO_BLTI_16(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out16++ = (unsigned short) *rdoinp++;	}
#define RDO_BLTI_24(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) (*rdoinp++ & 0xffffff);	}
#define RDO_BLTI_32(wc)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) *rdoinp++;	}

#define RDO_BLTI_16_M(wc,mask)	{ int i; for (i = 0; i < wc; i++) *evt.out16++ = (unsigned short) (*rdoinp++ & mask);	}
#define RDO_BLTI_32_M(wc,mask)	{ int i; for (i = 0; i < wc; i++) *evt.out32++ = (unsigned long) (*rdoinp++ & mask);	}

#define RDO_READ_16()	(unsigned short) *rdoinp
#define RDO_READ_24()	(unsigned long) (*rdoinp & 0xffffff)
#define RDO_READ_32()	(unsigned long) *rdoinp

#define RDO_READ_16_M(mask)	(unsigned short) (*rdoinp & mask)
#define RDO_READ_32_M(mask)	(*rdoinp & mask)

#define RDO_READI_16()	(unsigned short) *rdoinp++
#define RDO_READI_24()	(unsigned long) (*rdoinp++ & 0xffffff)
#define RDO_READI_32()	(unsigned long) *rdoinp++

#define RDO_READI_16_M(mask)	(unsigned short) (*rdoinp++ & mask)
#define RDO_READI_32_M(mask)	(*rdoinp++ & mask)

#ifdef CBV
#	define RDO_C2MI_16_C()	RDO_C2MI_16()
#	define RDO_C2MI_24_C()	RDO_C2MI_24()
#	define RDO_C2MI_32_C()	RDO_C2MI_32()

#	define RDO_C2MI_16_C_M(mask)	RDO_C2MI_16_M(mask)
#	define RDO_C2MI_16_C_M(mask)	RDO_C2MI_16_M(mask)

#	define RDO_BLTI_16_C(wc)	RDO_BLTI_16(wc)
#	define RDO_BLTI_24_C(wc)	RDO_BLTI_24(wc)
#	define RDO_BLTI_32_C(wc)	RDO_BLTI_32(wc)

#	define RDO_BLTI_16_C_M(wc,mask)	RDO_BLTI_16_M(wc,mask)
#	define RDO_BLTI_32_C_M(wc,mask)	RDO_BLTI_32_M(wc,mask)

#	define RDO_READI_16_C()	RDO_READI_16()
#	define RDO_READI_24_C()	RDO_READI_24()
#	define RDO_READI_32_C()	RDO_READI_32()

#	define RDO_READI_16_C_M(mask)	RDO_READI_16_M(mask)
#	define RDO_READI_32_C_M(mask)	RDO_READI_32_M(mask)
#endif

#ifdef CC32
#	define RDO_C2MI_16_C()	*evt.out16++ = (unsigned short) *rdoinp; rdoinp += ALONG(1)
#	define RDO_C2MI_24_C()	*evt.out32++ = (unsigned long) (*rdoinp & 0xffffff); rdoinp += ALONG(1)
#	define RDO_C2MI_32_C()	*evt.out32++ = (unsigned long) *rdoinp; rdoinp += ALONG(1)

#	define RDO_C2MI_16_C_M(mask)	*evt.out16++ = (unsigned short) (*rdoinp & mask); rdoinp += ALONG(1)
#	define RDO_C2MI_32_C_M(mask)	*evt.out32++ = (unsigned long) (*rdoinp & mask); rdoinp += ALONG(1)

#	define RDO_BLTI_16_C(wc)	{ int i; for (i = 0; i < wc; i++, rdoinp += ALONG(1)) *evt.out16++ = (unsigned short) *rdoinp;	}
#	define RDO_BLTI_24_C(wc)	{ int i; for (i = 0; i < wc; i++, rdoinp += ALONG(1)) *evt.out32++ = (unsigned long) (*rdoinp & 0xffffff);	}
#	define RDO_BLTI_32_C(wc)	{ int i; for (i = 0; i < wc; i++, rdoinp += ALONG(1)) *evt.out32++ = (unsigned long) *rdoinp;	}

#	define RDO_BLTI_16_C_M(wc,mask)	{ int i; for (i = 0; i < wc; i++, rdoinp += ALONG(1)) *evt.out16++ = (unsigned short) (*rdoinp & mask);	}
#	define RDO_BLTI_32_C_M(wc,mask)	{ int i; for (i = 0; i < wc; i++, rdoinp += ALONG(1)) *evt.out32++ = (unsigned long) (*rdoinp & mask);	}

#	define RDO_READI_16_C()	(unsigned short) *rdoinp; rdoinp += ALONG(1)
#	define RDO_READI_24_C()	(unsigned long) (*rdoinp & 0xffffff); rdoinp += ALONG(1)
#	define RDO_READI_32_C()	(unsigned long) *rdoinp; rdoinp += ALONG(1)

#	define RDO_READI_16_C_M(mask)	(unsigned short) (*rdoinp & mask); rdoinp += ALONG(1)
#	define RDO_READI_32_C_M(mask)	(*rdoinp & mask); rdoinp += ALONG(1)
#endif

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_INCR_ADDR() 	 	rdoinp++
#define RDO_SKIP_ADDR(naddr)	rdoinp += naddr

#ifdef CBV
#	define RDO_INCR_ADDR_C() 	 	RDO_INCR_ADDR()
#	define RDO_SKIP_ADDR_C(naddr)	RDO_SKIP_ADDR(naddr)
#endif

#ifdef CC32
#	define RDO_INCR_ADDR_C() 	 	rdoinp += ALONG(1)
#	define RDO_SKIP_ADDR_C(naddr)	rdoinp += naddr * ALONG(1)
#endif

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_ALIGN_16() \
		if (((((unsigned char *) ps_veshe) - evt.out8) & 1L) != 0L) evt.out8++

#define RDO_ALIGN_32() \
		switch ((((unsigned char *) ps_veshe) - evt.out8) & 3L) \
						{ \
							case 1:	evt.out8++; \
							case 2:	evt.out16++; break; \
							case 3: evt.out8++; \
						}

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define RDO_2MEM_16(data)	*evt.out16++ = data
#define RDO_2MEM_24(data)	*evt.out32++ = data & 0xffffff
#define RDO_2MEM_32(data)	*evt.out32++ = data

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

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

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           WRITE_TIME_STAMP
// Purpose:        Output time stamp (MBS subevent type [9000,1], ROOT Id = 999)
// Syntax:         WRITE_TIME_STAMP()
// Arguments:      --
// Results:        
// Description:    Writes time in secs/nsecs taken from the real-time clock
//                 via clock_gettime system-call.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

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

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           WRITE_DEAD_TIME
// Purpose:        Output dead time (MBS subevent type [9000,2], ROOT Id = 998)
// Syntax:         WRITE_DEAD_TIME()
// Arguments:      --
// Results:        
// Description:    Writes time in secs/nsecs taken from the real-time clock
//                 as well as dead time counts.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

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
