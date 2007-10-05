//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo.cxx
// Purpose:        Simulate MBS i/o offline
// Description:    Implements class methods to simulate MBS i/o
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: MbsSimRdo.cxx,v 1.1 2007-10-05 08:32:55 Rudolf.Lutter Exp $       
// Date:           $Date: 2007-10-05 08:32:55 $
//
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"

#include "TMrbLogger.h"

#include "MbsSimRdo.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

MbsSimRdo * gMbsSimRdo = NULL;

ClassImp(MbsSimRdo)
ClassImp(MbsSimModule)

unsigned long * MbsSimRdo::BcnafAddr(Int_t branch, Int_t crate, Int_t nstation, Int_t addr, Int_t function) { return(NULL); };
volatile unsigned long * MbsSimRdo::CioSetBase(Int_t branch, Int_t crate, Int_t nstation) { return(NULL); };
void MbsSimRdo::CioCtrl(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr) {};
void MbsSimRdo::CioCtrlR2b(unsigned long * base, Int_t function, Int_t addr) {};
Int_t MbsSimRdo::CioRead(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t mask) { return(-1); };
Int_t MbsSimRdo::CioReadR2b(unsigned long * base, Int_t function, Int_t addr, Int_t mask) { return(-1); };
void MbsSimRdo::CioWrite(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t data) {};
void MbsSimRdo::CioWriteR2b(unsigned long * base, Int_t function, Int_t addr, Int_t data) {};
void MbsSimRdo::RdoSetup(unsigned long *  base, Int_t function, Int_t addr) {};
void MbsSimRdo::RdoSetupVme(unsigned long * base, Int_t offset) {};
void MbsSimRdo::RdoC2M(Int_t mask, Bool_t shortflag) {};
void MbsSimRdo::RdoBlt(Int_t wc, Int_t mask, Bool_t shortflag) {};
void MbsSimRdo::RdoBltSw(Int_t wc, Int_t mask) {};
Int_t MbsSimRdo::RdoRead(Int_t mask, Bool_t shortflag) { return(-1); };
void MbsSimRdo::RdoIncrAddr(Int_t naddr) {};
void MbsSimRdo::RdoAlign(Bool_t shortflag) {};
void MbsSimRdo::Rdo2Mem(Int_t data, Int_t mask, Bool_t shortflag) {};

void f_ut_send_msg(const Char_t * hdr, const Char_t * msg, Int_t x1, Int_t x2) {
	gMrbLog->Out() << msg << endl;
	gMrbLog->Flush("MbsSimRdo", hdr);
}
