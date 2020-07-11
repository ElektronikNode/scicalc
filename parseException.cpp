#include "parseException.h"

ParseException::ParseException(QString s)
{
	string=s;
}

QString ParseException::text()
{
	return string;
}
