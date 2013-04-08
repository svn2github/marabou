#ifndef __vmecontrol_h__
#define __vmecontrol_h__

#ifndef C_STYLE_PROTOS
#define XC	extern "C"		/*!< ROOT/C==: extern "C", C: n/a */
#else
#define XC
#endif

#ifndef CPU_TYPE_RIO4
struct dmachain {
        void *address;
        int count;
};
#endif

//______________________________________________________________[C prototypes]
//////////////////////////////////////////////////////////////////////////////
// Name:           vmecontrol.h
// Purpose:        MARaBOU to Lynx: VME modules
// Description:    Interface C --> C++
// Author:         R. Lutter
// Revision:       $Id: vmecontrol.h,v 1.1 2010-12-27 09:02:14 Marabou Exp $       
// Date:           $Date: 2010-12-27 09:02:14 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

XC	unsigned long return_controller(unsigned long physaddr, unsigned long len);
XC	unsigned long find_controller(unsigned long vmeaddr, unsigned long len, unsigned long am, unsigned long offset, unsigned long size, struct pdparam_master * param);
XC	int vmtopm(int, struct dmachain *, char *, long);


#endif
