#include "TPad.h"
#include "TRegexp.h"
#include "Buttons.h"
#include "HprLegend.h"
#include <iostream>

ClassImp(HprLegend)

//________________________________________________________________________

HprLegend::HprLegend(Double_t x1, Double_t y1,Double_t x2, Double_t y2,
                  const char *header, Option_t *option)
           : TLegend(x1, y1, x2, y2, header, option)
{
//	std::cout << "ctor HprLegend x1,y1, x2,y2" << x1 << " " 
//	            << y1 << " "<< x2 << " "  <<y2 << std::endl;
}

void   HprLegend::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	TLegendEntry *entry = GetEntry();
	if ( !entry ) return;
//	TRegexp sa("SAME");
//	entry->Print();
	if ( event == kButton1Down ) {
		TObject *obj = entry->GetObject();
		if (obj) {
//			TString dopt = obj->GetDrawOption();
//			dopt(sa) = "";
//			obj->SetDrawOption(dopt);
			obj->Pop();
// 			std::cout << "HprLegend pop " << obj << " gPad " << gPad << std::endl
// 			<< " An open Set2DimOptDialog may get invalid"  << std::endl;
			this->Pop();
			gPad->Modified();
			gPad->Update();
		}
	}
	TLegend::ExecuteEvent(event, px, py);
}
