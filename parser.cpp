/*
 * 
 * This is a simple parser for scicalc
 * 
 * Friedrich Feichtinger, 09.09.2012
 * 
 */

#include "parser.h"
#include "scanner.h"
#include "variables.h"
#include "parseException.h"
#include "print.h"

#include <cmath>
#include <QList>
#include <QDebug>


Token* Parser::t=0;					// current token
Token* Parser::la=0;				// look-ahead token

Token::Kind Parser::sym=Token::none;			// kind of look-ahead-token


QString Parser::parse()
{
	//qDebug("start parsing");
	long double value;
	QString output;

	scan();

	try
	{
		if(sym==Token::eof)
		{
			// empty input line. no input, no output
			check(Token::eof);
			output="";
		}
		else
		{
			value=Assignment();
			if(sym==Token::semicolon)
			{
				// no output when ';' at end of line
				check(Token::semicolon);
				output="";
				
				// accept unit, but ignore it
				if(sym==Token::unit)
				{
					check(Token::unit);
				}
			}
			else
			{
				output=Print::sciPrint(value);
				
				// append unit
				if(sym==Token::unit)
				{
					check(Token::unit);
					output+=t->string;
				}
			}
			check(Token::eof);
		}
	}
	catch (ParseException e)
	{
		output=QString("ERROR: " + e.text());
	}
	return output;
}


long double Parser::Power()
{
	//qDebug("parse Power");
	long double value=0;

	if(sym==Token::ident)
	{
		check(Token::ident);
		if(sym==Token::lpar)
		{
			// function call
			value=Function();
		}
		else
		{
			// variable or constant
			value=Variables::get(t->string);
		}
	}
	else if(sym==Token::number)
	{
		check(Token::number);
		value=t->value;
	}
	else if(sym==Token::lpar)
	{
		check(Token::lpar);
		value=Expression();
		check(Token::rpar);
	}
	else
	{
		throw ParseException("unexpected symbol '"+symbol(la)+"'");
	}

	return value;
}


long double Parser::Factor()
{
	//qDebug("parse Factor");
	long double value=Power();
	while(sym==Token::hat)
	{
		check(Token::hat);
		value=pow(value, Power());
	}
	return value;
}

long double Parser::Parallel()
{
	//qDebug("parse Parallel");
	long double value=Factor();
	while(sym==Token::times || sym==Token::slash)
	{
		if(sym==Token::times)
		{
			check(Token::times);
			value*=Factor();
		}
		else
		{
			check(Token::slash);
			value=divide(value, Factor());
		}
	}
	return value;
}

long double Parser::Term()
{
	//qDebug("parse Term");
	long double sum=Parallel();

	if(sym==Token::parallel)
	{
		sum=divide(1, sum);

		while(sym==Token::parallel)
		{
			check(Token::parallel);
			sum+=divide(1, Parallel());
		}
		sum=divide(1, sum);
	}

	return sum;
}


long double Parser::Expression()
{
	//qDebug("parse Expression");
	long double value=0;
	int sign=1;

	do
	{
		if(sym==Token::plus)
		{
			check(Token::plus);
			sign=1;
		}
		else if(sym==Token::minus)
		{
			check(Token::minus);
			sign=(-1);
		}

		value+=sign*Term();
	}
	while(sym==Token::plus || sym==Token::minus);

	return value;
}


long double Parser::Assignment()
{
	//qDebug("parse Assingment");
	long double value=0;

	if(Scanner::peek()->kind==Token::assign)
	{
		// this is an assignment
		check(Token::ident);
		QString var=t->string;
		check(Token::assign);
		value=Expression();

		if(var!="$")
		{
			Variables::set(var, value);
		}
	}
	else
	{
		// this is a normal expression
		value=Expression();
	}

	Variables::set("$", value);

	return value;
}


long double Parser::Function()
{
	//qDebug("parse Function");
	QString fun;
	QList<long double> args;
	long double value=0;
	int n=0;

	// remember function name
	fun=t->string;

	// parse argument list
	check(Token::lpar);
	if(sym!=Token::rpar)
	{
		args.append(Expression());
		while(sym==Token::comma)
		{
			check(Token::comma);
			args.append(Expression());
		}
	}
	check(Token::rpar);

	// select function and execute it
	if     (fun=="sqrt"){	n=1;	value=sqrt(args.value(0));}
	else if(fun=="exp"){	n=1;	value=exp(args.value(0));}
	else if(fun=="log"){	n=1;	value=log(args.value(0));}
	else if(fun=="ln"){		n=1;	value=log(args.value(0));}
	else if(fun=="log10"){	n=1;	value=log10(args.value(0));}
	else if(fun=="sin"){	n=1;	value=sin(args.value(0));}
	else if(fun=="cos"){	n=1;	value=cos(args.value(0));}
	else if(fun=="tan"){	n=1;	value=tan(args.value(0));}
	else if(fun=="asin"){	n=1;	value=asin(args.value(0));}
	else if(fun=="acos"){	n=1;	value=acos(args.value(0));}
	else if(fun=="atan"){	n=1;	value=atan(args.value(0));}
	else if(fun=="atan2"){	n=2;	value=atan2(args.value(0), args.value(1));}
	else if(fun=="abs"){	n=1;	value=fabs(args.value(0));}
	else if(fun=="rad2deg"){n=1;	value=args.value(0)*180/M_PI;}
	else
	{
		throw ParseException("unknown function '"+fun+"'");
	}

	if(n!=args.size())
	{
		throw ParseException("invalid number of arguments for function '"+fun+"'");
	}

	if(value!=value)		// isnan(value) does only work with C++11
	{
		throw ParseException("function '"+fun+"' returned NaN");
	}

	return(value);
}


long double Parser::divide(long double dividend, long double divisor)
{
	if(divisor!=0)
	{
		return dividend/divisor;
	}
	else
	{
		throw ParseException("division by zero");
	}
}



void Parser::check(Token::Kind expected)
{
	//qDebug() << "check symbol" << symbol(expected);
	if(sym==expected)
	{
		scan();
	}
	else
	{
		throw ParseException("expected '"+symbol(expected)+"' but received '"+symbol(la)+"'");
	}
}


void Parser::scan()
{
	// delete old token and replace by new one from Scanner::next()
	if(t!=0)
	{
		delete t;
	}
	t=la;
	la=Scanner::next();
	sym=la->kind;
}


// returns the kind of token as QString
QString Parser::symbol(Token::Kind k)
{
	Token t;
	t.kind=k;
	t.string="name";
	return symbol(&t);
}

// returns the name of the tokens kind as QString
QString Parser::symbol(Token* t)
{
	QString string="";

	if(t->kind==Token::ident && !t->string.isEmpty())
	{
		string=t->string;
	}
	else
	{
		switch(t->kind)
		{
			case Token::none:		string="unknown symbol";	break;
			case Token::ident:		string="identifier";		break;
			case Token::unit:		string="unit";				break;
			case Token::number:		string="number";			break;
			case Token::assign:		string="=";					break;
			case Token::plus:		string="+";					break;
			case Token::minus:		string="-";					break;
			case Token::times:		string="*";					break;
			case Token::slash:		string="/";					break;
			case Token::hat:		string="^";					break;
			case Token::lpar:		string="(";					break;
			case Token::rpar:		string=")";					break;
			case Token::comma:		string=",";					break;
			case Token::parallel:	string="||";				break;
			case Token::semicolon:	string=";";					break;
			case Token::langle:		string="<";					break;
			case Token::rangle:		string=">";					break;
			case Token::eof:		string="end of line";		break;
		}
	}
	return string;
}
