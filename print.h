/*
 * 
 * This module makes the printing
 *  
 */


#ifndef PRINT
#define PRINT


#include <complex>
#include <QString>


class Print
{

public:

	static QString sciPrint(long double value);
	static QString complexPrint(std::complex<long double> value, bool parenthesize=false);
	static long double pow10(int n);
};

#endif
