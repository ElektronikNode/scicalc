/*
 * 
 * This is a simple scanner for scicalc
 * 
 * Friedrich Feichtinger, 08.09.2012
 * 
 */

#include "scanner.h"
#include <cmath>
#include <QDebug>


const QChar Scanner::eof_CHAR=QChar::LineSeparator;		// end of line

QChar Scanner::ch;							// last character
QString Scanner::in;						// input string
int Scanner::pos;							// current position in input string

Token* Scanner::peeked;						// peeked Token


void Scanner::init(QString line)
{
	in=line;
	peeked=0;
	pos=0;

	nextCh();
}


Token* Scanner::next()
{
	Token *t;
	if(peeked!=0)		// last token was peeked, so return this instead of reading a new one
	{
		t=peeked;
		peeked=0;
		return t;
	}

	t=new Token();

	while(ch<=' ')		// skip all whitespace characters
	{
		nextCh();
	}

	if(ch==eof_CHAR)
	{
		t->kind=Token::eof;
	}
	else if(isLetter(ch) || ch=='$')
	{
		readName(t);
	}
	else if(ch=='#')
	{
		readUnit(t);
	}
	else if(isDecimal(ch))
	{
		readNumber(t);
	}
	else
	{
        switch(ch.toAscii())
		{
			case '=':	nextCh(); t->kind=Token::assign;	break;
			case '+':	nextCh(); t->kind=Token::plus;		break;
			case '-':	nextCh(); t->kind=Token::minus;		break;
			case '*':	nextCh(); t->kind=Token::times;		break;
			case '/':	nextCh();
						if(ch!='/')
						{
							// this is a single '/'
							t->kind=Token::slash;
							break;
						}
						// this is "//" line comment so jump to next case
			case '%':
						// line comment (MATLAB style)
						do
						{
							nextCh();
						}
						while(ch!=eof_CHAR);
						t=next(); // continue scanning after comment
						
						break;
						
			case '^':	nextCh(); t->kind=Token::hat;	break;
			case '(':	nextCh(); t->kind=Token::lpar;	break;
			case ')':	nextCh(); t->kind=Token::rpar;	break;
			case ',':	nextCh(); t->kind=Token::comma;	break;
			case '|':	nextCh();
						if(ch=='|')
						{
							nextCh();
							t->kind=Token::parallel;
						}
						else
						{
							// unknown token
							t->kind=Token::none;
						}
						break;
			case ';':	nextCh(); t->kind=Token::semicolon;	break;
			case '<':	nextCh(); t->kind=Token::langle;	break;
			case '>':	nextCh(); t->kind=Token::rangle;	break;
			default:
			{
				// unknown token
				t->kind=Token::none;
				break;
			}
		}
	}
	return t;
}


Token* Scanner::peek()
{
	if(peeked==0)
	{
		peeked=next();
	}

	return peeked;
}


void Scanner::nextCh()
{
	if(pos<in.size())
	{
		ch=in.at(pos);
		
		// replace the latin mu by the greek mu
		if(ch.unicode()==181)
		{
			ch=956;
		}
		
		//qDebug() << "unicode:" << ch.unicode();
		pos++;
	}
	else
	{
		ch=eof_CHAR;
	}
}


bool Scanner::isLetter(QChar c)
{
	return c.isLetter() || c=='_';
}


bool Scanner::isDecimal(QChar c)
{
	return (c>='0' && c<='9');
}


void Scanner::readName(Token *t)
{
	int i=0;
	do
	{
		t->string.append(ch);
		nextCh();
		i++;
	}
	while(isLetter(ch) || isDecimal(ch));

	t->kind=Token::ident;
}

void Scanner::readUnit(Token *t)
{
	nextCh();
	int i=0;
	do
	{
		t->string.append(ch);
		nextCh();
		i++;
	}
	while(!ch.isSpace());

	t->kind=Token::unit;
}


void Scanner::readNumber(Token *t)
{
	t->kind=Token::number;

	long double value=0;

	// digits before '.'
	do
	{
		value*=10;
		//value+=(int)(ch-'0');
		value+=ch.digitValue();
		nextCh();
	}
	while(isDecimal(ch));


	if(ch=='.')
	{
		// digits after '.'
		nextCh();

		long double base=1;

		while(isDecimal(ch))
		{
			base/=10;
			//value+=(int)(ch-'0')*base;
			value+=ch.digitValue()*base;
			nextCh();
		}
	}


	switch(ch.toAscii())
	{
		// scientific prefix
		case 'a': value/=1000; // don't break!
		case 'f': value/=1000; // don't break!
		case 'p': value/=1000; // don't break!
		case 'n': value/=1000; // don't break!
		case 'u': value/=1000; // don't break!
		case 'm': value/=1000; nextCh(); break;

		case 'E': value*=1000; // don't break!
		case 'P': value*=1000; // don't break!
		case 'T': value*=1000; // don't break!
		case 'G': value*=1000; // don't break!
		case 'M': value*=1000; // don't break!
		case 'k': value*=1000; nextCh(); break;

		// power of 10 extension
		case 'e':
		{
			nextCh();

			int sign=1;
			if(ch=='+')
			{
				sign=1;
				nextCh();
			}
			else if(ch=='-')
			{
				sign=-1;
				nextCh();
			}

			int exp=0;
			while(isDecimal(ch))
			{
				exp*=10;
				//exp+=(int)(ch-'0');
				exp+=ch.digitValue();
				nextCh();
			}

			exp*=sign;
			value*=pow(10, exp);
			break;
		}
	}
	
	if(ch==QChar(0x00B0))	// ° degree sign
	{
		value*=M_PI/180;
		nextCh();
	}

	t->value=value;
	//qDebug() << "value scanned" << value;
}
