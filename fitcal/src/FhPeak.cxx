#include "FhPeak.h"
ClassImp(FhPeak)

FhPeak::FhPeak(Double_t pos) : fEstimate(pos), fMean(pos){
   fNominalEnergy = 0;
   fCalibratedEnergy = 0;
   fContent = 0;
   fWidth = 0;
   fMeanError = 0;
   fNominalEnergyError = 0;
   fContentError = 0;
   fWidthError =0;
   fChi2oNdf = 0;
   fTailContent = 0;
   fTailWidth = 0;
   fUsed =kFALSE;
};
void FhPeak::Print(Option_t * Option) const {
   printf("%10.2f %10.2f %10.2f %10.2f %10.2f", 
          fContent, fMean, fMeanError, fWidth, fChi2oNdf);
//    cout << setw(12)<< fContent << setw(12)<< fMean 
//         << setw(12)<< fMeanError << setw(12)<< fChi2oNdf;
    if (0) {
       if (fUsed) cout << "  yes";
       else       cout << "   no";
    } 
    cout << endl;
};
void FhPeak::PrintHeadLine(){
   cout << "   Content" << "       Mean" 
        << "  MeanError" << "      Sigma" << " Chi2 / Ndf";
   if (0) {
       cout << " Used";
   }
   cout << endl;
}

Int_t FhPeak::Compare(const TObject *obj) const { 
//   Double_t mthis = GetMean();
//   Double_t mobj  = (( FhPeak*)obj)->GetMean();
//   cout << " mthis,  mobj" <<  mthis " " <<  mobj << endl;
   if(GetMean() == ((FhPeak *)obj)->GetMean()) return  0; 
   if(GetMean() <  ((FhPeak *)obj)->GetMean()) return -1;
   else                                       return +1;
};
