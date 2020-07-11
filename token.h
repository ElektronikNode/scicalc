
#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

class Token
{
public:
	
	//                                    "="     "+"   "-"    "*"    "/"    "^"  "("   ")"   ","    "||"      ";"        "<"     ">"
	enum Kind {none, ident, unit, number, assign, plus, minus, times, slash, hat, lpar, rpar, comma, parallel, semicolon, langle, rangle, eof};

	Kind		kind;		// kind of token
	double		value;		// token value
	QString		string; 	// token string
};

#endif
