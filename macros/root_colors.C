
//___________________________________________________________________________

TColor * GetColorByInd(Int_t index) {   
   TIter next(gROOT->GetListOfColors());
   TColor * c;
   while (c = (TColor *)next()) {
      if (c->GetNumber() == index) {
         return c;
      }
   }
   return NULL;
}
//___________________________________________________________________________
  
void root_colors() 
{
//   TString hexcol;
   TString scol;
//   TString cmd;
   new TCanvas("colors", "rgb colors", 400, 20, 800, 400);
   Float_t dx = 1./10.2 , dy = 1./10.2 ;
   Float_t x0 = 0.1 * dx,  y0 =0.1 *  dy; 
   Float_t x = x0, y = y0;;
   TButton * b;
   TColor  * c;
   Int_t maxcolors = 100;
   Int_t basecolor = 1;
   Int_t colindex = basecolor;
//   Int_t palette = new Int_t[maxcolors];
   for (Int_t i=basecolor; i<= maxcolors; i++) {
      scol = Form("%d", colindex);
      b = new TButton(scol.Data(), scol.Data(), x, y, x + .9*dx , y + .9*dy );
      b->SetFillColor(colindex);
      b->SetTextAlign(22);
      b->SetTextFont(100);
      b->SetTextSize(0.8);
      c = GetColorByInd(colindex);
      if (c) {
         if ( c->GetRed() + c->GetBlue() + c->GetGreen() < 1.5 ) b->SetTextColor(0);
         else                   b->SetTextColor(1);
      } else {
         cout << "color not found " << colindex << endl;
      }
      if ( (colindex++ >= maxcolors + basecolor) ) {
         cout << "Too many colors " << maxcolors << endl;
         break;
      }
      b->Draw();
      y += dy;
      if ( y >= 1 - dy ){
         y = y0;
         x+= dx;
      }
   }
}
