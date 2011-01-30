#include "HTCanvas.h"
#include "HandleMenus.h"

#include "FitHist.h"
#include "HistPresent.h"
#include "SetColor.h"

ClassImp(HTCanvas)

//____________________________________________________________________________
HTCanvas::HTCanvas():GrCanvas()
{
   fHistPresent = NULL;
   fFitHist     = NULL;
   fGraph       = NULL;
   fHandleMenus = NULL;
};

HTCanvas::HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr, FitHist * fh,
           TGraph * graph, Int_t flag)
           : GrCanvas(name, title, wtopx, wtopy, ww,wh),
			    fHistPresent(hpr), fFitHist(fh),fGraph(graph)
{

//*-*-*-*-*-*-*-*-*-*-*-*Canvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ==================
//  Create a new canvas.
//
//  wtopx,wtopy are the pixel coordinates of the top left corner of the canvas
//     if wtopx < 0) the menubar is not shown.
//  ww is the canvas size in pixels along X
//  wh is the canvas size in pixels along Y
//
// HTCanvas has the following extra facilities:
//
// Markers:   Pressing the middle mouse sets markers used by class FitHist
// Menus:     The class HandleMenus adds popup menus the standard menubar
//            to invoke  functions in classes HistPresent and FitHist
//--------------------------------------------------------------------------

   fHandleMenus = NULL;
	
   if (TestBit(kMenuBar)) {
      BuildHprMenus(fHistPresent, fFitHist, fGraph);
      if (flag & GrCanvas::kIsAEditorPage) {
			cout << "HTCanvas::kIsAEditorPage" << endl;
         SetBit(GrCanvas::kIsAEditorPage);
      }
   }
	if ( gDebug > 0 )
		cout << "ctor HTCanvas: " << this << " " << name
        << " Id " << GetRootCanvas()->GetId() << endl;
};
//______________________________________________________________________________________

HTCanvas::~HTCanvas()
{
	if ( gDebug > 0 )
		cout << "dtor HTCanvas: " << this << " " << GetName()<< endl;
   Disconnect("TPad", "Modified()");
	if (fHandleMenus) {
      delete fHandleMenus;
      fHandleMenus = 0;
   }
   if (fHistPresent) fHistPresent->HandleDeleteCanvas(this);
   if(fFitHist) {
      fFitHist->UpdateCut();
      fFitHist->SetCanvasIsDeleted();
//      cout << "HTCanvas: delete fFitHist " << this << endl;
      delete fFitHist;
   }
};
//____________________________________________________________________________

void HTCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//

   TPad         *pad;
   TObject      *prevSelObj = 0;
   if ( event == kButton2Down ) {
       // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;
      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if(fFitHist) fFitHist->AddMark((TPad*)gPad,px,py);
	}
	GrCanvas::HandleInput(event, px, py);
}

//______________________________________________________________________________
//______________________________________________________________________________

void HTCanvas::BuildHprMenus(HistPresent *hpr, FitHist *fh, TGraph *gr)
{
   fHistPresent = hpr;
   fFitHist     = fh;
   fGraph       = gr;

   fHandleMenus = new HandleMenus(this, fHistPresent, fFitHist, fGraph);
	if ( gDebug  > 1)
		cout << "fHandleMenus->GetId() " << fHandleMenus->GetId() << endl;
   fHandleMenus->BuildMenus();
}
