#include "scicalcedit.h"
#include "variables.h"
#include "print.h"
#include "variable.h"
#include "parser.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMimeData>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemView>
#include <QTextBlock>

ScicalcEdit::ScicalcEdit(QWidget *parent) :
	QTextEdit(parent),
	currentBlock(0),
	currentLineIsInput(true),
	completer(new QCompleter(this)),
	completionModel(new QStandardItemModel(this))
{
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
	connect(this, SIGNAL(textChanged()), this, SLOT(readTextFromDisplay()));

	completer->setWidget(this);
	completer->setModel(completionModel);
	completer->setCompletionRole(Qt::UserRole);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseSensitive);
	completer->setWrapAround(false);
	connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}


void ScicalcEdit::keyPressEvent(QKeyEvent *e)
{
	QTextCursor cursor=textCursor();

	if(completer->popup()->isVisible())
	{
		switch(e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
			{
				e->ignore();
				return;
			}
			default:
				break;
		}
	}

	if(e->key()==Qt::Key_Space && e->modifiers()==Qt::ControlModifier)
	{
		showVariableCompletions(true);
		return;
	}
	
	switch(e->key())
	{
		case Qt::Key_Down:
		{
			completer->popup()->hide();
			if(currentBlock<blocks.size()-1)
			{
				currentBlock++;
				setCursorToBlock(currentBlock);
			}
			break;
		}
			
		case Qt::Key_Up:
		{
			completer->popup()->hide();
			if(currentBlock>0)
			{
				currentBlock--;
				setCursorToBlock(currentBlock);
			}
			break;
		}
			
		case Qt::Key_Right:
		{
			completer->popup()->hide();
			if(cursor.atBlockEnd())
			{
				// cursor is at end of line, jump to next block
				if(currentBlock<blocks.size()-1)
				{
					// go to start of next block
					currentBlock++;
					blocks[currentBlock].cursorPosition=0;
					setCursorToBlock(currentBlock);
				}
			}
			else
			{
				QTextEdit::keyPressEvent(e);
			}
			break;
		}
		
		case Qt::Key_Left:
		{
			completer->popup()->hide();
			if(cursor.atBlockStart())
			{
				// cursor is at end of line, jump to next block
				if(currentBlock>0)
				{
					// go to end of previous block
					currentBlock--;
					blocks[currentBlock].cursorPosition=blocks.at(currentBlock).input.length();
					setCursorToBlock(currentBlock);
				}
			}
			else
			{
				QTextEdit::keyPressEvent(e);
			}
			break;
		}
		
		
		case Qt::Key_Return: case Qt::Key_Enter:
		{
			completer->popup()->hide();
			QString currentInput = blocks.at(currentBlock).input;
			int splitPos = cursor.positionInBlock();
			
			if(cursor.atBlockStart())
			{
				// cursor is at the start of line, insert new block above
				blocks.insert(currentBlock, ScicalcBlock(QString()));
				currentBlock++;
			}
			else
			{
				// split current block into two blocks
				QString left=currentInput.left(splitPos);
				QString right=currentInput.mid(splitPos);
				blocks[currentBlock].input=left;
				blocks[currentBlock].cursorPosition=left.length();
				
				currentBlock++;
				blocks.insert(currentBlock, ScicalcBlock(right));
				blocks[currentBlock].cursorPosition=0;
			}
			refreshDisplay();
			emit returnPressed();
			break;
		}
		
			case Qt::Key_Backspace:
			{
				bool blockStructureChanged=false;
				if(cursor.hasSelection())
				{
					QTextEdit::keyPressEvent(e);
				}
				else if(cursor.atBlockStart())
			{
				if(blocks.at(currentBlock).input.isEmpty() && currentBlock>0)
					{
						// at the start of empty block -> remove block
						blocks.removeAt(currentBlock);
						currentBlock--;
						blocks[currentBlock].cursorPosition=blocks.at(currentBlock).input.length();
						refreshDisplay();
						blockStructureChanged=true;
					}
					else if(currentBlock>0)
					{
						// merge with previous block
						int prevIndex=currentBlock-1;
					int prevLength=blocks.at(prevIndex).input.length();
						blocks[prevIndex].input+=blocks.at(currentBlock).input;
						blocks.removeAt(currentBlock);
						currentBlock=prevIndex;
						blocks[currentBlock].cursorPosition=prevLength;
						refreshDisplay();
						blockStructureChanged=true;
					}
				}
				else
				{
					QTextEdit::keyPressEvent(e);
				}
				
				if(blockStructureChanged)
				{
					emit inputChanged();
					emit returnPressed();
				}
				showVariableCompletions(false);
				break;
			}
			
			case Qt::Key_Delete:
			{
				bool blockStructureChanged=false;
				if(cursor.hasSelection())
				{
					QTextEdit::keyPressEvent(e);
				}
				else if(cursor.atBlockEnd())
			{
				if(blocks.at(currentBlock).input.isEmpty() && blocks.size()>1)
				{
					// at the end of empty block -> remove block
					blocks.removeAt(currentBlock);
					if(currentBlock>=blocks.size()-1)
					{
						currentBlock=blocks.size()-1;
						blocks[currentBlock].cursorPosition=blocks.at(currentBlock).input.length();
					}
					else
						{
							// currentBlock=currentBlock;
							blocks[currentBlock].cursorPosition=0;
						}
						refreshDisplay();
						blockStructureChanged=true;
					}
					else if(currentBlock<blocks.size()-1)
					{
						// merge with next block
						int currentLength=blocks.at(currentBlock).input.length();
						blocks[currentBlock].input+=blocks.at(currentBlock+1).input;
						blocks.removeAt(currentBlock+1);
						blocks[currentBlock].cursorPosition=currentLength;
						refreshDisplay();
						blockStructureChanged=true;
					}
				}
				else
				{
					QTextEdit::keyPressEvent(e);
				}
				
				if(blockStructureChanged)
				{
					emit inputChanged();
					emit returnPressed();
				}
				showVariableCompletions(false);
				break;
			}
		
		
		default:
		{
			QTextEdit::keyPressEvent(e);
			showVariableCompletions(false);
			break;
		}
	}
}

void ScicalcEdit::insertFromMimeData(const QMimeData *source)
{
	if(source!=0 && source->hasText())
	{
		QTextEdit::insertPlainText(source->text());
	}
	else
	{
		QTextEdit::insertFromMimeData(source);
	}
}

void ScicalcEdit::insertCompletion(QString completion)
{
	if(completion.isEmpty())
	{
		completion=completer->currentIndex().data(Qt::UserRole).toString();
	}

	if(completion.isEmpty())
	{
		return;
	}

	QTextCursor cursor=textCursor();
	QString prefix=completionPrefix();
	for(int i=0; i<prefix.length(); i++)
	{
		cursor.deletePreviousChar();
	}
	cursor.insertText(completion);
	setTextCursor(cursor);
	completer->popup()->hide();
}

void ScicalcEdit::showVariableCompletions(bool manual)
{
	if(!currentLineIsInput)
	{
		completer->popup()->hide();
		return;
	}

	QString prefix=completionPrefix();
	if(prefix.isEmpty() && !manual)
	{
		completer->popup()->hide();
		return;
	}

	updateCompletionModel();
	if(completionModel->rowCount()==0)
	{
		completer->popup()->hide();
		return;
	}

	completer->setCompletionPrefix(prefix);
	if(completer->completionCount()==0)
	{
		completer->popup()->hide();
		return;
	}

	QRect popupRect=cursorRect();
	int width=completer->popup()->sizeHintForColumn(0)
		+ completer->popup()->verticalScrollBar()->sizeHint().width();
	popupRect.setWidth(qMax(width, 220));
	completer->complete(popupRect);
}

void ScicalcEdit::updateCompletionModel()
{
	completionModel->clear();

	QList<Variable*> variables=Variables::all();
	for(int i=0; i<variables.size(); i++)
	{
		Variable *var=variables.at(i);
		if(var==0 || var->name.isEmpty())
		{
			continue;
		}

		QString value=Print::sciPrint(var->value) + var->unit;
		QString label=var->name + "    " + value;
		QStandardItem *item=new QStandardItem(label);
		item->setData(var->name, Qt::UserRole);
		completionModel->appendRow(item);
	}

	QStringList functions=Parser::functionNames();
	for(int i=0; i<functions.size(); i++)
	{
		QString name=functions.at(i);
		QString label=name + "()    Funktion";
		QStandardItem *item=new QStandardItem(label);
		item->setData(name + "(", Qt::UserRole);
		completionModel->appendRow(item);
	}

	completionModel->sort(0);
}

QString ScicalcEdit::completionPrefix() const
{
	if(!currentLineIsInput)
	{
		return QString();
	}

	QString line=textCursor().block().text();
	if(line.startsWith("\t"))
	{
		return QString();
	}

	int position=textCursor().positionInBlock();
	int start=position;
	while(start>0 && isIdentifierCharacter(line.at(start-1)))
	{
		start--;
	}

	QString prefix=line.mid(start, position-start);
	if(prefix.isEmpty())
	{
		return QString();
	}

	QChar first=prefix.at(0);
	if(!(first.isLetter() || first=='_' || first=='$'))
	{
		return QString();
	}

	return prefix;
}

bool ScicalcEdit::isIdentifierCharacter(QChar ch) const
{
	return ch.isLetterOrNumber() || ch=='_' || ch=='$';
}


void ScicalcEdit::cursorPositionChanged()
{
	//printBlocks();
	
	// find out current block
	int currentLine=textCursor().blockNumber()+1;
	
	//qDebug() << "current line:" << currentLine;
	
	int blockLine=0;
	int block=0;
	bool inputLine=false;
	
	for(int i=0; i<blocks.size(); i++)
	{
		block=i;
		
		// one line for the input
		blockLine++;
		
		if(blockLine==currentLine)
		{
			inputLine=true;
		}
		
		// one line for the output (if any)
		if(!blocks.at(i).output.isEmpty())
		{
			blockLine++;
		}
		
		if(blockLine>=currentLine)
		{
			break;
		}
	}
	
	//qDebug() << "inputLine" << inputLine;
	currentLineIsInput=inputLine;
	
	if(block==currentBlock && inputLine && currentBlock<blocks.size())
	{
		// we are still in the same block and it is an input line
		blocks[currentBlock].cursorPosition=textCursor().positionInBlock();
	}
	
	currentBlock=block;
	//qDebug() << "currentBlock" << currentBlock;
	
	/*
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::darkGray);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
	
	setExtraSelections(extraSelections);
	*/
}


void ScicalcEdit::clear()
{
	completer->popup()->hide();
	blocks.clear();
	currentBlock=0;
	currentLineIsInput=true;
	QTextCursor cursor=textCursor();
	cursor.select(QTextCursor::Document);
	if(cursor.hasSelection())
	{
		cursor.removeSelectedText();
	}
	setTextCursor(cursor);
	history.clear();
	future.clear();
}

void ScicalcEdit::readTextFromDisplay()
{
	//qDebug() << toPlainText();
	
	QStringList lines=toPlainText().split("\n");
	//qDebug() << "number of lines:" << lines.size();
	
	int block=0;
	
	for(int i=0; i<lines.size(); i++)
	{
		// ignore output lines
		if(!lines.at(i).startsWith("\t"))
		{
			// this is an input line
			if(block<blocks.size())
			{
				// this block already exists
				blocks[block].input=lines.at(i);
			}
			else
			{
				// this block is new
				blocks.append(ScicalcBlock(lines.at(i)));
			}
			block++;
		}
	}
	
	while(blocks.size()>block)
	{
		blocks.removeLast();
	}
	
	if(blocks.isEmpty())
	{
		blocks.append(ScicalcBlock(QString()));
	}
	
	if(currentBlock>=blocks.size())
	{
		currentBlock=blocks.size()-1;
	}
	
	emit inputChanged();
}


void ScicalcEdit::refreshDisplay()
{
	//qDebug() << "refresh";
	
	/*
	 * refresh the display based on internal data
	 */
	
	int scrollValue=verticalScrollBar()->value();
	
	blockSignals(true);
	QTextEdit::clear();
	QTextCursor cursor;
	
	for(int i=0; i<blocks.size(); i++)
	{
		// print input
		setTextColor(palette().color(QPalette::Text));
		cursor=textCursor();
		cursor.insertText(blocks.at(i).input);
		
		if(!(i==blocks.size()-1 && blocks.at(i).input.isEmpty()))
		{
			// don't append to last empty line
			cursor.insertText("\n");
		}
		setTextCursor(cursor);
		
		if(!blocks.at(i).output.isEmpty())
		{
			// print output
			setTextColor(palette().color(QPalette::BrightText));
			cursor=textCursor();
			cursor.insertText("\t\t\t\t"+blocks.at(i).output);
			setTextCursor(cursor);
			
			// switch back to input color for next input line
			setTextColor(palette().color(QPalette::Text));
			cursor=textCursor();
			cursor.insertText("\n");
			setTextCursor(cursor);
		}
	}
	
	blockSignals(false);
	
	setCursorToBlock(currentBlock);
	verticalScrollBar()->setValue(scrollValue);
	ensureCursorVisible();
	
	// update history
	history.append(blocks);
	future.clear();
}


int ScicalcEdit::getBlockCount()
{
	return blocks.size();
}


ScicalcBlock ScicalcEdit::getBlock(int index)
{
	return blocks.at(index);
}

void ScicalcEdit::setBlock(int index, ScicalcBlock block)
{
	if(index>=0 && index<blocks.size())
	{
		blocks.replace(index, block);
	}
}


// move cursor to block
void ScicalcEdit::setCursorToBlock(int blockIndex)
{
	QTextCursor cursor=textCursor();
	int cursorLine=0;
	
	for(int i=0; i<blocks.size(); i++)
	{
		if(i==blockIndex)
		{
			break;
		}
		
		cursorLine++;
		if(!blocks.at(i).output.isEmpty())
		{
			cursorLine++;
		}
	}
	
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, cursorLine);
	if(blockIndex<blocks.size())
	{
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, blocks.at(blockIndex).cursorPosition);
	}
	setTextCursor(cursor);
}

void ScicalcEdit::undo()
{
	/*
	if(!history.isEmpty())
	{
		blocks=history.takeLast();
		future.append(blocks);
	}
	*/
}

void ScicalcEdit::redo()
{
	/*
	if(!future.isEmpty())
	{
		blocks=future.takeLast();
		history.append(blocks);
	}
	*/
}

void ScicalcEdit::printBlocks()
{
	// just for debugging
	qDebug() << "number of blocks:" << blocks.size();
	for(int i=0; i<blocks.size(); i++)
	{
		qDebug() << blocks.at(i).input;
	}
}
