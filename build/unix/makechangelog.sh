#! /bin/sh

CVS2CL=build/unix/cvs2cl.pl

VERSION=`date +%b-%Y`

echo ""
echo "Generating cvslog/ChangeLog from CVS logs..."
echo ""

$CVS2CL -f cvslog/ChangeLog -W 10 -P -S --no-wrap

rm -f cvslog/ChangeLog.bak

root -b <<makedoc
    THtml html;
    TString odir = gEnv.GetValue("Root.Html.OutputDir", "");
//	gSystem.Load("libTMrbUtils.so");
//    gSystem.Load("libTMbsControl.so");
//    gSystem.Load("libTMrbConfig.so");
//    gSystem.Load("libTMrbTransport.so");
//    gSystem.Load("libTMbsSetup.so");
//    gSystem.Load("libTMrbEsone.so");
//    gSystem.Load("libTMrbDGF.so");
//    gSystem.Load("libTGMrbUtils.so");
//    gSystem.Load("libTMrbHelpBrowser.so");
//    gSystem.Load("libMutex.so");
//    gSystem.Load("libTMrbAnalyze.so");
//    gSystem.Load("libDGFControl.so");
//    gSystem.Load("libTSnkDDA0816.so");
//    gSystem.Load("libTPolControl.so");
    html.Convert("cvslog/ChangeLog", "MARaBOU Release Notes ($VERSION)", odir.Data());
    .q
makedoc

exit 0
