#ifndef SCICALCBLOCK_H
#define SCICALCBLOCK_H

#include <QString>

class ScicalcBlock
{
public:
	explicit ScicalcBlock(QString input, QString output=QString());
	
public:
	QString input;
	QString output;
	int cursorPosition;
	
};

#endif // SCICALCBLOCK_H
