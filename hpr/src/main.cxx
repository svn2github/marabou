#include "TROOT.h"
#include "TRint.h"
#include "TEnv.h"
#include <iostream>
#include "HistPresent.h"
#include "SetColor.h"

int main(int argc, char **argv)
{
   gSystem->Load("libHistPainter.so");
   HistPresent *hp;
   TRint *theApp = new TRint("App", &argc, argv);
   TDatime dt;
   
//   gEnv->SetValue("Gui.IconPath", "$MARABOU/icons");
   cout << "Starting HistPresent at: " << dt.AsSQLString()<< endl;
   hp = new HistPresent("mypres","mypres");
   hp->ShowMain();
   if(!gSystem->AccessPathName(attrname, kFileExists))
      gROOT->LoadMacro(attrname);
//loop:
   theApp->Run(kTRUE);
	gSystem->Exit(0);
}

