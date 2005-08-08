#ifndef RAILWAYSLEEPER
#define RAILWAYSLEEPER

#include "TPolyLine.h"
#include "iostream"

class XSpline;
namespace std {} using namespace std;

//#endif
class RailwaySleeper : public  TPolyLine{
private:
   XSpline * fParent; //!
   Color_t fColor;
public:
   RailwaySleeper(){};
   RailwaySleeper(Double_t * x, Double_t * y, XSpline * parent, Color_t color = 1);
//   ~RailwaySleeper(){std::cout << "dtor RailwaySleeper: " << this << std::endl; };
   ~RailwaySleeper(){ };
//   Int_t DistancetoPrimitive(Int_t px, Int_t py){return 9999;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   Color_t GetFillColor() {return fColor;};
   void SetColor(Color_t color); 
   void SetFillColor(Color_t color);     // *MENU*
   void Draw(Option_t * opt = "");
ClassDef(RailwaySleeper,1)
};
#endif
