#include "scicalcedit.h"
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>

ScicalcEdit::ScicalcEdit(QWidget *parent) :
	QTextEdit(parent)
{
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
	connect(this, SIGNAL(textChanged()), this, SLOT(readTextFromDisplay()));
}


void ScicalcEdit::keyPressEvent(QKeyEvent *e)
{
	QTextCursor cursor=textCursor();
	
	switch(e->key())
	{
		case Qt::Key_Down:
		{
			if(currentBlock<blocks.size()-1)
			{
				currentBlock++;
				setCursorToBlock(currentBlock);
			}
			break;
		}
			
		case Qt::Key_Up:
		{
			if(currentBlock>0)
			{
				currentBlock--;
				setCursorToBlock(currentBlock);
			}
			break;
		}
			
		case Qt::Key_Right:
		{
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
				}
			}
			else
			{
				QTextEdit::keyPressEvent(e);
			}
			break;
		}
		
		case Qt::Key_Delete:
		{
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
				}
			}
			else
			{
				QTextEdit::keyPressEvent(e);
			}
			break;
		}
		
		
		default:
		{
			QTextEdit::keyPressEvent(e);
			break;
		}
	}
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
	blocks.clear();
	currentBlock=0;
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
