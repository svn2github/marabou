#include "TROOT.h"
#include "TRint.h"
#include "TGMsgBox.h"
#include "TMapFile.h"
// #include "TkInterface.h"
#include <iostream.h>
#include "HistPresent.h"
#include "FitHist.h"
#include "support.h"

int main(int argc, char **argv)
{
   HistPresent *hp;
   TRint *theApp = new TRint("App", &argc, argv);
   hp=new HistPresent("mypres","mypres");
   hp->ShowMain();
   if(!gSystem->AccessPathName(attrname, kFileExists))
      gROOT->LoadMacro(attrname);

//loop:
   theApp->Run(kTRUE);
   delete hp;
   return 0;
}

