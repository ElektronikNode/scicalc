#include "scicalc.h"
#include "ui_scicalc.h"
#include "variables.h"
#include "scanner.h"
#include "parser.h"
#include "scicalcblock.h"
#include "scicalc_version.h"

#include <QDebug>
#include <QtGui>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QTabBar>
#include <QToolButton>

#include <QSettings>


const QString scicalc::tempFile=QDir::homePath() + "/.temp.sc";
const QString scicalc::version=QString::fromLatin1(SCICALC_VERSION);

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
	Variables::init();
	
	ui->horizontalLayout->setStretch(0, 1);
	ui->horizontalLayout->setStretch(1, 0);
	ui->tabWidget->clear();
	ui->tabWidget->setTabsClosable(true);
	ui->tabWidget->setMovable(true);
	ui->tabWidget->tabBar()->installEventFilter(this);

	QToolButton *addTabButton=new QToolButton(ui->tabWidget);
	addTabButton->setText("+");
	addTabButton->setAutoRaise(true);
	addTabButton->setToolTip("New tab");
	ui->tabWidget->setCornerWidget(addTabButton, Qt::TopRightCorner);
	
	// connect refresh action
	connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(on_actionRefresh_triggered()));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabRequested(int)));
	connect(addTabButton, SIGNAL(clicked()), this, SLOT(on_actionNew_triggered()));
	connect(dialogGeneralSettings, SIGNAL(accepted()), this, SLOT(settingsChanged()));
	
	// load settings-checkbox states:
	ui->actionLoad_recent_file_on_startup->setChecked(set.value("autoload", false).toBool());
	ui->actionAuto_save_on_close->setChecked(set.value("saveonclose", false).toBool());
	
	// load the path of the last saved/loaded file:
	currentPath=set.value("recentdir", QDir::currentPath()).toString();
	
	// check if there is a filename given as parameter, open this file
	if(QApplication::arguments().size()==2 && !QCoreApplication::instance()->property("scicalcCliMode").toBool())
	{
		ScicalcEdit *edit=createTab();
		loadFileIntoEditor(edit, QApplication::arguments().at(1));
	}
	else if(!restoreSession())
	{
		createTab();
	}

	settingsChanged();	// set colors, font, number format, etc.
	updateWindowTitle();
}


scicalc* scicalc::app()
{
	return myApp;
}

bool scicalc::eventFilter(QObject *watched, QEvent *event)
{
	if(watched==ui->tabWidget->tabBar())
	{
		if(event->type()==QEvent::MouseButtonRelease)
		{
			QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(event);
			if(mouseEvent->button()==Qt::MiddleButton)
			{
				int index=ui->tabWidget->tabBar()->tabAt(mouseEvent->pos());
				if(index>=0)
				{
					closeTab(index);
					return true;
				}
			}
		}
		else if(event->type()==QEvent::MouseButtonDblClick)
		{
			QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(event);
			if(mouseEvent->button()==Qt::LeftButton && ui->tabWidget->tabBar()->tabAt(mouseEvent->pos())<0)
			{
				createTab();
				return true;
			}
		}
	}

	return QMainWindow::eventFilter(watched, event);
}

QString scicalc::getVersion()
{
	return version;
}




/************************************************************************************************/
/*							core functions for editing and refreshing							*/
/************************************************************************************************/

void scicalc::inputChanged()
{
	ScicalcEdit *edit=qobject_cast<ScicalcEdit*>(sender());
	if(edit==0)
	{
		edit=currentEdit();
	}
	if(edit!=0)
	{
		setSaved(edit, false);
	}
}


// slot for the refresh action. this can be triggered manually or by the return-key after the input of a new line

// for convinience the whole file will be updated.
void scicalc::on_actionRefresh_triggered()
{
	refreshEditor(currentEdit());
}

void scicalc::refreshEditor(ScicalcEdit *edit)
{
	if(edit==0)
	{
		return;
	}

	temporaryDigitsActive=false;
	temporaryTrailingZerosActive=false;
	temporaryAccountingActive=false;
	temporaryDigits=dialogGeneralSettings->getDigits();
	temporaryTrailingZeros=dialogGeneralSettings->getTrailingZeros();
	temporaryAccounting=dialogGeneralSettings->getAccountingMode();
	
	Variables::init();
	bool accountingMode=getAccountingModeSetting();
	bool previousResultAvailable=false;

	for(int i=0; i<edit->getBlockCount(); i++)
	{
		ScicalcBlock block=edit->getBlock(i);
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
		edit->setBlock(i, block);

		QString trimmedInput=block.input.trimmed();
		bool isCommentLine=(trimmedInput.startsWith("//") || trimmedInput.startsWith("%"));
		bool hasExpression=!trimmedInput.isEmpty() && !isCommentLine;
		bool parseOk=!block.output.startsWith("ERROR");
		if(hasExpression)
		{
			previousResultAvailable=accountingMode && parseOk;
		}
	}
	
	edit->refreshDisplay();
}

void scicalc::settingsChanged()
{
	// load color settings:
	QPalette pal;
	pal.setColor(QPalette::Base, dialogGeneralSettings->getBackColor());
	pal.setColor(QPalette::Text, dialogGeneralSettings->getInColor());
	pal.setColor(QPalette::BrightText, dialogGeneralSettings->getOutColor());
	QFont f=dialogGeneralSettings->getFont();
	for(int i=0; i<ui->tabWidget->count(); i++)
	{
		ScicalcEdit *edit=qobject_cast<ScicalcEdit*>(ui->tabWidget->widget(i));
		if(edit!=0)
		{
			edit->setPalette(pal);
			edit->setFont(f);
			// set tabulator-width to 4 spaces (TODO):
			edit->setTabStopWidth(QFontMetrics(f).width("    "));
			refreshEditor(edit);
		}
	}
}



/************************************************************************************************/
/*						functions for file handling (open, save, new, ...)						*/
/************************************************************************************************/

// this is needed for getting a new empty editor in temporary-mode
void scicalc::on_actionNew_triggered()
{
	createTab();
}


// slot for action from menu and the shortcut CTRL+S
void scicalc::on_actionSave_triggered()
{
	ScicalcEdit *edit=currentEdit();
	if(edit==0)
	{
		return;
	}

	QString fileName=tabFileName(edit);
	if(fileName.isEmpty())
	{
		// choose a filename first
		on_actionSave_as_triggered();
	}
	else
	{
		save(fileName);
	}
}


// slot for action from menu and the shortcut CTRL+Shift+S
void scicalc::on_actionSave_as_triggered()
{
	ScicalcEdit *edit=currentEdit();
	if(edit==0)
	{
		return;
	}

	// only choose the filename
	QString defaultName=currentPath + "/" + tabDefaultFileName(edit);
	QString fileName=QFileDialog::getSaveFileName(this,
		 tr("save file"), defaultName, tr("scicalc scripts (*.sc)"));
	
	if(!fileName.isNull())	// the user choosed a file
	{
		QFileInfo fi(fileName);
		currentPath=fi.absolutePath();			// update working directory
		
		if(fi.suffix()!="sc")					// append extension if not existing
		{
			fileName.append(".sc");
		}
		
		saveEditorToFile(edit, fileName);
	}
}


// opens the file for writing, and saves the current edit_input
void scicalc::save(QString fileName)
{
	saveEditorToFile(currentEdit(), fileName);
}

bool scicalc::saveEditorToFile(ScicalcEdit *edit, QString fileName)
{
	if(edit==0)
	{
		return false;
	}

	// try to open file
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(0, tr("Error"), tr("File could not be opend:\n"));
		return false;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");

	out<<edit->toPlainText();

	setTabFileName(edit, fileName);
	setSaved(edit, true);

	file.close();
	return true;
}


void scicalc::on_actionOpen_triggered()
{
	// ask for file to be opened
	QString fileName=QFileDialog::getOpenFileName(this,
		 tr("load file"), currentPath, tr("scicalc scripts (*.sc)"));
	
	if(!fileName.isEmpty())
	{
		ScicalcEdit *edit=currentEdit();
		if(edit==0 || !isEditorEmpty(edit) || !tabFileName(edit).isEmpty())
		{
			edit=createTab();
		}
		loadFileIntoEditor(edit, fileName);
	}
}


// loads a file into the editor
// currentFileIsTemp-flag has to be set before calling this function.
void scicalc::load(QString fileName)
{
	ScicalcEdit *edit=currentEdit();
	if(edit==0)
	{
		edit=createTab();
	}
	loadFileIntoEditor(edit, fileName);
}

bool scicalc::loadFileIntoEditor(ScicalcEdit *edit, QString fileName)
{
	if(edit==0)
	{
		return false;
	}

	QFileInfo fi(fileName);

	// try to open file
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(0, tr("Error"), tr("File could not be opend:\n"));
		return false;
	}

	QTextStream in(&file);
	in.setCodec("UTF-8");

	QString text=in.readAll();

	edit->setText(text);
	
	refreshEditor(edit);
	setTabFileName(edit, fileName);
	currentPath=fi.absolutePath();
	setSaved(edit, true);

	file.close();
	return true;
}


void scicalc::closeEvent(QCloseEvent* event)
{
	// write all the settings to the disk, so they can be loaded on next startup.
	QSettings set;
	set.setValue("recentdir", currentPath);
	set.setValue("autoload", ui->actionLoad_recent_file_on_startup->isChecked());
	set.setValue("saveonclose", ui->actionAuto_save_on_close->isChecked());

	for(int i=0; i<ui->tabWidget->count(); i++)
	{
		ScicalcEdit *edit=qobject_cast<ScicalcEdit*>(ui->tabWidget->widget(i));
		if(edit!=0 && !tabCanClose(i))
		{
			event->ignore();
			return;
		}
	}

	saveSession();
	event->accept();
}


bool scicalc::askForSave(ScicalcEdit *edit)
{
	//qDebug() << "ask for save";
	if(edit==0 || isSaved(edit) || tabFileName(edit).isEmpty())
	{
		return true;
	}

	if(ui->actionAuto_save_on_close->isChecked())
	{
		return saveEditorToFile(edit, tabFileName(edit));
	}

	QMessageBox msgBox;
	msgBox.setText("Do you want to save changes?");
	msgBox.setInformativeText(tabFileName(edit));
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	msgBox.button(QMessageBox::Discard)->setText("Don't save");
	int ret=msgBox.exec();

	if(ret==QMessageBox::Save)
	{
		return saveEditorToFile(edit, tabFileName(edit));
	}
	if(ret==QMessageBox::Cancel)
	{
		return false;
	}
	return true;
}


void scicalc::setSaved(bool saved)
{
	setSaved(currentEdit(), saved);
}

bool scicalc::isSaved(ScicalcEdit *edit) const
{
	if(edit==0)
	{
		return true;
	}
	return edit->property("saved").toBool();
}

void scicalc::setSaved(ScicalcEdit *edit, bool saved)
{
	if(edit==0)
	{
		return;
	}

	edit->setProperty("saved", saved);
	updateTabTitle(edit);
	updateWindowTitle();
}

ScicalcEdit *scicalc::currentEdit() const
{
	return qobject_cast<ScicalcEdit*>(ui->tabWidget->currentWidget());
}

ScicalcEdit *scicalc::createTab(QString text, QString fileName, QDateTime createdAt, bool saved)
{
	if(!createdAt.isValid())
	{
		createdAt=QDateTime::currentDateTime();
	}

	ScicalcEdit *edit=new ScicalcEdit(ui->tabWidget);
	edit->setLineWrapMode(QTextEdit::NoWrap);
	edit->setProperty("fileName", fileName);
	edit->setProperty("createdAt", createdAt);
	edit->setProperty("saved", saved);
	connectEditor(edit);
	applyEditorSettings(edit);

	int index=ui->tabWidget->addTab(edit, QString());
	ui->tabWidget->setCurrentIndex(index);
	edit->setText(text.isNull() ? QString() : text);
	setSaved(edit, saved);
	updateTabTitle(edit);
	return edit;
}

void scicalc::connectEditor(ScicalcEdit *edit)
{
	connect(edit, SIGNAL(inputChanged()), this, SLOT(inputChanged()));
	connect(edit, SIGNAL(returnPressed()), this, SLOT(on_actionRefresh_triggered()));
}

void scicalc::applyEditorSettings(ScicalcEdit *edit)
{
	if(edit==0)
	{
		return;
	}

	QPalette pal;
	pal.setColor(QPalette::Base, dialogGeneralSettings->getBackColor());
	pal.setColor(QPalette::Text, dialogGeneralSettings->getInColor());
	pal.setColor(QPalette::BrightText, dialogGeneralSettings->getOutColor());
	edit->setPalette(pal);

	QFont f=dialogGeneralSettings->getFont();
	edit->setFont(f);
	edit->setTabStopWidth(QFontMetrics(f).width("    "));
}

void scicalc::tabChanged(int /*index*/)
{
	updateWindowTitle();
}

void scicalc::closeTabRequested(int index)
{
	closeTab(index);
}

void scicalc::closeTab(int index)
{
	if(index<0 || index>=ui->tabWidget->count())
	{
		return;
	}
	if(!tabCanClose(index))
	{
		return;
	}

	QWidget *widget=ui->tabWidget->widget(index);
	ui->tabWidget->removeTab(index);
	delete widget;

	if(ui->tabWidget->count()==0)
	{
		createTab();
	}
	updateWindowTitle();
}

bool scicalc::tabCanClose(int index)
{
	ScicalcEdit *edit=qobject_cast<ScicalcEdit*>(ui->tabWidget->widget(index));
	return askForSave(edit);
}

bool scicalc::isEditorEmpty(ScicalcEdit *edit) const
{
	if(edit==0)
	{
		return true;
	}
	return edit->toPlainText().trimmed().isEmpty();
}

QString scicalc::tabFileName(ScicalcEdit *edit) const
{
	if(edit==0)
	{
		return QString();
	}
	return edit->property("fileName").toString();
}

void scicalc::setTabFileName(ScicalcEdit *edit, QString fileName)
{
	if(edit==0)
	{
		return;
	}
	edit->setProperty("fileName", fileName);
	updateTabTitle(edit);
	updateWindowTitle();
}

QDateTime scicalc::tabCreatedAt(ScicalcEdit *edit) const
{
	if(edit==0)
	{
		return QDateTime();
	}
	QDateTime createdAt=edit->property("createdAt").toDateTime();
	if(!createdAt.isValid())
	{
		createdAt=QDateTime::currentDateTime();
	}
	return createdAt;
}

QString scicalc::tabDefaultFileName(ScicalcEdit *edit) const
{
	QDateTime createdAt=tabCreatedAt(edit);
	return "scicalc-" + createdAt.toString("yyyy-MM-dd_HH-mm-ss") + ".sc";
}

QString scicalc::tabTitle(ScicalcEdit *edit) const
{
	QString fileName=tabFileName(edit);
	QString title;
	if(fileName.isEmpty())
	{
		title=tabCreatedAt(edit).toString("yyyy-MM-dd HH:mm");
	}
	else
	{
		title=QFileInfo(fileName).completeBaseName();
	}

	if(!isSaved(edit))
	{
		title+="*";
	}
	return title;
}

void scicalc::updateTabTitle(ScicalcEdit *edit)
{
	int index=ui->tabWidget->indexOf(edit);
	if(index>=0)
	{
		ui->tabWidget->setTabText(index, tabTitle(edit));
		ui->tabWidget->setTabToolTip(index, tabFileName(edit).isEmpty() ? tabDefaultFileName(edit) : tabFileName(edit));
	}
}

void scicalc::updateWindowTitle()
{
	ScicalcEdit *edit=currentEdit();
	QString windowtitle="scicalc " + version;
	if(edit!=0)
	{
		windowtitle.append(" - ");
		windowtitle.append(tabTitle(edit));
		windowtitle.append(" - created ");
		windowtitle.append(tabCreatedAt(edit).toString("yyyy-MM-dd HH:mm:ss"));
		QString fileName=tabFileName(edit);
		if(!fileName.isEmpty())
		{
			windowtitle.append(" - ");
			windowtitle.append(fileName);
		}
	}
	setWindowTitle(windowtitle);
}

void scicalc::saveSession()
{
	QSettings set;
	set.beginGroup("tabs");
	set.remove("");
	set.setValue("count", ui->tabWidget->count());
	set.setValue("current", ui->tabWidget->currentIndex());
	for(int i=0; i<ui->tabWidget->count(); i++)
	{
		ScicalcEdit *edit=qobject_cast<ScicalcEdit*>(ui->tabWidget->widget(i));
		set.beginGroup(QString::number(i));
		if(edit!=0)
		{
			set.setValue("text", edit->toPlainText());
			set.setValue("fileName", tabFileName(edit));
			set.setValue("createdAt", tabCreatedAt(edit).toString(Qt::ISODate));
			set.setValue("saved", isSaved(edit));
		}
		set.endGroup();
	}
	set.endGroup();
}

bool scicalc::restoreSession()
{
	QSettings set;
	set.beginGroup("tabs");
	int count=set.value("count", 0).toInt();
	if(count<=0)
	{
		set.endGroup();
		return false;
	}

	for(int i=0; i<count; i++)
	{
		set.beginGroup(QString::number(i));
		QString text=set.value("text").toString();
		QString fileName=set.value("fileName").toString();
		QDateTime createdAt=QDateTime::fromString(set.value("createdAt").toString(), Qt::ISODate);
		bool saved=set.value("saved", true).toBool();
		createTab(text, fileName, createdAt, saved);
		set.endGroup();
	}

	int current=set.value("current", 0).toInt();
	set.endGroup();
	if(ui->tabWidget->count()>0)
	{
		ui->tabWidget->setCurrentIndex(qBound(0, current, ui->tabWidget->count()-1));
	}
	return ui->tabWidget->count()>0;
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

void scicalc::resetTemporarySettings()
{
	temporaryDigitsActive=false;
	temporaryTrailingZerosActive=false;
	temporaryAccountingActive=false;
	temporaryDigits=dialogGeneralSettings->getDigits();
	temporaryTrailingZeros=dialogGeneralSettings->getTrailingZeros();
	temporaryAccounting=dialogGeneralSettings->getAccountingMode();
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
    QMessageBox::about(this, "About scicalc", "by Friedrich Feichtinger\nKarl Zeilhofer\nVersion: "+version+"\nGPL v2");
}
