#ifndef __QEnv_h__
#define __QEnv_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			QEnv.h
//! \brief			Define utilities to be used with QT
//! \details		Common class definitions to be used with QT
//! $Author: Rudolf.Lutter $
//! $Revision: 1.12 $
//! $Date: 2009-01-22 09:38:36 $
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
		//!< record types
		enum ERecType	{	kNew		= 0,				//!< a newly created record
							kEmpty, 						//!< an empty record
							kInteger,						//!< record value is integer
							kDouble,						//!< record value is double
							kBoolean,						//!< record value is boolean
							kString 						//!< record value ist string
						};

	public:
		QEnvRec() : qKey(""), qValue(""), qRecType(QEnvRec::kEmpty) {};
		QEnvRec(QString & key, QString & value);
		~QEnvRec() {};

 		//! \brief get key name
		inline QString & getKey() { return qKey; };
 		//! \brief get key value
		inline QString & getValue() { return qValue; };

		//! \brief set key value to integer
		inline void setValue(Int_t value) { qValue.setNum(value); };
		//! \brief set key value to double
		inline void setValue(Double_t value) { qValue.setNum(value); };
		//! \brief set key value to boolean
		inline void setValue(Bool_t value) { qValue.setNum(value); };
		//! \brief set key value to string
		inline void setValue(const Char_t * value) { qValue = value; };

		//! \brief set record type
		inline void setType(ERecType type) { qRecType = type; };
		//! \brief set record type according to data type
		void setType(QString & value);

 		//! \brief get record type
		inline ERecType getType() { return qRecType; };

		//! \brief test if record is empty
		inline Bool_t isEmpty() { return (qRecType == QEnvRec::kEmpty); };
		//! \brief test if record is integer
		inline Bool_t isInteger() { return (qRecType == QEnvRec::kInteger); };
		//! \brief test if record is double
		inline Bool_t isDouble() { return (qRecType == QEnvRec::kDouble); };
		//! \brief test if record is boolean
		inline Bool_t isBoolean() { return (qRecType == QEnvRec::kBoolean); };
		//! \brief test if record is string
		inline Bool_t isString() { return (qRecType == QEnvRec::kString); };

		//! \brief print record
		void print(Int_t recNo = -1, Int_t lkey = 2);

	protected:
		QString qKey;					//!< key name
		QString qValue; 				//!< key value
		ERecType qRecType;				//!< record type
};

class QEnv : public QObject {

	public:
		QEnv(const Char_t * fileName = ".qenvrc", Bool_t verbose = kFALSE);
		~QEnv() {};

		//! read database from file
		Int_t readFile(const Char_t * fileName);
		//! write/save database to file
		Int_t writeFile(const Char_t * fileName = "");

 		//! create entry / modify its value (integer)
		void setValue(const Char_t * key, Int_t value);
 		//! create entry / modify its value (double)
		void setValue(const Char_t * key, Double_t value);
 		//! create entry / modify its value (boolean)
		void setValue(const Char_t * key, Bool_t value);
 		//! create entry / modify its value (string)
		void setValue(const Char_t * key, const Char_t * value);

 		//! fetch key value (integer)
		Int_t getValue(const Char_t * key, Int_t defVal = 0);
 		//! fetch key value (double)
		Double_t getValue(const Char_t * key, Double_t defVal = 0.0);
 		//! fetch key value (boolean)
		Bool_t getValue(const Char_t * key, Bool_t defVal = kFALSE);
 		//! fetch key value (string)
		const Char_t * getValue(const Char_t * key, const Char_t * defVal = "");

 		//! search for a given entry
		QEnvRec & lookup(const Char_t * key, Bool_t singleMatch = kTRUE);
 		//! search all matching entries
		Int_t lookup(const Char_t * key, QList<QEnvRec> & recList);

		//! print database
		void print();

		//! turn verbosity on/off
		inline void setVerbose(Bool_t verbose = kTRUE) { qVerbose = verbose; };
		//! check if verbose
		inline Bool_t isVerbose() { return qVerbose; };

		//! start iteration
		QEnvRec & first();
		//! return next element in database
		QEnvRec & next();

	protected:
		 //! find matching entries
		Int_t find(const Char_t * key, QList<Int_t> & idxList);
 		//! calculate length of longest key name
		Int_t getKeyLength();

	protected:
		Bool_t qVerbose;							//!< kTRUE if verbose mode
		QString qEnvFile;							//!< name of env file
		QList<QEnvRec> qEnvList;					//!< env database
		Int_t qCurIndex;							//!< current index (used by iteration)
};

#endif
