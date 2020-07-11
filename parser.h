#ifndef PARSER_H
#define PARSER_H

/*
 * 
 * This is a simple parser for scicalc
 * 
 * Friedrich Feichtinger, 09.09.2012
 * 
 */

#include "token.h"


class Parser
{

public:

	static Token *t;				// current token
	static Token *la;				// look-ahead token

	static Token::Kind sym;			// kind of look-ahead-token
	
	static QString parse();
	
private:

	static long double Power();
	static long double Factor();
	static long double Parallel();
	static long double Term();
	static long double Expression();
	static long double Assignment();
	static long double Function();
	
	
	static long double divide(long double dividend, long double divisor);

	static void check(Token::Kind expected);

	static void scan();

	static QString symbol(Token::Kind k);

	static QString symbol(Token *t);
};


#endif
