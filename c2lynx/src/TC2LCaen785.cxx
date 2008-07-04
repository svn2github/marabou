//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LCaen785.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods to connect to VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TC2LCaen785.cxx,v 1.1 2008-07-04 11:58:06 Rudolf.Lutter Exp $     
// Date:           $Date: 2008-07-04 11:58:06 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TEnv.h"
#include "TROOT.h"

#include "TMrbC2Lynx.h"
#include "TC2LCaen785.h"

#include "TMrbLogger.h"
#include "SetColor.h"

extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

ClassImp(TC2LVMEModule)
