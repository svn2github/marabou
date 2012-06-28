#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TColor.h"
#include "TOrdCollection.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "SetColorModeDialog.h"
#include "hprbase.h"
#include <iostream>
#include <iomanip>

namespace std {} using namespace std;

//TString SetColorModeDialog::fDrawOpt2Dim = "COLZ";
Color_t SetColorModeDialog::fStartColor;
Color_t SetColorModeDialog::fEndColor;

Int_t   SetColorModeDialog::fStartColorIndex = 1;
Int_t   SetColorModeDialog::fNofTransLevels = 20;
Int_t   SetColorModeDialog::fNofGreyLevels = 20;
Int_t * SetColorModeDialog::fPalette = NULL;
Int_t * SetColorModeDialog::fGreyPalette = NULL;
Int_t * SetColorModeDialog::fGreyPaletteInv = NULL;
Int_t * SetColorModeDialog::fTransPaletteRGB = NULL;
Int_t * SetColorModeDialog::fTransPaletteHLS = NULL;
Int_t   SetColorModeDialog::fNofColorLevels;
Float_t SetColorModeDialog::fEnhenceRed;
Float_t SetColorModeDialog::fEnhenceGreen;
Float_t SetColorModeDialog::fEnhenceBlue;
Int_t   SetColorModeDialog::fStartHue;
Int_t   SetColorModeDialog::fEndHue;
Float_t SetColorModeDialog::fLightness;
Float_t SetColorModeDialog::fSaturation;
Int_t   SetColorModeDialog::fMtransRGB;
Int_t   SetColorModeDialog::fMtransHLS;
Int_t   SetColorModeDialog::fMbw;
Int_t   SetColorModeDialog::fMbwinv;
Int_t   SetColorModeDialog::fMrbow;

ClassImp(SetColorModeDialog)
//__________________________________________________________________

SetColorModeDialog::SetColorModeDialog(TGWindow * win)
{
static const Char_t helptext[] =
"This determines which color palette should be used.\n\
Nice effects may be produced playing with RGB (Red/Green/Blue)\n\
or HLS (Hue/Lightness/Saturation) transitions\n\
";

   fRgbSlider = NULL;
   fHlsSlider = NULL;
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   fWindow = win;
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
//   static Int_t dummy;
   static TString stycmd("SetColorMode()");
   static TString hlscmd("SetHLS()");
   static TString rgbcmd("SetBrightness()");
   fRow_lab = new TList();

   fRow_lab->Add(new TObjString("RadioButton_      Rainbow colors"));
   fValp[ind++] = &fMrbow;
   fRow_lab->Add(new TObjString("PlainIntVal+Nof Levels"));
   fValp[ind++] = &fNofTransLevels;
   fRow_lab->Add(new TObjString("RadioButton_Grey lev, high=white"));
   fValp[ind++] = &fMbw;
   fRow_lab->Add(new TObjString("RadioButton+Grey lev, high=black)"));
   fValp[ind++] = &fMbwinv;
   fRow_lab->Add(new TObjString("RadioButton_Color transition HLS"));
   fValp[ind++] = &fMtransHLS;
   fRow_lab->Add(new TObjString("CommandButt+Set HLS"));
   fValp[ind++] = &hlscmd;
   fRow_lab->Add(new TObjString("RadioButton_Color transition RGB"));
   fValp[ind++] = &fMtransRGB;
   fRow_lab->Add(new TObjString("CommandButt+Set RGB"));
   fValp[ind++] = &rgbcmd;
	fRow_lab->Add(new TObjString("ColorSelect_Start color RGB"));
	fRow_lab->Add(new TObjString("ColorSelect+End   color RGB"));
   fValp[ind++] = &fStartColor;
   fValp[ind++] = &fEndColor;

   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;

   static Int_t ok;
   Int_t itemwidth = 420;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//______________________________________________________

void SetColorModeDialog::SetColorMode()
{
   if ( fMbw ) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPalette;
      fStartColorIndex = 301;
   } else if (fMbwinv) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPaletteInv;
      fStartColorIndex = 331;
   } else if (fMtransRGB) {
      fNofColorLevels = fNofTransLevels;
      fPalette = fTransPaletteRGB;
      fStartColorIndex = 361;
		SetTransLevelsRGB();
   } else if (fMtransHLS) {
      fNofColorLevels = fNofTransLevels;
      fPalette = fTransPaletteHLS;
      fStartColorIndex = 391;
   } else if (fMrbow) {
      fNofColorLevels = 50;
      fStartColorIndex = 51;
      fPalette = NULL;
   } else {
      fNofColorLevels = 50;
      fPalette = NULL;
      fStartColorIndex = 51;
   }
//   cout << "SetColorPalette: fStartColorIndex "
 //       <<  fStartColorIndex<< "fPalette  " << fPalette;
//         if (fPalette) cout << " " <<  fPalette[0]  << " "
//         <<  fPalette[1]  << " "   <<  fPalette[2];
//    cout   << endl;
   if ( fMrbow ) {
      gStyle->SetPalette(1, NULL);
	} else {
      gStyle->SetPalette(fNofColorLevels, fPalette);
	}
}
//__________________________________________________________________

void SetColorModeDialog::SetBrightness()
{
   TOrdCollection * lab = new TOrdCollection();
   lab->Add(new TObjString("red"));
   lab->Add(new TObjString("green"));
   lab->Add(new TObjString("blue"));
   static Int_t val[3];
   static Int_t max[3];
   static Int_t min[3];
   min[0] = 0;
   min[1] = 0;
   min[2] = 0;
   max[0] = 200;
   max[1] = 200;
   max[2] = 200;

   val[0] = (Int_t)(100 * SetColorModeDialog::fEnhenceRed);
   val[1] = (Int_t)(100 * SetColorModeDialog::fEnhenceGreen);
   val[2] = (Int_t)(100 * SetColorModeDialog::fEnhenceBlue);

   Int_t id  = 1;
   fRgbSlider = new TGMrbSliders("Set Brightness", 3, min, max, val, lab, NULL,
   fWindow, id);
   fRgbSlider->Connect("SliderEvent(Int_t, Int_t)",this->ClassName() , this,
               "AdjustBrightness(Int_t, Int_t)");
   fRgbSlider->Connect("SliderClosed(TObject*)",this->ClassName() , this,
               "SliderClosed(TObject*)");
}
//___________________________________________________________________

void SetColorModeDialog::AdjustBrightness(Int_t row , Int_t val)
{
//   cout << "AdjustBrightness: " << id << " " << row << " " << val << endl;
   switch (row) {
      case 0:
        SetColorModeDialog::fEnhenceRed   = (Float_t)val / 100.;
        break;
      case 1:
        SetColorModeDialog::fEnhenceGreen = (Float_t)val / 100.;
        break;
      case 2:
        SetColorModeDialog::fEnhenceBlue  = (Float_t)val / 100.;
        break;
      default:
         return;
   }
   cout << "AdjustBrightness: " <<SetColorModeDialog::fEnhenceRed
        << " " << SetColorModeDialog::fEnhenceGreen << " " << SetColorModeDialog::fEnhenceBlue << endl;
   SetColorModeDialog::SetTransLevelsRGB();
//   hp->SetColorPalette(mycanvas);
   UpdateCanvas();
}
//___________________________________________________________________

void SetColorModeDialog::SetHLS()
{
   const Int_t NVAL = 4;
   TOrdCollection * lab = new TOrdCollection();
   lab->Add(new TObjString("Hue Start"));
   lab->Add(new TObjString("Hue End"));
   lab->Add(new TObjString("Lightness"));
   lab->Add(new TObjString("Saturation"));
   Int_t val[NVAL];
   const Int_t max[NVAL] = {360, 360, 100, 100};
   Int_t min[NVAL] = {0, 0, 0, 0};
   Int_t flags[NVAL] = {1, 1, 0, 0};
   val[0] = (Int_t)SetColorModeDialog::fStartHue;
   val[1] = (Int_t)SetColorModeDialog::fEndHue;
   val[2] = (Int_t)(100 * SetColorModeDialog::fLightness);
   val[3] = (Int_t)(100 * SetColorModeDialog::fSaturation);
   const Int_t id  = 1;
   fHlsSlider = new TGMrbSliders("Set HLS", NVAL,
                    min, max, val, lab, flags,
                    fWindow, id);
   fHlsSlider->Connect("SliderEvent(Int_t, Int_t)",this->ClassName() , this,
               "AdjustHLS(Int_t, Int_t)");
   fHlsSlider->Connect("SliderClosed(TObject*)",this->ClassName() , this,
               "SliderClosed(TObject*)");
}
//___________________________________________________________________

void SetColorModeDialog::AdjustHLS(Int_t row , Int_t val)
{
//   cout << "AdjustBrightness: " << id << " " << row << " " << val << endl;
   switch (row) {
      case 0:
        SetColorModeDialog::fStartHue = val;
        break;
      case 1:
        SetColorModeDialog::fEndHue = val;
        break;
      case 2:
        SetColorModeDialog::fLightness  = (Float_t)val / 100.;
        break;
      case 3:
        SetColorModeDialog::fSaturation = (Float_t)val / 100.;
        break;
      default:
         return;
   }
//   cout << "AdjustBrightness: " <<SetColorModeDialog::fEnhenceRed
//        << " " << SetColorModeDialog::fEnhenceGreen << " " << SetColorModeDialog::fEnhenceBlue << endl;
   SetColorModeDialog::SetTransLevelsHLS();
   UpdateCanvas();
}
//_______________________________________________________________

void SetColorModeDialog::UpdateCanvas()
{
   if (fCanvas) {
		TIter next(fCanvas->GetListOfPrimitives());
		TObject *obj;
		while ( (obj = next()) ) {
			if ((obj->InheritsFrom("TPad"))) {
				TPad *pad = (TPad*)obj;
				pad->Modified();
				pad->Update();
			}
		}
      fCanvas->Modified();
      fCanvas->Update();
   }
}
//_______________________________________________________________

void SetColorModeDialog::SetGreyLevels()
{
   fGreyPalette    = new Int_t[fNofGreyLevels];
   fGreyPaletteInv = new Int_t[fNofGreyLevels];
//   cout << " SetColorModeDialog::SetGreyLevels() fGreyPalette: " <<  fGreyPalette
//        << " fGreyPaletteInv: " <<  fGreyPaletteInv  << endl;
   static Int_t mono = 0;
   TColor *color;
   if (mono == 0) {
      mono = 1;
      Float_t frac = 1 / (Float_t) fNofGreyLevels;
      for (Int_t i = 0; i < fNofGreyLevels; i++) {
         Float_t x = (Float_t) i;
         fStartColorIndex = 331;
         color =
             new TColor(fStartColorIndex + i, 1 - x * frac, 1 - x * frac, 1 - x * frac,
                        "");
         fGreyPaletteInv[i] = fStartColorIndex + i;

         fStartColorIndex = 301;
         color = new TColor(fStartColorIndex + i, x * frac, x * frac, x * frac, "");
         fGreyPalette[i] = fStartColorIndex + i;
      }
   }
//   cout << " SetColorModeDialog::fGreyPalette: " <<  fGreyPalette
//        << " fGreyPaletteInv: " <<  fGreyPaletteInv  << endl;
}
//___________________________________________________________________

void SetColorModeDialog::SetTransLevelsRGB()
{
   if (fNofTransLevels < 2) fNofTransLevels = 2;
   if (fTransPaletteRGB) delete [] fTransPaletteRGB;
   fTransPaletteRGB    = new Int_t[fNofTransLevels];
   TColor * color;
   TColor * sc = GetColorByIndex(fStartColor);
   TColor * ec = GetColorByIndex(fEndColor);
   Float_t start_r = sc->GetRed();
   Float_t step_r = (ec->GetRed() - start_r) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_r = 0;
   Float_t start_g = sc->GetGreen();
   Float_t step_g = (ec->GetGreen() - start_g) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_g = 0;
   Float_t start_b = sc->GetBlue();
   Float_t step_b = (ec->GetBlue() - start_b) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_b = 0;
//   Float_t nof2 = fNofTransLevels / 2.;
   fStartColorIndex = 361;
   for (Int_t i = 0; i < fNofTransLevels; i++) {
      color = GetColorByIndex(fStartColorIndex + i);
      if (color) delete color;
//      Float_t dc = 1 + 2. * (1. - 2. * TMath::Abs((i - nof2) / fNofTransLevels ));
      Float_t rgb[3];
      Float_t h, l, s;
      rgb[0] = start_r + frac_r;
      rgb[1] = start_g + frac_g;
      rgb[2] = start_b + frac_b;
      TColor::RGB2HLS(rgb[0], rgb[1], rgb[2], h, l, s);
//      cout << setprecision(3) << "rgb: " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " "
//           << "hls " << h << " " << l << " " << s << endl;
//    adjust to same lightness
//      Float_t max = TMath::MaxElement(3, rgb);
//      if (max > 0) max = 1. / max;
//      for (Int_t j = 0; j < 3; j++) rgb[j] *= max;
      rgb[0] *= fEnhenceRed;
      rgb[1] *= fEnhenceGreen;
      rgb[2] *= fEnhenceBlue;
//		cout << "i rgb fEnhenceRed " << rgb[0] << " "  
//		<< rgb[1] << " "  << rgb[2] << " " << fEnhenceRed << endl; 
//      for (Int_t jj = 0; jj < 1; jj++) {
         if (rgb[0] > 1.) {
            rgb[1] += 0.5 * (rgb[0] - 1.);
            rgb[2] += 0.5 * (rgb[0] - 1.);
            rgb[0] = 1.;
         } else {
//            rgb[0] = 0;
         }
         if (rgb[1] > 1.) {
            rgb[0] += 0.5 * (rgb[1] - 1.);
            rgb[2] += 0.5 * (rgb[1] - 1.);
            rgb[1] = 1.;
         }else {
//            rgb[1] = 0;
         }
         if (rgb[2] > 1.) {
            rgb[0] += 0.5 * (rgb[2] - 1.);
            rgb[1] += 0.5 * (rgb[2] - 1.);
            rgb[2] = 1.;
         }else {
//            rgb[2] = 0;
         }
//      }
//      cout << "o rgb " << rgb[0] << " "  << rgb[1] << " "  << rgb[2] << endl; 
		color = new TColor(fStartColorIndex + i, rgb[0], rgb[1], rgb[2],"");
//      color->Print();
      fTransPaletteRGB[i] = fStartColorIndex + i;
      frac_r += step_r;
      frac_g += step_g;
      frac_b += step_b;
   }
}
//___________________________________________________________________

void SetColorModeDialog::SetTransLevelsHLS()
{
   if (fNofTransLevels < 2) fNofTransLevels = 2;
   if (fTransPaletteHLS) delete [] fTransPaletteHLS;
   fTransPaletteHLS    = new Int_t[fNofTransLevels];
   TColor * color;

   Float_t step_h = (fEndHue - fStartHue) / (Float_t)(fNofTransLevels - 1);
   Float_t rgb[3];
   Float_t h, l, s;

   fStartColorIndex = 391;
   h = fStartHue;
   l = fLightness;
   s = fSaturation;

   for (Int_t i = 0; i < fNofTransLevels; i++) {
      color = GetColorByIndex(fStartColorIndex + i);
      if (color) delete color;

      TColor::HLS2RGB(h, l, s, rgb[0], rgb[1], rgb[2]);

      color = new TColor(fStartColorIndex + i, rgb[0], rgb[1], rgb[2],"");
//      cout << setprecision(3) << "rgb: " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " "
//           << "hls " << h << " " << l << " " << s << endl;
      fTransPaletteHLS[i] = fStartColorIndex + i;
      h += step_h;
   }
}
//___________________________________________________________________

void SetColorModeDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("SetColorModeDialog.fMtransRGB", fMtransRGB);
   env.SetValue("SetColorModeDialog.fMtransHLS", fMtransHLS);
   env.SetValue("SetColorModeDialog.fMbw", fMbw);
   env.SetValue("SetColorModeDialog.fMbwinv", fMbwinv);
   env.SetValue("SetColorModeDialog.fMrbow", fMrbow);
   env.SetValue("SetColorModeDialog.NofTransLevels", fNofTransLevels);
   env.SetValue("SetColorModeDialog.StartColor", fStartColor);
   env.SetValue("SetColorModeDialog.EndColor", fEndColor);
   env.SetValue("SetColorModeDialog.EnhenceRed",fEnhenceRed);
   env.SetValue("SetColorModeDialog.EnhenceGreen",fEnhenceGreen);
   env.SetValue("SetColorModeDialog.EnhenceBlue",fEnhenceBlue);
   env.SetValue("SetColorModeDialog.StartHue",fStartHue);
   env.SetValue("SetColorModeDialog.EndHue",fEndHue);
   env.SetValue("SetColorModeDialog.Lightness", fLightness);
   env.SetValue("SetColorModeDialog.Saturation",fSaturation);
   env.SaveLevel(kEnvLocal);
}

//___________________________________________________________________

void SetColorModeDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
//      cout << "SetColorModeDialog: RecursiveRemove "  << endl;
      CloseDialog();
   }
}
//___________________________________________________________________

void SetColorModeDialog::CloseDialog()
{
//   cout << "SetColorModeDialog::CloseDialog() "
//        << "fHlsSlider " << fHlsSlider <<" fRgbSlider "
//        <<  fRgbSlider << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fHlsSlider) {

      fHlsSlider->DeleteWindow();
   }
   if (fRgbSlider)
      fRgbSlider->DeleteWindow();
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//___________________________________________________________________

void SetColorModeDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fMtransRGB = env.GetValue("SetColorModeDialog.fMtransRGB", 0);
   fMtransHLS = env.GetValue("SetColorModeDialog.fMtransHLS", 0);
   fMbw = env.GetValue("SetColorModeDialog.fMbw", 0);
   fMbwinv = env.GetValue("SetColorModeDialog.fMbwinv", 0);
   fMrbow = env.GetValue("SetColorModeDialog.fMrbow", 1);
   fNofTransLevels   = env.GetValue("SetColorModeDialog.NofTransLevels", 20);
   fStartColor       = env.GetValue("SetColorModeDialog.StartColor", 2);
   fEndColor         = env.GetValue("SetColorModeDialog.EndColor", 3);
   fEnhenceRed       = env.GetValue("SetColorModeDialog.EnhenceRed", 1.);
   fEnhenceGreen     = env.GetValue("SetColorModeDialog.EnhenceGreen", 1.);
   fEnhenceBlue      = env.GetValue("SetColorModeDialog.EnhenceBlue", 1.);
   fStartHue         = env.GetValue("SetColorModeDialog.StartHue", 120);
   fEndHue           = env.GetValue("SetColorModeDialog.EndHue", 1);
   fLightness        = env.GetValue("SetColorModeDialog.Lightness", 0.5);
   fSaturation       = env.GetValue("SetColorModeDialog.Saturation ", 1.);
}
//__________________________________________________________________

void SetColorModeDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
   CloseDialog();
}
//___________________________________________________________________

void SetColorModeDialog::SliderClosed(TObject *obj)
{
   if (obj == fHlsSlider)
      fHlsSlider = NULL;
   if (obj == fRgbSlider)
      fRgbSlider = NULL;
//   cout << "SliderClosed " << obj << " " << fHlsSlider << endl;
}
//___________________________________________________________________

void SetColorModeDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << " SetColorModeDialog::CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   if ( fMrbow ) {
		TH1* hh = Hpr::FindHistInPad((TVirtualPad *)fCanvas);
		if (hh->GetDimension() == 2) {
			hh->SetContour(fNofTransLevels);
		}
	}
   SetColorMode();
   UpdateCanvas();
}

//___________________________________________________________________

TColor * SetColorModeDialog::GetColorByIndex(Int_t index) {
	return gROOT->GetColor(index);
/*   TIter next(gROOT->GetListOfColors());
   while (TColor * c = (TColor *)next()) {
      if (c->GetNumber() == index) {
         return c;
      }
   }
   return NULL;*/
}
