
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
#include "TGMenu.h"
#include "TGStatusBar.h"
#include "TRootCanvas.h"

#include <iostream>

namespace std {} using namespace std;


class FitHist;
class HistPresent;
class HTimer;
class HandleMenus;

class HTCanvas : public TCanvas {

friend class HandleMenus;

private:
   HistPresent    * fHistPresent; 
   FitHist        * fFitHist;
   HTimer         * fTimer;
   TList          * fHistList;
   TRootCanvas    * fRootCanvas;
   HandleMenus * fHandleMenus;
   Double_t       fGridX;
   Double_t       fGridY;
   Bool_t         fUseGrid;
   void Build();
//
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
   void SetLog(Int_t state);
   Double_t GetGridX(){return fGridX;};
   Double_t GetGridY(){return fGridY;};
   void    SetGridX(Double_t x){fGridX = x;};
   void    SetGridY(Double_t y){fGridY = y;};
   void    SetUseGrid(Bool_t use){fUseGrid = use;};
   Bool_t  GetUseGrid(){return fUseGrid;};

   ClassDef(HTCanvas,0)  //Graphics canvas
};

#endif

