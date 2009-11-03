
#ifndef ROOT_HTCanvas
#define ROOT_HTCanvas

//+SEQ,CopyRight,T=NOINCLUDE.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HTCanvas                                                              //
//                                                                      //
// Graphics canvas.                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*KEEP,TPad.
#include "GrCanvas.h"
#include <iostream>

namespace std {} using namespace std;


class FitHist;
class HistPresent;
class HandleMenus;
class HprElement;
class GEdit;
class TGraph;

class HTCanvas : public GrCanvas {

friend class HandleMenus;

private:
   HistPresent    * fHistPresent;     //! dont stream
   FitHist        * fFitHist;         //! dont stream
   TGraph         * fGraph;           //! dont stream
   HandleMenus    * fHandleMenus;     //! dont stream
   GEdit          * fGEdit;           //! dont stream
//
public:
   HTCanvas();
   HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr = 0, FitHist * fh = 0,
           TGraph * graph = 0, Int_t flag = 0);
   virtual ~HTCanvas();
   virtual void HandleInput(EEventType button, Int_t x, Int_t y);
   HistPresent  *GetHistPresent() {return fHistPresent;};
   FitHist      *GetFitHist() {return fFitHist;};
   void          SetHistPresent(HistPresent* hpr) {fHistPresent = hpr;};
   void          SetFitHist(FitHist* fh) {fFitHist = fh;};
   void   BuildHprMenus(HistPresent *hpr, FitHist *fh, TGraph *gr);
   ClassDef(HTCanvas,5)  //Graphics canvas
};

#endif

