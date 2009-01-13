#ifndef __QEnv_h__
#define __QEnv_h__

//__________________________________________________[QT CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv.h
// Purpose:        Define utilities to be used with QT
// Class:          QEnv    -- an envirmonent manager like ROOT's TEnv
// Description:    Common class definitions to be used with QT
// Author:         R. Lutter
// Revision:       $Id: QEnv.h,v 1.1 2009-01-13 07:41:38 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <QObject>
#include <QString>
#include <QList>

using namespace std;

class QEnvRec {
	public:
		enum ERecType	{	isNew,
							isInteger,
							isDouble,
							isString
						};

	public:
		QEnvRec(QString & key, QString & value);
		~QEnvRec() {};

		inline QString & getKey() { return qKey; };
		inline QString & getValue() { return qValue; };
		inline void setValue(int value) { qValue.setNum(value); };
		inline void setValue(double value) { qValue.setNum(value); };
		inline void setValue(const char * value) { qValue = value; };
		inline void setType(ERecType type) { qRecType = type; };
		void setType(QString & value);
		inline ERecType getType() { return qRecType; };

		void print(int recNo = -1, int lkey = 2);

	protected:
		QString qKey;
		QString qValue;
		ERecType qRecType;
};

class QEnv : public QObject {

	public:
		QEnv(const char * fileName = ".qenvrc", bool verbose = FALSE);
		~QEnv() {};

		int readFile(const char * fileName);
		int writeFile(const char * fileName = "");
		void setValue(const char * key, int value);
		void setValue(const char * key, double value);
		void setValue(const char * key, const char * value);
		int getValue(const char * key, int defVal = 0);
		double getValue(const char * key, double defVal = 0.0);
		const char * getValue(const char * key, const char * defVal = "");

		int find(const char * key);

		void print();
		int getKeyLength();
		inline bool isVerbose() { return qVerbose; };

	protected:
		bool qVerbose;
		QString qEnvFile;
		QList<QEnvRec> qEnvList;
};

#endif
