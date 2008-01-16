#include "TPad.h"
#include "TSystem.h"
#include <fstream>

#include "HprImage.h"

ClassImp(HprImage)
//_________________________________________________________________________
//
// This class puts a file into a TNamed object, this allows
// the contents of the file to written to a root file and retrieved by name
//_________________________________________________________________________
HprImage::HprImage(const Char_t * fname, TPad * pad) :
          TNamed(gSystem->BaseName(fname), ""), fPad(pad){

   Long_t id, size, flags, modtime;
//   cout << "norm ctor HprImage: " << fname << endl;
   gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
   if(size <= 0){
//      cout << "File: " << fname << " does not exist of empty" << endl;
      fIsGood = kFALSE;
      fBuffer = NULL;
      fImage = NULL;
   } else {
      cout << "ctor HprImage: size " << size << endl;
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
//   fPad = pad;
   gROOT->GetListOfCleanups()->Add(this);
};
//______________________________________________

HprImage::~HprImage()
{
//   if (fPad) {
//      delete fPad;
//   }
   cout << "HprImage:: dtor" << endl;
   fPad = 0;
   if(fBuffer)delete fBuffer;
   fBuffer = 0;
};
//______________________________________________

void HprImage:: RecursiveRemove(TObject * obj) {
//   cout << "HprImage::RecursiveRemove: "
//        << this << " " << fPad << " " << fImage << " " << obj <<  endl;
   if (obj == fImage) {
      gROOT->GetListOfCleanups()->Remove(this);
      if (fPad) {
        fPad->GetListOfPrimitives()->Remove(this);
//        fPad->GetListOfPrimitives()->Delete("slow");
      }
//      cout << "HprImage::RecursiveRemove: delete this " <<  endl;
      delete this;
   }
};
//_________________________________________________________________________

Int_t HprImage::ToFile(const Char_t * fname){

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
   if (opt);
   if (GetVisibility() == 0)
      return;
   TString name(GetName());
   name.Prepend("temp_");
//   cout << "HprImage::Paint(): " << name << endl;
   TList * lop = gPad->GetListOfPrimitives();
   TObject *obj;
   TIter next_img(lop);
   while ( (obj = next_img()) ) {
      if (obj->InheritsFrom("TASImage")) {
 //        cout << "GetHeight(): " << ((TImage*)obj)->GetHeight() << endl;
         if (((TImage*)obj)->GetHeight() > 5) return;
      }
   }
//   cout << "HprImage::Paint(): " << name << endl;
//   Dump();
//   if (fImage) fImage->Dump();
   TObjOptLink *lnk = (TObjOptLink*)lop->FirstLink();
   cout << "HprImage::Paint(): bef Remove TASImage" << endl;
   while (lnk) {
      obj = lnk->GetObject();
      if (obj->InheritsFrom("TASImage")) lop->Remove(lnk);
      lnk = (TObjOptLink*)lnk->Next();
   }
   cout << "HprImage::Paint(): after Remove TASImage" << endl;

   lnk = (TObjOptLink*)lop->FirstLink();
   while (lnk) {
      obj = lnk->GetObject();
      if (obj->InheritsFrom("TFrame")) lop->Remove(lnk);
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
//   lop->ls();
// make sure image is drawn first
   TIter next(lop);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TASImage")) {
         break;
      } else {
         if (!obj->InheritsFrom("HprImage")) {
//            cout << "Pop " << obj << endl;
            obj->Pop();
         }
      }
   }
//   lop->ls();
   gPad->Modified(kTRUE);

}
