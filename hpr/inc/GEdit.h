#ifndef ROOT_GEdit
#define ROOT_GEdit
#include "TList.h"
#ifdef MARABOUVERS
#include "HTCanvas.h"
#include "HistPresent.h"
#else
#include "TCanvas.h"
#endif
#include "TGraph.h"
#include "TCutG.h"
#include <TGMenu.h>
#include <iostream>

using std::cout;
using std::endl;

class HprEditCommands;
class GEdit : public TObject {

private:
#ifdef MARABOUVERS
   HistPresent    *fHistPresent;
   HTCanvas        *fParent;
#else
   TCanvas        *fParent;
#endif
   TGPopupMenu     *fEditMenu;
   TRootCanvas     *fRootCanvas;      //! dont stream
   HprEditCommands *fEditCommands;    //! dont stream
   HprEditCommands *fModifyCommands;  //! dont stream
   TList           *fGObjectGroups;   // compound graphic objects
   Double_t       fEditGridX;
   Double_t       fEditGridY;
   Double_t       fVisibleGridX;
   Double_t       fVisibleGridY;
   Bool_t         fUseEditGrid;
   Bool_t         fSaveUseEditGrid;
   Bool_t         fInsertMacrosAsGroup;  
   Bool_t         fCommonRotate;         //! dont stream
   UInt_t         fOrigWw;
   UInt_t         fOrigWh;

   Int_t          fFeynmanPhi1;
   Int_t          fFeynmanPhi2;

public:
   GEdit() {};
#ifdef MARABOUVERS
   GEdit(HTCanvas * parent);
#else
   GEdit(TCanvas * parent);
#endif
   virtual ~GEdit();
   void RecursiveRemove(TObject * obj);

   Bool_t   SloppyInside(TCutG * cut, Double_t x, Double_t y);
   Double_t MinElement(Int_t n, Double_t * x);
   Double_t MaxElement(Int_t n, Double_t * x);
   void SetAllArrowSizes(TList *list, Double_t size,  Bool_t abs); 
   void SetAllCurlySizes(TList *list, Double_t wl, Double_t amp, Bool_t abs);
   void SetAllTextSizes(TList *list, Double_t size, Bool_t abs);
 
   void     BuildMenu();
   void     HandleMenu(Int_t id);
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

#ifdef MARABOUVERS
   void    InsertHist();
#endif
   void    InsertArc();
   void    InsertGraph();
   void    GrabImage();
   void    InsertImage();
   void    InsertAxis();
   void    InsertCurlyArrow();
   void    FeynmanDiagMenu();
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
   void    InsertTextF(){InsertText(kTRUE);};
   void    InsertTextK(){InsertText(kFALSE);};
   void    InsertFunction();
   TPad*   GetEmptyPad();
   void    WarnBox(const Char_t *text, TRootCanvas *win = NULL);
   Bool_t  QuestionBox(const Char_t *text, TRootCanvas *win = NULL);
   void    ZoomIn();
   void    ZoomOut();
   void    UnZoom();
   

//   void    MyClose();
   ClassDef(GEdit,0)  //Graphics canvas
};

#endif
