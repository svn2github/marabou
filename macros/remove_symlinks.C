#include "TSystem.h"
#include "TString.h"
#include "Riostream.h"

Int_t remove_symlinks(Bool_t real_remove = kFALSE)
{
   const char * fn;
   char buf[256];
   TString lname;
   TString target;
   Int_t nmoved =0;
   Int_t stat; 
   Long64_t size;
   Long_t id, flags, modtime;
   TString rsuf(".root");
   TRegexp version("\.[0-9]$", kFALSE);
   void * dirp=gSystem->OpenDirectory(".");
   while ( (fn = gSystem->GetDirEntry(dirp)) ) {
      lname = fn;
      if (lname.EndsWith(rsuf)) {
         stat = gSystem->GetPathInfo(fn, &id, &size, &flags, &modtime);
         if (stat != 0) {
            cout << "Cant stat: " << fn << endl;
            continue;
         } else {
            TString cmd(" [ -h ");
            cmd += lname;
            cmd += " ] ";
            stat = gSystem->Exec(cmd.Data());
            if (stat == 0) {
               cout << "Symbolic link found: " << fn;
               cmd = "readlink ";
               cmd += fn;
               FILE *fp = gSystem->OpenPipe(cmd.Data(), "r");
               stat = fgets(buf,256,fp);
               if (stat != 0) {
                  target = buf;
                  target.Resize(target.Length()-1);  //chop off the \0
                  cout << " -> |" << target <<"|" << endl;
//                does it end with something like .123 and points to cwd?
                  if (target.Index(version) > 0 && target.Index("/") < 0) {
                     cout << "has ser nr and points to cwd" << endl;
                     if (real_remove) {
                        cmd = "rm ";
                        cmd += fn;
                        stat = gSystem->Exec(cmd.Data());
                        if (stat == 0) {
                           cout << "removed: " << fn << endl;
                           cmd = "mv ";
                           cmd += target;
                           cmd += " ";
                           cmd += fn;
                           stat = gSystem->Exec(cmd.Data());
                           if (stat == 0) {
                              cout << "moved: " << target << " -> " << fn << endl;
                              nmoved++;
                           } else {
                              cout << "moving: " << target << " -> " << fn<< " failed" << end;
                           }
                        } else {
                           cout << "removing: " << fn << " failed" << end;;
                        }
                     }
                  }
               } else {
                  cout << "readlink failed" << endl;
               }
               gSystem->ClosePipe(fp);
            }
         }
      }  
   }
   return nmoved;
}
