/*
 * 
 * This is a simple variable list for scicalc
 * 
 * Friedrich Feichtinger, 13.09.2012
 * 
 */

#include "variables.h"
#include "parseException.h"

#include <cmath>
#include <QDebug>


QList<Variable*>Variables::variables;


void Variables::init()
{
	variables.clear();

	// math
	set("pi", M_PI);
	set("i", Value::Number(0, 1));
	set("j", Value::Number(0, 1));

	// mechanic
	set("_g", 9.81);
	set("_G", 6.67384e-11);

	// electromagnetism
	set("_mu0", 4*M_PI*1e-7);
	set("_eps0", 8.85418781762e-12);
	set("_c0", 299792458);
	set("_e", 1.60217653e-19);

	// thermodynamic
	set("_kB", 1.3806488e-23);

	// quantum physic
	set("_h", 6.62606957e-34);

}


void Variables::set(QString name, long double value, QString unit)
{
	set(name, Value(value), unit);
}

void Variables::set(QString name, Value::Number value, QString unit)
{
	set(name, Value(value), unit);
}


void Variables::set(QString name, Value value, QString unit)
{
	//qDebug() << "setVariable" << name;
	// search the list for this name
	Variable* var=search(name);

	// check if variable allready exists
	if(var==0)
	{
		// no: let's make a new variable
		var=new Variable(name, value, unit);
		variables.append(var);
	}
	else
	{
		// yes: let's change it's value
		var->value=value;
		var->unit=unit;
	}
}


long double Variables::get(QString name)
{
	Value value=getValue(name);
	if(!value.isScalar())
	{
		throw ParseException("variable '"+name+"' is not a scalar");
	}
	if(value.scalar().imag()!=0)
	{
		throw ParseException("variable '"+name+"' is not real");
	}
	return value.scalar().real();
}


Value Variables::getValue(QString name)
{
	Variable* var=search(name);
	if(var==0)
	{
		throw ParseException("variable '"+name+"' has no value yet");
	}
	else
	{
		return var->value;
	}
}

QList<Variable*> Variables::all()
{
	return variables;
}



Variable* Variables::search(QString name)
{
	for(int i=0; i<variables.length(); i++)
	{
		if(variables.at(i)->name==name)
		{
			return variables.at(i);
		}
	}
	return 0;
}
