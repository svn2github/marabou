/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           cc32_camio.h
// Purpose:        Camac I/O for CC32 controller
// Typedefs:       
// Description:    Defines macros to handle camac I/O for the CC32 controller
// Author:         R. Lutter 
// Revision:       
// Date:           Jun 2002
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

#define C(crate) 		((crate-1) << 15)
#define N(nstation) 	((nstation) << 10)
#define A(addr) 		((addr) << 6)
#define F(function) 	((function & 0xf) << 2)
#define ALONG(addr)		((addr) << 4)

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CNAF_ADDR
// Purpose:        Calculate CNAF address
// Syntax:         CNAF_ADDR(crate, nstation, addr, function)
// Arguments:      crate     -- crate number Cxx (xx = 1 .. 8)
//                 nstation  -- camac station Nxx (xx = 1 .. 31)
//                 addr      -- camac subaddress Axx (xx = 0 .. 15)
//                 function  -- camac function code Fxx (xx = 0 .. 31)
// Results:        Addr of cnaf instr
// Description:    Calculates the resulting addr for a given cnaf.
//                 %%%% Any addr calculation if done on a (char*) base %%%%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CNAF_ADDR(crate,nstation,addr,function) ((volatile unsigned long *) (((char *) cc32_baseAddr) + crate + nstation + addr + function))

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CC32_SET_BASE
// Purpose:        Return base address
// Syntax:         CC32_SET_BASE(crate, nstation)
// Arguments:      crate     -- camac crate
//                 station   -- camac station
// Results:        Returns a pointer of type "unsigned long *"
// Description:    Calculates the base address of a given module.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CC32_SET_BASE(crate,nstation) 	CNAF_ADDR((crate),(nstation),A(0),F(0))

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CC32_CTRL
//                 CC32_CTRL_R2B
// Purpose:        Execute a CONTROL cnaf
// Syntax:         CC32_CTRL(crate, station, function, addr)
//                 CC32_CTRL_R2B(base, function, addr)
// Arguments:      base      -- cnaf base given by CC32_SET_BASE(B, C, N)
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

#define CC32_CTRL(crate,station,function,addr) \
				*(volatile unsigned long *)(CNAF_ADDR(crate,station,function,addr))

#define CC32_CTRL_R2B(base,function,addr) \
				*(volatile unsigned long *)((char *)base + function + addr)

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CC32_READ
//                 CC32_READ_16
//                 CC32_READ_M
//                 CC32_READ_R2B
//                 CC32_READ_R2B_16
//                 CC32_READ_R2B_M
// Purpose:        Execute a READ cnaf
// Syntax:         CC32_READ(crate, station, function, addr)
//                 CC32_READ_16(crate, station, function, addr)
//                 CC32_READ_M(crate, station, function, addr, mask)
//                 CC32_READ_R2B(base, function, addr)
//                 CC32_READ_R2B_16(base, function, addr)
//                 CC32_READ_R2B_M(base, function, addr, mask)
// Arguments:      base      -- cnaf base given by CC32_SET_BASE(B, C, N)
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

#define CC32_READ(crate,station,function,addr) \
				*(volatile unsigned long *)(CNAF_ADDR(crate,station,function,addr))

#define CC32_READ_16(crate,station,function,addr) \
				(*(volatile unsigned long *)(CNAF_ADDR(crate,station,function,addr)) & 0xFFFF)

#define CC32_READ_M(crate,station,function,addr,mask) \
				(*(volatile unsigned long *)(CNAF_ADDR(crate,station,function,addr)) & mask)

#define CC32_READ_R2B(base,function,addr) \
				(*(volatile unsigned long *) ((char *)base + function + addr))

#define CC32_READ_R2B_16(base,function,addr) \
				((*(volatile unsigned long *) ((char *)base + function + addr)) & 0xFFFF)

#define CC32_READ_R2B_M(base,function,addr,mask) \
				((*(volatile unsigned long *) ((char *)base + function + addr)) & mask)

/*_____________________________________________________________________[MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:           CC32_WRITE
//                 CC32_WRITE_R2B
// Purpose:        Execute a WRITE cnaf
// Syntax:         CC32_WRITE(crate,station,function,addr,data)
//                 CC32_WRITE_R2B(base, function, addr, data)
// Arguments:      base      -- cnaf base given by CC32_SET_BASE(B, C, N)
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

#define CC32_WRITE(crate,station,function,addr,data) \
				*(CNAF_ADDR(crate,station,function,addr)) = data

#define CC32_WRITE_R2B(base,function,addr,data) \
			*(unsigned long *) ((char *)base + function + addr) = data

