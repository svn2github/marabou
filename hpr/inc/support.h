#ifndef HPR_SUPPORT
#define HPR_SUPPORT
#include "TROOT.h"
#include "TEnv.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TColor.h"
#include "HTCanvas.h"
#include "TButton.h"
#include "TObject.h"
#include "TVirtualPad.h"
#include "TList.h"
#include "TText.h"
#include "TLatex.h"
#include "TH1.h"
#include "TString.h"
#include "TMapFile.h"
#include "TGWindow.h"
#include "HistPresent.h"
#include "TMrbStatistics.h"

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
extern Bool_t is_a_file(const char *);
extern Bool_t is_memory(const char *);
extern Int_t contains_filenames(const char *);
extern Int_t CheckList(TList *, const char * cname = "");
//extern Int_t  GetInteger(const char *, Int_t);
//extern Float_t GetFloat(const char *, Float_t);
extern TButton* CommandButton(TString &, TString &,
              Float_t, Float_t, Float_t, Float_t, Bool_t selected = 0); 
extern TString* GetTitleString(TObject* obj);
extern void     SelectButton(TString & , Float_t, Float_t, Float_t, Float_t,
                             Bool_t selected = 0);
HTCanvas*        CommandPanel(const char *, TList *,
                             Int_t xpos, Int_t ypos, HistPresent * hpr =0);
Int_t XBinNumber(TH1*, Axis_t);
Int_t YBinNumber(TH1*, Axis_t);
Stat_t Content(TH1*, Axis_t xlow, Axis_t xup, Stat_t * mean,  Stat_t * sigma);

//extern const char *OpenFileDialog();
//extern const char *SaveFileDialog();

void  WarnBox( const char * message, TGWindow* win = 0);
Bool_t  QuestionBox( const char * message, TGWindow* win = 0);
extern Bool_t OpenWorkFile(TGWindow* win = 0);
extern void CloseWorkFile(TGWindow* win = 0);
extern Int_t GetUsedSize(TMapFile *);
extern TList * GetWindows(TFile *);
extern TList * GetWindows(TMapFile *);
extern TList * GetFunctions(TMapFile *);
extern TList * GetFunctions(TFile *);
extern TList * GetCanvases(TFile *);
extern TList * GetTrees(TFile *);
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
TH1 * gethist( const char * hname, const char * host, Int_t socket, Bool_t * ok);
TMrbStatistics * getstat(const char * host, Int_t socket, Bool_t * ok);
TColor * GetColorByInd(Int_t index);   
void SetUserPalette(Int_t startindex, TArrayI * pixels); 

inline void nothing();
#endif
