/*
 * 
 * This is a simple variable list for scicalc
 * 
 * Friedrich Feichtinger, 13.09.2012
 * 
 */

#include <QList>
#include <QString>
#include "variable.h"

class Variables
{
	
public:

	static void init();

	static void set(QString name, long double value, QString unit=QString());
	
	static long double get(QString name);
	static QList<Variable*> all();

private:

	static QList<Variable*>variables;
	static Variable* search(QString name);
};
