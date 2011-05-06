#include "TEnv.h"
#include "TPave.h"
#include "TFrame.h"
#include "TTimer.h"
#include "HprGaxis.h"
#include "TRegexp.h"
#include "TMath.h"
#include "TList.h"
#include <iostream>
ClassImp(HprGaxis)


HprGaxis::HprGaxis(TCanvas * canvas, Double_t xmin, Double_t ymin, Double_t xmax, Double_t ymax,
					Double_t wmin, Double_t wmax, Int_t ndiv , 
					Option_t* chopt, Double_t gridlength): 
					TGaxis(xmin, ymin, xmax, ymax, wmin, wmax,ndiv, chopt,gridlength)
{
	fAxisOffset = 0;
	fOffset = 0;
	fRatio = 1;
	fWhere = 1;
	fCanvas = canvas;
	fFrameX1 = fCanvas->GetFrame()->GetX1();
	fFrameX2 = fCanvas->GetFrame()->GetX2();
	fFrameY1 = fCanvas->GetFrame()->GetY1();
	fFrameY2 = fCanvas->GetFrame()->GetY2();
	fLogx = fCanvas->GetLogx();
	fLogy = fCanvas->GetLogy();
	TQObject::Connect("TPad", "Modified()",
							"HprGaxis", this, "HandlePadModified()");
							
};
//__________________________________________________________________

void HprGaxis::ReDoAxis()
{	
//	TIter next(canvas->GetListOfPrimitives());
//	TObject *obj;
	if ( (fFrameX1 == fCanvas->GetFrame()->GetX1() && fFrameX2 == fCanvas->GetFrame()->GetX2()
		&& fFrameY1 == fCanvas->GetFrame()->GetY1() && fFrameY2 == fCanvas->GetFrame()->GetY2())
		&& fCanvas->GetLogy() == fLogy &&  fCanvas->GetLogx() == fLogx ) {
//		return;
/*		TPave * leg;
		TString envn;
		leg = (TPave*)fCanvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeGraph");
		if (leg ) {
			envn = "SuperImposeGraph.";
		} else {
			leg = (TPave*)fCanvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
			if (leg ) {
				envn = "SuperImposeHist.";	
			}
		}
		if ( leg ) {
			TString res;
			TEnv env(".hprrc");
			res = envn + "fLegendX1";
			env.SetValue(res, leg->GetX1NDC());
			res = envn + "fLegendX2";
			env.SetValue(res, leg->GetX2NDC());
			res = envn + "fLegendY1";
			env.SetValue(res, leg->GetY1NDC());
			res = envn + "fLegendY2";
			env.SetValue(res, leg->GetY2NDC());
			env.SaveLevel(kEnvLocal);
		}*/
		return;
	}
	Double_t ledge = 0, uedge = 0;
	Double_t x, y, x1=0, y1=0, x2=1, y2=1;
	TRegexp loG("G");
//	canvas->GetListOfPrimitives()->ls();
//	while ( obj = next() ) {
//		if ( obj->IsA() == HprGaxis::Class() ) {
//			HprGaxis * a = (HprGaxis*)obj;
			HprGaxis * a = this;
			x1 = fCanvas->GetFrame()->GetX1();
			x2 = fCanvas->GetFrame()->GetX2();
			y1 = fCanvas->GetFrame()->GetY1();
			y2 = fCanvas->GetFrame()->GetY2();
//			cout << "x1... "<<x1<<" "<<x2<<" "<<y1<<" " << y2<<endl;
			Bool_t log_axis = kFALSE;
			if ( a->GetWhere() == 1 ) {
				if ( fCanvas->GetLogx() ) {
					x1 = TMath::Power(10, x1);
					x2 = TMath::Power(10, x2);
					log_axis = kTRUE;
				}
				ledge = a->GetOffset() + x1 * a->GetRatio();
				uedge = ledge + (x2 - x1) * a->GetRatio();
				y = y2 + (y2 - y1) * a->GetAxisOffset();
				if (fCanvas->GetLogy() ) {
					y = TMath::Power(10, y);
				}
				y1 = y2 = y;
			} else if ( a->GetWhere() == 2 ) {
				if ( fCanvas->GetLogy() ) {
					y1 = TMath::Power(10, y1);
					y2 = TMath::Power(10, y2);
					log_axis = kTRUE;
				}
				ledge = y1 * a->GetRatio();
//				ledge = a->GetOffset() + y1 * a->GetRatio();
				uedge = ledge + (y2 - y1) * a->GetRatio();
				x = x2 + (x2 - x1) * a->GetAxisOffset();
				if ( fCanvas->GetLogx() ) {
					x = TMath::Power(10, x);
				}
				x1 = x2 = x;
			}
			TString opt(a->GetOption());
			if ( log_axis ) {
				if ( !opt.Contains("G") ) {
					opt += "G";
					a->SetOption(opt);
				}
			} else {
				if ( opt.Contains("G") ) {
					opt(loG) =  "";
					a->SetOption(opt);
				}
			}
			a->SetX1(x1);
			a->SetX2(x2);
			a->SetY1(y1);
			a->SetY2(y2);
			a->SetWmin(ledge);
			a->SetWmax(uedge);
			if ( gDebug > 0 ) {
				cout << "ReDoAxis: " <<  a->GetWhere();
				if (log_axis ) 
					cout << " log_axis ";
				cout << " opt " << opt <<" "<<x1<<" "<<x2<<" "<<y1<<" " << y2<<" "<<
						ledge<<" " << uedge 
				<< endl;
			}
	fCanvas->Modified();
	fCanvas->Update();
	fFrameX1 = fCanvas->GetFrame()->GetX1();
	fFrameX2 = fCanvas->GetFrame()->GetX2();
	fFrameY1 = fCanvas->GetFrame()->GetY1();
	fFrameY2 = fCanvas->GetFrame()->GetY2();
	fLogx = fCanvas->GetLogx();
	fLogy = fCanvas->GetLogy();
}
//__________________________________________________________________

void HprGaxis::HandlePadModified()
{
	if (gPad == fCanvas) {
		if ( gDebug > 0 ) {
			cout << "HprGaxis::HandlePadModified this: " << this << "  fCanvas: " << fCanvas <<
					" fCanvas->GetLogy(), fLogy " <<  fCanvas->GetLogy() << " " << fLogy <<endl;
			cout << "GetX1(), fFrameX1: "<<fCanvas->GetFrame()->GetX1()<<" "<<fFrameX1 <<endl;
		}
		TTimer::SingleShot(200, "HprGaxis", this, "ReDoAxis()");
	}
}