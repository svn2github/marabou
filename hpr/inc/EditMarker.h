#ifndef EDITMARKER
#define EDITMARKER
//#ifndef __CINT__
#include "TMarker.h"

using namespace std;

//#endif
class EditMarker : public TMarker {

public:
   EditMarker(){};
   EditMarker(Float_t x1, Float_t y1);
   EditMarker(Float_t x1, Float_t y1, Float_t sz);
   ~EditMarker(){};
   Int_t DistancetoPrimitive(Int_t px, Int_t py){return 9999;};
ClassDef(EditMarker,0)
};
#endif
