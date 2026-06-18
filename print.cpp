/*
 * 
 * This module makes the printing
 *  
 */

#include <cmath>
#include <QDebug>

#include "print.h"
#include "scicalc.h"

namespace
{
	long double cleanPart(long double value)
	{
		long double rounded=roundl(value);
		if(rounded!=0 && fabsl(value-rounded)<1e-12L)
		{
			return rounded;
		}

		return value;
	}
}

QString Print::sciPrint(long double value)
{
	value=cleanPart(value);
	if(std::isnan(value))
	{
		return "nan";
	}
	if(std::isinf(value))
	{
		return value<0 ? "-inf" : "inf";
	}

	int digits=scicalc::app()->getDigitsSetting();
	bool showZeros=scicalc::app()->getTrailingZerosSetting();
	QString output;

	int exp;				// 10 base of value
	int exp3;				// 1000 base of value

	int intDigits;			// number of integer digits
	int fracDigits;			// number of fractional digits

	// find the 10 base and 1000 base of the value
	if(value!=0)
	{
		exp=(int)floor(log10(fabs(value)));
		exp3=(int)floor(log10(fabs(value))/3);
	}
	else
	{
		exp=0;
		exp3=0;
	}
	
	//qDebug() << "exp:" << exp;
	//qDebug() << "exp3:" << exp3;

	long double scaled=value/pow10(3*exp3);

	// get number of integer digits
	intDigits=exp-3*exp3+1;
	//qDebug() << "intDigits:" << intDigits;

	// get number of digits of fractional part
	fracDigits=std::max(digits-intDigits, 0);
	if(showZeros)
	{
		output=QString::number(static_cast<double>(scaled), 'f', fracDigits);
	}
	else
	{
		output=QString::number(static_cast<double>(scaled), 'g', digits);
	}

	// print scientific prefix
	switch(exp3)
	{
		case 0: break;
		case 1: output+="k"; break;
		case 2: output+="M"; break;
		case 3: output+="G"; break;
		case 4: output+="T"; break;
		case 5: output+="P"; break;
		case 6: output+="E"; break;
		case -1: output+="m"; break;
		case -2: output+="u"; break;
		case -3: output+="n"; break;
		case -4: output+="p"; break;
		case -5: output+="f"; break;
		case -6: output+="a"; break;

		default:
		{
			//qDebug() << "EE-case";
			//qDebug() << output;
			output+="e" + QString::number(3*exp3);
			//qDebug() << output;
			break;
		}
	}

	//printf("sciPrint ... complete\n");

	return output;
}

QString Print::complexPrint(std::complex<long double> value, bool parenthesize)
{
	long double real=cleanPart(value.real());
	long double imag=cleanPart(value.imag());

	if(imag==0)
	{
		return sciPrint(real);
	}

	QString output;
	if(real==0)
	{
		output=sciPrint(imag) + "j";
	}
	else
	{
		QString sign=imag<0 ? "-" : "+";
		output=sciPrint(real) + sign + sciPrint(fabsl(imag)) + "j";
	}

	if(parenthesize)
	{
		output="(" + output + ")";
	}
	return output;
}


long double Print::pow10(int n)
{
	long double pow=1;
	if(n>0)
	{
		for(int i=0; i<n; i++)
		{
			pow*=10;
		}
	}
	else if(n<0)
	{
		n=-n;
		for(int i=0; i<n; i++)
		{
			pow/=10;
		}
	}
	return pow;
}
