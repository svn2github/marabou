#ifndef PARALLEL_GRAPH
#define PARALLEL_GRAPH
#include "TArrayF.h"
#include "TGraph.h"
#include "iostream"

class XSpline; 

class ParallelGraph : public TGraph
{
private:
   XSpline* fParent;

public:
   ParallelGraph (Int_t npoints = 0, Double_t*  x = NULL, Double_t* y = NULL);
   virtual ~ParallelGraph() {};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void SetParent(XSpline* parent); 
   XSpline* GetParent(){return fParent;}; 
   virtual void Delete(Option_t *opt = " "); // *MENU*
   ClassDef(ParallelGraph, 1)
};
#endif
