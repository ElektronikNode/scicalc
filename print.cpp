/*
 * 
 * This module makes the printing
 *  
 */

#include <cmath>
#include <QDebug>
#include <stdint.h>

#include "print.h"
#include "scicalc.h"


QString Print::sciPrint(double value)
{

	int digits=scicalc::app()->getDigitsSetting();
	bool showZeros=scicalc::app()->getTrailingZerosSetting();
	QString output;


	//printf(" start sciPrint ...\n");

	int exp;				// 10 base of value
	int exp3;				// 1000 base of value

	int64_t rndValue;		// rounded value
	int64_t intValue;		// integer part of value
	int64_t fracValue;		// fractional part of value

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

	// transform the value to an integer format and round it
	rndValue=(int64_t)round(value*pow10(digits-1-exp));

	//qDebug() << "factor:" << pow10(digits-1-exp);
	//qDebug() << "rndValue:" << rndValue;

	// get number of integer digits
	intDigits=exp-3*exp3+1;
	//qDebug() << "intDigits:" << intDigits;

	// get the integer part of value
	intValue=rndValue/pow10(digits-intDigits);

	// print integer part
	//qDebug() << "intValue:" << intValue;
	output=QString::number(intValue);


	// get number of digits of fractional part
	fracDigits=std::max(digits-intDigits, 0);

	// get fractional part of value
	fracValue=fabs(rndValue % (int64_t)pow10(fracDigits));

	if(intDigits<digits && (fracValue!=0 || showZeros))
	{
		// we need some fractional digits
		//System.out.print(".");
		output+=".";

		for(int64_t base=pow10(fracDigits-1); base>0; base/=10)
		{
			//qDebug() << "digit:" << (fracValue/base);
			output+=QString::number(fracValue/base);
			fracValue%=base;
			//qDebug() << "fracValue:" << fracValue;
			if(fracValue==0 && !showZeros)
			{
				break;
			}
		}
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
