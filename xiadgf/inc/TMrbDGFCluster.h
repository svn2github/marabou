#ifndef __TMrbDGFCluster_h__
#define __TMrbDGFCluster_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFCluster.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGFCluster            -- cluster data
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       $Id: TMrbDGFCluster.h,v 1.2 2005-05-10 16:54:46 marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////


#include "TROOT.h"
#include "TNamed.h"
#include "TEnv.h"

#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember
// Purpose:        Class defs describing module XIA DGF-4C
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFClusterMember : public TObject {

	public:

		TMrbDGFClusterMember(Int_t ClusterIndex = 0, Int_t ClusterNo = 0, Char_t CapsuleId = 'X') {
			fClusterIndex = ClusterIndex;
			fClusterNo = ClusterNo;
			fCapsuleId = CapsuleId;
			fHex = 0;
			fVoltage = 0;
			fColor = "";
			fAF = "";
			fSide = "";
			fIsRight = kFALSE;
			fHeight = "";
			fIsUp = kFALSE;
			fAngle = "";
			fIsForward = kFALSE;
			fCrate = 0;
			fSlot[0] = 0;
			fSlot[1] = 0;
			fDgf[0] = "";
			fDgf[1] = "";
		}
		virtual ~TMrbDGFClusterMember() {};

		inline Int_t GetClusterIndex() { return(fClusterIndex); };
		inline Int_t GetClusterNo() { return(fClusterNo); };
		inline Char_t GetCapsuleId() { return(fCapsuleId); };
		inline void SetHex(Int_t Hex) { fHex = Hex; };
		inline Int_t GetHex() { return(fHex); };
		inline void SetVoltage(Int_t Voltage) { fVoltage = Voltage; };
		inline Int_t GetVoltage() { return(fVoltage); };
		inline void SetColor(const Char_t * Color) { fColor = Color; fColor.ToLower(); };
		inline const Char_t * GetColor() { return(fColor.Data()); };
		inline void SetAF(const Char_t * AF) { fAF = AF; };
		inline const Char_t * GetAF() { return(fAF.Data()); };
		Bool_t SetSide(const Char_t * Side);
		inline const Char_t * GetSide() { return(fSide.Data()); };
		inline Bool_t IsRight() { return(fIsRight); };
		inline Bool_t IsLeft() { return(!fIsRight); };		
		Bool_t SetHeight(const Char_t * Height);
		inline const Char_t * GetHeight() { return(fHeight.Data()); };
		inline Bool_t IsUp() { return(fIsUp); };
		inline Bool_t IsDown() { return(!fIsUp); };		
		Bool_t SetAngle(const Char_t * Angle);
		inline const Char_t * GetAngle() { return(fAngle.Data()); };
		inline Bool_t IsForward() { return(fIsForward); };
		inline Bool_t IsBackward() { return(!fIsForward); };	
		inline void SetCrate(Int_t Crate) { fCrate = Crate; };
		inline Int_t GetCrate() { return(fCrate); };
		inline void SetSlot(Int_t SlotNo, Int_t Slot) { fSlot[SlotNo] = Slot; };
		inline Int_t GetSlot(Int_t SlotNo) { return(fSlot[SlotNo]); };
		inline void SetDgf(Int_t SlotNo, const Char_t * DgfName) { fDgf[SlotNo] = DgfName; };
		inline const Char_t * GetDgf(Int_t SlotNo) { return(fDgf[SlotNo].Data()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm);
		virtual inline void Print() { Print(cout); };
		
	protected:
		Int_t fClusterIndex;
		Int_t fClusterNo;
		Char_t fCapsuleId;
		Int_t fHex;
		Int_t fVoltage;
		TString fColor;
		TString fAF;
		TString fSide;
		Bool_t fIsRight;
		TString fHeight;
		Bool_t fIsUp;
		TString fAngle;
		Bool_t fIsForward;
		Int_t fCrate;
		Int_t fSlot[2];
		TString fDgf[2];
		
	ClassDef(TMrbDGFClusterMember, 1)		// [XIA DGF-4C] DGF cluster member
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFCluster
// Purpose:        Class defs describing module XIA DGF-4C
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFCluster : public TMrbNamedX {

	public:

		TMrbDGFCluster(Int_t ClusterIndex = 0, const Char_t * ClusterName = "Clu0") : TMrbNamedX(ClusterIndex, ClusterName) {
			fMembers.Delete();
		};
		virtual ~TMrbDGFCluster() {};

		inline Int_t GetClusterIndex() { return(this->GetIndex()); };
		inline void SetClusterNo(Int_t ClusterNo) { fClusterNo = ClusterNo; };
		inline Int_t GetClusterNo() { return(fClusterNo); };

		inline void AddMember(TMrbDGFClusterMember * CluMember) { fMembers.Add(CluMember); };
		inline TMrbDGFClusterMember * GetMember(Char_t SubNo) {
			if (fMembers.GetEntriesFast() > SubNo) return((TMrbDGFClusterMember *) fMembers[SubNo]); else return(NULL);
		}

		inline Int_t GetNofMembers() { return(fMembers.GetEntriesFast()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm);
		virtual inline void Print() { Print(cout); };
		
	protected:
		Int_t fClusterNo;
		TObjArray fMembers;

	ClassDef(TMrbDGFCluster, 1)		// [XIA DGF-4C] DGF cluster data
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFClusters
// Purpose:        Class defs describing module XIA DGF-4C
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofDGFClusters : public TMrbLofNamedX {

	public:
		TMrbLofDGFClusters(const Char_t * CluListName = "LofDGFClusters") : TMrbLofNamedX(CluListName) {};
		virtual ~TMrbLofDGFClusters() {};

	public:
		Int_t ReadFile(const Char_t * ClusterFile = "cluster.dat");

		inline Int_t GetNofClusters() { return(this->GetEntriesFast()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm);
		virtual inline void Print() { Print(cout); };
		
	ClassDef(TMrbLofDGFClusters, 1)		// [XIA DGF-4C] List of DGF clusters
};

#endif
