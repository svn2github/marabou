
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
#include "TGraph.h"
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
   HistPresent    * fHistPresent;   //! dont stream
   FitHist        * fFitHist;       //! dont stream
   TList          * fHistList;      //! dont stream
   TGraph         * fGraph;         //! dont stream
   HTimer         * fTimer;         //! dont stream
   TRootCanvas    * fRootCanvas;    //! dont stream
   HandleMenus    * fHandleMenus;      //! dont stream
   Double_t       fGridX;
   Double_t       fGridY;
   Bool_t         fUseGrid;
   void Build();
//
public:
   HTCanvas();
   HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr = 0, FitHist * fh = 0,
           TList * hlist = 0, TGraph * graph = 0);
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
   Double_t GetGridX() {return fGridX;};
   Double_t GetGridY() {return fGridY;};
   void    SetGrid(Double_t x, Double_t y);
   void    SetGridX(Double_t x) {fGridX = x;};
   void    SetGridY(Double_t y) {fGridY = y;};
   void    SetUseGrid(Bool_t use = kTRUE) {fUseGrid = use;};
   Bool_t  GetUseGrid() {return fUseGrid;};
   void    DrawGrid();
   void    RemoveGrid();
   void    DrawHist();
   void    WritePrimitives();
   void    GetPrimitives();

   ClassDef(HTCanvas,1)  //Graphics canvas
};

#endif

