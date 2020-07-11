#ifndef SCANNER_H
#define SCANNER_H

/*
 * 
 * This is a simple scanner for scicalc
 * 
 * Friedrich Feichtinger, 08.09.2012
 * 
 */

#include <QString>
#include "token.h"

class Scanner
{

public:

	static void init(QString line);
	static Token *next();
	static Token *peek();
	static void flush();

private:

	static void nextCh();
	static bool isLetter(QChar c);
	static bool isDecimal(QChar c);
	static void readName(Token *t);
	static void readUnit(Token *t);
	static void readNumber(Token *t);

	static const QChar eof_CHAR;				// end of line

    static QChar ch;						// last character
	static QString in;						// input string
	static int pos;							// current position in input string

	static Token *peeked;					// peeked Token
};

#endif
