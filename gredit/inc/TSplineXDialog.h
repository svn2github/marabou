#ifndef TSPLINEXDIALOG
#define TSPLINEXDIALOG
#include "Gtypes.h"
#include "TGWindow.h"
#include "TObject.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class TSplineXDialog : public TObject {

private:
   TString   fCommand;
	Int_t    fClosed;  
	Int_t    fApprox; 
	Int_t 	fFixends; 
	Double_t fPrec;    
	Int_t    fShowcp;  
	Color_t  fColor;   
	Width_t  fLwidth;  
	Style_t  fLstyle;  
	Color_t  fFcolor;  
	Style_t  fFstyle;  

	Double_t fFilled;		  
	Double_t fEmpty; 		  
	Double_t fGage;  		  
	Int_t    fArrow_at_start;
	Int_t    fArrow_at_end;  
	Int_t    fArrow_filled;  
	Double_t fArrow_size;    
	Double_t fArrow_angle;   
	Double_t fArrow_indent_angle;
public:
   TSplineXDialog();
   virtual ~TSplineXDialog() {cout << "dtor:~TSplineXDialog()" << endl;};
   void Draw_The_TSplineX();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();
   void CRButtonPressed(){};

ClassDef(TSplineXDialog,0)
};
#endif
