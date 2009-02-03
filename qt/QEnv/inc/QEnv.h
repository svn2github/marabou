#ifndef __QEnv_h__
#define __QEnv_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			QEnv.h
//! \brief			Define utilities to be used with QT
//! \details		Common class definitions to be used with QT
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.16 $
//! $Date: 2009-02-03 08:29:20 $
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
		enum ERecType	{	kNew		= 0,				//!< a newly created record
							kEmpty, 						//!< an empty record
							kInteger,						//!< record value is integer
							kDouble,						//!< record value is double
							kBoolean,						//!< record value is boolean
							kString 						//!< record value ist string
						};

	public:
		//! Constructor
		//! "The empty record" -- key name and value set to ""
		QEnvRec() : qKey(""), qValue(""), qRecType(QEnvRec::kEmpty) {};

		//! Constructor
		//! \param[in]		key 	-- key name
		//! \param[in]		value 	-- key value
		QEnvRec(QString & key, QString & value);

		//! Destructor
		~QEnvRec() {};

 		//! Get key name
		//! \retval KeyName 	-- key name
		inline QString & getKey() { return qKey; };

 		//! Get key value
		//! \retval KeyVal 		-- key value
		inline QString & getValue() { return qValue; };

		//! Set key value to integer
		//! \param[in]		value 	-- key value (integer)
		inline void setValue(Int_t value) { qValue.setNum(value); };

		//! Set key value to double
		//! \param[in]		value 	-- key value (double)
		inline void setValue(Double_t value) { qValue.setNum(value); };

		//! Set key value to boolean
		//! \param[in]		value 	-- key value (boolean)
		inline void setValue(Bool_t value) { qValue.setNum(value); };

		//! Set key value to string
		//! \param[in]		value 	-- key value (string)
		inline void setValue(const Char_t * value) { qValue = value; };

		//! Set record type
		//! \param[in]		type 	-- record type
		inline void setType(ERecType type) { qRecType = type; };

		//! Set record type according to data type
		void setType(QString & value);

 		//! Get record type
		//! \retval RecType 		-- record type (integer, double, boolean, string ...)
		inline ERecType getType() { return qRecType; };

		//! Test if record is empty
		//! \return 	TRUE or FALSE
		inline Bool_t isEmpty() { return (qRecType == QEnvRec::kEmpty); };

		//! Test if record is integer
		//! \return 	TRUE or FALSE
		inline Bool_t isInteger() { return (qRecType == QEnvRec::kInteger); };

		//! Test if record is double
		//! \return 	TRUE or FALSE
		inline Bool_t isDouble() { return (qRecType == QEnvRec::kDouble); };

		//! Test if record is boolean
		//! \return 	TRUE or FALSE
		inline Bool_t isBoolean() { return (qRecType == QEnvRec::kBoolean); };

		//! Test if record is string
		//! \return 	TRUE or FALSE
		inline Bool_t isString() { return (qRecType == QEnvRec::kString); };

		//! Print record
		void print(Int_t recNo = -1, Int_t lkey = 2);

	protected:
		QString qKey;					//!< key name
		QString qValue; 				//!< key value
		ERecType qRecType;				//!< record type
};

class QEnv : public QObject {

	public:
		//! Constructor
		//! \param[in]		fileName 	-- file name
		//! \param[in]		verbose 	-- kTRUE if verbose mode
		QEnv(const Char_t * fileName = ".qenvrc", Bool_t verbose = kFALSE);

		//! Destructor
		~QEnv() {};

		//! Read database from file
		Int_t readFile(const Char_t * fileName);
		//! Write/save database to file
		Int_t writeFile(const Char_t * fileName = "");

 		//! Create entry / modify its value (integer)
		void setValue(const Char_t * key, Int_t value);
 		//! Create entry / modify its value (double)
		void setValue(const Char_t * key, Double_t value);
 		//! Create entry / modify its value (boolean)
		void setValue(const Char_t * key, Bool_t value);
 		//! Create entry / modify its value (string)
		void setValue(const Char_t * key, const Char_t * value);

 		//! Fetch key value (integer)
		Int_t getValue(const Char_t * key, Int_t defVal = 0);
 		//! Fetch key value (double)
		Double_t getValue(const Char_t * key, Double_t defVal = 0.0);
 		//! Fetch key value (boolean)
		Bool_t getValue(const Char_t * key, Bool_t defVal = kFALSE);
 		//! Fetch key value (string)
		const Char_t * getValue(const Char_t * key, const Char_t * defVal = "");

 		//! Search for a given entry
		QEnvRec & lookup(const Char_t * key, Bool_t singleMatch = kTRUE);
 		//! Search all matching entries
		Int_t lookup(const Char_t * key, QList<QEnvRec> & recList);

		//! Print database
		void print();

		//! Turn verbosity on/off
		//! \param[in]		verbose 	-- kTRUE if verbose mode
		inline void setVerbose(Bool_t verbose = kTRUE) { qVerbose = verbose; };

		//! Check if verbose
		inline Bool_t isVerbose() { return qVerbose; };

		//! Start iteration
		QEnvRec & first();

		//! Return next element in database
		QEnvRec & next();

	protected:
		 //! Find matching entries
		Int_t find(const Char_t * key, QList<Int_t> & idxList);

 		//! Calculate length of longest key name
		Int_t getKeyLength();

	protected:
		Bool_t qVerbose;							//!< kTRUE if verbose mode
		QString qEnvFile;							//!< name of env file
		QList<QEnvRec> qEnvList;					//!< env database
		Int_t qCurIndex;							//!< current index (used by iteration)
};

#endif
