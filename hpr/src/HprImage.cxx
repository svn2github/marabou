#include "TPad.h"
#include "TSystem.h"
#include <fstream>
#include <iostream>

#include "HprImage.h"

namespace std {} using namespace std;

ClassImp(HprImage)
//_________________________________________________________________________
//
// This class puts a file into a TNamed object, this allows 
// the contents of tke file to written to a root file and retrieved by name
//_________________________________________________________________________
HprImage::HprImage(const Char_t * fname) :
         TNamed(gSystem->BaseName(fname), "") {

   Long_t id, size, flags, modtime;
//   cout << "norm ctor HprImage: " << fname << endl;
   gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
   if(size <= 0){
//      cout << "File: " << fname << " does not exist of empty" << endl;
      fIsGood = kFALSE;
      fBuffer = NULL;
      fImage = NULL;
   } else {
//      cout << "ctor HprImage: size " << size << endl;
      fImage = TImage::Open(fname);
      if (!fImage) {
         cout << "Could not create an image... exit" << endl;
         return;
      }
      fBuffer = new TArrayC(size);
//         Char_t * b = (Char_t *)&fBuffer[0];
      ifstream inf(fname);
      inf.read(fBuffer->fArray,size);
//         inf.close();
      fIsGood = kTRUE;
      gPad->Modified(kTRUE);
   }
};
//_________________________________________________________________________

Int_t HprImage::ToFile(const Char_t * fname){
//______________________________________________

// Write buffer contents to a file with name fname
//   cout << "HprImage::ToFile(): " << fname << endl;
   if(!fBuffer){
      cerr << "Buffer is empty"  << endl;
      return -1;
   }

   ofstream of(fname);
   if(of.fail()){
      cerr << "Cant open " << fname << endl;
      return -2;
   }
//      Char_t * b = (Char_t *)&fBuffer[0];
   of.write(fBuffer->fArray, fBuffer->GetSize());
   of.close();
   return fBuffer->GetSize();
}
//_________________________________________________________________________

void HprImage::Paint(Option_t * opt)
{
   TString name(GetName());
   name.Prepend("temp_");
//   cout << "HprImage::Paint(): " << name << endl;
//   Dump();
//   if (fImage) fImage->Dump();
   TObject *obj;
   TList * lop = gPad->GetListOfPrimitives();
   TObjOptLink *lnk = (TObjOptLink*)lop->FirstLink();
   while (lnk) {
      obj = lnk->GetObject();
      if (obj->InheritsFrom("TASImage")) lop->Remove(obj);
      lnk = (TObjOptLink*)lnk->Next();
   }

   if (!fImage || !fIsGood) {
      ToFile(name.Data());
      fImage = TImage::Open(name.Data());
      if (!fImage) {
         cout << "Could not create an image... exit" << endl;
         return;
      }
   }
   fImage->SetConstRatio(kTRUE);
   fImage->SetEditable(kTRUE);
   fImage->SetImageQuality(TAttImage::kImgBest);
   fImage->Draw("xxx");
   gPad->Modified(kTRUE);
}
