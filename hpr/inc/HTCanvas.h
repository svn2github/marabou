
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
#include "HprEditCommands.h"

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
   HandleMenus    * fHandleMenus;   //! dont stream
   HprEditCommands * fEditCommands; //! dont stream
   HprEditCommands * fModifyCommands; //! dont stream
   TList          * fGObjectGroups;      // compound graphic objects
   
   Double_t       fEditGridX;
   Double_t       fEditGridY;
   Double_t       fVisibleGridX;
   Double_t       fVisibleGridY;
   Bool_t         fUseEditGrid;
   Bool_t         fSaveUseEditGrid;
   Bool_t         fInsertMacrosAsGroup;  
   Double_t       fMouseX;               //! dont stream
   Double_t       fMouseY;               //! dont stream
   TPad         * fMousePad;             //! dont stream
   Bool_t         fGetMouse;             //! dont stream
   Bool_t         fCommonRotate;         //! dont stream
   UInt_t         fOrigWw;
   UInt_t         fOrigWh;

   void Build();
//
public:
   enum {
      kIsAEditorPage = BIT(23)
   };
   HTCanvas();
   HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr = 0, FitHist * fh = 0,
           TList * hlist = 0, TGraph * graph = 0, Int_t flag = 0);
   virtual ~HTCanvas();
   virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
   virtual void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   HistPresent * GetHistPresent();
   FitHist     * GetFitHist();
   TList * GetHistList() {return fHistList;};
   void BuildHprMenus(HistPresent *hpr, FitHist *fh, TGraph *gr);
   Bool_t GetCommonRotate() {return fCommonRotate;};
   void   SetCommonRotate(Bool_t cr) {fCommonRotate = cr;};
   HTimer * GetHTimer() {return fTimer;};
   void ActivateTimer(Int_t delay);    // start timer, delay(milli seconds)
   void RunAutoExec();
   void UpdateHists();
   void ClearFitHist(){fFitHist = 0;};
   void ClearHistPresent(){fHistPresent = 0;};
   void SetLog(Int_t state);
   void     InitEditCommands();
   void     ModifyObjects();
   void     ModifyTexts();
   void     ModifyCurlyLines();
   void     ModifyCurlyArcs();
   void     ModifyArrows();
   void     ModifyArcs();
   void     ModifyEllipses();
   void     ModifyMarkers();
   void     ModifyPaves();
   void     ModifyPads();
   void     ModifyGraphs();
   void     ListAllObjects();

   Double_t GetEditGridX() {return fEditGridX;};
   Double_t GetEditGridY() {return fEditGridY;};
   Double_t GetVisibleGridX() {return fVisibleGridX;};
   void     PutObjectsOnGrid(TList* list = 0);
   Double_t PutOnGridX(Double_t);
   Double_t PutOnGridY(Double_t);
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
   void    RemoveControlGraphs();
   void    RemoveParallelGraphs();
   void    RemoveTSplineXsPolyLines();
   void    DrawTSplineXsParallelGraphs();
   void    DrawControlGraphs();
   void    SetVisibilityOfEnclosingCuts(Bool_t visible);
   void    InsertHist();
   void    InsertGraph();
   void    GrabImage();
   void    InsertImage();
   void    InsertAxis();
   void    InsertTSplineX();
   void    WritePrimitives();
   void    DefineBox();
   void    DefinePolygone();
   Int_t   ExtractGObjectsE();
   Int_t   MarkGObjects();
   Int_t   ExtractGObjects(Bool_t markonly = kFALSE);
   void    InsertGObjects(const char * fname = 0);
   void    DeleteObjects();
   void    WriteGObjects();
   void    ReadGObjects();
   void    ShowGallery();
//   void    ShiftObjects(TList * list, Double_t xshift, Double_t yshift);
   void    InsertText(Bool_t);
   void    InsertFunction();
   void    InsertTextF(){InsertText(kTRUE);};
   void    InsertTextK(){InsertText(kFALSE);};
   TPad*   GetEmptyPad();
   void    ZoomIn();
   void    ZoomOut();
   void    UnZoom();
   void    MyClose();

   ClassDef(HTCanvas,1)  //Graphics canvas
};

#endif

