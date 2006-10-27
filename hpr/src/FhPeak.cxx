#include "FhPeak.h"
#include "FhRegion.h"

ClassImp(FhRegion)
ClassImp(FhPeak)

FhRegion::FhRegion(Double_t low, Double_t up) : fLowEdge(low), fUpperEdge(up){
   fNpeaks=0;
   fPeaks = new TList();
};

FhPeak::FhPeak(Double_t pos) : fEstimate(pos), fMean(pos){
   fNominalEnergy = 0;
   fCalibratedEnergy = 0;
   fContent = 0;
   fWidth = 0;
   fTailContent = 0;
   fTailWidth = 0;
   fRegion = 0;
   fUsed =kFALSE;
};
void FhPeak::Print(){
    cout << setw(12)<< fContent << setw(12)<< fMean 
         << setw(12)<< fWidth << setw(12)<< fNominalEnergy;
    if (fUsed) cout << "  yes";
    else       cout << "   no";
    cout << endl;
};
void FhPeak::PrintHeadLine(){
    cout << "     Content" << "        Mean" 
         << "       Error" << "     Nom Val" << " Used"<< endl;
}

Int_t FhPeak::Compare(const TObject *obj) const { 
//   Double_t mthis = GetMean();
//   Double_t mobj  = (( FhPeak*)obj)->GetMean();
//   cout << " mthis,  mobj" <<  mthis " " <<  mobj << endl;
   if(GetMean() == ((FhPeak *)obj)->GetMean()) return  0; 
   if(GetMean() <  ((FhPeak *)obj)->GetMean()) return -1;
   else                                       return +1;
};
