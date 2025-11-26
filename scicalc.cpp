#include "scicalc.h"
#include "ui_scicalc.h"
#include "variables.h"
#include "scanner.h"
#include "parser.h"
#include "scicalcblock.h"

#include <QDebug>
#include <QtGui>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

#include <QSettings>


const QString scicalc::tempFile=QDir::homePath() + "/.temp.sc";
const QString scicalc::version="1.0.0";

scicalc* scicalc::myApp=0;

/************************************************************************************************/
/*											constructor											*/
/************************************************************************************************/
scicalc::scicalc(QMainWindow *parent) :
	QMainWindow(parent),
	ui(new Ui::scicalc)
{
	myApp=this;
	ui->setupUi(this);
	
	QSettings set;

    setWindowIcon(QIcon(":/logo.svg"));
	dialogGeneralSettings=new DialogGeneralSettings(this);
	
	temporaryDigitsActive=false;
	temporaryTrailingZerosActive=false;
	temporaryAccountingActive=false;
	temporaryDigits=dialogGeneralSettings->getDigits();
	temporaryTrailingZeros=dialogGeneralSettings->getTrailingZeros();
	temporaryAccounting=dialogGeneralSettings->getAccountingMode();
	
	ui->horizontalLayout->setStretch(0, 1);
	ui->horizontalLayout->setStretch(1, 0);
	ui->edit_input->setLineWrapMode(QTextEdit::NoWrap);
	
	// connect refresh action
	connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(on_actionRefresh_triggered()));
	connect(ui->edit_input, SIGNAL(inputChanged()), this, SLOT(inputChanged()));
	connect(ui->edit_input, SIGNAL(returnPressed()), this, SLOT(on_actionRefresh_triggered()));
	connect(dialogGeneralSettings, SIGNAL(accepted()), this, SLOT(settingsChanged()));

	settingsChanged();	// set colors, font, number format, etc.
	
	// load settings-checkbox states:
	ui->actionLoad_recent_file_on_startup->setChecked(set.value("autoload", false).toBool());
	ui->actionAuto_save_on_close->setChecked(set.value("saveonclose", false).toBool());
	
	// load the path of the last saved/loaded file:
	currentPath=set.value("recentdir", QDir::currentPath()).toString();
	currentFile=set.value("recentfile", QString("")).toString();
	
	// check if there is a filename given as parameter, open this file
	if(QApplication::arguments().size()==2)
	{
		currentFile=QApplication::arguments().at(1);
		load(currentFile);
	}
	else
	{
		// if auto-load is active try to load last file
		if(ui->actionLoad_recent_file_on_startup->isChecked())
		{
			// if last session was in temporary-mode, load the temp-file (=history):
			if(currentFile.isEmpty())
			{
				load(tempFile);
			}
			else // else load recent opened file:
			{
				load(currentFile);
			}
		}
		else // else: default empty sheet
		{
			on_actionNew_triggered();
		}
	}
}


scicalc* scicalc::app()
{
	return myApp;
}




/************************************************************************************************/
/*							core functions for editing and refreshing							*/
/************************************************************************************************/

void scicalc::inputChanged()
{
	setSaved(false);
}


// slot for the refresh action. this can be triggered manually or by the return-key after the input of a new line

// for convinience the whole file will be updated.
void scicalc::on_actionRefresh_triggered()
{
	temporaryDigitsActive=false;
	temporaryTrailingZerosActive=false;
	temporaryAccountingActive=false;
	temporaryDigits=dialogGeneralSettings->getDigits();
	temporaryTrailingZeros=dialogGeneralSettings->getTrailingZeros();
	temporaryAccounting=dialogGeneralSettings->getAccountingMode();
	
	Variables::init();
	bool accountingMode=getAccountingModeSetting();
	bool previousResultAvailable=false;

	for(int i=0; i<ui->edit_input->getBlockCount(); i++)
	{
		ScicalcBlock block=ui->edit_input->getBlock(i);
		QString expression=block.input;

		if(accountingMode && previousResultAvailable)
		{
			int firstCharIndex=0;
			while(firstCharIndex<expression.size() && expression.at(firstCharIndex).isSpace())
			{
				firstCharIndex++;
			}

			if(firstCharIndex<expression.size())
			{
				QChar firstChar=expression.at(firstCharIndex);
				bool isDivisionComment=(firstChar=='/' && firstCharIndex+1<expression.size() && expression.at(firstCharIndex+1)=='/');
				if(!isDivisionComment &&
					(firstChar=='+' || firstChar=='-' || firstChar=='*' || firstChar=='/' || firstChar=='^'))
				{
					expression.insert(firstCharIndex, QChar('$'));
				}
			}
		}

		Scanner::init(expression);
		block.output=Parser::parse();
		ui->edit_input->setBlock(i, block);

		QString trimmedInput=block.input.trimmed();
		bool isCommentLine=(trimmedInput.startsWith("//") || trimmedInput.startsWith("%"));
		bool hasExpression=!trimmedInput.isEmpty() && !isCommentLine;
		bool parseOk=!block.output.startsWith("ERROR");
		previousResultAvailable=accountingMode && hasExpression && parseOk;
	}
	
	ui->edit_input->refreshDisplay();
}

void scicalc::settingsChanged()
{
	// load color settings:
	QPalette pal;
	pal.setColor(QPalette::Base, dialogGeneralSettings->getBackColor());
	pal.setColor(QPalette::Text, dialogGeneralSettings->getInColor());
	pal.setColor(QPalette::BrightText, dialogGeneralSettings->getOutColor());
	ui->edit_input->setPalette(pal);
	
	QFont f=dialogGeneralSettings->getFont();
	ui->edit_input->setFont(f);
	// set tabulator-width to 4 spaces (TODO):
	ui->edit_input->setTabStopWidth(QFontMetrics(f).width("    "));
	
	on_actionRefresh_triggered();
}



/************************************************************************************************/
/*						functions for file handling (open, save, new, ...)						*/
/************************************************************************************************/

// this is needed for getting a new empty editor in temporary-mode
void scicalc::on_actionNew_triggered()
{
	askForSave();
	currentFile.clear();
	ui->edit_input->clear();
	
	setSaved(true);		// if there was no editing to an empty input it does not have to be saved
}


// slot for action from menu and the shortcut CTRL+S
void scicalc::on_actionSave_triggered()
{
	if(currentFile.isEmpty())
	{
		// choose a filename first
		on_actionSave_as_triggered();
	}
	else
	{
		save(currentFile);
	}
}


// slot for action from menu and the shortcut CTRL+Shift+S
void scicalc::on_actionSave_as_triggered()
{
	// only choose the filename
	QString fileName=QFileDialog::getSaveFileName(this,
		 tr("save file"), currentPath, tr("scicalc scripts (*.sc)"));
	
	if(!fileName.isNull())	// the user choosed a file
	{
		QFileInfo fi(fileName);
		currentPath=fi.absolutePath();			// update working directory
		
		if(fi.suffix()!="sc")					// append extension if not existing
		{
			fileName.append(".sc");
		}
		
		currentFile=fileName;
		
		// now do the real saving
		on_actionSave_triggered();
	}
}


// opens the file for writing, and saves the current edit_input
void scicalc::save(QString fileName)
{
	// try to open file
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(0, tr("Error"), tr("File could not be opend:\n"));
		return;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");

	out<<ui->edit_input->toPlainText();

	setSaved(true);

	file.close();
}


void scicalc::on_actionOpen_triggered()
{

	// ask for saving before opening a file
	askForSave();
	
	// ask for file to be opened
	QString fileName=QFileDialog::getOpenFileName(this,
		 tr("load file"), currentPath, tr("scicalc scripts (*.sc)"));

	if(!fileName.isEmpty())
	{
		// if the user selected a valid file, load it
		load(fileName);
	}
}


// loads a file into the editor
// currentFileIsTemp-flag has to be set before calling this function.
void scicalc::load(QString fileName)
{
	QFileInfo fi(fileName);

	// try to open file
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(0, tr("Error"), tr("File could not be opend:\n"));
		return;
	}

	QTextStream in(&file);
	in.setCodec("UTF-8");

	QString text=in.readAll();

	ui->edit_input->setText(text);
	
	on_actionRefresh_triggered();
	
	if(fileName==tempFile)
	{
		// this file is an autoloaded temp file
		currentFile.clear();
		setSaved(false);
	}
	else
	{
		currentFile=fileName;
		currentPath=fi.absolutePath();
		setSaved(true);
	}

	file.close();
}


void scicalc::closeEvent(QCloseEvent* /*event*/)
{
	// write all the settings to the disk, so they can be loaded on next startup.
	QSettings set;
	set.setValue("recentdir", currentPath);
	set.setValue("recentfile", currentFile);
	set.setValue("autoload", ui->actionLoad_recent_file_on_startup->isChecked());
	set.setValue("saveonclose", ui->actionAuto_save_on_close->isChecked());
	
	if(currentFile.isEmpty())
	{
		// save temp file to its (constant) location
		save(tempFile);
	}
	else
	{
		// save existing file
		if(ui->actionAuto_save_on_close->isChecked())
		{
			on_actionSave_triggered();
		}
		else
		{
			askForSave();
		}
	}
}


void scicalc::askForSave()
{
	//qDebug() << "ask for save";
	// if the content is not related to a file yet, don't ask for saving
	if(!saved && !currentFile.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Do you want to save changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.button(QMessageBox::Discard)->setText("Don't save");
		int ret=msgBox.exec();
		
		if(ret==QMessageBox::Save)
		{
			on_actionSave_triggered();
		}
	}
}


void scicalc::setSaved(bool saved)
{
	this->saved=saved;
	
    QString windowtitle="scicalc " + version;
	
	if(!currentFile.isEmpty())
	{
		windowtitle.append(" - ");
		windowtitle.append(currentFile);
	}
	
	if(!saved)
	{
		windowtitle.append(" (unsaved)");
	}
	
	setWindowTitle(windowtitle);
}


int scicalc::getDigitsSetting() const
{
	if(temporaryDigitsActive)
	{
		return temporaryDigits;
	}
	return dialogGeneralSettings->getDigits();
}

bool scicalc::getTrailingZerosSetting() const
{
	if(temporaryTrailingZerosActive)
	{
		return temporaryTrailingZeros;
	}
	return dialogGeneralSettings->getTrailingZeros();
}

bool scicalc::getAccountingModeSetting() const
{
	if(temporaryAccountingActive)
	{
		return temporaryAccounting;
	}
	return dialogGeneralSettings->getAccountingMode();
}

bool scicalc::setTemporaryDigits(int digits)
{
	if(temporaryDigitsActive)
	{
		return false;
	}
	temporaryDigits=digits;
	temporaryDigitsActive=true;
	return true;
}

bool scicalc::setTemporaryTrailingZeros(bool enabled)
{
	if(temporaryTrailingZerosActive)
	{
		return false;
	}
	temporaryTrailingZeros=enabled;
	temporaryTrailingZerosActive=true;
	return true;
}

bool scicalc::setTemporaryAccounting(bool enabled)
{
	if(temporaryAccountingActive)
	{
		return false;
	}
	temporaryAccounting=enabled;
	temporaryAccountingActive=true;
	return true;
}


scicalc::~scicalc()
{
	delete ui;
}



/************************************************************************************************/
/*							actions for help menu and settings									*/
/************************************************************************************************/

void scicalc::on_actionReadme_triggered()
{
    showTextFile(":/doc/README.md");
}

void scicalc::on_actionConstants_triggered()
{
    showTextFile(":/doc/constants.txt");
}

void scicalc::on_actionFunctions_triggered()
{
    showTextFile(":/doc/functions.txt");
}

void scicalc::on_actionChangelog_triggered()
{
    showTextFile(":/doc/CHANGELOG.md");
}


void scicalc::showTextFile(QString filename)
{
	
	//qDebug() << "show text file:" << filename;
	QFile file(filename);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		return;
	}
	
	QTextEdit* text=new QTextEdit(this);
	text->setFont(QFont("Courier New"));
	text->setMinimumWidth(800);
	text->setMinimumHeight(500);
	text->setWindowFlags(Qt::Window);
	text->setReadOnly(true);
	text->setTabStopWidth(text->fontMetrics().width("    "));
	
	QTextStream in(&file);
	in.setCodec("UTF-8");
	text->setText(in.readAll());
	text->show();
	
	file.close();
}


void scicalc::on_actionGeneral_settings_triggered()
{
	if(dialogGeneralSettings->exec()==1)
	{
		on_actionRefresh_triggered();
	}
}

void scicalc::on_actionAbout_scicalc_triggered()
{
    QMessageBox::about(this, "About scicalc", "by Friedrich Feichtinger\nVersion: "+version+"\nGPL v2");
}
