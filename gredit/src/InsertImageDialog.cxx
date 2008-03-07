
#include "TROOT.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TGMsgBox.h"
#include "GEdit.h"
#include "HTPad.h"
#include "InsertImageDialog.h"
#include "HprImage.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertImageDialog)

InsertImageDialog::InsertImageDialog()
{
static const Char_t helptext[] =
"Insert a gif, jpg, png picture\n\
";
   gROOT->GetListOfCleanups()->Add(this);
   fCanvas = gPad->GetCanvas();
   fWindow = NULL;
   if (fCanvas)
      fWindow = (TRootCanvas*)fCanvas->GetCanvasImp();
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();

// make list of image files
   const char hist_file[] = {"images_hist.txt"};
   ofstream hfile(hist_file);
   const char *fname;
   void* dirp=gSystem->OpenDirectory(".");
   TRegexp dotGif = "\\.gif$";
   TRegexp dotJpg = "\\.jpg$";
   TRegexp dotPng = "\\.png$";
   Long_t id, size, flags, modtime;
   while ( (fname=gSystem->GetDirEntry(dirp)) ) {
      TString sname(fname);
      if (!sname.BeginsWith("temp_") &&
          (sname.Index(dotGif)>0 || sname.Index(dotJpg)>0
         || sname.Index(dotPng)>0)) {
         size = 0;
         gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
         if (size <= 0)
            cout << "Warning, empty file: " << fname << endl;
         else
            hfile << fname << endl;
         if (fPname.Length() < 1) fPname = fname;
      }
   }

   fRow_lab->Add(new TObjString("CheckButton_Use a new pad"));
   fValp[ind++] = &fNewPad;
   fRow_lab->Add(new TObjString("RadioButton+Preserve defined height"));
   fValp[ind++] = &fFix_h;
   fRow_lab->Add(new TObjString("RadioButton_Preserve defined width"));
   fValp[ind++] = &fFix_w;
   fRow_lab->Add(new TObjString("RadioButton+Preserve width and height"));
   fValp[ind++] = &fFix_wh;
   fRow_lab->Add(new TObjString("PlainIntVal_Offset X"));
   fValp[ind++] = &fOffset_x;
   fRow_lab->Add(new TObjString("PlainIntVal+Offset Y"));
   fValp[ind++] = &fOffset_y;


//   TImage *img = TImage::Open(name.Data());
   static TString execute_cmd("ExecuteInsert()");
   fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
   fValp[ind++] = &execute_cmd;
   Int_t itemwidth = 300;
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Insert Image", &fPname, &ok,
          itemwidth, fWindow, NULL, NULL,
          fRow_lab, fValp, NULL, NULL,
          helptext, this, this->ClassName());
}
//____________________________________________________________________________

void InsertImageDialog::ExecuteInsert()
{
   HTPad *pad = NULL;
   HprImage * hprimg = new HprImage(fPname.Data(), pad);
   TImage *img = hprimg->GetImage();
   if (!img) {
      cout << "Could not create an image... exit" << endl;
      return;
   }
   Double_t img_width = (Double_t )img->GetWidth();
   Double_t img_height = (Double_t )img->GetHeight();

   if (fNewPad) {
      pad = GEdit::GetEmptyPad();
//      pad = (HTPad*)gPad;
      if (pad) {
         gROOT->SetSelectedPad(pad);
         Double_t aspect_ratio = 1.;
         if ( img_width > 0 )
   		   aspect_ratio = img_height * fCanvas->GetXsizeReal()
                        		/ (img_width* fCanvas->GetYsizeReal());

   		if (fFix_w) {
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetWNDC(),
                  		pad->GetYlowNDC() + pad->GetWNDC() * aspect_ratio);
   		} else if (fFix_h) {
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetHNDC() / aspect_ratio,
                  		pad->GetYlowNDC() + pad->GetHNDC());
   		}

      } else {
         Int_t retval = 0;
         new TGMsgBox(gClient->GetRoot(), fWindow, "Warning",
             "Please create a new Pad in this Canvas",
             kMBIconExclamation, kMBDismiss, &retval);
         return;
      }
   }

   cout << "InsertImage(): " <<  gPad->GetXlowNDC() << " " << gPad->GetYlowNDC() << " "
        <<  gPad->GetWNDC()    << " " << gPad->GetHNDC()    << endl;
   cout << "Image size, X,Y: " << img_width
                        << " " << img_height << endl;
   img->SetConstRatio(kTRUE);
   img->SetEditable(kTRUE);
   img->SetImageQuality(TAttImage::kImgBest);
   TString drawopt;
   if (fOffset_x == 0 && fOffset_y == 0) {
      drawopt= "xxx";
   } else {
      drawopt = "T";
      drawopt += fOffset_x;
      drawopt += ",";
      drawopt += fOffset_x;
      drawopt += ",#ffffff";
   }
//   cout << drawopt << endl;
   hprimg->Draw(drawopt);
   gPad->Modified();
   gPad->Update();
};
//____________________________________________________________________________

void InsertImageDialog::SaveDefaults()
{
//   cout << "InsertImageDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertImageDialog.fNewPad", fNewPad);
   env.SetValue("InsertImageDialog.fFix_h", fFix_h);
   env.SetValue("InsertImageDialog.fFix_w", fFix_w);
   env.SetValue("InsertImageDialog.fFix_wh", fFix_wh);
   env.SetValue("InsertImageDialog.fOffset_x", fOffset_x);
   env.SetValue("InsertImageDialog.fOffset_y", fOffset_y);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertImageDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fNewPad = env.GetValue("InsertImageDialog.fNewPad", 1);
   fFix_h = env.GetValue("InsertImageDialog.fFix_h", 0);
   fFix_w = env.GetValue("InsertImageDialog.fFix_w", 1);
   fFix_wh = env.GetValue("InsertImageDialog.fFix_wh", 0);
   fOffset_x = env.GetValue("InsertImageDialog.fOffset_x", 0);
   fOffset_y = env.GetValue("InsertImageDialog.fOffset_y", 0);
}
//_________________________________________________________________________

InsertImageDialog::~InsertImageDialog()
{
   gROOT->GetListOfCleanups()->Remove(this);
   fRow_lab->Delete();
   delete fRow_lab;
};
//_______________________________________________________________________

void InsertImageDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "InsertImageDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void InsertImageDialog::CloseDialog()
{
//   cout << "InsertImageDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertImageDialog::CloseDown(Int_t wid)
{
//   cout << "InsertImageDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}

