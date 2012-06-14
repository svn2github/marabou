#include "TEnv.h"
#include "TPaveStats.h"
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
//	TTimer::SingleShot(20, "HTCanvas", this, "ConnectToModified()");
	ConnectToModified();
//   fTimer = new TTimer();
	fTimer.Connect("Timeout()", "HTCanvas", this, "DoSaveLegendStats()");
}
//______________________________________________________________________________________

void HTCanvas::ConnectToModified() 
{
	TQObject::Connect((TPad*)this, "Modified()",
		  "HTCanvas", this, "HandlePadModified()");
};
//______________________________________________________________________________________

HTCanvas::~HTCanvas()
{
	if ( gDebug > 0 )
		cout << "dtor HTCanvas: " << this << " " << GetName()<< endl;
//   TQObject::Disconnect((TPad*)this, "Modified()");
	fTimer.Stop();
   TQObject::Disconnect((TPad*)this, "Modified()", this, "HandlePadModified()");
	if (fHandleMenus) {
      delete fHandleMenus;
      fHandleMenus = 0;
   }
   if (fHistPresent) fHistPresent->HandleDeleteCanvas(this);
/*	TList * lop = GetListOfPrimitives();
	TIter next(lop);
	TObject *obj;
	TList temp;
	while ( (obj = next()) ){
		if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph") )
			temp.Add(obj);
	}
	TIter next1(&temp);
	while ( (obj = next()) ){
		lop->Remove(obj);
	}*/
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
	if ( event == kButton1Down ) {
		// find pad in which input occured
		pad = Pick(px, py, prevSelObj);
		if (!pad) return;
		gPad = pad;   // don't use cd() because we won't draw in pad
		// we will only use its coordinate system
		if (fSelected->InheritsFrom("TH1") || fSelected->InheritsFrom("TGraph") ) {
			cout << "Select: " << fSelected->ClassName()<< " " << fSelected->GetName() << endl;
		}
	}
	GrCanvas::HandleInput(event, px, py);
}
//______________________________________________________________________________

void HTCanvas::HandlePadModified()
{
	if ( TCanvas::fUpdating ) {
		if ( gDebug > 0 ) 
			cout << "TCanvas::fUpdating" << endl;
		return;
	}
	if ( gDebug > 0 ) 
		cout << "HTCanvas::HandlePadModified() " << this << endl;
	fTimer.Start(20, kTRUE);
//	TTimer::SingleShot(10, "HTCanvas", this, "DoSaveLegendStats()");
//	DoSaveLegendStats();
}	
//______________________________________________________________________________

void HTCanvas::DoSaveLegendStats()
{
	TEnv env(".hprrc");
	if (! env.GetValue("GeneralAttDialog.fRememberStatBox", 1) )
		return;
	if ( gDebug > 0 ) 
		cout << "HTCanvas::DoSaveLegendStats " << this << endl;
	TPave * leg;
	TString envn;
	TList * lop = this->GetListOfPrimitives();
	if ( lop == NULL ) {
		cout << "HTCanvas:: lop == NULL " << endl;
		return;
	}
	leg = (TPave*)lop->FindObject("Legend_SuperImposeGraph");
	if ( TCanvas::fUpdating ) {
		cout << "TCanvas::fUpdating" << endl;
		return;
	}
	if (leg ) {
		envn = "SuperImposeGraph.fLegend";
	} else {
		leg = (TPave*)this->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
		if (leg ) {
			envn = "SuperImposeHist.fLegend";	
		} else {
			TIter next(this->GetListOfPrimitives());
			TObject *obj;
			while ( obj = next() ) {
				if ( obj->InheritsFrom("TH1") ) {
					leg = (TPave*)((TH1*)obj)->GetListOfFunctions()->FindObject("stats");
					if (leg ) {
						envn = "StatBox";	
						if ( ((TH1*)obj)->GetDimension() == 1 ) {
							envn += "1D.f";
						} else if ( ((TH1*)obj)->GetDimension() == 2 ) {
							envn += "2D.f";
						}
					}
				}
			}
		}
	}
	if ( leg ) {
		TString res;
//		TEnv env(".hprrc");
		res = envn + "X1";
		env.SetValue(res, leg->GetX1NDC());
		res = envn + "X2";
		env.SetValue(res, leg->GetX2NDC());
		res = envn + "Y1";
		env.SetValue(res, leg->GetY1NDC());
		res = envn + "Y2";
		env.SetValue(res, leg->GetY2NDC());
		env.SaveLevel(kEnvLocal);
	}
}
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
