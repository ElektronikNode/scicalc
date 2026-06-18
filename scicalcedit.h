#ifndef SCICALCEDIT_H
#define SCICALCEDIT_H

#include <QTextEdit>
#include "scicalcblock.h"

class QMimeData;
class QCompleter;
class QStandardItemModel;

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
	void insertCompletion(QString completion);
	
private:
	void keyPressEvent(QKeyEvent *e);
	void insertFromMimeData(const QMimeData *source);
	void undo();
	void redo();
	
	void setCursorToBlock(int blockIndex);
	void showVariableCompletions(bool manual);
	void updateCompletionModel();
	QString completionPrefix() const;
	bool isIdentifierCharacter(QChar ch) const;
	
	QList<ScicalcBlock> blocks;
	int currentBlock;
	bool currentLineIsInput;
	QCompleter *completer;
	QStandardItemModel *completionModel;
	
	QList <QList<ScicalcBlock> > history;
	QList <QList<ScicalcBlock> > future;
	
	void printBlocks();
	
};

#endif // SCICALCEDIT_H
