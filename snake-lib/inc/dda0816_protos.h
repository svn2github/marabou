#include <Rtypes.h>

//_________________________________________________________[C DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           dda0816_protos.h
// Purpose:        C interface to dda0816 calls
// Description:    C prototypes for dda0816
// Author:         R. Lutter
// Revision:       $Id: dda0816_protos.h,v 1.1 2009-03-26 11:18:28 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

extern "C" Int_t dda0816_SetUpdateGroup(Int_t Fd);
extern "C" Int_t dda0816_SetPacerClock(Int_t Fd, Int_t Clock);
extern "C" Int_t dda0816_SetPreScaler(Int_t Fd, Int_t Peroid);
extern "C" Int_t dda0816_SetOutputClockGeneration(Int_t Fd);
extern "C" Int_t dda0816_SetClockSource(Int_t Fd, Int_t Source);
extern "C" Int_t dda0816_SetCurve(Int_t Fd, struct dda0816_curve Curve);
extern "C" Int_t dda0816_SetCycle(Int_t Fd, Int_t Cycles);
extern "C" Int_t dda0816_StartPacer(Int_t Fd);
extern "C" Int_t dda0816_StopPacer(Int_t Fd);
extern "C" Int_t dda0816_GetPacer(Int_t Fd);
