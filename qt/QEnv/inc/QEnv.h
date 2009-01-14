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
//! Revision:       $Id: QEnv.h,v 1.5 2009-01-14 14:56:33 Rudolf.Lutter Exp $       
//! Date:           
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <QObject>
#include <QString>
#include <QList>

using namespace std;

class QEnvRec {
	public:
		// record types
		enum ERecType	{	kNew		= 0,
							kEmpty,
							kInteger,
							kDouble,
							kString
						};

	public:
		QEnvRec() : qKey(""), qValue(""), qRecType(QEnvRec::kEmpty) {}; 	// the "empty" record
		QEnvRec(QString & key, QString & value);							// ctor
		~QEnvRec() {};														// dtor

		inline QString & getKey() { return qKey; }; 						// get key name
		inline QString & getValue() { return qValue; }; 					// get key value

		inline void setValue(int value) { qValue.setNum(value); };			// set key value
		inline void setValue(double value) { qValue.setNum(value); };
		inline void setValue(const char * value) { qValue = value; };

		inline void setType(ERecType type) { qRecType = type; };			// set record type
		void setType(QString & value);										// ... according to data type

		inline ERecType getType() { return qRecType; }; 					// get record type

		inline bool isEmpty() { return (qRecType == QEnvRec::kEmpty); };	// test if record is ...
		inline bool isInteger() { return (qRecType == QEnvRec::kInteger); };
		inline bool isDouble() { return (qRecType == QEnvRec::kDouble); };
		inline bool isString() { return (qRecType == QEnvRec::kString); };

		void print(int recNo = -1, int lkey = 2);							// print record

	protected:
		QString qKey;
		QString qValue;
		ERecType qRecType;
};

class QEnv : public QObject {

	public:
		QEnv(const char * fileName = ".qenvrc", bool verbose = FALSE);		// ctor
		~QEnv() {}; 														// dtor

		int readFile(const char * fileName);								// read database from file
		int writeFile(const char * fileName = "");							// write/save database to file

		void setValue(const char * key, int value); 						// create entry / modify its value
		void setValue(const char * key, double value);
		void setValue(const char * key, const char * value);

		int getValue(const char * key, int defVal = 0); 					// fetch key value
		double getValue(const char * key, double defVal = 0.0);
		const char * getValue(const char * key, const char * defVal = "");

		QEnvRec & lookup(const char * key); 								// search for a given entry

		void print();														// print database

		inline void setVerbose(bool verbose = TRUE) { qVerbose = verbose; };	//!< turn verbosity on/off
		inline bool isVerbose() { return qVerbose; };							// check if verbose

		QEnvRec & first();													// start iteration
		QEnvRec & next();													// return next element in database

	protected:
		int find(const char * key); 										// [internal] find entry, return index
		int getKeyLength(); 												// calculate length of longest key name

	protected:
		bool qVerbose;
		QString qEnvFile;
		QList<QEnvRec> qEnvList;
		int qCurIndex;
};

#endif
