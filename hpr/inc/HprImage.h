#include "TROOT.h"
#include "TArrayC.h"
#include "TASImage.h"
#include <iostream>

class HprImage : public TNamed {
private:
   TArrayC * fBuffer;
   TImage * fImage;
   Bool_t fIsGood; //! dont stream

public:

   HprImage(){fImage = 0; fBuffer = 0; fIsGood = kFALSE;};
   HprImage(const Char_t * fname);
   virtual ~HprImage(){if(fBuffer)delete fBuffer;};
   TImage * GetImage() {return fImage;};
   Bool_t IsGood(){return fIsGood;};
   void Paint(Option_t * opt = 0);
//
   Int_t ToFile(const Char_t * fname);
   ClassDef(HprImage,1)   // A TNamed object containing a sequence of bytes
};
