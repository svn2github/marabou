#include "TROOT.h"
#include "TRint.h"
#include "TEnv.h"
#include <iostream>
#include "HistPresent.h"
static HistPresent *mypres = NULL;

int main(int argc, char **argv)
{
   gSystem->Load("libHistPainter.so");
   HistPresent *hp;
   TRint *theApp = new TRint("App", &argc, argv);
//   gEnv->SetValue("Gui.IconPath", "$MARABOU/icons");
   hp = new HistPresent("mypres","mypres");
//   TString cmd("HistPresent *mypres = (HistPresent*)");
//   cmd += Form("0x%x", hp);
//   cout << cmd << endl;
//   gROOT->ProcessLine(cmd);
   hp->ShowMain();
   if(!gSystem->AccessPathName(attrname, kFileExists))
      gROOT->LoadMacro(attrname);
//loop:
   theApp->Run(kTRUE);
   delete hp;
   return 0;
}

