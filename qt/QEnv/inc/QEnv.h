#ifndef __QEnv_h__
#define __QEnv_h__

//__________________________________________________[QT CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file
//! \verbatim
//! Name:           QEnv.h
//! Purpose:        Define utilities to be used with QT
//! Class:          QEnv    -- an envirmonent manager like ROOT's TEnv
//! Description:    Common class definitions to be used with QT
//! Author:         R. Lutter
//! Revision:       $Id: QEnv.h,v 1.6 2009-01-15 12:08:44 Rudolf.Lutter Exp $       
//! Date:           
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <Rtypes.h>

#include <QObject>
#include <QString>
#include <QList>

using namespace std;

class QEnvRec {
	public:
		// record types
		enum ERecType	{	kNew		= 0,				//!< a newly created record
							kEmpty, 						//!< an empty record
							kInteger,						//!< record value is integer
							kDouble,						//!< record value is double
							kString 						//!< record value ist string
						};

	public:
		QEnvRec() : qKey(""), qValue(""), qRecType(QEnvRec::kEmpty) {}; 	// the "empty" record
		QEnvRec(QString & key, QString & value);							// ctor
		~QEnvRec() {};														// dtor

		inline QString & getKey() { return qKey; }; 						// get key name
		inline QString & getValue() { return qValue; }; 					// get key value

		inline void setValue(Int_t value) { qValue.setNum(value); };		// set key value
		inline void setValue(Double_t value) { qValue.setNum(value); };
		inline void setValue(const Char_t * value) { qValue = value; };

		inline void setType(ERecType type) { qRecType = type; };			// set record type
		void setType(QString & value);										// ... according to data type

		inline ERecType getType() { return qRecType; }; 					// get record type

		inline Bool_t isEmpty() { return (qRecType == QEnvRec::kEmpty); };	// test if record is ...
		inline Bool_t isInteger() { return (qRecType == QEnvRec::kInteger); };
		inline Bool_t isDouble() { return (qRecType == QEnvRec::kDouble); };
		inline Bool_t isString() { return (qRecType == QEnvRec::kString); };

		void print(Int_t recNo = -1, Int_t lkey = 2);						// print record

	protected:
		QString qKey;
		QString qValue;
		ERecType qRecType;
};

class QEnv : public QObject {

	public:
		QEnv(const Char_t * fileName = ".qenvrc", Bool_t verbose = kFALSE);	// ctor
		~QEnv() {}; 														// dtor

		Int_t readFile(const Char_t * fileName);							// read database from file
		Int_t writeFile(const Char_t * fileName = "");						// write/save database to file

		void setValue(const Char_t * key, Int_t value); 					// create entry / modify its value
		void setValue(const Char_t * key, Double_t value);
		void setValue(const Char_t * key, const Char_t * value);

		Int_t getValue(const Char_t * key, Int_t defVal = 0); 				// fetch key value
		Double_t getValue(const Char_t * key, Double_t defVal = 0.0);
		const Char_t * getValue(const Char_t * key, const Char_t * defVal = "");

		QEnvRec & lookup(const Char_t * key, Bool_t singleMatch = kTRUE); 	// search for a given entry
		Int_t lookup(const Char_t * key, QList<QEnvRec> & recList); 		// search all matching entries

		void print();														// print database

		inline void setVerbose(Bool_t verbose = kTRUE) { qVerbose = verbose; };	//!< turn verbosity on/off
		inline Bool_t isVerbose() { return qVerbose; };							// check if verbose

		QEnvRec & first();													// start iteration
		QEnvRec & next();													// return next element in database

	protected:
		Int_t find(const Char_t * key, QList<Int_t> & idxList); 			// find matching entries
		Int_t getKeyLength(); 												// calculate length of longest key name

	protected:
		Bool_t qVerbose;
		QString qEnvFile;
		QList<QEnvRec> qEnvList;
		Int_t qCurIndex;
};

#endif
