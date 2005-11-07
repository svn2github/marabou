#ifndef PARALLEL_GRAPH
#define PARALLEL_GRAPH
#include "TArrayF.h"
#include "TGraph.h"
#include "iostream"

class ParallelGraph : public TGraph
{
private:
   TGraph* fParent;
   Double_t fDist;       // distance to parent
   Bool_t fClosed;
public:
   ParallelGraph () {};
   ParallelGraph (TGraph *parent, Double_t dist, Bool_t closed);
   virtual ~ParallelGraph() {if (fParent) fParent->RecursiveRemove(this);};
   void Compute ();
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   TGraph *GetParent(){return fParent;}; 
   Double_t GetDist(){return fDist;}; 
   virtual void Delete(Option_t *opt = " "); // *MENU*
   void CorrectForArrows(Double_t alength, Double_t aangle,Double_t aindent_angle,
                                     Bool_t at_start, Bool_t at_end);
   ClassDef(ParallelGraph, 1)
};
#endif
