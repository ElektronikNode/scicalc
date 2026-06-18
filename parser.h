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
#include "value.h"
#include <QStringList>


class Parser
{

public:

	static Token *t;				// current token
	static Token *la;				// look-ahead token

	static Token::Kind sym;			// kind of look-ahead-token
	
	static QString parse();
	static QStringList functionNames();
	
private:

	static QString assignedVariable;

	static Value Power();
	static Value Factor();
	static Value Parallel();
	static Value Term();
	static Value Expression();
	static Value Range();
	static Value Assignment();
	static Value Function();
	static Value MatrixLiteral();
	static bool VersionFunction(QString *output);
	
	
	static long double divide(long double dividend, long double divisor);
	static long double requireScalar(Value value, QString context);
	static QString formatValue(Value value);
	static bool isExpressionStart(Token::Kind kind);
	static void requireSameSize(Value left, Value right, QString op);
	static Value elementWise(Value left, Value right, Token::Kind op);
	static Value add(Value left, Value right);
	static Value subtract(Value left, Value right);
	static Value multiply(Value left, Value right);
	static Value divide(Value left, Value right);
	static Value power(Value left, Value right);
	static Value parallel(Value left, Value right);
	static Value range(Value start, Value end);
	static Value range(Value start, Value step, Value end);
	static Value inverse(Value value);
	static Value transpose(Value value);
	static Value negate(Value value);
	static Value mapUnary(Value value, long double (*function)(long double), QString context);
	static Value mapBinary(Value left, Value right, long double (*function)(long double, long double), QString context);

	static void check(Token::Kind expected);

	static void scan();

	static QString symbol(Token::Kind k);

	static QString symbol(Token *t);
};


#endif
