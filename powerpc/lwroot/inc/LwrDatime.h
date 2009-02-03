#ifndef __LwrDatime_h__
#define __LwrDatime_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrDatime.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TDatime<br>
//! 				Date & time handling class.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"

class TDatime {

private:

protected:
   UInt_t     fDatime;            //Date (relative to 1995) + time

public:
   TDatime();
   TDatime(const TDatime &d): fDatime(d.fDatime) { }
   TDatime(UInt_t tloc, Bool_t dosDate = kFALSE): fDatime(0) { Set(tloc, dosDate); }
   TDatime(Int_t date, Int_t time);
   TDatime(Int_t year, Int_t month, Int_t day,
           Int_t hour, Int_t min, Int_t sec);
   TDatime(const char *sqlDateTime);
   virtual ~TDatime() { }

   TDatime& operator=(const TDatime &d);

   const char  *AsString() const;
   const char  *AsString(char *out) const;
   const char  *AsSQLString() const;
   UInt_t       Convert(Bool_t toGMT = kFALSE) const;
   void         Copy(TDatime &datime) const;
   UInt_t       Get() const { return fDatime; }
   Int_t        GetDate() const;
   Int_t        GetTime() const;
   Int_t        GetYear() const { return (fDatime>>26) + 1995; }
   Int_t        GetMonth() const { return (fDatime<<6)>>28; }
   Int_t        GetDay() const { return (fDatime<<10)>>27; }
   Int_t        GetDayOfWeek() const;
   Int_t        GetHour() const { return (fDatime<<15)>>27; }
   Int_t        GetMinute() const { return (fDatime<<20)>>26; }
   Int_t        GetSecond() const { return (fDatime<<26)>>26; }
   void         Print(Option_t *option="") const;
   void         Set();
   void         Set(UInt_t tloc, Bool_t dosDate = kFALSE);
   void         Set(Int_t date, Int_t time);
   void         Set(Int_t year, Int_t month, Int_t day,
                    Int_t hour, Int_t min, Int_t sec);
   void         Set(const char *sqlDateTime);
   Int_t        Sizeof() const {return sizeof(UInt_t);}

   friend Bool_t operator==(const TDatime &d1, const TDatime &d2);
   friend Bool_t operator!=(const TDatime &d1, const TDatime &d2);
   friend Bool_t operator< (const TDatime &d1, const TDatime &d2);
   friend Bool_t operator<=(const TDatime &d1, const TDatime &d2);
   friend Bool_t operator> (const TDatime &d1, const TDatime &d2);
   friend Bool_t operator>=(const TDatime &d1, const TDatime &d2);

   inline const Char_t * ClassName() const { return "TDatime"; };

   static void GetDateTime(UInt_t datetime, Int_t &date, Int_t &time);
};


inline TDatime& TDatime::operator=(const TDatime &d)
   { fDatime = d.fDatime; return *this; }

inline Bool_t operator==(const TDatime &d1, const TDatime &d2)
   { return d1.fDatime == d2.fDatime; }
inline Bool_t operator!=(const TDatime &d1, const TDatime &d2)
   { return d1.fDatime != d2.fDatime; }
inline Bool_t operator< (const TDatime &d1, const TDatime &d2)
   { return d1.fDatime < d2.fDatime; }
inline Bool_t operator<=(const TDatime &d1, const TDatime &d2)
   { return d1.fDatime <= d2.fDatime; }
inline Bool_t operator> (const TDatime &d1, const TDatime &d2)
   { return d1.fDatime > d2.fDatime; }
inline Bool_t operator>=(const TDatime &d1, const TDatime &d2)
   { return d1.fDatime >= d2.fDatime; }

#endif
