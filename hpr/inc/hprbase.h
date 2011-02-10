#ifndef HPRBASE
#define HPRBASE
#include "TCanvas.h"
#include "TGWindow.h"
#include "TH1.h"
#include "TGraph.h"
#include "TGraph.h"
#include "HprGaxis.h"

namespace Hpr
{
   void WriteHistasASCII(TH1 *hist, TGWindow *window = NULL);
   void WarnBox(const char *, TGWindow *window = NULL);
   Bool_t QuestionBox(const char *message, TGWindow *window);
   TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t pop_push);
	TH1 * FindHistInPad(TVirtualPad * ca);
	TGraph * FindGraphInPad(TVirtualPad * ca);
	Bool_t HistLimitsMatch(TH1* h1, TH1* h2);
	HprGaxis * DoAddAxis(TCanvas * canvas, TH1 *hist, Int_t where, 
			Double_t ledge, Double_t uedge, Double_t axis_offset=0, Color_t col=0);
}
#endif
