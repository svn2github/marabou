#ifndef HPR_SUPPORT
#define HPR_SUPPORT
#include "TROOT.h"
#include "TEnv.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TColor.h"
#include "HTCanvas.h"
#include "TButton.h"
#include "TSocket.h"
#include "TObject.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TList.h"
#include "TText.h"
#include "TLatex.h"
#include "TH1.h"
#include "TString.h"
#include "TMapFile.h"
#include "TGWindow.h"
#include "TGraphErrors.h"
#include "HistPresent.h"
#include "TMrbStatistics.h"

namespace std {} using namespace std;


const Int_t kSelected = BIT(16), kCommand  = BIT(17), kSelection  = BIT(18);
extern int  GetPosition(TVirtualPad*, UInt_t *, UInt_t *);
extern TButton * CButton(const char* , const char*, TObject* , 
                    int, int, float, float dx=0.1);
extern TButton * SButton(const char* , const char*, TObject* , 
                    int, int, float, int, float dx=0.1);
extern void HButton(const char* , TObject*, int, int, float, float dx=0.1 );
extern TLatex * GetPadLatex(TButton *);
extern Bool_t is_a_function(TObject *);
extern Bool_t is_a_hist(TObject *);
extern Bool_t is_a_list(TObject *);
extern Bool_t is_a_cut(TObject *);
extern Bool_t is_a_tree(TObject *);
extern Bool_t is_a_window(TObject *);
extern Bool_t is2dim(TH1 *);
extern Bool_t is3dim(TH1 *);
extern Bool_t is_a_file(const char *);
extern Bool_t is_memory(const char *);
extern Int_t contains_filenames(const char *);
extern Int_t CheckList(TList *, const char * cname = "");
extern TButton* CommandButton(TString &, TString &,
              Float_t, Float_t, Float_t, Float_t, Bool_t selected = 0); 
extern TString* GetTitleString(TObject* obj);
extern void     SelectButton(TString & , Float_t, Float_t, Float_t, Float_t,
                             Bool_t selected = 0);
HTCanvas*       CommandPanel(const char *, TList *, Int_t xpos, Int_t ypos,
                              HistPresent * hpr =0, Int_t xwid =0);
Int_t XBinNumber(TH1*, Axis_t);
Int_t YBinNumber(TH1*, Axis_t);
Stat_t Content(TH1*, Axis_t xlow, Axis_t xup, Stat_t * mean,  Stat_t * sigma);

void  WarnBox( const char * message, TGWindow* win = 0);
Bool_t  QuestionBox( const char * message, TGWindow* win = 0);
extern Bool_t OpenWorkFile(TGWindow* win = 0);
extern void CloseWorkFile(TGWindow* win = 0);
extern Int_t GetUsedSize(TMapFile *);
extern Int_t GetObjects(TList &, TFile *, const char *);

extern TH1 * GetTheHist(TVirtualPad * pad);

void Show_Fonts();
void ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, Int_t mode,
                       TGWindow* win = 0);
void CalibrateAllAsSelected(TVirtualPad * pad, TCanvas * canvas, Int_t mode);
void RebinAll(TVirtualPad * pad, TCanvas * canvas, Int_t mode);
void Canvas2LP(TCanvas * canvas, const Char_t * opt = 0, TGWindow * = 0,
               Bool_t autops = kFALSE);
void  Canvas2RootFile(TCanvas * canvas, TGWindow * win = 0);
TEnv * GetDefaults(TString &, Bool_t mustexist = kTRUE);
TH1 * gethist( const char * hname, TSocket * sock);
TMrbStatistics * getstat(TSocket * sock);
TColor * GetColorByInd(Int_t index);   
void SetUserPalette(Int_t startindex, TArrayI * pixels); 
void AdjustMaximum(TH1 * h2, TArrayD * xyvals);
Int_t DeleteOnFile(const char * fname, TList * list, TGWindow * win =0);
TH1 * calhist(TH1 * hist, TF1 * calfunc,
              Int_t  nbin_cal, Axis_t low_cal, Axis_t binw_cal,
              const char * origname =0);
void PrintGraph(TGraphErrors * gr); 
Bool_t IsInsideFrame(TCanvas * c, Int_t px, Int_t py);
TGraph * FindGraph(TCanvas * ca);
void DrawColors();
void DrawFillStyles();
void DrawLineStyles();

inline void nothing();
#endif
