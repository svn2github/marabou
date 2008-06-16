#ifndef __LwrDatime_h__
#define __LwrDatime_h__


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDatime                                                              //
//                                                                      //
// This class stores the date and time with a precision of one second   //
// in an unsigned 32 bit word (e.g. 950130 124559). The date is stored  //
// with the origin being the 1st january 1995.                          //
//                                                                      //
// This class has no support for time zones. The time is assumed        //
// to be in the local time of the machine where the object was created. //
// As a result, TDatime objects are not portable between machines       //
// operating in different time zones and unsuitable for storing the     //
// date/time of data taking events and the like. If absolute time is    //
// required, use TTimeStamp.                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter, Apr 2008                                                  //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"

class TDatime {

private:

protected:
   UInt_t     fDatime;            //Date (relative to 1995) + time

public:
   TDatime();
   TDatime(const TDatime &d): fDatime(d.fDatime) { }
   TDatime(UInt_t tloc, Bool_t dosDate = kFALSE): fDatime(0)
     { Set(tloc, dosDate); }
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
