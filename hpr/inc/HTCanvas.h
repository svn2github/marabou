
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
   TList          * fGObjectGroups;      // compound graphic objects
   
   Double_t       fEditGridX;
   Double_t       fEditGridY;
   Double_t       fVisibleGridX;
   Double_t       fVisibleGridY;
   Bool_t         fUseEditGrid;
   Bool_t         fInsertMacrosAsGroup;
   Double_t       fMouseX;
   Double_t       fMouseY;
   Bool_t         fGetMouse;

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
   Double_t GetEditGridX() {return fEditGridX;};
   Double_t GetEditGridY() {return fEditGridY;};
   Double_t GetVisibleGridX() {return fVisibleGridX;};
   Double_t GetVisibleGridY() {return fVisibleGridY;};
   void    SetEditGrid(Double_t x, Double_t y, Double_t xvis, Double_t yvis);
   void    SetEditGridX(Double_t x) {fEditGridX = x;};
   void    SetEditGridY(Double_t y) {fEditGridY = y;};
   void    SetVisibleGridX(Double_t x) {fVisibleGridX = x;};
   void    SetVisibleGridY(Double_t y) {fVisibleGridY = y;};
   void    SetUseEditGrid(Bool_t use = kTRUE);
   Bool_t  GetUseEditGrid() {return fUseEditGrid;};
   void    DrawEditGrid(Bool_t visible);
   void    RemoveEditGrid();
   void    DrawHist();
   void    InsertImage();
   void    WritePrimitives();
   void    GetPrimitives();
   Int_t   ExtractGObjects();
   void    InsertGObjects(const char * fname = 0);
   void    WriteGObjects();
   void    ReadGObjects();
   void    ShowGallery();

   ClassDef(HTCanvas,1)  //Graphics canvas
};

#endif

