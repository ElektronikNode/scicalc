
#ifndef PARSE_EXCEPTION
#define PARSE_EXCEPTION

#include <QString>

class ParseException
{
public:
	ParseException();

	ParseException(QString s);

	QString text();

private:
	QString string;
};

#endif
