#include "TTimer.h"
#include "TSystem.h"
#include "TDatime.h"
#include "HTCanvas.h"

#include <iostream>
//_____________________________________________________________________________________


class HTimer : public TTimer {

private:
   HTCanvas * fHCanvas;
public:
   HTimer(Long_t ms, Bool_t synch, HTCanvas * ca);
   Bool_t Notify();
   Bool_t ReadNotify(){return Notify();}
};
//_____________________________________________________________________________________

HTimer::HTimer(Long_t ms, Bool_t synch, HTCanvas * ca):TTimer(ms,synch){
     cout << "init mytimer" << endl;
   fHCanvas = ca;
   gSystem->AddTimer(this);
}

Bool_t HTimer::Notify() {
   fHCanvas->UpdateHists();
   Reset();
   return kTRUE;
};
