#include "HprGaxis.h"

ClassImp(HprGaxis)

HprGaxis::HprGaxis(Double_t xmin, Double_t ymin, Double_t xmax, Double_t ymax,
					Double_t wmin, Double_t wmax, Int_t ndiv , 
					Option_t* chopt, Double_t gridlength):
	TGaxis(xmin, ymin, xmax, ymax, wmin, wmax,ndiv, chopt,gridlength)
{
	fAxisOffset = 0;
	fOffset = 0;
	fRatio = 1;
	fWhere = 1;
};
