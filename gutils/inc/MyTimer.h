#ifndef __MyTimer_h__
#define __MyTimer_h__
#include <TROOT.h>
#include "TSystem.h"
#include "TTimer.h"
// #include "TGMrbSliders.h"

class TGMrbSliders; 

class MyTimer : public TTimer 
{
private:
  TGMrbSliders * fSlider;
public:  
   MyTimer(Long_t ms, Bool_t synch, TGMrbSliders * slider);
   virtual ~MyTimer(){};
   Bool_t Notify();

ClassDef(MyTimer,0)
};
#endif

