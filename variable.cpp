#include "variable.h"

Variable::Variable(QString name, long double value, QString unit)
	: Variable(name, Value(value), unit)
{
}

Variable::Variable(QString name, Value value, QString unit)
{
	this->name=name;
	this->value=value;
	this->unit=unit;
}
