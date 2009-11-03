#include "TROOT.h"
#include "TApplication.h"
#include "TWbox.h"
#include "TButton.h"
#include "TGClient.h"
#include "TGuiFactory.h"
#include "TRootHelpDialog.h"
#include "TGMenu.h"
#include "TGWindow.h"
#include "TStyle.h"
#include "TString.h"
#include "TH1.h"
#include "TList.h"
#include "TMarker.h"
#include "TArrow.h"
#include "TEllipse.h"
#include "TMath.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "TContextMenu.h"
#include "TVirtualPadEditor.h"

#include "FitHist.h"
#include "GEdit.h"
#include "HistPresent.h"
#include "support.h"
#include "SetColor.h"
#include "TMrbHelpBrowser.h"
#include "TMrbString.h"
#include "TGMrbValuesAndText.h"
#include "EditMarker.h"
#include "HprEditBits.h"
#include "HprElement.h"
#include "GroupOfGObjects.h"

const Size_t kDefaultCanvasSize   = 20;

ClassImp(HTCanvas)

//____________________________________________________________________________
HTCanvas::HTCanvas():GrCanvas()
{
   fHistPresent = NULL;
   fFitHist     = NULL;
   fGraph       = NULL;
   fHandleMenus = NULL;
   fGEdit       = NULL;
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

//   HTCanvas *old = (HTCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
//   if (old && old->IsOnHeap()) delete old;
   fHandleMenus = NULL;
   if(fHistPresent && !fFitHist)fHistPresent->SetMyCanvas(GetRootCanvas());
   if (TestBit(kMenuBar)) {
      fHandleMenus = new HandleMenus(this, fHistPresent, fFitHist, fGraph);
      fHandleMenus->BuildMenus();
		fCanvasImp->ShowEditor(kFALSE);
		fCanvasImp->ShowToolBar(kFALSE);
		fCanvasImp->ShowStatusBar(kFALSE);
		fCanvasImp->ShowMenuBar(HasMenuBar());
      if (flag & HTCanvas::kIsAEditorPage) {
			cout << "HTCanvas::kIsAEditorPage" << endl;
         SetBit(HTCanvas::kIsAEditorPage);
      }
		SetWindowSize(ww , wh );
   }
//   cout << "ctor HTCanvas: " << this << " " << name
//        << " Id " << GetRootCanvas()->GetId() << endl;
};
//______________________________________________________________________________________

HTCanvas::~HTCanvas()
{
//   cout << "dtor HTCanvas: " << this << " " << GetName()<< endl;
//   if (fEditCommands) { delete fEditCommands; fEditCommands = NULL;};
   if (fHandleMenus) {
      delete fHandleMenus;
      fHandleMenus = 0;
   }
   if (fHistPresent) fHistPresent->HandleDeleteCanvas(this);
//   if(fGraph) delete fGraph;
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

//   fHandleMenus = new HandleMenus(this, fHistPresent, fFitHist, fGraph);
//   cout << "fHandleMenus->GetId() " << fHandleMenus->GetId() << endl;
//   fHandleMenus->BuildMenus();
/*
   fCanvasImp->ShowEditor(kFALSE);
   fCanvasImp->ShowToolBar(kFALSE);
   fCanvasImp->ShowStatusBar(kFALSE);
   fCanvasImp->ShowMenuBar(HasMenuBar());
*/
}
