#ifndef QTSJIS_H
#define QTSJIS_H

#include <QTextCodec>

namespace qtsjis
{ 
	// QString( SJIS )‚Æstd::string( UNICODE )‚Ì‘ŠŒÝ•ÏŠ·ƒc[ƒ‹

	static std::string
	toString( const QString& qstr )
	{
		return QTextCodec::codecForName("SJIS")->fromUnicode( qstr ).constData();
	}

	static QString
	toQString( const std::string& str )
	{
		return QTextCodec::codecForName("SJIS")->toUnicode( str.c_str(), str.length() );
	}
};

#endif // QTSJIS_H