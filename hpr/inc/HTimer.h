#include "TTimer.h"
#include "TSystem.h"
#include <iostream>
#include "HTCanvas.h"
//_____________________________________________________________________________________


class HTimer : public TTimer {

private:
   HTCanvas * fHCanvas;
public:
   HTimer(Long_t ms, Bool_t synch, HTCanvas * ca);
   Bool_t Notify();
   Bool_t ReadNotify(){return Notify();}
};
