#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>


class Variable
{

public:

	Variable(QString name, long double value, QString unit=QString());

	QString name;
	long double value;
	QString unit;
};

#endif
