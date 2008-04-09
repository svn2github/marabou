#ifndef HPRBASE
#define HPRBASE
#include "TGWindow.h"
#include "TH1.h"

namespace Hpr
{
   void WriteHistasASCII(TH1 *hist, TGWindow *window = NULL);
   void WarnBox(const char *, TGWindow *window = NULL);
   Bool_t QuestionBox(const char *message, TGWindow *window);
   TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t pop_push);
}
#endif