#ifndef __TMrbStatistics_h__
#define __TMrbStatistics_h__

using namespace std;

#include <iostream>
#include <iomanip>
#include "TROOT.h"
#include "Rtypes.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TKey.h"
#include "TList.h"
#include "TNamed.h"
#include "TFile.h"
#include "TMapFile.h"

class TMrbStatEntry: public TNamed {
public:
   TMrbStatEntry();

   TMrbStatEntry(TH1*, const char *, const char *);

   ~TMrbStatEntry(){};

   void Set(const Stat_t ent, const Stat_t sofw, const Stat_t mx,
            const Stat_t sx);                  // set statistics values for 1-dim
   void Set(const Stat_t ent, const Stat_t sofw); // set statistics values for 2-dim
   Int_t Get(Stat_t *ent, Stat_t *sofw, Stat_t *mx, Stat_t *sx);
   Int_t Get(Stat_t *ent, Stat_t *sofw);
   inline Stat_t GetEntries(){return fEntries;}
   inline Stat_t GetSumofw(){return fSumofw;}
   inline Stat_t GetMean(){return fMean;}
   inline Stat_t GetSigma(){return fSigma;}   
   TH1* GetHist(){return fHist;}
   Int_t GetDimension(){return fDim;};        // Get histogram dimension  
   virtual void Print(ostream & ostr = cout);                      // print out one line

protected:
   TH1 * fHist;            //! pointer to histogram, dont stream out hists
   Int_t fDim;             // Dimension of histogram
   Int_t fNofUpdates;      // Number of updates
   Stat_t fEntries;        // Entries
   Stat_t fSumofw;         // Sum of Weights, contents without under/overflows
   Stat_t fMean;           // Mean (along X for 2 dim)
   Stat_t fSigma;          // RMS (root mean squared)
	ClassDef(TMrbStatEntry, 1)
};


class TMrbStatCheck: public TNamed {
public:
   TMrbStatCheck();
   TMrbStatCheck(const char *, const char *);
   ~TMrbStatCheck(){};
   Bool_t InsideMean(Stat_t val);
   Bool_t InsideSigma(Stat_t val);
public:
   Bool_t fAnyCheck;
   Stat_t fEntriesLow;        // Entries
   Stat_t fSumofwLow;         // Sum of Weights, contents without under/overflows
   Stat_t fMeanLow;           // Mean (along X for 2 dim)
   Stat_t fSigmaLow;          // RMS (root mean squared)
   Stat_t fEntriesUp;         // Entries
   Stat_t fSumofwUp;          // Sum of Weights, contents without under/overfUps
   Stat_t fMeanUp;            // Mean (along X for 2 dim)
   Stat_t fSigmaUp;           // RMS (root mean squared)
	ClassDef(TMrbStatCheck, 1)	// [Utils] Check histogram statistics
};

class TMrbStatistics: public TNamed {
public:
   TMrbStatistics(){fStatEntries=new TList(this);};

   TMrbStatistics(const char *);

   ~TMrbStatistics();

   Int_t Init();              // find hists and initialize table    
   Int_t Update();            // go through list of hists and update values
   Int_t Fill();              // fill stats for hists in memory
   Int_t Fill(TFile *);       // fill stats for hists in root file
   Int_t Fill(TMapFile *);    // fill stats for hists in map file
   TList * GetListOfEntries(){return fStatEntries;};
   virtual void Print(ostream & ostr = cout);

protected:
   TList  *fStatEntries;

	ClassDef(TMrbStatistics, 1)	// [Utils] Histogram statistics
};
#endif
