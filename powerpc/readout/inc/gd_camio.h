#ifndef CC32
#	ifndef CBV
#		define CBV
#	endif
#endif

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       %M%
// @(#)Purpose:    Camac I/O
// Typedefs:       
// Description:    Defines macros to handle camac I/O.
// @(#)Author:     R. Lutter 
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           C, N, A, F
// Purpose:        Encode CNAF components
// Syntax:         B(x)
//                 C(x)
//                 N(x)
//                 A(x)
//                 F(x)
// Arguments:      x  -- value of cnaf component
// Results:        CNAF components shifted into right place
// Description:    Encodes cnaf components such as B, C, N, A, and F to hardware format.
//                 %%%% Any addr calculation if done on a (char*) base %%%%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#ifdef CBV
#	define B(branch)	((branch) * 0)
#	define C(crate) 	(crate)
#	define N(nstation) 	((nstation) << 11)
#	define A(addr) 		((addr) << 2)
#	define F(function) 	((function) << 6)
#endif


#ifdef CC32
#	define B(branch)	(0)
#	define C(crate) 	(crate)
#	define N(nstation) 	((nstation) << 10)
#	define A(addr) 		((addr) << 6)
#	define F(function) 	((function & 0xf) << 2)
#	define ALONG(addr)	((addr) << 4)
#endif

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           [B]CNAF_ADDR
// Purpose:        Calculate [B]CNAF address
// Syntax:         BCNAF_ADDR(branch, crate, nstation, addr, function)
// Arguments:      branch    -- branch number (default 0)
//                 crate     -- crate number (default 0)
//                 nstation  -- camac station Nxx (xx = 1 .. 31)
//                 addr      -- camac subaddress Axx (xx = 0 .. 15)
//                 function  -- camac function code Fxx (xx = 0 .. 31)
// Results:        Addr of cnaf instr
// Description:    Calculates the resulting addr for a given cnaf.
//                 %%%% Any addr calculation if done on a (char*) base %%%%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define BCNAF_ADDR(branch,crate,nstation,addr,function) \
      ((unsigned long *) ((((branch) == 0 && (crate) == 0) ? \
              ((char *) pl_loc_hwacc) : \
              ((char *) pl_rem_cam[crate] + branch)) \
                       + nstation + addr + function))

#define CNAF_ADDR(crate,nstation,addr,function) \
			(BCNAF_ADDR((0),(crate),(nstation),(addr),(function)))

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_SET_BASE
// Purpose:        Return base address
// Syntax:         CIO_SET_BASE(branch, crate, nstation)
// Arguments:      branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
// Results:        Returns a pointer of type "unsigned long *"
// Description:    Calculates the base address of a given module.
//                 ( = address of B.C.N.A(0).F(0))
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CIO_SET_BASE(branch,crate,nstation)	\
			BCNAF_ADDR((branch),(crate),(nstation),A(0),F(0))

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CIO_CTRL
//                 CIO_CTRL_R2B
// Purpose:        Execute a CONTROL cnaf
// Syntax:         CIO_CTRL(branch, crate, station, function, addr)
//                 CIO_CTRL_R2B(base, function, addr)
// Arguments:      base      -- cnaf base given by CIO_SET_BASE(B, C, N)
//                 branch    -- camac branch
//                 crate     -- camac crate
//                 station   -- camac station
//                 function  -- function code
//                 addr      -- subaddress
//                 data      -- data word
// Results:        
// Description:    Executes a control cnaf for a given module.
//                 R2B macro calculates cnaf address relative to base.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CIO_CTRL(branch,crate,station,function,addr) \
				*(volatile unsigned long *)(BCNAF_ADDR(branch,crate,station,function,addr))

#define CIO_CTRL_R2B(base,function,addr) \
				*(volatile unsigned long *)((char *)base + function + addr)

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define CIO_READ(branch,crate,station,function,addr) \
				*(volatile unsigned long *)(BCNAF_ADDR(branch,crate,station,function,addr))

#define CIO_READ_16(branch,crate,station,function,addr) \
				(*(volatile unsigned long *)(BCNAF_ADDR(branch,crate,station,function,addr)) & 0xFFFF)

#define CIO_READ_M(branch,crate,station,function,addr,mask) \
				(*(volatile unsigned long *)(BCNAF_ADDR(branch,crate,station,function,addr)) & mask)

#define CIO_READ_R2B(base,function,addr) \
				(*(volatile unsigned long *) ((char *)base + function + addr))

#define CIO_READ_R2B_16(base,function,addr) \
				((*(volatile unsigned long *) ((char *)base + function + addr)) & 0xFFFF)

#define CIO_READ_R2B_M(base,function,addr,mask) \
				((*(volatile unsigned long *) ((char *)base + function + addr)) & mask)

/*_____________________________________________________________________[MACRO]
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
////////////////////////////////////////////////////////////////////////////*/

#define CIO_WRITE(branch,crate,station,function,addr,data) \
				*(BCNAF_ADDR(branch,crate,station,function,addr)) = data

#define CIO_WRITE_R2B(base,function,addr,data) \
			*(unsigned long *) ((char *)base + function + addr) = data

