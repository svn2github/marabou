#ifndef __HPRGAXIS_h__
#define __HPRGAXIS_h__
#include "TGaxis.h"

namespace std {} using namespace std;

//////////////////////////////////////////////////////////////////////////////
// Name:           HprGaxis
// Description:    a TGaxis, which remembers offset and ratio wr to original
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class  HprGaxis : public TGaxis {
   protected:
      Double_t fOffset;          // offset of low edge compared to main axis
      Double_t fRatio;           // ratio of axis scales 
		Double_t fAxisOffset;      // offset to right/ upper frame boundary (0-1.)
		Int_t    fWhere;				// 1: X axis, 2: Y, 3: Z
   public:
		HprGaxis(Double_t xmin, Double_t ymin, Double_t xmax, Double_t ymax,
					Double_t wmin, Double_t wmax, Int_t ndiv = 510, 
					Option_t* chopt = "", Double_t gridlength = 0);
      HprGaxis() {};
		void SetOffset(Double_t off) { fOffset = off; };           // *MENU*
		Double_t GetOffset() { return fOffset; };
		void SetAxisOffset(Double_t off) { fAxisOffset = off; };   // *MENU*
		Double_t GetAxisOffset() { return fAxisOffset; };
		void SetRatio(Double_t ratio) {fRatio = ratio; };           // *MENU*
		Double_t GetRatio() {return fRatio; };
		void SetWhere(Double_t where) {fWhere = where; };
		Double_t GetWhere() {return fWhere; };
		
ClassDef(HprGaxis, 1)
};
#endif