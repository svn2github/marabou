
#ifndef ROOT_HTCanvas
#define ROOT_HTCanvas

//+SEQ,CopyRight,T=NOINCLUDE.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCanvas                                                              //
//                                                                      //
// Graphics canvas.                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*KEEP,TPad.
#include "TList.h"
#include "TCanvas.h"
#include "HTCanvasImp.h"

using namespace std;


class FitHist;
class HistPresent;
class HTimer;


class HTCanvas : public TCanvas {

private:
   HistPresent    * fHistPresent; 
   FitHist        * fFitHist;
   HTimer         * fTimer;
   TList          * fHistList;
   void Build();

public:
   HTCanvas();
   HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr = 0, FitHist * fh = 0,
           TList * hlist = 0);
   virtual ~HTCanvas();
   virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
   virtual void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   HistPresent * GetHistPresent();
   FitHist     * GetFitHist();
   TList * GetHistList() {return fHistList;};
   HTimer * GetHTimer() {return fTimer;};
   void ActivateTimer(Int_t delay);    // start timer, delay(milli seconds)
   void RunAutoExec();
   void UpdateHists();
   void ClearFitHist(){fFitHist = 0;};
   void ClearHistPresent(){fHistPresent = 0;};
   ClassDef(HTCanvas,0)  //Graphics canvas
};

#endif

