#ifndef __TMrbDGFCluster_h__
#define __TMrbDGFCluster_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFCluster.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGFCluster            -- cluster data
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       $Id: TMrbDGFCluster.h,v 1.3 2006-10-09 10:30:35 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////


#include "TROOT.h"
#include "TNamed.h"
#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

enum	EMrbDGFClusterIdx	{	kMrbDGFClusterIdx = 0,
								kMrbDGFClusterNo,
								kMrbDGFClusterCapsId,
								kMrbDGFClusterHex,
								kMrbDGFClusterVoltage,
								kMrbDGFClusterColor,
								kMrbDGFClusterAF,
								kMrbDGFClusterSide,
								kMrbDGFClusterHeight,
								kMrbDGFClusterAngle,
								kMrbDGFClusterCrate
							};

enum	{	kMrbDgfClusterNofItems = kMrbDGFClusterCrate+ 1 };
enum	{	kMrbDGFClusterNofModules = 	4	};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFClusterMember
// Purpose:        Class defs describing module XIA DGF-4C
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFClusterMember : public TObject {

	public:

		TMrbDGFClusterMember(Int_t ClusterIndex = 0, Int_t ClusterNo = 0, Char_t CapsuleId = 'X');
		virtual ~TMrbDGFClusterMember() {};

		inline void SetClusterIndex(Int_t CluIndex) { fClusterIndex = CluIndex; };
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

		inline void SetNofModules(Int_t NofModules) { fNofModules = NofModules; };
		inline Int_t GetNofModules() { return(fNofModules); };

		inline void SetCmt(const Char_t * Cmt) { fCmt = Cmt; };
		inline const Char_t * GetCmt() { return(fCmt.Data()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, Bool_t CmtFlag = kFALSE);
		virtual inline void Print(Bool_t CmtFlag) { Print(cout, CmtFlag); };
		
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
		Int_t fNofModules;
		Int_t fSlot[kMrbDGFClusterNofModules];
		TString fDgf[kMrbDGFClusterNofModules];
		TString fCmt;
		
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
		enum	{	kMrbXiaChansPerModule	=	4	};

	public:

		TMrbDGFCluster(Int_t ClusterIndex = 0, const Char_t * ClusterName = "Clu0");
		virtual ~TMrbDGFCluster() {};

		inline Int_t GetClusterIndex() { return(this->GetIndex()); };
		inline void SetClusterNo(Int_t ClusterNo) { fClusterNo = ClusterNo; };
		inline Int_t GetClusterNo() { return(fClusterNo); };

		Bool_t AddMember(TMrbDGFClusterMember * CluMember);
		inline TMrbDGFClusterMember * GetMember(Int_t CapsNo) {
			if (fMembers.GetEntriesFast() > CapsNo) return((TMrbDGFClusterMember *) fMembers[CapsNo]); else return(NULL);
		}

		inline Int_t GetNofMembers() { return(fMembers.GetEntriesFast()); };
		inline Int_t GetCrate() { return(fCrate); };
		inline const Char_t * GetColor() { return(fColor.Data()); };
		inline const Char_t * GetSide() { return(fSide.Data()); };
		inline const Char_t * GetHeight() { return(fHeight.Data()); };
		inline const Char_t * GetAngle() { return(fAngle.Data()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, Bool_t CmtFlag = kFALSE);
		virtual inline void Print(Bool_t CmtFlag) { Print(cout, CmtFlag); };
		
		Bool_t SetChannelLayout(const Char_t * LayoutName, Int_t NofChannels, const Char_t * ChannelNames); // define channel names to be used
		const Char_t * GetChannelName(Int_t Channel, const Char_t * Layout = "6fold");					// get channel name from layout
		const Char_t * GetChannelLayout(Int_t ModuleNumber, const Char_t * LayoutName = "6fold");		// get channel layout per module

	protected:
		Int_t fClusterNo;
		Int_t fCrate;
		TString fColor;
		TString fSide;
		TString fHeight;
		TString fAngle;

		TObjArray fMembers;

		TMrbLofNamedX fLofChannelLayouts; 	// layouts how to name dgf channels

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
		Int_t ReadFile(const Char_t * ClusterFile = "cluster.def");

		TMrbDGFCluster * GetCluster(Int_t CluNumber);

		inline Int_t GetNofClusters() { return(this->GetEntriesFast()); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm);
		virtual inline void Print() { Print(cout); };
		
	ClassDef(TMrbLofDGFClusters, 1)		// [XIA DGF-4C] List of DGF clusters
};

#endif
