#ifndef CONTROL_GRAPH
#define CONTROL_GRAPH
#include "TArrayF.h"
#include "TGraph.h"
#include "iostream"

class XSpline; 

class ControlGraph : public TGraph
{
private:
   XSpline* fParent;
   TArrayF  fShapeFactors;
   Int_t    fSelectedPoint;
   Double_t fSelectedX;
   Double_t fSelectedY;
   Float_t  fSelectedShapeFactor;

public:
   ControlGraph (Int_t npoints = 0, Double_t*  x = NULL, Double_t* y = NULL);
   virtual ~ControlGraph() {std::cout << "~ControlGraph() " << std::endl <<std:: flush;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void SetParent(XSpline* parent); 
   void SetAllShapeFactors(Int_t npoints, Float_t* sf);
   Float_t   GetShapeFactorByPointNumber(Int_t ipoint) {return fShapeFactors[ipoint];};
   Int_t     GetSelectedPoint() { return fSelectedPoint; };
   Double_t  GetSelectedX()     { return fSelectedX; };
   Double_t  GetSelectedY()     { return fSelectedY; };
   Float_t   GetSelectedShapeFactor(){ return fSelectedShapeFactor; };

   virtual void SetOneShapeFactor(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor); // *MENU* *ARGS={ipoint=>fSelectedPoint,x=>fSelectedX,y=>fSelectedY,sfactor=>fSelectedShapeFactor}
   Int_t InsertPoint();                                          // *MENU*
   Int_t RemovePoint();                                          // *MENU*
   virtual void EditControlGraph();    //  *MENU*
   virtual void ControlGraphMixer();    //  *MENU*
   virtual void Delete(Option_t *opt = " "); // *MENU*
   void SetShapeFactors(Int_t id, Int_t ip, Int_t val);
   TGraph* AddPGraph(Double_t dist = 2, Color_t color=0, 
                            Width_t width=0, Style_t style=0);   // *MENU*
   ClassDef(ControlGraph, 1)
};
#endif
