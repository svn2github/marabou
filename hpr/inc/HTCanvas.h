
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
class HandleMenus;
class HprElement;

class HTCanvas : public TCanvas {

friend class HandleMenus;

private:
   HistPresent    * fHistPresent;   //! dont stream
   FitHist        * fFitHist;       //! dont stream
   TGraph         * fGraph;         //! dont stream
   TRootCanvas    * fRootCanvas;    //! dont stream
   HandleMenus    * fHandleMenus;   //! dont stream
   Bool_t         fHasConnection;    //! dont stream

   TList          *fHiddenPrimitives;

   Double_t       fEditGridX;
   Double_t       fEditGridY;
   Int_t          fUseEditGrid;
   UInt_t         fOrigWw;
   UInt_t         fOrigWh;
   Int_t          fCurrentPlane;
   void Build();
//
public:
   enum {
      kIsObjectList  = BIT(22),
      kIsAEditorPage = BIT(23)
   };
   HTCanvas();
   HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr = 0, FitHist * fh = 0,
           TGraph * graph = 0, Int_t flag = 0);
   virtual ~HTCanvas();
   virtual void HandleInput(EEventType button, Int_t x, Int_t y);
   virtual void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   HistPresent  *GetHistPresent() {return fHistPresent;};
   FitHist      *GetFitHist() {return fFitHist;};
   void          SetHistPresent(HistPresent* hpr) {fHistPresent = hpr;};
   void          SetFitHist(FitHist* fh) {fFitHist = fh;};
   void   BuildHprMenus(HistPresent *hpr, FitHist *fh, TGraph *gr);
   void   RunAutoExec();
   void   SetLog(Int_t state);
   void   MyClose();
   Double_t GetEditGridX() {return fEditGridX;};
   Double_t GetEditGridY() {return fEditGridY;};
   void     SetEditGrid(Double_t x, Double_t y) {fEditGridX = x, fEditGridY = y;};
   void    SetUseEditGrid(Int_t use = 1) {fUseEditGrid = use;};
   Int_t   GetUseEditGrid() {return fUseEditGrid;};
   void    Add2ConnectedClasses(TObject *obj);
   void    ObjectCreated(Int_t, Int_t, TObject*);
   void    ObjectMoved(Int_t, Int_t, TObject*);
   Double_t PutOnGridX(Double_t);
   Double_t PutOnGridY(Double_t);
   Double_t PutOnGridX_NDC(Double_t);
   Double_t PutOnGridY_NDC(Double_t);
   void     HideObject(TObject*);
   void     ViewObject(TObject*);
   void     ViewAllObjects(); // *MENU*
   Int_t    GetCurrentPlane() {return fCurrentPlane;};
   void     SetCurrentPlane(Int_t p) {fCurrentPlane = p;};
   TList    *GetHiddenPrimitives() {return fHiddenPrimitives;};
   void PrintAddress()  {std::cout << ClassName() << "* aa = "
                         << "(" << ClassName() << "*)" << this
                         << std::endl; }; // *MENU*

//   TList   *GetConnectedClasses() { return &fConnectedClasses;};
   ClassDef(HTCanvas,3)  //Graphics canvas
};

#endif

