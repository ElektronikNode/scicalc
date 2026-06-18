#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include "value.h"


class Variable
{

public:

	Variable(QString name, long double value, QString unit=QString());
	Variable(QString name, Value::Number value, QString unit=QString());
	Variable(QString name, Value value, QString unit=QString());

	QString name;
	Value value;
	QString unit;
};

#endif
