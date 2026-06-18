
#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

class Token
{
public:
	
	//                                    "="     "+"   "-"    "*"    "/"    "^"  "("   ")"   ","    "||"      ";"        "<"     ">"
	enum Kind {none, ident, unit, number, assign, plus, minus, times, slash, hat, lpar, rpar, comma, parallel, semicolon, langle, rangle,
		lsquare, rsquare, colon, dotTimes, dotSlash, dotHat, transpose, eof};

	Kind		kind;		// kind of token
	long double	value;		// token value
	QString		string; 	// token string
};

#endif
