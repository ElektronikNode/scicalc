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
	
	
	static Value::Number divide(Value::Number dividend, Value::Number divisor);
	static Value::Number requireScalar(Value value, QString context);
	static long double requireRealScalar(Value value, QString context);
	static QString formatValue(Value value, QString unit=QString());
	static bool isExpressionStart(Token::Kind kind);
	static bool isImaginaryUnit(QString name);
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
	static Value mapUnary(Value value, Value::Number (*function)(Value::Number), QString context);
	static Value mapBinary(Value left, Value right, Value::Number (*function)(Value::Number, Value::Number), QString context);
	static int requireInteger(Value value, QString context);
	static int dimensionArgument(Value value, QString context);
	static int firstNonSingletonDimension(Value value);
	static Value reduce(Value value, QString function, int dimension);
	static Value cumulative(Value value, QString function, int dimension);
	static Value sizeFunction(QList<Value> args, QString context);
	static Value reshape(Value value, int rows, int columns);
	static Value zeros(int rows, int columns);
	static Value ones(int rows, int columns);
	static Value eye(int rows, int columns);
	static Value diag(Value value);
	static Value linspace(Value start, Value end, int count, bool logarithmic);
	static Value determinant(Value value);
	static Value trace(Value value);
	static Value rank(Value value);
	static Value norm(Value value);
	static Value dot(Value left, Value right);
	static Value cross(Value left, Value right);
	static Value eig(Value value);
	static Value diff(Value value, int order, int dimension);
	static Value gradient(Value value, int dimension);
	static Value sort(Value value, int dimension);
	static Value unique(Value value);
	static Value trapz(Value value, int dimension);
	static Value unwrap(Value value, int dimension);

	static void check(Token::Kind expected);

	static void scan();

	static QString symbol(Token::Kind k);

	static QString symbol(Token *t);
};


#endif
