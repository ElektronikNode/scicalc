#ifndef SCICALCEDIT_H
#define SCICALCEDIT_H

#include <QTextEdit>
#include "scicalcblock.h"

class ScicalcEdit : public QTextEdit
{
	Q_OBJECT
public:
	explicit ScicalcEdit(QWidget *parent = 0);
	
	int getBlockCount();
	ScicalcBlock getBlock(int index);
	void setBlock(int index, ScicalcBlock block);
	
signals:
	void returnPressed();
	void inputChanged();
	
public slots:
	void clear();
	void refreshDisplay();
	
private slots:
	void cursorPositionChanged();
	void readTextFromDisplay();
	
private:
	void keyPressEvent(QKeyEvent *e);
	void undo();
	void redo();
	
	void setCursorToBlock(int blockIndex);
	
	QList<ScicalcBlock> blocks;
	int currentBlock;
	
	QList <QList<ScicalcBlock> > history;
	QList <QList<ScicalcBlock> > future;
	
	void printBlocks();
	
};

#endif // SCICALCEDIT_H
